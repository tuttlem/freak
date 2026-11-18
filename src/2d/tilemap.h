#ifndef __freak_tilemap_h_
#define __freak_tilemap_h_

#include <stdint.h>

#include "camera.h"
#include "../render/sprite.h"

#define FREAK_TILEMAP_MAX_LAYERS 4
#define FREAK_TILE_EMPTY 0xffffU
#define FREAK_TILE_COLLISION_SOLID   0x01
#define FREAK_TILE_COLLISION_ONE_WAY 0x02
#define FREAK_TILE_COLLISION_SLOPE_UP_RIGHT 0x04
#define FREAK_TILE_COLLISION_SLOPE_UP_LEFT  0x08
#define FREAK_TILE_COLLISION_LADDER         0x10

struct freak_tile_atlas {
    const struct freak_sprite *tiles;
    unsigned int count;
};

struct freak_tilemap {
    uint16_t width;
    uint16_t height;
    uint16_t tile_width;
    uint16_t tile_height;
    uint8_t layer_count;
    uint16_t *layers[FREAK_TILEMAP_MAX_LAYERS];
    uint8_t *collision;
};

int freak_tilemap_init(struct freak_tilemap *map, uint16_t width,
                       uint16_t height, uint16_t tile_width,
                       uint16_t tile_height, uint8_t layer_count,
                       uint8_t with_collision);
void freak_tilemap_destroy(struct freak_tilemap *map);
void freak_tilemap_set(struct freak_tilemap *map, uint8_t layer,
                       uint16_t x, uint16_t y, uint16_t tile);
uint16_t freak_tilemap_get(const struct freak_tilemap *map, uint8_t layer,
                           uint16_t x, uint16_t y);
void freak_tilemap_set_collision(struct freak_tilemap *map, uint16_t x,
                                 uint16_t y, uint8_t flags);
uint8_t freak_tilemap_collision(const struct freak_tilemap *map, uint16_t x,
                                uint16_t y);
void freak_tilemap_draw(uint8_t *buffer, const struct freak_camera_2d *camera,
                        const struct freak_tilemap *map,
                        const struct freak_tile_atlas *atlas, uint8_t layer);

/* Native FMP1 format: a compact little-endian grid with optional collision. */
int freak_tilemap_save(const char *filename, const struct freak_tilemap *map);
int freak_tilemap_load(const char *filename, struct freak_tilemap *map);

#endif
