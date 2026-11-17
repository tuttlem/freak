#ifndef __freak_timing_h_
#define __freak_timing_h_

#include <stdint.h>

/* A monotonic PIT-backed clock. Call update once per outer frame. */
struct freak_timing {
    unsigned int tick_hz;
    unsigned long last_tick;
};

/* A deterministic simulation clock driven by elapsed PIT ticks. */
struct freak_fixed_step {
    unsigned int ticks_per_step;
    unsigned int max_steps;
    unsigned long accumulated_ticks;
};

struct freak_frame_pacer {
    unsigned int tick_hz;
    unsigned int target_fps;
    unsigned int ticks_per_frame;
    unsigned long next_tick;
    unsigned long sample_tick;
    unsigned int sample_frames;
    unsigned int measured_fps;
};

uint8_t freak_timing_start(struct freak_timing *timing, unsigned int tick_hz);
void freak_timing_stop(void);
unsigned long freak_timing_update(struct freak_timing *timing);
void freak_timing_wait_until(unsigned long target_tick);

void freak_fixed_step_init(struct freak_fixed_step *step,
                           unsigned int ticks_per_step,
                           unsigned int max_steps);
void freak_fixed_step_add(struct freak_fixed_step *step,
                          unsigned long elapsed_ticks);
uint8_t freak_fixed_step_take(struct freak_fixed_step *step);
uint16_t freak_fixed_step_alpha(const struct freak_fixed_step *step);

void freak_frame_pacer_init(struct freak_frame_pacer *pacer,
                            unsigned int tick_hz, unsigned int target_fps);
void freak_frame_pacer_set_fps(struct freak_frame_pacer *pacer,
                               unsigned int target_fps);
void freak_frame_pacer_wait(struct freak_frame_pacer *pacer);
unsigned int freak_frame_pacer_fps(const struct freak_frame_pacer *pacer);

#endif
