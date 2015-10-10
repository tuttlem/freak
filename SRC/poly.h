
#ifndef __freak_poly_h_

#define __freak_poly_h_

#include <stdint.h>
#include <stdio.h>

#include "util.h"
#include "fixed.h"

struct _poly_scanline_t {
  int       x1, x2;   
  uint8_t   c1, c2;
  int       u1, v1, u2, v2;
};

void freak_poly_c(int x1, int y1, int x2, int y2,
                  int x3, int y3, int x4, int y4,
                  uint8_t c, uint8_t *buffer);

void freak_poly_g(int x1, int y1, int x2, int y2,
                  int x3, int y3, int x4, int y4,
                  uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4,
                  uint8_t *buffer);

void freak_poly_t(int x1, int y1, int x2, int y2,
                  int x3, int y3, int x4, int y4,
                  int u1, int v1, int u2, int v2,
                  int u3, int v3, int u4, int v4,
                  uint8_t *texture, uint8_t *buffer);

#endif /* __freak_poly_h_ */
