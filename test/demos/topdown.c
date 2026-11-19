#include "democom.h"

void freak_demo_topdown(void)
{
    struct freak_tilemap map;
    struct freak_tile_atlas atlas;
    struct freak_rect player;
    struct freak_rect crate;
    struct freak_camera_2d camera;
    struct freak_topdown_door door;
    struct freak_topdown_trigger trigger;
    struct freak_topdown_path path;
    unsigned char visible[1000];
    unsigned char explored[1000];
    unsigned long last_tick;
    unsigned long now;
    unsigned long elapsed;
    int x;
    int y;
    int dx;
    int dy;
    int path_x;
    int path_y;
    unsigned int index;

    if (!freak_tilemap_init(&map, 40, 25, 8, 8, 2, 1))
        return;
    atlas.tiles = tile_sprites;
    atlas.count = 5;
    for (y = 0; y < map.height; ++y)
        for (x = 0; x < map.width; ++x) {
            freak_tilemap_set(&map, 0, (uint16_t)x, (uint16_t)y,
                              (uint16_t)((x + y) & 1));
            if (x == 0 || y == 0 || x == map.width - 1 ||
                y == map.height - 1 ||
                (x > 8 && x < 32 && y == 8) ||
                (x == 8 && y > 8 && y < 20) ||
                (x == 29 && y > 4 && y < 17) ||
                (y == 17 && x > 15 && x < 35)) {
                freak_tilemap_set(&map, 1, (uint16_t)x, (uint16_t)y, 2);
                freak_tilemap_set_collision(&map, (uint16_t)x, (uint16_t)y,
                                            FREAK_TILE_COLLISION_SOLID);
            }
        }
    player.x = 24;
    player.y = 24;
    player.width = 8;
    player.height = 8;
    crate.x = 184;
    crate.y = 120;
    crate.width = 8;
    crate.height = 8;
    door.x = 20;
    door.y = 8;
    freak_topdown_door_set(&map, &door, 0);
    trigger.bounds.x = 104;
    trigger.bounds.y = 32;
    trigger.bounds.width = 16;
    trigger.bounds.height = 16;
    trigger.active = 0;
    for (index = 0; index < 1000; ++index)
        explored[index] = 0;
    freak_camera_2d_set(&camera, 0, 0);
    last_tick = freak_timer_ticks();
    freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings,
                                FREAK_ACTION_BACK))
            break;
        now = freak_timer_ticks();
        elapsed = now - last_tick;
        last_tick = now;
        if (elapsed > 4) elapsed = 4;
        dx = 0;
        dy = 0;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_LEFT))
            dx = -(int)elapsed * 2;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_RIGHT))
            dx = (int)elapsed * 2;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_UP))
            dy = -(int)elapsed * 2;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_DOWN))
            dy = (int)elapsed * 2;
        freak_topdown_move_with_push(&map, &player, &crate, dx, dy,
                                     FREAK_TILE_COLLISION_SOLID);
        freak_topdown_trigger_test(&trigger, &player);
        freak_topdown_door_set(&map, &door, trigger.active);
        freak_tilemap_set(&map, 1, door.x, door.y,
                          door.open ? FREAK_TILE_EMPTY : 2);
        freak_topdown_fov(&map, player.x / 8, player.y / 8, 8,
                           FREAK_TILE_COLLISION_SOLID, visible, 1000);
        for (index = 0; index < 1000; ++index)
            if (visible[index]) explored[index] = 1;
        freak_topdown_pathfind(&map, player.x / 8, player.y / 8, 35, 20,
                                FREAK_TILE_COLLISION_SOLID, &path);
        freak_demo_palette_default();
        freak_clear_buffer(0, screen);
        freak_tilemap_draw(screen, &camera, &map, &atlas, 0);
        freak_tilemap_draw(screen, &camera, &map, &atlas, 1);
        for (y = 0; y < map.height; ++y)
            for (x = 0; x < map.width; ++x)
                if (!visible[(unsigned int)y * map.width + x])
                    freak_fill_rect(screen, x * 8, y * 8, x * 8 + 7, y * 8 + 7,
                                    explored[(unsigned int)y * map.width + x] ? 16 : 0);
        freak_fill_rect(screen, trigger.bounds.x, trigger.bounds.y,
                        trigger.bounds.x + trigger.bounds.width - 1,
                        trigger.bounds.y + trigger.bounds.height - 1,
                        trigger.active ? 15 : 60);
        freak_fill_rect(screen, crate.x, crate.y, crate.x + crate.width - 1,
                        crate.y + crate.height - 1, 100);
        for (index = 1; index < path.count; ++index) {
            path_x = path.x[index - 1] * 8 + 4;
            path_y = path.y[index - 1] * 8 + 4;
            freak_draw_line(screen, path_x, path_y,
                            path.x[index] * 8 + 4, path.y[index] * 8 + 4, 80);
        }
        freak_sprite_blit_key(screen, player.x, player.y, &runner_frames[0]);
        freak_draw_line(screen, player.x - 1, player.y - 1,
                        player.x + player.width, player.y - 1, 15);
        freak_draw_line(screen, player.x + player.width, player.y - 1,
                        player.x + player.width, player.y + player.height, 15);
        freak_draw_line(screen, player.x + player.width, player.y + player.height,
                        player.x - 1, player.y + player.height, 15);
        freak_draw_line(screen, player.x - 1, player.y + player.height,
                        player.x - 1, player.y - 1, 15);
        freak_font_text(screen, 8, 8, &font,
                        "Top-down toolkit: plate, door, crate, path & fog", 15);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
    }
    freak_tilemap_destroy(&map);
}
