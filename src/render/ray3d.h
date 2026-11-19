#ifndef __freak_ray3d_h_
#define __freak_ray3d_h_

#include <stdint.h>

#include "../core/vec.h"

struct freak_ray3d_map {
    const uint8_t *tiles;
    int width;
    int height;
};

struct freak_ray3d_hit {
    fixed distance;
    int cell_x;
    int cell_y;
    uint8_t tile;
};

/* Grid ray marcher used by the column renderer; direction must be unit-ish. */
int freak_ray3d_cast(const struct freak_ray3d_map *map, fixed x, fixed y,
                     fixed direction_x, fixed direction_y,
                     fixed maximum_distance, struct freak_ray3d_hit *hit);

enum freak_raytrace_kind { FREAK_RAYTRACE_SPHERE, FREAK_RAYTRACE_PLANE };
struct freak_raytrace_object {
    enum freak_raytrace_kind kind;
    vec3 position;
    fixed radius;
    uint8_t colour;
    uint8_t reflective;
    uint8_t refractive;
};

/* Traces a small scene with hard shadows and palette-friendly reflections. */
uint8_t freak_raytrace_sample(const struct freak_raytrace_object *objects,
                              unsigned int count, const vec3 *origin,
                              const vec3 *direction, unsigned int depth);

#endif
