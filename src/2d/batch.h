#ifndef __freak_sprite_batch_h_
#define __freak_sprite_batch_h_

#include "sprite.h"

#define FREAK_SPRITE_BATCH_MAX 128

struct freak_sprite_batch_item {
    struct freak_sprite_2d sprite;
    int layer;
    int order;
};

struct freak_sprite_batch {
    struct freak_sprite_batch_item items[FREAK_SPRITE_BATCH_MAX];
    unsigned int count;
};

void freak_sprite_batch_reset(struct freak_sprite_batch *batch);
int freak_sprite_batch_submit(struct freak_sprite_batch *batch,
                              const struct freak_sprite_2d *sprite,
                              int layer, int order);
void freak_sprite_batch_draw(unsigned char *buffer,
                             const struct freak_camera_2d *camera,
                             struct freak_sprite_batch *batch);

#endif
