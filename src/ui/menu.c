#include "menu.h"
#include "../render/draw.h"

static int menu_next(const struct freak_menu *menu, int selected,
                     int direction) {
    int tries = menu->count;
    while (tries--) {
        selected = (selected + direction + menu->count) % menu->count;
        if (menu->items[selected].enabled)
            return selected;
    }
    return selected;
}

void freak_menu_open(struct freak_menu_stack *stack,
                     const struct freak_menu *menu) {
    if (stack == 0 || menu == 0)
        return;
    stack->depth = 0;
    stack->menus[0] = menu;
    stack->selected[0] = menu_next(menu, menu->count - 1, 1);
}

void freak_menu_move(struct freak_menu_stack *stack, int direction) {
    if (stack == 0 || stack->depth < 0 || direction == 0)
        return;
    stack->selected[stack->depth] =
        menu_next(stack->menus[stack->depth], stack->selected[stack->depth],
                  direction > 0 ? 1 : -1);
}

int freak_menu_accept(struct freak_menu_stack *stack) {
    const struct freak_menu_item *item = freak_menu_current(stack);
    if (item == 0 || !item->enabled)
        return 0;
    if (item->child != 0 && stack->depth + 1 < FREAK_MENU_STACK_MAX) {
        ++stack->depth;
        stack->menus[stack->depth] = item->child;
        stack->selected[stack->depth] =
            menu_next(item->child, item->child->count - 1, 1);
        return 0;
    }
    if (item->value == FREAK_MENU_BACK)
        return freak_menu_back(stack);
    return item->value;
}

int freak_menu_back(struct freak_menu_stack *stack) {
    if (stack == 0 || stack->depth < 0)
        return 0;
    if (stack->depth == 0)
        return -1;
    --stack->depth;
    return 0;
}

const struct freak_menu_item *
freak_menu_current(const struct freak_menu_stack *stack) {
    if (stack == 0 || stack->depth < 0 || stack->menus[stack->depth] == 0)
        return 0;
    return &stack->menus[stack->depth]->items[stack->selected[stack->depth]];
}

void freak_menu_draw(uint8_t *buffer, int x, int y,
                     const struct freak_font *font,
                     const struct freak_menu_stack *stack,
                     const struct freak_menu_style *style) {
    const struct freak_menu *menu;
    struct freak_text_size size;
    int i, width = 0, height;
    if (!buffer || !font || !stack || !style || stack->depth < 0)
        return;
    menu = stack->menus[stack->depth];
    size = freak_font_measure(font, menu->title);
    width = size.width / font->width;
    for (i = 0; i < menu->count; ++i) {
        size = freak_font_measure(font, menu->items[i].label);
        if (size.width / font->width > width)
            width = size.width / font->width;
    }
    width = width * font->width + style->padding * 2;
    height = (menu->count + 1) * style->row_height + style->padding * 2;
    freak_fill_rect(buffer, x, y, x + width, y + height, style->background);
    freak_draw_line(buffer, x, y, x + width, y, style->border);
    freak_draw_line(buffer, x + width, y, x + width, y + height, style->border);
    freak_draw_line(buffer, x + width, y + height, x, y + height,
                    style->border);
    freak_draw_line(buffer, x, y + height, x, y, style->border);
    freak_font_text(buffer, x + style->padding, y + style->padding, font,
                    menu->title, style->text);
    for (i = 0; i < menu->count; ++i) {
        int yy = y + style->padding + style->row_height * (i + 1);
        uint8_t c = menu->items[i].enabled ? style->text : style->disabled;
        if (i == stack->selected[stack->depth]) {
            freak_fill_rect(
                buffer, x + style->padding, yy, x + width - style->padding,
                yy + style->row_height - 1, style->selected_background);
            c = style->selected_text;
        }
        freak_font_text(buffer, x + style->padding, yy, font,
                        menu->items[i].label, c);
    }
}
