#include <malloc.h>
#include "../src/freak.h"

static const struct freak_menu_item options_items[] = {
    {"SOUND", 2, 0, 1},
    {"CONTROLS", 3, 0, 1},
    {"BACK", FREAK_MENU_BACK, 0, 1}
};
static const struct freak_menu options_menu = {
    "OPTIONS", options_items, 3
};
static const struct freak_menu_item main_items[] = {
    {"START", 1, 0, 1},
    {"OPTIONS", 0, &options_menu, 1},
    {"QUIT", 9, 0, 1}
};
static const struct freak_menu main_menu = {
    "FREAK MENU", main_items, 3
};

int main(void) {
    unsigned char *screen;
    unsigned short key;
    int action;
    struct freak_font font = {(const unsigned char *)0x000FFA6EL, 8, 8, 0, 128};
    struct freak_menu_stack menu;
    struct freak_menu_style style = {1, 15, 15, 1, 12, 8, 6, 10};
    screen = (unsigned char *)malloc(64000U);
    if (!screen) return 1;
    freak_set_mcga();
    freak_palette_greyscale(0, 16, 0, 63);
    freak_menu_open(&menu, &main_menu);
    key = 0;
    while ((unsigned char)key != 27) {
        freak_clear_buffer(0, screen);
        freak_menu_draw(screen, 80, 55, &font, &menu, &style);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
        if (freak_kbhit()) {
            key = freak_getch();
            if ((unsigned char)key == 'w' || (key >> 8) == FREAK_KEY_UP) freak_menu_move(&menu, -1);
            if ((unsigned char)key == 's' || (key >> 8) == FREAK_KEY_DOWN) freak_menu_move(&menu, 1);
            if ((unsigned char)key == 13) {
                action = freak_menu_accept(&menu);
                if (action == 9) key = 27;
            }
            if ((unsigned char)key == 8 || (unsigned char)key == 27) if (freak_menu_back(&menu) == -1) key = 27;
        }
    }
    freak_set_text(); free(screen); return 0;
}
