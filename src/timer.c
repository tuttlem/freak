#include <conio.h>
#include <dos.h>
#include <i86.h>

#include "timer.h"

#define PIT_CONTROL_PORT 0x43
#define PIT_COUNTER_0    0x40
#define PIC_COMMAND_PORT 0x20
#define PIT_BASE_HZ      1193182UL
#define PIT_BIOS_DIVISOR 65536UL

static volatile unsigned long timer_ticks;
static unsigned long bios_phase;
static unsigned int timer_divisor;
static int timer_installed;
static void (__interrupt __far *previous_irq_0)(void);

static void pit_set_divisor(unsigned int divisor)
{
    outp(PIT_CONTROL_PORT, 0x36);
    outp(PIT_COUNTER_0, divisor & 0xff);
    outp(PIT_COUNTER_0, divisor >> 8);
}

static void __interrupt __far timer_irq_0(void)
{
    ++timer_ticks;
    bios_phase += timer_divisor;

    /* The old IRQ-0 handler sends the EOI when we chain to it. */
    if (bios_phase >= PIT_BIOS_DIVISOR) {
        bios_phase -= PIT_BIOS_DIVISOR;
        _chain_intr(previous_irq_0);
    }

    /* On our extra interrupts, acknowledge the PIC ourselves. */
    outp(PIC_COMMAND_PORT, 0x20);
}

void freak_timer_init(unsigned int hz)
{
    unsigned long divisor;

    if (timer_installed || hz == 0) {
        return;
    }

    divisor = PIT_BASE_HZ / hz;
    if (divisor == 0 || divisor >= PIT_BIOS_DIVISOR) {
        return;
    }

    timer_ticks = 0;
    bios_phase = 0;
    timer_divisor = (unsigned int)divisor;

    _disable();
    previous_irq_0 = _dos_getvect(0x08);
    _dos_setvect(0x08, timer_irq_0);
    pit_set_divisor(timer_divisor);
    timer_installed = 1;
    _enable();
}

unsigned long freak_timer_ticks(void)
{
    return timer_ticks;
}

void freak_timer_shutdown(void)
{
    if (!timer_installed) {
        return;
    }

    _disable();
    pit_set_divisor(0);
    _dos_setvect(0x08, previous_irq_0);
    timer_installed = 0;
    _enable();
}
