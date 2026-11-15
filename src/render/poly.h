
#ifndef __freak_poly_h_

#define __freak_poly_h_

#include <stdint.h>
#include <stdio.h>

#include "../core/util.h"
#include "../core/fixed.h"

struct _poly_scanline_t {
  int       x1, x2;   
  uint8_t   c1, c2;
  int       u1, v1, u2, v2;
};

void freak_tri_c(int x1, int y1, int x2, int y2, int x3, int y3,
                 uint8_t c, uint8_t *buffer);

void freak_tri_g(int x1, int y1, int x2, int y2, int x3, int y3,
                 uint8_t c1, uint8_t c2, uint8_t c3,
                 uint8_t *buffer);

void freak_tri_t(int x1, int y1, int x2, int y2,
                 int x3, int y3, int u1, int v1, 
                 int u2, int v2, int u3, int v3,
                 uint8_t *texture, uint8_t *buffer);

/* Perspective-correct textured triangle.  z values are camera-space 16.16
   depths; u/v address a 256x256 texture. */
void freak_tri_pt(int x1, int y1, fixed z1, int u1, int v1,
                  int x2, int y2, fixed z2, int u2, int v2,
                  int x3, int y3, fixed z3, int u3, int v3,
                  uint8_t *texture, uint8_t *buffer);

void freak_quad_c(int x1, int y1, int x2, int y2,
                  int x3, int y3, int x4, int y4,
                  uint8_t c, uint8_t *buffer);

void freak_quad_g(int x1, int y1, int x2, int y2,
                  int x3, int y3, int x4, int y4,
                  uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4,
                  uint8_t *buffer);

void freak_quad_t(int x1, int y1, int x2, int y2,
                  int x3, int y3, int x4, int y4,
                  int u1, int v1, int u2, int v2,
                  int u3, int v3, int u4, int v4,
                  uint8_t *texture, uint8_t *buffer);

#endif /* __freak_poly_h_ */
