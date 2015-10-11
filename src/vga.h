
/******************************************************************************
 * Freak
 *
 * The nostalic DOS games programming library
 ******************************************************************************/

#ifndef __freak_vga_h_

#define __freak_vga_h_

#include <stdint.h>

#define BIOS_VIDEO_80x25            0x03
#define BIOS_VIDEO_320x200x256      0x13

extern uint8_t *freak_vga;

void freak_set_video(uint8_t mode);
#pragma aux freak_set_video = \
  "mov    ah, 0"              \
  "int    0x10"               \
  parm [al];

/** Sets the video to 320x240x256 */
inline void freak_set_mcga() { freak_set_video(BIOS_VIDEO_320x200x256); }

/** Sets the video to 80x25 text */
inline void freak_set_text() { freak_set_video(BIOS_VIDEO_80x25); }

/** Waits for a vertical sync to occur */
void freak_wait_vsync();
#pragma aux freak_wait_vsync = \
  "mov  dx, 03dah"             \
  "@@vsync1:"                  \
  "in   al, dx"                \
  "test al, 08h"               \
  "jz   @@vsync1"              \
  "@@vsync2:"                  \
  "in   al, dx"                \
  "test al, 08h"               \
  "jnz  @@vsync2"              \
  modify [ax dx];

/** Clears a buffer with a value */
void freak_clear_buffer(uint8_t c, uint8_t *buf);
#pragma aux freak_clear_buffer = \
  "mov  ah, al"                  \
  "mov  bx, ax"                  \
  "shl  eax, 16"                 \
  "mov  ax, bx"                  \
  "mov  ecx, 16000"              \
  "rep  stosd"                   \
  modify [eax ebx ecx]           \
  parm [al] [edi]; 

/** Copies a buffer onto another */
void freak_copy_buffer(uint8_t *dest, uint8_t *src);
#pragma aux freak_copy_buffer = \
  "mov  ecx, 16000"             \
  "rep  movsd"                  \
  modify [ecx]                  \
  parm [edi] [esi];

void freak_set_palette(uint8_t c, uint8_t r, uint8_t g, uint8_t b);
#pragma aux freak_set_palette = \
  "mov  dx, 0x3c7"              \
  "out  dx, al"                 \
  "mov  dx, 0x3c9"              \
  "mov  al, bh"                 \
  "out  dx, al"                 \
  "mov  al, bl"                 \
  "out  dx, al"                 \
  "mov  al, ch"                 \
  "out  dx, al"                 \
  parm [al] [bh] [bl] [ch]      \
  modify [dx];

void freak_get_palette(uint8_t c, uint8_t *r, uint8_t *g, uint8_t *b);
#pragma aux freak_get_palette = \
  "mov  dx, 0x3c7"              \
  "out  dx, al"                 \
  "mov  dx, 0x3c8"              \
  "in   al, dx"                 \
  "mov  [ebx], al"              \
  "in   al, dx"                 \
  "mov  [ecx], al"              \
  "in   al, dx"                 \
  "mov  [edi], al"              \
  parm [dx] [ebx] [ecx] [edi]   \
  modify [dx];

#endif /* __freak_vga_h_ */
