#include "debug.h"
#include "draw.h"

void freak_debug_cross(uint8_t *buffer, int x, int y, int radius,
                       uint8_t colour) {
    freak_draw_hline(buffer, y, x - radius, x + radius, colour);
    freak_draw_line(buffer, x, y - radius, x, y + radius, colour);
}

void freak_debug_box(uint8_t *buffer, int left, int top, int right, int bottom,
                     uint8_t colour) {
    freak_draw_line(buffer, left, top, right, top, colour);
    freak_draw_line(buffer, right, top, right, bottom, colour);
    freak_draw_line(buffer, right, bottom, left, bottom, colour);
    freak_draw_line(buffer, left, bottom, left, top, colour);
}
