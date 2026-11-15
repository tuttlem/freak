#ifndef __freak_dialog_h_
#define __freak_dialog_h_

#include <stdint.h>

#include "../render/font.h"

#define FREAK_DIALOG_ACCEPT 1
#define FREAK_DIALOG_CANCEL -1

struct freak_dialog_style {
    uint8_t background;
    uint8_t border;
    uint8_t text;
    uint8_t selected_text;
    uint8_t selected_background;
    uint8_t cursor;
    int padding;
    int row_height;
};

struct freak_confirm {
    int selected;
};

struct freak_text_input {
    char *text;
    int capacity;
    int length;
    int cursor;
};

/* Draws an informational box, wrapping text to its fixed character columns. */
void freak_message_box(uint8_t *buffer, int x, int y, int width,
                       const struct freak_font *font, const char *title,
                       const char *message,
                       const struct freak_dialog_style *style);

/* A tiny two-button state machine. Handle_key returns ACCEPT or CANCEL. */
void freak_confirm_open(struct freak_confirm *confirm);
int freak_confirm_handle_key(struct freak_confirm *confirm, unsigned short key);
void freak_confirm_draw(uint8_t *buffer, int x, int y, int width,
                        const struct freak_font *font, const char *title,
                        const char *message, const char *accept_label,
                        const char *cancel_label,
                        const struct freak_confirm *confirm,
                        const struct freak_dialog_style *style);

/* An editable, single-line ASCII field. The caller owns its character buffer.
 */
void freak_text_input_open(struct freak_text_input *input, char *text,
                           int capacity);
int freak_text_input_handle_key(struct freak_text_input *input,
                                unsigned short key);
void freak_text_input_draw(uint8_t *buffer, int x, int y, int width,
                           const struct freak_font *font, const char *title,
                           const char *prompt,
                           const struct freak_text_input *input,
                           const struct freak_dialog_style *style);

#endif
