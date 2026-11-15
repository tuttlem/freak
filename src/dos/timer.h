/******************************************************************************
 * Freak timer
 *
 * A fixed-rate PIT clock for DOS/4GW programs.  It owns IRQ 0 while active
 * and restores the normal BIOS PIT rate and vector during shutdown.
 ******************************************************************************/

#ifndef __freak_timer_h_
#define __freak_timer_h_

/* Starts the timer at hz ticks per second.  One call is sufficient. */
void freak_timer_init(unsigned int hz);

/* Returns the monotonically increasing number of timer ticks. */
unsigned long freak_timer_ticks(void);

/* Restores the BIOS PIT rate and interrupt handler. */
void freak_timer_shutdown(void);

#endif /* __freak_timer_h_ */
