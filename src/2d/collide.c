#include "collide.h"

int freak_collision_rects(const struct freak_rect *a,
                          const struct freak_rect *b)
{
    if (a == 0 || b == 0)
        return 0;
    return a->x < b->x + b->width && a->x + a->width > b->x &&
           a->y < b->y + b->height && a->y + a->height > b->y;
}

int freak_collision_point(const struct freak_rect *rect, int x, int y)
{
    return rect != 0 && x >= rect->x && x < rect->x + rect->width &&
           y >= rect->y && y < rect->y + rect->height;
}

int freak_collision_circles(int ax, int ay, int ar, int bx, int by, int br)
{
    long dx = (long)bx - ax;
    long dy = (long)by - ay;
    long radius = (long)ar + br;
    return dx * dx + dy * dy < radius * radius;
}

int freak_collision_colliders(const struct freak_collider *a,
                              const struct freak_collider *b)
{
    if (a == 0 || b == 0 || !a->enabled || !b->enabled)
        return 0;
    if (!(a->mask & b->layer) || !(b->mask & a->layer))
        return 0;
    return freak_collision_rects(&a->bounds, &b->bounds);
}

uint8_t freak_collision_tile_flags(const struct freak_tilemap *map, int x, int y)
{
    if (map == 0 || x < 0 || y < 0 || x >= map->width * map->tile_width ||
        y >= map->height * map->tile_height)
        return FREAK_TILE_COLLISION_SOLID;
    return freak_tilemap_collision(map, (uint16_t)(x / map->tile_width),
                                   (uint16_t)(y / map->tile_height));
}

int freak_collision_tiles(const struct freak_tilemap *map,
                          const struct freak_rect *rect, uint8_t mask)
{
    int left;
    int right;
    int top;
    int bottom;
    int x;
    int y;

    if (map == 0 || rect == 0 || rect->width <= 0 || rect->height <= 0)
        return 0;
    left = rect->x / map->tile_width;
    right = (rect->x + rect->width - 1) / map->tile_width;
    top = rect->y / map->tile_height;
    bottom = (rect->y + rect->height - 1) / map->tile_height;
    for (y = top; y <= bottom; ++y)
        for (x = left; x <= right; ++x) {
            if (x < 0 || y < 0 || x >= map->width || y >= map->height)
                return 1;
            if (freak_tilemap_collision(map, (uint16_t)x, (uint16_t)y) & mask)
                return 1;
        }
    return 0;
}

uint8_t freak_collision_move_x(const struct freak_tilemap *map,
                               struct freak_rect *rect, int dx, uint8_t mask)
{
    int step;

    if (map == 0 || rect == 0)
        return 0;
    step = dx < 0 ? -1 : 1;
    while (dx != 0) {
        rect->x += step;
        if (freak_collision_tiles(map, rect, mask)) {
            rect->x -= step;
            return FREAK_COLLISION_HIT_X;
        }
        dx -= step;
    }
    return 0;
}

uint8_t freak_collision_move_y(const struct freak_tilemap *map,
                               struct freak_rect *rect, int dy, uint8_t mask)
{
    int step;

    if (map == 0 || rect == 0)
        return 0;
    step = dy < 0 ? -1 : 1;
    while (dy != 0) {
        rect->y += step;
        if (freak_collision_tiles(map, rect, mask)) {
            rect->y -= step;
            return FREAK_COLLISION_HIT_Y;
        }
        dy -= step;
    }
    return 0;
}

uint8_t freak_collision_move(const struct freak_tilemap *map,
                             struct freak_rect *rect, int dx, int dy,
                             uint8_t mask)
{
    return freak_collision_move_x(map, rect, dx, mask) |
           freak_collision_move_y(map, rect, dy, mask);
}

uint8_t freak_collision_sweep_tiles(const struct freak_tilemap *map,
                                    struct freak_rect *rect, int dx, int dy,
                                    uint8_t mask)
{
    return freak_collision_move(map, rect, dx, dy, mask);
}

int freak_collision_segment_rect(int x0, int y0, int x1, int y1,
                                 const struct freak_rect *rect,
                                 int *hit_x, int *hit_y)
{
    int dx;
    int dy;
    int step_x;
    int step_y;
    int error;
    int twice_error;

    if (rect == 0)
        return 0;
    dx = x1 >= x0 ? x1 - x0 : x0 - x1;
    dy = y1 >= y0 ? y1 - y0 : y0 - y1;
    step_x = x0 < x1 ? 1 : -1;
    step_y = y0 < y1 ? 1 : -1;
    error = dx - dy;
    while (1) {
        if (freak_collision_point(rect, x0, y0)) {
            if (hit_x) *hit_x = x0;
            if (hit_y) *hit_y = y0;
            return 1;
        }
        if (x0 == x1 && y0 == y1)
            break;
        twice_error = error * 2;
        if (twice_error > -dy) {
            error -= dy;
            x0 += step_x;
        }
        if (twice_error < dx) {
            error += dx;
            y0 += step_y;
        }
    }
    return 0;
}

int freak_collision_raycast_tiles(const struct freak_tilemap *map,
                                  int x0, int y0, int x1, int y1,
                                  uint8_t mask, int *hit_x, int *hit_y)
{
    int dx;
    int dy;
    int step_x;
    int step_y;
    int error;
    int twice_error;

    if (map == 0)
        return 0;
    dx = x1 >= x0 ? x1 - x0 : x0 - x1;
    dy = y1 >= y0 ? y1 - y0 : y0 - y1;
    step_x = x0 < x1 ? 1 : -1;
    step_y = y0 < y1 ? 1 : -1;
    error = dx - dy;
    while (1) {
        if (freak_collision_tile_flags(map, x0, y0) & mask) {
            if (hit_x) *hit_x = x0;
            if (hit_y) *hit_y = y0;
            return 1;
        }
        if (x0 == x1 && y0 == y1)
            break;
        twice_error = error * 2;
        if (twice_error > -dy) {
            error -= dy;
            x0 += step_x;
        }
        if (twice_error < dx) {
            error += dx;
            y0 += step_y;
        }
    }
    return 0;
}

void freak_collision_grid_init(struct freak_collision_grid *grid,
                               int x, int y, int width, int height,
                               int cell_width, int cell_height)
{
    int columns;
    int rows;

    if (grid == 0)
        return;
    columns = cell_width > 0 ? (width + cell_width - 1) / cell_width : 0;
    rows = cell_height > 0 ? (height + cell_height - 1) / cell_height : 0;
    while (columns > 0 && rows > 0 && columns * rows > FREAK_COLLISION_GRID_MAX_CELLS)
        --rows;
    grid->x = x;
    grid->y = y;
    grid->cell_width = cell_width;
    grid->cell_height = cell_height;
    grid->columns = columns;
    grid->rows = rows;
    freak_collision_grid_clear(grid);
}

void freak_collision_grid_clear(struct freak_collision_grid *grid)
{
    unsigned int index;

    if (grid == 0)
        return;
    for (index = 0; index < FREAK_COLLISION_GRID_MAX_CELLS; ++index)
        grid->counts[index] = 0;
}

void freak_collision_grid_insert(struct freak_collision_grid *grid,
                                 uint16_t item, const struct freak_rect *rect)
{
    int left;
    int right;
    int top;
    int bottom;
    int x;
    int y;
    unsigned int index;

    if (grid == 0 || rect == 0 || grid->columns <= 0 || grid->rows <= 0)
        return;
    left = (rect->x - grid->x) / grid->cell_width;
    right = (rect->x + rect->width - 1 - grid->x) / grid->cell_width;
    top = (rect->y - grid->y) / grid->cell_height;
    bottom = (rect->y + rect->height - 1 - grid->y) / grid->cell_height;
    if (left < 0) left = 0;
    if (top < 0) top = 0;
    if (right >= grid->columns) right = grid->columns - 1;
    if (bottom >= grid->rows) bottom = grid->rows - 1;
    for (y = top; y <= bottom; ++y)
        for (x = left; x <= right; ++x) {
            index = (unsigned int)y * grid->columns + x;
            if (grid->counts[index] < FREAK_COLLISION_GRID_CELL_ITEMS)
                grid->items[index][grid->counts[index]++] = item;
        }
}

unsigned int freak_collision_grid_query(const struct freak_collision_grid *grid,
                                        const struct freak_rect *rect,
                                        uint16_t *items, unsigned int capacity)
{
    int left;
    int right;
    int top;
    int bottom;
    int x;
    int y;
    unsigned int index;
    unsigned int item_index;
    unsigned int found;
    unsigned int check;
    uint16_t item;

    if (grid == 0 || rect == 0 || items == 0 || grid->columns <= 0 || grid->rows <= 0)
        return 0;
    left = (rect->x - grid->x) / grid->cell_width;
    right = (rect->x + rect->width - 1 - grid->x) / grid->cell_width;
    top = (rect->y - grid->y) / grid->cell_height;
    bottom = (rect->y + rect->height - 1 - grid->y) / grid->cell_height;
    if (left < 0) left = 0;
    if (top < 0) top = 0;
    if (right >= grid->columns) right = grid->columns - 1;
    if (bottom >= grid->rows) bottom = grid->rows - 1;
    found = 0;
    for (y = top; y <= bottom; ++y)
        for (x = left; x <= right; ++x) {
            index = (unsigned int)y * grid->columns + x;
            for (item_index = 0; item_index < grid->counts[index]; ++item_index) {
                item = grid->items[index][item_index];
                for (check = 0; check < found; ++check)
                    if (items[check] == item)
                        break;
                if (check == found && found < capacity)
                    items[found++] = item;
            }
        }
    return found;
}

static int contains_pair(const unsigned long *pairs, unsigned int count,
                         unsigned long pair)
{
    unsigned int index;

    for (index = 0; index < count; ++index)
        if (pairs[index] == pair)
            return 1;
    return 0;
}

unsigned int freak_collision_contacts_update(
    struct freak_collision_contacts *contacts,
    const struct freak_collider *colliders, unsigned int collider_count,
    struct freak_collision_event *events, unsigned int event_capacity)
{
    unsigned long current[FREAK_COLLISION_MAX_CONTACTS];
    unsigned int current_count;
    unsigned int event_count;
    unsigned int first;
    unsigned int second;
    unsigned int index;
    unsigned int low;
    unsigned int high;
    unsigned long pair;
    uint8_t type;

    if (contacts == 0 || colliders == 0)
        return 0;
    current_count = 0;
    event_count = 0;
    for (first = 0; first < collider_count; ++first)
        for (second = first + 1; second < collider_count; ++second)
            if (freak_collision_colliders(&colliders[first], &colliders[second])) {
                low = colliders[first].id < colliders[second].id ?
                      colliders[first].id : colliders[second].id;
                high = colliders[first].id < colliders[second].id ?
                       colliders[second].id : colliders[first].id;
                pair = ((unsigned long)low << 16) | high;
                if (current_count < FREAK_COLLISION_MAX_CONTACTS)
                    current[current_count++] = pair;
                type = contains_pair(contacts->pairs, contacts->count, pair) ?
                       FREAK_COLLISION_EVENT_STAY : FREAK_COLLISION_EVENT_ENTER;
                if (events && event_count < event_capacity) {
                    events[event_count].first_id = low;
                    events[event_count].second_id = high;
                    events[event_count].type = type;
                    ++event_count;
                }
            }
    for (index = 0; index < contacts->count; ++index)
        if (!contains_pair(current, current_count, contacts->pairs[index]) &&
            events && event_count < event_capacity) {
            events[event_count].first_id = (unsigned int)(contacts->pairs[index] >> 16);
            events[event_count].second_id = (unsigned int)(contacts->pairs[index] & 0xffffU);
            events[event_count].type = FREAK_COLLISION_EVENT_EXIT;
            ++event_count;
        }
    contacts->count = current_count;
    for (index = 0; index < current_count; ++index)
        contacts->pairs[index] = current[index];
    return event_count;
}
