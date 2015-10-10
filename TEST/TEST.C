
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "../src/freak.h"

int main(int argc, char *argv[]) {
    unsigned char *tex;
    unsigned char c;
    int  x, y;

    tex = (unsigned char *)malloc(256*256);

    for (x = 0; x < 256; x ++) {
        for (y = 0; y < 256; y ++) {
            tex[x+(y<<8)] = x^y;
        }
    }

    freak_set_mcga();
    // freak_clear_buffer(0, buf);

    while (!freak_kbhit()) {

        int x1 = rand() % 320, y1 = rand() % 200,
            x2 = rand() % 320, y2 = rand() % 200,
            x3 = rand() % 320, y3 = rand() % 200,
            x4 = rand() % 320, y4 = rand() % 200;
        
        uint8_t c1 = rand() % 256, c2 = rand() % 256,
                c3 = rand() % 256, c4 = rand() % 256;

        freak_poly_t(
            x1, y1, x2, y2, x3, y3, x4, y4,
            0, 0, 255, 0, 255, 255, 0, 255,
            tex, freak_vga
        );

       // freak_wait_vsync();
        //freak_copy_buffer(freak_vga, buf);

    }

    freak_set_text();
    free(tex);    

    return 0;
}
