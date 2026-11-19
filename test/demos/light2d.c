#include <malloc.h>
#include "democom.h"
void freak_demo_lighting(void)
{
    struct freak_lighting2d lighting;
    struct freak_light2d light;
    struct freak_tilemap occlusion;
    unsigned char *lightmap;
    unsigned long last_tick;
    unsigned long now;
    unsigned long elapsed;
    int player_x;
    int player_y;
    int x;
    int y;

    lightmap = (unsigned char *)malloc(FREAK_SCREEN_WIDTH * FREAK_SCREEN_HEIGHT);
    if (!lightmap) return;
    if (!freak_tilemap_init(&occlusion, 40, 25, 8, 8, 1, 1)) {
        free(lightmap);
        return;
    }
    for (y = 0; y < 25; ++y)
        for (x = 0; x < 40; ++x)
            if ((y == 4 && x >= 2 && x <= 37) ||
                (y == 20 && x >= 2 && x <= 37) ||
                (x == 14 && y >= 7 && y <= 18) ||
                (x == 26 && y >= 7 && y <= 18) ||
                (y == 12 && x >= 15 && x <= 25))
                freak_tilemap_set_collision(&occlusion, (uint16_t)x, (uint16_t)y,
                                            FREAK_TILE_COLLISION_SOLID);
    player_x = 160;
    player_y = 104;
    freak_lighting2d_init(&lighting, 38);
    light.x = player_x; light.y = player_y; light.radius = 52;
    light.intensity = 230; light.colour = 15; light.flicker = 0; light.enabled = 1;
    freak_lighting2d_add(&lighting, &light);
    light.x = 64; light.y = 60; light.radius = 42;
    light.intensity = 200; light.colour = 92; light.flicker = 17; light.enabled = 1;
    freak_lighting2d_add(&lighting, &light);
    light.x = 260; light.y = 146; light.radius = 48;
    light.intensity = 205; light.colour = 88; light.flicker = 23; light.enabled = 1;
    freak_lighting2d_add(&lighting, &light);
    last_tick = freak_timer_ticks();
    freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK))
            break;
        now = freak_timer_ticks(); elapsed = now - last_tick; last_tick = now;
        if (elapsed > 4) elapsed = 4;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_LEFT)) player_x -= (int)elapsed * 3;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_RIGHT)) player_x += (int)elapsed * 3;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_UP)) player_y -= (int)elapsed * 3;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_DOWN)) player_y += (int)elapsed * 3;
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_ACCEPT))
            lighting.ambient = lighting.ambient < 100 ? 150 : 38;
        lighting.lights[0].x = player_x;
        lighting.lights[0].y = player_y;
        freak_lighting2d_update(&lighting, elapsed);
        freak_demo_palette_default();
        freak_clear_buffer(48, screen);
        freak_fill_rect(screen, 16, 32, 303, 39, 32);
        freak_fill_rect(screen, 16, 160, 303, 167, 32);
        freak_fill_rect(screen, 112, 56, 119, 151, 32);
        freak_fill_rect(screen, 208, 56, 215, 151, 32);
        freak_fill_rect(screen, 120, 96, 207, 103, 32);
        freak_lighting2d_render_occluded(&lighting, &occlusion,
                                         FREAK_TILE_COLLISION_SOLID, lightmap,
                                         FREAK_SCREEN_WIDTH, FREAK_SCREEN_HEIGHT);
        freak_lighting2d_shade(screen, lightmap, FREAK_SCREEN_WIDTH,
                               FREAK_SCREEN_HEIGHT);
        freak_lighting2d_colour_glow(&lighting, screen, FREAK_SCREEN_WIDTH,
                                     FREAK_SCREEN_HEIGHT);
        freak_fill_rect(screen, player_x - 3, player_y - 3,
                        player_x + 3, player_y + 3, 15);
        freak_font_text(screen, 8, 8, &font,
                        "2D lights: radial, flicker & palette glow", 15);
        freak_font_text(screen, 8, 18, &font,
                        "Arrows: move light  Enter: day/night  Esc: back", 15);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
    }
    freak_tilemap_destroy(&occlusion);
    free(lightmap);
}

