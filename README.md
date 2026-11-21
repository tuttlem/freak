# Freak

A compact DOS game-programming library for Watcom/DOS4GW projects.

`src/freak.h` is the public umbrella header.  Internals are grouped by role:

- `src/core` — fixed-point math, vectors, matrices, and utilities
- `src/dos` — VGA, IRQ timer, keyboard, and PC-speaker hardware
- `src/render` — framebuffer drawing, polygon rasterisation, and 256x256
  paletted texture pages
- `src/game` — reusable game-facing helpers such as bodies and cameras

## Procedural texture pages

`render/texture` owns Freak's fixed 256x256, 8-bit texture pages. It provides
raw hash noise, smooth value noise, and fBm octave composition through
`core/noise`, then exposes page writers for raw or fractal noise, asphalt,
dirt, grass, clouds, marble, plasma, and fire. Material and effect writers
take a contiguous palette range, keeping the generated pixels independent of
the actual VGA palette.

`freak_texture_pixel(texture, u, v)` is the zero-overhead wrapped pixel macro
for hot loops. The page must be valid; it can be read or assigned to directly.

Run `make run` (or `scripts\\test.bat`, then `bin\\demo.exe`) to open the
interactive demo launcher. Its Textures menu contains the Mode 13h gallery. The top row is
raw noise, value noise, fBm, asphalt, and dirt; the bottom row is grass,
clouds, marble, animated plasma, and animated fire. Press any key to leave.

The launcher uses `W`/`S` or the arrow keys to select an item and Enter to
open it. Escape returns to the previous menu, or exits from the root menu.

## Asset pipeline

Freak games load compact native assets, while `tools/freak_asset.py` performs
host-side conversion. Run `make assets` to regenerate the included sample
pack, or use `python3 tools/freak_asset.py --help` for the converter commands.
`make demo` always regenerates those assets first. The launcher’s **Utilities
→ Asset loading** gallery loads the generated image, sprites, font, sound
bank, tracker/IMF music data, and F3M model directly from `test/assets/`.

Plasma takes four mutable byte phase counters (`c1` through `c4`) and advances
them at independent rates. `freak_texture_smudge_up` is the shared upward,
cooling palette-ramp operation used for fire today and smoke tomorrow.

## Deterministic randomness

`freak_rng` is a small deterministic generator with explicit seeds. Use
`freak_rng_seed_derived` to create an independent stream for a subsystem, or
`freak_rng_streams_seed` for the standard gameplay, particles, and visuals
streams. This keeps a particle effect from changing gameplay randomness and
makes a reported seed reproducible.

## Sound and music

`audio/mixer` is an eight-channel, fixed-point PCM mixer with per-channel
volume, pan, looping, mono/stereo output, and Sound-Blaster-ready unsigned
8-bit output.  `dos/sblaster` parses the conventional `BLASTER` setting,
detects the DSP, and starts an auto-init DMA stream from a caller-owned
conventional-memory ring buffer. The owning DOS-extender layer supplies that
buffer's physical address, installs its normal IRQ wrapper, and calls
`freak_sb_stream_irq` there to refill and acknowledge each block. This keeps
the hardware layer independent of any particular DOS extender.

`audio/asset` loads raw signed/unsigned PCM and PCM WAV (8/16-bit mono or
stereo), resamples, normalises, handles loop points, saves raw output, and
loads compact `FSBK` sound banks. `audio/tracker` plays built-in patterns or
the compact `FTR1` row-major tracker asset. `audio/seq` is the device-neutral
event sequencer for music or visual cues. `audio/opl` plays OPL2/IMF streams,
while `dos/midi` uses MPU-401 UART mode where a compatible interface exists.

The Utilities menu keeps the outputs deliberately separate: **PC Speaker:
music & effects** has the scale, tracker (`T`), and `1`–`8` effects;
**Sound Blaster: PCM scale** allocates a conventional DMA buffer and plays a
looping 8-bit PCM scale only through an available Sound Blaster; **MIDI:
MPU-401 scale** sends its scale only through an available MPU-401 interface.
