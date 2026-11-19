#ifndef __freak_flare_h_
#define __freak_flare_h_

#include <stdint.h>

struct freak_lens_flare {
    int source_x;
    int source_y;
    int radius;
    uint8_t colour;
    uint8_t intensity;
    uint8_t occluded;
};

void freak_lens_flare_draw(uint8_t *buffer, const struct freak_lens_flare *flare,
                           int screen_center_x, int screen_center_y);

#endif
