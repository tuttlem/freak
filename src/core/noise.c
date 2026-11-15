#include "noise.h"

int freak_noise_2d(int x, int y)
{
    unsigned long value;

    value = (unsigned long)(x + y * 57);
    value = (value << 13) ^ value;
    value = value * (value * value * 15731UL + 789221UL) + 1376312589UL;
    return (int)((value >> 16) & 0xffffUL) - 32768;
}

int freak_noise_smooth_2d(int x, int y)
{
    int corners;
    int sides;
    int centre;

    corners = freak_noise_2d(x - 1, y - 1) +
              freak_noise_2d(x + 1, y - 1) +
              freak_noise_2d(x - 1, y + 1) +
              freak_noise_2d(x + 1, y + 1);
    sides = freak_noise_2d(x - 1, y) + freak_noise_2d(x + 1, y) +
            freak_noise_2d(x, y - 1) + freak_noise_2d(x, y + 1);
    centre = freak_noise_2d(x, y);
    return corners / 16 + sides / 8 + centre / 4;
}
