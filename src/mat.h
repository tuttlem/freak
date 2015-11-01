
#ifndef __freak_mat_h_

#define __freak_mat_h_

#include <stdio.h>

#include "fixed.h"

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

#endif /* __freak_mat_h_ */
