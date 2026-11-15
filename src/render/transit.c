#include "transit.h"
#include "draw.h"
#include "../core/noise.h"
#include "../dos/vga.h"

static int transition_progress(int step, int steps) {
    if (steps <= 0)
        return 65535;
    if (step <= 0)
        return 0;
    if (step >= steps)
        return 65535;
    return (step * 65535) / steps;
}

void freak_transition_wipe_lr(uint8_t *output, const uint8_t *from,
                              const uint8_t *to, int step, int steps) {
    int x, y,
        cut = transition_progress(step, steps) * FREAK_SCREEN_WIDTH / 65535;
    for (y = 0; y < FREAK_SCREEN_HEIGHT; ++y)
        for (x = 0; x < FREAK_SCREEN_WIDTH; ++x)
            output[y * FREAK_SCREEN_WIDTH + x] =
                x < cut ? to[y * FREAK_SCREEN_WIDTH + x]
                        : from[y * FREAK_SCREEN_WIDTH + x];
}

void freak_transition_wipe_tb(uint8_t *output, const uint8_t *from,
                              const uint8_t *to, int step, int steps) {
    int x, y,
        cut = transition_progress(step, steps) * FREAK_SCREEN_HEIGHT / 65535;
    for (y = 0; y < FREAK_SCREEN_HEIGHT; ++y)
        for (x = 0; x < FREAK_SCREEN_WIDTH; ++x)
            output[y * FREAK_SCREEN_WIDTH + x] =
                y < cut ? to[y * FREAK_SCREEN_WIDTH + x]
                        : from[y * FREAK_SCREEN_WIDTH + x];
}

void freak_transition_scroll_lr(uint8_t *output, const uint8_t *from,
                                const uint8_t *to, int step, int steps) {
    int x, y,
        offset = transition_progress(step, steps) * FREAK_SCREEN_WIDTH / 65535;
    for (y = 0; y < FREAK_SCREEN_HEIGHT; ++y)
        for (x = 0; x < FREAK_SCREEN_WIDTH; ++x)
            output[y * FREAK_SCREEN_WIDTH + x] =
                x < FREAK_SCREEN_WIDTH - offset
                    ? from[y * FREAK_SCREEN_WIDTH + x + offset]
                    : to[y * FREAK_SCREEN_WIDTH + x + offset -
                         FREAK_SCREEN_WIDTH];
}

void freak_transition_dissolve(uint8_t *output, const uint8_t *from,
                               const uint8_t *to, int step, int steps,
                               unsigned int seed) {
    int x, y, progress = transition_progress(step, steps);
    for (y = 0; y < FREAK_SCREEN_HEIGHT; ++y)
        for (x = 0; x < FREAK_SCREEN_WIDTH; ++x)
            output[y * FREAK_SCREEN_WIDTH + x] =
                freak_noise_2d(x + (int)seed, y + (int)(seed >> 16)) + 32768 <
                        progress
                    ? to[y * FREAK_SCREEN_WIDTH + x]
                    : from[y * FREAK_SCREEN_WIDTH + x];
}

void freak_transition_checker(uint8_t *output, const uint8_t *from,
                              const uint8_t *to, int step, int steps,
                              int cell_size) {
    int x, y, rank, progress = transition_progress(step, steps);
    if (cell_size < 1)
        cell_size = 1;
    for (y = 0; y < FREAK_SCREEN_HEIGHT; ++y)
        for (x = 0; x < FREAK_SCREEN_WIDTH; ++x) {
            rank = ((x / cell_size) + (y / cell_size) * 3) & 15;
            output[y * FREAK_SCREEN_WIDTH + x] =
                rank * 4096 < progress ? to[y * FREAK_SCREEN_WIDTH + x]
                                       : from[y * FREAK_SCREEN_WIDTH + x];
        }
}

void freak_transition_palette_capture(struct freak_palette_snapshot *snapshot,
                                      uint8_t first, uint8_t count) {
    int index;
    if (snapshot == 0)
        return;
    for (index = 0; index < count; ++index)
        freak_get_palette(
            (uint8_t)(first + index), &snapshot->colours[index].red,
            &snapshot->colours[index].green, &snapshot->colours[index].blue);
}

static void transition_fade(const struct freak_palette_snapshot *snapshot,
                            uint8_t first, uint8_t count, int step, int steps,
                            int up) {
    int index, scale;
    if (snapshot == 0)
        return;
    scale = transition_progress(step, steps);
    if (!up)
        scale = 65535 - scale;
    for (index = 0; index < count; ++index)
        freak_set_palette(
            (uint8_t)(first + index),
            (uint8_t)((snapshot->colours[index].red * scale) / 65535),
            (uint8_t)((snapshot->colours[index].green * scale) / 65535),
            (uint8_t)((snapshot->colours[index].blue * scale) / 65535));
}

void freak_transition_fade_down(const struct freak_palette_snapshot *snapshot,
                                uint8_t first, uint8_t count, int step,
                                int steps) {
    transition_fade(snapshot, first, count, step, steps, 0);
}

void freak_transition_fade_up(const struct freak_palette_snapshot *snapshot,
                              uint8_t first, uint8_t count, int step,
                              int steps) {
    transition_fade(snapshot, first, count, step, steps, 1);
}
