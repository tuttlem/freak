#include "profile.h"
#include "../dos/timer.h"

void freak_profiler_init(struct freak_profiler *profiler)
{
    if (profiler == 0)
        return;
    profiler->frame_started_cycles = freak_timer_cycles();
    profiler->last_frame_cycles = 0;
}

void freak_profiler_begin_frame(struct freak_profiler *profiler)
{
    if (profiler)
        profiler->frame_started_cycles = freak_timer_cycles();
}

void freak_profiler_end_frame(struct freak_profiler *profiler)
{
    if (profiler)
        profiler->last_frame_cycles = freak_timer_cycles() -
                                      profiler->frame_started_cycles;
}

void freak_profile_scope_begin(struct freak_profile_scope *scope)
{
    if (scope)
        scope->started_cycles = freak_timer_cycles();
}

void freak_profile_scope_end(struct freak_profile_scope *scope)
{
    if (scope) {
        scope->last_cycles = freak_timer_cycles() - scope->started_cycles;
        scope->total_cycles += scope->last_cycles;
        ++scope->calls;
    }
}

void freak_profile_scope_reset(struct freak_profile_scope *scope)
{
    if (scope == 0)
        return;
    scope->started_cycles = 0;
    scope->last_cycles = 0;
    scope->total_cycles = 0;
    scope->calls = 0;
}

unsigned long freak_profile_microseconds(unsigned long pit_cycles)
{
    return (pit_cycles / 1193UL) * 1000UL +
           ((pit_cycles % 1193UL) * 1000UL) / 1193UL;
}
