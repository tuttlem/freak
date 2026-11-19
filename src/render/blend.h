#ifndef __freak_blend_h_
#define __freak_blend_h_

#include <stdint.h>

#define FREAK_BLEND_TABLE_SIZE 65536UL

enum freak_blend_mode {
    FREAK_BLEND_ADDITIVE,
    FREAK_BLEND_SUBTRACTIVE,
    FREAK_BLEND_MULTIPLY,
    FREAK_BLEND_TRANSLUCENT
};

/* The caller owns a 64 KiB table, allowing game-specific palette tables. */
void freak_blend_table_build(uint8_t *table, enum freak_blend_mode mode);
uint8_t freak_blend_pixel(const uint8_t *table, uint8_t destination,
                          uint8_t source);
void freak_blend_rect(uint8_t *pixels, int left, int top, int right, int bottom,
                      uint8_t source, const uint8_t *table);

#endif
