
/******************************************************************************
 * Freak
 *
 * The nostalic DOS games programming library
 ******************************************************************************/

#ifndef __freak_vga_h_

#define __freak_vga_h_

extern unsigned char *freak_vga;

/** Sets the video to 320x240x256 */
extern void freak_set_modex();

/** Resets video back to 80x25 text */
extern void freak_set_text();

/** Waits for a vertical sync to occur */
extern void freak_wait_vsync();

#endif /* __freak_vga_h_ */
