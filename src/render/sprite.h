#ifndef __freak_sprite_h_
#define __freak_sprite_h_

#include <stdint.h>

struct freak_sprite {
    const uint8_t *pixels;
    uint16_t width;
    uint16_t height;
    uint8_t transparent;
};

void freak_sprite_blit(uint8_t *buffer, int x, int y,
                       const struct freak_sprite *sprite);
void freak_sprite_blit_key(uint8_t *buffer, int x, int y,
                           const struct freak_sprite *sprite);
void freak_sprite_blit_flip_x(uint8_t *buffer, int x, int y,
                              const struct freak_sprite *sprite);
void freak_sprite_tile(uint8_t *buffer, int left, int top, int right,
                       int bottom, const struct freak_sprite *sprite);

#endif
