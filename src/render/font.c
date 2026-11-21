#include "font.h"
#include "draw.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

void freak_font_asset_init(struct freak_font_asset *asset) {
    if (!asset) return;
    asset->storage = 0;
    asset->font.bits = 0; asset->font.width = asset->font.height = 0;
    asset->font.first = asset->font.count = 0;
}

void freak_font_asset_destroy(struct freak_font_asset *asset) {
    if (!asset) return;
    if (asset->storage) free(asset->storage);
    freak_font_asset_init(asset);
}

int freak_font_asset_load(struct freak_font_asset *asset, const char *filename) {
    FILE *file;
    char magic[4];
    unsigned long bytes;
    int width, height, first, count;
    if (!asset || !filename) return 0;
    freak_font_asset_destroy(asset);
    file = fopen(filename, "rb"); if (!file) return 0;
    if (fread(magic, 1, 4, file) != 4 || memcmp(magic, "FFN1", 4) ||
        (width = fgetc(file)) < 1 || width > 8 ||
        (height = fgetc(file)) < 1 || (first = fgetc(file)) == EOF ||
        (count = fgetc(file)) < 1) { fclose(file); return 0; }
    bytes = (unsigned long)height * count;
    asset->storage = (uint8_t *)malloc(bytes);
    if (!asset->storage || fread(asset->storage, 1, bytes, file) != bytes) {
        fclose(file); freak_font_asset_destroy(asset); return 0;
    }
    fclose(file);
    asset->font.bits = asset->storage; asset->font.width = (uint8_t)width;
    asset->font.height = (uint8_t)height; asset->font.first = (uint8_t)first;
    asset->font.count = (uint8_t)count;
    return 1;
}

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

struct freak_text_size freak_font_measure(const struct freak_font *font,
                                          const char *text) {
    struct freak_text_size size = {0, 0};
    int line_width = 0;
    if (font == 0 || text == 0)
        return size;
    size.height = font->height;
    while (*text) {
        if (*text == '\n') {
            if (line_width > size.width)
                size.width = line_width;
            line_width = 0;
            size.height += font->height;
        } else
            line_width += font->width;
        ++text;
    }
    if (line_width > size.width)
        size.width = line_width;
    return size;
}
