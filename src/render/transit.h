#ifndef __freak_transition_h_
#define __freak_transition_h_

#include <stdint.h>
#include "palette.h"

struct freak_palette_snapshot {
    struct freak_colour colours[256];
};

void freak_transition_wipe_lr(uint8_t *output, const uint8_t *from,
                              const uint8_t *to, int step, int steps);
void freak_transition_wipe_tb(uint8_t *output, const uint8_t *from,
                              const uint8_t *to, int step, int steps);
void freak_transition_scroll_lr(uint8_t *output, const uint8_t *from,
                                const uint8_t *to, int step, int steps);
void freak_transition_dissolve(uint8_t *output, const uint8_t *from,
                               const uint8_t *to, int step, int steps,
                               unsigned int seed);
void freak_transition_checker(uint8_t *output, const uint8_t *from,
                              const uint8_t *to, int step, int steps,
                              int cell_size);

void freak_transition_palette_capture(struct freak_palette_snapshot *snapshot,
                                      uint8_t first, uint8_t count);
void freak_transition_fade_down(const struct freak_palette_snapshot *snapshot,
                                uint8_t first, uint8_t count, int step,
                                int steps);
void freak_transition_fade_up(const struct freak_palette_snapshot *snapshot,
                              uint8_t first, uint8_t count, int step,
                              int steps);

#endif
