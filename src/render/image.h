#ifndef __freak_image_h_
#define __freak_image_h_

#include <stdint.h>

struct freak_image {
    uint8_t *pixels; /* RGB or RGBA, top-left origin */
    uint16_t width;
    uint16_t height;
    uint8_t bytes_per_pixel;
};

/* Mode 13h-ready 8-bit image. The 3:3:2 palette helper installs its DAC. */
struct freak_indexed_image {
    uint8_t *pixels;
    uint16_t width;
    uint16_t height;
};

int freak_tga_load(const char *filename, struct freak_image *image);
int freak_tga_load_indexed_332(const char *filename,
                               struct freak_indexed_image *image);
void freak_image_destroy(struct freak_image *image);
void freak_indexed_image_destroy(struct freak_indexed_image *image);
void freak_image_palette_332(void);

#endif
