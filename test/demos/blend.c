
#include <malloc.h>
#include "../democtx.h"

void freak_demo_blend(const struct freak_demo_context *context)
{
    unsigned char *table;
    enum freak_blend_mode mode;
    static const char *labels[] = {"Additive", "Subtractive", "Multiply", "Translucent"};
    table = (unsigned char *)malloc(FREAK_BLEND_TABLE_SIZE);
    if (!table) return;
    freak_palette_greyscale(0, 64, 0, 63); freak_palette_fire(64, 64);
    freak_palette_sky(128, 64); freak_palette_preset(192, 63, FREAK_PALETTE_FOREST, FREAK_PALETTE_FOREST_COUNT);
    freak_set_palette(15, 63, 63, 63); mode = FREAK_BLEND_ADDITIVE;
    freak_blend_table_build(table, mode); freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_LEFT)) {
            mode = mode == FREAK_BLEND_ADDITIVE ? FREAK_BLEND_TRANSLUCENT : mode - 1;
            freak_blend_table_build(table, mode);
        }
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_RIGHT)) {
            mode = mode == FREAK_BLEND_TRANSLUCENT ? FREAK_BLEND_ADDITIVE : mode + 1;
            freak_blend_table_build(table, mode);
        }
        freak_clear_buffer(0, context->screen);
        freak_fill_rect(context->screen, 24, 46, 295, 150, 100);
        freak_fill_rect(context->screen, 50, 66, 185, 132, 160);
        freak_blend_rect(context->screen, 115, 55, 245, 142, 80, table);
        freak_blend_rect(context->screen, 82, 82, 218, 158, 220, table);
        freak_font_text(context->screen, 8, 8, context->font, "Blend lookup tables", 15);
        freak_font_text(context->screen, 8, 18, context->font, labels[mode], 15);
        freak_font_text(context->screen, 8, 180, context->font, "Left/Right: mode  Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, context->screen);
    }
    free(table);
}
