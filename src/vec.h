
#ifndef __freak_vec_h_

#define __freak_vec_h_

#include "fixed.h"

union _vec3 {
  fixed v[3];

  struct {
    fixed x, y, z;
  };
};

typedef union _vec3 vec3;

void vec3_zero(vec3 *v);
#pragma aux vec3_zero =  \
  "xor eax, eax"         \
  "mov ecx, 3"           \
  "rep stosd"            \
  parm [edi]             \
  modify [eax ecx];

void vec3_set(vec3* v, fixed x, fixed y, fixed z);
#pragma aux vec3_set =                       \
  "mov [edi], eax"                           \
  "add edi, 4"                               \
  "mov [edi], ebx"                           \
  "add edi, 4"                               \
  "mov [edi], ecx"                           \
  parm [edi] [eax] [ebx] [ecx];

inline void vec3_neg(vec3 *c) {
  c->x = fixed_mul(c->x, FIXED_NEGONE);
  c->y = fixed_mul(c->y, FIXED_NEGONE);
  c->z = fixed_mul(c->z, FIXED_NEGONE);
}

inline void vec3_add(vec3 *c, vec3 *a, vec3 *b) {
  c->x = a->x + b->x;
  c->y = a->y + b->y;
  c->z = a->z + b->z;
}

inline void vec3_inc(vec3 *c, vec3 *a) {
  c->x += a->x;
  c->y += a->y;
  c->z += a->z;
}

inline void vec3_sub(vec3 *c, vec3 *a, vec3 *b) {
  c->x = a->x - b->x;
  c->y = a->y - b->y;
  c->z = a->z - b->z;
}

inline void vec3_dec(vec3 *c, vec3 *a) {
  c->x -= a->x;
  c->y -= a->y;
  c->z -= a->z;
}

inline void vec3_mul(vec3 *c, vec3 *a, fixed f) {
  c->x = fixed_mul(a->x, f);
  c->y = fixed_mul(a->y, f);
  c->z = fixed_mul(a->z, f);
}

inline void vec3_div(vec3 *c, vec3 *a, fixed f) {
  c->x = fixed_div(a->x, f);
  c->y = fixed_div(a->y, f);
  c->z = fixed_div(a->z, f);
}

inline fixed vec3_len_sqr(vec3 *v) {
  return fixed_mul(v->x, v->x) +
    fixed_mul(v->y, v->y) +
    fixed_mul(v->z, v->z);
}

inline fixed vec3_len(vec3 *v) {
  return fixed_sqrt(vec3_len_sqr(v));
}

inline void vec3_normalize(vec3 *v) {
  fixed inv_len = fixed_div(FIXED_ONE, vec3_len(v));
  v->x = fixed_mul(v->x, inv_len);
  v->y = fixed_mul(v->y, inv_len);
  v->z = fixed_mul(v->z, inv_len);
}

#endif /* __freak_vec_h_ */
