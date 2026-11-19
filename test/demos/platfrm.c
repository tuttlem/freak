#include "democom.h"
void freak_demo_platformer(void)
{
    struct freak_tilemap map;
    struct freak_tile_atlas atlas;
    struct freak_camera_2d camera;
    struct freak_platformer_body player;
    struct freak_platformer_rules rules;
    struct freak_sprite_2d player_sprite;
    struct freak_sprite_animation animation;
    struct freak_moving_platform moving[1];
    struct freak_sprite_2d moving_sprite;
    unsigned long last_tick;
    unsigned long now;
    unsigned long elapsed;
    int x;
    int y;
    int direction;
    int vertical;

    if (!freak_tilemap_init(&map, 80, 25, 8, 8, 2, 1))
        return;
    atlas.tiles = tile_sprites;
    atlas.count = 5;
    for (y = 0; y < map.height; ++y) {
        for (x = 0; x < map.width; ++x) {
            freak_tilemap_set(&map, 0, (uint16_t)x, (uint16_t)y,
                              (uint16_t)((x + y) & 1));
            if (y >= 22) {
                freak_tilemap_set(&map, 1, (uint16_t)x, (uint16_t)y, 2);
                freak_tilemap_set_collision(&map, (uint16_t)x, (uint16_t)y,
                                            FREAK_TILE_COLLISION_SOLID);
            } else if ((y == 16 && x > 8 && x < 18) ||
                       (y == 12 && x > 30 && x < 40) ||
                       (y == 18 && x > 52 && x < 64)) {
                freak_tilemap_set(&map, 1, (uint16_t)x, (uint16_t)y, 2);
                freak_tilemap_set_collision(&map, (uint16_t)x, (uint16_t)y,
                                            FREAK_TILE_COLLISION_ONE_WAY);
            }
        }
    }
    /* A 16-tile hill: a continuous eight-tile rise and eight-tile descent. */
    for (x = 18; x < 26; ++x) {
        y = 21 - (x - 18);
        freak_tilemap_set(&map, 1, (uint16_t)x, (uint16_t)y, 3);
        freak_tilemap_set_collision(&map, (uint16_t)x, (uint16_t)y,
                                    FREAK_TILE_COLLISION_SLOPE_UP_RIGHT);
        for (++y; y < 22; ++y) {
            freak_tilemap_set(&map, 1, (uint16_t)x, (uint16_t)y, 2);
            freak_tilemap_set_collision(&map, (uint16_t)x, (uint16_t)y,
                                        FREAK_TILE_COLLISION_SOLID);
        }
    }
    for (x = 26; x < 34; ++x) {
        y = 14 + (x - 26);
        freak_tilemap_set(&map, 1, (uint16_t)x, (uint16_t)y, 4);
        freak_tilemap_set_collision(&map, (uint16_t)x, (uint16_t)y,
                                    FREAK_TILE_COLLISION_SLOPE_UP_LEFT);
        for (++y; y < 22; ++y) {
            freak_tilemap_set(&map, 1, (uint16_t)x, (uint16_t)y, 2);
            freak_tilemap_set_collision(&map, (uint16_t)x, (uint16_t)y,
                                        FREAK_TILE_COLLISION_SOLID);
        }
    }
    for (y = 16; y < 22; ++y) {
        freak_tilemap_set(&map, 1, 12, (uint16_t)y, 2);
        freak_tilemap_set_collision(&map, 12, (uint16_t)y,
                                    FREAK_TILE_COLLISION_LADDER |
                                    FREAK_TILE_COLLISION_ONE_WAY);
    }
    rules.run_speed = double_to_fixed(0.80);
    rules.gravity = double_to_fixed(0.10);
    rules.jump_speed = double_to_fixed(3.40);
    rules.max_fall_speed = double_to_fixed(3.20);
    rules.climb_speed = double_to_fixed(0.75);
    freak_platformer_body_init(&player, 32, 168, 8, 8);
    player.grounded = 1;
    freak_sprite_animation_init(&animation, runner_frames, 4, 12);
    player_sprite.keyed = 1;
    player_sprite.flip_x = 0;
    freak_moving_platform_init(&moving[0], 336, 136, 48, 8,
                               336, 448, 136, 136,
                               double_to_fixed(0.45), 0);
    moving_sprite.sprite = &tile_sprites[2];
    moving_sprite.keyed = 1;
    moving_sprite.flip_x = 0;
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
        direction = 0;
        vertical = 0;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_LEFT))
            direction = -1;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_RIGHT))
            direction = 1;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_UP))
            vertical = -1;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_DOWN))
            vertical = 1;
        freak_moving_platform_step(moving, elapsed);
        freak_platformer_step(&player, &map, &rules, direction, vertical,
                              freak_input_pressed(&freak_input_default_bindings,
                                                  FREAK_ACTION_UP),
                              freak_input_pressed(&freak_input_default_bindings,
                                                  FREAK_ACTION_DOWN), elapsed);
        freak_platformer_resolve_moving_platforms(&player, moving, 1);
        if (direction != 0)
            player_sprite.flip_x = direction < 0;
        if (direction != 0)
            freak_sprite_animation_advance(&animation, elapsed);
        player_sprite.sprite = freak_sprite_animation_frame(&animation);
        player_sprite.x = fixed_to_int(player.x);
        player_sprite.y = fixed_to_int(player.y);
        freak_camera_2d_set(&camera, player_sprite.x - 160, player_sprite.y - 100);
        if (camera.x < 0) camera.x = 0;
        if (camera.y < 0) camera.y = 0;
        if (camera.x > map.width * map.tile_width - 320)
            camera.x = map.width * map.tile_width - 320;
        if (camera.y > map.height * map.tile_height - 200)
            camera.y = map.height * map.tile_height - 200;
        freak_demo_palette_default();
        freak_clear_buffer(0, screen);
        freak_tilemap_draw(screen, &camera, &map, &atlas, 0);
        freak_tilemap_draw(screen, &camera, &map, &atlas, 1);
        moving_sprite.x = fixed_to_int(moving[0].x);
        moving_sprite.y = fixed_to_int(moving[0].y);
        for (x = 0; x < 6; ++x) {
            moving_sprite.x = fixed_to_int(moving[0].x) + x * 8;
            freak_sprite_2d_draw(screen, &camera, &moving_sprite);
        }
        freak_sprite_2d_draw(screen, &camera, &player_sprite);
        freak_font_text(screen, 8, 8, &font,
                        "Platformer: slopes, ladders & moving platform", 15);
        freak_font_text(screen, 8, 18, &font,
                        "Arrows: move/jump/climb/drop  Esc: back", 15);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
    }
    freak_tilemap_destroy(&map);
}

