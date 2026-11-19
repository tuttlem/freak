#ifndef __freak_distort_h_
#define __freak_distort_h_

#include <stdint.h>

void freak_distort_lens(uint8_t *output, const uint8_t *input, int center_x,
                        int center_y, int radius, int strength);
void freak_distort_wave(uint8_t *output, const uint8_t *input, unsigned int phase,
                        int amplitude);
void freak_distort_shockwave(uint8_t *output, const uint8_t *input,
                             int center_x, int center_y, int radius,
                             int width, int strength);

#endif
