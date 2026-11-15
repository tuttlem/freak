#include "font.h"
#include "draw.h"

void freak_font_char(uint8_t *buffer, int x, int y,
                     const struct freak_font *font, char character,
                     uint8_t colour) {
    int row, column, glyph;
    uint8_t bits;
    if (buffer == 0 || font == 0 || font->bits == 0 || font->width > 8 ||
        character < font->first || character >= font->first + font->count)
        return;
    glyph = (unsigned char)character - font->first;
    for (row = 0; row < font->height; ++row) {
        if (y + row < 0 || y + row >= FREAK_SCREEN_HEIGHT)
            continue;
        bits = font->bits[glyph * font->height + row];
        for (column = 0; column < font->width; ++column)
            if ((bits & (128 >> column)) && x + column >= 0 &&
                x + column < FREAK_SCREEN_WIDTH)
                buffer[(y + row) * FREAK_SCREEN_WIDTH + x + column] = colour;
    }
}

void freak_font_text(uint8_t *buffer, int x, int y,
                     const struct freak_font *font, const char *text,
                     uint8_t colour) {
    if (font == 0 || text == 0)
        return;
    while (*text) {
        freak_font_char(buffer, x, y, font, *text++, colour);
        x += font->width;
    }
}
