/******************************************************************************
 * Freak texture pages
 *
 * Freak's fast texture path deliberately has one format: a 256x256 8-bit
 * paletted page.  One page is 64 KiB and uses the application's VGA palette.
 ******************************************************************************/

#ifndef __freak_texture_h_
#define __freak_texture_h_

#include <stdint.h>

#define FREAK_TEXTURE_WIDTH  256
#define FREAK_TEXTURE_HEIGHT 256
#define FREAK_TEXTURE_BYTES  65536U

struct freak_texture {
    uint8_t *pixels;
};

/* Allocates one standard texture page.  Returns non-zero on success. */
int freak_texture_create(struct freak_texture *texture);
void freak_texture_destroy(struct freak_texture *texture);

void freak_texture_clear(struct freak_texture *texture, uint8_t colour);
void freak_texture_copy(struct freak_texture *destination,
                        const struct freak_texture *source);

/* Fills a texture with deterministic value noise. scale is the size in
   pixels of one noise cell; palette_range is the number of palette entries
   beginning at palette_base. */
void freak_texture_noise(struct freak_texture *texture, unsigned int seed,
                         int scale, uint8_t palette_base,
                         uint8_t palette_range);
void freak_texture_fractal_noise(struct freak_texture *texture,
                                 unsigned int seed, int scale, int octaves,
                                 unsigned int persistence,
                                 uint8_t palette_base,
                                 uint8_t palette_range);

/* Deterministic material writers. palette_range is the number of contiguous
   palette entries beginning at palette_base. */
void freak_texture_asphalt(struct freak_texture *texture, unsigned int seed,
                           uint8_t palette_base, uint8_t palette_range);
void freak_texture_dirt(struct freak_texture *texture, unsigned int seed,
                        uint8_t palette_base, uint8_t palette_range);
void freak_texture_grass(struct freak_texture *texture, unsigned int seed,
                         uint8_t palette_base, uint8_t palette_range);
void freak_texture_clouds(struct freak_texture *texture, unsigned int seed,
                          uint8_t palette_base, uint8_t palette_range);
void freak_texture_marble(struct freak_texture *texture, unsigned int seed,
                          uint8_t palette_base, uint8_t palette_range);

/* Animated effect writers. Plasma advances four independent byte phase clocks.
   Smudge may use one texture for source/destination or ping-pong pages. */
void freak_texture_plasma(struct freak_texture *texture, uint8_t *c1,
                          uint8_t *c2, uint8_t *c3, uint8_t *c4,
                          uint8_t palette_base, uint8_t palette_range);
void freak_texture_smudge_up(struct freak_texture *destination,
                             const struct freak_texture *source,
                             unsigned int seed, uint8_t palette_base,
                             uint8_t palette_range);

/* Hot-path wrapped pixel access. texture must be valid; the result is an
   lvalue, so it supports both reads and writes. Each argument is evaluated
   exactly once. */
#define freak_texture_pixel(texture, u, v) \
    ((texture)->pixels[(((unsigned int)(u) & 255U) + \
                        (((unsigned int)(v) & 255U) << 8))])

#endif /* __freak_texture_h_ */
