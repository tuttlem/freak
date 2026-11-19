#include "shade3d.h"

static void unit3(vec3 *v)
{
    fixed length;
    length = vec3_len(v);
    if (length > 0) {
        v->x = fixed_div(v->x, length);
        v->y = fixed_div(v->y, length);
        v->z = fixed_div(v->z, length);
    }
}

uint8_t freak_shade3d_lambert(const vec3 *position, const vec3 *normal,
                              const struct freak_light3d *light,
                              uint8_t ambient, uint8_t specular)
{
    vec3 direction;
    fixed diffuse;
    fixed distance;
    fixed attenuation;
    fixed amount;
    long result;
    if (!position || !normal || !light) return ambient;
    if (light->type == FREAK_LIGHT3D_DIRECTIONAL) {
        direction = light->direction;
        direction.x = -direction.x; direction.y = -direction.y; direction.z = -direction.z;
        attenuation = FIXED_ONE;
    } else {
        direction.x = light->position.x - position->x;
        direction.y = light->position.y - position->y;
        direction.z = light->position.z - position->z;
        distance = vec3_len(&direction);
        if (distance <= 0) distance = FIXED_ONE;
        if (light->range > 0 && distance >= light->range) return ambient;
        attenuation = light->range > 0 ? FIXED_ONE - fixed_div(distance, light->range) : FIXED_ONE;
        unit3(&direction);
    }
    diffuse = vec3_dot(normal, &direction);
    if (diffuse < 0) diffuse = 0;
    amount = fixed_mul(fixed_mul(diffuse, attenuation), light->intensity);
    result = ambient + fixed_to_int(fixed_mul(amount, int_to_fixed(255 - ambient)));
    /* Paletted specular is deliberately a small brightening term: callers can
       reserve the top of a material's palette ramp for its highlight. */
    if (specular && diffuse > int_to_fixed(3) / 4) result += specular;
    if (result > 255) result = 255;
    return (uint8_t)result;
}

uint8_t freak_shade3d_bump(const uint8_t *normal_map, int u, int v,
                           const vec3 *position,
                           const struct freak_light3d *light,
                           uint8_t ambient)
{
    vec3 normal;
    unsigned int index;
    if (!normal_map) return ambient;
    index = (((unsigned int)v & 255U) << 8) + ((unsigned int)u & 255U);
    normal.x = int_to_fixed((int)normal_map[index * 3U] - 128) / 127;
    normal.y = int_to_fixed((int)normal_map[index * 3U + 1] - 128) / 127;
    normal.z = int_to_fixed((int)normal_map[index * 3U + 2] - 128) / 127;
    unit3(&normal);
    return freak_shade3d_lambert(position, &normal, light, ambient, 0);
}

void freak_shade3d_sphere_uv(const vec3 *normal, const vec3 *view,
                             uint8_t *u, uint8_t *v)
{
    vec3 reflection;
    fixed twice_dot;
    if (!normal || !view || !u || !v) return;
    twice_dot = fixed_mul(vec3_dot(normal, view), int_to_fixed(2));
    reflection.x = fixed_mul(normal->x, twice_dot) - view->x;
    reflection.y = fixed_mul(normal->y, twice_dot) - view->y;
    reflection.z = fixed_mul(normal->z, twice_dot) - view->z;
    *u = (uint8_t)(128 + fixed_to_int(fixed_mul(reflection.x, int_to_fixed(127))));
    *v = (uint8_t)(128 - fixed_to_int(fixed_mul(reflection.y, int_to_fixed(127))));
}

uint8_t freak_shade3d_cube_uv(const vec3 *direction, uint8_t *u, uint8_t *v)
{
    fixed ax;
    fixed ay;
    fixed az;
    fixed major;
    fixed su;
    fixed sv;
    uint8_t face;
    if (!direction || !u || !v) return 0;
    ax = direction->x < 0 ? -direction->x : direction->x;
    ay = direction->y < 0 ? -direction->y : direction->y;
    az = direction->z < 0 ? -direction->z : direction->z;
    if (ax >= ay && ax >= az) {
        major = direction->x; face = major < 0 ? 1 : 0;
        su = fixed_div(direction->z, ax); sv = fixed_div(direction->y, ax);
        if (major < 0) su = -su;
    } else if (ay >= az) {
        major = direction->y; face = major < 0 ? 3 : 2;
        su = fixed_div(direction->x, ay); sv = fixed_div(direction->z, ay);
        if (major < 0) sv = -sv;
    } else {
        major = direction->z; face = major < 0 ? 5 : 4;
        su = fixed_div(direction->x, az); sv = fixed_div(direction->y, az);
        if (major < 0) su = -su;
    }
    *u = (uint8_t)(128 + fixed_to_int(fixed_mul(su, int_to_fixed(127))));
    *v = (uint8_t)(128 - fixed_to_int(fixed_mul(sv, int_to_fixed(127))));
    return face;
}
