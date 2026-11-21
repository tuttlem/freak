#include "../src/freak.h"
#include "demoaud.h"
#include "demofx.h"
#include "demo3d.h"
#include "demo2d.h"
#include "demor3d.h"
#include "demosh3d.h"
#include "demoray.h"
#include "demotol3.h"
#include "demomat3.h"
#include "demoasst.h"
#include "demos/democom.h"

#define DEMO_QUIT 17
#define DEMO_RASTER3D 28
#define DEMO_LIGHT3D 29
#define DEMO_BUMP3D 30
#define DEMO_ENV3D 31
#define DEMO_GLENZ3D 32
#define DEMO_RAYCAST3D 33
#define DEMO_RAYTRACE3D 34
#define DEMO_MODEL3D 35
#define DEMO_CAMERA3D 36
#define DEMO_MATERIAL3D 37
#define DEMO_ASSETS 38
extern int freak_demo_texture_gallery(void);
extern int freak_demo_utilities(void);
extern int freak_demo_ui(void);
unsigned char *screen;
struct freak_font font = {(const unsigned char *)0x000FFA6EL, 8, 8, 0, 128};
struct freak_timing demo_timing;
static struct freak_demo_context context;
static struct freak_menu_style style = {1, 15, 15, 1, 12, 8, 6, 10};
#define I(s, a) {s, a, 0, 1}
static const struct freak_menu_item basicitems[] = {
    I("Flat triangles", 1),          I("Gouraud triangles", 2),
    I("Textured triangles", 3),      I("Spinning cube (flat)", 4),
    I("Spinning cube (Gouraud)", 5), I("Spinning cube (textured)", 6),
    I("Cube frame pacing", 7),       I("Rasterizer depth & clip", DEMO_RASTER3D),
    I("Materials & lightmaps", DEMO_MATERIAL3D),
    I("Back", FREAK_MENU_BACK)};
static const struct freak_menu basicmenu = {"Raster & materials", basicitems, 10};
static const struct freak_menu_item shadeitems[] = {
    I("3D lights & specular", DEMO_LIGHT3D), I("Bump mapping", DEMO_BUMP3D),
    I("Environment mapping", DEMO_ENV3D), I("Glenz translucency", DEMO_GLENZ3D),
    I("Back", FREAK_MENU_BACK)};
static const struct freak_menu shademenu = {"Shading", shadeitems, 5};
static const struct freak_menu_item rayitems[] = {
    I("Raycast maze", DEMO_RAYCAST3D), I("Raytrace preview", DEMO_RAYTRACE3D),
    I("Model pipeline", DEMO_MODEL3D), I("Camera toolkit", DEMO_CAMERA3D),
    I("Back", FREAK_MENU_BACK)};
static const struct freak_menu raymenu = {"Rays & 3D tools", rayitems, 5};
static const struct freak_menu_item ritems[] = {
    {"Raster & materials", 0, &basicmenu, 1},
    {"Lighting & effects", 0, &shademenu, 1},
    {"Rays, models & camera", 0, &raymenu, 1},
    I("Back", FREAK_MENU_BACK)};
static const struct freak_menu rmenu = {"3D software rendering", ritems, 4};
static const struct freak_menu_item titems[] = {I("Noise, plasma & fire", 8),
                                                I("Back", FREAK_MENU_BACK)};
static const struct freak_menu tmenu = {"Textures", titems, 2};
static const struct freak_menu_item uitems[] = {
    I("Sprites, fonts & transitions", 9), I("UI controls", 10),
    I("PC Speaker: music & effects", 25), I("Sound Blaster: PCM scale", 26),
    I("MIDI: MPU-401 scale", 27), I("Asset loading", DEMO_ASSETS),
    I("Back", FREAK_MENU_BACK)};
static const struct freak_menu umenu = {"Utilities", uitems, 7};
static const struct freak_menu_item eitems[] = {
    I("Palette fades & cycling", 18),
    I("Blend modes", 19),
    I("Lens flare", 20),
    I("Glass lens & distortion", 21),
    I("Scalers, rotozoom & tunnel", 22),
    I("Text effects & starfield", 23),
    I("Voxel height field", 24),
    I("Back", FREAK_MENU_BACK)};
static const struct freak_menu emenu = {"Effects", eitems, 8};
static const struct freak_menu_item ditems[] = {
    I("Camera & animated sprites", 11),
    I("Scrolling tile map", 12),
    I("Platformer movement", 13),
    I("Top-down collision", 14),
    I("Particles", 15),
    I("2D lighting", 16),
    I("Back", FREAK_MENU_BACK)};
static const struct freak_menu dmenu = {"2D", ditems, 7};
static const struct freak_menu_item mainitems[] = {
    {"2D", 0, &dmenu, 1},        {"Rendering", 0, &rmenu, 1},
    {"Textures", 0, &tmenu, 1},  {"Effects", 0, &emenu, 1},
    {"Utilities", 0, &umenu, 1}, I("Quit", 17)};
static const struct freak_menu mainmenu = {"Freak demo", mainitems, 6};

static void drawmenu(const struct freak_menu_stack *m) {
    const struct freak_menu *a = m->menus[m->depth];
    struct freak_text_size z;
    int i, c, w, h;
    z = freak_font_measure(&font, a->title);
    c = z.width / font.width;
    for (i = 0; i < a->count; ++i) {
        z = freak_font_measure(&font, a->items[i].label);
        if (z.width / font.width > c) {
            c = z.width / font.width;
        }
    }
    w = c * font.width + style.padding * 2;
    h = (a->count + 1) * style.row_height + style.padding * 2;
    freak_menu_draw(screen, (320 - w) / 2, (200 - h) / 2, &font, m, &style);
}

static int hit(const struct freak_menu_stack *m, int x, int y) {
    const struct freak_menu *a = m->menus[m->depth];
    struct freak_text_size z;
    int i, c, w, h, l, t, n;
    z = freak_font_measure(&font, a->title);
    c = z.width / font.width;
    for (i = 0; i < a->count; ++i) {
        z = freak_font_measure(&font, a->items[i].label);
        if (z.width / font.width > c) {
            c = z.width / font.width;
        }
    }
    w = c * font.width + style.padding * 2;
    h = (a->count + 1) * style.row_height + style.padding * 2;
    l = (320 - w) / 2;
    t = (200 - h) / 2;
    if (x < l + style.padding || x >= l + w - style.padding) {
        return -1;
    }
    n = (y - t - style.padding) / style.row_height - 1;
    return n < 0 || n >= a->count || !a->items[n].enabled ? -1 : n;
}

static void run(int a) {
    freak_mouse_hide();
    if (a <= 3) {
        freak_demo_triangles(a);
    } else if (a <= 7) {
        freak_demo_cube(a);
    } else if (a == DEMO_RASTER3D) {
        freak_demo_raster3d();
    } else if (a == DEMO_LIGHT3D) {
        freak_demo_lighting3d();
    } else if (a == DEMO_BUMP3D) {
        freak_demo_bump3d();
    } else if (a == DEMO_ENV3D) {
        freak_demo_environment3d();
    } else if (a == DEMO_GLENZ3D) {
        freak_demo_glenz3d();
    } else if (a == DEMO_RAYCAST3D) {
        freak_demo_raycast3d();
    } else if (a == DEMO_RAYTRACE3D) {
        freak_demo_raytrace3d();
    } else if (a == DEMO_MODEL3D) {
        freak_demo_model3d();
    } else if (a == DEMO_CAMERA3D) {
        freak_demo_camera3d();
    } else if (a == DEMO_MATERIAL3D) {
        freak_demo_material3d();
    } else if (a == 8) {
        freak_demo_texture_gallery();
    } else if (a == 9) {
        freak_demo_utilities();
    } else if (a == 10) {
        freak_demo_ui();
    } else if (a == 11) {
        freak_demo_sprites();
    } else if (a == 12) {
        freak_demo_tilemap();
    } else if (a == 13) {
        freak_demo_platformer();
    } else if (a == 14) {
        freak_demo_topdown();
    } else if (a == 15) {
        freak_demo_particles();
    } else if (a == 16) {
        freak_demo_lighting();
    } else if (a == 18) {
        freak_demo_palette(&context);
    } else if (a == 19) {
        freak_demo_blend(&context);
    } else if (a == 20) {
        freak_demo_lens_flare();
    } else if (a == 21) {
        freak_demo_distortion();
    } else if (a == 22) {
        freak_demo_xform();
    } else if (a == 23) {
        freak_demo_textfx();
    } else if (a == 24) {
        freak_demo_voxel();
    } else if (a == 25) {
        freak_demo_pc_speaker();
    } else if (a == 26) {
        freak_demo_sound_blaster(&context);
    } else if (a == 27) {
        freak_demo_midi(&context);
    } else if (a == DEMO_ASSETS) {
        freak_demo_assets();
    }
    freak_mouse_show();
    freak_input_clear_events();
}

int main(void) {
    struct freak_arena arena;
    struct freak_menu_stack menu;
    struct freak_input_event e;
    int a, n, go = 1;
    if (!freak_arena_init(&arena, 64000U)) {
        return 1;
    }
    screen = (unsigned char *)freak_arena_alloc(&arena, 64000U);
    if (!screen) {
        return 1;
    }
    context.screen = screen;
    context.font = &font;
    context.timing = &demo_timing;
    freak_set_mcga();
    if (!freak_timing_start(&demo_timing, 120)) {
        return 1;
    }
    freak_input_init();
    freak_input_update();
    freak_menu_open(&menu, &mainmenu);
    while (go) {
        freak_input_update();
        freak_timing_update(&demo_timing);
        freak_demo_palette_default();
        freak_clear_buffer(0, screen);
        drawmenu(&menu);
        freak_wait_vsync();
        freak_copy_buffer(freak_vga, screen);
        while (freak_input_poll_event(&e)) {
            if (e.type == FREAK_EVENT_MOUSE_MOVE) {
                n = hit(&menu, e.mouse_x, e.mouse_y);
                if (n >= 0) {
                    menu.selected[menu.depth] = n;
                }
            } else if (e.type == FREAK_EVENT_ACTION_PRESSED) {
                if (e.action == FREAK_ACTION_UP) {
                    freak_menu_move(&menu, -1);
                } else if (e.action == FREAK_ACTION_DOWN) {
                    freak_menu_move(&menu, 1);
                } else if (e.action == FREAK_ACTION_ACCEPT) {
                    a = freak_menu_accept(&menu);
                    if (a == DEMO_QUIT) {
                        go = 0;
                    } else if (a) {
                        run(a);
                    }
                } else if (e.action == FREAK_ACTION_BACK &&
                           freak_menu_back(&menu) == -1) {
                    go = 0;
                }
            } else if (e.type == FREAK_EVENT_MOUSE_BUTTON_DOWN &&
                       e.button == FREAK_MOUSE_LEFT) {
                n = hit(&menu, e.mouse_x, e.mouse_y);
                if (n >= 0) {
                    menu.selected[menu.depth] = n;
                    a = freak_menu_accept(&menu);
                    if (a == DEMO_QUIT) {
                        go = 0;
                    } else if (a) {
                        run(a);
                    }
                }
            }
        }
    }
    freak_input_shutdown();
    freak_timing_stop();
    freak_set_text();
    freak_arena_destroy(&arena);
    return 0;
}
