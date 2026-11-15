/******************************************************************************
 * Freak PC speaker audio
 ******************************************************************************/

#ifndef __freak_audio_h_
#define __freak_audio_h_

#include <stdint.h>

/* Plays a square-wave tone through the PC speaker; zero silences it. */
void freak_audio_tone(uint16_t hertz);
void freak_audio_silence(void);

#endif /* __freak_audio_h_ */
