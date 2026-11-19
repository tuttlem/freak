#include "light2d.h"

#include "tilemap.h"
#include "collide.h"

#define FREAK_LIGHT2D_MAX_OCCLUSION_TILES 4096

static uint8_t light_visibility[FREAK_LIGHT2D_MAX_OCCLUSION_TILES];

static int abs_value(int value)
{
    return value < 0 ? -value : value;
}

static int light_visible(const struct freak_tilemap *map, uint8_t mask,
                         int x0, int y0, int x1, int y1)
{
    int dx = abs_value(x1 - x0);
    int dy = abs_value(y1 - y0);
    int step_x = x0 < x1 ? 1 : -1;
    int step_y = y0 < y1 ? 1 : -1;
    int error = dx - dy;
    int twice_error;

    while (!(x0 == x1 && y0 == y1)) {
        twice_error = error * 2;
        if (twice_error > -dy) { error -= dy; x0 += step_x; }
        if (twice_error < dx) { error += dx; y0 += step_y; }
        if (x0 == x1 && y0 == y1) return 1;
        if (freak_collision_tile_flags(map, x0, y0) & mask) return 0;
    }
    return 1;
}

void freak_lighting2d_init(struct freak_lighting2d *lighting, uint8_t ambient)
{
    unsigned int index;
    if (!lighting) return;
    lighting->ambient = ambient;
    lighting->ticks = 0;
    for (index = 0; index < FREAK_LIGHT2D_MAX_LIGHTS; ++index)
        lighting->lights[index].enabled = 0;
}

int freak_lighting2d_add(struct freak_lighting2d *lighting,
                         const struct freak_light2d *light)
{
    unsigned int index;
    if (!lighting || !light) return -1;
    for (index = 0; index < FREAK_LIGHT2D_MAX_LIGHTS; ++index)
        if (!lighting->lights[index].enabled) {
            lighting->lights[index] = *light;
            lighting->lights[index].enabled = 1;
            return (int)index;
        }
    return -1;
}

void freak_lighting2d_update(struct freak_lighting2d *lighting,
                             unsigned long elapsed_ticks)
{
    if (lighting) lighting->ticks += elapsed_ticks;
}

void freak_lighting2d_render(const struct freak_lighting2d *lighting,
                             uint8_t *lightmap, int width, int height)
{
    int x, y, left, right, top, bottom, distance, strength;
    unsigned int index;
    const struct freak_light2d *light;

    if (!lighting || !lightmap) return;
    for (y = 0; y < height; ++y)
        for (x = 0; x < width; ++x)
            lightmap[y * width + x] = lighting->ambient;
    for (index = 0; index < FREAK_LIGHT2D_MAX_LIGHTS; ++index) {
        light = &lighting->lights[index];
        if (!light->enabled || light->radius < 1) continue;
        left = light->x - light->radius; right = light->x + light->radius;
        top = light->y - light->radius; bottom = light->y + light->radius;
        if (left < 0) left = 0; if (top < 0) top = 0;
        if (right >= width) right = width - 1;
        if (bottom >= height) bottom = height - 1;
        for (y = top; y <= bottom; ++y)
            for (x = left; x <= right; ++x) {
                distance = (x > light->x ? x - light->x : light->x - x) +
                           (y > light->y ? y - light->y : light->y - y);
                if (distance > light->radius) continue;
                strength = (light->intensity * (light->radius - distance)) /
                           light->radius;
                if (light->flicker)
                    strength -= (int)((lighting->ticks + x + y) % light->flicker);
                if (strength > 0 && lightmap[y * width + x] < strength)
                    lightmap[y * width + x] = (uint8_t)strength;
            }
    }
}

void freak_lighting2d_render_occluded(const struct freak_lighting2d *lighting,
                                      const struct freak_tilemap *map,
                                      uint8_t solid_mask, uint8_t *lightmap,
                                      int width, int height)
{
    int x, y, left, right, top, bottom, distance, strength;
    int tile_x, tile_y;
    unsigned int tile_count;
    unsigned int tile_index;
    unsigned int index;
    const struct freak_light2d *light;

    if (!lighting || !map || !lightmap) return;
    tile_count = (unsigned int)map->width * map->height;
    for (y = 0; y < height; ++y)
        for (x = 0; x < width; ++x)
            lightmap[y * width + x] = lighting->ambient;
    for (index = 0; index < FREAK_LIGHT2D_MAX_LIGHTS; ++index) {
        light = &lighting->lights[index];
        if (!light->enabled || light->radius < 1) continue;
        /* Visibility is constant across a tile.  Compute the expensive
           line-of-sight test once per tile, not once per screen pixel. */
        if (tile_count <= FREAK_LIGHT2D_MAX_OCCLUSION_TILES)
            for (tile_y = 0; tile_y < map->height; ++tile_y)
                for (tile_x = 0; tile_x < map->width; ++tile_x) {
                    tile_index = (unsigned int)tile_y * map->width + tile_x;
                    light_visibility[tile_index] = light_visible(map, solid_mask,
                        light->x / map->tile_width, light->y / map->tile_height,
                        tile_x, tile_y);
                }
        left = light->x - light->radius; right = light->x + light->radius;
        top = light->y - light->radius; bottom = light->y + light->radius;
        if (left < 0) left = 0; if (top < 0) top = 0;
        if (right >= width) right = width - 1;
        if (bottom >= height) bottom = height - 1;
        for (y = top; y <= bottom; ++y)
            for (x = left; x <= right; ++x) {
                distance = abs_value(x - light->x) + abs_value(y - light->y);
                tile_x = x / map->tile_width;
                tile_y = y / map->tile_height;
                tile_index = (unsigned int)tile_y * map->width + tile_x;
                if (distance > light->radius ||
                    (tile_count <= FREAK_LIGHT2D_MAX_OCCLUSION_TILES &&
                     !light_visibility[tile_index]))
                    continue;
                strength = (light->intensity * (light->radius - distance)) /
                           light->radius;
                if (light->flicker)
                    strength -= (int)((lighting->ticks + x + y) % light->flicker);
                if (strength > 0 && lightmap[y * width + x] < strength)
                    lightmap[y * width + x] = (uint8_t)strength;
            }
    }
}

void freak_lighting2d_shade(uint8_t *pixels, const uint8_t *lightmap,
                            int width, int height)
{
    int x, y;
    static const uint8_t bayer[16] = {
        0, 8, 2, 10, 12, 4, 14, 6,
        3, 11, 1, 9, 15, 7, 13, 5
    };
    if (!pixels || !lightmap) return;
    for (y = 0; y < height; ++y)
        for (x = 0; x < width; ++x)
            if (bayer[(y & 3) * 4 + (x & 3)] * 16 >
                lightmap[y * width + x])
                pixels[y * width + x] = 0;
}

void freak_lighting2d_colour_glow(const struct freak_lighting2d *lighting,
                                  uint8_t *pixels, int width, int height)
{
    unsigned int index;
    int x, y;
    const struct freak_light2d *light;
    if (!lighting || !pixels) return;
    for (index = 0; index < FREAK_LIGHT2D_MAX_LIGHTS; ++index) {
        light = &lighting->lights[index];
        if (!light->enabled) continue;
        for (y = -2; y <= 2; ++y)
            for (x = -2; x <= 2; ++x)
                if (light->x + x >= 0 && light->x + x < width &&
                    light->y + y >= 0 && light->y + y < height &&
                    x * x + y * y < 7)
                    pixels[(light->y + y) * width + light->x + x] = light->colour;
    }
}
