#include "sprite.h"

void freak_sprite_2d_draw(unsigned char *buffer,
                          const struct freak_camera_2d *camera,
                          const struct freak_sprite_2d *sprite)
{
    int x;
    int y;

    if (buffer == 0 || camera == 0 || sprite == 0 || sprite->sprite == 0)
        return;
    freak_camera_2d_world_to_screen(camera, sprite->x, sprite->y, &x, &y);
    if (sprite->flip_x)
        freak_sprite_blit_flip_x(buffer, x, y, sprite->sprite);
    else if (sprite->keyed)
        freak_sprite_blit_key(buffer, x, y, sprite->sprite);
    else
        freak_sprite_blit(buffer, x, y, sprite->sprite);
}

void freak_sprite_animation_init(struct freak_sprite_animation *animation,
                                 const struct freak_sprite *frames,
                                 unsigned int frame_count,
                                 unsigned int ticks_per_frame)
{
    if (animation == 0)
        return;
    animation->frames = frames;
    animation->frame_count = frame_count;
    animation->ticks_per_frame = ticks_per_frame == 0 ? 1 : ticks_per_frame;
    animation->ticks = 0;
}

void freak_sprite_animation_advance(struct freak_sprite_animation *animation,
                                    unsigned long elapsed_ticks)
{
    if (animation)
        animation->ticks += elapsed_ticks;
}

const struct freak_sprite *freak_sprite_animation_frame(
    const struct freak_sprite_animation *animation)
{
    unsigned long frame;

    if (animation == 0 || animation->frames == 0 || animation->frame_count == 0)
        return 0;
    frame = (animation->ticks / animation->ticks_per_frame) %
            animation->frame_count;
    return &animation->frames[frame];
}
