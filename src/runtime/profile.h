#ifndef __freak_profile_h_
#define __freak_profile_h_

#include <stdint.h>

struct freak_profile_scope {
    unsigned long started_cycles;
    unsigned long last_cycles;
    unsigned long total_cycles;
    unsigned int calls;
};

struct freak_profiler {
    unsigned long frame_started_cycles;
    unsigned long last_frame_cycles;
};

void freak_profiler_init(struct freak_profiler *profiler);
void freak_profiler_begin_frame(struct freak_profiler *profiler);
void freak_profiler_end_frame(struct freak_profiler *profiler);
void freak_profile_scope_begin(struct freak_profile_scope *scope);
void freak_profile_scope_end(struct freak_profile_scope *scope);
void freak_profile_scope_reset(struct freak_profile_scope *scope);
unsigned long freak_profile_microseconds(unsigned long pit_cycles);

#endif
