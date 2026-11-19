#include "distort.h"

#include "../core/fixed.h"
#include "draw.h"

static uint8_t sample(const uint8_t *input, int x, int y)
{
    if (x < 0 || x >= FREAK_SCREEN_WIDTH || y < 0 || y >= FREAK_SCREEN_HEIGHT)
        return 0;
    return input[y * FREAK_SCREEN_WIDTH + x];
}

void freak_distort_lens(uint8_t *output, const uint8_t *input, int center_x,
                        int center_y, int radius, int strength)
{
    int x, y, dx, dy;
    long distance;
    long radius2;
    int source_x, source_y;

    if (!output || !input || radius < 1) return;
    radius2 = (long)radius * radius;
    for (y = 0; y < FREAK_SCREEN_HEIGHT; ++y)
        for (x = 0; x < FREAK_SCREEN_WIDTH; ++x) {
            dx = x - center_x; dy = y - center_y;
            distance = (long)dx * dx + (long)dy * dy;
            if (distance < radius2) {
                source_x = x - (int)((long)dx * strength * (radius2 - distance) /
                                    (radius2 * radius));
                source_y = y - (int)((long)dy * strength * (radius2 - distance) /
                                    (radius2 * radius));
                output[y * FREAK_SCREEN_WIDTH + x] = sample(input, source_x, source_y);
            } else output[y * FREAK_SCREEN_WIDTH + x] = input[y * FREAK_SCREEN_WIDTH + x];
        }
}

void freak_distort_wave(uint8_t *output, const uint8_t *input, unsigned int phase,
                        int amplitude)
{
    int x, y, offset;
    if (!output || !input) return;
    for (y = 0; y < FREAK_SCREEN_HEIGHT; ++y) {
        offset = fixed_to_int(fixed_sin((y * 9 + phase * 7) & 1023) * amplitude);
        for (x = 0; x < FREAK_SCREEN_WIDTH; ++x)
            output[y * FREAK_SCREEN_WIDTH + x] = sample(input, x + offset, y);
    }
}

void freak_distort_shockwave(uint8_t *output, const uint8_t *input,
                             int center_x, int center_y, int radius,
                             int width, int strength)
{
    int x, y, dx, dy, distance, delta;
    if (!output || !input || radius < 1 || width < 1) return;
    for (y = 0; y < FREAK_SCREEN_HEIGHT; ++y)
        for (x = 0; x < FREAK_SCREEN_WIDTH; ++x) {
            dx = x - center_x; dy = y - center_y;
            distance = ((dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy)) / 2;
            delta = distance - radius;
            if (delta < 0) delta = -delta;
            if (delta < width) {
                output[y * FREAK_SCREEN_WIDTH + x] = sample(input,
                    x + dx * strength * (width - delta) / (width * radius),
                    y + dy * strength * (width - delta) / (width * radius));
            } else output[y * FREAK_SCREEN_WIDTH + x] = input[y * FREAK_SCREEN_WIDTH + x];
        }
}
