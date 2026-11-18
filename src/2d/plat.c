#include "plat.h"

static uint8_t collision_at(const struct freak_tilemap *map, int x, int y)
{
    if (x < 0 || y < 0 || x >= map->width * map->tile_width ||
        y >= map->height * map->tile_height)
        return FREAK_TILE_COLLISION_SOLID;
    return freak_tilemap_collision(map, (uint16_t)(x / map->tile_width),
                                   (uint16_t)(y / map->tile_height));
}

static void step_once(struct freak_platformer_body *body,
                      const struct freak_tilemap *map,
                      const struct freak_platformer_rules *rules,
                      int horizontal_input, int vertical_input,
                      unsigned char jump,
                      unsigned char drop)
{
    int x;
    int y;
    int next_x;
    int next_y;
    int edge;
    int sample;
    int top;
    int bottom;
    int center_x;
    int surface;
    int tile_left;
    int local_x;
    int slope_y;
    int on_slope;
    fixed next_x_fixed;
    fixed next_y_fixed;
    uint8_t collision;
    uint8_t below_collision;

    on_slope = 0;

    center_x = fixed_to_int(body->x) + body->width / 2;
    collision = collision_at(map, center_x,
                             fixed_to_int(body->y) + body->height / 2);
    below_collision = collision_at(map, center_x,
                                   fixed_to_int(body->y) + body->height);
    if (((collision & FREAK_TILE_COLLISION_LADDER) ||
         (vertical_input > 0 &&
          (below_collision & FREAK_TILE_COLLISION_LADDER))) &&
        vertical_input != 0)
        body->climbing = 1;
    if (body->climbing && !(collision & FREAK_TILE_COLLISION_LADDER) &&
        !(below_collision & FREAK_TILE_COLLISION_LADDER))
        body->climbing = 0;
    if (body->drop_ticks)
        --body->drop_ticks;
    if (drop && body->grounded &&
        !(below_collision & FREAK_TILE_COLLISION_LADDER) &&
        (below_collision & FREAK_TILE_COLLISION_ONE_WAY)) {
        body->drop_ticks = 8;
        body->grounded = 0;
    }

    if (jump)
        body->jump_buffer_ticks = 6;
    if (body->grounded)
        body->coyote_ticks = 6;
    else if (body->coyote_ticks)
        --body->coyote_ticks;
    body->vx = fixed_mul(rules->run_speed, int_to_fixed(horizontal_input));
    if (body->jump_buffer_ticks && (body->grounded || body->coyote_ticks)) {
        body->vy = -rules->jump_speed;
        body->grounded = 0;
        body->coyote_ticks = 0;
        body->jump_buffer_ticks = 0;
    } else if (body->jump_buffer_ticks) {
        --body->jump_buffer_ticks;
    }
    if (body->climbing) {
        body->vy = fixed_mul(rules->climb_speed, int_to_fixed(vertical_input));
        body->grounded = 0;
    } else {
        body->vy += rules->gravity;
        if (body->vy > rules->max_fall_speed)
            body->vy = rules->max_fall_speed;
    }

    x = fixed_to_int(body->x);
    y = fixed_to_int(body->y);
    next_x_fixed = body->x + body->vx;
    next_x = fixed_to_int(next_x_fixed);
    if (body->vx > 0) {
        edge = next_x + body->width - 1;
        for (sample = y + 1; sample < y + body->height; sample += map->tile_height) {
            collision = collision_at(map, edge, sample);
            if (collision & FREAK_TILE_COLLISION_SOLID) {
                next_x = (edge / map->tile_width) * map->tile_width - body->width;
                body->vx = 0;
                break;
            }
        }
    } else if (body->vx < 0) {
        edge = next_x;
        for (sample = y + 1; sample < y + body->height; sample += map->tile_height) {
            collision = collision_at(map, edge, sample);
            if (collision & FREAK_TILE_COLLISION_SOLID) {
                next_x = (edge / map->tile_width + 1) * map->tile_width;
                body->vx = 0;
                break;
            }
        }
    }
    if (body->vx == 0)
        body->x = int_to_fixed(next_x);
    else
        body->x = next_x_fixed;

    /* A slope must also lift a player walking onto its rising side; waiting
       for vertical gravity alone would let the player pass through it. */
    if (body->grounded && body->vx != 0) {
        center_x = next_x + body->width / 2;
        for (sample = y - map->tile_height;
             sample <= y + body->height;
             sample += map->tile_height) {
            collision = collision_at(map, center_x, sample);
            if (!(collision & (FREAK_TILE_COLLISION_SLOPE_UP_RIGHT |
                               FREAK_TILE_COLLISION_SLOPE_UP_LEFT)))
                continue;
            top = (sample / map->tile_height) * map->tile_height;
            tile_left = (center_x / map->tile_width) * map->tile_width;
            local_x = center_x - tile_left;
            if (collision & FREAK_TILE_COLLISION_SLOPE_UP_RIGHT)
                surface = top + map->tile_height - 1 - local_x;
            else
                surface = top + local_x;
            slope_y = y + body->height - 1;
            if (slope_y >= surface &&
                slope_y <= surface + map->tile_height) {
                y = surface - body->height;
                body->y = int_to_fixed(y);
                body->vy = 0;
                on_slope = 1;
                break;
            }
        }
    }

    next_y_fixed = body->y + body->vy;
    next_y = fixed_to_int(next_y_fixed);
    body->grounded = 0;
    if (body->vy > 0) {
        edge = next_y + body->height - 1;
        for (sample = next_x + 1; sample < next_x + body->width;
             sample += body->width > 2 ? body->width - 2 : 1) {
            collision = collision_at(map, sample, edge);
            top = (edge / map->tile_height) * map->tile_height;
            bottom = y + body->height - 1;
            if (collision & (FREAK_TILE_COLLISION_SLOPE_UP_RIGHT |
                             FREAK_TILE_COLLISION_SLOPE_UP_LEFT)) {
                tile_left = (sample / map->tile_width) * map->tile_width;
                local_x = sample - tile_left;
                if (collision & FREAK_TILE_COLLISION_SLOPE_UP_RIGHT)
                    surface = top + map->tile_height - 1 - local_x;
                else
                    surface = top + local_x;
                if (bottom <= surface && edge >= surface) {
                    next_y = surface - body->height;
                    body->vy = 0;
                    body->grounded = 1;
                    body->coyote_ticks = 6;
                    break;
                }
            }
            if ((collision & FREAK_TILE_COLLISION_SOLID) ||
                ((collision & FREAK_TILE_COLLISION_ONE_WAY) &&
                 !body->climbing &&
                 body->drop_ticks == 0 && bottom <= top)) {
                next_y = top - body->height;
                body->vy = 0;
                body->grounded = 1;
                body->coyote_ticks = 6;
                break;
            }
        }
    } else if (body->vy < 0) {
        edge = next_y;
        for (sample = next_x + 1; sample < next_x + body->width;
             sample += body->width > 2 ? body->width - 2 : 1) {
            collision = collision_at(map, sample, edge);
            if (collision & FREAK_TILE_COLLISION_SOLID) {
                next_y = (edge / map->tile_height + 1) * map->tile_height;
                body->vy = 0;
                break;
            }
        }
    }
    if (body->vy == 0)
        body->y = int_to_fixed(next_y);
    else
        body->y = next_y_fixed;
    if (on_slope) {
        body->grounded = 1;
        body->coyote_ticks = 6;
    }
}

void freak_platformer_body_init(struct freak_platformer_body *body,
                                int x, int y, int width, int height)
{
    if (body == 0)
        return;
    body->x = int_to_fixed(x);
    body->y = int_to_fixed(y);
    body->vx = 0;
    body->vy = 0;
    body->width = width;
    body->height = height;
    body->grounded = 0;
    body->drop_ticks = 0;
    body->jump_buffer_ticks = 0;
    body->coyote_ticks = 0;
    body->climbing = 0;
    body->moving_platform = -1;
    body->moving_platform_offset_x = 0;
}

void freak_platformer_step(struct freak_platformer_body *body,
                           const struct freak_tilemap *map,
                           const struct freak_platformer_rules *rules,
                           int horizontal_input, int vertical_input,
                           unsigned char jump,
                           unsigned char drop,
                           unsigned long elapsed_ticks)
{
    unsigned long tick;

    if (body == 0 || map == 0 || rules == 0)
        return;
    if (horizontal_input < -1) horizontal_input = -1;
    if (horizontal_input > 1) horizontal_input = 1;
    if (vertical_input < -1) vertical_input = -1;
    if (vertical_input > 1) vertical_input = 1;
    if (elapsed_ticks > 8)
        elapsed_ticks = 8;
    for (tick = 0; tick < elapsed_ticks; ++tick)
        step_once(body, map, rules, horizontal_input, vertical_input,
                  jump && tick == 0, drop && tick == 0);
}

void freak_moving_platform_init(struct freak_moving_platform *platform,
                                int x, int y, int width, int height,
                                int min_x, int max_x, int min_y, int max_y,
                                fixed vx, fixed vy)
{
    if (platform == 0)
        return;
    platform->x = int_to_fixed(x);
    platform->y = int_to_fixed(y);
    platform->vx = vx;
    platform->vy = vy;
    platform->min_x = int_to_fixed(min_x);
    platform->max_x = int_to_fixed(max_x);
    platform->min_y = int_to_fixed(min_y);
    platform->max_y = int_to_fixed(max_y);
    platform->last_dx = 0;
    platform->last_dy = 0;
    platform->width = width;
    platform->height = height;
}

void freak_moving_platform_step(struct freak_moving_platform *platform,
                                unsigned long elapsed_ticks)
{
    unsigned long tick;
    fixed old_x;
    fixed old_y;

    if (platform == 0)
        return;
    if (elapsed_ticks > 8) elapsed_ticks = 8;
    old_x = platform->x;
    old_y = platform->y;
    for (tick = 0; tick < elapsed_ticks; ++tick) {
        platform->x += platform->vx;
        platform->y += platform->vy;
        if (platform->x < platform->min_x || platform->x > platform->max_x) {
            platform->vx = -platform->vx;
            platform->x += platform->vx * 2;
        }
        if (platform->y < platform->min_y || platform->y > platform->max_y) {
            platform->vy = -platform->vy;
            platform->y += platform->vy * 2;
        }
    }
    /* Carry riders by the actual displacement.  This matters at an endpoint,
       where the position changes in the opposite direction to the old speed. */
    platform->last_dx = platform->x - old_x;
    platform->last_dy = platform->y - old_y;
}

void freak_platformer_resolve_moving_platforms(
    struct freak_platformer_body *body,
    const struct freak_moving_platform *platforms, unsigned int count)
{
    unsigned int index;
    int attached;
    int platform_x;
    int platform_y;
    int body_x;
    int body_y;
    int new_bottom;

    if (body == 0 || platforms == 0)
        return;
    attached = body->moving_platform;
    if (attached >= 0 && attached < (int)count && body->vy >= 0) {
        /* A rider is defined in platform-local coordinates.  Reconstructing
           world X from that offset eliminates accumulated carry drift. */
        if (body->vx != 0)
            body->moving_platform_offset_x = body->x +
                platforms[attached].last_dx - platforms[attached].x;
        body->x = platforms[attached].x + body->moving_platform_offset_x;
        body->y = platforms[attached].y - int_to_fixed(body->height);
        body_x = fixed_to_int(body->x);
        platform_x = fixed_to_int(platforms[attached].x);
        platform_y = fixed_to_int(platforms[attached].y);
        if (body_x + body->width > platform_x - 2 &&
            body_x < platform_x + platforms[attached].width + 2) {
            body->y = int_to_fixed(platform_y - body->height);
            body->vy = 0;
            body->grounded = 1;
            body->coyote_ticks = 6;
            return;
        }
    }
    body->moving_platform = -1;
    body_x = fixed_to_int(body->x);
    body_y = fixed_to_int(body->y);
    new_bottom = body_y + body->height - 1;
    for (index = 0; index < count; ++index) {
        platform_x = fixed_to_int(platforms[index].x);
        platform_y = fixed_to_int(platforms[index].y);
        if (body_x + body->width <= platform_x || body_x >= platform_x + platforms[index].width)
            continue;
        if (body->vy >= 0 &&
            new_bottom >= platform_y - 2 &&
            body_y < platform_y + platforms[index].height) {
            body->y = int_to_fixed(platform_y - body->height);
            body->vy = 0;
            body->grounded = 1;
            body->coyote_ticks = 6;
            body->moving_platform = (int)index;
            body->moving_platform_offset_x = body->x - platforms[index].x;
            break;
        }
    }
}
