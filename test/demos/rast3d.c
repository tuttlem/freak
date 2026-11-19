#include "democom.h"

void freak_demo_raster3d(void)
{
    struct freak_raster3d raster;
    struct freak_raster3d_vertex triangle[3];
    struct freak_raster3d_vertex clipped[4];
    unsigned long last_tick;
    unsigned long now;
    unsigned long tick;
    int count;

    if (!freak_raster3d_init(&raster, 320, 200)) return;
    freak_demo_palette_default();
    last_tick = freak_timer_ticks(); tick = 0;
    freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        now = freak_timer_ticks(); tick += now - last_tick; last_tick = now;
        freak_clear_buffer(0, screen); freak_raster3d_clear(&raster);

        triangle[0].position.x = int_to_fixed(-2); triangle[0].position.y = int_to_fixed(-1); triangle[0].position.z = int_to_fixed(2);
        triangle[1].position.x = int_to_fixed(0); triangle[1].position.y = int_to_fixed(2); triangle[1].position.z = int_to_fixed(2);
        triangle[2].position.x = int_to_fixed(2); triangle[2].position.y = int_to_fixed(-1); triangle[2].position.z = int_to_fixed(3);
        triangle[0].colour = 80; triangle[1].colour = 120; triangle[2].colour = 104;
        freak_raster3d_triangle_gouraud(&raster, screen, &triangle[0], &triangle[1], &triangle[2], 160, 100, int_to_fixed(100));

        triangle[0].position.x = fixed_sin_interp((fixed)(tick * 11UL) & 1023) * 2; triangle[0].position.y = int_to_fixed(-1); triangle[0].position.z = int_to_fixed(1) + fixed_sin_interp((fixed)(tick * 5UL) & 1023);
        triangle[1].position.x = int_to_fixed(2); triangle[1].position.y = int_to_fixed(1); triangle[1].position.z = int_to_fixed(2);
        triangle[2].position.x = int_to_fixed(1); triangle[2].position.y = int_to_fixed(-2); triangle[2].position.z = int_to_fixed(2);
        count = freak_raster3d_clip_near(triangle, 3, clipped, FREAK_RASTER3D_NEAR);
        if (count == 3) freak_raster3d_triangle_flat(&raster, screen, &clipped[0], &clipped[1], &clipped[2], 160, 100, int_to_fixed(100), 64);
        if (count == 4) {
            freak_raster3d_triangle_flat(&raster, screen, &clipped[0], &clipped[1], &clipped[2], 160, 100, int_to_fixed(100), 64);
            freak_raster3d_triangle_flat(&raster, screen, &clipped[0], &clipped[2], &clipped[3], 160, 100, int_to_fixed(100), 64);
        }
        freak_font_text(screen, 8, 8, &font, "3D raster: clipping, culling & z buffer", 15);
        freak_font_text(screen, 8, 18, &font, "Moving triangle crosses the near plane  Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, screen);
    }
    freak_raster3d_destroy(&raster);
}
