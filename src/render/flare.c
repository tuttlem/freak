#include "flare.h"

#include "draw.h"

static void flare_disc(uint8_t *buffer, int cx, int cy, int radius,
                       uint8_t colour, uint8_t intensity)
{
    int x, y;
    int distance;
    if (radius < 1) return;
    for (y = -radius; y <= radius; ++y)
        for (x = -radius; x <= radius; ++x) {
            distance = x * x + y * y;
            if (distance <= radius * radius &&
                ((distance + x * 3 + y * 5) & 31) < intensity / 8)
                freak_draw_pixel(buffer, cx + x, cy + y, colour);
        }
}

void freak_lens_flare_draw(uint8_t *buffer, const struct freak_lens_flare *flare,
                           int screen_center_x, int screen_center_y)
{
    int dx;
    int dy;
    int ghost_x;
    int ghost_y;
    int index;
    uint8_t intensity;

    if (!buffer || !flare || flare->occluded) return;
    intensity = flare->intensity;
    flare_disc(buffer, flare->source_x, flare->source_y, flare->radius,
               flare->colour, intensity);
    freak_draw_line(buffer, flare->source_x - flare->radius * 3, flare->source_y,
                    flare->source_x + flare->radius * 3, flare->source_y,
                    flare->colour);
    dx = screen_center_x - flare->source_x;
    dy = screen_center_y - flare->source_y;
    for (index = 1; index <= 4; ++index) {
        ghost_x = flare->source_x + dx * index / 5;
        ghost_y = flare->source_y + dy * index / 5;
        flare_disc(buffer, ghost_x, ghost_y,
                   flare->radius / (index + 1) + 2,
                   (uint8_t)(flare->colour - (index & 1 ? 0 : 4)),
                   (uint8_t)(intensity / 2));
    }
}
