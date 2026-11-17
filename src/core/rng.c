#include "rng.h"

void freak_rng_seed(struct freak_rng *rng, uint32_t seed) {
    if (rng == 0)
        return;
    rng->state = seed ? seed : 1U;
}

uint32_t freak_rng_derive_seed(uint32_t master_seed, uint32_t stream_id) {
    uint32_t mixed;

    /* SplitMix-style integer scrambling gives unrelated deterministic seeds. */
    mixed = master_seed + 0x9e3779b9UL + stream_id * 0x85ebca6bUL;
    mixed ^= mixed >> 16;
    mixed *= 0x7feb352dUL;
    mixed ^= mixed >> 15;
    mixed *= 0x846ca68bUL;
    mixed ^= mixed >> 16;
    return mixed ? mixed : 1U;
}

void freak_rng_seed_derived(struct freak_rng *rng, uint32_t master_seed,
                            uint32_t stream_id) {
    freak_rng_seed(rng, freak_rng_derive_seed(master_seed, stream_id));
}

void freak_rng_streams_seed(struct freak_rng_streams *streams,
                            uint32_t master_seed) {
    if (streams == 0)
        return;
    freak_rng_seed_derived(&streams->gameplay, master_seed, 1U);
    freak_rng_seed_derived(&streams->particles, master_seed, 2U);
    freak_rng_seed_derived(&streams->visuals, master_seed, 3U);
}

uint32_t freak_rng_next_u32(struct freak_rng *rng) {
    if (rng == 0)
        return 0;
    rng->state = rng->state * 1664525UL + 1013904223UL;
    return rng->state;
}

uint16_t freak_rng_next_u16(struct freak_rng *rng) {
    return (uint16_t)(freak_rng_next_u32(rng) >> 16);
}

int freak_rng_range(struct freak_rng *rng, int limit) {
    if (limit <= 0)
        return 0;
    return ((int)freak_rng_next_u16(rng) * limit) >> 16;
}

int freak_rng_between(struct freak_rng *rng, int minimum, int maximum) {
    if (maximum <= minimum)
        return minimum;
    return minimum + freak_rng_range(rng, maximum - minimum);
}

uint8_t freak_rng_chance(struct freak_rng *rng, unsigned int numerator,
                         unsigned int denominator) {
    if (denominator == 0 || numerator == 0)
        return 0;
    if (numerator >= denominator)
        return 1;
    return freak_rng_range(rng, (int)denominator) < (int)numerator;
}
