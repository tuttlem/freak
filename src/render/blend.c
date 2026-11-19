#include "blend.h"
#include "palette.h"

#include "../dos/vga.h"
#include "draw.h"

static struct freak_colour blend_palette[256];

static uint8_t nearest_palette(uint8_t red, uint8_t green, uint8_t blue)
{
    int index;
    int best;
    long distance;
    long best_distance;
    int dr, dg, db;
    uint8_t palette_red, palette_green, palette_blue;

    best = 0;
    best_distance = 0x7fffffffL;
    for (index = 0; index < 256; ++index) {
        palette_red = blend_palette[index].red;
        palette_green = blend_palette[index].green;
        palette_blue = blend_palette[index].blue;
        dr = (int)palette_red - red;
        dg = (int)palette_green - green;
        db = (int)palette_blue - blue;
        distance = (long)dr * dr + (long)dg * dg + (long)db * db;
        if (distance < best_distance) {
            best_distance = distance;
            best = index;
        }
    }
    return (uint8_t)best;
}

void freak_blend_table_build(uint8_t *table, enum freak_blend_mode mode)
{
    int destination;
    int source;
    int red, green, blue;
    uint8_t dr, dg, db, sr, sg, sb;

    if (table == 0) return;
    for (destination = 0; destination < 256; ++destination)
        freak_get_palette((uint8_t)destination, &blend_palette[destination].red,
                          &blend_palette[destination].green,
                          &blend_palette[destination].blue);
    for (destination = 0; destination < 256; ++destination) {
        dr = blend_palette[destination].red;
        dg = blend_palette[destination].green;
        db = blend_palette[destination].blue;
        for (source = 0; source < 256; ++source) {
            sr = blend_palette[source].red;
            sg = blend_palette[source].green;
            sb = blend_palette[source].blue;
            if (mode == FREAK_BLEND_ADDITIVE) {
                red = dr + sr; green = dg + sg; blue = db + sb;
                if (red > 63) red = 63; if (green > 63) green = 63; if (blue > 63) blue = 63;
            } else if (mode == FREAK_BLEND_SUBTRACTIVE) {
                red = dr - sr; green = dg - sg; blue = db - sb;
                if (red < 0) red = 0; if (green < 0) green = 0; if (blue < 0) blue = 0;
            } else if (mode == FREAK_BLEND_MULTIPLY) {
                red = dr * sr / 63; green = dg * sg / 63; blue = db * sb / 63;
            } else {
                red = (dr + sr) / 2; green = (dg + sg) / 2; blue = (db + sb) / 2;
            }
            table[(destination << 8) | source] = nearest_palette((uint8_t)red,
                (uint8_t)green, (uint8_t)blue);
        }
    }
}

uint8_t freak_blend_pixel(const uint8_t *table, uint8_t destination,
                          uint8_t source)
{
    return table ? table[(destination << 8) | source] : source;
}

void freak_blend_rect(uint8_t *pixels, int left, int top, int right, int bottom,
                      uint8_t source, const uint8_t *table)
{
    int x, y;
    if (!pixels || !table) return;
    if (left < 0) left = 0; if (top < 0) top = 0;
    if (right >= FREAK_SCREEN_WIDTH) right = FREAK_SCREEN_WIDTH - 1;
    if (bottom >= FREAK_SCREEN_HEIGHT) bottom = FREAK_SCREEN_HEIGHT - 1;
    for (y = top; y <= bottom; ++y)
        for (x = left; x <= right; ++x)
            pixels[y * FREAK_SCREEN_WIDTH + x] =
                freak_blend_pixel(table, pixels[y * FREAK_SCREEN_WIDTH + x], source);
}
