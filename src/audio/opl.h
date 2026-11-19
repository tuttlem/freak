#ifndef __freak_opl_h_
#define __freak_opl_h_

#include <stdint.h>

#define FREAK_OPL_CHANNELS 9

struct freak_imf_song {
    const uint8_t *data;
    uint32_t length;
    uint32_t position;
    uint16_t delay;
    uint8_t loop;
    uint8_t playing;
};

uint8_t freak_opl_available(void);
void freak_opl_write(uint8_t reg, uint8_t value);
void freak_opl_silence(void);
void freak_opl_note_on(uint8_t channel, uint16_t frequency, uint8_t block);
void freak_opl_note_off(uint8_t channel);
void freak_imf_play(struct freak_imf_song *song, const uint8_t *data,
                    uint32_t length, uint8_t loop);
void freak_imf_stop(struct freak_imf_song *song);
void freak_imf_update(struct freak_imf_song *song, unsigned long ticks_700hz);

#endif
