#include <malloc.h>

#include "raster3d.h"

static struct freak_raster3d_vertex lerp_vertex(const struct freak_raster3d_vertex *a,
                                                 const struct freak_raster3d_vertex *b,
                                                 fixed t)
{
    struct freak_raster3d_vertex out;
    out.position.x = a->position.x + fixed_mul(b->position.x - a->position.x, t);
    out.position.y = a->position.y + fixed_mul(b->position.y - a->position.y, t);
    out.position.z = a->position.z + fixed_mul(b->position.z - a->position.z, t);
    out.normal.x = a->normal.x + fixed_mul(b->normal.x - a->normal.x, t);
    out.normal.y = a->normal.y + fixed_mul(b->normal.y - a->normal.y, t);
    out.normal.z = a->normal.z + fixed_mul(b->normal.z - a->normal.z, t);
    out.u = a->u + fixed_to_int(fixed_mul(int_to_fixed(b->u - a->u), t));
    out.v = a->v + fixed_to_int(fixed_mul(int_to_fixed(b->v - a->v), t));
    out.colour = (uint8_t)(a->colour + fixed_to_int(fixed_mul(int_to_fixed((int)b->colour - a->colour), t)));
    return out;
}

int freak_raster3d_init(struct freak_raster3d *raster, int width, int height)
{
    if (!raster || width < 1 || height < 1) return 0;
    raster->depth = (uint16_t *)malloc((unsigned long)width * height * sizeof(uint16_t));
    if (!raster->depth) return 0;
    raster->width = width; raster->height = height;
    freak_raster3d_clear(raster);
    return 1;
}

void freak_raster3d_destroy(struct freak_raster3d *raster)
{
    if (!raster) return;
    if (raster->depth) free(raster->depth);
    raster->depth = 0; raster->width = raster->height = 0;
}

void freak_raster3d_clear(struct freak_raster3d *raster)
{
    unsigned long index;
    if (!raster || !raster->depth) return;
    for (index = 0; index < (unsigned long)raster->width * raster->height; ++index)
        raster->depth[index] = 0;
}

int freak_raster3d_clip_near(const struct freak_raster3d_vertex *input,
                             unsigned int count,
                             struct freak_raster3d_vertex *output,
                             fixed near_plane)
{
    unsigned int index, out_count;
    const struct freak_raster3d_vertex *a;
    const struct freak_raster3d_vertex *b;
    uint8_t a_inside, b_inside;
    fixed t;
    if (!input || !output || count == 0) return 0;
    out_count = 0;
    for (index = 0; index < count; ++index) {
        a = &input[index]; b = &input[(index + 1) % count];
        a_inside = a->position.z >= near_plane;
        b_inside = b->position.z >= near_plane;
        if (a_inside) output[out_count++] = *a;
        if (a_inside != b_inside) {
            t = fixed_div(near_plane - a->position.z, b->position.z - a->position.z);
            output[out_count++] = lerp_vertex(a, b, t);
        }
    }
    return (int)out_count;
}

int freak_raster3d_backface(const struct freak_raster3d_vertex *a,
                            const struct freak_raster3d_vertex *b,
                            const struct freak_raster3d_vertex *c)
{
    vec3 first;
    vec3 second;
    vec3 normal;
    if (!a || !b || !c) return 1;
    first.x = b->position.x - a->position.x;
    first.y = b->position.y - a->position.y;
    first.z = b->position.z - a->position.z;
    second.x = c->position.x - a->position.x;
    second.y = c->position.y - a->position.y;
    second.z = c->position.z - a->position.z;
    vec3_cross(&normal, &first, &second);
    /* The camera looks along +Z.  Front faces therefore have a -Z normal
       in view space.  The framebuffer's inverted Y then gives the rasterizer
       its expected positive screen-space area. */
    return normal.z >= 0;
}

void freak_raster3d_project(const struct freak_raster3d_vertex *view,
                            int center_x, int center_y, fixed focal_length,
                            int *x, int *y, uint16_t *depth)
{
    fixed inverse;
    if (!view || view->position.z <= 0) return;
    inverse = fixed_div(FIXED_ONE, view->position.z);
    if (x) *x = center_x + fixed_to_int(fixed_mul(fixed_mul(view->position.x, focal_length), inverse));
    if (y) *y = center_y - fixed_to_int(fixed_mul(fixed_mul(view->position.y, focal_length), inverse));
    /* 16.16 reciprocal depth maps directly to a useful 16-bit z buffer:
       z=1 becomes full precision; farther geometry naturally compares lower. */
    if (depth) *depth = (uint16_t)(inverse > 65535L ? 65535 : inverse);
}

static void draw_triangle(struct freak_raster3d *raster, uint8_t *pixels,
                          int x0, int y0, uint16_t z0, uint8_t c0,
                          int x1, int y1, uint16_t z1, uint8_t c1,
                          int x2, int y2, uint16_t z2, uint8_t c2,
                          uint8_t gouraud)
{
    int min_x, max_x, min_y, max_y, x, y;
    long area, w0, w1, w2, depth, colour;
    if (!raster || !pixels || !raster->depth) return;
    min_x = x0; if (x1 < min_x) min_x = x1; if (x2 < min_x) min_x = x2;
    max_x = x0; if (x1 > max_x) max_x = x1; if (x2 > max_x) max_x = x2;
    min_y = y0; if (y1 < min_y) min_y = y1; if (y2 < min_y) min_y = y2;
    max_y = y0; if (y1 > max_y) max_y = y1; if (y2 > max_y) max_y = y2;
    if (min_x < 0) min_x = 0; if (min_y < 0) min_y = 0;
    if (max_x >= raster->width) max_x = raster->width - 1;
    if (max_y >= raster->height) max_y = raster->height - 1;
    area = (long)(x1 - x0) * (y2 - y0) - (long)(y1 - y0) * (x2 - x0);
    if (area <= 0) return;
    for (y = min_y; y <= max_y; ++y) for (x = min_x; x <= max_x; ++x) {
        w0 = (long)(x1 - x) * (y2 - y) - (long)(y1 - y) * (x2 - x);
        w1 = (long)(x2 - x) * (y0 - y) - (long)(y2 - y) * (x0 - x);
        w2 = area - w0 - w1;
        if (w0 < 0 || w1 < 0 || w2 < 0) continue;
        depth = (w0 * z0 + w1 * z1 + w2 * z2) / area;
        if (depth <= raster->depth[y * raster->width + x]) continue;
        raster->depth[y * raster->width + x] = (uint16_t)depth;
        colour = gouraud ? (w0 * c0 + w1 * c1 + w2 * c2) / area : c0;
        pixels[y * raster->width + x] = (uint8_t)colour;
    }
}

void freak_raster3d_triangle_flat(struct freak_raster3d *raster, uint8_t *pixels,
                                  const struct freak_raster3d_vertex *a,
                                  const struct freak_raster3d_vertex *b,
                                  const struct freak_raster3d_vertex *c,
                                  int center_x, int center_y, fixed focal_length,
                                  uint8_t colour)
{
    int x0,y0,x1,y1,x2,y2; uint16_t z0,z1,z2;
    if (!a || !b || !c || freak_raster3d_backface(a,b,c)) return;
    freak_raster3d_project(a,center_x,center_y,focal_length,&x0,&y0,&z0);
    freak_raster3d_project(b,center_x,center_y,focal_length,&x1,&y1,&z1);
    freak_raster3d_project(c,center_x,center_y,focal_length,&x2,&y2,&z2);
    draw_triangle(raster,pixels,x0,y0,z0,colour,x1,y1,z1,colour,x2,y2,z2,colour,0);
}

void freak_raster3d_triangle_gouraud(struct freak_raster3d *raster, uint8_t *pixels,
                                     const struct freak_raster3d_vertex *a,
                                     const struct freak_raster3d_vertex *b,
                                     const struct freak_raster3d_vertex *c,
                                     int center_x, int center_y, fixed focal_length)
{
    int x0,y0,x1,y1,x2,y2; uint16_t z0,z1,z2;
    if (!a || !b || !c || freak_raster3d_backface(a,b,c)) return;
    freak_raster3d_project(a,center_x,center_y,focal_length,&x0,&y0,&z0);
    freak_raster3d_project(b,center_x,center_y,focal_length,&x1,&y1,&z1);
    freak_raster3d_project(c,center_x,center_y,focal_length,&x2,&y2,&z2);
    draw_triangle(raster,pixels,x0,y0,z0,a->colour,x1,y1,z1,b->colour,x2,y2,z2,c->colour,1);
}
