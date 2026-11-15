#ifndef __freak_palette_h_
#define __freak_palette_h_

#include <stdint.h>

struct freak_colour {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

/* Reusable key ramps. They can be stretched across any owned palette range. */
extern const struct freak_colour FREAK_PALETTE_GREYSCALE[];
extern const struct freak_colour FREAK_PALETTE_SKY[];
extern const struct freak_colour FREAK_PALETTE_FIRE[];
extern const struct freak_colour FREAK_PALETTE_SMOKE[];
extern const struct freak_colour FREAK_PALETTE_WATER[];
extern const struct freak_colour FREAK_PALETTE_FOREST[];
extern const struct freak_colour FREAK_PALETTE_SEPIA[];
extern const struct freak_colour FREAK_PALETTE_ICE[];

#define FREAK_PALETTE_GREYSCALE_COUNT 2
#define FREAK_PALETTE_SKY_COUNT 3
#define FREAK_PALETTE_FIRE_COUNT 5
#define FREAK_PALETTE_SMOKE_COUNT 4
#define FREAK_PALETTE_WATER_COUNT 4
#define FREAK_PALETTE_FOREST_COUNT 4
#define FREAK_PALETTE_SEPIA_COUNT 4
#define FREAK_PALETTE_ICE_COUNT 4

/* All writers are restricted to first..first + count - 1. RGB values use
   the VGA DAC's native 0..63 range. */
void freak_palette_preset(uint8_t first, uint8_t count,
                          const struct freak_colour *keys, int key_count);
#define freak_palette_gradient freak_palette_preset
void freak_palette_greyscale(uint8_t first, uint8_t count, uint8_t dark,
                             uint8_t light);
void freak_palette_sky(uint8_t first, uint8_t count);
void freak_palette_fire(uint8_t first, uint8_t count);
void freak_palette_smoke(uint8_t first, uint8_t count);

/* Rotates the existing DAC entries in first..first + count - 1. A positive
   direction moves colour n to n + 1; a negative direction moves it to n - 1. */
void freak_palette_rotate(uint8_t first, uint8_t count, int direction);

#endif
