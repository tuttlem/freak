#ifndef __freak_platformer_h_
#define __freak_platformer_h_

#include "../core/fixed.h"
#include "tilemap.h"

struct freak_platformer_body {
    fixed x;
    fixed y;
    fixed vx;
    fixed vy;
    int width;
    int height;
    unsigned char grounded;
    unsigned char drop_ticks;
    unsigned char jump_buffer_ticks;
    unsigned char coyote_ticks;
    unsigned char climbing;
    int moving_platform;
    fixed moving_platform_offset_x;
};

struct freak_platformer_rules {
    fixed run_speed;
    fixed gravity;
    fixed jump_speed;
    fixed max_fall_speed;
    fixed climb_speed;
};

struct freak_moving_platform {
    fixed x;
    fixed y;
    fixed vx;
    fixed vy;
    fixed min_x;
    fixed max_x;
    fixed min_y;
    fixed max_y;
    fixed last_dx;
    fixed last_dy;
    int width;
    int height;
};

void freak_platformer_body_init(struct freak_platformer_body *body,
                                int x, int y, int width, int height);
void freak_platformer_step(struct freak_platformer_body *body,
                           const struct freak_tilemap *map,
                           const struct freak_platformer_rules *rules,
                           int horizontal_input, int vertical_input,
                           unsigned char jump,
                           unsigned char drop,
                           unsigned long elapsed_ticks);
void freak_moving_platform_init(struct freak_moving_platform *platform,
                                int x, int y, int width, int height,
                                int min_x, int max_x, int min_y, int max_y,
                                fixed vx, fixed vy);
void freak_moving_platform_step(struct freak_moving_platform *platform,
                                unsigned long elapsed_ticks);
void freak_platformer_resolve_moving_platforms(
    struct freak_platformer_body *body,
    const struct freak_moving_platform *platforms, unsigned int count);

#endif
