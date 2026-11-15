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

Run `make run` (or `test.bat`, then `textures.exe`) for a Mode 13h gallery. The top row is
raw noise, value noise, fBm, asphalt, and dirt; the bottom row is grass,
clouds, marble, animated plasma, and animated fire. Press any key to leave.

Plasma takes four mutable byte phase counters (`c1` through `c4`) and advances
them at independent rates. `freak_texture_smudge_up` is the shared upward,
cooling palette-ramp operation used for fire today and smoke tomorrow.
