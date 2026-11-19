#include "democom.h"

void freak_demo_material3d(void)
{
    struct freak_texture texture;
    int x;
    int y;
    int have_texture;
    unsigned int tick;
    have_texture = freak_texture_create(&texture);
    if (!have_texture) return;
    for (y = 0; y < 256; ++y) for (x = 0; x < 256; ++x) {
        texture.pixels[y * 256 + x] = ((x >> 4) ^ (y >> 4)) & 1 ? 96 : 64;
        if ((x - 128) * (x - 128) + (y - 128) * (y - 128) < 38 * 38) texture.pixels[y * 256 + x] = 0;
    }
    tick = 0; freak_demo_palette_default(); freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        ++tick; freak_clear_buffer(8, screen);
        /* The left triangle is affine; the centre triangle uses its key; the
           right hand one uses the perspective-correct renderer. */
        freak_tri_t(15, 50, 115, 50, 65, 170, 0, 0, 255, 0, 128, 255, &texture, screen);
        freak_tri_t_key(110, 170, 160, 40, 220, 165, 0, 255, 128, 0, 255, 255, &texture, 0, screen);
        freak_tri_pt(205, 170, int_to_fixed(3), 0, 255, 260, 45, int_to_fixed(1), 128, 0,
                     315, 170, int_to_fixed(3), 255, 255, &texture, screen);
        for (y = 178; y <= 188; ++y) for (x = 5; x <= 315; ++x)
            screen[y * 320 + x] = (uint8_t)(32 + ((x + y + tick) & 15));
        freak_font_text(screen, 8, 8, &font, "Materials: flat/Gouraud, affine/perspective, key transparency", 15);
        freak_font_text(screen, 8, 18, &font, "The animated lower strip is a palette lightmap surface  Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, screen);
    }
    freak_texture_destroy(&texture);
}
