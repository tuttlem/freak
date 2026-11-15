/******************************************************************************
 * Freak procedural texture gallery
 *
 * Layout, left to right:
 *   raw noise, value noise, fBm, asphalt, dirt
 *   grass, clouds, marble, plasma, fire
 ******************************************************************************/

#include <malloc.h>

#include "../src/freak.h"

#define GALLERY_COLUMNS 5
#define GALLERY_ROWS    2
#define TILE_WIDTH      64
#define TILE_HEIGHT     100
#define GALLERY_PAGES   10

static unsigned char *screen;
static struct freak_texture pages[GALLERY_PAGES];
static struct freak_texture fire_page;
static struct freak_texture *fire_current;
static struct freak_texture *fire_next;
static unsigned char c1;
static unsigned char c2;
static unsigned char c3;
static unsigned char c4;

static void set_palette_range(int first, int count, int red, int green,
                              int blue, int red_step, int green_step,
                              int blue_step)
{
    int index;

    for (index = 0; index < count; ++index) {
        freak_set_palette((unsigned char)(first + index),
                          (unsigned char)(red + index * red_step),
                          (unsigned char)(green + index * green_step),
                          (unsigned char)(blue + index * blue_step));
    }
}

static void set_gallery_palette(void)
{
    set_palette_range(16, 16, 4, 4, 4, 3, 3, 3);
    set_palette_range(32, 16, 12, 5, 1, 3, 2, 1);
    set_palette_range(48, 16, 2, 10, 1, 1, 3, 1);
    set_palette_range(64, 16, 8, 12, 20, 3, 3, 2);
    set_palette_range(80, 16, 10, 2, 16, 3, 2, 3);
    set_palette_range(96, 16, 4, 50, 28, 3, -3, 2);
    set_palette_range(112, 8, 8, 0, 0, 7, 0, 0);
    set_palette_range(120, 8, 63, 4, 0, 0, 8, 0);
}

static void draw_tile(const struct freak_texture *texture, int left, int top)
{
    int x;
    int y;
    int source_y;

    for (y = 0; y < TILE_HEIGHT; ++y) {
        source_y = (y * FREAK_TEXTURE_HEIGHT) / TILE_HEIGHT;
        for (x = 0; x < TILE_WIDTH; ++x) {
            screen[(top + y) * 320 + left + x] =
                freak_texture_pixel(texture, x << 2, source_y);
        }
    }
}

static void draw_gallery(void)
{
    int index;
    int left;
    int top;

    for (index = 0; index < GALLERY_PAGES; ++index) {
        left = (index % GALLERY_COLUMNS) * TILE_WIDTH;
        top = (index / GALLERY_COLUMNS) * TILE_HEIGHT;
        if (index == 9) {
            draw_tile(fire_current, left, top);
        } else {
            draw_tile(&pages[index], left, top);
        }
    }
}

static int create_gallery(void)
{
    int index;

    for (index = 0; index < GALLERY_PAGES; ++index) {
        if (!freak_texture_create(&pages[index])) {
            return 0;
        }
    }
    if (!freak_texture_create(&fire_page)) {
        return 0;
    }

    freak_texture_noise(&pages[0], 1U, 2, 16, 16);
    freak_texture_fractal_noise(&pages[1], 2U, 32, 1, 128U, 16, 16);
    freak_texture_fractal_noise(&pages[2], 3U, 64, 5, 150U, 16, 16);
    freak_texture_asphalt(&pages[3], 4U, 16, 16);
    freak_texture_dirt(&pages[4], 5U, 32, 16);
    freak_texture_grass(&pages[5], 6U, 48, 16);
    freak_texture_clouds(&pages[6], 7U, 64, 16);
    freak_texture_marble(&pages[7], 8U, 80, 16);
    freak_texture_clear(&pages[9], 112);
    freak_texture_clear(&fire_page, 112);
    fire_current = &pages[9];
    fire_next = &fire_page;
    return 1;
}

static void destroy_gallery(void)
{
    int index;

    for (index = 0; index < GALLERY_PAGES; ++index) {
        freak_texture_destroy(&pages[index]);
    }
    freak_texture_destroy(&fire_page);
}

int main(void)
{
    unsigned int tick;
    struct freak_texture *swap;

    screen = (unsigned char *)malloc(64000U);
    if (screen == 0 || !create_gallery()) {
        destroy_gallery();
        free(screen);
        return 1;
    }

    freak_set_mcga();
    set_gallery_palette();
    tick = 0;
    c1 = 0;
    c2 = 0;
    c3 = 0;
    c4 = 0;
    while (!freak_kbhit()) {
        freak_texture_plasma(&pages[8], &c1, &c2, &c3, &c4, 96, 16);
        freak_texture_smudge_up(fire_next, fire_current, tick, 112, 16);
        swap = fire_current;
        fire_current = fire_next;
        fire_next = swap;
        draw_gallery();
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
        ++tick;
    }
    freak_getch();
    freak_set_text();
    destroy_gallery();
    free(screen);
    return 0;
}
