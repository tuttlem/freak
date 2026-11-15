/******************************************************************************
 * Freak drawing primitives
 ******************************************************************************/

#ifndef __freak_draw_h_
#define __freak_draw_h_

#include <stdint.h>

#define FREAK_SCREEN_WIDTH  320
#define FREAK_SCREEN_HEIGHT 200

void freak_draw_pixel(uint8_t *buffer, int x, int y, uint8_t colour);
void freak_draw_hline(uint8_t *buffer, int y, int left, int right,
                      uint8_t colour);
void freak_fill_rect(uint8_t *buffer, int left, int top, int right,
                     int bottom, uint8_t colour);
void freak_draw_line(uint8_t *buffer, int x0, int y0, int x1, int y1,
                     uint8_t colour);

#endif /* __freak_draw_h_ */
