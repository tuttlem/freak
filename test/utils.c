#include <malloc.h>
#include "../src/freak.h"

static const unsigned char sprite_bits[64] = {
    0,0,0,0,12,0,0,0, 0,0,0,12,14,0,0,0,
    0,0,12,14,14,14,0,0, 0,12,14,14,14,14,14,0,
    12,14,14,12,14,14,14,14, 0,12,14,14,14,14,14,0,
    0,0,12,14,14,14,0,0, 0,0,0,12,14,0,0,0
};
static const unsigned char font_a[8] = {24,36,66,126,66,66,66,0};

int main(void)
{
    unsigned char *screen;
    struct freak_sprite sprite = {sprite_bits, 8, 8, 0};
    struct freak_font font = {font_a, 8, 8, 'A', 1};
    screen = (unsigned char *)malloc(64000U);
    if (screen == 0) return 1;
    freak_set_mcga();
    freak_palette_greyscale(16, 16, 4, 56);
    freak_palette_fire(32, 16);
    freak_clear_buffer(16, screen);
    freak_fill_rect(screen, 32, 28, 287, 171, 18);
    freak_debug_box(screen, 32, 28, 287, 171, 31);
    freak_fill_rect(screen, 48, 48, 143, 151, 20);
    freak_fill_rect(screen, 176, 48, 271, 151, 20);
    freak_debug_box(screen, 48, 48, 143, 151, 32);
    freak_debug_box(screen, 176, 48, 271, 151, 32);
    freak_sprite_blit_key(screen, 80, 92, &sprite);
    freak_sprite_blit_flip_x(screen, 104, 92, &sprite);
    freak_font_char(screen, 216, 78, &font, 'A', 31);
    freak_debug_cross(screen, 224, 112, 24, 47);
    freak_fill_rect(screen, 64, 132, 127, 139, 32);
    freak_copy_buffer(freak_vga, screen);
    while (!freak_kbhit()) { }
    freak_getch();
    freak_set_text();
    free(screen);
    return 0;
}
