/******************************************************************************
 * Small fixed-point shading helpers for paletted software renderers.
 ******************************************************************************/
#ifndef __freak_shade3d_h_
#define __freak_shade3d_h_

#include <stdint.h>

#include "../core/vec.h"

enum freak_light3d_type {
    FREAK_LIGHT3D_DIRECTIONAL,
    FREAK_LIGHT3D_POINT
};

struct freak_light3d {
    enum freak_light3d_type type;
    vec3 direction;
    vec3 position;
    fixed intensity;
    fixed range;
};

/* Returns an 0..255 intensity suitable for a contiguous shade ramp. */
uint8_t freak_shade3d_lambert(const vec3 *position, const vec3 *normal,
                              const struct freak_light3d *light,
                              uint8_t ambient, uint8_t specular);

/* Decodes a conventional RGB normal-map texel and lights it. */
uint8_t freak_shade3d_bump(const uint8_t *normal_map, int u, int v,
                           const vec3 *position,
                           const struct freak_light3d *light,
                           uint8_t ambient);

/* Produces wrapped 0..255 sphere-map texture coordinates. */
void freak_shade3d_sphere_uv(const vec3 *normal, const vec3 *view,
                             uint8_t *u, uint8_t *v);

/* Cube-map-like lookup: returns face 0..5 and wrapped texture coordinates. */
uint8_t freak_shade3d_cube_uv(const vec3 *direction, uint8_t *u, uint8_t *v);

#endif
