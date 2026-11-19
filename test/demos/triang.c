#include "democom.h"
#define DEMO_FLAT_TRIANGLES 1
#define DEMO_GOURAUD_TRIANGLES 2
#define DEMO_TEXTURE_TRIANGLES 3
void freak_demo_triangles(int mode)
{
    struct freak_texture texture;
    unsigned int tick;
    int x;
    int y;
    int have_texture;

    have_texture = 0;
    if (mode == DEMO_TEXTURE_TRIANGLES) {
        have_texture = freak_texture_create(&texture);
        if (have_texture) {
            for (y = 0; y < 256; ++y)
                for (x = 0; x < 256; ++x)
                    texture.pixels[x + (y << 8)] = 64 + ((x ^ y) & 31);
        }
    }
    freak_demo_palette_default();
    tick = 0;
    freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings,
                                FREAK_ACTION_BACK))
            break;
        freak_clear_buffer(0, screen);
        if (mode == DEMO_FLAT_TRIANGLES)
            freak_tri_c(30 + (tick % 180), 30, 290, 90,
                        100, 185, 72 + (tick & 15), screen);
        else if (mode == DEMO_GOURAUD_TRIANGLES)
            freak_tri_g(30 + (tick % 180), 30, 290, 90, 100, 185,
                        64, 95, 80, screen);
        else if (have_texture)
            freak_tri_t(30 + (tick % 180), 30, 290, 90, 100, 185,
                        0, 0, 255, 0, 64, 255, &texture, screen);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
        tick += (unsigned int)freak_timing_update(&demo_timing);
    }
    if (have_texture)
        freak_texture_destroy(&texture);
}

static const int cube_faces[6][4] = {
    {0, 1, 2, 3}, {4, 7, 6, 5}, {0, 4, 5, 1},
    {3, 2, 6, 7}, {0, 3, 7, 4}, {1, 5, 6, 2}
};
static const int cube_uv[4][2] = {{0, 0}, {255, 0}, {255, 255}, {0, 255}};

