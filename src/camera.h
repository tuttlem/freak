/******************************************************************************
 * Freak camera pose and world-to-view transform
 *
 * Angles use Freak's 16.16 trig-table units: 1024 is one complete turn.
 * The view coordinate system is right-handed: +X right, +Y up, +Z forward.
 ******************************************************************************/

#ifndef __freak_camera_h_
#define __freak_camera_h_

#include "vec.h"

struct freak_camera {
    vec3 position;
    fixed yaw;
    fixed pitch;
    fixed roll;
};

void freak_camera_init(struct freak_camera *camera);
void freak_camera_set_pose(struct freak_camera *camera, const vec3 *position,
                           fixed yaw, fixed pitch, fixed roll);

/* Transform an absolute world position into camera-local view coordinates. */
void freak_camera_world_to_view(const struct freak_camera *camera,
                                const vec3 *world, vec3 *view);

#endif /* __freak_camera_h_ */
