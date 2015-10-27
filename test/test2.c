
#include <stdio.h>

#include "../src/freak.h"

void print_vec3(vec3 *v);

int main(int argc, char *argv[]) {
  vec3 v1, v2, v3;
  
  vec3_set(&v1, int_to_fixed(5), int_to_fixed(7), int_to_fixed(9));
  /*  vec3_set(&v2, double_to_fixed(0.5), double_to_fixed(0.7), double_to_fixed(0.9));
      vec3_add(&v3, &v1, &v2);*/
  
  print_vec3(&v1);

  printf("(%d, %d, %d)\n", v1.x, v1.y, v1.z);
  
  printf("lensqr: %f\n", fixed_to_double(vec3_len_sqr(&v1)));
  printf("length: %f\n", fixed_to_double(vec3_len(&v1)));

  vec3_neg(&v1);
  print_vec3(&v1);

  return 0;
}

void print_vec3(vec3 *v) {
  double x = fixed_to_double(v->x),
    y = fixed_to_double(v->y),
    z = fixed_to_double(v->z);

  printf("(%f, %f, %f)\n", x, y, z);
}
