
#include "poly.h"

struct _poly_scanline_t _scanlines[200];

void hline_c(int x1, int x2, int y, uint8_t c, uint8_t *buffer) {
  int l = x2 - x1;
  uint8_t *temp = buffer + (x1 + (y << 6) + (y << 8));

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

  if (l == 0) { return; }

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
  int u, v, tofs;
  uint8_t *temp;
  fixed du, cu, dv, cv;

  if (l == 0) { return; }

  u = 0; 
  v = 0; 
  tofs = 0;
  
  temp = buffer + (x1 + (y << 6) + (y << 8));
  
  du = fixed_div(int_to_fixed(u2-u1), int_to_fixed(l));
  cu = int_to_fixed(u1);
  dv = fixed_div(int_to_fixed(v2-v1), int_to_fixed(l));
  cv = int_to_fixed(v1);

  while (l > 0) {
    u = fixed_to_int(cu);
    v = fixed_to_int(cv);
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

    if (xi < _scanlines[t].x1) { _scanlines[t].x1 = xi; _scanlines[t].c1 = c; }
    if (xi > _scanlines[t].x2) { _scanlines[t].x2 = xi; _scanlines[t].c1 = c; }

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

    if (xi < _scanlines[t].x1) { _scanlines[t].x1 = xi; _scanlines[t].c1 = ci; }
    if (xi > _scanlines[t].x2) { _scanlines[t].x2 = xi; _scanlines[t].c2 = ci; }

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

    if (xi < _scanlines[t].x1) { _scanlines[t].x1 = xi; _scanlines[t].u1 = ui; _scanlines[t].v1 = vi; }
    if (xi > _scanlines[t].x2) { _scanlines[t].x2 = xi; _scanlines[t].u2 = ui; _scanlines[t].v2 = vi; }

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
                 uint8_t *texture, uint8_t *buffer) {
  int min_y = y1, max_y = y1, y = 0;

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
            texture, buffer);
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
                  uint8_t *texture, uint8_t *buffer) {
  int min_y = y1, max_y = y1, y = 0;

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
            texture, buffer);
  }

}
