#include "democom.h"

void freak_demo_particles(void)
{
    struct freak_particles particles;
    struct freak_particle_emitter fire;
    struct freak_particle_emitter smoke;
    struct freak_particle_emitter spark;
    unsigned long last_tick;
    unsigned long now;
    unsigned long elapsed;
    int source_x;
    int source_y;

    source_x = 160;
    source_y = 164;
    fire.velocity_x_min = -75; fire.velocity_x_max = 75;
    fire.velocity_y_min = -290; fire.velocity_y_max = -100;
    fire.gravity = 12; fire.life_min = 14; fire.life_max = 32;
    fire.first_colour = 95; fire.last_colour = 64;
    fire.size_min = 1; fire.size_max = 3;
    fire.flags = FREAK_PARTICLE_ADDITIVE;
    smoke.velocity_x_min = -35; smoke.velocity_x_max = 35;
    smoke.velocity_y_min = -130; smoke.velocity_y_max = -45;
    smoke.gravity = -2; smoke.life_min = 30; smoke.life_max = 58;
    smoke.first_colour = 48; smoke.last_colour = 16;
    smoke.size_min = 1; smoke.size_max = 2;
    smoke.flags = FREAK_PARTICLE_TRANSLUCENT;
    spark.velocity_x_min = -270; spark.velocity_x_max = 270;
    spark.velocity_y_min = -340; spark.velocity_y_max = -80;
    spark.gravity = 20; spark.life_min = 18; spark.life_max = 42;
    spark.first_colour = 15; spark.last_colour = 64;
    spark.size_min = 1; spark.size_max = 1;
    spark.flags = FREAK_PARTICLE_TRAIL | FREAK_PARTICLE_ADDITIVE;
    freak_particles_init(&particles, 0x50415254UL);
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
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_LEFT))
            source_x -= (int)elapsed * 3;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_RIGHT))
            source_x += (int)elapsed * 3;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_UP))
            source_y -= (int)elapsed * 3;
        if (freak_input_down(&freak_input_default_bindings, FREAK_ACTION_DOWN))
            source_y += (int)elapsed * 3;
        if (source_x < 8) source_x = 8;
        if (source_x > 311) source_x = 311;
        if (source_y < 24) source_y = 24;
        if (source_y > 191) source_y = 191;
        fire.x = source_x; fire.y = source_y;
        smoke.x = source_x; smoke.y = source_y + 2;
        spark.x = source_x; spark.y = source_y;
        freak_particles_emit(&particles, &fire, (unsigned int)(elapsed * 3));
        freak_particles_emit(&particles, &smoke, (unsigned int)elapsed);
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_ACCEPT))
            freak_particles_emit(&particles, &spark, 48);
        freak_particles_update(&particles, elapsed);
        freak_demo_palette_default();
        freak_clear_buffer(0, screen);
        freak_particles_draw(&particles, screen);
        freak_fill_rect(screen, source_x - 3, source_y + 3,
                        source_x + 3, source_y + 5, 48);
        freak_font_text(screen, 8, 8, &font,
                        "Particles: fire + smoke  Enter: sparks  Esc: back", 15);
        freak_font_text(screen, 8, 18, &font,
                        "Arrows move the emitter", 15);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
    }
}
