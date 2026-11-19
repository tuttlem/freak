#ifndef __freak_xform_h_
#define __freak_xform_h_

#include <stdint.h>
#include "texture.h"

void freak_xform_scale(uint8_t *output, const struct freak_texture *texture,
                       int left, int top, int width, int height);
void freak_xform_rotozoom(uint8_t *output, const struct freak_texture *texture,
                          int center_x, int center_y, int angle, int zoom);
void freak_xform_tunnel(uint8_t *output, const struct freak_texture *texture,
                        int center_x, int center_y, unsigned int phase);
void freak_xform_floor(uint8_t *output, const struct freak_texture *texture,
                       int horizon, unsigned int phase);

#endif
