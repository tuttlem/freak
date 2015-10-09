
/******************************************************************************
 * Freak
 *
 * The nostalic DOS games programming library
 ******************************************************************************/

#ifndef __freak_kbd_h_

#define __freak_kbd_h_

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
