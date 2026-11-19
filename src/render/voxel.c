#include "voxel.h"

#include "draw.h"

void freak_voxel_heightfield(uint8_t *output, const uint8_t *heights,
                             const struct freak_voxel_view *view,
                             uint8_t colour_base, uint8_t colour_range)
{
    int screen_x, depth, world_x, world_y, terrain, projected, highest;
    int shade;

    if (!output || !heights || !view || colour_range == 0) return;
    for (screen_x = 0; screen_x < FREAK_SCREEN_WIDTH; ++screen_x) {
        highest = FREAK_SCREEN_HEIGHT;
        for (depth = 1; depth < view->distance && highest > 0; depth += 2) {
            world_x = (view->x + (screen_x - 160) * depth / 180) & 255;
            world_y = (view->y + depth) & 255;
            terrain = heights[(world_y << 8) + world_x];
            projected = view->horizon - (terrain - view->height) * 90 / depth;
            if (projected < 0) projected = 0;
            if (projected < highest) {
                shade = terrain * colour_range / 256 + depth * colour_range / (view->distance * 2);
                if (shade >= colour_range) shade = colour_range - 1;
                freak_draw_line(output, screen_x, projected, screen_x, highest - 1,
                                (uint8_t)(colour_base + shade));
                highest = projected;
            }
        }
    }
}
