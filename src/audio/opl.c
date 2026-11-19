#include <conio.h>

#include "opl.h"

#define OPL_ADDRESS 0x388
#define OPL_DATA 0x389

static void opl_delay(void)
{
    unsigned int index;
    for (index = 0; index < 12; ++index) inp(OPL_ADDRESS);
}

static void opl_wait(unsigned int count)
{
    while (count--) inp(OPL_ADDRESS);
}

uint8_t freak_opl_available(void)
{
    uint8_t before, status;
    /* Standard AdLib timer test.  It avoids relying on BLASTER, since an
       AdLib-compatible OPL2 can exist without a Sound Blaster DSP. */
    outp(OPL_ADDRESS, 4); opl_delay(); outp(OPL_DATA, 0x60); opl_delay();
    outp(OPL_ADDRESS, 4); opl_delay(); outp(OPL_DATA, 0x80); opl_delay();
    before = (uint8_t)inp(OPL_ADDRESS);
    outp(OPL_ADDRESS, 2); opl_delay(); outp(OPL_DATA, 0xff); opl_delay();
    outp(OPL_ADDRESS, 4); opl_delay(); outp(OPL_DATA, 0x21); opl_wait(300);
    status = (uint8_t)inp(OPL_ADDRESS);
    outp(OPL_ADDRESS, 4); opl_delay(); outp(OPL_DATA, 0x60); opl_delay();
    outp(OPL_ADDRESS, 4); opl_delay(); outp(OPL_DATA, 0x80); opl_delay();
    return (uint8_t)((before & 0xe0) == 0 && (status & 0xe0) == 0xc0);
}

void freak_opl_write(uint8_t reg, uint8_t value)
{
    outp(OPL_ADDRESS, reg); opl_delay(); outp(OPL_DATA, value); opl_delay();
}

void freak_opl_silence(void)
{
    uint8_t channel;
    for (channel = 0; channel < FREAK_OPL_CHANNELS; ++channel)
        freak_opl_write((uint8_t)(0xb0 + channel), 0);
}

void freak_opl_note_on(uint8_t channel, uint16_t frequency, uint8_t block)
{
    if (channel >= FREAK_OPL_CHANNELS) return;
    freak_opl_write((uint8_t)(0xa0 + channel), (uint8_t)frequency);
    freak_opl_write((uint8_t)(0xb0 + channel),
                    (uint8_t)(0x20 | ((block & 7) << 2) | (frequency >> 8)));
}

void freak_opl_note_off(uint8_t channel)
{
    if (channel < FREAK_OPL_CHANNELS) freak_opl_write((uint8_t)(0xb0 + channel), 0);
}

void freak_imf_play(struct freak_imf_song *song, const uint8_t *data,
                    uint32_t length, uint8_t loop)
{
    if (!song) return;
    song->data=data; song->length=length; song->position=0; song->delay=0;
    song->loop=loop; song->playing=data != 0 && length >= 4;
}
void freak_imf_stop(struct freak_imf_song *song) { if (song) song->playing=0; }

void freak_imf_update(struct freak_imf_song *song, unsigned long ticks_700hz)
{
    uint16_t delay;
    if (!song || !song->playing) return;
    while (ticks_700hz && song->playing) {
        if (song->delay) { --song->delay; --ticks_700hz; continue; }
        if (song->position + 4 > song->length) {
            if (song->loop) song->position=0; else { song->playing=0; break; }
        }
        freak_opl_write(song->data[song->position], song->data[song->position+1]);
        delay = (uint16_t)(song->data[song->position+2] |
                           (song->data[song->position+3] << 8));
        song->position += 4;
        song->delay = delay;
    }
}
