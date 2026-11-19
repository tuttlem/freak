#ifndef __freak_raster3d_h_
#define __freak_raster3d_h_

#include <stdint.h>

#include "../core/fixed.h"
#include "../core/vec.h"

#define FREAK_RASTER3D_WIDTH 320
#define FREAK_RASTER3D_HEIGHT 200
#define FREAK_RASTER3D_NEAR int_to_fixed(1)

struct freak_raster3d {
    uint16_t *depth;
    int width;
    int height;
};

struct freak_raster3d_vertex {
    vec3 position;
    vec3 normal;
    int u;
    int v;
    uint8_t colour;
};

int freak_raster3d_init(struct freak_raster3d *raster, int width, int height);
void freak_raster3d_destroy(struct freak_raster3d *raster);
void freak_raster3d_clear(struct freak_raster3d *raster);
int freak_raster3d_clip_near(const struct freak_raster3d_vertex *input,
                             unsigned int count,
                             struct freak_raster3d_vertex *output,
                             fixed near_plane);
int freak_raster3d_backface(const struct freak_raster3d_vertex *a,
                            const struct freak_raster3d_vertex *b,
                            const struct freak_raster3d_vertex *c);
void freak_raster3d_project(const struct freak_raster3d_vertex *view,
                            int center_x, int center_y, fixed focal_length,
                            int *x, int *y, uint16_t *depth);
void freak_raster3d_triangle_flat(struct freak_raster3d *raster,
                                  uint8_t *pixels,
                                  const struct freak_raster3d_vertex *a,
                                  const struct freak_raster3d_vertex *b,
                                  const struct freak_raster3d_vertex *c,
                                  int center_x, int center_y,
                                  fixed focal_length, uint8_t colour);
void freak_raster3d_triangle_gouraud(struct freak_raster3d *raster,
                                     uint8_t *pixels,
                                     const struct freak_raster3d_vertex *a,
                                     const struct freak_raster3d_vertex *b,
                                     const struct freak_raster3d_vertex *c,
                                     int center_x, int center_y,
                                     fixed focal_length);

#endif
