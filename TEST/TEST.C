
#include "../src/freak.h"

int main(int argc, char *argv[]) {

  unsigned char c = 15;
  unsigned int x = 0;

  freak_set_modex();

  while (!freak_kbhit()) {
    for (x = 0; x < (320*240); x++) 
      freak_vga[x] = x % 16;
    
    freak_wait_vsync();
    c++;
  }

  freak_set_text();

  return 0;
  
}
