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
    unsigned char *from;
    unsigned char *to;
    unsigned int tick;
    int step;
    unsigned int mode;
    struct freak_palette_snapshot palette;
    struct freak_sprite sprite = {sprite_bits, 8, 8, 0};
    struct freak_font font = {font_a, 8, 8, 'A', 1};
    screen = (unsigned char *)malloc(64000U);
    from = (unsigned char *)malloc(64000U);
    to = (unsigned char *)malloc(64000U);
    if (screen == 0 || from == 0 || to == 0) return 1;
    freak_set_mcga();
    freak_palette_greyscale(16, 16, 4, 56);
    freak_palette_fire(32, 16);
    freak_transition_palette_capture(&palette, 0, 48);
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
    freak_copy_buffer(from, screen);
    freak_clear_buffer(32, to);
    freak_fill_rect(to, 24, 20, 295, 179, 18);
    freak_debug_box(to, 24, 20, 295, 179, 47);
    freak_sprite_blit_key(to, 112, 70, &sprite);
    freak_sprite_blit_flip_x(to, 192, 108, &sprite);
    freak_font_char(to, 156, 92, &font, 'A', 31);
    freak_debug_cross(to, 160, 100, 40, 16);
    tick = 0;
    while (!freak_kbhit()) {
        step = tick % 120U;
        mode = (tick / 120U) % 7U;
        if (mode < 5)
            freak_transition_fade_up(&palette, 0, 48, 1, 1);
        switch (mode) {
        case 0: freak_transition_wipe_lr(screen, from, to, step, 119); break;
        case 1: freak_transition_wipe_tb(screen, from, to, step, 119); break;
        case 2: freak_transition_scroll_lr(screen, from, to, step, 119); break;
        case 3: freak_transition_dissolve(screen, from, to, step, 119, 1234U); break;
        case 4: freak_transition_checker(screen, from, to, step, 119, 16); break;
        case 5:
            freak_copy_buffer(screen, to);
            freak_transition_fade_down(&palette, 0, 48, step, 119);
            break;
        default:
            freak_copy_buffer(screen, from);
            freak_transition_fade_up(&palette, 0, 48, step, 119);
            break;
        }
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
        ++tick;
    }
    freak_getch();
    freak_set_text();
    free(from);
    free(to);
    free(screen);
    return 0;
}
