
#ifndef __freak_mat_h_

#define __freak_mat_h_

#include <stdio.h>

#include "fixed.h"
#include "vec.h"

union _mat44 {

  fixed m[16];

  struct {
    fixed e00, e01, e02, e03;
    fixed e10, e11, e12, e13;
    fixed e20, e21, e22, e23;
    fixed e30, e31, e32, e33;
  };
  
};

typedef union _mat44 mat44;

extern mat44 _mat44_identity;

void mat44_print(mat44 *m);

void mat44_zero(mat44 *m);
#pragma aux mat44_zero = \
  "xor eax, eax"         \
  "mov ecx, 0x10"        \
  "rep stosd"            \
  parm [edi]             \
  modify [eax ecx];

void mat44_identity(mat44 *m);
#pragma aux mat44_identity =   \
  "lea esi, _mat44_identity"   \
  "mov ecx, 0x10"              \
  "rep movsd"                  \
  parm [edi]                   \
  modify [esi ecx];

inline void mat44_set(mat44 *m,
                      fixed m00, fixed m01, fixed m02, fixed m03,
                      fixed m10, fixed m11, fixed m12, fixed m13,
                      fixed m20, fixed m21, fixed m22, fixed m23,
                      fixed m30, fixed m31, fixed m32, fixed m33) {
  m->e00 = m00; m->e01 = m01; m->e02 = m02; m->e03 = m03;
  m->e10 = m10; m->e11 = m11; m->e12 = m12; m->e13 = m13;
  m->e20 = m20; m->e21 = m21; m->e22 = m22; m->e23 = m23;
  m->e30 = m30; m->e31 = m31; m->e32 = m32; m->e33 = m33;
}

void mat44_perspective(mat44 *m, fixed fov, fixed aspect, fixed near, fixed far);

void mat44_axis_rot(mat44 *m, vec3 *axis, fixed angle);

void mat44_euler_rot(mat44 *m, fixed yaw, fixed pitch, fixed roll);

inline void mat44_translation(mat44 *m, vec3 *v) {
  mat44_set(m,
            FIXED_ONE, 0        , 0        , v->x,
            0        , FIXED_ONE, 0        , v->y,
            0        , 0        , FIXED_ONE, v->z,
            0        , 0        , 0        , FIXED_ONE);
}

inline void mat44_scale(mat44 *m, vec3 *v) {
  mat44_set(m,
            v->x, 0   , 0   , 0,
            0   , v->y, 0   , 0,
            0   , 0   , v->z, 0,
            0   , 0   , 0   , FIXED_ONE);
}

#define mrm(l,r,i,j) (fixed_mul(l->m[i], r->m[j]))

inline void mat44_mul(mat44 *m, mat44 *l, mat44 *r) {

  mat44_set(m,
            mrm(l,r,0,0) + mrm(l,r,4,1) + mrm(l,r,8,2) + mrm(l,r,12,3),
            mrm(l,r,1,0) + mrm(l,r,5,1) + mrm(l,r,9,2) + mrm(l,r,13,3),
            mrm(l,r,2,0) + mrm(l,r,6,1) + mrm(l,r,10,2) + mrm(l,r,14,3),
            mrm(l,r,3,0) + mrm(l,r,7,1) + mrm(l,r,11,2) + mrm(l,r,15,3),

            mrm(l,r,0,4) + mrm(l,r,4,5) + mrm(l,r,8,6) + mrm(l,r,12,7),
            mrm(l,r,1,4) + mrm(l,r,5,5) + mrm(l,r,9,6) + mrm(l,r,13,7),
            mrm(l,r,2,4) + mrm(l,r,6,5) + mrm(l,r,10,6) + mrm(l,r,14,7),
            mrm(l,r,3,4) + mrm(l,r,7,5) + mrm(l,r,11,6) + mrm(l,r,15,7),

            mrm(l,r,0,8) + mrm(l,r,4,9) + mrm(l,r,8,10) + mrm(l,r,12,11),
            mrm(l,r,1,8) + mrm(l,r,5,9) + mrm(l,r,9,10) + mrm(l,r,13,11),
            mrm(l,r,2,8) + mrm(l,r,6,9) + mrm(l,r,10,10) + mrm(l,r,14,11),
            mrm(l,r,3,8) + mrm(l,r,7,9) + mrm(l,r,11,10) + mrm(l,r,15,11),

            mrm(l,r,0,12) + mrm(l,r,4,13) + mrm(l,r,8,14) + mrm(l,r,12,15),
            mrm(l,r,1,12) + mrm(l,r,5,13) + mrm(l,r,9,14) + mrm(l,r,13,15),
            mrm(l,r,2,12) + mrm(l,r,6,13) + mrm(l,r,10,14) + mrm(l,r,14,15),
            mrm(l,r,3,12) + mrm(l,r,7,13) + mrm(l,r,11,14) + mrm(l,r,15,15));            
  
}

inline void mat44_mul_vec(vec3 *v, mat44 *l, vec3 *r) {

  v->x = fixed_mul(l->m[0], r->x) + fixed_mul(l->m[1], r->y) + fixed_mul(l->m[2],  r->z) + l->m[3];
  v->y = fixed_mul(l->m[4], r->x) + fixed_mul(l->m[5], r->y) + fixed_mul(l->m[6],  r->z) + l->m[7];
  v->z = fixed_mul(l->m[8], r->x) + fixed_mul(l->m[9], r->y) + fixed_mul(l->m[10], r->z) + l->m[11];
  
}

#endif /* __freak_mat_h_ */
