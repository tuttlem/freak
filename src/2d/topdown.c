#include "topdown.h"

static unsigned short path_cost[FREAK_TOPDOWN_MAX_TILES];
static short path_parent[FREAK_TOPDOWN_MAX_TILES];
static unsigned char path_state[FREAK_TOPDOWN_MAX_TILES];

static int abs_value(int value)
{
    return value < 0 ? -value : value;
}

static int tile_blocked(const struct freak_tilemap *map, int x, int y,
                        uint8_t solid_mask)
{
    if (x < 0 || y < 0 || x >= map->width || y >= map->height)
        return 1;
    return (freak_tilemap_collision(map, (uint16_t)x, (uint16_t)y) &
            solid_mask) != 0;
}

int freak_topdown_pathfind(const struct freak_tilemap *map,
                           int start_x, int start_y, int goal_x, int goal_y,
                           uint8_t solid_mask, struct freak_topdown_path *path)
{
    unsigned int tile_count;
    unsigned int index;
    unsigned int current;
    unsigned int best;
    unsigned int neighbour;
    unsigned int best_score;
    unsigned int score;
    unsigned int current_x;
    unsigned int current_y;
    int next_x;
    int next_y;
    int direction;
    unsigned int reverse_count;

    if (map == 0 || path == 0)
        return 0;
    tile_count = (unsigned int)map->width * map->height;
    if (tile_count == 0 || tile_count > FREAK_TOPDOWN_MAX_TILES ||
        tile_blocked(map, start_x, start_y, solid_mask) ||
        tile_blocked(map, goal_x, goal_y, solid_mask))
        return 0;
    for (index = 0; index < tile_count; ++index) {
        path_cost[index] = 0xffffU;
        path_parent[index] = -1;
        path_state[index] = 0;
    }
    current = (unsigned int)start_y * map->width + start_x;
    path_cost[current] = 0;
    path_state[current] = 1;
    while (1) {
        best = tile_count;
        best_score = 0xffffU;
        for (index = 0; index < tile_count; ++index)
            if (path_state[index] == 1) {
                score = path_cost[index] + (unsigned int)(
                    abs_value((int)(index % map->width) - goal_x) +
                    abs_value((int)(index / map->width) - goal_y));
                if (score < best_score) {
                    best_score = score;
                    best = index;
                }
            }
        if (best == tile_count)
            return 0;
        current = best;
        if (current == (unsigned int)goal_y * map->width + goal_x)
            break;
        path_state[current] = 2;
        current_x = current % map->width;
        current_y = current / map->width;
        for (direction = 0; direction < 4; ++direction) {
            next_x = (int)current_x;
            next_y = (int)current_y;
            if (direction == 0) --next_x;
            else if (direction == 1) ++next_x;
            else if (direction == 2) --next_y;
            else ++next_y;
            if (tile_blocked(map, next_x, next_y, solid_mask))
                continue;
            neighbour = (unsigned int)next_y * map->width + next_x;
            if (path_state[neighbour] == 2)
                continue;
            if (path_state[neighbour] == 0 ||
                path_cost[neighbour] > path_cost[current] + 1) {
                path_cost[neighbour] = path_cost[current] + 1;
                path_parent[neighbour] = (short)current;
                path_state[neighbour] = 1;
            }
        }
    }
    reverse_count = 0;
    while (current != (unsigned int)start_y * map->width + start_x &&
           reverse_count < FREAK_TOPDOWN_MAX_PATH) {
        path->x[reverse_count] = (uint16_t)(current % map->width);
        path->y[reverse_count] = (uint16_t)(current / map->width);
        ++reverse_count;
        current = (unsigned int)path_parent[current];
    }
    path->count = reverse_count;
    for (index = 0; index < reverse_count / 2; ++index) {
        current_x = path->x[index]; path->x[index] = path->x[reverse_count - 1 - index];
        path->x[reverse_count - 1 - index] = (uint16_t)current_x;
        current_y = path->y[index]; path->y[index] = path->y[reverse_count - 1 - index];
        path->y[reverse_count - 1 - index] = (uint16_t)current_y;
    }
    return 1;
}

static int clear_line(const struct freak_tilemap *map, int x0, int y0,
                      int x1, int y1, uint8_t solid_mask)
{
    int dx = abs_value(x1 - x0);
    int dy = abs_value(y1 - y0);
    int step_x = x0 < x1 ? 1 : -1;
    int step_y = y0 < y1 ? 1 : -1;
    int error = dx - dy;
    int twice_error;

    while (1) {
        if (!(x0 == x1 && y0 == y1) && tile_blocked(map, x0, y0, solid_mask))
            return 0;
        if (x0 == x1 && y0 == y1)
            return 1;
        twice_error = error * 2;
        if (twice_error > -dy) { error -= dy; x0 += step_x; }
        if (twice_error < dx) { error += dx; y0 += step_y; }
    }
}

void freak_topdown_fov(const struct freak_tilemap *map, int origin_x,
                       int origin_y, int radius, uint8_t solid_mask,
                       uint8_t *visible, unsigned int visible_size)
{
    int x;
    int y;
    unsigned int index;
    unsigned int tiles;

    if (map == 0 || visible == 0)
        return;
    tiles = (unsigned int)map->width * map->height;
    if (visible_size < tiles)
        return;
    for (index = 0; index < tiles; ++index)
        visible[index] = 0;
    for (y = origin_y - radius; y <= origin_y + radius; ++y)
        for (x = origin_x - radius; x <= origin_x + radius; ++x)
            if (x >= 0 && y >= 0 && x < map->width && y < map->height &&
                abs_value(x - origin_x) + abs_value(y - origin_y) <= radius &&
                clear_line(map, origin_x, origin_y, x, y, solid_mask))
                visible[(unsigned int)y * map->width + x] = 1;
}

void freak_topdown_door_set(struct freak_tilemap *map,
                            struct freak_topdown_door *door, unsigned char open)
{
    if (map == 0 || door == 0)
        return;
    door->open = open;
    freak_tilemap_set_collision(map, door->x, door->y,
                                open ? 0 : FREAK_TILE_COLLISION_SOLID);
}

int freak_topdown_trigger_test(struct freak_topdown_trigger *trigger,
                               const struct freak_rect *actor)
{
    if (trigger == 0 || actor == 0)
        return 0;
    trigger->active = freak_collision_rects(&trigger->bounds, actor) != 0;
    return trigger->active;
}

uint8_t freak_topdown_move_with_push(const struct freak_tilemap *map,
                                     struct freak_rect *actor,
                                     struct freak_rect *crate, int dx, int dy,
                                     uint8_t solid_mask)
{
    struct freak_rect next;
    struct freak_rect old_crate;
    uint8_t result;

    if (map == 0 || actor == 0)
        return 0;
    next = *actor;
    next.x += dx;
    next.y += dy;
    if (crate && freak_collision_rects(&next, crate)) {
        old_crate = *crate;
        result = freak_collision_sweep_tiles(map, crate, dx, dy, solid_mask);
        if (result || crate->x != old_crate.x + dx || crate->y != old_crate.y + dy) {
            *crate = old_crate;
            return FREAK_COLLISION_HIT_X | FREAK_COLLISION_HIT_Y;
        }
    }
    return freak_collision_sweep_tiles(map, actor, dx, dy, solid_mask);
}
