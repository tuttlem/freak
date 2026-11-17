#ifndef __freak_camera_2d_h_
#define __freak_camera_2d_h_

struct freak_camera_2d {
    int x;
    int y;
};

void freak_camera_2d_set(struct freak_camera_2d *camera, int x, int y);
void freak_camera_2d_world_to_screen(const struct freak_camera_2d *camera,
                                     int world_x, int world_y,
                                     int *screen_x, int *screen_y);

#endif
