#ifndef __freak_light2d_h_
#define __freak_light2d_h_

#include <stdint.h>
#include "tilemap.h"

#define FREAK_LIGHT2D_MAX_LIGHTS 16

struct freak_light2d {
    int x, y;
    int radius;
    uint8_t intensity;
    uint8_t colour;
    uint8_t flicker;
    uint8_t enabled;
};

struct freak_lighting2d {
    struct freak_light2d lights[FREAK_LIGHT2D_MAX_LIGHTS];
    uint8_t ambient;
    unsigned long ticks;
};

void freak_lighting2d_init(struct freak_lighting2d *lighting, uint8_t ambient);
int freak_lighting2d_add(struct freak_lighting2d *lighting,
                         const struct freak_light2d *light);
void freak_lighting2d_update(struct freak_lighting2d *lighting,
                             unsigned long elapsed_ticks);
void freak_lighting2d_render(const struct freak_lighting2d *lighting,
                             uint8_t *lightmap, int width, int height);
void freak_lighting2d_render_occluded(const struct freak_lighting2d *lighting,
                                      const struct freak_tilemap *map,
                                      uint8_t solid_mask, uint8_t *lightmap,
                                      int width, int height);
void freak_lighting2d_shade(uint8_t *pixels, const uint8_t *lightmap,
                            int width, int height);
void freak_lighting2d_colour_glow(const struct freak_lighting2d *lighting,
                                  uint8_t *pixels, int width, int height);

#endif
