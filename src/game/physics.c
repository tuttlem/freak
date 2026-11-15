#include "physics.h"

void freak_body3_init(struct freak_body3 *body)
{
    vec3_zero(&body->position);
    vec3_zero(&body->velocity);
    vec3_zero(&body->acceleration);
    body->linear_drag = FIXED_ZERO;
}

void freak_body3_step(struct freak_body3 *body, fixed delta_seconds)
{
    fixed drag_factor;

    body->velocity.x += fixed_mul(body->acceleration.x, delta_seconds);
    body->velocity.y += fixed_mul(body->acceleration.y, delta_seconds);
    body->velocity.z += fixed_mul(body->acceleration.z, delta_seconds);

    drag_factor = FIXED_ONE - fixed_mul(body->linear_drag, delta_seconds);
    if (drag_factor < FIXED_ZERO) {
        drag_factor = FIXED_ZERO;
    }
    body->velocity.x = fixed_mul(body->velocity.x, drag_factor);
    body->velocity.y = fixed_mul(body->velocity.y, drag_factor);
    body->velocity.z = fixed_mul(body->velocity.z, drag_factor);

    body->position.x += fixed_mul(body->velocity.x, delta_seconds);
    body->position.y += fixed_mul(body->velocity.y, delta_seconds);
    body->position.z += fixed_mul(body->velocity.z, delta_seconds);
}
