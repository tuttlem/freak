#ifndef __freak_sprite_2d_h_
#define __freak_sprite_2d_h_

#include "camera.h"
#include "../render/sprite.h"

struct freak_sprite_2d {
    const struct freak_sprite *sprite;
    int x;
    int y;
    unsigned char keyed;
    unsigned char flip_x;
};

struct freak_sprite_animation {
    const struct freak_sprite *frames;
    unsigned int frame_count;
    unsigned int ticks_per_frame;
    unsigned long ticks;
};

void freak_sprite_2d_draw(unsigned char *buffer,
                          const struct freak_camera_2d *camera,
                          const struct freak_sprite_2d *sprite);
void freak_sprite_animation_init(struct freak_sprite_animation *animation,
                                 const struct freak_sprite *frames,
                                 unsigned int frame_count,
                                 unsigned int ticks_per_frame);
void freak_sprite_animation_advance(struct freak_sprite_animation *animation,
                                    unsigned long elapsed_ticks);
const struct freak_sprite *freak_sprite_animation_frame(
    const struct freak_sprite_animation *animation);

#endif
