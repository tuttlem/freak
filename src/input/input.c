#include "input.h"
#include "keyboard.h"
#include "mouse.h"

static uint8_t previous_state[128];
static uint8_t current_state[128];
static uint8_t previous_actions[FREAK_ACTION_COUNT];
static uint8_t current_actions[FREAK_ACTION_COUNT];
static struct freak_input_event event_queue[FREAK_INPUT_EVENT_QUEUE_SIZE];
static unsigned int event_read;
static unsigned int event_write;
static int previous_mouse_x;
static int previous_mouse_y;

struct freak_input_bindings freak_input_default_bindings;

static void push_event(enum freak_input_event_type type, uint8_t scan_code,
                       enum freak_input_action action, uint16_t button)
{
    struct freak_input_event *event;
    unsigned int next;

    next = (event_write + 1U) % FREAK_INPUT_EVENT_QUEUE_SIZE;
    if (next == event_read)
        event_read = (event_read + 1U) % FREAK_INPUT_EVENT_QUEUE_SIZE;
    event = &event_queue[event_write];
    event->type = type;
    event->scan_code = scan_code;
    event->action = action;
    event->button = button;
    event->mouse_x = freak_mouse_x();
    event->mouse_y = freak_mouse_y();
    event_write = next;
}

static uint8_t binding_state(const struct freak_input_bindings *bindings,
                             enum freak_input_action action,
                             uint8_t (*query)(uint8_t))
{
    uint8_t primary;
    uint8_t secondary;

    if (bindings == 0 || action < 0 || action >= FREAK_ACTION_COUNT)
        return 0;
    primary = bindings->primary[action];
    secondary = bindings->secondary[action];
    return (primary != FREAK_INPUT_UNBOUND && query(primary)) ||
           (secondary != FREAK_INPUT_UNBOUND && query(secondary));
}

void freak_input_bind(struct freak_input_bindings *bindings,
                      enum freak_input_action action,
                      uint8_t primary, uint8_t secondary)
{
    if (bindings == 0 || action < 0 || action >= FREAK_ACTION_COUNT)
        return;
    bindings->primary[action] = primary;
    bindings->secondary[action] = secondary;
}

void freak_input_init(void)
{
    int index;

    for (index = 0; index < 128; ++index) {
        previous_state[index] = 0;
        current_state[index] = 0;
    }
    for (index = 0; index < FREAK_ACTION_COUNT; ++index) {
        previous_actions[index] = 0;
        current_actions[index] = 0;
    }
    event_read = 0;
    event_write = 0;
    for (index = 0; index < FREAK_INPUT_MAX_ACTIONS; ++index)
        freak_input_bind(&freak_input_default_bindings,
                         (enum freak_input_action)index,
                         FREAK_INPUT_UNBOUND, FREAK_INPUT_UNBOUND);

    freak_input_bind(&freak_input_default_bindings, FREAK_ACTION_UP,
                     FREAK_KEY_UP, FREAK_KEY_W);
    freak_input_bind(&freak_input_default_bindings, FREAK_ACTION_DOWN,
                     FREAK_KEY_DOWN, FREAK_KEY_S);
    freak_input_bind(&freak_input_default_bindings, FREAK_ACTION_LEFT,
                     FREAK_KEY_LEFT, FREAK_KEY_A);
    freak_input_bind(&freak_input_default_bindings, FREAK_ACTION_RIGHT,
                     FREAK_KEY_RIGHT, FREAK_KEY_D);
    freak_input_bind(&freak_input_default_bindings, FREAK_ACTION_ACCEPT,
                     FREAK_KEY_ENTER, FREAK_INPUT_UNBOUND);
    freak_input_bind(&freak_input_default_bindings, FREAK_ACTION_BACK,
                     FREAK_KEY_ESCAPE, FREAK_KEY_BACKSPACE);
    freak_kbd_init();
    freak_mouse_init();
    freak_mouse_show();
    previous_mouse_x = freak_mouse_x();
    previous_mouse_y = freak_mouse_y();
}

void freak_input_shutdown(void)
{
    freak_mouse_shutdown();
    freak_kbd_shutdown();
}

void freak_input_update(void)
{
    int index;

    for (index = 0; index < 128; ++index) {
        previous_state[index] = current_state[index];
        current_state[index] = freak_key_down((uint8_t)index);
        if (current_state[index] && !previous_state[index])
            push_event(FREAK_EVENT_KEY_DOWN, (uint8_t)index,
                       FREAK_ACTION_COUNT, 0);
        else if (!current_state[index] && previous_state[index])
            push_event(FREAK_EVENT_KEY_UP, (uint8_t)index,
                       FREAK_ACTION_COUNT, 0);
    }
    for (index = 0; index < FREAK_ACTION_COUNT; ++index) {
        previous_actions[index] = current_actions[index];
        current_actions[index] = freak_input_down(&freak_input_default_bindings,
                                                   (enum freak_input_action)index);
        if (current_actions[index] && !previous_actions[index])
            push_event(FREAK_EVENT_ACTION_PRESSED, 0,
                       (enum freak_input_action)index, 0);
        else if (!current_actions[index] && previous_actions[index])
            push_event(FREAK_EVENT_ACTION_RELEASED, 0,
                       (enum freak_input_action)index, 0);
    }
    freak_mouse_update();
    if (freak_mouse_x() != previous_mouse_x || freak_mouse_y() != previous_mouse_y) {
        push_event(FREAK_EVENT_MOUSE_MOVE, 0, FREAK_ACTION_COUNT, 0);
        previous_mouse_x = freak_mouse_x();
        previous_mouse_y = freak_mouse_y();
    }
    if (freak_mouse_pressed(FREAK_MOUSE_LEFT))
        push_event(FREAK_EVENT_MOUSE_BUTTON_DOWN, 0, FREAK_ACTION_COUNT,
                   FREAK_MOUSE_LEFT);
    if (freak_mouse_released(FREAK_MOUSE_LEFT))
        push_event(FREAK_EVENT_MOUSE_BUTTON_UP, 0, FREAK_ACTION_COUNT,
                   FREAK_MOUSE_LEFT);
    if (freak_mouse_pressed(FREAK_MOUSE_RIGHT))
        push_event(FREAK_EVENT_MOUSE_BUTTON_DOWN, 0, FREAK_ACTION_COUNT,
                   FREAK_MOUSE_RIGHT);
    if (freak_mouse_released(FREAK_MOUSE_RIGHT))
        push_event(FREAK_EVENT_MOUSE_BUTTON_UP, 0, FREAK_ACTION_COUNT,
                   FREAK_MOUSE_RIGHT);
    if (freak_mouse_pressed(FREAK_MOUSE_MIDDLE))
        push_event(FREAK_EVENT_MOUSE_BUTTON_DOWN, 0, FREAK_ACTION_COUNT,
                   FREAK_MOUSE_MIDDLE);
    if (freak_mouse_released(FREAK_MOUSE_MIDDLE))
        push_event(FREAK_EVENT_MOUSE_BUTTON_UP, 0, FREAK_ACTION_COUNT,
                   FREAK_MOUSE_MIDDLE);
}

uint8_t freak_input_poll_event(struct freak_input_event *event)
{
    if (event == 0 || event_read == event_write)
        return 0;
    *event = event_queue[event_read];
    event_read = (event_read + 1U) % FREAK_INPUT_EVENT_QUEUE_SIZE;
    return 1;
}

void freak_input_clear_events(void)
{
    event_read = event_write;
}

uint8_t freak_input_key_down(uint8_t scan_code)
{
    return current_state[scan_code & 0x7f];
}

uint8_t freak_input_key_pressed(uint8_t scan_code)
{
    scan_code &= 0x7f;
    return current_state[scan_code] && !previous_state[scan_code];
}

uint8_t freak_input_key_released(uint8_t scan_code)
{
    scan_code &= 0x7f;
    return !current_state[scan_code] && previous_state[scan_code];
}

uint8_t freak_input_down(const struct freak_input_bindings *bindings,
                         enum freak_input_action action)
{
    return binding_state(bindings, action, freak_input_key_down);
}

uint8_t freak_input_pressed(const struct freak_input_bindings *bindings,
                            enum freak_input_action action)
{
    return binding_state(bindings, action, freak_input_key_pressed);
}
