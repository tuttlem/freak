#ifndef __freak_particle_h_
#define __freak_particle_h_

#include <stdint.h>

#include "../core/fixed.h"
#include "../core/rng.h"

#define FREAK_PARTICLE_MAX 256
#define FREAK_PARTICLE_TRAIL    0x01
#define FREAK_PARTICLE_ADDITIVE 0x02
#define FREAK_PARTICLE_TRANSLUCENT 0x04

struct freak_particle {
    fixed x, y;
    fixed old_x, old_y;
    fixed vx, vy;
    fixed gravity;
    uint16_t life;
    uint16_t total_life;
    uint8_t first_colour;
    uint8_t last_colour;
    uint8_t size;
    uint8_t flags;
    uint8_t alive;
};

struct freak_particle_emitter {
    int x, y;
    int velocity_x_min, velocity_x_max;
    int velocity_y_min, velocity_y_max;
    int gravity;
    uint16_t life_min, life_max;
    uint8_t first_colour, last_colour;
    uint8_t size_min, size_max;
    uint8_t flags;
};

struct freak_particles {
    struct freak_particle items[FREAK_PARTICLE_MAX];
    struct freak_rng rng;
};

void freak_particles_init(struct freak_particles *particles, uint32_t seed);
void freak_particles_clear(struct freak_particles *particles);
void freak_particles_emit(struct freak_particles *particles,
                          const struct freak_particle_emitter *emitter,
                          unsigned int count);
void freak_particles_update(struct freak_particles *particles,
                            unsigned long elapsed_ticks);
void freak_particles_draw(const struct freak_particles *particles,
                          uint8_t *buffer);
unsigned int freak_particles_alive(const struct freak_particles *particles);

#endif
