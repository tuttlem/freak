/******************************************************************************
 * Freak procedural texture gallery
 *
 * Layout, left to right:
 *   raw noise, value noise, fBm, asphalt, dirt
 *   grass, clouds, marble, plasma, fire
 ******************************************************************************/

#include <malloc.h>
#include <stdio.h>

#include "../../src/freak.h"

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
static struct freak_metaball metaballs[3];
static struct freak_font gallery_font = {
    (const unsigned char *)0x000FFA6EL, 8, 8, 0, 128
};

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
    freak_palette_greyscale(16, 16, 4, 49);
    set_palette_range(32, 16, 12, 5, 1, 3, 2, 1);
    set_palette_range(48, 16, 2, 10, 1, 1, 3, 1);
    freak_palette_smoke(64, 16);
    set_palette_range(80, 16, 10, 2, 16, 3, 2, 3);
    set_palette_range(96, 16, 4, 50, 28, 3, -3, 2);
    freak_palette_fire(112, 16);
    freak_set_palette(15, 63, 63, 63);
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

static int create_gallery(uint32_t master_seed)
{
    int index;
    struct freak_rng_streams random;

    freak_rng_streams_seed(&random, master_seed);

    for (index = 0; index < GALLERY_PAGES; ++index) {
        if (!freak_texture_create(&pages[index])) {
            return 0;
        }
    }
    if (!freak_texture_create(&fire_page)) {
        return 0;
    }

    freak_texture_noise(&pages[0], freak_rng_next_u32(&random.visuals), 2, 16, 16);
    freak_texture_fractal_noise(&pages[1], freak_rng_next_u32(&random.visuals),
                                32, 1, 128U, 16, 16);
    freak_texture_fractal_noise(&pages[2], freak_rng_next_u32(&random.visuals),
                                64, 5, 150U, 16, 16);
    freak_texture_asphalt(&pages[3], freak_rng_next_u32(&random.visuals), 16, 16);
    freak_texture_dirt(&pages[4], freak_rng_next_u32(&random.visuals), 32, 16);
    freak_texture_grass(&pages[5], freak_rng_next_u32(&random.visuals), 48, 16);
    freak_texture_clouds(&pages[6], freak_rng_next_u32(&random.visuals), 64, 16);
    freak_texture_marble(&pages[7], freak_rng_next_u32(&random.visuals), 80, 16);
    freak_texture_clear(&pages[9], 112);
    freak_texture_clear(&fire_page, 112);
    fire_current = &pages[9];
    fire_next = &fire_page;
    c1 = (unsigned char)freak_rng_next_u16(&random.visuals);
    c2 = (unsigned char)freak_rng_next_u16(&random.visuals);
    c3 = (unsigned char)freak_rng_next_u16(&random.visuals);
    c4 = (unsigned char)freak_rng_next_u16(&random.visuals);
    metaballs[0].radius = 54;
    metaballs[1].radius = 46;
    metaballs[2].radius = 38;
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

int freak_demo_texture_gallery(void)
{
    unsigned int tick;
    unsigned long last_tick;
    unsigned long now;
    uint32_t master_seed;
    struct freak_rng seed_rng;
    char seed_text[48];
    struct freak_texture *swap;

    screen = (unsigned char *)malloc(64000U);
    master_seed = 0x00c0ffeeUL;
    freak_rng_seed(&seed_rng, master_seed);
    if (screen == 0 || !create_gallery(master_seed)) {
        destroy_gallery();
        free(screen);
        return 1;
    }

    set_gallery_palette();
    tick = 0;
    last_tick = freak_timer_ticks();
    freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings,
                                FREAK_ACTION_BACK))
            break;
        if (freak_input_key_pressed(FREAK_KEY_R)) {
            master_seed = freak_rng_next_u32(&seed_rng);
            destroy_gallery();
            if (!create_gallery(master_seed))
                break;
            tick = 0;
        }
        if (freak_input_key_pressed(FREAK_KEY_0)) {
            master_seed = 0x00c0ffeeUL;
            freak_rng_seed(&seed_rng, master_seed);
            destroy_gallery();
            if (!create_gallery(master_seed))
                break;
            tick = 0;
        }
        now = freak_timer_ticks();
        tick += (unsigned int)(now - last_tick);
        last_tick = now;
        metaballs[0].x = 80 + fixed_to_int(fixed_sin((tick * 9) & 1023) * 42);
        metaballs[0].y = 112 + fixed_to_int(fixed_cos((tick * 7) & 1023) * 36);
        metaballs[1].x = 150 + fixed_to_int(fixed_sin((tick * 5 + 250) & 1023) * 58);
        metaballs[1].y = 130 + fixed_to_int(fixed_cos((tick * 11 + 250) & 1023) * 40);
        metaballs[2].x = 130 + fixed_to_int(fixed_sin((tick * 13 + 620) & 1023) * 50);
        metaballs[2].y = 70 + fixed_to_int(fixed_cos((tick * 6 + 620) & 1023) * 38);
        freak_texture_metaballs(&pages[0], metaballs, 3, 16, 16);
        freak_texture_water_ripple(&pages[1], &pages[2], tick, 3);
        freak_texture_plasma(&pages[8], &c1, &c2, &c3, &c4, 96, 16);
        freak_texture_smudge_up(fire_next, fire_current, tick, 112, 16);
        /* Palette cycling animates existing pixels without touching the page. */
        if ((tick & 3U) == 0)
            freak_palette_rotate(96, 16, 1);
        if ((tick & 7U) == 0)
            freak_palette_rotate(112, 16, -1);
        swap = fire_current;
        fire_current = fire_next;
        fire_next = swap;
        draw_gallery();
        sprintf(seed_text, "Seed: %lu  R: new  0: reset", master_seed);
        freak_font_text(screen, 4, 4, &gallery_font, seed_text, 15);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
    }
    destroy_gallery();
    free(screen);
    return 0;
}
