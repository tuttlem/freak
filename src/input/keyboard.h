
/******************************************************************************
 * Freak
 *
 * The nostalic DOS games programming library
 ******************************************************************************/

#ifndef __freak_kbd_h_

#define __freak_kbd_h_

#include <stdint.h>

#define FREAK_KEY_ESCAPE 0x01
#define FREAK_KEY_0      0x0b
#define FREAK_KEY_BACKSPACE 0x0e
#define FREAK_KEY_R      0x13
#define FREAK_KEY_ENTER  0x1c
#define FREAK_KEY_UP     0x48
#define FREAK_KEY_DOWN   0x50
#define FREAK_KEY_LEFT   0x4b
#define FREAK_KEY_RIGHT  0x4d
#define FREAK_KEY_A      0x1e
#define FREAK_KEY_D      0x20
#define FREAK_KEY_S      0x1f
#define FREAK_KEY_W      0x11

/* Installs a raw IRQ-1 handler for continuous keyboard state. */
void freak_kbd_init(void);

/* Restores BIOS keyboard handling. */
void freak_kbd_shutdown(void);

/* Returns non-zero while the key identified by an XT scan code is held. */
uint8_t freak_key_down(uint8_t scan_code);

/* freak_getch
 * Returns the next available character in the keyboard buffer; if no character
 * is available, this service will wait
 * 
 * Returns: low-8-bits  ascii character code
 *          high-8-bits scan code
 */
unsigned short freak_getch();
#pragma aux freak_getch = \
  "mov    ah, 0"          \
  "int    0x16"           \
  value [ax];

/* freak_kbhit
 * Checks to see if a character is available in the buffer
 *
 * Returns: 1 a key press was waiting in the buffer, otherwise 0
 */
unsigned char freak_kbhit();
#pragma aux freak_kbhit = \
  "mov    ah, 1"          \
  "int    0x16"           \
  "jz     @@kbhit_no_key" \
  "mov    al, 1"          \
  "jmp    @@kbhit_done"   \
  "@@kbhit_no_key:"       \
  "xor    al, al"         \
  "@@kbhit_done:"         \
  value [al];

#endif /* __freak_kbd_h_ */
