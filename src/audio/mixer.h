#ifndef __freak_mixer_h_
#define __freak_mixer_h_

#include <stdint.h>

#define FREAK_MIXER_MAX_CHANNELS 8

struct freak_audio_sample {
    int8_t *data;
    uint32_t length;
    uint32_t loop_start;
    uint32_t loop_end;
    uint16_t sample_rate;
    uint8_t loop;
};

struct freak_mixer_channel {
    const struct freak_audio_sample *sample;
    uint32_t position;
    uint32_t step;
    uint8_t volume;
    uint8_t pan;
    uint8_t loop;
    uint8_t active;
};

struct freak_mixer {
    struct freak_mixer_channel channels[FREAK_MIXER_MAX_CHANNELS];
    uint16_t output_rate;
    uint8_t master_volume;
};

void freak_mixer_init(struct freak_mixer *mixer, uint16_t output_rate);
int freak_mixer_play(struct freak_mixer *mixer,
                     const struct freak_audio_sample *sample,
                     uint16_t frequency, uint8_t volume, uint8_t pan,
                     uint8_t loop);
void freak_mixer_stop(struct freak_mixer *mixer, int channel);
void freak_mixer_mix(struct freak_mixer *mixer, int8_t *output,
                     unsigned int frames);
/* Interleaved signed 8-bit stereo.  Pan 0 is left, 128 centre, 255 right. */
void freak_mixer_mix_stereo(struct freak_mixer *mixer, int8_t *output,
                            unsigned int frames);
/* Sound Blaster-ready unsigned 8-bit mono output (silence is 128). */
void freak_mixer_mix_unsigned(struct freak_mixer *mixer, uint8_t *output,
                              unsigned int frames);
unsigned int freak_mixer_active(const struct freak_mixer *mixer);

#endif
