#ifndef __freak_democtx_h_
#define __freak_democtx_h_

#include "../src/freak.h"

struct freak_demo_context {
    uint8_t *screen;
    const struct freak_font *font;
    struct freak_timing *timing;
};

#endif
