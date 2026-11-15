#include "noise.h"

static int noise_floor_div(int value, int divisor)
{
    if (value < 0) {
        return -(((-value) + divisor - 1) / divisor);
    }
    return value / divisor;
}

/* The half-range keeps delta * fraction safely inside a signed 32-bit long. */
static int noise_lerp(int from, int to, int fraction)
{
    int from_half;
    int to_half;

    from_half = from / 2;
    to_half = to / 2;
    return (from_half +
            (int)(((long)(to_half - from_half) * fraction) >> 16)) * 2;
}

static int noise_fade(int fraction)
{
    long value;
    long slope;

    /* f*f*(3-2*f), kept in range for Watcom's 32-bit long. */
    value = ((long)(fraction >> 1) * fraction) >> 15;
    slope = (196608L - ((long)fraction << 1)) >> 2;
    return (int)(((value >> 1) * slope) >> 13);
}

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

int freak_noise_value_2d(int x, int y, int scale)
{
    int cell_x;
    int cell_y;
    int local_x;
    int local_y;
    int fraction_x;
    int fraction_y;
    int top;
    int bottom;

    if (scale < 1) {
        scale = 1;
    }
    if (scale > 32767) {
        scale = 32767;
    }
    cell_x = noise_floor_div(x, scale);
    cell_y = noise_floor_div(y, scale);
    local_x = x - cell_x * scale;
    local_y = y - cell_y * scale;
    fraction_x = (int)(((long)local_x << 16) / scale);
    fraction_y = (int)(((long)local_y << 16) / scale);
    fraction_x = noise_fade(fraction_x);
    fraction_y = noise_fade(fraction_y);
    top = noise_lerp(freak_noise_2d(cell_x, cell_y),
                     freak_noise_2d(cell_x + 1, cell_y), fraction_x);
    bottom = noise_lerp(freak_noise_2d(cell_x, cell_y + 1),
                        freak_noise_2d(cell_x + 1, cell_y + 1), fraction_x);
    return noise_lerp(top, bottom, fraction_y);
}

int freak_noise_fractal_2d(int x, int y, int scale, int octaves,
                           unsigned int persistence)
{
    int octave;
    int amplitude;
    int amplitude_total;
    long total;

    if (scale < 1) {
        scale = 1;
    }
    if (scale > 32767) {
        scale = 32767;
    }
    if (octaves < 1) {
        octaves = 1;
    }
    if (octaves > 8) {
        octaves = 8;
    }
    if (persistence > 256U) {
        persistence = 256U;
    }
    amplitude = 256;
    amplitude_total = 0;
    total = 0;
    for (octave = 0; octave < octaves; ++octave) {
        total += (long)freak_noise_value_2d(x, y, scale) * amplitude;
        amplitude_total += amplitude;
        amplitude = (amplitude * (int)persistence) >> 8;
        if (amplitude == 0) {
            break;
        }
        if (scale > 1) {
            scale >>= 1;
        }
    }
    if (amplitude_total == 0) {
        return 0;
    }
    return (int)(total / amplitude_total);
}
