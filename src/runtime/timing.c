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

void freak_frame_pacer_init(struct freak_frame_pacer *pacer,
                            unsigned int tick_hz, unsigned int target_fps)
{
    if (pacer == 0)
        return;
    pacer->tick_hz = tick_hz == 0 ? 1 : tick_hz;
    pacer->next_tick = freak_timer_ticks();
    pacer->sample_tick = pacer->next_tick;
    pacer->sample_frames = 0;
    pacer->measured_fps = 0;
    freak_frame_pacer_set_fps(pacer, target_fps);
}

void freak_frame_pacer_set_fps(struct freak_frame_pacer *pacer,
                               unsigned int target_fps)
{
    if (pacer == 0)
        return;
    if (target_fps == 0 || target_fps > pacer->tick_hz)
        target_fps = pacer->tick_hz;
    pacer->target_fps = target_fps;
    pacer->ticks_per_frame = pacer->tick_hz / target_fps;
    if (pacer->ticks_per_frame == 0)
        pacer->ticks_per_frame = 1;
    pacer->next_tick = freak_timer_ticks();
}

void freak_frame_pacer_wait(struct freak_frame_pacer *pacer)
{
    unsigned long now;
    unsigned long elapsed;

    if (pacer == 0)
        return;
    pacer->next_tick += pacer->ticks_per_frame;
    now = freak_timer_ticks();
    if ((long)(now - pacer->next_tick) < 0)
        freak_timing_wait_until(pacer->next_tick);
    else
        pacer->next_tick = now;

    ++pacer->sample_frames;
    now = freak_timer_ticks();
    elapsed = now - pacer->sample_tick;
    if (elapsed >= pacer->tick_hz) {
        pacer->measured_fps = (unsigned int)
            ((unsigned long)pacer->sample_frames * pacer->tick_hz / elapsed);
        pacer->sample_frames = 0;
        pacer->sample_tick = now;
    }
}

unsigned int freak_frame_pacer_fps(const struct freak_frame_pacer *pacer)
{
    return pacer == 0 ? 0 : pacer->measured_fps;
}
