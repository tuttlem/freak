#include <malloc.h>

#include "democom.h"

static uint8_t normal_map[196608UL];
static int bump_heights[65536UL];

static void build_normal_map(void)
{
    int x;
    int y;
    int slope_x;
    int slope_y;
    int red;
    int green;
    unsigned long p;
    for (y = 0; y < 256; ++y) for (x = 0; x < 256; ++x)
        bump_heights[y * 256 + x] = freak_noise_fractal_2d(x + 173, y - 911,
                                                             32, 5, 175U) +
                                  freak_noise_value_2d(x + 71, y + 401, 7) / 4;
    for (y = 0; y < 256; ++y) for (x = 0; x < 256; ++x) {
        p = ((unsigned long)y * 256UL + x) * 3UL;
        slope_x = bump_heights[y * 256 + ((x + 1) & 255)] -
                  bump_heights[y * 256 + ((x - 1) & 255)];
        slope_y = bump_heights[((y + 1) & 255) * 256 + x] -
                  bump_heights[((y - 1) & 255) * 256 + x];
        red = 128 - slope_x / 512;
        green = 128 - slope_y / 512;
        if (red < 0) red = 0; if (red > 255) red = 255;
        if (green < 0) green = 0; if (green > 255) green = 255;
        normal_map[p] = (uint8_t)red;
        normal_map[p + 1] = (uint8_t)green;
        normal_map[p + 2] = 255;
    }
}

static void draw_sphere(int cx, int cy, int radius, int tick, int chrome)
{
    struct freak_light3d light;
    vec3 position;
    vec3 normal;
    vec3 view;
    int x;
    int y;
    int dx;
    int dy;
    int d2;
    int z;
    uint8_t shade;
    uint8_t u;
    uint8_t v;
    light.type = FREAK_LIGHT3D_POINT;
    light.position.x = int_to_fixed(2) * fixed_sin_interp((tick * 7) & 1023);
    light.position.y = int_to_fixed(2) * fixed_cos_interp((tick * 5) & 1023);
    light.position.z = int_to_fixed(3);
    light.intensity = FIXED_ONE;
    light.range = int_to_fixed(5);
    view.x = 0; view.y = 0; view.z = -FIXED_ONE;
    for (y = cy - radius; y <= cy + radius; ++y) for (x = cx - radius; x <= cx + radius; ++x) {
        dx = x - cx; dy = y - cy; d2 = dx * dx + dy * dy;
        if (d2 > radius * radius || x < 0 || x >= 320 || y < 0 || y >= 200) continue;
        z = fixed_to_int(fixed_sqrt(int_to_fixed(radius * radius - d2)));
        normal.x = fixed_div(int_to_fixed(dx), int_to_fixed(radius));
        normal.y = fixed_div(int_to_fixed(-dy), int_to_fixed(radius));
        normal.z = fixed_div(int_to_fixed(z), int_to_fixed(radius));
        position = normal;
        if (chrome) {
            freak_shade3d_sphere_uv(&normal, &view, &u, &v);
            u = (uint8_t)(u + tick * 3);
            if (chrome == 1) {
                /* A tiny reflected world: blue sky, green horizon, blue
                   water, and a moving white sun highlight. */
                if (u > 104 && u < 136 && v < 60) screen[y * 320 + x] = 15;
                else if (v < 82) screen[y * 320 + x] = (uint8_t)(96 + (v >> 3));
                else if (v < 150) screen[y * 320 + x] = (uint8_t)(160 + ((u ^ v) >> 4));
                else screen[y * 320 + x] = (uint8_t)(128 + ((u + v) >> 4));
            } else {
                screen[y * 320 + x] = (uint8_t)(128 + ((u + v + tick) >> 4));
            }
        } else {
            shade = freak_shade3d_lambert(&position, &normal, &light, 36, 20);
            screen[y * 320 + x] = (uint8_t)(32 + (shade >> 2));
        }
    }
}

void freak_demo_lighting3d(void)
{
    unsigned long last;
    unsigned long now;
    unsigned long tick;
    freak_demo_palette_default();
    last = freak_timer_ticks(); tick = 0;
    freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        now = freak_timer_ticks(); tick += now - last; last = now;
        freak_clear_buffer(0, screen);
        draw_sphere(95, 105, 70, (int)tick, 0);
        freak_font_text(screen, 8, 8, &font, "3D lighting: ambient, point attenuation, specular", 15);
        freak_font_text(screen, 8, 18, &font, "Per-pixel preview; meshes use the same per-vertex helper  Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, screen);
    }
}

void freak_demo_bump3d(void)
{
    struct freak_light3d light;
    vec3 position;
    int x;
    int y;
    unsigned long tick;
    int automatic;
    build_normal_map();
    light.type = FREAK_LIGHT3D_POINT; light.intensity = FIXED_ONE; light.range = int_to_fixed(4);
    tick = 0; automatic = 1; freak_demo_palette_default();
    freak_palette_fire(64, 64);
    /* Reserve the top of this material's palette ramp for a hard specular
       highlight instead of ending on fire-yellow. */
    freak_set_palette(127, 63, 63, 63);
    freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        ++tick;
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_ACCEPT)) automatic = !automatic;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_LEFT) ||
            freak_input_down(&freak_input_default_bindings, FREAK_ACTION_RIGHT) ||
            freak_input_down(&freak_input_default_bindings, FREAK_ACTION_UP) ||
            freak_input_down(&freak_input_default_bindings, FREAK_ACTION_DOWN)) automatic = 0;
        if (automatic) {
            light.position.x = fixed_sin_interp((tick * 9) & 1023) * 2;
            light.position.y = fixed_cos_interp((tick * 7) & 1023) * 2;
        } else {
            if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_LEFT)) light.position.x -= FIXED_ONE / 16;
            if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_RIGHT)) light.position.x += FIXED_ONE / 16;
            if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_UP)) light.position.y += FIXED_ONE / 16;
            if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_DOWN)) light.position.y -= FIXED_ONE / 16;
        }
        light.position.z = int_to_fixed(2);
        freak_clear_buffer(0, screen);
        for (y = 35; y < 185; ++y) for (x = 25; x < 295; ++x) {
            position.x = fixed_div(int_to_fixed(x - 160), int_to_fixed(100));
            position.y = fixed_div(int_to_fixed(110 - y), int_to_fixed(100)); position.z = 0;
            screen[y * 320 + x] = (uint8_t)(64 +
                ((unsigned int)freak_shade3d_bump(normal_map, x,
                y, &position, &light, 8) * 63U) / 255U);
        }
        for (x = 25; x < 295; x += 16) for (y = 35; y < 185; ++y) screen[y * 320 + x] = 15;
        for (y = 35; y < 185; y += 16) freak_draw_hline(screen, y, 25, 294, 15);
        freak_font_text(screen, 8, 8, &font, "Bump mapping: noisy fractal height field, 64 red shades", 15);
        freak_font_text(screen, 8, 18, &font, automatic ? "Arrows: take control; Enter: freeze light  Esc: back" : "Arrows: move light; Enter: animate  Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, screen);
    }
}

void freak_demo_environment3d(void)
{
    unsigned long tick;
    vec3 direction;
    uint8_t face;
    uint8_t u;
    uint8_t v;
    int x;
    int y;
    int rotation;
    tick = 0; rotation = 0; freak_demo_palette_default();
    freak_palette_preset(128, 32, FREAK_PALETTE_WATER,
                         FREAK_PALETTE_WATER_COUNT);
    freak_palette_preset(160, 32, FREAK_PALETTE_FOREST,
                         FREAK_PALETTE_FOREST_COUNT);
    freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        ++tick;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_LEFT)) --rotation;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_RIGHT)) ++rotation;
        freak_clear_buffer(0, screen);
        draw_sphere(92, 110, 76, (int)(tick + rotation * 16), 1);
        draw_sphere(228, 110, 60, (int)(tick + 81 + rotation * 16), 2);
        for (x = 0; x < 320; ++x) {
            direction.x = fixed_div(int_to_fixed(x - 160), int_to_fixed(160));
            direction.y = fixed_sin_interp((tick * 3) & 1023) / 3;
            direction.z = FIXED_ONE;
            face = freak_shade3d_cube_uv(&direction, &u, &v);
            for (y = 190; y < 200; ++y) screen[y * 320 + x] = (uint8_t)(64 + face * 8 + (u >> 5));
        }
        freak_font_text(screen, 8, 8, &font, "Environment mapping: chrome reflects sky, forest, water and sun", 15);
        freak_font_text(screen, 8, 18, &font, "Left/right: rotate lookup; left is chrome, right is water  Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, screen);
    }
}

void freak_demo_glenz3d(void)
{
    uint8_t *table;
    uint8_t *glow;
    unsigned int tick;
    table = (uint8_t *)malloc(FREAK_BLEND_TABLE_SIZE);
    glow = (uint8_t *)malloc(FREAK_BLEND_TABLE_SIZE);
    if (!table || !glow) { if (table) free(table); if (glow) free(glow); return; }
    freak_blend_table_build(table, FREAK_BLEND_TRANSLUCENT);
    freak_blend_table_build(glow, FREAK_BLEND_ADDITIVE);
    tick = 0; freak_demo_palette_default(); freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        ++tick; freak_clear_buffer(8, screen);
        freak_tri_c(30, 170, 160, 25 + (tick & 31), 285, 170, 72, screen);
        freak_blend_rect(screen, 55 + (tick & 31), 55, 230 + (tick & 31), 145, 112, table);
        freak_blend_rect(screen, 90, 45 + ((tick >> 1) & 31), 255, 125 + ((tick >> 1) & 31), 136, table);
        freak_blend_rect(screen, 145, 75, 175, 105, 180, glow);
        freak_font_text(screen, 8, 8, &font, "Glenz: ordered translucent polygons + additive glow", 15);
        freak_font_text(screen, 8, 18, &font, "Glass/refraction lives in the distortion effect demo  Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, screen);
    }
    free(table);
    free(glow);
}
