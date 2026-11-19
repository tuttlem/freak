/******************************************************************************
 * Freak camera pose and world-to-view transform
 *
 * Angles use Freak's 16.16 trig-table units: 1024 is one complete turn.
 * The view coordinate system is right-handed: +X right, +Y up, +Z forward.
 ******************************************************************************/

#ifndef __freak_camera_h_
#define __freak_camera_h_

#include "../core/vec.h"

struct freak_camera {
    vec3 position;
    fixed yaw;
    fixed pitch;
    fixed roll;
};

struct freak_camera_rail_key {
    vec3 position;
    fixed yaw;
    fixed pitch;
};

struct freak_camera_spring {
    vec3 position_velocity;
    fixed yaw_velocity;
    fixed pitch_velocity;
};

void freak_camera_init(struct freak_camera *camera);
void freak_camera_set_pose(struct freak_camera *camera, const vec3 *position,
                           fixed yaw, fixed pitch, fixed roll);

/* Transform an absolute world position into camera-local view coordinates. */
void freak_camera_world_to_view(const struct freak_camera *camera,
                                const vec3 *world, vec3 *view);

/* Movement and framing helpers. All speed values are world units per tick in
   16.16 fixed point; angles use the normal 1024-units-per-turn convention. */
void freak_camera_fps_move(struct freak_camera *camera, fixed forward,
                           fixed strafe, fixed rise);
void freak_camera_orbit(struct freak_camera *camera, const vec3 *target,
                        fixed yaw, fixed pitch, fixed distance);
void freak_camera_chase(struct freak_camera *camera, const vec3 *target,
                        fixed yaw, fixed height, fixed distance);
void freak_camera_rail(struct freak_camera *camera,
                       const struct freak_camera_rail_key *keys,
                       unsigned int count, fixed position);
void freak_camera_shake(struct freak_camera *camera, fixed amount,
                        unsigned long tick);
void freak_camera_smooth_to(struct freak_camera *camera,
                            struct freak_camera_spring *spring,
                            const struct freak_camera *target,
                            fixed response, fixed damping);

#endif /* __freak_camera_h_ */
