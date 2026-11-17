#include "batch.h"

static int draw_before(const struct freak_sprite_batch_item *left,
                       const struct freak_sprite_batch_item *right)
{
    if (left->layer != right->layer)
        return left->layer < right->layer;
    return left->order <= right->order;
}

void freak_sprite_batch_reset(struct freak_sprite_batch *batch)
{
    if (batch)
        batch->count = 0;
}

int freak_sprite_batch_submit(struct freak_sprite_batch *batch,
                              const struct freak_sprite_2d *sprite,
                              int layer, int order)
{
    struct freak_sprite_batch_item *item;

    if (batch == 0 || sprite == 0 || batch->count >= FREAK_SPRITE_BATCH_MAX)
        return 0;
    item = &batch->items[batch->count++];
    item->sprite = *sprite;
    item->layer = layer;
    item->order = order;
    return 1;
}

void freak_sprite_batch_draw(unsigned char *buffer,
                             const struct freak_camera_2d *camera,
                             struct freak_sprite_batch *batch)
{
    unsigned int index;
    unsigned int sorted;
    struct freak_sprite_batch_item item;

    if (buffer == 0 || camera == 0 || batch == 0)
        return;
    /* Stable insertion sort keeps equal commands in submission order. */
    for (index = 1; index < batch->count; ++index) {
        item = batch->items[index];
        sorted = index;
        while (sorted > 0 && !draw_before(&batch->items[sorted - 1], &item)) {
            batch->items[sorted] = batch->items[sorted - 1];
            --sorted;
        }
        batch->items[sorted] = item;
    }
    for (index = 0; index < batch->count; ++index)
        freak_sprite_2d_draw(buffer, camera, &batch->items[index].sprite);
}
