
#include <stdlib.h>
#include "../src/freak.h"

int main(int argc, char *argv[]) {

  unsigned char buf[64000];

  unsigned char c = 15;
  unsigned int x = 0;

  freak_clear_buffer(0, buf);
  freak_set_mcga();

  for (x = 0; x < 256; x ++) {
    freak_set_palette(x, x >> 2, x >> 2, x >> 2);
  }

  while (!freak_kbhit()) {
    for (x = 0; x < 1000; x ++) {
    unsigned short o = (rand() % 32000) + (rand() % 32000);
    buf[o] = rand() % 255;
  }

    freak_wait_vsync();
    freak_copy_buffer(freak_vga, buf);
    freak_wait_vsync();
    c++;
  }

  freak_set_text();

  return 0;
  
}
