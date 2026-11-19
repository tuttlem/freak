#ifndef __freak_widgets_h_
#define __freak_widgets_h_

#include <stdint.h>

#include "../render/font.h"

struct freak_ui_style {
    uint8_t panel;
    uint8_t border;
    uint8_t text;
    uint8_t fill;
    uint8_t highlight;
    uint8_t cursor;
};

void freak_ui_panel(uint8_t *buffer, int x, int y, int width, int height,
                    const struct freak_ui_style *style);
int freak_ui_button(uint8_t *buffer, int x, int y, int width, int height,
                    const struct freak_font *font, const char *label,
                    int hovered, const struct freak_ui_style *style);
void freak_ui_bar(uint8_t *buffer, int x, int y, int width, int height,
                  int value, int maximum, const struct freak_ui_style *style);
void freak_ui_slider(uint8_t *buffer, int x, int y, int width, int height,
                     int value, int maximum, const struct freak_ui_style *style);
int freak_ui_slider_value(int x, int width, int maximum, int mouse_x);
void freak_ui_inventory(uint8_t *buffer, int x, int y, int columns, int rows,
                        int selected, const uint8_t *items,
                        const struct freak_ui_style *style);
void freak_ui_cursor(uint8_t *buffer, int x, int y,
                     const struct freak_ui_style *style);

#endif
