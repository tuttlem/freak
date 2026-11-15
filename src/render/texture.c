#include <malloc.h>
#include <string.h>

#include "texture.h"
#include "../core/noise.h"
#include "../core/fixed.h"
#include "draw.h"

static int texture_seed_x(unsigned int seed)
{
    return (int)(seed & 0xffffU);
}

static int texture_seed_y(unsigned int seed)
{
    return (int)(seed >> 16);
}

static uint8_t texture_palette_colour(int value, uint8_t palette_base,
                                      uint8_t palette_range)
{
    int shade;
    int colour;

    shade = ((value + 32768) * (int)palette_range) / 65536;
    if (shade < 0) {
        shade = 0;
    }
    if (shade >= (int)palette_range) {
        shade = (int)palette_range - 1;
    }
    colour = (int)palette_base + shade;
    if (colour > 255) {
        colour = 255;
    }
    return (uint8_t)colour;
}

static int texture_fire_intensity(const struct freak_texture *texture,
                                  int x, int y, uint8_t palette_base,
                                  uint8_t palette_range)
{
    int intensity;

    intensity = (int)freak_texture_pixel(texture, x, y) - palette_base;
    if (intensity < 0) {
        return 0;
    }
    if (intensity >= (int)palette_range) {
        return (int)palette_range - 1;
    }
    return intensity;
}

static uint8_t texture_fire_colour(int intensity, uint8_t palette_base)
{
    int colour;

    colour = (int)palette_base + intensity;
    if (colour > 255) {
        colour = 255;
    }
    return (uint8_t)colour;
}

int freak_texture_create(struct freak_texture *texture)
{
    if (texture == 0) {
        return 0;
    }
    texture->pixels = (uint8_t *)malloc(FREAK_TEXTURE_BYTES);
    return texture->pixels != 0;
}

void freak_texture_destroy(struct freak_texture *texture)
{
    if (texture == 0) {
        return;
    }
    if (texture->pixels != 0) {
        free(texture->pixels);
        texture->pixels = 0;
    }
}

void freak_texture_clear(struct freak_texture *texture, uint8_t colour)
{
    if (texture != 0 && texture->pixels != 0) {
        memset(texture->pixels, colour, FREAK_TEXTURE_BYTES);
    }
}

void freak_texture_copy(struct freak_texture *destination,
                        const struct freak_texture *source)
{
    if (destination != 0 && source != 0 &&
        destination->pixels != 0 && source->pixels != 0) {
        memcpy(destination->pixels, source->pixels, FREAK_TEXTURE_BYTES);
    }
}

void freak_texture_stamp_key(struct freak_texture *destination,
                             const struct freak_texture *source,
                             uint8_t transparent)
{
    unsigned int index;
    if (destination == 0 || source == 0 || destination->pixels == 0 || source->pixels == 0) return;
    for (index = 0; index < FREAK_TEXTURE_BYTES; ++index)
        if (source->pixels[index] != transparent) destination->pixels[index] = source->pixels[index];
}

void freak_texture_blit(uint8_t *buffer, int x, int y, const struct freak_texture *texture)
{
    int source_x, source_y, target_x, target_y;
    if (buffer == 0 || texture == 0 || texture->pixels == 0) return;
    for (source_y = 0; source_y < FREAK_TEXTURE_HEIGHT; ++source_y) {
        target_y = y + source_y; if (target_y < 0 || target_y >= FREAK_SCREEN_HEIGHT) continue;
        for (source_x = 0; source_x < FREAK_TEXTURE_WIDTH; ++source_x) {
            target_x = x + source_x; if (target_x >= 0 && target_x < FREAK_SCREEN_WIDTH)
                buffer[target_y * FREAK_SCREEN_WIDTH + target_x] = freak_texture_pixel(texture, source_x, source_y);
        }
    }
}

void freak_texture_blit_tile(uint8_t *buffer, int left, int top, int right,
                             int bottom, const struct freak_texture *texture,
                             int u, int v)
{
    int x, y;
    if (buffer == 0 || texture == 0 || texture->pixels == 0) return;
    for (y = top; y <= bottom; ++y)
        for (x = left; x <= right; ++x)
            if (x >= 0 && x < FREAK_SCREEN_WIDTH && y >= 0 && y < FREAK_SCREEN_HEIGHT)
                buffer[y * FREAK_SCREEN_WIDTH + x] = freak_texture_pixel(texture, u + x - left, v + y - top);
}

void freak_texture_noise(struct freak_texture *texture, unsigned int seed,
                         int scale, uint8_t palette_base,
                         uint8_t palette_range)
{
    int x;
    int y;
    int sample_x;
    int sample_y;
    int value;
    int colour;

    if (texture == 0 || texture->pixels == 0 || palette_range == 0) {
        return;
    }
    if (scale < 1) {
        scale = 1;
    }
    for (y = 0; y < FREAK_TEXTURE_HEIGHT; ++y) {
        sample_y = y / scale + (int)(seed >> 16);
        for (x = 0; x < FREAK_TEXTURE_WIDTH; ++x) {
            sample_x = x / scale + (int)(seed & 0xffffU);
            value = freak_noise_2d(sample_x, sample_y) + 32768;
            colour = (int)palette_base +
                     (value * (int)palette_range) / 65536;
            if (colour > 255) {
                colour = 255;
            }
            texture->pixels[x + (y << 8)] = (uint8_t)colour;
        }
    }
}

void freak_texture_fractal_noise(struct freak_texture *texture,
                                 unsigned int seed, int scale, int octaves,
                                 unsigned int persistence,
                                 uint8_t palette_base,
                                 uint8_t palette_range)
{
    int x;
    int y;
    int value;

    if (texture == 0 || texture->pixels == 0 || palette_range == 0) {
        return;
    }
    for (y = 0; y < FREAK_TEXTURE_HEIGHT; ++y) {
        for (x = 0; x < FREAK_TEXTURE_WIDTH; ++x) {
            value = freak_noise_fractal_2d(x + texture_seed_x(seed),
                                           y + texture_seed_y(seed), scale,
                                           octaves, persistence);
            freak_texture_pixel(texture, x, y) =
                texture_palette_colour(value, palette_base, palette_range);
        }
    }
}

void freak_texture_asphalt(struct freak_texture *texture, unsigned int seed,
                           uint8_t palette_base, uint8_t palette_range)
{
    int x;
    int y;
    int value;
    int detail;

    if (texture == 0 || texture->pixels == 0 || palette_range == 0) {
        return;
    }
    for (y = 0; y < FREAK_TEXTURE_HEIGHT; ++y) {
        for (x = 0; x < FREAK_TEXTURE_WIDTH; ++x) {
            value = freak_noise_fractal_2d(x + texture_seed_x(seed),
                                           y + texture_seed_y(seed),
                                           32, 4, 128U);
            detail = freak_noise_2d(x + texture_seed_y(seed),
                                    y + texture_seed_x(seed));
            value = (value * 3 + detail) / 4;
            freak_texture_pixel(texture, x, y) =
                texture_palette_colour(value, palette_base, palette_range);
        }
    }
}

void freak_texture_dirt(struct freak_texture *texture, unsigned int seed,
                        uint8_t palette_base, uint8_t palette_range)
{
    int x;
    int y;
    int value;

    if (texture == 0 || texture->pixels == 0 || palette_range == 0) {
        return;
    }
    for (y = 0; y < FREAK_TEXTURE_HEIGHT; ++y) {
        for (x = 0; x < FREAK_TEXTURE_WIDTH; ++x) {
            value = freak_noise_fractal_2d(x + texture_seed_x(seed),
                                           y + texture_seed_y(seed),
                                           18, 5, 150U);
            freak_texture_pixel(texture, x, y) =
                texture_palette_colour(value, palette_base, palette_range);
        }
    }
}

void freak_texture_grass(struct freak_texture *texture, unsigned int seed,
                         uint8_t palette_base, uint8_t palette_range)
{
    int x;
    int y;
    int broad;
    int blade;

    if (texture == 0 || texture->pixels == 0 || palette_range == 0) {
        return;
    }
    for (y = 0; y < FREAK_TEXTURE_HEIGHT; ++y) {
        for (x = 0; x < FREAK_TEXTURE_WIDTH; ++x) {
            broad = freak_noise_fractal_2d(x + texture_seed_x(seed),
                                           y + texture_seed_y(seed),
                                           48, 3, 144U);
            blade = freak_noise_value_2d(x + texture_seed_y(seed),
                                         y + texture_seed_x(seed), 5);
            freak_texture_pixel(texture, x, y) = texture_palette_colour(
                (broad * 3 + blade) / 4, palette_base, palette_range);
        }
    }
}

void freak_texture_clouds(struct freak_texture *texture, unsigned int seed,
                          uint8_t palette_base, uint8_t palette_range)
{
    int x;
    int y;
    int value;

    if (texture == 0 || texture->pixels == 0 || palette_range == 0) {
        return;
    }
    for (y = 0; y < FREAK_TEXTURE_HEIGHT; ++y) {
        for (x = 0; x < FREAK_TEXTURE_WIDTH; ++x) {
            value = freak_noise_fractal_2d(x + texture_seed_x(seed),
                                           y + texture_seed_y(seed),
                                           64, 5, 160U);
            freak_texture_pixel(texture, x, y) =
                texture_palette_colour(value, palette_base, palette_range);
        }
    }
}

void freak_texture_marble(struct freak_texture *texture, unsigned int seed,
                          uint8_t palette_base, uint8_t palette_range)
{
    int x;
    int y;
    int warp;
    int wave;

    if (texture == 0 || texture->pixels == 0 || palette_range == 0) {
        return;
    }
    for (y = 0; y < FREAK_TEXTURE_HEIGHT; ++y) {
        for (x = 0; x < FREAK_TEXTURE_WIDTH; ++x) {
            warp = freak_noise_fractal_2d(x + texture_seed_x(seed),
                                          y + texture_seed_y(seed),
                                          28, 4, 144U);
            wave = fixed_sin((x * 5 + y * 3 + warp / 192) & 1023);
            freak_texture_pixel(texture, x, y) = texture_palette_colour(
                wave / 2, palette_base, palette_range);
        }
    }
}

void freak_texture_plasma(struct freak_texture *texture, uint8_t *c1,
                          uint8_t *c2, uint8_t *c3, uint8_t *c4,
                          uint8_t palette_base, uint8_t palette_range)
{
    int x;
    int y;
    int wave;

    if (texture == 0 || texture->pixels == 0 || c1 == 0 || c2 == 0 ||
        c3 == 0 || c4 == 0 || palette_range == 0) {
        return;
    }
    for (y = 0; y < FREAK_TEXTURE_HEIGHT; ++y) {
        for (x = 0; x < FREAK_TEXTURE_WIDTH; ++x) {
            wave = fixed_sin((x * 4 + ((int)*c1 << 2)) & 1023) +
                   fixed_sin((y * 5 + ((int)*c2 << 2)) & 1023) +
                   fixed_sin(((x + y) * 3 + ((int)*c3 << 2)) & 1023) +
                   fixed_sin(((x - y) * 2 + ((int)*c4 << 2)) & 1023);
            freak_texture_pixel(texture, x, y) = texture_palette_colour(
                wave / 4, palette_base, palette_range);
        }
    }
    *c1 += 1;
    *c2 += 2;
    *c3 += 3;
    *c4 += 5;
}

void freak_texture_smudge_up(struct freak_texture *destination,
                             const struct freak_texture *source,
                             unsigned int seed, uint8_t palette_base,
                             uint8_t palette_range)
{
    int x;
    int y;
    int value;
    int cooling;
    int noise;

    if (destination == 0 || source == 0 || destination->pixels == 0 ||
        source->pixels == 0 || palette_range == 0) {
        return;
    }
    for (y = 0; y < FREAK_TEXTURE_HEIGHT - 1; ++y) {
        for (x = 0; x < FREAK_TEXTURE_WIDTH; ++x) {
            value = texture_fire_intensity(source, x - 1, y + 1,
                                           palette_base, palette_range) +
                    texture_fire_intensity(source, x, y + 1,
                                           palette_base, palette_range) * 2 +
                    texture_fire_intensity(source, x + 1, y + 1,
                                           palette_base, palette_range);
            noise = freak_noise_2d(x + texture_seed_x(seed),
                                   y + texture_seed_y(seed));
            cooling = 1 + ((noise + 32768) >> 14);
            value = value / 4 - cooling;
            if (value < 0) {
                value = 0;
            }
            if (value >= (int)palette_range) {
                value = (int)palette_range - 1;
            }
            freak_texture_pixel(destination, x, y) =
                texture_fire_colour(value, palette_base);
        }
    }
    y = FREAK_TEXTURE_HEIGHT - 1;
    for (x = 0; x < FREAK_TEXTURE_WIDTH; ++x) {
        noise = freak_noise_2d(x + texture_seed_x(seed),
                               y + texture_seed_y(seed));
        value = ((noise + 32768) * (int)palette_range) / 65536;
        if (value >= (int)palette_range) {
            value = (int)palette_range - 1;
        }
        freak_texture_pixel(destination, x, y) =
            texture_fire_colour(value, palette_base);
    }
}


