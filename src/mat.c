
#include "mat.h"

mat44 _mat44_identity = {
  FIXED_ONE , FIXED_ZERO, FIXED_ZERO, FIXED_ZERO,
  FIXED_ZERO, FIXED_ONE , FIXED_ZERO, FIXED_ZERO,
  FIXED_ZERO, FIXED_ZERO, FIXED_ONE , FIXED_ZERO,
  FIXED_ZERO, FIXED_ZERO, FIXED_ZERO, FIXED_ONE
};

void mat44_print(mat44 *m) {
  int o = 0, y = 0;
  double a, b, c, d;

  for (y = 0; y < 4; y ++) {
    a = fixed_to_double(m->m[0]);
    b = fixed_to_double(m->m[o + 1]);
    c = fixed_to_double(m->m[o + 2]);
    d = fixed_to_double(m->m[o + 3]);
    
    printf("[ %f, %f, %f, %f ]\n", a, b, c, d);

    o += 4;
  }

}
