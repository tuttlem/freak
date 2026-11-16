#include "timing.h"
#include "../dos/timer.h"

uint8_t freak_timing_start(struct freak_timing *timing, unsigned int tick_hz)
{
    if (timing == 0 || tick_hz < 19)
        return 0;
    freak_timer_init(tick_hz);
    timing->tick_hz = tick_hz;
    timing->last_tick = freak_timer_ticks();
    return 1;
}

void freak_timing_stop(void)
{
    freak_timer_shutdown();
}

unsigned long freak_timing_update(struct freak_timing *timing)
{
    unsigned long now;
    unsigned long elapsed;

    if (timing == 0)
        return 0;
    now = freak_timer_ticks();
    elapsed = now - timing->last_tick;
    timing->last_tick = now;
    return elapsed;
}

void freak_timing_wait_until(unsigned long target_tick)
{
    while ((long)(freak_timer_ticks() - target_tick) < 0) {}
}

void freak_fixed_step_init(struct freak_fixed_step *step,
                           unsigned int ticks_per_step,
                           unsigned int max_steps)
{
    if (step == 0)
        return;
    step->ticks_per_step = ticks_per_step == 0 ? 1 : ticks_per_step;
    step->max_steps = max_steps == 0 ? 1 : max_steps;
    step->accumulated_ticks = 0;
}

void freak_fixed_step_add(struct freak_fixed_step *step,
                          unsigned long elapsed_ticks)
{
    unsigned long maximum;

    if (step == 0)
        return;
    maximum = (unsigned long)step->ticks_per_step * step->max_steps;
    step->accumulated_ticks += elapsed_ticks;
    /* Drop excess catch-up work rather than letting a slow frame spiral. */
    if (step->accumulated_ticks > maximum)
        step->accumulated_ticks = maximum;
}

uint8_t freak_fixed_step_take(struct freak_fixed_step *step)
{
    if (step == 0 || step->accumulated_ticks < step->ticks_per_step)
        return 0;
    step->accumulated_ticks -= step->ticks_per_step;
    return 1;
}

uint16_t freak_fixed_step_alpha(const struct freak_fixed_step *step)
{
    if (step == 0 || step->ticks_per_step == 0)
        return 0;
    return (uint16_t)((step->accumulated_ticks * 256UL) /
                      step->ticks_per_step);
}
