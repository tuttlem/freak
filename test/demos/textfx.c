#include "democom.h"
void freak_demo_textfx(void)
{
    int stars_x[64];
    int stars_y[64];
    int stars_z[64];
    unsigned long last_tick, now, tick;
    int index, x, y, text_x;
    const char *scroll = "  FREAK -- DOS GAME CODE, TINY DEMOS, BIG IDEAS -- ";
    int length = 0;

    while (scroll[length]) ++length;
    for (index = 0; index < 64; ++index) {
        stars_x[index] = ((index * 73) % 320) - 160;
        stars_y[index] = ((index * 47) % 200) - 100;
        stars_z[index] = 20 + (index * 31) % 180;
    }
    freak_palette_sky(0, 64); freak_palette_fire(64, 64);
    freak_set_palette(15, 63, 63, 63);
    tick = 0; last_tick = freak_timer_ticks(); freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        now = freak_timer_ticks(); tick += now - last_tick; last_tick = now;
        freak_clear_buffer(0, screen);
        for (index = 0; index < 64; ++index) {
            stars_z[index] -= 2;
            if (stars_z[index] < 4) { stars_z[index] = 180; stars_x[index] = ((index * 97 + tick) % 320) - 160; stars_y[index] = ((index * 59 + tick) % 200) - 100; }
            x = 160 + stars_x[index] * 80 / stars_z[index];
            y = 100 + stars_y[index] * 80 / stars_z[index];
            freak_draw_pixel(screen, x, y, (uint8_t)(8 + (180 - stars_z[index]) / 14));
        }
        text_x = 160 - freak_font_measure(&font, "F R E A K").width / 2;
        for (index = 0; index < 9; ++index)
            freak_font_char(screen, text_x + index * 8,
                            44 + fixed_to_int(fixed_sin((index * 80 + tick * 12) & 1023) * 8),
                            &font, "F R E A K"[index], (uint8_t)(64 + index * 3));
        for (index = 0; index < length; ++index) {
            x = 320 - (int)((tick * 3 + index * 8) % (length * 8 + 320));
            y = 154 + fixed_to_int(fixed_sin((index * 48 + tick * 10) & 1023) * 7);
            freak_font_char(screen, x, y, &font, scroll[index], 15);
        }
        freak_font_text(screen, 86, 90, &font, "CODE / ART / SOUND / PLAY", 15);
        freak_font_text(screen, 8, 184, &font, "Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, screen);
    }
}

