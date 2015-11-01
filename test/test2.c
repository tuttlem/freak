
#include <stdio.h>

#include "../src/freak.h"

void print_vec3(vec3 *v);

int main(int argc, char *argv[]) {
  mat44 a, b;

  mat44_zero(&a);
  mat44_identity(&b);

  mat44_print(&a);
  mat44_print(&b);

  return 0;
}

void print_vec3(vec3 *v) {
  double x = fixed_to_double(v->x),
    y = fixed_to_double(v->y),
    z = fixed_to_double(v->z);

  printf("(%f, %f, %f)\n", x, y, z);
}
