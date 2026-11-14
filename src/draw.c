#include "draw.h"

void freak_draw_pixel(uint8_t *buffer, int x, int y, uint8_t colour)
{
    if (x < 0 || x >= FREAK_SCREEN_WIDTH ||
        y < 0 || y >= FREAK_SCREEN_HEIGHT) {
        return;
    }
    buffer[x + y * FREAK_SCREEN_WIDTH] = colour;
}

void freak_draw_hline(uint8_t *buffer, int y, int left, int right,
                      uint8_t colour)
{
    int x;

    if (y < 0 || y >= FREAK_SCREEN_HEIGHT || right < 0 ||
        left >= FREAK_SCREEN_WIDTH) {
        return;
    }
    if (left < 0) {
        left = 0;
    }
    if (right >= FREAK_SCREEN_WIDTH) {
        right = FREAK_SCREEN_WIDTH - 1;
    }
    for (x = left; x <= right; ++x) {
        buffer[x + y * FREAK_SCREEN_WIDTH] = colour;
    }
}

void freak_fill_rect(uint8_t *buffer, int left, int top, int right,
                     int bottom, uint8_t colour)
{
    int y;

    for (y = top; y <= bottom; ++y) {
        freak_draw_hline(buffer, y, left, right, colour);
    }
}

void freak_draw_line(uint8_t *buffer, int x0, int y0, int x1, int y1,
                     uint8_t colour)
{
    int dx;
    int dy;
    int sx;
    int sy;
    int error;
    int twice_error;

    dx = x0 < x1 ? x1 - x0 : x0 - x1;
    dy = y0 < y1 ? y1 - y0 : y0 - y1;
    sx = x0 < x1 ? 1 : -1;
    sy = y0 < y1 ? 1 : -1;
    error = dx - dy;

    for (;;) {
        freak_draw_pixel(buffer, x0, y0, colour);
        if (x0 == x1 && y0 == y1) {
            return;
        }
        twice_error = error * 2;
        if (twice_error > -dy) {
            error -= dy;
            x0 += sx;
        }
        if (twice_error < dx) {
            error += dx;
            y0 += sy;
        }
    }
}
