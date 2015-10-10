
#include <stdlib.h>
#include "../src/freak.h"

int main(int argc, char *argv[]) {

    unsigned char buf[64000], c;
    fixed x = 0, y = 0, q = 0;
    short ang = 0;

    freak_set_mcga();
    freak_clear_buffer(0, buf);

    c = 0;
    ang = 0;

    while (!freak_kbhit()) {

        x = fixed_mul(fixed_cos(ang), int_to_fixed(q)) + int_to_fixed(160);
        y = fixed_mul(fixed_sin(ang), int_to_fixed(q)) + int_to_fixed(100);

        buf[fixed_to_int(x) + (fixed_to_int(y) * 320)] = c;

        ang ++;
        if (ang > 1023) { 
            ang = 0; 
            q++;

            if (q > 90) { q = 0; }

            freak_wait_vsync();
            freak_copy_buffer(freak_vga, buf);

            c ++;
        }

    }

    freak_set_text();

    return 0;
}