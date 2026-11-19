#ifndef __freak_model3d_h_
#define __freak_model3d_h_

#include <stdint.h>
#include <stdio.h>

#include "../core/vec.h"

#define FREAK_MODEL3D_MAGIC 0x314D3346UL /* F3M1 */

struct freak_model3d_vertex {
    vec3 position;
    vec3 normal;
    uint8_t u;
    uint8_t v;
};

struct freak_model3d {
    const struct freak_model3d_vertex *vertices;
    unsigned int vertex_count;
    const uint16_t *indices;
    unsigned int index_count;
    uint8_t material;
};

/* Reads F3M1 emitted by tools/obj2f3m.py into caller-owned fixed buffers. */
int freak_model3d_read(FILE *stream, struct freak_model3d *model,
                       struct freak_model3d_vertex *vertices,
                       unsigned int vertex_capacity, uint16_t *indices,
                       unsigned int index_capacity);

/* Interpolates compatible keyframe meshes into caller-owned vertex storage. */
int freak_model3d_interpolate(const struct freak_model3d *from,
                              const struct freak_model3d *to, fixed t,
                              struct freak_model3d_vertex *output,
                              unsigned int capacity);

#endif
