#include "democom.h"
void freak_demo_lens_flare(void)
{
    struct freak_lens_flare flare;
    unsigned long last_tick;
    unsigned long now;
    unsigned long elapsed;

    flare.source_x = 255;
    flare.source_y = 55;
    flare.radius = 18;
    flare.colour = 92;
    flare.intensity = 220;
    flare.occluded = 0;
    freak_palette_sky(0, 64);
    freak_palette_fire(64, 32);
    freak_set_palette(15, 63, 63, 63);
    last_tick = freak_timer_ticks();
    freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK))
            break;
        now = freak_timer_ticks(); elapsed = now - last_tick; last_tick = now;
        if (elapsed > 4) elapsed = 4;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_LEFT)) flare.source_x -= (int)elapsed * 3;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_RIGHT)) flare.source_x += (int)elapsed * 3;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_UP)) flare.source_y -= (int)elapsed * 3;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_DOWN)) flare.source_y += (int)elapsed * 3;
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_ACCEPT))
            flare.occluded = !flare.occluded;
        freak_clear_buffer(18, screen);
        freak_fill_rect(screen, 0, 130, 319, 199, 8);
        freak_fill_rect(screen, 60, 118, 260, 132, 12);
        freak_lens_flare_draw(screen, &flare, 160, 100);
        freak_font_text(screen, 8, 8, &font,
                        "Lens flare: halo, streaks & ghosts", 15);
        freak_font_text(screen, 8, 18, &font,
                        "Arrows: light  Enter: occlusion  Esc: back", 15);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
    }
}

