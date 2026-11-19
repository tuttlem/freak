#ifndef __freak_democom_h_
#define __freak_democom_h_

#include "../democtx.h"

extern unsigned char *screen;
extern struct freak_font font;
extern struct freak_timing demo_timing;
void freak_demo_palette_default(void);
extern const struct freak_sprite runner_frames[4];
extern const struct freak_sprite tile_sprites[5];

#endif
