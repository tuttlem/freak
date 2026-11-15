#ifndef __freak_menu_h_
#define __freak_menu_h_

#include <stdint.h>
#include "../render/font.h"

#define FREAK_MENU_STACK_MAX 8
#define FREAK_MENU_BACK -1

struct freak_menu;

struct freak_menu_item {
    const char *label;
    int value;
    const struct freak_menu *child;
    uint8_t enabled;
};

struct freak_menu {
    const char *title;
    const struct freak_menu_item *items;
    int count;
};

struct freak_menu_stack {
    const struct freak_menu *menus[FREAK_MENU_STACK_MAX];
    int selected[FREAK_MENU_STACK_MAX];
    int depth;
};

struct freak_menu_style {
    uint8_t background, border, text, selected_text, selected_background,
        disabled;
    int padding, row_height;
};

void freak_menu_open(struct freak_menu_stack *stack,
                     const struct freak_menu *menu);
void freak_menu_move(struct freak_menu_stack *stack, int direction);
int freak_menu_accept(struct freak_menu_stack *stack);
int freak_menu_back(struct freak_menu_stack *stack);
const struct freak_menu_item *
freak_menu_current(const struct freak_menu_stack *stack);
void freak_menu_draw(uint8_t *buffer, int x, int y,
                     const struct freak_font *font,
                     const struct freak_menu_stack *stack,
                     const struct freak_menu_style *style);

#endif
