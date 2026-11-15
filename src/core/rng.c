#include "rng.h"

void freak_rng_seed(struct freak_rng *rng, uint32_t seed) {
    rng->state = seed ? seed : 1U;
}

uint16_t freak_rng_next_u16(struct freak_rng *rng) {
    rng->state = rng->state * 1664525UL + 1013904223UL;
    return (uint16_t)(rng->state >> 16);
}

int freak_rng_range(struct freak_rng *rng, int limit) {
    if (limit <= 0)
        return 0;
    return ((int)freak_rng_next_u16(rng) * limit) >> 16;
}
