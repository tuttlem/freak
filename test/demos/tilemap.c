#include "democom.h"

void freak_demo_tilemap(void)
{
    struct freak_tilemap map;
    struct freak_tile_atlas atlas;
    struct freak_camera_2d camera;
    unsigned long last_tick;
    unsigned long now;
    unsigned long elapsed;
    int x;
    int y;

    if (!freak_tilemap_init(&map, 96, 32, 8, 8, 2, 1))
        return;
    atlas.tiles = tile_sprites;
    atlas.count = 5;
    for (y = 0; y < map.height; ++y) {
        for (x = 0; x < map.width; ++x) {
            freak_tilemap_set(&map, 0, (uint16_t)x, (uint16_t)y,
                              (uint16_t)(((x / 4 + y / 3) & 1) ? 0 : 1));
            if (y == 22 || (y == 17 && (x % 12) < 6)) {
                freak_tilemap_set(&map, 1, (uint16_t)x, (uint16_t)y, 2);
                freak_tilemap_set_collision(&map, (uint16_t)x, (uint16_t)y, 1);
            }
        }
    }
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
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_LEFT))
            camera.x -= (int)elapsed * 2;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_RIGHT))
            camera.x += (int)elapsed * 2;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_UP))
            camera.y -= (int)elapsed * 2;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_DOWN))
            camera.y += (int)elapsed * 2;
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
        freak_font_text(screen, 8, 8, &font,
                        "Scrolling tile map  Arrows: scroll  Esc: back", 15);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
    }
    freak_tilemap_destroy(&map);
}
