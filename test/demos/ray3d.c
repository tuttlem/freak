#include "democom.h"

static uint8_t maze[12 * 12] = {
    1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,0,0,0,0,1,
    1,0,1,1,0,1,1,1,0,1,0,1,
    1,0,0,1,0,0,0,1,0,1,0,1,
    1,1,0,1,1,1,0,1,0,1,0,1,
    1,0,0,0,0,2,0,0,0,0,0,1,
    1,0,1,1,0,1,1,1,1,1,0,1,
    1,0,1,0,0,0,0,0,0,1,0,1,
    1,0,1,0,1,1,1,1,0,1,0,1,
    1,0,0,0,1,0,0,1,0,0,0,1,
    1,1,1,0,0,0,0,1,1,1,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1
};

void freak_demo_raycast3d(void)
{
    struct freak_ray3d_map map;
    struct freak_ray3d_hit hit;
    fixed player_x;
    fixed player_y;
    fixed yaw;
    fixed angle;
    fixed direction_x;
    fixed direction_y;
    fixed move_x;
    fixed move_y;
    int x;
    int y;
    int top;
    int bottom;
    int height;
    int colour;
    map.tiles = maze; map.width = 12; map.height = 12;
    player_x = int_to_fixed(1) + FIXED_ONE / 2;
    player_y = int_to_fixed(1) + FIXED_ONE / 2;
    yaw = 0; freak_demo_palette_default(); freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_ACCEPT))
            maze[5 * 12 + 5] = maze[5 * 12 + 5] == 2 ? 0 : 2;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_LEFT)) yaw -= int_to_fixed(2);
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_RIGHT)) yaw += int_to_fixed(2);
        direction_x = fixed_sin_interp(yaw); direction_y = fixed_cos_interp(yaw);
        move_x = player_x; move_y = player_y;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_UP)) {
            move_x += direction_x / 10; move_y += direction_y / 10;
        }
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_DOWN)) {
            move_x -= direction_x / 10; move_y -= direction_y / 10;
        }
        if (!freak_ray3d_cast(&map, move_x, move_y, direction_x, direction_y, FIXED_ONE / 3, &hit)) {
            player_x = move_x; player_y = move_y;
        }
        for (y = 0; y < 100; ++y) for (x = 0; x < 320; ++x) screen[y * 320 + x] = (uint8_t)(12 + y / 12);
        for (y = 100; y < 200; ++y) for (x = 0; x < 320; ++x) screen[y * 320 + x] = (uint8_t)(24 + (199 - y) / 9);
        for (x = 0; x < 320; ++x) {
            angle = yaw + ((long)(x - 160) * FIXED_ONE * 3L) / 4L;
            direction_x = fixed_sin_interp(angle); direction_y = fixed_cos_interp(angle);
            if (!freak_ray3d_cast(&map, player_x, player_y, direction_x, direction_y, int_to_fixed(12), &hit)) continue;
            if (hit.distance < FIXED_ONE / 64) hit.distance = FIXED_ONE / 64;
            height = fixed_to_int(fixed_div(int_to_fixed(100), hit.distance));
            if (height > 199) height = 199;
            top = 100 - height / 2; bottom = 100 + height / 2;
            if (top < 0) top = 0; if (bottom > 199) bottom = 199;
            colour = 64 + hit.tile * 16 + ((hit.cell_x * 11 + hit.cell_y * 7) & 7);
            for (y = top; y <= bottom; ++y) screen[y * 320 + x] = (uint8_t)(colour + (y & 7));
        }
        /* Billboard sprite: a deliberately keyed, camera-facing marker beyond
           the nearest wall rendering pass. */
        freak_fill_rect(screen, 153, 82, 166, 130, 144);
        freak_fill_rect(screen, 156, 87, 163, 124, 160);
        freak_font_text(screen, 8, 8, &font, "Raycast maze: walls, floor/ceiling, door & billboard sprite", 15);
        freak_font_text(screen, 8, 18, &font, "Arrows: move/turn  Enter: toggle door  Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, screen);
    }
}

void freak_demo_raytrace3d(void)
{
    struct freak_raytrace_object objects[4];
    vec3 origin;
    vec3 ray;
    int x;
    int y;
    uint8_t colour;
    objects[0].kind = FREAK_RAYTRACE_SPHERE; vec3_set(&objects[0].position, int_to_fixed(-1), 0, int_to_fixed(4)); objects[0].radius = int_to_fixed(1); objects[0].colour = 64; objects[0].reflective = 1; objects[0].refractive = 0;
    objects[1].kind = FREAK_RAYTRACE_SPHERE; vec3_set(&objects[1].position, int_to_fixed(1), 0, int_to_fixed(5)); objects[1].radius = int_to_fixed(1); objects[1].colour = 96; objects[1].reflective = 0; objects[1].refractive = 1;
    objects[2].kind = FREAK_RAYTRACE_SPHERE; vec3_set(&objects[2].position, 0, int_to_fixed(1), int_to_fixed(3)); objects[2].radius = FIXED_ONE / 2; objects[2].colour = 128; objects[2].reflective = 0; objects[2].refractive = 0;
    objects[3].kind = FREAK_RAYTRACE_PLANE; vec3_set(&objects[3].position, 0, int_to_fixed(-1), 0); objects[3].radius = 0; objects[3].colour = 32; objects[3].reflective = 0; objects[3].refractive = 0;
    origin.x = 0; origin.y = 0; origin.z = 0; freak_demo_palette_default(); freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        for (y = 24; y < 200; y += 2) for (x = 0; x < 320; x += 2) {
            ray.x = fixed_div(int_to_fixed(x - 160), int_to_fixed(160));
            ray.y = fixed_div(int_to_fixed(100 - y), int_to_fixed(160)); ray.z = FIXED_ONE;
            colour = freak_raytrace_sample(objects, 4, &origin, &ray, 0);
            screen[y * 320 + x] = colour; screen[y * 320 + x + 1] = colour;
            screen[(y + 1) * 320 + x] = colour; screen[(y + 1) * 320 + x + 1] = colour;
        }
        freak_font_text(screen, 8, 8, &font, "Ray tracer: spheres, plane and hard shadows", 15);
        freak_font_text(screen, 8, 18, &font, "Safe realtime primary pass; API supports reflection/refraction  Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, screen);
    }
}
