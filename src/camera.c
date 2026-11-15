#include "camera.h"

void freak_camera_init(struct freak_camera *camera)
{
    camera->position.x = FIXED_ZERO;
    camera->position.y = FIXED_ZERO;
    camera->position.z = FIXED_ZERO;
    camera->yaw = FIXED_ZERO;
    camera->pitch = FIXED_ZERO;
    camera->roll = FIXED_ZERO;
}

void freak_camera_set_pose(struct freak_camera *camera, const vec3 *position,
                           fixed yaw, fixed pitch, fixed roll)
{
    camera->position = *position;
    camera->yaw = yaw;
    camera->pitch = pitch;
    camera->roll = roll;
}

void freak_camera_world_to_view(const struct freak_camera *camera,
                                const vec3 *world, vec3 *view)
{
    fixed x;
    fixed y;
    fixed z;
    fixed sine;
    fixed cosine;
    fixed term_a;
    fixed term_b;

    x = world->x - camera->position.x;
    y = world->y - camera->position.y;
    z = world->z - camera->position.z;

    /* Inverse yaw: align the camera's forward vector with +Z. */
    sine = fixed_sin_interp(camera->yaw);
    cosine = fixed_cos_interp(camera->yaw);
    term_a = fixed_mul(cosine, x);
    term_b = fixed_mul(sine, z);
    view->x = term_a - term_b;
    term_a = fixed_mul(sine, x);
    term_b = fixed_mul(cosine, z);
    view->z = term_a + term_b;
    view->y = y;

    /* Inverse pitch around local X. */
    sine = fixed_sin_interp(camera->pitch);
    cosine = fixed_cos_interp(camera->pitch);
    term_a = fixed_mul(cosine, view->y);
    term_b = fixed_mul(sine, view->z);
    y = term_a - term_b;
    term_a = fixed_mul(sine, view->y);
    term_b = fixed_mul(cosine, view->z);
    z = term_a + term_b;
    view->y = y;
    view->z = z;

    /* Inverse roll around local Z. */
    sine = fixed_sin_interp(camera->roll);
    cosine = fixed_cos_interp(camera->roll);
    term_a = fixed_mul(cosine, view->x);
    term_b = fixed_mul(sine, view->y);
    x = term_a + term_b;
    term_a = fixed_mul(sine, view->x);
    term_b = fixed_mul(cosine, view->y);
    y = -term_a + term_b;
    view->x = x;
    view->y = y;
}
