# Freak

A compact DOS game-programming library for Watcom/DOS4GW projects.

`src/freak.h` is the public umbrella header.  Internals are grouped by role:

- `src/core` — fixed-point math, vectors, matrices, and utilities
- `src/dos` — VGA, IRQ timer, keyboard, and PC-speaker hardware
- `src/render` — framebuffer drawing and polygon rasterisation
- `src/game` — reusable game-facing helpers such as bodies and cameras
