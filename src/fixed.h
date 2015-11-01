
#ifndef __freak_math_h_

#define __freak_math_h_

#define MATH_FRAC_BITS    16
#define MATH_FRAC_MAG     65536.0

#define int_to_fixed(x)       ((x) << MATH_FRAC_BITS)
#define double_to_fixed(x)    ((fixed)(x * MATH_FRAC_MAG + 0.5))
#define fixed_to_int(x)       ((x) >> MATH_FRAC_BITS)
#define fixed_to_double(x)    (((double)x) / MATH_FRAC_MAG)

/* fixed-point data type */
typedef long fixed;

extern fixed _fixed_sin[];
extern fixed _fixed_cos[];

// theta radian (fixed) to angle index
#define fixed_to_theta(t)     (fixed_to_int(fixed_mul(fixed_div(t, FIXED_2PI), int_to_fixed(1024))) & 1023)

#define fixed_sin(t)          (_fixed_sin[t & 1023])
#define fixed_cos(t)          (_fixed_cos[t & 1023])
#define fixed_tan(t)          (fixed_div(fixed_sin(t) << 16, fixed_cos(t)) >> 16)

#define ONE             int_to_fixed(1)
#define FIXED_ZERO      0
#define FIXED_ONE       int_to_fixed(1)
#define FIXED_NEGONE    int_to_fixed(-1)
#define FIXED_PI        205887L
#define FIXED_2PI       411775L
#define FIXED_E         178144L
#define FIXED_ROOT2      74804L
#define FIXED_ROOT3     113512L
#define FIXED_GOLDEN    106039L

fixed fixed_mul(fixed a, fixed b);
#pragma aux fixed_mul =   \
  "imul   edx"            \
  "add    eax, 8000h"     \
  "adc    edx, 0"         \
  "shrd   eax, edx, 16"   \
  parm caller [eax] [edx] \
  value [eax]             \
  modify [eax edx];

fixed fixed_div(fixed a, fixed b);
#pragma aux fixed_div =   \
  "xor    eax, eax"       \
  "shrd   eax, edx, 16"   \
  "sar    edx, 16"        \
  "idiv   ebx"            \
  parm caller [edx] [ebx] \
  value [eax]             \
  modify [eax ebx edx];

fixed fixed_sqr(fixed n);
#pragma aux fixed_sqr = \
  "imul   eax"          \
  "add    eax, 8000h"   \
  "adc    edx, 0"       \
  "shrd   eax, edx, 16" \
  parm caller [eax]     \
  value [eax]           \
  modify [eax edx];

fixed fixed_one_over(fixed n);
#pragma aux fixed_one_over = \
  "xor    eax, eax"          \
  "mov    edx, 1"            \
  "idiv   ebx"               \
  parm caller [ebx]          \
  value [eax]                \
  modify [eax ebx edx];

fixed fixed_sqrt_fast(fixed n);
#pragma aux fixed_sqrt_fast = \
  "xor  eax, eax"             \
  "mov  ebx, 40000000h"       \
  "sqrtLP1: "                 \
  "mov edx,  ecx"             \
  "sub  edx, ebx"             \
  "jl   sqrtLP2"              \
  "sub  edx, eax"             \
  "jl   sqrtLP2"              \
  "mov  ecx, edx"             \
  "shr  eax, 1"               \
  "or   eax, ebx"             \
  "shr  ebx, 2"               \
  "jnz  sqrtLP1"              \
  "shl  eax, 8"               \
  "jmp  sqrtLP3"              \
  "sqrtLP2: "                 \
  "shr  eax, 1"               \
  "shr  ebx, 2"               \
  "jnz  sqrtLP1"              \
  "shl  eax, 8"               \
  "sqrtLP3: "                 \
  "nop"                       \
  parm caller [ecx]           \
  value [eax]                 \
  modify [eax ebx ecx edx];

fixed fixed_sqrt(fixed n);
#pragma aux fixed_sqrt =   \
  "xor  eax, eax"          \
  "mov  ebx, 40000000h"    \
  "sqrtHP1: "              \
  "mov  edx, ecx"          \
  "sub  edx, ebx"          \
  "jb   sqrtHP2"           \
  "sub  edx, eax"          \
  "jb   sqrtHP2"           \
  "mov  ecx,edx"           \
  "shr  eax, 1"            \
  "or   eax, ebx"          \
  "shr  ebx, 2"            \
  "jnz  sqrtHP1"           \
  "jz   sqrtHP5"           \
  "sqrtHP2: "              \
  "shr  eax, 1"            \
  "shr  ebx, 2"            \
  "jnz  sqrtHP1"           \
  "sqrtHP5:"               \
  "mov  ebx, 00004000h"    \
  "shl  eax, 16"           \
  "shl  ecx, 16"           \
  "sqrtHP3: "              \
  "mov  edx, ecx"          \
  "sub  edx, ebx"          \
  "jb   sqrtHP4"           \
  "sub  edx, eax"          \
  "jb   sqrtHP4"           \
  "mov  ecx, edx"          \
  "shr  eax, 1"            \
  "or   eax, ebx"          \
  "shr  ebx, 2"            \
  "jnz  sqrtHP3"           \
  "jmp  sqrtHP6"           \
  "sqrtHP4: "              \
  "shr  eax, 1"            \
  "shr  ebx, 2"            \
  "jnz  sqrtHP3"           \
  "sqrtHP6: "              \
  "nop"                    \
  parm caller [ecx]        \
  value [eax]              \
  modify [eax ebx ecx edx];

#endif /* __freak_math_h_ */

