#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "../src/freak.h"

#define DEMO_FLAT_TRIANGLES    1
#define DEMO_GOURAUD_TRIANGLES 2
#define DEMO_TEXTURE_TRIANGLES 3
#define DEMO_CUBE_FLAT         4
#define DEMO_CUBE_GOURAUD      5
#define DEMO_CUBE_TEXTURED     6
#define DEMO_CUBE_PACING       7
#define DEMO_TEXTURE_GALLERY   8
#define DEMO_UTILITIES         9
#define DEMO_UI               10
#define DEMO_2D_SPRITES       11
#define DEMO_2D_TILEMAP       12
#define DEMO_QUIT             13

extern int freak_demo_texture_gallery(void);
extern int freak_demo_utilities(void);
extern int freak_demo_ui(void);

static const struct freak_menu_item rendering_items[] = {
    {"Flat triangles", DEMO_FLAT_TRIANGLES, 0, 1},
    {"Gouraud triangles", DEMO_GOURAUD_TRIANGLES, 0, 1},
    {"Textured triangles", DEMO_TEXTURE_TRIANGLES, 0, 1},
    {"Spinning cube (flat)", DEMO_CUBE_FLAT, 0, 1},
    {"Spinning cube (Gouraud)", DEMO_CUBE_GOURAUD, 0, 1},
    {"Spinning cube (textured)", DEMO_CUBE_TEXTURED, 0, 1},
    {"Cube frame pacing", DEMO_CUBE_PACING, 0, 1},
    {"Back", FREAK_MENU_BACK, 0, 1}
};
static const struct freak_menu rendering_menu = {"Rendering", rendering_items, 8};
static const struct freak_menu_item texture_items[] = {
    {"Noise, plasma & fire", DEMO_TEXTURE_GALLERY, 0, 1},
    {"Back", FREAK_MENU_BACK, 0, 1}
};
static const struct freak_menu texture_menu = {"Textures", texture_items, 2};
static const struct freak_menu_item utility_items[] = {
    {"Sprites, fonts & transitions", DEMO_UTILITIES, 0, 1},
    {"UI controls", DEMO_UI, 0, 1},
    {"Back", FREAK_MENU_BACK, 0, 1}
};
static const struct freak_menu utility_menu = {"Utilities", utility_items, 3};
static const struct freak_menu_item two_d_items[] = {
    {"Camera & animated sprites", DEMO_2D_SPRITES, 0, 1},
    {"Scrolling tile map", DEMO_2D_TILEMAP, 0, 1},
    {"Back", FREAK_MENU_BACK, 0, 1}
};
static const struct freak_menu two_d_menu = {"2D", two_d_items, 2};
static const struct freak_menu_item main_items[] = {
    {"2D", 0, &two_d_menu, 1},
    {"Rendering", 0, &rendering_menu, 1},
    {"Textures", 0, &texture_menu, 1},
    {"Utilities", 0, &utility_menu, 1},
    {"Quit", DEMO_QUIT, 0, 1}
};
static const struct freak_menu main_menu = {"Freak demo", main_items, 5};

static unsigned char *screen;
static struct freak_arena demo_arena;
static struct freak_font font = {(const unsigned char *)0x000FFA6EL, 8, 8, 0, 128};
static struct freak_menu_style menu_style = {1, 15, 15, 1, 12, 8, 6, 10};
static struct freak_timing demo_timing;
static const unsigned char runner_bits[4][64] = {
    {0,0,0,15,15,0,0,0, 0,0,15,80,80,15,0,0,
     0,15,80,15,15,80,15,0, 0,15,15,80,80,15,15,0,
     0,0,15,80,80,15,0,0, 0,15,15,0,0,15,15,0,
     15,0,0,0,0,0,0,15, 0,0,0,0,0,0,0,0},
    {0,0,0,15,15,0,0,0, 0,0,15,80,80,15,0,0,
     0,15,80,15,15,80,15,0, 0,15,15,80,80,15,15,0,
     0,0,15,80,80,15,0,0, 0,0,15,0,15,0,0,0,
     0,15,0,0,0,0,15,0, 0,0,0,0,0,0,0,0},
    {0,0,0,15,15,0,0,0, 0,0,15,80,80,15,0,0,
     0,15,80,15,15,80,15,0, 0,15,15,80,80,15,15,0,
     0,0,15,80,80,15,0,0, 0,15,15,0,0,15,15,0,
     0,0,0,15,15,0,0,0, 0,0,0,0,0,0,0,0},
    {0,0,0,15,15,0,0,0, 0,0,15,80,80,15,0,0,
     0,15,80,15,15,80,15,0, 0,15,15,80,80,15,15,0,
     0,0,15,80,80,15,0,0, 0,0,15,0,15,0,0,0,
     15,0,0,0,0,0,0,15, 0,0,0,0,0,0,0,0}
};
static const struct freak_sprite runner_frames[4] = {
    {runner_bits[0], 8, 8, 0}, {runner_bits[1], 8, 8, 0},
    {runner_bits[2], 8, 8, 0}, {runner_bits[3], 8, 8, 0}
};
static const unsigned char tile_bits[3][64] = {
    {48,48,48,48,48,48,48,48, 48,52,48,48,48,48,52,48,
     48,48,48,52,48,48,48,48, 52,48,48,48,48,52,48,48,
     48,48,52,48,48,48,48,52, 48,48,48,48,52,48,48,48,
     48,52,48,48,48,48,52,48, 48,48,48,52,48,48,48,48},
    {96,96,100,96,96,96,100,96, 96,100,96,96,100,96,96,96,
     100,96,96,100,96,96,96,100, 96,96,100,96,96,100,96,96,
     96,100,96,96,100,96,96,96, 100,96,96,100,96,96,96,100,
     96,96,100,96,96,100,96,96, 96,100,96,96,100,96,96,96},
    {0,0,0,0,0,0,0,0, 0,0,32,32,32,32,0,0,
     0,32,36,36,36,36,32,0, 32,36,36,36,36,36,36,32,
     32,32,32,32,32,32,32,32, 0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0}
};
static const struct freak_sprite tile_sprites[3] = {
    {tile_bits[0], 8, 8, 0}, {tile_bits[1], 8, 8, 0},
    {tile_bits[2], 8, 8, 0}
};

static void set_demo_palette(void)
{
    freak_palette_greyscale(0, 64, 0, 63);
    freak_palette_fire(64, 32);
    freak_palette_sky(96, 32);
    freak_set_palette(15, 63, 63, 63);
}

static void draw_menu_centered(const struct freak_menu_stack *menu)
{
    const struct freak_menu *active;
    struct freak_text_size size;
    int index;
    int columns;
    int width;
    int height;

    active = menu->menus[menu->depth];
    size = freak_font_measure(&font, active->title);
    columns = size.width / font.width;
    for (index = 0; index < active->count; ++index) {
        size = freak_font_measure(&font, active->items[index].label);
        if (size.width / font.width > columns)
            columns = size.width / font.width;
    }
    width = columns * font.width + menu_style.padding * 2;
    height = (active->count + 1) * menu_style.row_height +
             menu_style.padding * 2;
    freak_menu_draw(screen, (320 - width) / 2, (200 - height) / 2,
                    &font, menu, &menu_style);
}

static int menu_item_at(const struct freak_menu_stack *menu, int mouse_x,
                        int mouse_y)
{
    const struct freak_menu *active;
    struct freak_text_size size;
    int index;
    int columns;
    int width;
    int height;
    int left;
    int top;
    int item;

    active = menu->menus[menu->depth];
    size = freak_font_measure(&font, active->title);
    columns = size.width / font.width;
    for (index = 0; index < active->count; ++index) {
        size = freak_font_measure(&font, active->items[index].label);
        if (size.width / font.width > columns)
            columns = size.width / font.width;
    }
    width = columns * font.width + menu_style.padding * 2;
    height = (active->count + 1) * menu_style.row_height +
             menu_style.padding * 2;
    left = (320 - width) / 2;
    top = (200 - height) / 2;
    if (mouse_x < left + menu_style.padding ||
        mouse_x >= left + width - menu_style.padding)
        return -1;
    item = (mouse_y - top - menu_style.padding) / menu_style.row_height - 1;
    if (item < 0 || item >= active->count || !active->items[item].enabled)
        return -1;
    return item;
}

static void run_triangles(int mode)
{
    struct freak_texture texture;
    unsigned int tick;
    int x;
    int y;
    int have_texture;

    have_texture = 0;
    if (mode == DEMO_TEXTURE_TRIANGLES) {
        have_texture = freak_texture_create(&texture);
        if (have_texture) {
            for (y = 0; y < 256; ++y)
                for (x = 0; x < 256; ++x)
                    texture.pixels[x + (y << 8)] = 64 + ((x ^ y) & 31);
        }
    }
    set_demo_palette();
    tick = 0;
    freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings,
                                FREAK_ACTION_BACK))
            break;
        freak_clear_buffer(0, screen);
        if (mode == DEMO_FLAT_TRIANGLES)
            freak_tri_c(30 + (tick % 180), 30, 290, 90,
                        100, 185, 72 + (tick & 15), screen);
        else if (mode == DEMO_GOURAUD_TRIANGLES)
            freak_tri_g(30 + (tick % 180), 30, 290, 90, 100, 185,
                        64, 95, 80, screen);
        else if (have_texture)
            freak_tri_t(30 + (tick % 180), 30, 290, 90, 100, 185,
                        0, 0, 255, 0, 64, 255, &texture, screen);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
        tick += (unsigned int)freak_timing_update(&demo_timing);
    }
    if (have_texture)
        freak_texture_destroy(&texture);
}

static const int cube_faces[6][4] = {
    {0, 1, 2, 3}, {4, 7, 6, 5}, {0, 4, 5, 1},
    {3, 2, 6, 7}, {0, 3, 7, 4}, {1, 5, 6, 2}
};
static const int cube_uv[4][2] = {{0, 0}, {255, 0}, {255, 255}, {0, 255}};

static void run_cube(int mode)
{
    vec3 base[8];
    vec3 rotated[8];
    int px[8], py[8];
    fixed sx, cx, sy, cy;
    fixed x, y, z;
    fixed face_depth[6];
    struct freak_texture texture;
    unsigned int tick;
    unsigned long elapsed;
    struct freak_fixed_step simulation;
    struct freak_frame_pacer pacer;
    struct freak_profiler profiler;
    struct freak_profile_scope update_profile;
    struct freak_profile_scope render_profile;
    int face_order[6];
    int index, face, sort, swap, a, b, c, d;
    int have_texture;
    int paced;
    int target_fps;
    char pacing_text[80];
    char profile_text[80];

    vec3_set(&base[0], -FIXED_ONE, -FIXED_ONE, -FIXED_ONE);
    vec3_set(&base[1],  FIXED_ONE, -FIXED_ONE, -FIXED_ONE);
    vec3_set(&base[2],  FIXED_ONE,  FIXED_ONE, -FIXED_ONE);
    vec3_set(&base[3], -FIXED_ONE,  FIXED_ONE, -FIXED_ONE);
    vec3_set(&base[4], -FIXED_ONE, -FIXED_ONE,  FIXED_ONE);
    vec3_set(&base[5],  FIXED_ONE, -FIXED_ONE,  FIXED_ONE);
    vec3_set(&base[6],  FIXED_ONE,  FIXED_ONE,  FIXED_ONE);
    vec3_set(&base[7], -FIXED_ONE,  FIXED_ONE,  FIXED_ONE);
    have_texture = 0;
    paced = mode == DEMO_CUBE_PACING;
    target_fps = 60;
    if (mode == DEMO_CUBE_TEXTURED) {
        have_texture = freak_texture_create(&texture);
        if (have_texture)
            for (index = 0; index < 65536; ++index)
                texture.pixels[index] = 64 + (((index & 255) >> 4) ^ ((index >> 12) & 15));
    }
    set_demo_palette();
    tick = 0;
    freak_fixed_step_init(&simulation, 2, 8);
    if (paced)
        freak_frame_pacer_init(&pacer, 120, (unsigned int)target_fps);
    freak_profiler_init(&profiler);
    freak_profile_scope_reset(&update_profile);
    freak_profile_scope_reset(&render_profile);
    freak_input_clear_events();
    while (1) {
        if (paced) {
            freak_profiler_begin_frame(&profiler);
            freak_profile_scope_begin(&update_profile);
        }
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings,
                                FREAK_ACTION_BACK))
            break;
        if (paced && freak_input_key_pressed(FREAK_KEY_LEFT)) {
            target_fps = target_fps == 60 ? 30 : 15;
            freak_frame_pacer_set_fps(&pacer, (unsigned int)target_fps);
        }
        if (paced && freak_input_key_pressed(FREAK_KEY_RIGHT)) {
            target_fps = target_fps == 15 ? 30 : 60;
            freak_frame_pacer_set_fps(&pacer, (unsigned int)target_fps);
        }
        elapsed = freak_timing_update(&demo_timing);
        freak_fixed_step_add(&simulation, elapsed);
        while (freak_fixed_step_take(&simulation))
            ++tick;
        sx = fixed_sin((tick * 3) & 1023);
        cx = fixed_cos((tick * 3) & 1023);
        sy = fixed_sin((tick * 2) & 1023);
        cy = fixed_cos((tick * 2) & 1023);
        for (index = 0; index < 8; ++index) {
            x = fixed_mul(base[index].x, cy) + fixed_mul(base[index].z, sy);
            z = fixed_mul(base[index].z, cy) - fixed_mul(base[index].x, sy);
            y = fixed_mul(base[index].y, cx) - fixed_mul(z, sx);
            rotated[index].z = fixed_mul(base[index].y, sx) + fixed_mul(z, cx) + int_to_fixed(4);
            rotated[index].x = x;
            rotated[index].y = y;
            px[index] = 160 + fixed_to_int(fixed_div(fixed_mul(x, int_to_fixed(100)), rotated[index].z));
            py[index] = 100 + fixed_to_int(fixed_div(fixed_mul(y, int_to_fixed(100)), rotated[index].z));
        }
        if (paced) {
            freak_profile_scope_end(&update_profile);
            freak_profile_scope_begin(&render_profile);
        }
        freak_clear_buffer(0, screen);
        for (face = 0; face < 6; ++face) {
            a = cube_faces[face][0]; b = cube_faces[face][1];
            c = cube_faces[face][2]; d = cube_faces[face][3];
            face_order[face] = face;
            face_depth[face] = (rotated[a].z + rotated[b].z +
                                rotated[c].z + rotated[d].z) >> 2;
        }
        /* Painter's algorithm: draw the farthest face first. */
        for (face = 1; face < 6; ++face) {
            for (sort = face; sort > 0 &&
                 face_depth[face_order[sort]] > face_depth[face_order[sort - 1]];
                 --sort) {
                swap = face_order[sort];
                face_order[sort] = face_order[sort - 1];
                face_order[sort - 1] = swap;
            }
        }
        for (index = 0; index < 6; ++index) {
            face = face_order[index];
            a = cube_faces[face][0]; b = cube_faces[face][1];
            c = cube_faces[face][2]; d = cube_faces[face][3];
            if (mode == DEMO_CUBE_FLAT || paced) {
                freak_quad_c(px[a], py[a], px[b], py[b], px[c], py[c], px[d], py[d],
                             64 + face * 5, screen);
            } else if (mode == DEMO_CUBE_GOURAUD) {
                freak_quad_g(px[a], py[a], px[b], py[b], px[c], py[c], px[d], py[d],
                             64 + (a << 2), 64 + (b << 2), 64 + (c << 2),
                             64 + (d << 2), screen);
            } else if (have_texture) {
                freak_tri_pt(px[a], py[a], rotated[a].z, cube_uv[0][0], cube_uv[0][1],
                             px[b], py[b], rotated[b].z, cube_uv[1][0], cube_uv[1][1],
                             px[c], py[c], rotated[c].z, cube_uv[2][0], cube_uv[2][1],
                             &texture, screen);
                freak_tri_pt(px[a], py[a], rotated[a].z, cube_uv[0][0], cube_uv[0][1],
                             px[c], py[c], rotated[c].z, cube_uv[2][0], cube_uv[2][1],
                             px[d], py[d], rotated[d].z, cube_uv[3][0], cube_uv[3][1],
                             &texture, screen);
            }
        }
        if (paced) {
            freak_profile_scope_end(&render_profile);
            sprintf(pacing_text, "Frame pacing: %d FPS  measured: %d", target_fps,
                    freak_frame_pacer_fps(&pacer));
            sprintf(profile_text, "Update: %lu us  Render: %lu us  Frame: %lu us",
                    freak_profile_microseconds(update_profile.last_cycles),
                    freak_profile_microseconds(render_profile.last_cycles),
                    freak_profile_microseconds(profiler.last_frame_cycles));
            freak_font_text(screen, 8, 8, &font, pacing_text, 15);
            freak_font_text(screen, 8, 18, &font, "Left/right: change rate  Esc: back", 15);
            freak_font_text(screen, 8, 28, &font, profile_text, 15);
        }
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
        if (paced)
            freak_frame_pacer_wait(&pacer);
        if (paced)
            freak_profiler_end_frame(&profiler);
    }
    if (have_texture)
        freak_texture_destroy(&texture);
}

static void run_2d_sprites(void)
{
    struct freak_camera_2d camera;
    struct freak_sprite_2d runner;
    struct freak_sprite_2d trail;
    struct freak_sprite_animation animation;
    struct freak_sprite_batch batch;
    unsigned long tick;
    unsigned long elapsed;
    int world_x;
    int grid;
    int screen_x;
    int screen_y;

    freak_sprite_animation_init(&animation, runner_frames, 4, 12);
    runner.x = 0;
    runner.y = 96;
    runner.keyed = 1;
    runner.flip_x = 0;
    tick = 0;
    freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings,
                                FREAK_ACTION_BACK))
            break;
        elapsed = freak_timing_update(&demo_timing);
        tick += elapsed;
        runner.x = (int)(tick % 960UL);
        runner.y = 96 + fixed_to_int(fixed_mul(fixed_sin((tick * 8UL) & 1023),
                                                int_to_fixed(24)));
        runner.flip_x = ((tick / 480UL) & 1U) != 0;
        freak_camera_2d_set(&camera, runner.x - 160, 0);
        freak_sprite_animation_advance(&animation, elapsed);
        runner.sprite = freak_sprite_animation_frame(&animation);
        freak_sprite_batch_reset(&batch);

        freak_clear_buffer(0, screen);
        for (world_x = (camera.x / 32) * 32 - 32;
             world_x < camera.x + 352; world_x += 32) {
            freak_camera_2d_world_to_screen(&camera, world_x, 0,
                                             &screen_x, &screen_y);
            freak_draw_line(screen, screen_x, 0, screen_x, 199, 20);
        }
        for (grid = 8; grid < 200; grid += 32)
            freak_draw_line(screen, 0, grid, 319, grid, 20);
        for (world_x = (camera.x / 64) * 64 - 64;
             world_x < camera.x + 384; world_x += 64) {
            freak_camera_2d_world_to_screen(&camera, world_x, 160,
                                             &screen_x, &screen_y);
            freak_fill_rect(screen, screen_x, screen_y,
                            screen_x + 63, screen_y + 15, 32);
        }
        /* Submit in reverse depth order; the batch resolves the draw order. */
        for (grid = 4; grid >= 0; --grid) {
            trail = runner;
            trail.x -= grid * 14;
            trail.y += grid * 3;
            freak_sprite_batch_submit(&batch, &trail, 1, trail.y);
        }
        freak_sprite_batch_draw(screen, &camera, &batch);
        freak_font_text(screen, 8, 8, &font,
                        "2D camera + animated sprite  Esc: back", 15);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
    }
}

static void run_tilemap(void)
{
    struct freak_tilemap map;
    struct freak_tile_atlas atlas;
    struct freak_camera_2d camera;
    unsigned long last_tick;
    unsigned long now;
    unsigned long elapsed;
    int x;
    int y;

    if (!freak_tilemap_init(&map, 96, 32, 8, 8, 2, 1))
        return;
    atlas.tiles = tile_sprites;
    atlas.count = 3;
    for (y = 0; y < map.height; ++y) {
        for (x = 0; x < map.width; ++x) {
            freak_tilemap_set(&map, 0, (uint16_t)x, (uint16_t)y,
                              (uint16_t)(((x / 4 + y / 3) & 1) ? 0 : 1));
            if (y == 22 || (y == 17 && (x % 12) < 6)) {
                freak_tilemap_set(&map, 1, (uint16_t)x, (uint16_t)y, 2);
                freak_tilemap_set_collision(&map, (uint16_t)x, (uint16_t)y, 1);
            }
        }
    }
    freak_camera_2d_set(&camera, 0, 0);
    last_tick = freak_timer_ticks();
    freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings,
                                FREAK_ACTION_BACK))
            break;
        now = freak_timer_ticks();
        elapsed = now - last_tick;
        last_tick = now;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_LEFT))
            camera.x -= (int)elapsed * 2;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_RIGHT))
            camera.x += (int)elapsed * 2;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_UP))
            camera.y -= (int)elapsed * 2;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_DOWN))
            camera.y += (int)elapsed * 2;
        if (camera.x < 0) camera.x = 0;
        if (camera.y < 0) camera.y = 0;
        if (camera.x > map.width * map.tile_width - 320)
            camera.x = map.width * map.tile_width - 320;
        if (camera.y > map.height * map.tile_height - 200)
            camera.y = map.height * map.tile_height - 200;
        set_demo_palette();
        freak_clear_buffer(0, screen);
        freak_tilemap_draw(screen, &camera, &map, &atlas, 0);
        freak_tilemap_draw(screen, &camera, &map, &atlas, 1);
        freak_font_text(screen, 8, 8, &font,
                        "Scrolling tile map  Arrows: scroll  Esc: back", 15);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
    }
    freak_tilemap_destroy(&map);
}

static void launch_demo(int action, int *running)
{
    if (action == DEMO_FLAT_TRIANGLES || action == DEMO_GOURAUD_TRIANGLES ||
        action == DEMO_TEXTURE_TRIANGLES) {
        freak_mouse_hide();
        run_triangles(action);
        freak_mouse_show();
        freak_input_clear_events();
    } else if (action == DEMO_CUBE_FLAT || action == DEMO_CUBE_GOURAUD ||
               action == DEMO_CUBE_TEXTURED || action == DEMO_CUBE_PACING) {
        freak_mouse_hide();
        run_cube(action);
        freak_mouse_show();
        freak_input_clear_events();
    } else if (action == DEMO_TEXTURE_GALLERY) {
        freak_mouse_hide();
        freak_demo_texture_gallery();
        freak_mouse_show();
        freak_input_clear_events();
    } else if (action == DEMO_UTILITIES) {
        freak_mouse_hide();
        freak_demo_utilities();
        freak_mouse_show();
        freak_input_clear_events();
    } else if (action == DEMO_UI) {
        freak_mouse_hide();
        freak_demo_ui();
        freak_mouse_show();
        freak_input_clear_events();
    } else if (action == DEMO_2D_SPRITES) {
        freak_mouse_hide();
        run_2d_sprites();
        freak_mouse_show();
        freak_input_clear_events();
    } else if (action == DEMO_2D_TILEMAP) {
        freak_mouse_hide();
        run_tilemap();
        freak_mouse_show();
        freak_input_clear_events();
    } else if (action == DEMO_QUIT) {
        *running = 0;
    }
}

int main(void)
{
    struct freak_menu_stack menu;
    struct freak_input_event event;
    int action;
    int hovered_item;
    int running;

    if (!freak_arena_init(&demo_arena, 64000U))
        return 1;
    screen = (unsigned char *)freak_arena_alloc(&demo_arena, 64000U);
    if (!screen) {
        freak_arena_destroy(&demo_arena);
        return 1;
    }
    freak_set_mcga();
    if (!freak_timing_start(&demo_timing, 120)) {
        freak_set_text();
        freak_arena_destroy(&demo_arena);
        return 1;
    }
    freak_input_init();
    freak_input_update();
    freak_menu_open(&menu, &main_menu);
    running = 1;
    while (running) {
        freak_input_update();
        freak_timing_update(&demo_timing);
        set_demo_palette();
        freak_clear_buffer(0, screen);
        draw_menu_centered(&menu);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
        while (freak_input_poll_event(&event)) {
            if (event.type == FREAK_EVENT_MOUSE_MOVE) {
                hovered_item = menu_item_at(&menu, event.mouse_x, event.mouse_y);
                if (hovered_item >= 0)
                    menu.selected[menu.depth] = hovered_item;
            } else if (event.type == FREAK_EVENT_ACTION_PRESSED) {
                if (event.action == FREAK_ACTION_UP)
                    freak_menu_move(&menu, -1);
                else if (event.action == FREAK_ACTION_DOWN)
                    freak_menu_move(&menu, 1);
                else if (event.action == FREAK_ACTION_ACCEPT) {
                    action = freak_menu_accept(&menu);
                    launch_demo(action, &running);
                } else if (event.action == FREAK_ACTION_BACK &&
                           freak_menu_back(&menu) == -1) {
                    running = 0;
                }
            } else if (event.type == FREAK_EVENT_MOUSE_BUTTON_DOWN &&
                       event.button == FREAK_MOUSE_LEFT) {
                hovered_item = menu_item_at(&menu, event.mouse_x, event.mouse_y);
                if (hovered_item >= 0) {
                    menu.selected[menu.depth] = hovered_item;
                    action = freak_menu_accept(&menu);
                    launch_demo(action, &running);
                }
            }
        }
    }
    freak_input_shutdown();
    freak_timing_stop();
    freak_set_text();
    freak_arena_destroy(&demo_arena);
    return 0;
}
