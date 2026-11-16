#include <conio.h>
#include <dos.h>
#include <i86.h>
#include <stdint.h>

#include "keyboard.h"

#define KEYBOARD_DATA_PORT 0x60
#define PIC_COMMAND_PORT   0x20

static volatile uint8_t key_state[128];
static int keyboard_installed;
static unsigned short previous_selector;
static unsigned long previous_offset;

static void __interrupt keyboard_irq_1(void)
{
    uint8_t scan_code;

    scan_code = (uint8_t)inp(KEYBOARD_DATA_PORT);

    if (scan_code != 0xe0 && scan_code != 0xe1) {
        key_state[scan_code & 0x7f] = (scan_code & 0x80) == 0;
    }

    outp(PIC_COMMAND_PORT, 0x20);
}

void freak_kbd_init(void)
{
    unsigned int index;
    union REGS regs;
    void __far *handler;

    if (keyboard_installed) {
        return;
    }

    for (index = 0; index < 128; ++index) {
        key_state[index] = 0;
    }
    /* DOS/4GW requires DPMI protected-mode vector services for IRQ hooks. */
    regs.x.eax = 0x0204;
    regs.x.ebx = 0x09;
    int386(0x31, &regs, &regs);
    if (regs.x.cflag) {
        return;
    }
    previous_selector = (unsigned short)regs.x.ecx;
    previous_offset = regs.x.edx;

    handler = (void __far *)keyboard_irq_1;
    regs.x.eax = 0x0205;
    regs.x.ebx = 0x09;
    regs.x.ecx = (unsigned long)FP_SEG(handler);
    regs.x.edx = FP_OFF(handler);
    int386(0x31, &regs, &regs);
    if (regs.x.cflag) {
        return;
    }
    keyboard_installed = 1;
}

void freak_kbd_shutdown(void)
{
    union REGS regs;

    if (!keyboard_installed) {
        return;
    }

    regs.x.eax = 0x0205;
    regs.x.ebx = 0x09;
    regs.x.ecx = (unsigned long)previous_selector;
    regs.x.edx = previous_offset;
    int386(0x31, &regs, &regs);
    keyboard_installed = 0;
}

uint8_t freak_key_down(uint8_t scan_code)
{
    return key_state[scan_code & 0x7f];
}
