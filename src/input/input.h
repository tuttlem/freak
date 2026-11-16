#ifndef __freak_input_h_
#define __freak_input_h_

#include <stdint.h>

#define FREAK_INPUT_MAX_ACTIONS 16
#define FREAK_INPUT_UNBOUND     0xff
#define FREAK_INPUT_EVENT_QUEUE_SIZE 64

enum freak_input_action {
    FREAK_ACTION_UP,
    FREAK_ACTION_DOWN,
    FREAK_ACTION_LEFT,
    FREAK_ACTION_RIGHT,
    FREAK_ACTION_ACCEPT,
    FREAK_ACTION_BACK,
    FREAK_ACTION_COUNT
};

struct freak_input_bindings {
    uint8_t primary[FREAK_INPUT_MAX_ACTIONS];
    uint8_t secondary[FREAK_INPUT_MAX_ACTIONS];
};

enum freak_input_event_type {
    FREAK_EVENT_KEY_DOWN,
    FREAK_EVENT_KEY_UP,
    FREAK_EVENT_ACTION_PRESSED,
    FREAK_EVENT_ACTION_RELEASED,
    FREAK_EVENT_MOUSE_MOVE,
    FREAK_EVENT_MOUSE_BUTTON_DOWN,
    FREAK_EVENT_MOUSE_BUTTON_UP
};

struct freak_input_event {
    enum freak_input_event_type type;
    uint8_t scan_code;
    enum freak_input_action action;
    uint16_t button;
    int mouse_x;
    int mouse_y;
};

/* Install the raw keyboard handler and initialise the default bindings. */
void freak_input_init(void);
void freak_input_shutdown(void);

/* Take one snapshot per frame before querying pressed/released state. */
void freak_input_update(void);
uint8_t freak_input_poll_event(struct freak_input_event *event);
void freak_input_clear_events(void);

uint8_t freak_input_key_down(uint8_t scan_code);
uint8_t freak_input_key_pressed(uint8_t scan_code);
uint8_t freak_input_key_released(uint8_t scan_code);

void freak_input_bind(struct freak_input_bindings *bindings,
                      enum freak_input_action action,
                      uint8_t primary, uint8_t secondary);
uint8_t freak_input_down(const struct freak_input_bindings *bindings,
                         enum freak_input_action action);
uint8_t freak_input_pressed(const struct freak_input_bindings *bindings,
                            enum freak_input_action action);

extern struct freak_input_bindings freak_input_default_bindings;

#endif
