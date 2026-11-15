
#include "poly.h"

struct _poly_scanline_t _scanlines[200];

struct _poly_perspective_scanline_t {
  int x1, x2;
  fixed q1, uq1, vq1;
  fixed q2, uq2, vq2;
};

static struct _poly_perspective_scanline_t _perspective_scanlines[200];

void hline_c(int x1, int x2, int y, uint8_t c, uint8_t *buffer) {
  int l = x2 - x1;
  uint8_t *temp;

  if (y < 0 || y >= 200 || x2 < 0 || x1 >= 320) { return; }
  if (x1 < 0) { x1 = 0; }
  if (x2 >= 320) { x2 = 319; }

  l = x2 - x1;
  temp = buffer + (x1 + (y << 6) + (y << 8));

  while (l >= 0) {
    *temp = c;
    temp ++;
    l --;
  }
}

void hline_g(int x1, int x2, int y, uint8_t c1, uint8_t c2, uint8_t *buffer) {
  int l = x2 - x1;
  uint8_t *temp;
  fixed dc, cur;

  if (y < 0 || y >= 200 || x2 < 0 || x1 >= 320 || l <= 0) { return; }
  if (x1 < 0) { x1 = 0; }
  if (x2 >= 320) { x2 = 319; }
  l = x2 - x1;
  if (l <= 0) { return; }

  temp = buffer + (x1 + (y << 6) + (y << 8));
  dc = fixed_div(int_to_fixed(c2-c1), int_to_fixed(l)); 
  cur = int_to_fixed(c1);
  
  while (l > 0) {
    *temp = fixed_to_int(cur);
    temp ++;
    cur += dc;
    l --;
  }
}

void hline_t(int x1, int x2, int y, int u1, int v1, int u2, int v2, uint8_t *texture, uint8_t *buffer) {
  int l = x2 - x1;
  int original_x1 = x1;
  int original_l = l;
  int u, v, tofs;
  uint8_t *temp;
  fixed du, cu, dv, cv;

  if (y < 0 || y >= 200 || x2 < 0 || x1 >= 320 || l <= 0) { return; }
  if (x1 < 0) { x1 = 0; }
  if (x2 >= 320) { x2 = 319; }
  l = x2 - x1;
  if (l <= 0) { return; }

  u = 0; 
  v = 0; 
  tofs = 0;
  
  temp = buffer + (x1 + (y << 6) + (y << 8));
  
  /* Interpolate over the unclipped span, then advance to the clipped edge.
     Rescaling over the shortened span made textures pull toward screen edges. */
  du = fixed_div(int_to_fixed(u2-u1), int_to_fixed(original_l));
  cu = int_to_fixed(u1);
  dv = fixed_div(int_to_fixed(v2-v1), int_to_fixed(original_l));
  cv = int_to_fixed(v1);
  if (x1 != original_x1) {
    cu += fixed_mul(du, int_to_fixed(x1 - original_x1));
    cv += fixed_mul(dv, int_to_fixed(x1 - original_x1));
  }

  while (l > 0) {
    u = fixed_to_int(cu);
    v = fixed_to_int(cv);
    /* Perspective-projected polygons may be screen-clipped while their
       texture endpoints remain outside the source image. */
    if (u < 0) { u = 0; }
    if (u > 255) { u = 255; }
    if (v < 0) { v = 0; }
    if (v > 255) { v = 255; }
    tofs = u + (v << 8);

    cu += du;
    cv += dv;

    *temp = texture[tofs];
    temp ++;
    l --;
  }
}

void scanline_c(int x1, int y1, int x2, int y2, uint8_t c) {
  int t = 0, xi = 0;
  fixed dx = 0, x = 0;

  if (y1 == y2) { return; }

  if (y1 > y2) {
    t = x1;
    x1 = x2;
    x2 = t;

    t = y1;
    y1 = y2;
    y2 = t;
  }

  dx = fixed_div(int_to_fixed(x2-x1), int_to_fixed(y2-y1));
  x = int_to_fixed(x1);

  for (t = y1; t <= y2; t ++) {
    xi = fixed_to_int(x);

    if (t >= 0 && t < 200) {
      if (xi < _scanlines[t].x1) { _scanlines[t].x1 = xi; _scanlines[t].c1 = c; }
      if (xi > _scanlines[t].x2) { _scanlines[t].x2 = xi; _scanlines[t].c1 = c; }
    }

    x += dx;
  }
}

void scanline_g(int x1, int y1, int x2, int y2, uint8_t c1, uint8_t c2) {
  int t = 0, xi = 0;
  uint8_t tb = 0, ci = 0;
  fixed dx = 0, x = 0,
        dc = 0, c = 0;

  if (y1 == y2) { return; }

  if (y1 > y2) {
    t = x1;
    x1 = x2;
    x2 = t;

    t = y1;
    y1 = y2;
    y2 = t;

    tb = c1;
    c1 = c2;
    c2 = tb;
  }

  dx = fixed_div(int_to_fixed(x2-x1), int_to_fixed(y2-y1));
  dc = fixed_div(int_to_fixed(c2-c1), int_to_fixed(y2-y1));

  x = int_to_fixed(x1);
  c = int_to_fixed(c1);

  for (t = y1; t <= y2; t ++) {
    xi = fixed_to_int(x);
    ci = fixed_to_int(c);

    if (t >= 0 && t < 200) {
      if (xi < _scanlines[t].x1) { _scanlines[t].x1 = xi; _scanlines[t].c1 = ci; }
      if (xi > _scanlines[t].x2) { _scanlines[t].x2 = xi; _scanlines[t].c2 = ci; }
    }

    x += dx;
    c += dc;
  }
}

void scanline_t(int x1, int y1, int x2, int y2, int u1, int v1, int u2, int v2) {
  int t = 0, xi = 0, ui = 0, vi = 0;
  fixed dx = 0, x = 0,
        du = 0, u = 0,
        dv = 0, v = 0;

  if (y1 == y2) { return; }

  if (y1 > y2) {
    t = x1;
    x1 = x2;
    x2 = t;

    t = y1;
    y1 = y2;
    y2 = t;

    t = u1;
    u1 = u2;
    u2 = t;

    t = v1;
    v1 = v2;
    v2 = t;
  }

  dx = fixed_div(int_to_fixed(x2-x1), int_to_fixed(y2-y1));
  du = fixed_div(int_to_fixed(u2-u1), int_to_fixed(y2-y1));
  dv = fixed_div(int_to_fixed(v2-v1), int_to_fixed(y2-y1));

  x = int_to_fixed(x1);
  u = int_to_fixed(u1);
  v = int_to_fixed(v1);

  for (t = y1; t <= y2; t ++) {
    xi = fixed_to_int(x);
    ui = fixed_to_int(u);
    vi = fixed_to_int(v);

    if (t >= 0 && t < 200) {
      if (xi < _scanlines[t].x1) { _scanlines[t].x1 = xi; _scanlines[t].u1 = ui; _scanlines[t].v1 = vi; }
      if (xi > _scanlines[t].x2) { _scanlines[t].x2 = xi; _scanlines[t].u2 = ui; _scanlines[t].v2 = vi; }
    }

    x += dx;
    u += du;
    v += dv;
  }
}

void reset_poly_state() {
  int y = 0;

  for (y = 0; y < 200; y ++) {
    _scanlines[y].x1 = 100000;
    _scanlines[y].x2 = -100000;
    _scanlines[y].u1 = 0;
    _scanlines[y].u2 = 0;
    _scanlines[y].v1 = 0;
    _scanlines[y].v2 = 0;
    _scanlines[y].c1 = 0;
    _scanlines[y].c2 = 0;
  }
}

void freak_tri_c(int x1, int y1, int x2, int y2, int x3, int y3,
                 uint8_t c, uint8_t *buffer) {
  int min_y = y1, max_y = y1, y = 0;

  reset_poly_state();

  scanline_c(x1, y1, x2, y2, c);
  scanline_c(x2, y2, x3, y3, c);
  scanline_c(x3, y3, x1, y1, c);

  if (y2 < min_y) { min_y = y2; }
  if (y3 < min_y) { min_y = y3; }

  if (y2 > max_y) { max_y = y2; }
  if (y3 > max_y) { max_y = y3; }

  if (min_y < 0) { min_y = 0; }
  if (max_y > 199) { max_y = 199; }

  for (y = min_y; y <= max_y; y ++) {
    hline_c(_scanlines[y].x1, _scanlines[y].x2, y, c, buffer);
  }
}

void freak_tri_g(int x1, int y1, int x2, int y2, int x3, int y3, 
                 uint8_t c1, uint8_t c2, uint8_t c3, 
                 uint8_t *buffer) {
  int min_y = y1, max_y = y1, y = 0;

  reset_poly_state();

  scanline_g(x1, y1, x2, y2, c1, c2);
  scanline_g(x2, y2, x3, y3, c2, c3);
  scanline_g(x3, y3, x1, y1, c3, c1);

  if (y2 < min_y) { min_y = y2; }
  if (y3 < min_y) { min_y = y3; }

  if (y2 > max_y) { max_y = y2; }
  if (y3 > max_y) { max_y = y3; }

  if (min_y < 0) { min_y = 0; }
  if (max_y > 199) { max_y = 199; }

  for (y = min_y; y <= max_y; y ++) {
    hline_g(_scanlines[y].x1, _scanlines[y].x2, y, 
            _scanlines[y].c1, _scanlines[y].c2, 
            buffer);
  }
}

void freak_tri_t(int x1, int y1, int x2, int y2,
                 int x3, int y3, 
                 int u1, int v1, int u2, int v2,
                 int u3, int v3, 
                 const struct freak_texture *texture, uint8_t *buffer) {
  int min_y = y1, max_y = y1, y = 0;

  if (texture == 0 || texture->pixels == 0) { return; }
  reset_poly_state();

  scanline_t(x1, y1, x2, y2, u1, v1, u2, v2);
  scanline_t(x2, y2, x3, y3, u2, v2, u3, v3);
  scanline_t(x3, y3, x1, y1, u3, v3, u1, v1);

  if (y2 < min_y) { min_y = y2; }
  if (y3 < min_y) { min_y = y3; }

  if (y2 > max_y) { max_y = y2; }
  if (y3 > max_y) { max_y = y3; }

  if (min_y < 0) { min_y = 0; }
  if (max_y > 199) { max_y = 199; }

  for (y = min_y; y <= max_y; y ++) {
    hline_t(_scanlines[y].x1, _scanlines[y].x2, y, 
            _scanlines[y].u1, _scanlines[y].v1, 
            _scanlines[y].u2, _scanlines[y].v2, 
            texture->pixels, buffer);
  }

}

static void reset_perspective_state(void) {
  int y;

  for (y = 0; y < 200; ++y) {
    _perspective_scanlines[y].x1 = 100000;
    _perspective_scanlines[y].x2 = -100000;
  }
}

static void scanline_pt(int x1, int y1, fixed q1, fixed uq1, fixed vq1,
                        int x2, int y2, fixed q2, fixed uq2, fixed vq2) {
  int t;
  int xi;
  int swap;
  fixed x;
  fixed dx;
  fixed q;
  fixed uq;
  fixed vq;
  fixed dq;
  fixed duq;
  fixed dvq;

  if (y1 == y2) { return; }
  if (y1 > y2) {
    swap = x1; x1 = x2; x2 = swap;
    swap = y1; y1 = y2; y2 = swap;
    q = q1; q1 = q2; q2 = q;
    uq = uq1; uq1 = uq2; uq2 = uq;
    vq = vq1; vq1 = vq2; vq2 = vq;
  }

  dx = fixed_div(int_to_fixed(x2 - x1), int_to_fixed(y2 - y1));
  dq = fixed_div(q2 - q1, int_to_fixed(y2 - y1));
  duq = fixed_div(uq2 - uq1, int_to_fixed(y2 - y1));
  dvq = fixed_div(vq2 - vq1, int_to_fixed(y2 - y1));
  x = int_to_fixed(x1);
  q = q1;
  uq = uq1;
  vq = vq1;

  for (t = y1; t <= y2; ++t) {
    xi = fixed_to_int(x);
    if (t >= 0 && t < 200) {
      if (xi < _perspective_scanlines[t].x1) {
        _perspective_scanlines[t].x1 = xi;
        _perspective_scanlines[t].q1 = q;
        _perspective_scanlines[t].uq1 = uq;
        _perspective_scanlines[t].vq1 = vq;
      }
      if (xi > _perspective_scanlines[t].x2) {
        _perspective_scanlines[t].x2 = xi;
        _perspective_scanlines[t].q2 = q;
        _perspective_scanlines[t].uq2 = uq;
        _perspective_scanlines[t].vq2 = vq;
      }
    }
    x += dx;
    q += dq;
    uq += duq;
    vq += dvq;
  }
}

static void hline_pt(int x1, int x2, int y,
                     fixed q1, fixed uq1, fixed vq1,
                     fixed q2, fixed uq2, fixed vq2,
                     uint8_t *texture, uint8_t *buffer) {
  int original_x1;
  int original_length;
  int length;
  int block;
  int pixel;
  int u_start;
  int v_start;
  int u_end;
  int v_end;
  int u;
  int v;
  fixed dq;
  fixed duq;
  fixed dvq;
  uint8_t *target;

  original_x1 = x1;
  original_length = x2 - x1;
  if (y < 0 || y >= 200 || x2 < 0 || x1 >= 320 ||
      original_length <= 0) { return; }
  if (x1 < 0) { x1 = 0; }
  if (x2 >= 320) { x2 = 319; }
  length = x2 - x1;
  if (length <= 0) { return; }

  dq = fixed_div(q2 - q1, int_to_fixed(original_length));
  duq = fixed_div(uq2 - uq1, int_to_fixed(original_length));
  dvq = fixed_div(vq2 - vq1, int_to_fixed(original_length));
  if (x1 != original_x1) {
    q1 += fixed_mul(dq, int_to_fixed(x1 - original_x1));
    uq1 += fixed_mul(duq, int_to_fixed(x1 - original_x1));
    vq1 += fixed_mul(dvq, int_to_fixed(x1 - original_x1));
  }
  target = buffer + x1 + (y << 6) + (y << 8);

  while (length > 0) {
    block = length > 4 ? 4 : length;
    if (q1 <= 0) { return; }
    u_start = fixed_to_int(fixed_div(uq1, q1));
    v_start = fixed_to_int(fixed_div(vq1, q1));
    q2 = q1 + dq * block;
    uq2 = uq1 + duq * block;
    vq2 = vq1 + dvq * block;
    if (q2 <= 0) { return; }
    u_end = fixed_to_int(fixed_div(uq2, q2));
    v_end = fixed_to_int(fixed_div(vq2, q2));
    for (pixel = 0; pixel < block; ++pixel) {
      u = u_start + ((u_end - u_start) * pixel) / block;
      v = v_start + ((v_end - v_start) * pixel) / block;
      u &= 255;
      v &= 255;
      *target++ = texture[u + (v << 8)];
    }
    q1 = q2;
    uq1 = uq2;
    vq1 = vq2;
    length -= block;
  }
}

void freak_tri_pt(int x1, int y1, fixed z1, int u1, int v1,
                  int x2, int y2, fixed z2, int u2, int v2,
                  int x3, int y3, fixed z3, int u3, int v3,
                  const struct freak_texture *texture, uint8_t *buffer) {
  fixed q1;
  fixed q2;
  fixed q3;
  fixed uq1;
  fixed uq2;
  fixed uq3;
  fixed vq1;
  fixed vq2;
  fixed vq3;
  int min_y;
  int max_y;
  int y;

  if (z1 <= 0 || z2 <= 0 || z3 <= 0 || texture == 0 ||
      texture->pixels == 0) { return; }
  q1 = fixed_div(FIXED_ONE, z1);
  q2 = fixed_div(FIXED_ONE, z2);
  q3 = fixed_div(FIXED_ONE, z3);
  uq1 = fixed_mul(int_to_fixed(u1), q1);
  uq2 = fixed_mul(int_to_fixed(u2), q2);
  uq3 = fixed_mul(int_to_fixed(u3), q3);
  vq1 = fixed_mul(int_to_fixed(v1), q1);
  vq2 = fixed_mul(int_to_fixed(v2), q2);
  vq3 = fixed_mul(int_to_fixed(v3), q3);

  reset_perspective_state();
  scanline_pt(x1, y1, q1, uq1, vq1, x2, y2, q2, uq2, vq2);
  scanline_pt(x2, y2, q2, uq2, vq2, x3, y3, q3, uq3, vq3);
  scanline_pt(x3, y3, q3, uq3, vq3, x1, y1, q1, uq1, vq1);

  min_y = y1;
  max_y = y1;
  if (y2 < min_y) { min_y = y2; }
  if (y3 < min_y) { min_y = y3; }
  if (y2 > max_y) { max_y = y2; }
  if (y3 > max_y) { max_y = y3; }
  if (min_y < 0) { min_y = 0; }
  if (max_y > 199) { max_y = 199; }
  for (y = min_y; y <= max_y; ++y) {
    hline_pt(_perspective_scanlines[y].x1, _perspective_scanlines[y].x2, y,
             _perspective_scanlines[y].q1, _perspective_scanlines[y].uq1,
             _perspective_scanlines[y].vq1, _perspective_scanlines[y].q2,
             _perspective_scanlines[y].uq2, _perspective_scanlines[y].vq2,
             texture->pixels, buffer);
  }
}

void freak_quad_c(int x1, int y1, int x2, int y2,
                  int x3, int y3, int x4, int y4,
                  uint8_t c, uint8_t *buffer) {
  int min_y = y1, max_y = y1, y = 0;

  reset_poly_state();

  scanline_c(x1, y1, x2, y2, c);
  scanline_c(x2, y2, x3, y3, c);
  scanline_c(x3, y3, x4, y4, c);
  scanline_c(x4, y4, x1, y1, c);

  if (y2 < min_y) { min_y = y2; }
  if (y3 < min_y) { min_y = y3; }
  if (y4 < min_y) { min_y = y4; }

  if (y2 > max_y) { max_y = y2; }
  if (y3 > max_y) { max_y = y3; }
  if (y4 > max_y) { max_y = y4; }

  if (min_y < 0) { min_y = 0; }
  if (max_y > 199) { max_y = 199; }

  for (y = min_y; y <= max_y; y ++) {
    hline_c(_scanlines[y].x1, _scanlines[y].x2, y, c, buffer);
  }
}

void freak_quad_g(int x1, int y1, int x2, int y2,
                  int x3, int y3, int x4, int y4,
                  uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4,
                  uint8_t *buffer) {
  int min_y = y1, max_y = y1, y = 0;

  reset_poly_state();

  scanline_g(x1, y1, x2, y2, c1, c2);
  scanline_g(x2, y2, x3, y3, c2, c3);
  scanline_g(x3, y3, x4, y4, c3, c4);
  scanline_g(x4, y4, x1, y1, c4, c1);

  if (y2 < min_y) { min_y = y2; }
  if (y3 < min_y) { min_y = y3; }
  if (y4 < min_y) { min_y = y4; }

  if (y2 > max_y) { max_y = y2; }
  if (y3 > max_y) { max_y = y3; }
  if (y4 > max_y) { max_y = y4; }

  if (min_y < 0) { min_y = 0; }
  if (max_y > 199) { max_y = 199; }

  for (y = min_y; y <= max_y; y ++) {
    hline_g(_scanlines[y].x1, _scanlines[y].x2, y, 
            _scanlines[y].c1, _scanlines[y].c2, 
            buffer);
  }
}

void freak_quad_t(int x1, int y1, int x2, int y2,
                  int x3, int y3, int x4, int y4,
                  int u1, int v1, int u2, int v2,
                  int u3, int v3, int u4, int v4,
                  const struct freak_texture *texture, uint8_t *buffer) {
  int min_y = y1, max_y = y1, y = 0;

  if (texture == 0 || texture->pixels == 0) { return; }
  reset_poly_state();

  scanline_t(x1, y1, x2, y2, u1, v1, u2, v2);
  scanline_t(x2, y2, x3, y3, u2, v2, u3, v3);
  scanline_t(x3, y3, x4, y4, u3, v3, u4, v4);
  scanline_t(x4, y4, x1, y1, u4, v4, u1, v1);

  if (y2 < min_y) { min_y = y2; }
  if (y3 < min_y) { min_y = y3; }
  if (y4 < min_y) { min_y = y4; }

  if (y2 > max_y) { max_y = y2; }
  if (y3 > max_y) { max_y = y3; }
  if (y4 > max_y) { max_y = y4; }

  if (min_y < 0) { min_y = 0; }
  if (max_y > 199) { max_y = 199; }

  for (y = min_y; y <= max_y; y ++) {
    hline_t(_scanlines[y].x1, _scanlines[y].x2, y, 
            _scanlines[y].u1, _scanlines[y].v1, 
            _scanlines[y].u2, _scanlines[y].v2, 
            texture->pixels, buffer);
  }

}
