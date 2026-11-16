#include "dialog.h"

#include "../input/keyboard.h"
#include "../render/draw.h"

static void dialog_frame(uint8_t *buffer, int x, int y, int width, int height,
                         const struct freak_dialog_style *style) {
    freak_fill_rect(buffer, x, y, x + width, y + height, style->background);
    freak_draw_line(buffer, x, y, x + width, y, style->border);
    freak_draw_line(buffer, x + width, y, x + width, y + height, style->border);
    freak_draw_line(buffer, x + width, y + height, x, y + height,
                    style->border);
    freak_draw_line(buffer, x, y + height, x, y, style->border);
}

static int dialog_lines(const char *text, int columns) {
    int lines = 1;
    int column = 0;
    while (*text) {
        if (*text == '\n') {
            ++lines;
            column = 0;
        } else if (++column > columns) {
            ++lines;
            column = 1;
        }
        ++text;
    }
    return lines;
}

static void dialog_text(uint8_t *buffer, int x, int y, int columns,
                        const struct freak_font *font, const char *text,
                        uint8_t colour, int row_height) {
    int column = 0;
    while (*text) {
        if (*text == '\n' || column == columns) {
            x -= column * font->width;
            y += row_height;
            column = 0;
            if (*text == '\n') {
                ++text;
                continue;
            }
        }
        freak_font_char(buffer, x, y, font, *text++, colour);
        x += font->width;
        ++column;
    }
}

void freak_message_box(uint8_t *buffer, int x, int y, int width,
                       const struct freak_font *font, const char *title,
                       const char *message,
                       const struct freak_dialog_style *style) {
    int columns;
    int height;
    if (!buffer || !font || !title || !message || !style)
        return;
    columns = (width - style->padding * 2) / font->width;
    if (columns < 1)
        return;
    height = style->padding * 2 +
             style->row_height * (1 + dialog_lines(message, columns));
    dialog_frame(buffer, x, y, width, height, style);
    freak_font_text(buffer, x + style->padding, y + style->padding, font, title,
                    style->text);
    dialog_text(buffer, x + style->padding,
                y + style->padding + style->row_height, columns, font, message,
                style->text, style->row_height);
}

void freak_confirm_open(struct freak_confirm *confirm) {
    if (confirm)
        confirm->selected = 0;
}

int freak_confirm_handle_key(struct freak_confirm *confirm,
                             unsigned short key) {
    unsigned char character = (unsigned char)key;
    unsigned char scan = key >> 8;
    if (!confirm)
        return 0;
    if (scan == FREAK_KEY_LEFT || scan == FREAK_KEY_RIGHT || character == 'a' ||
        character == 'd')
        confirm->selected = !confirm->selected;
    if (character == 27)
        return FREAK_DIALOG_CANCEL;
    if (character == 13)
        return confirm->selected ? FREAK_DIALOG_CANCEL : FREAK_DIALOG_ACCEPT;
    return 0;
}

void freak_confirm_draw(uint8_t *buffer, int x, int y, int width,
                        const struct freak_font *font, const char *title,
                        const char *message, const char *accept_label,
                        const char *cancel_label,
                        const struct freak_confirm *confirm,
                        const struct freak_dialog_style *style) {
    int columns;
    int lines;
    int height;
    int buttons_y;
    int accept_x;
    int cancel_x;
    if (!buffer || !font || !title || !message || !accept_label ||
        !cancel_label || !confirm || !style)
        return;
    columns = (width - style->padding * 2) / font->width;
    if (columns < 1)
        return;
    lines = dialog_lines(message, columns);
    height = style->padding * 2 + style->row_height * (lines + 2);
    buttons_y = y + style->padding + style->row_height * (lines + 1);
    accept_x = x + width / 2 -
               (int)freak_font_measure(font, accept_label).width -
               style->padding;
    cancel_x = x + width / 2 + style->padding;
    dialog_frame(buffer, x, y, width, height, style);
    freak_font_text(buffer, x + style->padding, y + style->padding, font, title,
                    style->text);
    dialog_text(buffer, x + style->padding,
                y + style->padding + style->row_height, columns, font, message,
                style->text, style->row_height);
    if (!confirm->selected)
        freak_fill_rect(
            buffer, accept_x - 2, buttons_y - 1,
            accept_x + freak_font_measure(font, accept_label).width + 1,
            buttons_y + style->row_height - 1, style->selected_background);
    if (confirm->selected)
        freak_fill_rect(
            buffer, cancel_x - 2, buttons_y - 1,
            cancel_x + freak_font_measure(font, cancel_label).width + 1,
            buttons_y + style->row_height - 1, style->selected_background);
    freak_font_text(buffer, accept_x, buttons_y, font, accept_label,
                    !confirm->selected ? style->selected_text : style->text);
    freak_font_text(buffer, cancel_x, buttons_y, font, cancel_label,
                    confirm->selected ? style->selected_text : style->text);
}

void freak_text_input_open(struct freak_text_input *input, char *text,
                           int capacity) {
    int length = 0;
    if (!input || !text || capacity < 2)
        return;
    while (length < capacity - 1 && text[length])
        ++length;
    text[length] = 0;
    input->text = text;
    input->capacity = capacity;
    input->length = length;
    input->cursor = length;
}

int freak_text_input_handle_key(struct freak_text_input *input,
                                unsigned short key) {
    unsigned char character = (unsigned char)key;
    unsigned char scan = key >> 8;
    int i;
    if (!input || !input->text)
        return 0;
    if (character == 13)
        return FREAK_DIALOG_ACCEPT;
    if (character == 27)
        return FREAK_DIALOG_CANCEL;
    if (scan == FREAK_KEY_LEFT && input->cursor > 0)
        --input->cursor;
    else if (scan == FREAK_KEY_RIGHT && input->cursor < input->length)
        ++input->cursor;
    else if (character == 8 && input->cursor > 0) {
        for (i = input->cursor - 1; i < input->length; ++i)
            input->text[i] = input->text[i + 1];
        --input->cursor;
        --input->length;
    } else if (character >= 32 && character <= 126 &&
               input->length < input->capacity - 1) {
        for (i = input->length; i >= input->cursor; --i)
            input->text[i + 1] = input->text[i];
        input->text[input->cursor++] = character;
        ++input->length;
    }
    return 0;
}

void freak_text_input_draw(uint8_t *buffer, int x, int y, int width,
                           const struct freak_font *font, const char *title,
                           const char *prompt,
                           const struct freak_text_input *input,
                           const struct freak_dialog_style *style) {
    int text_y;
    if (!buffer || !font || !title || !prompt || !input || !input->text ||
        !style)
        return;
    dialog_frame(buffer, x, y, width,
                 style->padding * 2 + style->row_height * 3, style);
    freak_font_text(buffer, x + style->padding, y + style->padding, font, title,
                    style->text);
    text_y = y + style->padding + style->row_height;
    freak_font_text(buffer, x + style->padding, text_y, font, prompt,
                    style->text);
    freak_font_text(buffer, x + style->padding, text_y + style->row_height,
                    font, input->text, style->text);
    freak_fill_rect(buffer, x + style->padding + input->cursor * font->width,
                    text_y + style->row_height + font->height - 1,
                    x + style->padding + (input->cursor + 1) * font->width - 1,
                    text_y + style->row_height + font->height - 1,
                    style->cursor);
}
