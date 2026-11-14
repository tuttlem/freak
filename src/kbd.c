#include <conio.h>
#include <dos.h>
#include <i86.h>
#include <stdint.h>

#include "kbd.h"

#define KEYBOARD_DATA_PORT 0x60
#define PIC_COMMAND_PORT   0x20

static volatile uint8_t key_state[128];
static uint8_t extended_prefix;
static int keyboard_installed;
static void (__interrupt __far *previous_irq_1)(void);

static void __interrupt __far keyboard_irq_1(void)
{
    uint8_t scan_code;

    scan_code = (uint8_t)inp(KEYBOARD_DATA_PORT);

    if (scan_code == 0xe0) {
        extended_prefix = 1;
    } else if (scan_code != 0xe1) {
        key_state[scan_code & 0x7f] = (scan_code & 0x80) == 0;
        extended_prefix = 0;
    }

    outp(PIC_COMMAND_PORT, 0x20);
}

void freak_kbd_init(void)
{
    unsigned int index;

    if (keyboard_installed) {
        return;
    }

    for (index = 0; index < 128; ++index) {
        key_state[index] = 0;
    }
    extended_prefix = 0;

    _disable();
    previous_irq_1 = _dos_getvect(0x09);
    _dos_setvect(0x09, keyboard_irq_1);
    keyboard_installed = 1;
    _enable();
}

void freak_kbd_shutdown(void)
{
    if (!keyboard_installed) {
        return;
    }

    _disable();
    _dos_setvect(0x09, previous_irq_1);
    keyboard_installed = 0;
    _enable();
}

uint8_t freak_key_down(uint8_t scan_code)
{
    return key_state[scan_code & 0x7f];
}
