#include "particle.h"

#include "../render/draw.h"

static struct freak_particle *next_free(struct freak_particles *particles)
{
    unsigned int index;

    for (index = 0; index < FREAK_PARTICLE_MAX; ++index)
        if (!particles->items[index].alive)
            return &particles->items[index];
    return 0;
}

void freak_particles_init(struct freak_particles *particles, uint32_t seed)
{
    if (particles == 0)
        return;
    freak_rng_seed(&particles->rng, seed);
    freak_particles_clear(particles);
}

void freak_particles_clear(struct freak_particles *particles)
{
    unsigned int index;

    if (particles == 0)
        return;
    for (index = 0; index < FREAK_PARTICLE_MAX; ++index)
        particles->items[index].alive = 0;
}

void freak_particles_emit(struct freak_particles *particles,
                          const struct freak_particle_emitter *emitter,
                          unsigned int count)
{
    struct freak_particle *particle;
    unsigned int index;

    if (particles == 0 || emitter == 0)
        return;
    for (index = 0; index < count; ++index) {
        particle = next_free(particles);
        if (particle == 0)
            return;
        particle->x = int_to_fixed(emitter->x);
        particle->y = int_to_fixed(emitter->y);
        particle->old_x = particle->x;
        particle->old_y = particle->y;
        particle->vx = double_to_fixed(freak_rng_between(&particles->rng,
            emitter->velocity_x_min, emitter->velocity_x_max) / 100.0);
        particle->vy = double_to_fixed(freak_rng_between(&particles->rng,
            emitter->velocity_y_min, emitter->velocity_y_max) / 100.0);
        particle->gravity = double_to_fixed(emitter->gravity / 100.0);
        particle->life = (uint16_t)freak_rng_between(&particles->rng,
            emitter->life_min, emitter->life_max);
        if (particle->life == 0) particle->life = 1;
        particle->total_life = particle->life;
        particle->first_colour = emitter->first_colour;
        particle->last_colour = emitter->last_colour;
        particle->size = (uint8_t)freak_rng_between(&particles->rng,
            emitter->size_min, emitter->size_max);
        if (particle->size == 0) particle->size = 1;
        particle->flags = emitter->flags;
        particle->alive = 1;
    }
}

void freak_particles_update(struct freak_particles *particles,
                            unsigned long elapsed_ticks)
{
    unsigned int index;
    unsigned long tick;
    struct freak_particle *particle;

    if (particles == 0) return;
    if (elapsed_ticks > 8) elapsed_ticks = 8;
    for (tick = 0; tick < elapsed_ticks; ++tick)
        for (index = 0; index < FREAK_PARTICLE_MAX; ++index) {
            particle = &particles->items[index];
            if (!particle->alive) continue;
            particle->old_x = particle->x;
            particle->old_y = particle->y;
            particle->x += particle->vx;
            particle->y += particle->vy;
            particle->vy += particle->gravity;
            if (--particle->life == 0)
                particle->alive = 0;
        }
}

void freak_particles_draw(const struct freak_particles *particles,
                          uint8_t *buffer)
{
    unsigned int index;
    const struct freak_particle *particle;
    int x, y, old_x, old_y;
    int colour;

    if (particles == 0 || buffer == 0) return;
    for (index = 0; index < FREAK_PARTICLE_MAX; ++index) {
        particle = &particles->items[index];
        if (!particle->alive) continue;
        colour = particle->last_colour +
            ((particle->first_colour - particle->last_colour) * particle->life) /
            particle->total_life;
        x = fixed_to_int(particle->x);
        y = fixed_to_int(particle->y);
        old_x = fixed_to_int(particle->old_x);
        old_y = fixed_to_int(particle->old_y);
        if (particle->flags & FREAK_PARTICLE_TRAIL)
            freak_draw_line(buffer, old_x, old_y, x, y, (uint8_t)colour);
        if ((particle->flags & FREAK_PARTICLE_TRANSLUCENT) &&
            ((x + y + particle->life) & 1))
            continue;
        if (particle->flags & FREAK_PARTICLE_ADDITIVE) {
            if (x >= 0 && x < FREAK_SCREEN_WIDTH && y >= 0 && y < FREAK_SCREEN_HEIGHT &&
                buffer[y * FREAK_SCREEN_WIDTH + x] < colour)
                buffer[y * FREAK_SCREEN_WIDTH + x] = (uint8_t)colour;
        } else
            freak_fill_rect(buffer, x, y, x + particle->size - 1,
                            y + particle->size - 1, (uint8_t)colour);
    }
}

unsigned int freak_particles_alive(const struct freak_particles *particles)
{
    unsigned int index;
    unsigned int count;

    if (particles == 0) return 0;
    count = 0;
    for (index = 0; index < FREAK_PARTICLE_MAX; ++index)
        if (particles->items[index].alive) ++count;
    return count;
}
