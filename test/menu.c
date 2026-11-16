#include <malloc.h>
#include "../src/freak.h"

static const struct freak_menu_item options_items[] = {
    {"Sound", 2, 0, 1}, {"Controls", 3, 0, 1}, {"Back", FREAK_MENU_BACK, 0, 1}};
static const struct freak_menu options_menu = {"Options", options_items, 3};
static const struct freak_menu_item main_items[] = {
    {"Message box", 1, 0, 1},
    {"Confirm box", 2, 0, 1},
    {"Text input", 3, 0, 1},
    {"Options", 0, &options_menu, 1},
    {"Quit", 9, 0, 1}};
static const struct freak_menu main_menu = {"Freak UI", main_items, 5};

int freak_demo_ui(void) {
    unsigned char *screen;
    unsigned short key;
    int action;
    int modal = 0;
    char command[32] = "LOOK";
    struct freak_font font = {(const unsigned char *)0x000FFA6EL, 8, 8, 0, 128};
    struct freak_menu_stack menu;
    struct freak_menu_style style = {1, 15, 15, 1, 12, 8, 6, 10};
    struct freak_dialog_style dialog_style = {1, 15, 15, 1, 12, 15, 6, 10};
    struct freak_confirm confirm;
    struct freak_text_input input;
    screen = (unsigned char *)malloc(64000U);
    if (!screen)
        return 1;
    freak_palette_greyscale(0, 16, 0, 63);
    freak_menu_open(&menu, &main_menu);
    key = 0;
    while ((unsigned char)key != 27) {
        freak_clear_buffer(0, screen);
        freak_menu_draw(screen, 80, 55, &font, &menu, &style);
        if (modal == 1)
            freak_message_box(screen, 46, 58, 228, &font, "MESSAGE",
                              "A functional little box. ESC closes it.",
                              &dialog_style);
        if (modal == 2)
            freak_confirm_draw(screen, 46, 58, 228, &font, "CONFIRM",
                               "Abandon your adventure?", "YES", "NO", &confirm,
                               &dialog_style);
        if (modal == 3)
            freak_text_input_draw(screen, 46, 58, 228, &font, "COMMAND",
                                  "What do you do?", &input, &dialog_style);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
        if (freak_kbhit()) {
            key = freak_getch();
            if (modal == 1) {
                if ((unsigned char)key == 13 || (unsigned char)key == 27)
                    modal = 0;
                continue;
            }
            if (modal == 2) {
                if (freak_confirm_handle_key(&confirm, key))
                    modal = 0;
                continue;
            }
            if (modal == 3) {
                if (freak_text_input_handle_key(&input, key))
                    modal = 0;
                continue;
            }
            if ((unsigned char)key == 'w' || (key >> 8) == FREAK_KEY_UP)
                freak_menu_move(&menu, -1);
            if ((unsigned char)key == 's' || (key >> 8) == FREAK_KEY_DOWN)
                freak_menu_move(&menu, 1);
            if ((unsigned char)key == 13) {
                action = freak_menu_accept(&menu);
                if (action == 1)
                    modal = 1;
                if (action == 2) {
                    freak_confirm_open(&confirm);
                    modal = 2;
                }
                if (action == 3) {
                    freak_text_input_open(&input, command, sizeof(command));
                    modal = 3;
                }
                if (action == 9)
                    key = 27;
            }
            if ((unsigned char)key == 8 || (unsigned char)key == 27)
                if (freak_menu_back(&menu) == -1)
                    key = 27;
        }
    }
    free(screen);
    return 0;
}
