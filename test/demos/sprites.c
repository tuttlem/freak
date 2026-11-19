#include "democom.h"

void freak_demo_sprites(void)
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
