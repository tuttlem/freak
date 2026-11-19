# Freak demo HOWTOs

Each `.c` file is one runnable demo scene. The main demo executable only owns
the menu and calls these entry points.

| File | Demonstrates |
| --- | --- |
| `triang.c` | flat, Gouraud, and textured triangles |
| `cube.c` | cube transforms, painter sorting, and frame pacing |
| `sprites.c` | 2D camera, animation, and sprite batching |
| `tilemap.c` | scrolling layered tile map |
| `topdown.c` | collision, FOV, doors, paths, and pushing |
| `platfrm.c` | slopes, ladders, and moving platforms |
| `particl.c` | fire, smoke, sparks, and trails |
| `light2d.c` | 2D lighting and occlusion |
| `palette.c` | palette fades, gradients, and cycling |
| `blend.c` | lookup-table blend modes |
| `flare.c` | lens flare |
| `distort.c` | glass, wave, and shockwave distortion |
| `xform.c` | scaling, rotozoom, tunnel, and floor mapping |
| `textfx.c` | starfield and text effects |
| `voxel.c` | voxel height field |
| `pcspkr.c` | PC speaker sequence, tracker, and effects |
| `sbpcm.c` | Sound Blaster DMA PCM |
| `midimp.c` | MPU-401 UART MIDI |
| `textures.c` | procedural texture gallery |
| `utils.c` | sprites, debug drawing, and transitions |
| `menu.c` | UI controls, dialogs, and text input |

`democom.c` provides the common palette setup. `demodata.c` owns the small
shared runner/tile art. `democom.h` documents the launcher-provided state.
