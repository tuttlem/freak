#include "widgets.h"

#include "../render/draw.h"

void freak_ui_panel(uint8_t *buffer, int x, int y, int width, int height,
                    const struct freak_ui_style *style)
{
    if (!buffer || !style) return;
    freak_fill_rect(buffer, x, y, x + width - 1, y + height - 1, style->panel);
    freak_draw_line(buffer, x, y, x + width - 1, y, style->border);
    freak_draw_line(buffer, x, y, x, y + height - 1, style->border);
    freak_draw_line(buffer, x + width - 1, y, x + width - 1, y + height - 1, style->border);
    freak_draw_line(buffer, x, y + height - 1, x + width - 1, y + height - 1, style->border);
}

int freak_ui_button(uint8_t *buffer, int x, int y, int width, int height,
                    const struct freak_font *font, const char *label,
                    int hovered, const struct freak_ui_style *style)
{
    struct freak_text_size size;
    if (!buffer || !font || !label || !style) return 0;
    freak_ui_panel(buffer, x, y, width, height, style);
    if (hovered)
        freak_fill_rect(buffer, x + 2, y + 2, x + width - 3, y + height - 3,
                        style->highlight);
    size = freak_font_measure(font, label);
    freak_font_text(buffer, x + (width - size.width) / 2,
                    y + (height - font->height) / 2, font, label,
                    hovered ? style->panel : style->text);
    return hovered;
}

void freak_ui_bar(uint8_t *buffer, int x, int y, int width, int height,
                  int value, int maximum, const struct freak_ui_style *style)
{
    int filled;
    if (!buffer || !style || maximum <= 0) return;
    if (value < 0) value = 0;
    if (value > maximum) value = maximum;
    filled = (width - 4) * value / maximum;
    freak_ui_panel(buffer, x, y, width, height, style);
    freak_fill_rect(buffer, x + 2, y + 2, x + 1 + filled, y + height - 3,
                    style->fill);
}

void freak_ui_slider(uint8_t *buffer, int x, int y, int width, int height,
                     int value, int maximum, const struct freak_ui_style *style)
{
    int knob;
    if (!buffer || !style || maximum <= 0 || width < 9) return;
    if (value < 0) value = 0;
    if (value > maximum) value = maximum;
    knob = x + 3 + (width - 8) * value / maximum;
    freak_ui_panel(buffer, x, y, width, height, style);
    freak_fill_rect(buffer, x + 3, y + height / 2 - 1, x + width - 4,
                    y + height / 2 + 1, style->border);
    freak_fill_rect(buffer, knob - 2, y + 2, knob + 2, y + height - 3,
                    style->highlight);
}

int freak_ui_slider_value(int x, int width, int maximum, int mouse_x)
{
    int value;
    if (maximum <= 0 || width < 9) return 0;
    value = (mouse_x - (x + 3)) * maximum / (width - 8);
    if (value < 0) value = 0;
    if (value > maximum) value = maximum;
    return value;
}

void freak_ui_inventory(uint8_t *buffer, int x, int y, int columns, int rows,
                        int selected, const uint8_t *items,
                        const struct freak_ui_style *style)
{
    int column, row, index;
    int cell = 16;
    if (!buffer || !style || columns < 1 || rows < 1) return;
    freak_ui_panel(buffer, x, y, columns * cell + 4, rows * cell + 4, style);
    for (row = 0; row < rows; ++row)
        for (column = 0; column < columns; ++column) {
            index = row * columns + column;
            freak_fill_rect(buffer, x + 3 + column * cell, y + 3 + row * cell,
                            x + 3 + column * cell + 13, y + 3 + row * cell + 13,
                            index == selected ? style->highlight : style->border);
            if (items && items[index])
                freak_fill_rect(buffer, x + 6 + column * cell, y + 6 + row * cell,
                                x + 13 + column * cell, y + 13 + row * cell,
                                items[index]);
        }
}

void freak_ui_cursor(uint8_t *buffer, int x, int y,
                     const struct freak_ui_style *style)
{
    if (!buffer || !style) return;
    freak_draw_line(buffer, x - 4, y, x + 4, y, style->cursor);
    freak_draw_line(buffer, x, y - 4, x, y + 4, style->cursor);
}
