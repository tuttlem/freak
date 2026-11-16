#include <dos.h>
#include <i86.h>

#include "mouse.h"

static uint8_t installed;
static uint8_t visible;
static int mouse_x;
static int mouse_y;
static uint16_t buttons;
static uint16_t previous_buttons;

static void mouse_call(unsigned short function, union REGS *regs)
{
    regs->w.ax = function;
    int386(0x33, regs, regs);
}

uint8_t freak_mouse_init(void)
{
    union REGS regs;

    regs.w.ax = 0;
    int386(0x33, &regs, &regs);
    installed = regs.w.ax != 0;
    visible = 0;
    mouse_x = 160;
    mouse_y = 100;
    buttons = 0;
    previous_buttons = 0;
    if (!installed)
        return 0;

    regs.w.cx = 0;
    /* DOS mouse drivers conventionally use 640 horizontal units in mode 13h. */
    regs.w.dx = 639;
    mouse_call(7, &regs);
    regs.w.cx = 0;
    regs.w.dx = 199;
    mouse_call(8, &regs);
    freak_mouse_update();
    return 1;
}

void freak_mouse_shutdown(void)
{
    freak_mouse_hide();
    installed = 0;
}

void freak_mouse_update(void)
{
    union REGS regs;

    if (!installed)
        return;
    previous_buttons = buttons;
    mouse_call(3, &regs);
    buttons = regs.w.bx;
    mouse_x = regs.w.cx >> 1;
    mouse_y = regs.w.dx;
    if (mouse_x < 0) mouse_x = 0;
    if (mouse_x > 319) mouse_x = 319;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_y > 199) mouse_y = 199;
}

uint8_t freak_mouse_available(void) { return installed; }
int freak_mouse_x(void) { return mouse_x; }
int freak_mouse_y(void) { return mouse_y; }
uint16_t freak_mouse_buttons(void) { return buttons; }
uint8_t freak_mouse_down(uint16_t button) { return (buttons & button) != 0; }
uint8_t freak_mouse_pressed(uint16_t button)
{
    return (buttons & button) != 0 && (previous_buttons & button) == 0;
}
uint8_t freak_mouse_released(uint16_t button)
{
    return (buttons & button) == 0 && (previous_buttons & button) != 0;
}

void freak_mouse_show(void)
{
    union REGS regs;

    if (!installed || visible)
        return;
    mouse_call(1, &regs);
    visible = 1;
}

void freak_mouse_hide(void)
{
    union REGS regs;

    if (!installed || !visible)
        return;
    mouse_call(2, &regs);
    visible = 0;
}
