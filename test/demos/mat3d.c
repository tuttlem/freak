#include "democom.h"

void freak_demo_material3d(void)
{
    struct freak_texture texture;
    struct freak_texture keyed_texture;
    int x;
    int y;
    int light_x;
    int distance;
    int shade;
    int have_texture;
    unsigned int tick;
    have_texture = freak_texture_create(&texture) && freak_texture_create(&keyed_texture);
    if (!have_texture) return;
    for (y = 0; y < 256; ++y) for (x = 0; x < 256; ++x) {
        texture.pixels[y * 256 + x] = ((x >> 4) ^ (y >> 4)) & 1 ? 96 : 72;
        keyed_texture.pixels[y * 256 + x] = texture.pixels[y * 256 + x];
        if ((x - 128) * (x - 128) + (y - 128) * (y - 128) < 38 * 38)
            keyed_texture.pixels[y * 256 + x] = 0;
    }
    tick = 0; freak_demo_palette_default(); freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        ++tick; freak_clear_buffer(8, screen);
        freak_font_text(screen, 8, 8, &font, "Material mapping modes", 15);
        freak_font_text(screen, 16, 28, &font, "AFFINE", 15);
        freak_font_text(screen, 122, 28, &font, "KEYED", 15);
        freak_font_text(screen, 212, 28, &font, "PERSPECTIVE", 15);
        freak_tri_t(15, 48, 115, 48, 65, 138, 0, 0, 255, 0, 128, 255, &texture, screen);
        freak_tri_t_key(110, 138, 160, 38, 220, 135, 0, 255, 128, 0, 255, 255, &keyed_texture, 0, screen);
        freak_tri_pt(205, 138, int_to_fixed(3), 0, 255, 260, 43, int_to_fixed(1), 128, 0,
                     315, 138, int_to_fixed(3), 255, 255, &texture, screen);
        /* A checker material multiplied by a moving radial lightmap. */
        light_x = 160 + fixed_to_int(fixed_sin((tick * 5U) & 1023U) * 90);
        for (y = 153; y < 193; ++y) for (x = 10; x < 310; ++x) {
            distance = (x - light_x) * (x - light_x) + (y - 173) * (y - 173);
            shade = 31 - distance / 350;
            if (((x >> 3) ^ (y >> 3)) & 1) shade -= 6;
            if (shade < 0) shade = 0; if (shade > 31) shade = 31;
            screen[y * 320 + x] = (uint8_t)(64 + shade);
        }
        freak_font_text(screen, 12, 158, &font, "LIGHTMAP", 15);
        freak_font_text(screen, 8, 198 - font.height, &font, "Centre hole: transparency key; lower panel: material x lightmap  Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, screen);
    }
    freak_texture_destroy(&texture);
    freak_texture_destroy(&keyed_texture);
}
