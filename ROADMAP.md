# Freak roadmap

Freak is already a useful DOS4GW / Mode 13h game-programming base. The next
steps should favour small, visible demos that harden reusable library pieces.

## Foundations

- [x] **Input polish** — keyboard state, pressed/released edges, configurable
  action bindings, DOS mouse support, and a unified event queue.
- [ ] **Joystick input** — DOS gameport calibration, axes, buttons, and
  configurable controller bindings.
- [x] **Timing** — PIT-backed elapsed ticks, fixed-step accumulation, and
  interpolation.
- [x] **Frame pacing and FPS reporting** — configurable frame limits and
  measured FPS.
- [x] **Detailed profiling** — high-resolution frame, update, and render
  timings.
- [x] **Arena allocator** — aligned linear allocation, reset, and high-water
  tracking for fixed-lifetime memory.
- [ ] **Memory pools and debug allocation** — fixed object pools plus leak and
  allocation reporting.
- [x] **Randomness** — seeded deterministic RNG streams for gameplay,
  particles, visuals, and repeatable demos.
- [ ] **Debug tools** — on-screen console, assertions, logging levels,
  debug-draw primitives, and performance overlays.

## 2D games and sprites

- [x] **2D camera** — world-to-screen transforms and a camera-relative demo.
- [x] **Sprite animation** — timed frames, flips, positioned sprites, and an
  animated-sprite demo.
- [x] **Sprite batching** — fixed-capacity layered command list with stable
  draw ordering and camera-relative rendering.
- [x] **Tile maps** — scrolling layered tile maps with collision attributes.
- [x] **Map formats** — compact native FMP1 save/load format for tile layers
  and collision data.
- [x] **Platformer toolkit** — slopes, one-way platforms, moving platforms,
  ladders, and responsive character movement.
- [x] **Top-down toolkit** — grid movement, pathfinding, field-of-view/fog of
  war, triggers, doors, and pushable objects.
- [x] **Collision helpers** — AABB, circle, segment, swept collision,
  broad-phase spatial grid, and debug collision bounds.
- [x] **Particles** — pooled emitters, gravity, trails, sparks, smoke, fire,
  palette ramps, and additive/translucent modes.
- [x] **2D lighting** — radial light masks, flicker, coloured-light palette
  approximations, shadows, and day/night palette transitions.
- [x] **UI/HUD** — bitmap font layout, panels, buttons, lists, dialogue boxes,
  inventory grids, cursors, and save/load menus.

## 3D and software rendering

- [x] **Rasterizer polish** — near-plane clipping, back-face culling, depth
  buffer or painter sorting, screen clipping, and perspective correction.
- [x] **Materials** — flat, Gouraud, textured, affine/perspective texture
  mapping, transparency keys, and lightmapped surfaces.
- [x] **Lighting** — ambient/directional/point lights, attenuation, per-vertex
  Phong-style lighting, specular highlights, and normal interpolation.
- [x] **Bump mapping** — normal maps or palette-friendly approximations for
  embossed surfaces and moving lights.
- [x] **Environment mapping** — sphere mapping, cube-map-like lookup tables,
  chrome, water, and reflective-material demos.
- [x] **Glenz and translucency** — ordered translucent polygons, stipple/table
  blending, additive glow, and refraction-style distortions.
- [x] **Raycasting** — Wolfenstein-style grid renderer with textured walls,
  doors, sprites, floor/ceiling casting, and a playable maze demo.
- [x] **Raytracing** — a small offline or low-resolution realtime renderer for
  spheres/planes, shadows, reflections, and refractions.
- [x] **Model pipeline** — compact mesh format, normals/UVs/materials,
  keyframe animation, and host-side conversion.
- [x] **Camera toolkit** — FPS, orbit, chase, rail, shake, and smooth camera
  transitions.

## Demo-scene and visual effects

- [x] **Palette effects** — fades, cycling, gradients, colour cycling, and
  palette-range animation.
- [x] **Blending effects** — additive, subtractive, multiplicative, and
  translucent lookup-table blending.
- [x] **Plasma and fire** — expand the existing texture work with metaballs,
  interference patterns, heat haze, smoke, and water ripples.
- [x] **Lens flare** — configurable flare elements, screen-space occlusion,
  streaks, halos, and animated light sources.
- [x] **Glass lens/distortion** — magnifying glass, fisheye, wobbly water,
  shockwaves, refraction, and page-warp effects.
- [x] **Scalers and rotozoom** — stretch, rotate, zoom, tunnel, and floor
  effects using texture lookup tables.
- [x] **Voxel and height-field experiments** — voxel landscape, terrain
  renderer, marching cubes, or a small signed-distance-field demo.
- [x] **Text effects** — sine scrollers, vector/logo reveals, starfields,
  credits, and beat-synchronised transitions.

## Sound and music

- [x] **PCM effects** — Sound Blaster detection, 8-bit sample playback,
  DMA/IRQ buffering, volume/pan, looping, and a channel mixer.
- [x] **Music playback** — IMF/AdLib (OPL2) support first, then MIDI/MPU-401
  where hardware permits.
- [x] **Tracker music** — a compact MOD/S3M/XM-style player or converter for
  small DOS-friendly module files.
- [x] **Sequencer** — pattern-b ased event/music sequencer that can also drive
  palette, camera, and demo effects.
- [x] **Audio assets** — WAV-to-raw conversion, resampling, normalisation,
  loop points, and a small sound-bank format.
- [x] **Audio demo** — mixer visualiser plus a tiny music/sound-effects scene.

## Tooling and quality

- [x] **Asset pipeline** — host-side conversion from authoring formats into
  Freak-native palettes, images, sprite sheets, fonts, maps, WAV sound banks,
  and F3M 3D models; runtime loaders consume only those compact assets.
- [ ] **Image tools** — PCX/BMP loading, palette quantisation/dithering,
  sprite-sheet slicing, and transparent-colour handling.
- [ ] **Build ergonomics** — named demo targets, dependency generation,
  incremental builds, release/debug profiles, and a one-command clean build.
- [x] **Demo launcher** — a single DOS menu executable for selecting every
  gallery and test program.
- [ ] **Regression captures** — deterministic headless builds plus checksum or
  screenshot-based checks for rendering primitives.
- [ ] **API guide** — short focused examples for the public header and every
  major subsystem.
- [x] **Reference demos** — one focused, documented demo per major subsystem.

## Game-facing systems

- [ ] **Entity/component helpers** — lightweight IDs, component storage,
  message/events, and scene lifetime management.
- [ ] **State machines** — reusable state machines for player, enemies, UI,
  and game flow.
- [ ] **AI** — steering, waypoint/path following, finite-state enemies, and
  simple behaviour trees.
- [ ] **Scripting/data** — data-driven entities, dialogue, levels, and
  cutscenes; optionally a tiny embedded scripting language.
- [ ] **Save games** — versioned binary saves, checksums, and high-score
  tables.
- [ ] **Networking experiment** — serial/null-modem or IPX-era local multiplayer
  for a compact two-player example.

## Later experiments

- [ ] 3D textured scene with the existing polygon and fixed-point utilities.
- [ ] Procedural world/level generation and replay recording.
- [ ] A tiny complete game built entirely from Freak's public API.
