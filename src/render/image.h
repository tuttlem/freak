#ifndef __freak_image_h_
#define __freak_image_h_

#include <stdint.h>

struct freak_image {
    uint8_t *pixels; /* RGB or RGBA, top-left origin */
    uint16_t width;
    uint16_t height;
    uint8_t bytes_per_pixel;
};

int freak_tga_load(const char *filename, struct freak_image *image);
void freak_image_destroy(struct freak_image *image);

#endif
