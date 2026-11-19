#include <malloc.h>
#include "../../src/freak.h"

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

static unsigned char key_ascii(unsigned char scan)
{
    static const char top_row[] = "qwertyuiop";
    static const char home_row[] = "asdfghjkl";
    static const char bottom_row[] = "zxcvbnm";

    if (scan == FREAK_KEY_ESCAPE) return 27;
    if (scan == FREAK_KEY_ENTER) return 13;
    if (scan == FREAK_KEY_BACKSPACE) return 8;
    if (scan >= 0x02 && scan <= 0x0a) return (unsigned char)('1' + scan - 0x02);
    if (scan == 0x0b) return '0';
    if (scan == 0x39) return ' ';
    if (scan >= 0x10 && scan <= 0x19) return top_row[scan - 0x10];
    if (scan >= 0x1e && scan <= 0x26) return home_row[scan - 0x1e];
    if (scan >= 0x2c && scan <= 0x32) return bottom_row[scan - 0x2c];
    return 0;
}

int freak_demo_ui(void) {
    unsigned char *screen;
    struct freak_input_event event;
    unsigned short key;
    int action;
    int volume = 65;
    int modal = 0;
    int running = 1;
    char command[32] = "LOOK";
    struct freak_font font = {(const unsigned char *)0x000FFA6EL, 8, 8, 0, 128};
    struct freak_menu_stack menu;
    struct freak_menu_style style = {1, 15, 15, 1, 12, 8, 6, 10};
    struct freak_dialog_style dialog_style = {1, 15, 15, 1, 12, 15, 6, 10};
    struct freak_ui_style widget_style = {1, 15, 15, 48, 12, 15};
    static const unsigned char inventory[] = {80, 0, 92, 0, 48, 0, 100, 0};
    struct freak_confirm confirm;
    struct freak_text_input input;
    screen = (unsigned char *)malloc(64000U);
    if (!screen)
        return 1;
    freak_palette_greyscale(0, 16, 0, 63);
    freak_menu_open(&menu, &main_menu);
    freak_input_clear_events();
    while (running) {
        freak_clear_buffer(0, screen);
        freak_ui_panel(screen, 8, 8, 64, 44, &widget_style);
        freak_font_text(screen, 14, 13, &font, "HUD", 15);
        freak_ui_bar(screen, 14, 25, 50, 9, 72, 100, &widget_style);
        freak_ui_bar(screen, 14, 38, 50, 8, 40, 100, &widget_style);
        freak_font_text(screen, 8, 57, &font, "VOL", 15);
        freak_ui_slider(screen, 32, 55, 40, 12, volume, 100, &widget_style);
        freak_ui_inventory(screen, 236, 8, 4, 2, menu.selected[menu.depth],
                           inventory, &widget_style);
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
        freak_ui_cursor(screen, freak_mouse_x(), freak_mouse_y(), &widget_style);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
        freak_input_update();
        if (!modal && freak_mouse_down(FREAK_MOUSE_LEFT) &&
            freak_mouse_x() >= 32 && freak_mouse_x() < 72 &&
            freak_mouse_y() >= 55 && freak_mouse_y() < 67)
            volume = freak_ui_slider_value(32, 40, 100, freak_mouse_x());
        while (freak_input_poll_event(&event)) {
            if (!modal && event.type == FREAK_EVENT_MOUSE_MOVE &&
                event.mouse_x >= 80 && event.mouse_x < 240 &&
                event.mouse_y >= 71 &&
                (event.mouse_y - 71) / 10 < menu.menus[menu.depth]->count)
                menu.selected[menu.depth] = (event.mouse_y - 71) / 10;
            if (!modal && event.type == FREAK_EVENT_MOUSE_BUTTON_DOWN &&
                event.button == FREAK_MOUSE_LEFT && event.mouse_x >= 80 &&
                event.mouse_x < 240 && event.mouse_y >= 71 &&
                (event.mouse_y - 71) / 10 < menu.menus[menu.depth]->count) {
                menu.selected[menu.depth] = (event.mouse_y - 71) / 10;
                action = freak_menu_accept(&menu);
                if (action == 1) modal = 1;
                if (action == 2) { freak_confirm_open(&confirm); modal = 2; }
                if (action == 3) { freak_text_input_open(&input, command, sizeof(command)); modal = 3; }
                if (action == 9) running = 0;
                continue;
            }
            key = (unsigned short)(event.scan_code << 8) |
                  key_ascii(event.scan_code);
            if (modal == 1) {
                if (event.type == FREAK_EVENT_MOUSE_BUTTON_DOWN &&
                    event.button == FREAK_MOUSE_LEFT) {
                    modal = 0;
                    freak_input_clear_events();
                    break;
                }
                if (event.type == FREAK_EVENT_ACTION_PRESSED &&
                    (event.action == FREAK_ACTION_ACCEPT ||
                     event.action == FREAK_ACTION_BACK)) {
                    modal = 0;
                    freak_input_clear_events();
                    break;
                }
                continue;
            }
            if (modal == 2) {
                if (event.type == FREAK_EVENT_MOUSE_BUTTON_DOWN &&
                    event.button == FREAK_MOUSE_LEFT && event.mouse_y >= 80 &&
                    event.mouse_y < 98) {
                    if (event.mouse_x >= 120 && event.mouse_x < 160)
                        modal = 0;
                    else if (event.mouse_x >= 160 && event.mouse_x < 205)
                        modal = 0;
                    freak_input_clear_events();
                    break;
                }
                if (event.type == FREAK_EVENT_ACTION_PRESSED &&
                    (event.action == FREAK_ACTION_LEFT ||
                     event.action == FREAK_ACTION_RIGHT))
                    confirm.selected = !confirm.selected;
                if (event.type == FREAK_EVENT_ACTION_PRESSED &&
                    (event.action == FREAK_ACTION_ACCEPT ||
                     event.action == FREAK_ACTION_BACK)) {
                    modal = 0;
                    freak_input_clear_events();
                    break;
                }
                continue;
            }
            if (modal == 3) {
                if (event.type == FREAK_EVENT_KEY_DOWN &&
                    freak_text_input_handle_key(&input, key)) {
                    modal = 0;
                    freak_input_clear_events();
                    break;
                }
                continue;
            }
            if (event.type != FREAK_EVENT_ACTION_PRESSED)
                continue;
            if (event.action == FREAK_ACTION_UP)
                freak_menu_move(&menu, -1);
            if (event.action == FREAK_ACTION_DOWN)
                freak_menu_move(&menu, 1);
            if (event.action == FREAK_ACTION_ACCEPT) {
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
                    running = 0;
            }
            if (event.action == FREAK_ACTION_BACK)
                if (freak_menu_back(&menu) == -1)
                    running = 0;
        }
    }
    free(screen);
    return 0;
}
