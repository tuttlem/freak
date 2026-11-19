#include "democom.h"

static void draw_world_point(const struct freak_camera *camera, const vec3 *world, uint8_t colour)
{
    struct freak_raster3d_vertex point;
    int x;
    int y;
    freak_camera_world_to_view(camera, world, &point.position);
    if (point.position.z <= FREAK_RASTER3D_NEAR) return;
    freak_raster3d_project(&point, 160, 100, int_to_fixed(80), &x, &y, 0);
    freak_fill_rect(screen, x - 2, y - 2, x + 2, y + 2, colour);
}

void freak_demo_model3d(void)
{
    struct freak_model3d_vertex first[3];
    struct freak_model3d_vertex last[3];
    struct freak_model3d_vertex current[3];
    struct freak_model3d a;
    struct freak_model3d b;
    unsigned int tick;
    int phase;
    vec3_set(&first[0].position, int_to_fixed(-1), int_to_fixed(-1), int_to_fixed(3));
    vec3_set(&first[1].position, int_to_fixed(1), int_to_fixed(-1), int_to_fixed(3));
    vec3_set(&first[2].position, 0, int_to_fixed(1), int_to_fixed(3));
    last[0] = first[0]; last[1] = first[1]; last[2] = first[2];
    last[0].position.y = int_to_fixed(-2); last[1].position.x = int_to_fixed(2); last[2].position.y = int_to_fixed(2);
    a.vertices = first; a.vertex_count = 3; a.indices = 0; a.index_count = 0; a.material = 0;
    b = a; b.vertices = last;
    tick = 0; freak_demo_palette_default(); freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        ++tick; phase = (int)(tick & 127); if (phase > 64) phase = 128 - phase;
        freak_model3d_interpolate(&a, &b, fixed_div(int_to_fixed(phase), int_to_fixed(64)), current, 3);
        freak_clear_buffer(1, screen);
        freak_tri_g(160 + fixed_to_int(current[0].position.x * 60), 100 - fixed_to_int(current[0].position.y * 45),
                    160 + fixed_to_int(current[1].position.x * 60), 100 - fixed_to_int(current[1].position.y * 45),
                    160 + fixed_to_int(current[2].position.x * 60), 100 - fixed_to_int(current[2].position.y * 45), 64, 96, 128, screen);
        freak_font_text(screen, 8, 8, &font, "Model pipeline: position, normal, UV, material and keyframe blend", 15);
        freak_font_text(screen, 8, 18, &font, "tools/obj2f3m.py converts triangulated OBJ into compact F3M1  Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, screen);
    }
}

void freak_demo_camera3d(void)
{
    struct freak_camera camera;
    struct freak_camera target;
    struct freak_camera_spring spring;
    struct freak_camera_rail_key rail[3];
    vec3 points[4];
    vec3 focus;
    unsigned long tick;
    unsigned int i;
    vec3_set(&points[0], int_to_fixed(-2), 0, int_to_fixed(5)); vec3_set(&points[1], int_to_fixed(2), 0, int_to_fixed(5));
    vec3_set(&points[2], 0, int_to_fixed(2), int_to_fixed(6)); vec3_set(&points[3], 0, int_to_fixed(-2), int_to_fixed(6));
    vec3_set(&rail[0].position, 0, 0, 0); rail[0].yaw = 0; rail[0].pitch = 0;
    vec3_set(&rail[1].position, int_to_fixed(-1), int_to_fixed(1), int_to_fixed(1)); rail[1].yaw = 0; rail[1].pitch = 0;
    vec3_set(&rail[2].position, int_to_fixed(1), 0, int_to_fixed(2)); rail[2].yaw = 0; rail[2].pitch = 0;
    freak_camera_init(&camera); freak_camera_init(&target); vec3_zero(&spring.position_velocity); spring.yaw_velocity = 0; spring.pitch_velocity = 0;
    vec3_set(&focus, 0, 0, int_to_fixed(5));
    tick = 0; freak_demo_palette_default(); freak_input_clear_events();
    while (1) {
        freak_input_update(); if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        ++tick;
        if (((tick >> 7) % 3) == 0)
            freak_camera_rail(&target, rail, 3, fixed_div(int_to_fixed((int)(tick & 255)), int_to_fixed(128)));
        else if (((tick >> 7) % 3) == 1)
            freak_camera_orbit(&target, &focus, (tick * FIXED_ONE) / 4, FIXED_ONE / 12, int_to_fixed(3));
        else
            freak_camera_chase(&target, &focus, (tick * FIXED_ONE) / 4, FIXED_ONE, int_to_fixed(3));
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_UP))
            freak_camera_fps_move(&target, FIXED_ONE / 16, 0, 0);
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_LEFT))
            freak_camera_fps_move(&target, 0, -FIXED_ONE / 16, 0);
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_RIGHT))
            freak_camera_fps_move(&target, 0, FIXED_ONE / 16, 0);
        freak_camera_shake(&target, FIXED_ONE / 32, tick);
        freak_camera_smooth_to(&camera, &spring, &target, FIXED_ONE / 8, int_to_fixed(3) / 4);
        freak_clear_buffer(1, screen); for (i = 0; i < 4; ++i) draw_world_point(&camera, &points[i], (uint8_t)(64 + i * 16));
        freak_font_text(screen, 8, 8, &font, "Camera toolkit: FPS, orbit, chase, rail, shake, smooth transitions", 15);
        freak_font_text(screen, 8, 18, &font, "Rail/orbit/chase cycle; arrows add FPS motion  Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, screen);
    }
}
