#include <stdio.h>
#include "democom.h"
#define DEMO_CUBE_FLAT 4
#define DEMO_CUBE_GOURAUD 5
#define DEMO_CUBE_TEXTURED 6
#define DEMO_CUBE_PACING 7

static const int cube_faces[6][4] = {
    {0, 1, 2, 3}, {4, 7, 6, 5}, {0, 4, 5, 1},
    {3, 2, 6, 7}, {0, 3, 7, 4}, {1, 5, 6, 2}
};
static const int cube_uv[4][2] = {{0, 0}, {255, 0}, {255, 255}, {0, 255}};
void freak_demo_cube(int mode)
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
    freak_demo_palette_default();
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

