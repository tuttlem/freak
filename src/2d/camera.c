#include "camera.h"

void freak_camera_2d_set(struct freak_camera_2d *camera, int x, int y)
{
    if (camera == 0)
        return;
    camera->x = x;
    camera->y = y;
}

void freak_camera_2d_world_to_screen(const struct freak_camera_2d *camera,
                                     int world_x, int world_y,
                                     int *screen_x, int *screen_y)
{
    if (camera == 0 || screen_x == 0 || screen_y == 0)
        return;
    *screen_x = world_x - camera->x;
    *screen_y = world_y - camera->y;
}
