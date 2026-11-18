#ifndef __freak_collision_h_
#define __freak_collision_h_

#include <stdint.h>

#include "tilemap.h"

#define FREAK_COLLISION_HIT_X 0x01
#define FREAK_COLLISION_HIT_Y 0x02
#define FREAK_COLLISION_EVENT_ENTER 1
#define FREAK_COLLISION_EVENT_STAY  2
#define FREAK_COLLISION_EVENT_EXIT  3
#define FREAK_COLLISION_GRID_MAX_CELLS 128
#define FREAK_COLLISION_GRID_CELL_ITEMS 16
#define FREAK_COLLISION_MAX_CONTACTS 64

struct freak_rect {
    int x;
    int y;
    int width;
    int height;
};

struct freak_collider {
    struct freak_rect bounds;
    uint16_t layer;
    uint16_t mask;
    unsigned int id;
    unsigned char enabled;
};

struct freak_collision_event {
    unsigned int first_id;
    unsigned int second_id;
    uint8_t type;
};

struct freak_collision_contacts {
    unsigned long pairs[FREAK_COLLISION_MAX_CONTACTS];
    unsigned int count;
};

struct freak_collision_grid {
    int x;
    int y;
    int cell_width;
    int cell_height;
    int columns;
    int rows;
    uint8_t counts[FREAK_COLLISION_GRID_MAX_CELLS];
    uint16_t items[FREAK_COLLISION_GRID_MAX_CELLS]
                  [FREAK_COLLISION_GRID_CELL_ITEMS];
};

/* Rectangles touch only when their interiors overlap; shared edges are clear. */
int freak_collision_rects(const struct freak_rect *a,
                          const struct freak_rect *b);
int freak_collision_point(const struct freak_rect *rect, int x, int y);
int freak_collision_circles(int ax, int ay, int ar, int bx, int by, int br);
int freak_collision_colliders(const struct freak_collider *a,
                              const struct freak_collider *b);

/* Query world pixels against a tile map's collision flags. */
uint8_t freak_collision_tile_flags(const struct freak_tilemap *map, int x, int y);
int freak_collision_tiles(const struct freak_tilemap *map,
                          const struct freak_rect *rect, uint8_t mask);

/* Axis-separated tile movement.  The rectangle is left at the nearest clear
   pixel and the return value contains FREAK_COLLISION_HIT_X/Y as appropriate. */
uint8_t freak_collision_move_x(const struct freak_tilemap *map,
                               struct freak_rect *rect, int dx, uint8_t mask);
uint8_t freak_collision_move_y(const struct freak_tilemap *map,
                               struct freak_rect *rect, int dy, uint8_t mask);
uint8_t freak_collision_move(const struct freak_tilemap *map,
                             struct freak_rect *rect, int dx, int dy,
                             uint8_t mask);

/* Pixel-swept tile movement: no object can skip a one-tile-wide wall. */
uint8_t freak_collision_sweep_tiles(const struct freak_tilemap *map,
                                    struct freak_rect *rect, int dx, int dy,
                                    uint8_t mask);
int freak_collision_segment_rect(int x0, int y0, int x1, int y1,
                                 const struct freak_rect *rect,
                                 int *hit_x, int *hit_y);
int freak_collision_raycast_tiles(const struct freak_tilemap *map,
                                  int x0, int y0, int x1, int y1,
                                  uint8_t mask, int *hit_x, int *hit_y);

void freak_collision_grid_init(struct freak_collision_grid *grid,
                               int x, int y, int width, int height,
                               int cell_width, int cell_height);
void freak_collision_grid_clear(struct freak_collision_grid *grid);
void freak_collision_grid_insert(struct freak_collision_grid *grid,
                                 uint16_t item, const struct freak_rect *rect);
unsigned int freak_collision_grid_query(const struct freak_collision_grid *grid,
                                        const struct freak_rect *rect,
                                        uint16_t *items, unsigned int capacity);

unsigned int freak_collision_contacts_update(
    struct freak_collision_contacts *contacts,
    const struct freak_collider *colliders, unsigned int collider_count,
    struct freak_collision_event *events, unsigned int event_capacity);

#endif
