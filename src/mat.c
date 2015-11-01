
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
    a = fixed_to_double(m->m[o]);
    b = fixed_to_double(m->m[o + 1]);
    c = fixed_to_double(m->m[o + 2]);
    d = fixed_to_double(m->m[o + 3]);
    
    printf("[ %f, %f, %f, %f ]\n", a, b, c, d);

    o += 4;
  }

}

void mat44_perspective(mat44 *m, fixed fov, fixed aspect, fixed near_z, fixed far_z) {

  fixed _fov = fixed_div(fixed_mul(FIXED_PI, fov), int_to_fixed(180));
  fixed _f = fixed_div(FIXED_ONE, fixed_tan(fixed_mul(_fov, double_to_fixed(0.5))));

  fixed nf = fixed_div((far_z + near_z), (near_z - far_z));
  fixed nfr = fixed_div(fixed_mul(fixed_mul(int_to_fixed(2), far_z), near_z), (near_z - far_z));
  
  mat44_set(m,
            fixed_div(_f, aspect), 0 , 0           , 0  ,
            0                    , _f, 0           , 0  ,
            0                    , 0 , nf          , nfr,
            0                    , 0 , FIXED_NEGONE, 0);
  
}

void mat44_axis_rot(mat44 *m, vec3 *axis, fixed angle) {
  fixed ca = fixed_cos(angle),
    sa = fixed_sin(angle);

  fixed nca = FIXED_ONE - ca;

  fixed
    a00 = ca + fixed_mul3(axis->x, axis->x, nca),
    a01 = fixed_mul3(axis->x, axis->y, nca) - fixed_mul(axis->z, sa),
    a02 = fixed_mul3(axis->x, axis->z, nca) + fixed_mul(axis->y, sa),
    a10 = fixed_mul3(axis->y, axis->x, nca) + fixed_mul(axis->z, sa),
    a11 = ca + fixed_mul3(axis->y, axis->y, nca),
    a12 = fixed_mul3(axis->y, axis->z, nca) - fixed_mul(axis->x, sa),
    a20 = fixed_mul3(axis->z, axis->x, nca) - fixed_mul(axis->y, sa),
    a21 = fixed_mul3(axis->z, axis->y, nca) + fixed_mul(axis->x, sa),
    a22 = ca + fixed_mul3(axis->z, axis->z, nca);
                                                    
  mat44_set(m,
            a00, a01, a02, 0,
            a10, a11, a12, 0,
            a20, a21, a22, 0,
            0  , 0  , 0  , FIXED_ONE);
  
}

void mat44_euler_rot(mat44 *m, fixed yaw, fixed pitch, fixed roll) {
  fixed sb = fixed_sin(yaw), cb = fixed_cos(yaw),
    sa = fixed_sin(pitch), ca = fixed_cos(pitch),
    sc = fixed_sin(roll), cc = fixed_cos(roll);

  fixed
    a00 = fixed_mul(cc, cb),
    a01 = fixed_mul(-cb, sc),
    a02 = sb,
    a10 = fixed_mul3(sa, sb, cc) + fixed_mul(ca, sc),
    a11 = fixed_mul3(-sa, sb, sc) + fixed_mul(ca, cc),
    a12 = fixed_mul(-sa, cb),
    a20 = fixed_mul3(-ca, sb, cc) + fixed_mul(sa, sc),
    a21 = fixed_mul3(ca, sb, sc) + fixed_mul(sa, cc),
    a22 = fixed_mul(ca, cb);

  mat44_set(m,
            a00, a01, a02, 0,
            a10, a11, a12, 0,
            a20, a21, a22, 0,
            0  , 0  , 0  , FIXED_ONE);
}
