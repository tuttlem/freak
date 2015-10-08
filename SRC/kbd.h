
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

/* freak_kbhit
 * Checks to see if a character is available in the buffer
 *
 * Returns: low-8-bits  ascii character code
 *          high-8-bits scan code
 */
unsigned char freak_kbhit();

#endif /* __freak_kbd_h_ */
