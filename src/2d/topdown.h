#ifndef __freak_topdown_h_
#define __freak_topdown_h_

#include "collide.h"

#define FREAK_TOPDOWN_MAX_PATH 128
#define FREAK_TOPDOWN_MAX_TILES 1024

struct freak_topdown_path {
    uint16_t x[FREAK_TOPDOWN_MAX_PATH];
    uint16_t y[FREAK_TOPDOWN_MAX_PATH];
    unsigned int count;
};

struct freak_topdown_door {
    uint16_t x;
    uint16_t y;
    unsigned char open;
};

struct freak_topdown_trigger {
    struct freak_rect bounds;
    unsigned char active;
};

int freak_topdown_pathfind(const struct freak_tilemap *map,
                           int start_x, int start_y, int goal_x, int goal_y,
                           uint8_t solid_mask, struct freak_topdown_path *path);
void freak_topdown_fov(const struct freak_tilemap *map, int origin_x,
                       int origin_y, int radius, uint8_t solid_mask,
                       uint8_t *visible, unsigned int visible_size);
void freak_topdown_door_set(struct freak_tilemap *map,
                            struct freak_topdown_door *door, unsigned char open);
int freak_topdown_trigger_test(struct freak_topdown_trigger *trigger,
                               const struct freak_rect *actor);
uint8_t freak_topdown_move_with_push(const struct freak_tilemap *map,
                                     struct freak_rect *actor,
                                     struct freak_rect *crate, int dx, int dy,
                                     uint8_t solid_mask);

#endif
