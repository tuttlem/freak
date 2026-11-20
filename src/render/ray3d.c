#include "ray3d.h"

int freak_ray3d_cast(const struct freak_ray3d_map *map, fixed x, fixed y,
                     fixed direction_x, fixed direction_y,
                     fixed maximum_distance, struct freak_ray3d_hit *hit)
{
    fixed distance_x;
    fixed distance_y;
    fixed side_x;
    fixed side_y;
    fixed boundary;
    fixed abs_x;
    fixed abs_y;
    int step_x;
    int step_y;
    int cell_x;
    int cell_y;
    if (!map || !map->tiles || !hit) return 0;
    if (!direction_x && !direction_y) return 0;
    cell_x = fixed_to_int(x); cell_y = fixed_to_int(y);
    abs_x = direction_x < 0 ? -direction_x : direction_x;
    abs_y = direction_y < 0 ? -direction_y : direction_y;
    /* Never divide by near-axis directions: their reciprocal can overflow the
       16.16 range and raise a DOS divide exception. */
    distance_x = abs_x < FIXED_ONE / 256 ? int_to_fixed(16384) : fixed_div(FIXED_ONE, abs_x);
    distance_y = abs_y < FIXED_ONE / 256 ? int_to_fixed(16384) : fixed_div(FIXED_ONE, abs_y);
    if (abs_x < FIXED_ONE / 256) {
        step_x = 0; side_x = int_to_fixed(16384);
    } else if (direction_x < 0) {
        step_x = -1; boundary = x - int_to_fixed(cell_x); side_x = fixed_mul(boundary, distance_x);
    } else {
        step_x = 1; boundary = int_to_fixed(cell_x + 1) - x; side_x = fixed_mul(boundary, distance_x);
    }
    if (abs_y < FIXED_ONE / 256) {
        step_y = 0; side_y = int_to_fixed(16384);
    } else if (direction_y < 0) {
        step_y = -1; boundary = y - int_to_fixed(cell_y); side_y = fixed_mul(boundary, distance_y);
    } else {
        step_y = 1; boundary = int_to_fixed(cell_y + 1) - y; side_y = fixed_mul(boundary, distance_y);
    }
    while (side_x < maximum_distance || side_y < maximum_distance) {
        if (side_x < side_y) { cell_x += step_x; hit->distance = side_x; side_x += distance_x; }
        else { cell_y += step_y; hit->distance = side_y; side_y += distance_y; }
        if (cell_x < 0 || cell_y < 0 || cell_x >= map->width || cell_y >= map->height) return 0;
        if (map->tiles[cell_y * map->width + cell_x]) {
            /* A ray starting exactly on a cell boundary may enter its first
               wall at distance zero.  Keep the result projectable. */
            if (hit->distance < FIXED_ONE / 64) hit->distance = FIXED_ONE / 64;
            hit->cell_x = cell_x; hit->cell_y = cell_y;
            hit->tile = map->tiles[cell_y * map->width + cell_x]; return 1;
        }
    }
    return 0;
}

static int sphere_hit(const struct freak_raytrace_object *object, const vec3 *origin,
                      const vec3 *direction, fixed *distance, vec3 *normal)
{
    vec3 offset;
    fixed projection;
    fixed closest;
    fixed discriminant;
    fixed root;
    fixed d;
    offset.x = origin->x - object->position.x;
    offset.y = origin->y - object->position.y;
    offset.z = origin->z - object->position.z;
    projection = vec3_dot(&offset, direction);
    closest = vec3_dot(&offset, &offset) - fixed_mul(projection, projection);
    discriminant = fixed_mul(object->radius, object->radius) - closest;
    if (discriminant < 0) return 0;
    root = fixed_sqrt(discriminant);
    d = -projection - root;
    if (d <= FIXED_ONE / 100) d = -projection + root;
    if (d <= FIXED_ONE / 100) return 0;
    *distance = d;
    normal->x = fixed_mul(direction->x, d) + offset.x;
    normal->y = fixed_mul(direction->y, d) + offset.y;
    normal->z = fixed_mul(direction->z, d) + offset.z;
    normal->x = fixed_div(normal->x, object->radius);
    normal->y = fixed_div(normal->y, object->radius);
    normal->z = fixed_div(normal->z, object->radius);
    return 1;
}

static int plane_hit(const struct freak_raytrace_object *object, const vec3 *origin,
                     const vec3 *direction, fixed *distance, vec3 *normal)
{
    fixed d;
    if (direction->y >= 0) return 0;
    d = fixed_div(object->position.y - origin->y, direction->y);
    if (d <= FIXED_ONE / 100) return 0;
    *distance = d; normal->x = 0; normal->y = FIXED_ONE; normal->z = 0;
    return 1;
}

static int object_hit(const struct freak_raytrace_object *object,
                      const vec3 *origin, const vec3 *direction,
                      fixed *distance, vec3 *normal)
{
    if (object->kind == FREAK_RAYTRACE_SPHERE)
        return sphere_hit(object, origin, direction, distance, normal);
    if (object->kind == FREAK_RAYTRACE_PLANE)
        return plane_hit(object, origin, direction, distance, normal);
    return 0;
}

uint8_t freak_raytrace_sample(const struct freak_raytrace_object *objects,
                              unsigned int count, const vec3 *origin,
                              const vec3 *direction, unsigned int depth)
{
    unsigned int index;
    int found;
    fixed nearest;
    fixed distance;
    vec3 normal;
    vec3 normal_at_nearest;
    vec3 point;
    vec3 shadow_origin;
    vec3 shadow_direction;
    vec3 reflection;
    vec3 refraction;
    const struct freak_raytrace_object *object;
    fixed light;
    fixed twice_dot;
    uint8_t colour;
    if (!objects || !origin || !direction) return 1;
    found = 0; nearest = int_to_fixed(1000); object = 0;
    for (index = 0; index < count; ++index) {
        if (object_hit(&objects[index], origin, direction, &distance, &normal) && distance < nearest) {
            nearest = distance; normal_at_nearest = normal; object = &objects[index]; found = 1;
        }
    }
    if (!found) return direction->y > 0 ? 12 : 4;
    point.x = origin->x + fixed_mul(direction->x, nearest);
    point.y = origin->y + fixed_mul(direction->y, nearest);
    point.z = origin->z + fixed_mul(direction->z, nearest);
    light = fixed_mul(normal_at_nearest.x, int_to_fixed(-1) / 2) +
            fixed_mul(normal_at_nearest.y, int_to_fixed(3) / 4) +
            fixed_mul(normal_at_nearest.z, int_to_fixed(-1) / 2);
    if (light < 0) light = 0;
    shadow_direction.x = int_to_fixed(-1) / 2;
    shadow_direction.y = int_to_fixed(3) / 4;
    shadow_direction.z = int_to_fixed(-1) / 2;
    shadow_origin.x = point.x + fixed_mul(normal_at_nearest.x, FIXED_ONE / 64);
    shadow_origin.y = point.y + fixed_mul(normal_at_nearest.y, FIXED_ONE / 64);
    shadow_origin.z = point.z + fixed_mul(normal_at_nearest.z, FIXED_ONE / 64);
    for (index = 0; index < count; ++index)
        if (&objects[index] != object && object_hit(&objects[index], &shadow_origin,
            &shadow_direction, &distance, &normal)) { light /= 3; break; }
    colour = (uint8_t)(object->colour + fixed_to_int(fixed_mul(light, int_to_fixed(28))));
    twice_dot = fixed_mul(vec3_dot(direction, &normal_at_nearest), int_to_fixed(2));
    reflection.x = direction->x - fixed_mul(normal_at_nearest.x, twice_dot);
    reflection.y = direction->y - fixed_mul(normal_at_nearest.y, twice_dot);
    reflection.z = direction->z - fixed_mul(normal_at_nearest.z, twice_dot);
    if (object->reflective && depth) {
        colour = (uint8_t)((colour + freak_raytrace_sample(objects, count,
            &shadow_origin, &reflection, depth - 1)) >> 1);
    }
    if (object->refractive && depth) {
        /* A small palette-friendly refractive bend; secondary sampling makes
           it a genuine transmitted ray rather than merely a colour shift. */
        refraction.x = direction->x - fixed_mul(normal_at_nearest.x, FIXED_ONE / 3);
        refraction.y = direction->y - fixed_mul(normal_at_nearest.y, FIXED_ONE / 3);
        refraction.z = direction->z - fixed_mul(normal_at_nearest.z, FIXED_ONE / 3);
        colour = (uint8_t)((colour + freak_raytrace_sample(objects, count,
            &point, &refraction, depth - 1)) >> 1);
    }
    return colour;
}
