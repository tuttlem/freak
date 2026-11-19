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

void freak_camera_fps_move(struct freak_camera *camera, fixed forward,
                           fixed strafe, fixed rise)
{
    fixed sine;
    fixed cosine;
    if (!camera) return;
    sine = fixed_sin_interp(camera->yaw);
    cosine = fixed_cos_interp(camera->yaw);
    camera->position.x += fixed_mul(sine, forward) + fixed_mul(cosine, strafe);
    camera->position.z += fixed_mul(cosine, forward) - fixed_mul(sine, strafe);
    camera->position.y += rise;
}

void freak_camera_orbit(struct freak_camera *camera, const vec3 *target,
                        fixed yaw, fixed pitch, fixed distance)
{
    fixed sine_yaw;
    fixed cosine_yaw;
    fixed sine_pitch;
    fixed cosine_pitch;
    if (!camera || !target) return;
    sine_yaw = fixed_sin_interp(yaw);
    cosine_yaw = fixed_cos_interp(yaw);
    sine_pitch = fixed_sin_interp(pitch);
    cosine_pitch = fixed_cos_interp(pitch);
    camera->position.x = target->x - fixed_mul(fixed_mul(sine_yaw, cosine_pitch), distance);
    camera->position.y = target->y + fixed_mul(sine_pitch, distance);
    camera->position.z = target->z - fixed_mul(fixed_mul(cosine_yaw, cosine_pitch), distance);
    camera->yaw = yaw;
    camera->pitch = -pitch;
}

void freak_camera_chase(struct freak_camera *camera, const vec3 *target,
                        fixed yaw, fixed height, fixed distance)
{
    if (!camera || !target) return;
    freak_camera_orbit(camera, target, yaw, FIXED_ZERO, distance);
    camera->position.y = target->y + height;
}

void freak_camera_rail(struct freak_camera *camera,
                       const struct freak_camera_rail_key *keys,
                       unsigned int count, fixed position)
{
    unsigned int index;
    fixed fraction;
    if (!camera || !keys || count == 0) return;
    if (count == 1 || position <= FIXED_ZERO) {
        camera->position = keys[0].position; camera->yaw = keys[0].yaw; camera->pitch = keys[0].pitch; return;
    }
    index = (unsigned int)fixed_to_int(position);
    if (index >= count - 1) {
        camera->position = keys[count - 1].position; camera->yaw = keys[count - 1].yaw; camera->pitch = keys[count - 1].pitch; return;
    }
    fraction = position - int_to_fixed((int)index);
    camera->position.x = keys[index].position.x + fixed_mul(keys[index + 1].position.x - keys[index].position.x, fraction);
    camera->position.y = keys[index].position.y + fixed_mul(keys[index + 1].position.y - keys[index].position.y, fraction);
    camera->position.z = keys[index].position.z + fixed_mul(keys[index + 1].position.z - keys[index].position.z, fraction);
    camera->yaw = keys[index].yaw + fixed_mul(keys[index + 1].yaw - keys[index].yaw, fraction);
    camera->pitch = keys[index].pitch + fixed_mul(keys[index + 1].pitch - keys[index].pitch, fraction);
}

void freak_camera_shake(struct freak_camera *camera, fixed amount,
                        unsigned long tick)
{
    if (!camera || !amount) return;
    camera->position.x += fixed_mul(fixed_sin_interp((fixed)(tick * 97UL) & 1023), amount);
    camera->position.y += fixed_mul(fixed_sin_interp((fixed)(tick * 151UL) & 1023), amount);
    camera->yaw += fixed_mul(fixed_sin_interp((fixed)(tick * 211UL) & 1023), amount);
}

void freak_camera_smooth_to(struct freak_camera *camera,
                            struct freak_camera_spring *spring,
                            const struct freak_camera *target,
                            fixed response, fixed damping)
{
    if (!camera || !spring || !target) return;
    spring->position_velocity.x += fixed_mul(target->position.x - camera->position.x, response);
    spring->position_velocity.y += fixed_mul(target->position.y - camera->position.y, response);
    spring->position_velocity.z += fixed_mul(target->position.z - camera->position.z, response);
    spring->position_velocity.x = fixed_mul(spring->position_velocity.x, damping);
    spring->position_velocity.y = fixed_mul(spring->position_velocity.y, damping);
    spring->position_velocity.z = fixed_mul(spring->position_velocity.z, damping);
    camera->position.x += spring->position_velocity.x;
    camera->position.y += spring->position_velocity.y;
    camera->position.z += spring->position_velocity.z;
    spring->yaw_velocity = fixed_mul(spring->yaw_velocity + fixed_mul(target->yaw - camera->yaw, response), damping);
    spring->pitch_velocity = fixed_mul(spring->pitch_velocity + fixed_mul(target->pitch - camera->pitch, response), damping);
    camera->yaw += spring->yaw_velocity;
    camera->pitch += spring->pitch_velocity;
}
