#include "democom.h"
void freak_demo_voxel(void)
{
    struct freak_texture height;
    struct freak_voxel_view view;
    unsigned long last_tick, now, elapsed;

    if (!freak_texture_create(&height)) return;
    freak_texture_fractal_noise(&height, 0x564f5845UL, 32, 5, 150U, 0, 255);
    freak_palette_sky(0, 64);
    freak_palette_preset(64, 64, FREAK_PALETTE_FOREST, FREAK_PALETTE_FOREST_COUNT);
    freak_palette_preset(128, 64, FREAK_PALETTE_SEPIA, FREAK_PALETTE_SEPIA_COUNT);
    view.x = 128; view.y = 0; view.height = 180; view.horizon = 70; view.distance = 180;
    last_tick = freak_timer_ticks(); freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        now = freak_timer_ticks(); elapsed = now - last_tick; last_tick = now;
        if (elapsed > 4) elapsed = 4;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_LEFT)) view.x -= (int)elapsed * 3;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_RIGHT)) view.x += (int)elapsed * 3;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_UP)) view.height += (int)elapsed * 4;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_DOWN)) view.height -= (int)elapsed * 4;
        if (freak_input_key_down(FREAK_KEY_W)) view.y += (int)elapsed * 3;
        if (freak_input_key_down(FREAK_KEY_S)) view.y -= (int)elapsed * 3;
        if (view.height < 20) view.height = 20;
        if (view.height > 250) view.height = 250;
        freak_clear_buffer(26, screen);
        freak_voxel_heightfield(screen, height.pixels, &view, 64, 64);
        freak_font_text(screen, 8, 8, &font, "Voxel height field", 15);
        freak_font_text(screen, 8, 18, &font, "Left/Right: strafe  Up/Down: altitude  W/S: forward", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, screen);
    }
    freak_texture_destroy(&height);
}

