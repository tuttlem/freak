
#include <stdlib.h>
#include "../src/freak.h"

int main(int argc, char *argv[]) {

    unsigned char buf[64000], c;

    freak_set_mcga();
    freak_clear_buffer(0, buf);

    while (!freak_kbhit()) {

        freak_poly_c(50, 50, 100, 100, 50, 150, 0, 100, 15, buf);
        freak_copy_buffer(freak_vga, buf);

    }

    freak_set_text();

    return 0;
}