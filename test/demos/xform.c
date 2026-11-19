#include "democom.h"
void freak_demo_xform(void)
{
    struct freak_texture texture;
    uint8_t a, b, c, d;
    unsigned long last_tick, now, tick;
    int mode;
    static const char *labels[] = {"Scale", "Rotozoom", "Tunnel", "Floor"};

    if (!freak_texture_create(&texture)) return;
    a = 0; b = 64; c = 128; d = 192; mode = 0; tick = 0;
    freak_palette_sky(0, 64); freak_palette_fire(64, 64);
    freak_palette_preset(128, 64, FREAK_PALETTE_WATER, FREAK_PALETTE_WATER_COUNT);
    last_tick = freak_timer_ticks(); freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        now = freak_timer_ticks(); tick += now - last_tick; last_tick = now;
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_ACCEPT)) mode = (mode + 1) & 3;
        freak_texture_plasma(&texture, &a, &b, &c, &d, 64, 64);
        freak_clear_buffer(12, screen);
        if (mode == 0) freak_xform_scale(screen, &texture, 48, 35, 224, 150);
        else if (mode == 1) freak_xform_rotozoom(screen, &texture, 160, 100,
                                                  (int)(tick * 12), 100 + (int)(tick % 80U));
        else if (mode == 2) freak_xform_tunnel(screen, &texture, 160, 100, (unsigned int)tick);
        else { freak_fill_rect(screen, 0, 0, 319, 99, 20); freak_xform_floor(screen, &texture, 100, (unsigned int)tick); }
        freak_font_text(screen, 8, 8, &font, "Scalers & mapping", 15);
        freak_font_text(screen, 8, 18, &font, labels[mode], 15);
        freak_font_text(screen, 8, 180, &font, "Enter: mode  Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, screen);
    }
    freak_texture_destroy(&texture);
}

