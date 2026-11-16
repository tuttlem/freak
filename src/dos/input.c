#include "input.h"
#include "kbd.h"
#include "mouse.h"

static uint8_t previous_state[128];
static uint8_t current_state[128];

struct freak_input_bindings freak_input_default_bindings;

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
    }
    freak_mouse_update();
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
