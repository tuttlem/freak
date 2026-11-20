#include <malloc.h>

#include "democom.h"

static uint8_t normal_map[196608UL];
static int bump_heights[65536UL];
static const int light_cube_faces[6][4] = {
    {0, 1, 2, 3}, {4, 7, 6, 5}, {0, 4, 5, 1},
    {3, 2, 6, 7}, {0, 3, 7, 4}, {1, 5, 6, 2}
};

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
    vec3 base[8];
    vec3 rotated[8];
    vec3 normal;
    vec3 world_position;
    struct freak_light3d light;
    fixed sx;
    fixed cx;
    fixed sy;
    fixed cy;
    fixed x;
    fixed y;
    fixed z;
    fixed depths[6];
    unsigned long last;
    unsigned long now;
    unsigned long tick;
    int order[6];
    int px[8];
    int py[8];
    uint8_t colour[8];
    int index;
    int face;
    int sort;
    int swap;
    int a;
    int b;
    int c;
    int d;
    int mid_x;
    int mid_y;
    uint8_t mid_colour;
    vec3_set(&base[0], -FIXED_ONE, -FIXED_ONE, -FIXED_ONE);
    vec3_set(&base[1],  FIXED_ONE, -FIXED_ONE, -FIXED_ONE);
    vec3_set(&base[2],  FIXED_ONE,  FIXED_ONE, -FIXED_ONE);
    vec3_set(&base[3], -FIXED_ONE,  FIXED_ONE, -FIXED_ONE);
    vec3_set(&base[4], -FIXED_ONE, -FIXED_ONE,  FIXED_ONE);
    vec3_set(&base[5],  FIXED_ONE, -FIXED_ONE,  FIXED_ONE);
    vec3_set(&base[6],  FIXED_ONE,  FIXED_ONE,  FIXED_ONE);
    vec3_set(&base[7], -FIXED_ONE,  FIXED_ONE,  FIXED_ONE);
    light.type = FREAK_LIGHT3D_POINT; light.intensity = FIXED_ONE;
    light.range = int_to_fixed(6);
    freak_demo_palette_default();
    freak_palette_greyscale(64, 32, 4, 63);
    freak_set_palette(95, 63, 63, 63);
    last = freak_timer_ticks(); tick = 0;
    freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        now = freak_timer_ticks(); tick += now - last; last = now;
        sx = fixed_sin((unsigned int)(tick * 3UL) & 1023U); cx = fixed_cos((unsigned int)(tick * 3UL) & 1023U);
        sy = fixed_sin((unsigned int)(tick * 2UL) & 1023U); cy = fixed_cos((unsigned int)(tick * 2UL) & 1023U);
        /* Camera-facing, upper-right key light: it shines diagonally down
           over the cube as its faces rotate through the beam. */
        light.position.x = int_to_fixed(3);
        light.position.y = int_to_fixed(3);
        light.position.z = int_to_fixed(-3);
        for (index = 0; index < 8; ++index) {
            x = fixed_mul(base[index].x, cy) + fixed_mul(base[index].z, sy);
            z = fixed_mul(base[index].z, cy) - fixed_mul(base[index].x, sy);
            y = fixed_mul(base[index].y, cx) - fixed_mul(z, sx);
            rotated[index].x = x; rotated[index].y = y;
            rotated[index].z = fixed_mul(base[index].y, sx) + fixed_mul(z, cx) + int_to_fixed(4);
            world_position = rotated[index]; world_position.z -= int_to_fixed(4);
            normal = world_position; vec3_normalize(&normal);
            colour[index] = (uint8_t)(64 + ((unsigned int)freak_shade3d_lambert(
                &world_position, &normal, &light, 72, 64) * 31U) / 255U);
            px[index] = 160 + fixed_to_int(fixed_div(fixed_mul(rotated[index].x, int_to_fixed(85)), rotated[index].z));
            py[index] = 108 - fixed_to_int(fixed_div(fixed_mul(rotated[index].y, int_to_fixed(85)), rotated[index].z));
        }
        freak_clear_buffer(0, screen);
        for (face = 0; face < 6; ++face) {
            a = light_cube_faces[face][0]; b = light_cube_faces[face][1];
            c = light_cube_faces[face][2]; d = light_cube_faces[face][3];
            order[face] = face; depths[face] = (rotated[a].z + rotated[b].z + rotated[c].z + rotated[d].z) >> 2;
        }
        for (face = 1; face < 6; ++face) for (sort = face; sort > 0 &&
             depths[order[sort]] > depths[order[sort - 1]]; --sort) {
            swap = order[sort]; order[sort] = order[sort - 1]; order[sort - 1] = swap;
        }
        for (index = 0; index < 6; ++index) {
            face = order[index]; a = light_cube_faces[face][0]; b = light_cube_faces[face][1];
            c = light_cube_faces[face][2]; d = light_cube_faces[face][3];
            /* Four triangles per face: this is still Gouraud shading, but
               additional lighting samples make the radial falloff readable. */
            mid_x = (px[a] + px[b] + px[c] + px[d]) >> 2;
            mid_y = (py[a] + py[b] + py[c] + py[d]) >> 2;
            world_position.x = (rotated[a].x + rotated[b].x + rotated[c].x + rotated[d].x) >> 2;
            world_position.y = (rotated[a].y + rotated[b].y + rotated[c].y + rotated[d].y) >> 2;
            world_position.z = ((rotated[a].z + rotated[b].z + rotated[c].z + rotated[d].z) >> 2) - int_to_fixed(4);
            normal = world_position; vec3_normalize(&normal);
            mid_colour = (uint8_t)(64 + ((unsigned int)freak_shade3d_lambert(
                &world_position, &normal, &light, 72, 64) * 31U) / 255U);
            freak_tri_g(px[a], py[a], px[b], py[b], mid_x, mid_y, colour[a], colour[b], mid_colour, screen);
            freak_tri_g(px[b], py[b], px[c], py[c], mid_x, mid_y, colour[b], colour[c], mid_colour, screen);
            freak_tri_g(px[c], py[c], px[d], py[d], mid_x, mid_y, colour[c], colour[d], mid_colour, screen);
            freak_tri_g(px[d], py[d], px[a], py[a], mid_x, mid_y, colour[d], colour[a], mid_colour, screen);
            freak_draw_line(screen, px[a], py[a], px[b], py[b], 15);
            freak_draw_line(screen, px[b], py[b], px[c], py[c], 15);
            freak_draw_line(screen, px[c], py[c], px[d], py[d], 15);
            freak_draw_line(screen, px[d], py[d], px[a], py[a], 15);
        }
        freak_font_text(screen, 8, 8, &font, "3D lighting: tessellated Gouraud cube, upper-right key light", 15);
        freak_font_text(screen, 8, 18, &font, "4 samples/face improve falloff; true Phong is per-pixel  Esc: back", 15);
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
    uint8_t *layer;
    vec3 base[8];
    vec3 rotated[8];
    fixed sx, cx, sy, cy, x, y, z;
    fixed depths[6];
    unsigned int tick;
    int px[8], py[8], order[6];
    int index, face, sort, swap, a, b, c, d, pixel;
    table = (uint8_t *)malloc(FREAK_BLEND_TABLE_SIZE);
    layer = (uint8_t *)malloc(64000U);
    if (!table || !layer) { if (table) free(table); if (layer) free(layer); return; }
    vec3_set(&base[0], -FIXED_ONE, -FIXED_ONE, -FIXED_ONE);
    vec3_set(&base[1],  FIXED_ONE, -FIXED_ONE, -FIXED_ONE);
    vec3_set(&base[2],  FIXED_ONE,  FIXED_ONE, -FIXED_ONE);
    vec3_set(&base[3], -FIXED_ONE,  FIXED_ONE, -FIXED_ONE);
    vec3_set(&base[4], -FIXED_ONE, -FIXED_ONE,  FIXED_ONE);
    vec3_set(&base[5],  FIXED_ONE, -FIXED_ONE,  FIXED_ONE);
    vec3_set(&base[6],  FIXED_ONE,  FIXED_ONE,  FIXED_ONE);
    vec3_set(&base[7], -FIXED_ONE,  FIXED_ONE,  FIXED_ONE);
    freak_demo_palette_default();
    freak_palette_preset(64, 32, FREAK_PALETTE_WATER, FREAK_PALETTE_WATER_COUNT);
    freak_palette_preset(96, 32, FREAK_PALETTE_ICE, FREAK_PALETTE_ICE_COUNT);
    freak_set_palette(127, 63, 63, 63);
    freak_blend_table_build(table, FREAK_BLEND_TRANSLUCENT);
    tick = 0; freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        ++tick;
        sx = fixed_sin((tick * 3U) & 1023U); cx = fixed_cos((tick * 3U) & 1023U);
        sy = fixed_sin((tick * 2U) & 1023U); cy = fixed_cos((tick * 2U) & 1023U);
        for (index = 0; index < 8; ++index) {
            x = fixed_mul(base[index].x, cy) + fixed_mul(base[index].z, sy);
            z = fixed_mul(base[index].z, cy) - fixed_mul(base[index].x, sy);
            y = fixed_mul(base[index].y, cx) - fixed_mul(z, sx);
            rotated[index].x = x; rotated[index].y = y;
            rotated[index].z = fixed_mul(base[index].y, sx) + fixed_mul(z, cx) + int_to_fixed(4);
            px[index] = 160 + fixed_to_int(fixed_div(fixed_mul(x, int_to_fixed(85)), rotated[index].z));
            py[index] = 108 - fixed_to_int(fixed_div(fixed_mul(y, int_to_fixed(85)), rotated[index].z));
        }
        freak_clear_buffer(4, screen);
        for (face = 0; face < 6; ++face) {
            a = light_cube_faces[face][0]; b = light_cube_faces[face][1]; c = light_cube_faces[face][2]; d = light_cube_faces[face][3];
            order[face] = face; depths[face] = (rotated[a].z + rotated[b].z + rotated[c].z + rotated[d].z) >> 2;
        }
        for (face = 1; face < 6; ++face) for (sort = face; sort > 0 && depths[order[sort]] > depths[order[sort - 1]]; --sort) {
            swap = order[sort]; order[sort] = order[sort - 1]; order[sort - 1] = swap;
        }
        for (index = 0; index < 6; ++index) {
            face = order[index]; a = light_cube_faces[face][0]; b = light_cube_faces[face][1]; c = light_cube_faces[face][2]; d = light_cube_faces[face][3];
            freak_clear_buffer(0, layer);
            freak_tri_c(px[a], py[a], px[b], py[b], px[c], py[c], (uint8_t)(96 + face * 4), layer);
            freak_tri_c(px[a], py[a], px[c], py[c], px[d], py[d], (uint8_t)(96 + face * 4), layer);
            for (pixel = 0; pixel < 64000; ++pixel) if (layer[pixel]) screen[pixel] = freak_blend_pixel(table, screen[pixel], layer[pixel]);
        }
        freak_font_text(screen, 8, 8, &font, "Glenz: ordered translucent rotating blue-green glass cube", 15);
        freak_font_text(screen, 8, 18, &font, "Palette-table translucency: overlapping faces create the glow  Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, screen);
    }
    free(table);
    free(layer);
}
