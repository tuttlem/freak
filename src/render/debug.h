#ifndef __freak_debug_h_
#define __freak_debug_h_
#include <stdint.h>
void freak_debug_cross(uint8_t *buffer, int x, int y, int radius,
                       uint8_t colour);
void freak_debug_box(uint8_t *buffer, int left, int top, int right, int bottom,
                     uint8_t colour);
#endif
