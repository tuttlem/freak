#ifndef __freak_audio_asset_h_
#define __freak_audio_asset_h_

#include "mixer.h"

int freak_audio_sample_load_raw(struct freak_audio_sample *sample,
                                const char *filename, uint16_t sample_rate,
                                uint8_t unsigned_pcm);
int freak_audio_sample_load_wav(struct freak_audio_sample *sample,
                                const char *filename);
void freak_audio_sample_init(struct freak_audio_sample *sample);
void freak_audio_sample_destroy(struct freak_audio_sample *sample);
void freak_audio_sample_set_loop(struct freak_audio_sample *sample,
                                 uint32_t start, uint32_t end);
/* Offline-style preparation helpers, also useful at load time. */
int freak_audio_sample_resample(struct freak_audio_sample *sample,
                                uint16_t output_rate);
void freak_audio_sample_normalise(struct freak_audio_sample *sample,
                                  uint8_t peak);
int freak_audio_sample_save_raw(const struct freak_audio_sample *sample,
                                const char *filename, uint8_t unsigned_pcm);

/* A deliberately tiny sound-bank: FSBK + count, followed by named raw 8-bit
   samples.  Banks keep DOS data loading simple and deterministic. */
struct freak_sound_bank_entry {
    char name[13];
    struct freak_audio_sample sample;
};
struct freak_sound_bank {
    struct freak_sound_bank_entry *entries;
    uint16_t count;
};
void freak_sound_bank_init(struct freak_sound_bank *bank);
void freak_sound_bank_destroy(struct freak_sound_bank *bank);
int freak_sound_bank_load(struct freak_sound_bank *bank, const char *filename);
int freak_sound_bank_save(const struct freak_sound_bank *bank, const char *filename);

#endif
