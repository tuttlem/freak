#ifndef __freak_rng_h_
#define __freak_rng_h_

#include <stdint.h>

struct freak_rng {
    uint32_t state;
};

void freak_rng_seed(struct freak_rng *rng, uint32_t seed);

uint16_t freak_rng_next_u16(struct freak_rng *rng);

int freak_rng_range(struct freak_rng *rng, int limit);

#endif
