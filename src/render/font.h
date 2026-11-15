#ifndef __freak_font_h_
#define __freak_font_h_
#include <stdint.h>

struct freak_font {
    const uint8_t *bits;
    uint8_t width, height, first, count;
};

void freak_font_char(uint8_t *buffer, int x, int y,
                     const struct freak_font *font, char character,
                     uint8_t colour);
void freak_font_text(uint8_t *buffer, int x, int y,
                     const struct freak_font *font, const char *text,
                     uint8_t colour);
#endif
