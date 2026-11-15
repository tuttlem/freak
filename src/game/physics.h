/******************************************************************************
 * Freak fixed-point body integration
 ******************************************************************************/

#ifndef __freak_physics_h_
#define __freak_physics_h_

#include "../core/vec.h"

struct freak_body3 {
    vec3 position;
    vec3 velocity;
    vec3 acceleration;
    fixed linear_drag;
};

void freak_body3_init(struct freak_body3 *body);
void freak_body3_step(struct freak_body3 *body, fixed delta_seconds);

#endif /* __freak_physics_h_ */
