# Freak asset tools

The DOS runtime deliberately loads only compact Freak-native assets.  It does
not parse editor and authoring formats such as PNG, OBJ, WAV, BDF, TMX, or
JSON.  `tools/` contains host-side importers that convert those files before a
DOS build is made.

## Runtime formats

| Asset | Freak format | Runtime owner | Authoring input |
| --- | --- | --- | --- |
| Indexed image / palette | `FIM1` | `render/image` | PNG, BMP, TGA, PCX |
| Sprite sheet | `FSP1` | `render/sprite` | PNG, BMP, TGA, PCX plus frame size |
| Bitmap font | `FFN1` | `render/font` | BDF or bitmap sheet |
| Tile map | `FMP1` | `2d/tilemap` | TMX/JSON or CSV |
| PCM sound bank | `FSBK` | `audio/asset` | WAV |
| 3D mesh | `F3M1` | `render/model3d` | triangulated OBJ |

All multibyte values are little-endian.  Pixels are 8-bit palette indices.
Palette RGB components are VGA DAC values (0–63), not 0–255.  Formats are
versioned by their four-byte magic, so a loader can reject incompatible data
cleanly.

`F3M1`, `FMP1`, and `FSBK` already exist in the runtime.  `obj2f3m.py` is the
first importer: it turns a triangulated OBJ (positions, normals, UVs) into an
F3M mesh.  The unified asset command will add the other importers while
keeping those established formats compatible.

The first map importer accepts a rectangular CSV grid:

```sh
python3 tools/freak_asset.py map art/level.csv assets/level.fmp --tile-width 16 --tile-height 16
```

MIDI can be normalised into a native `FTR1` tracker pattern (up to eight
channels, four tracker rows per MIDI quarter-note):

```sh
python3 tools/freak_asset.py music art/theme.mid assets/theme.ftr --kind midi
```

## Intended workflow

```text
art/player.png --sprite --frame 16x16--> build/assets/player.fsp
art/title.bdf -------------------------> build/assets/title.ffn
art/jump.wav --------------------------> build/assets/game.fsb
art/crate.obj --------------------------> build/assets/crate.f3m
```

Games then ship and load `*.fim`, `*.fsp`, `*.ffn`, `*.fmp`, `*.fsb`, and
`*.f3m`; source art stays on the host.  The converter owns quantisation,
dithering, transparent-index selection, frame slicing, sample-rate conversion,
and fixed-point model conversion, making DOS-side loading simple and
deterministic.

Run `make assets` to generate the deterministic demo-only sample pack in
`test/assets/`.
