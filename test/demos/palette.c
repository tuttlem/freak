#include "../democtx.h"

void freak_demo_palette(const struct freak_demo_context *context)
{
    struct freak_palette_snapshot snapshot;
    unsigned long last_tick, now, tick;
    unsigned int phase;
    int band;

    freak_palette_greyscale(0, 32, 0, 24);
    freak_palette_sky(32, 16); freak_palette_fire(48, 16);
    freak_palette_preset(64, 16, FREAK_PALETTE_WATER, FREAK_PALETTE_WATER_COUNT);
    freak_palette_preset(80, 16, FREAK_PALETTE_FOREST, FREAK_PALETTE_FOREST_COUNT);
    freak_palette_fire(96, 32); freak_transition_palette_capture(&snapshot, 0, 96);
    tick = 0; last_tick = freak_timer_ticks(); freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        now = freak_timer_ticks(); tick += now - last_tick; last_tick = now;
        phase = (unsigned int)(tick % 144U);
        if (phase < 72) freak_transition_fade_down(&snapshot, 0, 96, phase, 72);
        else freak_transition_fade_up(&snapshot, 0, 96, phase - 72, 72);
        if ((tick & 1U) == 0) freak_palette_rotate(96, 32, 1);
        for (band = 0; band < 6; ++band)
            freak_fill_rect(context->screen, 20, 42 + band * 22, 299,
                            60 + band * 22, (uint8_t)(band * 16 + 8));
        freak_fill_rect(context->screen, 20, 176, 299, 190, (uint8_t)(96 + (tick & 31U)));
        freak_font_text(context->screen, 8, 8, context->font, "Palette effects: gradients, fades & cycling", 15);
        freak_font_text(context->screen, 8, 18, context->font, "Pixels stay put; only the DAC palette changes", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, context->screen);
    }
}
