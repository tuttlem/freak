#include "palette.h"
#include "../dos/vga.h"

const struct freak_colour FREAK_PALETTE_GREYSCALE[] = {{0, 0, 0}, {63, 63, 63}};
const struct freak_colour FREAK_PALETTE_SKY[] = {
    {2, 4, 18}, {8, 24, 52}, {40, 55, 63}};
const struct freak_colour FREAK_PALETTE_FIRE[] = {
    {0, 0, 0}, {28, 0, 0}, {63, 12, 0}, {63, 50, 4}, {63, 63, 48}};
const struct freak_colour FREAK_PALETTE_SMOKE[] = {
    {0, 0, 0}, {10, 10, 14}, {32, 32, 36}, {56, 56, 60}};
const struct freak_colour FREAK_PALETTE_WATER[] = {
    {0, 4, 18}, {0, 18, 42}, {8, 40, 60}, {45, 62, 63}};
const struct freak_colour FREAK_PALETTE_FOREST[] = {
    {1, 8, 1}, {4, 24, 3}, {14, 42, 8}, {42, 60, 20}};
const struct freak_colour FREAK_PALETTE_SEPIA[] = {
    {8, 4, 1}, {24, 13, 3}, {44, 29, 12}, {63, 54, 32}};
const struct freak_colour FREAK_PALETTE_ICE[] = {
    {3, 10, 20}, {8, 28, 42}, {30, 52, 62}, {63, 63, 63}};

static uint8_t palette_lerp(uint8_t from, uint8_t to, int fraction) {
    return (uint8_t)((int)from + (((int)to - from) * fraction) / 65535);
}

void freak_palette_preset(uint8_t first, uint8_t count,
                          const struct freak_colour *keys, int key_count) {
    int index;
    int position;
    int segment;
    int fraction;
    struct freak_colour colour;

    if (count == 0 || keys == 0 || key_count < 1)
        return;
    for (index = 0; index < count; ++index) {
        if (count == 1 || key_count == 1) {
            colour = keys[0];
        } else {
            position = (index * (key_count - 1) * 65535) / (count - 1);
            segment = position / 65535;
            fraction = position % 65535;
            if (segment >= key_count - 1) {
                segment = key_count - 2;
                fraction = 65535;
            }
            colour.red = palette_lerp(keys[segment].red, keys[segment + 1].red,
                                      fraction);
            colour.green = palette_lerp(keys[segment].green,
                                        keys[segment + 1].green, fraction);
            colour.blue = palette_lerp(keys[segment].blue,
                                       keys[segment + 1].blue, fraction);
        }
        freak_set_palette((uint8_t)(first + index), colour.red, colour.green,
                          colour.blue);
    }
}

void freak_palette_greyscale(uint8_t first, uint8_t count, uint8_t dark,
                             uint8_t light) {
    struct freak_colour keys[2];
    keys[0].red = keys[0].green = keys[0].blue = dark;
    keys[1].red = keys[1].green = keys[1].blue = light;
    freak_palette_gradient(first, count, keys, 2);
}

void freak_palette_sky(uint8_t first, uint8_t count) {
    freak_palette_preset(first, count, FREAK_PALETTE_SKY,
                         FREAK_PALETTE_SKY_COUNT);
}

void freak_palette_fire(uint8_t first, uint8_t count) {
    freak_palette_preset(first, count, FREAK_PALETTE_FIRE,
                         FREAK_PALETTE_FIRE_COUNT);
}

void freak_palette_smoke(uint8_t first, uint8_t count) {
    freak_palette_preset(first, count, FREAK_PALETTE_SMOKE,
                         FREAK_PALETTE_SMOKE_COUNT);
}

void freak_palette_rotate(uint8_t first, uint8_t count, int direction) {
    struct freak_colour colours[256];
    int index;
    int source;

    if (count < 2 || direction == 0)
        return;
    for (index = 0; index < count; ++index)
        freak_get_palette((uint8_t)(first + index), &colours[index].red,
                          &colours[index].green, &colours[index].blue);
    for (index = 0; index < count; ++index) {
        source = direction > 0 ? index - 1 : index + 1;
        if (source < 0)
            source = count - 1;
        if (source >= count)
            source = 0;
        freak_set_palette((uint8_t)(first + index), colours[source].red,
                          colours[source].green, colours[source].blue);
    }
}
