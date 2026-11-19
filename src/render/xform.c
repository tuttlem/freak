#include "xform.h"

#include "../core/fixed.h"
#include "draw.h"

void freak_xform_scale(uint8_t *output, const struct freak_texture *texture,
                       int left, int top, int width, int height)
{
    int x, y;
    if (!output || !texture || !texture->pixels || width < 1 || height < 1) return;
    for (y = 0; y < height; ++y)
        for (x = 0; x < width; ++x)
            freak_draw_pixel(output, left + x, top + y,
                freak_texture_pixel(texture, x * 256 / width, y * 256 / height));
}

void freak_xform_rotozoom(uint8_t *output, const struct freak_texture *texture,
                          int center_x, int center_y, int angle, int zoom)
{
    int x, y, dx, dy, u, v;
    fixed sine, cosine;
    if (!output || !texture || !texture->pixels || zoom < 1) return;
    sine = fixed_sin(angle & 1023); cosine = fixed_cos(angle & 1023);
    for (y = 0; y < FREAK_SCREEN_HEIGHT; ++y)
        for (x = 0; x < FREAK_SCREEN_WIDTH; ++x) {
            dx = x - center_x; dy = y - center_y;
            u = 128 + fixed_to_int((cosine * dx + sine * dy) / zoom);
            v = 128 + fixed_to_int((-sine * dx + cosine * dy) / zoom);
            output[y * FREAK_SCREEN_WIDTH + x] = freak_texture_pixel(texture, u, v);
        }
}

void freak_xform_tunnel(uint8_t *output, const struct freak_texture *texture,
                        int center_x, int center_y, unsigned int phase)
{
    int x, y, dx, dy, distance, angle;
    if (!output || !texture || !texture->pixels) return;
    for (y = 0; y < FREAK_SCREEN_HEIGHT; ++y)
        for (x = 0; x < FREAK_SCREEN_WIDTH; ++x) {
            dx = x - center_x; dy = y - center_y;
            distance = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy) + 1;
            angle = (dy * 64) / distance;
            output[y * FREAK_SCREEN_WIDTH + x] = freak_texture_pixel(texture,
                (2560 / distance + phase * 2) & 255, (angle * 8 + phase) & 255);
        }
}

void freak_xform_floor(uint8_t *output, const struct freak_texture *texture,
                       int horizon, unsigned int phase)
{
    int x, y, distance;
    if (!output || !texture || !texture->pixels) return;
    for (y = horizon; y < FREAK_SCREEN_HEIGHT; ++y) {
        distance = (y - horizon) + 1;
        for (x = 0; x < FREAK_SCREEN_WIDTH; ++x)
            output[y * FREAK_SCREEN_WIDTH + x] = freak_texture_pixel(texture,
                ((x - 160) * 80 / distance + phase) & 255,
                (1800 / distance + phase * 2) & 255);
    }
}
