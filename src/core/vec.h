
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
  modify [eax ecx edi];

void vec3_set(vec3* v, fixed x, fixed y, fixed z);
#pragma aux vec3_set =                       \
  "mov [edi], eax"                           \
  "add edi, 4"                               \
  "mov [edi], ebx"                           \
  "add edi, 4"                               \
  "mov [edi], ecx"                           \
  parm [edi] [eax] [ebx] [ecx]                \
  modify [edi];

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

/* Core geometric primitives.  Keep these here rather than duplicating small
   renderer-local helpers so gameplay, collision, and render code agree on
   the same fixed-point operations. */
inline fixed vec3_dot(const vec3 *a, const vec3 *b) {
  return fixed_mul(a->x, b->x) +
    fixed_mul(a->y, b->y) +
    fixed_mul(a->z, b->z);
}

inline void vec3_cross(vec3 *out, const vec3 *a, const vec3 *b) {
  vec3 result;
  result.x = fixed_mul(a->y, b->z) - fixed_mul(a->z, b->y);
  result.y = fixed_mul(a->z, b->x) - fixed_mul(a->x, b->z);
  result.z = fixed_mul(a->x, b->y) - fixed_mul(a->y, b->x);
  *out = result;
}

inline fixed vec3_len_sqr(const vec3 *v) {
  return fixed_mul(v->x, v->x) +
    fixed_mul(v->y, v->y) +
    fixed_mul(v->z, v->z);
}

inline fixed vec3_len(const vec3 *v) {
  return fixed_sqrt(vec3_len_sqr(v));
}

inline void vec3_normalize(vec3 *v) {
  fixed length = vec3_len(v);
  fixed inv_len;
  if (length <= 0) { return; }
  inv_len = fixed_div(FIXED_ONE, length);
  v->x = fixed_mul(v->x, inv_len);
  v->y = fixed_mul(v->y, inv_len);
  v->z = fixed_mul(v->z, inv_len);
}

#endif /* __freak_vec_h_ */
