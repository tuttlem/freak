#include <malloc.h>
#include "democom.h"
void freak_demo_distortion(void)
{
    unsigned char *source;
    unsigned char *output;
    unsigned long last_tick, now, tick;
    int center_x, center_y, x, y, mode;
    static const char *labels[] = {"Magnifying glass", "Water wobble", "Shockwave"};

    source = (unsigned char *)malloc(64000U);
    output = (unsigned char *)malloc(64000U);
    if (!source || !output) { free(source); free(output); return; }
    freak_palette_sky(0, 64);
    freak_palette_fire(64, 32);
    freak_palette_preset(96, 64, FREAK_PALETTE_ICE, FREAK_PALETTE_ICE_COUNT);
    freak_set_palette(15, 63, 63, 63);
    center_x = 160; center_y = 100; mode = 0; tick = 0;
    last_tick = freak_timer_ticks();
    freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        now = freak_timer_ticks(); tick += now - last_tick; last_tick = now;
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_ACCEPT)) mode = (mode + 1) % 3;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_LEFT)) center_x -= 2;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_RIGHT)) center_x += 2;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_UP)) center_y -= 2;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_DOWN)) center_y += 2;
        for (y = 0; y < 200; ++y)
            for (x = 0; x < 320; ++x)
                source[y * 320 + x] = (uint8_t)(96 + ((x / 16 + y / 12) & 31));
        freak_fill_rect(source, 80, 70, 240, 130, 72);
        freak_fill_rect(source, 138, 40, 182, 160, 48);
        if (mode == 0) freak_distort_lens(output, source, center_x, center_y, 54, 85);
        else if (mode == 1) freak_distort_wave(output, source, (unsigned int)tick, 5);
        else freak_distort_shockwave(output, source, center_x, center_y,
                                     20 + (int)(tick % 70U), 12, 45);
        freak_font_text(output, 8, 8, &font, "Glass lens & distortion", 15);
        freak_font_text(output, 8, 18, &font, labels[mode], 15);
        freak_font_text(output, 8, 180, &font,
                        "Enter: mode  Arrows: centre  Esc: back", 15);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, output);
    }
    free(output);
    free(source);
}

