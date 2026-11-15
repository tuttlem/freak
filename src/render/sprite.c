#include "sprite.h"
#include "draw.h"

static void sprite_blit(uint8_t *buffer, int x, int y,
                        const struct freak_sprite *sprite, int keyed,
                        int flip) {
    int source_x;
    int source_y;
    int target_x;
    int target_y;
    uint8_t pixel;

    if (buffer == 0 || sprite == 0 || sprite->pixels == 0)
        return;
    for (source_y = 0; source_y < sprite->height; ++source_y) {
        target_y = y + source_y;
        if (target_y < 0 || target_y >= FREAK_SCREEN_HEIGHT)
            continue;
        for (source_x = 0; source_x < sprite->width; ++source_x) {
            target_x = x + source_x;
            if (target_x < 0 || target_x >= FREAK_SCREEN_WIDTH)
                continue;
            pixel =
                sprite
                    ->pixels[source_y * sprite->width +
                             (flip ? sprite->width - source_x - 1 : source_x)];
            if (!keyed || pixel != sprite->transparent)
                buffer[target_y * FREAK_SCREEN_WIDTH + target_x] = pixel;
        }
    }
}

void freak_sprite_blit(uint8_t *buffer, int x, int y,
                       const struct freak_sprite *sprite) {
    sprite_blit(buffer, x, y, sprite, 0, 0);
}

void freak_sprite_blit_key(uint8_t *buffer, int x, int y,
                           const struct freak_sprite *sprite) {
    sprite_blit(buffer, x, y, sprite, 1, 0);
}

void freak_sprite_blit_flip_x(uint8_t *buffer, int x, int y,
                              const struct freak_sprite *sprite) {
    sprite_blit(buffer, x, y, sprite, 1, 1);
}

void freak_sprite_tile(uint8_t *buffer, int left, int top, int right,
                       int bottom, const struct freak_sprite *sprite) {
    int x, y;
    if (buffer == 0 || sprite == 0 || sprite->pixels == 0 || !sprite->width ||
        !sprite->height)
        return;
    for (y = top; y <= bottom; ++y)
        for (x = left; x <= right; ++x)
            if (x >= 0 && x < FREAK_SCREEN_WIDTH && y >= 0 &&
                y < FREAK_SCREEN_HEIGHT)
                buffer[y * FREAK_SCREEN_WIDTH + x] =
                    sprite->pixels[(y - top) % sprite->height * sprite->width +
                                   (x - left) % sprite->width];
}
