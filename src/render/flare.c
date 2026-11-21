#include "flare.h"

#include "blend.h"
#include "draw.h"

static void flare_pixel(uint8_t *buffer, int x, int y, uint8_t colour,
                        const uint8_t *blend_table,
                        const uint8_t *soften_table, int softened)
{
    int offset;
    if (x < 0 || x >= FREAK_SCREEN_WIDTH || y < 0 || y >= FREAK_SCREEN_HEIGHT)
        return;
    offset = y * FREAK_SCREEN_WIDTH + x;
    buffer[offset] = softened && soften_table ?
        freak_blend_pixel(soften_table, buffer[offset], colour) : blend_table ?
        freak_blend_pixel(blend_table, buffer[offset], colour) : colour;
}

static void flare_disc(uint8_t *buffer, int cx, int cy, int radius,
                       uint8_t colour, uint8_t intensity,
                       const uint8_t *blend_table, const uint8_t *soften_table)
{
    int x, y;
    int distance;
    int shade;
    int colour_index;
    int opacity;
    int radius_squared;
    if (radius < 1) return;
    radius_squared = radius * radius;
    for (y = -radius; y <= radius; ++y)
        for (x = -radius; x <= radius; ++x) {
            distance = x * x + y * y;
            if (distance <= radius_squared) {
                /* A radial coverage mask acts as alpha.  The stable ordered
                   dither makes the dim outer edge blend into the scene. */
                opacity = ((radius_squared - distance) * intensity) /
                          (radius_squared + 1);
                if (((x * 17 + y * 29 + 127) & 255) >= opacity)
                    continue;
                /* The palette is ordered dark-to-bright.  Stepping inward
                   through it gives a soft coloured halo on indexed VGA. */
                shade = (distance * 22) / (radius_squared + 1);
                colour_index = (int)colour - shade;
                if (colour_index < 0) colour_index = 0;
                flare_pixel(buffer, cx + x, cy + y, (uint8_t)colour_index,
                            blend_table, soften_table, opacity < 112);
            }
        }
}

static void flare_streak(uint8_t *buffer, int cx, int cy, int radius,
                         uint8_t colour, const uint8_t *blend_table,
                         const uint8_t *soften_table)
{
    int x;
    int span;
    int shade;

    span = radius * 3;
    for (x = -span; x <= span; ++x) {
        /* Sparse far ends keep the streak delicate instead of a hard line. */
        if (((x * x + x * 11) & 15) > 5 && (x < -radius || x > radius))
            continue;
        shade = (x < 0 ? -x : x) * 12 / (span + 1);
        flare_pixel(buffer, cx + x, cy, (uint8_t)((int)colour - shade),
                    blend_table, soften_table, x < -radius || x > radius);
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
               flare->colour, intensity, flare->blend_table,
               flare->soften_table);
    flare_streak(buffer, flare->source_x, flare->source_y, flare->radius,
                 flare->colour, flare->blend_table, flare->soften_table);
    dx = screen_center_x - flare->source_x;
    dy = screen_center_y - flare->source_y;
    for (index = 1; index <= 4; ++index) {
        ghost_x = flare->source_x + dx * index / 5;
        ghost_y = flare->source_y + dy * index / 5;
        flare_disc(buffer, ghost_x, ghost_y,
                   flare->radius / (index + 1) + 2,
                   (uint8_t)(flare->colour - 7 - index * 2),
                   (uint8_t)(intensity / 2), flare->blend_table,
                   flare->soften_table);
    }
}
