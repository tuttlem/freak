#ifndef __freak_mouse_h_
#define __freak_mouse_h_

#include <stdint.h>

#define FREAK_MOUSE_LEFT   0x0001
#define FREAK_MOUSE_RIGHT  0x0002
#define FREAK_MOUSE_MIDDLE 0x0004

/* Uses the DOS mouse driver (INT 33h). Returns non-zero when installed. */
uint8_t freak_mouse_init(void);
void freak_mouse_shutdown(void);
void freak_mouse_update(void);
uint8_t freak_mouse_available(void);
int freak_mouse_x(void);
int freak_mouse_y(void);
uint16_t freak_mouse_buttons(void);
uint8_t freak_mouse_down(uint16_t button);
uint8_t freak_mouse_pressed(uint16_t button);
uint8_t freak_mouse_released(uint16_t button);
void freak_mouse_show(void);
void freak_mouse_hide(void);

#endif
