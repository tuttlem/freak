
#include "../src/freak.h"

int main(int argc, char *argv[]) {

    unsigned char c = 0;
    int x[4], y[4];

    freak_set_mcga();

    for (c = 0; c < 64; c ++) {
        freak_set_palette(c, c, 0, 0);
        freak_set_palette(c + 64, 63, c, 0);
        freak_set_palette(c + 128, 63, 63, c);
        freak_set_palette(c + 192, 63-c, 63-c, 63-c);
    }

/*
    for (x = 0; x < 256; x ++) {
        for (y = 0; y < 200; y ++) {
            freak_vga[x + (y * 320)] = (unsigned char)x;
        }
    }
*/
    while (!freak_kbhit()) { 
    }

    freak_set_text();

    return 0;
}
