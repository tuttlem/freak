#ifndef __freak_demoaud_h_
#define __freak_demoaud_h_

#include "democtx.h"

void freak_demo_pc_speaker(void);
void freak_demo_sound_blaster(const struct freak_demo_context *context);
void freak_demo_midi(const struct freak_demo_context *context);

#endif
