#ifndef __freak_render_asset_h_
#define __freak_render_asset_h_

#include <stdint.h>
#include "image.h"
#include "sprite.h"

/* Host tools/freak_asset.py emits these self-contained paletted assets. */
struct freak_image_asset {
    struct freak_indexed_image image;
    uint8_t palette[768]; /* VGA DAC RGB triples, 0..63 */
    uint8_t transparent;
};

struct freak_sprite_sheet_asset {
    uint8_t *pixels;
    uint16_t frame_width;
    uint16_t frame_height;
    uint16_t frame_count;
    uint8_t transparent;
    uint8_t palette[768];
};

void freak_image_asset_init(struct freak_image_asset *asset);
void freak_image_asset_destroy(struct freak_image_asset *asset);
int freak_image_asset_load(struct freak_image_asset *asset, const char *filename);
void freak_image_asset_apply_palette(const struct freak_image_asset *asset);

void freak_sprite_sheet_asset_init(struct freak_sprite_sheet_asset *asset);
void freak_sprite_sheet_asset_destroy(struct freak_sprite_sheet_asset *asset);
int freak_sprite_sheet_asset_load(struct freak_sprite_sheet_asset *asset,
                                  const char *filename);
int freak_sprite_sheet_asset_frame(const struct freak_sprite_sheet_asset *asset,
                                   uint16_t frame, struct freak_sprite *sprite);
void freak_sprite_sheet_asset_apply_palette(const struct freak_sprite_sheet_asset *asset);
int freak_palette_asset_load(const char *filename, uint8_t palette[768]);
void freak_palette_asset_apply(const uint8_t palette[768]);

#endif
