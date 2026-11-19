#ifndef __freak_voxel_h_
#define __freak_voxel_h_

#include <stdint.h>

struct freak_voxel_view {
    int x;
    int y;
    int height;
    int horizon;
    int distance;
};

void freak_voxel_heightfield(uint8_t *output, const uint8_t *heights,
                             const struct freak_voxel_view *view,
                             uint8_t colour_base, uint8_t colour_range);

#endif
