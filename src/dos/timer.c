#include <conio.h>
#include <dos.h>
#include <i86.h>

#include "timer.h"

#define PIT_CONTROL_PORT 0x43
#define PIT_COUNTER_0    0x40
#define PIC_COMMAND_PORT 0x20
#define PIT_BASE_HZ      1193182UL

static volatile unsigned long timer_ticks;
static unsigned int timer_divisor;
static int timer_installed;
static unsigned short previous_selector;
static unsigned long previous_offset;

static void pit_set_divisor(unsigned int divisor)
{
    outp(PIT_CONTROL_PORT, 0x36);
    outp(PIT_COUNTER_0, divisor & 0xff);
    outp(PIT_COUNTER_0, divisor >> 8);
}

static void __interrupt timer_irq_0(void)
{
    ++timer_ticks;

    /* The protected-mode handler owns this IRQ while the game is active. */
    outp(PIC_COMMAND_PORT, 0x20);
}

void freak_timer_init(unsigned int hz)
{
    unsigned long divisor;
    union REGS regs;
    void __far *handler;

    if (timer_installed || hz == 0) {
        return;
    }

    divisor = PIT_BASE_HZ / hz;
    if (divisor == 0 || divisor >= 65536UL) {
        return;
    }

    timer_ticks = 0;
    timer_divisor = (unsigned int)divisor;

    regs.x.eax = 0x0204;
    regs.x.ebx = 0x08;
    int386(0x31, &regs, &regs);
    if (regs.x.cflag) {
        return;
    }
    previous_selector = (unsigned short)regs.x.ecx;
    previous_offset = regs.x.edx;

    handler = (void __far *)timer_irq_0;
    regs.x.eax = 0x0205;
    regs.x.ebx = 0x08;
    regs.x.ecx = (unsigned long)FP_SEG(handler);
    regs.x.edx = FP_OFF(handler);
    int386(0x31, &regs, &regs);
    if (regs.x.cflag) {
        return;
    }
    pit_set_divisor(timer_divisor);
    timer_installed = 1;
}

unsigned long freak_timer_ticks(void)
{
    return timer_ticks;
}

unsigned long freak_timer_cycles(void)
{
    unsigned long first_tick;
    unsigned long last_tick;
    unsigned int counter;

    if (!timer_installed)
        return 0;
    do {
        first_tick = timer_ticks;
        /* Latch counter 0 without changing the programmed PIT mode. */
        outp(PIT_CONTROL_PORT, 0x00);
        counter = (unsigned int)inp(PIT_COUNTER_0);
        counter |= (unsigned int)inp(PIT_COUNTER_0) << 8;
        last_tick = timer_ticks;
    } while (first_tick != last_tick);
    return first_tick * timer_divisor + (timer_divisor - counter);
}

void freak_timer_shutdown(void)
{
    union REGS regs;

    if (!timer_installed) {
        return;
    }

    pit_set_divisor(0);
    regs.x.eax = 0x0205;
    regs.x.ebx = 0x08;
    regs.x.ecx = (unsigned long)previous_selector;
    regs.x.edx = previous_offset;
    int386(0x31, &regs, &regs);
    timer_installed = 0;
}
