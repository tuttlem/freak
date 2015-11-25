
#include "../src/freak.h"

vec3 verts[8], vs[8];
int polys[6*4] = {
  0, 1, 2, 3,
  4, 5, 6, 7,
  0, 1, 5, 4,
  3, 2, 6, 7,
  0, 3, 7, 4,
  1, 2, 6, 7
};

void init_cube() {
  fixed f2 = double_to_fixed(1.0), f1 = double_to_fixed(-1.0);
  
  vec3_set(&verts[0], f1, f1, f2);
  vec3_set(&verts[1], f2, f1, f2);
  vec3_set(&verts[2], f2, f2, f2);
  vec3_set(&verts[3], f1, f2, f2);

  vec3_set(&verts[4], f1, f1, f1);
  vec3_set(&verts[5], f2, f1, f1);
  vec3_set(&verts[6], f2, f2, f1);
  vec3_set(&verts[7], f1, f2, f1);

}

void vec3_to_2d(vec3 *v, int *x, int *y) {
  *x = fixed_to_int(fixed_div(fixed_mul(v->x, int_to_fixed(256)), v->z) + int_to_fixed(160));
  *y = fixed_to_int(fixed_div(fixed_mul(v->y, int_to_fixed(256)), v->Z) + int_to_fixed(100));
}

void draw_poly(int i) {
  vec3 a = vs[polys[i << 2]],
    b = vs[polys[(i << 2) + 1]],
    c = vs[polys[(i << 2) + 2]],
    d = vs[polys[(i << 2) + 3]];

  int x1, y1, x2, y2, x3, y3, x4, y4;

  vec3_to_2d(&a, &x1, &y1);
  vec3_to_2d(&b, &x2, &y2);
  vec3_to_2d(&c, &x3, &y3);
  vec3_to_2d(&d, &x4, &y4);
}
