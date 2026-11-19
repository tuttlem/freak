#include "model3d.h"

int freak_model3d_read(FILE *stream, struct freak_model3d *model,
                       struct freak_model3d_vertex *vertices,
                       unsigned int vertex_capacity, uint16_t *indices,
                       unsigned int index_capacity)
{
    unsigned long magic;
    unsigned short vertex_count;
    unsigned short index_count;
    uint8_t material;
    unsigned int index;
    if (!stream || !model || !vertices || !indices) return 0;
    if (fread(&magic, sizeof(magic), 1, stream) != 1 || magic != FREAK_MODEL3D_MAGIC ||
        fread(&vertex_count, sizeof(vertex_count), 1, stream) != 1 ||
        fread(&index_count, sizeof(index_count), 1, stream) != 1 ||
        fread(&material, sizeof(material), 1, stream) != 1 ||
        vertex_count > vertex_capacity || index_count > index_capacity) return 0;
    for (index = 0; index < vertex_count; ++index) {
        if (fread(&vertices[index].position.x, sizeof(fixed), 1, stream) != 1 ||
            fread(&vertices[index].position.y, sizeof(fixed), 1, stream) != 1 ||
            fread(&vertices[index].position.z, sizeof(fixed), 1, stream) != 1 ||
            fread(&vertices[index].normal.x, sizeof(fixed), 1, stream) != 1 ||
            fread(&vertices[index].normal.y, sizeof(fixed), 1, stream) != 1 ||
            fread(&vertices[index].normal.z, sizeof(fixed), 1, stream) != 1 ||
            fread(&vertices[index].u, sizeof(uint8_t), 1, stream) != 1 ||
            fread(&vertices[index].v, sizeof(uint8_t), 1, stream) != 1) return 0;
    }
    if (fread(indices, sizeof(uint16_t), index_count, stream) != index_count) return 0;
    model->vertices = vertices; model->vertex_count = vertex_count;
    model->indices = indices; model->index_count = index_count; model->material = material;
    return 1;
}

int freak_model3d_interpolate(const struct freak_model3d *from,
                              const struct freak_model3d *to, fixed t,
                              struct freak_model3d_vertex *output,
                              unsigned int capacity)
{
    unsigned int index;
    if (!from || !to || !output || from->vertex_count != to->vertex_count ||
        capacity < from->vertex_count || t < 0 || t > FIXED_ONE) return 0;
    for (index = 0; index < from->vertex_count; ++index) {
        output[index].position.x = from->vertices[index].position.x + fixed_mul(to->vertices[index].position.x - from->vertices[index].position.x, t);
        output[index].position.y = from->vertices[index].position.y + fixed_mul(to->vertices[index].position.y - from->vertices[index].position.y, t);
        output[index].position.z = from->vertices[index].position.z + fixed_mul(to->vertices[index].position.z - from->vertices[index].position.z, t);
        output[index].normal.x = from->vertices[index].normal.x + fixed_mul(to->vertices[index].normal.x - from->vertices[index].normal.x, t);
        output[index].normal.y = from->vertices[index].normal.y + fixed_mul(to->vertices[index].normal.y - from->vertices[index].normal.y, t);
        output[index].normal.z = from->vertices[index].normal.z + fixed_mul(to->vertices[index].normal.z - from->vertices[index].normal.z, t);
        output[index].u = from->vertices[index].u;
        output[index].v = from->vertices[index].v;
    }
    return 1;
}
