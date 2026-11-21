#include "democom.h"
#include <malloc.h>
void freak_demo_lens_flare(void)
{
    static const struct freak_colour halo_palette[] = {
        {2, 4, 18}, {4, 5, 20}, {10, 5, 24}
    };
    static const struct freak_colour flare_palette[] = {
        {16, 5, 26}, {38, 7, 23}, {57, 13, 10}, {63, 31, 4},
        {63, 53, 18}, {63, 63, 63}
    };
    struct freak_lens_flare flare;
    unsigned long last_tick;
    unsigned long now;
    unsigned long elapsed;
    uint8_t *blend_table;
    uint8_t *soften_table;

    flare.source_x = 255;
    flare.source_y = 55;
    flare.radius = 18;
    flare.colour = 95;
    flare.intensity = 220;
    flare.occluded = 0;
    flare.blend_table = 0;
    flare.soften_table = 0;
    freak_palette_sky(0, 64);
    /* Keep most of the flare range close to the background.  The blend
       tables can then emulate low alpha at the edge, reserving real brightness
       for the final 16 entries around the source. */
    freak_palette_preset(64, 16, halo_palette, 3);
    freak_palette_preset(80, 16, flare_palette, 6);
    freak_set_palette(15, 63, 63, 63);
    blend_table = (uint8_t *)malloc(FREAK_BLEND_TABLE_SIZE);
    if (!blend_table) return;
    soften_table = (uint8_t *)malloc(FREAK_BLEND_TABLE_SIZE);
    if (!soften_table) { free(blend_table); return; }
    freak_blend_table_build(blend_table, FREAK_BLEND_ADDITIVE);
    freak_blend_table_build(soften_table, FREAK_BLEND_TRANSLUCENT);
    flare.blend_table = blend_table;
    flare.soften_table = soften_table;
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
    free(soften_table);
    free(blend_table);
}

