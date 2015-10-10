
#include <stdio.h>
#include <stdlib.h>
#include "../src/freak.h"

int main(int argc, char *argv[]) {

    unsigned char buf[64000], c;

    freak_set_mcga();
    freak_clear_buffer(0, buf);

    while (!freak_kbhit()) {

        int x1 = rand() % 320, y1 = rand() % 200,
            x2 = rand() % 320, y2 = rand() % 200,
            x3 = rand() % 320, y3 = rand() % 200,
            x4 = rand() % 320, y4 = rand() % 200;
        
        uint8_t c1 = rand() % 256, c2 = rand() % 256,
                c3 = rand() % 256, c4 = rand() % 256;

        freak_poly_g(
            x1, y1, x2, y2, x3, y3, x4, y4,
            c1, c2, c3, c4,
            buf
        );

        freak_wait_vsync();
        freak_copy_buffer(freak_vga, buf);

    }

    freak_set_text();

    return 0;
}
