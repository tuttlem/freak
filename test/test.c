
#include <malloc.h>

#include "../src/freak.h"

unsigned char *dbl_buffer, *texture;
unsigned int ticks = 0;
char key_char = 0x00;

struct func_set_t {
    void (*init)(void);
    void (*frame)(unsigned int t);
    void (*die)(void);
};

void blank_init();
void blank_frame(unsigned int t);
void blank_die();

struct func_set_t  blank_funcs = { 
    blank_init, 
    blank_frame,
    blank_die
};

void tri_c_init();
void tri_c_frame(unsigned int t);
void tri_c_die();

struct func_set_t  tri_c_funcs = { 
    tri_c_init, 
    tri_c_frame,
    tri_c_die
};

void tri_g_init();
void tri_g_frame(unsigned int t);
void tri_g_die();

struct func_set_t  tri_g_funcs = { 
    tri_g_init, 
    tri_g_frame,
    tri_g_die
};

void tri_t_init();
void tri_t_frame(unsigned int t);
void tri_t_die();

struct func_set_t  tri_t_funcs = { 
    tri_t_init, 
    tri_t_frame,
    tri_t_die
};

struct func_set_t *live_funcs = &blank_funcs;

void switch_funcs(struct func_set_t *s);

int main(int argc, char *argv[]) {

    /* create the double buffer (that any implementation will use) */
    dbl_buffer = (unsigned char *)malloc(64000);

    /* drop into mcga */
    freak_set_mcga();

    /* keep going until we've encountered the ESC key */
    while (key_char != 27) { 

        /* draw a frame (if we have an implmentation set */
        if (live_funcs && live_funcs->frame) {
            live_funcs->frame(ticks);
        }

        /* copy over anything drawn onto the back buffer into the vga */
        freak_copy_buffer(freak_vga, dbl_buffer);

        /* keyboard handler */
        if (freak_kbhit()) { 

            key_char = freak_getch(); 

            switch (key_char) {
                case '1': 
                    switch_funcs(&tri_c_funcs);    
                    break;

                case '2': 
                    switch_funcs(&tri_g_funcs);    
                    break;

                case '3': 
                    switch_funcs(&tri_t_funcs);    
                    break;

                case ' ':
                    switch_funcs(NULL);
                    break;

                case '0':
                    switch_funcs(&blank_funcs);
                    break;
            }

        }
    
        ticks ++;
    }

    switch_funcs(NULL);

    freak_set_text();

    free(dbl_buffer);

    return 0;
}

void switch_funcs(struct func_set_t *s) {

    if (s == live_funcs) { return; }

    if (live_funcs) {
        live_funcs->die();
    }

    ticks = 0;
    live_funcs = s;

    if (live_funcs) {
        live_funcs->init();
    }

}

/************************************************************************
 * Blank implementation
 ************************************************************************/

void blank_init() { }

void blank_frame(unsigned int t) {
    freak_clear_buffer(0x00, dbl_buffer);
}

void blank_die() { }

/************************************************************************
 * Colour triangles
 ************************************************************************/

void tri_c_init() { 
    freak_clear_buffer(0x00, dbl_buffer);
}

void tri_c_frame(unsigned int t) {

    freak_tri_c(
        rand() % 320, rand() % 200,
        rand() % 320, rand() % 200,
        rand() % 320, rand() % 200,
        rand() % 256,
        dbl_buffer
    );

}

void tri_c_die() { }

/************************************************************************
 * Shaded triangles
 ************************************************************************/

void tri_g_init() { 
    freak_clear_buffer(0x00, dbl_buffer);
}

void tri_g_frame(unsigned int t) {

    freak_tri_g(
        rand() % 320, rand() % 200,
        rand() % 320, rand() % 200,
        rand() % 320, rand() % 200,
        rand() % 256, rand() % 256, rand() % 256,
        dbl_buffer
    );

}

void tri_g_die() { }

/************************************************************************
 * Textured triangles
 ************************************************************************/

void tri_t_init() { 
    int x, y;

    texture = (unsigned char *)malloc(256 * 256);

    for (x = 0; x < 256; x ++) {
        for (y = 0; y < 256; y ++) {
            texture[x + (y << 8)] = 16 + ((x ^ y) % 16);
        }
    }

    freak_clear_buffer(0x00, dbl_buffer);
}

void tri_t_frame(unsigned int t) {

    freak_tri_t(
        rand() % 320, rand() % 200,
        rand() % 320, rand() % 200,
        rand() % 320, rand() % 200,
        127, 127, 255, 255, 0, 255,
        texture, dbl_buffer
    );

}

void tri_t_die() { 
    free(texture);
    texture = NULL;
}

