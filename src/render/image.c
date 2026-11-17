#include <stdio.h>
#include <malloc.h>
#include "image.h"
#include "../dos/vga.h"

void freak_image_destroy(struct freak_image *image) {
    if (image != 0 && image->pixels != 0) {
        free(image->pixels);
        image->pixels = 0;
    }
}

int freak_tga_load(const char *filename, struct freak_image *image) {
    FILE *file;
    uint8_t header[18], pixel[4];
    unsigned long total, index;
    int type, bpp, packet, run, i, source_y, target_y;

    if (image == 0 || filename == 0)
        return 0;
    image->pixels = 0;
    file = fopen(filename, "rb");
    if (file == 0)
        return 0;
    if (fread(header, 1, 18, file) != 18) {
        fclose(file);
        return 0;
    }
    type = header[2];
    bpp = header[16] / 8;
    image->width = (uint16_t)(header[12] | (header[13] << 8));
    image->height = (uint16_t)(header[14] | (header[15] << 8));
    if (header[1] != 0 || (type != 2 && type != 10) || (bpp != 3 && bpp != 4) ||
        !image->width || !image->height) {
        fclose(file);
        return 0;
    }
    if (header[0] && fseek(file, header[0], SEEK_CUR)) {
        fclose(file);
        return 0;
    }
    total = (unsigned long)image->width * image->height;
    image->bytes_per_pixel = (uint8_t)bpp;
    image->pixels = (uint8_t *)malloc(total * bpp);
    if (image->pixels == 0) {
        fclose(file);
        return 0;
    }
    index = 0;
    while (index < total) {
        packet = 1;
        if (type == 10) {
            packet = fgetc(file);
            if (packet == EOF)
                break;
            run = (packet & 127) + 1;
        } else {
            run = 1;
            packet = 0;
        }
        while (run-- && index < total) {
            if (!(packet & 128) || run == ((packet & 127)))
                if (fread(pixel, 1, bpp, file) != (unsigned)bpp) {
                    index = total + 1;
                    break;
                }
            source_y = (int)(index / image->width);
            target_y =
                (header[17] & 32) ? source_y : image->height - source_y - 1;
            i = (target_y * image->width + index % image->width) * bpp;
            image->pixels[i] = pixel[2];
            image->pixels[i + 1] = pixel[1];
            image->pixels[i + 2] = pixel[0];
            if (bpp == 4)
                image->pixels[i + 3] = pixel[3];
            ++index;
        }
    }
    fclose(file);
    if (index != total) {
        freak_image_destroy(image);
        return 0;
    }
    return 1;
}

int freak_tga_load_indexed_332(const char *filename,
                               struct freak_indexed_image *image)
{
    struct freak_image source;
    unsigned long count;
    unsigned long index;
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    if (image == 0)
        return 0;
    image->pixels = 0;
    if (!freak_tga_load(filename, &source))
        return 0;
    count = (unsigned long)source.width * source.height;
    image->pixels = (uint8_t *)malloc(count);
    if (image->pixels == 0) {
        freak_image_destroy(&source);
        return 0;
    }
    for (index = 0; index < count; ++index) {
        red = source.pixels[index * source.bytes_per_pixel];
        green = source.pixels[index * source.bytes_per_pixel + 1];
        blue = source.pixels[index * source.bytes_per_pixel + 2];
        image->pixels[index] = (uint8_t)(((red >> 5) << 5) |
                                         ((green >> 5) << 2) |
                                         (blue >> 6));
    }
    image->width = source.width;
    image->height = source.height;
    freak_image_destroy(&source);
    return 1;
}

void freak_indexed_image_destroy(struct freak_indexed_image *image)
{
    if (image && image->pixels) {
        free(image->pixels);
        image->pixels = 0;
    }
}

void freak_image_palette_332(void)
{
    int index;
    int red;
    int green;
    int blue;

    for (index = 0; index < 256; ++index) {
        red = (index >> 5) & 7;
        green = (index >> 2) & 7;
        blue = index & 3;
        freak_set_palette((uint8_t)index, (uint8_t)(red * 9),
                          (uint8_t)(green * 9), (uint8_t)(blue * 21));
    }
}
