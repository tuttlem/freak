#include <stdio.h>
#include <malloc.h>

#include "tilemap.h"

static unsigned long tile_count(const struct freak_tilemap *map)
{
    return (unsigned long)map->width * map->height;
}

static int write_u16(FILE *file, uint16_t value)
{
    return fputc(value & 255, file) != EOF && fputc(value >> 8, file) != EOF;
}

static int read_u16(FILE *file, uint16_t *value)
{
    int low = fgetc(file);
    int high = fgetc(file);
    if (low == EOF || high == EOF)
        return 0;
    *value = (uint16_t)(low | (high << 8));
    return 1;
}

int freak_tilemap_init(struct freak_tilemap *map, uint16_t width,
                       uint16_t height, uint16_t tile_width,
                       uint16_t tile_height, uint8_t layer_count,
                       uint8_t with_collision)
{
    unsigned long count;
    unsigned int layer;
    unsigned long index;

    if (map == 0 || width == 0 || height == 0 || tile_width == 0 ||
        tile_height == 0 || layer_count == 0 ||
        layer_count > FREAK_TILEMAP_MAX_LAYERS)
        return 0;
    map->width = width;
    map->height = height;
    map->tile_width = tile_width;
    map->tile_height = tile_height;
    map->layer_count = layer_count;
    map->collision = 0;
    for (layer = 0; layer < FREAK_TILEMAP_MAX_LAYERS; ++layer)
        map->layers[layer] = 0;
    count = tile_count(map);
    for (layer = 0; layer < layer_count; ++layer) {
        map->layers[layer] = (uint16_t *)malloc(count * sizeof(uint16_t));
        if (map->layers[layer] == 0) {
            freak_tilemap_destroy(map);
            return 0;
        }
        for (index = 0; index < count; ++index)
            map->layers[layer][index] = FREAK_TILE_EMPTY;
    }
    if (with_collision) {
        map->collision = (uint8_t *)malloc(count);
        if (map->collision == 0) {
            freak_tilemap_destroy(map);
            return 0;
        }
        for (index = 0; index < count; ++index)
            map->collision[index] = 0;
    }
    return 1;
}

void freak_tilemap_destroy(struct freak_tilemap *map)
{
    unsigned int layer;

    if (map == 0)
        return;
    for (layer = 0; layer < FREAK_TILEMAP_MAX_LAYERS; ++layer) {
        free(map->layers[layer]);
        map->layers[layer] = 0;
    }
    free(map->collision);
    map->collision = 0;
    map->width = map->height = 0;
    map->tile_width = map->tile_height = 0;
    map->layer_count = 0;
}

void freak_tilemap_set(struct freak_tilemap *map, uint8_t layer,
                       uint16_t x, uint16_t y, uint16_t tile)
{
    if (map == 0 || layer >= map->layer_count || x >= map->width ||
        y >= map->height)
        return;
    map->layers[layer][(unsigned long)y * map->width + x] = tile;
}

uint16_t freak_tilemap_get(const struct freak_tilemap *map, uint8_t layer,
                           uint16_t x, uint16_t y)
{
    if (map == 0 || layer >= map->layer_count || x >= map->width ||
        y >= map->height)
        return FREAK_TILE_EMPTY;
    return map->layers[layer][(unsigned long)y * map->width + x];
}

void freak_tilemap_set_collision(struct freak_tilemap *map, uint16_t x,
                                 uint16_t y, uint8_t flags)
{
    if (map && map->collision && x < map->width && y < map->height)
        map->collision[(unsigned long)y * map->width + x] = flags;
}

uint8_t freak_tilemap_collision(const struct freak_tilemap *map, uint16_t x,
                                uint16_t y)
{
    if (map == 0 || map->collision == 0 || x >= map->width || y >= map->height)
        return 0;
    return map->collision[(unsigned long)y * map->width + x];
}

void freak_tilemap_draw(uint8_t *buffer, const struct freak_camera_2d *camera,
                        const struct freak_tilemap *map,
                        const struct freak_tile_atlas *atlas, uint8_t layer)
{
    int first_x;
    int first_y;
    int last_x;
    int last_y;
    int x;
    int y;
    int screen_x;
    int screen_y;
    uint16_t tile;

    if (buffer == 0 || camera == 0 || map == 0 || atlas == 0 ||
        atlas->tiles == 0 || layer >= map->layer_count)
        return;
    first_x = camera->x / map->tile_width;
    first_y = camera->y / map->tile_height;
    if (first_x < 0) first_x = 0;
    if (first_y < 0) first_y = 0;
    last_x = (camera->x + 319) / map->tile_width + 1;
    last_y = (camera->y + 199) / map->tile_height + 1;
    if (last_x > map->width) last_x = map->width;
    if (last_y > map->height) last_y = map->height;
    for (y = first_y; y < last_y; ++y) {
        for (x = first_x; x < last_x; ++x) {
            tile = freak_tilemap_get(map, layer, (uint16_t)x, (uint16_t)y);
            if (tile == FREAK_TILE_EMPTY || tile >= atlas->count)
                continue;
            freak_camera_2d_world_to_screen(camera, x * map->tile_width,
                                             y * map->tile_height,
                                             &screen_x, &screen_y);
            freak_sprite_blit_key(buffer, screen_x, screen_y,
                                  &atlas->tiles[tile]);
        }
    }
}

int freak_tilemap_save(const char *filename, const struct freak_tilemap *map)
{
    FILE *file;
    unsigned int layer;
    unsigned long count;
    unsigned long index;

    if (filename == 0 || map == 0)
        return 0;
    file = fopen(filename, "wb");
    if (file == 0)
        return 0;
    if (fwrite("FMP1", 1, 4, file) != 4 || !write_u16(file, map->width) ||
        !write_u16(file, map->height) || !write_u16(file, map->tile_width) ||
        !write_u16(file, map->tile_height) || fputc(map->layer_count, file) == EOF ||
        fputc(map->collision != 0, file) == EOF) {
        fclose(file);
        return 0;
    }
    count = tile_count(map);
    for (layer = 0; layer < map->layer_count; ++layer)
        for (index = 0; index < count; ++index)
            if (!write_u16(file, map->layers[layer][index])) {
                fclose(file);
                return 0;
            }
    if (map->collision && fwrite(map->collision, 1, count, file) != count) {
        fclose(file);
        return 0;
    }
    fclose(file);
    return 1;
}

int freak_tilemap_load(const char *filename, struct freak_tilemap *map)
{
    FILE *file;
    char magic[4];
    uint16_t width, height, tile_width, tile_height;
    int layer_count, has_collision;
    unsigned int layer;
    unsigned long count;
    unsigned long index;

    if (filename == 0 || map == 0)
        return 0;
    file = fopen(filename, "rb");
    if (file == 0)
        return 0;
    if (fread(magic, 1, 4, file) != 4 || magic[0] != 'F' || magic[1] != 'M' ||
        magic[2] != 'P' || magic[3] != '1' || !read_u16(file, &width) ||
        !read_u16(file, &height) || !read_u16(file, &tile_width) ||
        !read_u16(file, &tile_height)) {
        fclose(file);
        return 0;
    }
    layer_count = fgetc(file);
    has_collision = fgetc(file);
    if (layer_count == EOF || has_collision == EOF ||
        !freak_tilemap_init(map, width, height, tile_width, tile_height,
                            (uint8_t)layer_count, (uint8_t)has_collision)) {
        fclose(file);
        return 0;
    }
    count = tile_count(map);
    for (layer = 0; layer < map->layer_count; ++layer)
        for (index = 0; index < count; ++index)
            if (!read_u16(file, &map->layers[layer][index])) {
                freak_tilemap_destroy(map);
                fclose(file);
                return 0;
            }
    if (map->collision && fread(map->collision, 1, count, file) != count) {
        freak_tilemap_destroy(map);
        fclose(file);
        return 0;
    }
    fclose(file);
    return 1;
}
