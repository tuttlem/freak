#include "democom.h"

void freak_demo_palette_default(void)
{
    freak_palette_greyscale(0, 64, 0, 63);
    freak_palette_fire(64, 32);
    freak_palette_sky(96, 32);
    freak_set_palette(15, 63, 63, 63);
}
