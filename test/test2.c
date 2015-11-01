
#include <stdio.h>

#include "../src/freak.h"

void print_vec3(vec3 *v);

int main(int argc, char *argv[]) {
  /*
  mat44 a, b, c;
  vec3 v1, v2, v3;
  
  vec3_set(&v1, int_to_fixed(0), int_to_fixed(0), int_to_fixed(1));
  vec3_set(&v2, int_to_fixed(0), int_to_fixed(1), int_to_fixed(0));
  
  mat44_axis_rot(&a, &v1, double_to_fixed(0.003906));
  mat44_mul_vec(&v3, &a, &v2); 

  mat44_print(&a);
  print_vec3(&v3);
  */

  printf("%d\n", fixed_to_theta(FIXED_PI / 2));
  
  return 0;
}

void print_vec3(vec3 *v) {
  double x = fixed_to_double(v->x),
    y = fixed_to_double(v->y),
    z = fixed_to_double(v->z);

  printf("(%f, %f, %f)\n", x, y, z);
}
