#include <conio.h>

#include "audio.h"

#define PIT_CONTROL_PORT 0x43
#define PIT_CHANNEL_2    0x42
#define SPEAKER_PORT     0x61
#define PIT_BASE_HZ      1193182UL

void freak_audio_silence(void)
{
    outp(SPEAKER_PORT, inp(SPEAKER_PORT) & 0xfc);
}

void freak_audio_tone(uint16_t hertz)
{
    unsigned int divisor;

    if (hertz == 0) {
        freak_audio_silence();
        return;
    }

    divisor = (unsigned int)(PIT_BASE_HZ / hertz);
    if (divisor == 0) {
        divisor = 1;
    }
    outp(PIT_CONTROL_PORT, 0xb6);
    outp(PIT_CHANNEL_2, divisor & 0xff);
    outp(PIT_CHANNEL_2, divisor >> 8);
    outp(SPEAKER_PORT, inp(SPEAKER_PORT) | 0x03);
}
