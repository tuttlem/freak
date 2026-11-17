#ifndef __freak_rng_h_
#define __freak_rng_h_

#include <stdint.h>

struct freak_rng {
    uint32_t state;
};

/* Convenient independent streams derived from one reproducible master seed. */
struct freak_rng_streams {
    struct freak_rng gameplay;
    struct freak_rng particles;
    struct freak_rng visuals;
};

void freak_rng_seed(struct freak_rng *rng, uint32_t seed);

/* Derive a stable seed for a subsystem without advancing another stream. */
uint32_t freak_rng_derive_seed(uint32_t master_seed, uint32_t stream_id);
void freak_rng_seed_derived(struct freak_rng *rng, uint32_t master_seed,
                            uint32_t stream_id);
void freak_rng_streams_seed(struct freak_rng_streams *streams,
                            uint32_t master_seed);

uint32_t freak_rng_next_u32(struct freak_rng *rng);
uint16_t freak_rng_next_u16(struct freak_rng *rng);

int freak_rng_range(struct freak_rng *rng, int limit);
int freak_rng_between(struct freak_rng *rng, int minimum, int maximum);
uint8_t freak_rng_chance(struct freak_rng *rng, unsigned int numerator,
                         unsigned int denominator);

#endif
