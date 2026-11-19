#include <stdio.h>
#include "democom.h"
static void audio_sequence_event(const struct freak_sequence_event *event, void *user)
{
    uint16_t *current_tone = (uint16_t *)user;
    if (event->type == 1) {
        if (current_tone) *current_tone = event->value;
        freak_audio_tone(event->value);
    }
    if (event->type == 2) {
        if (current_tone) *current_tone = 0;
        freak_audio_silence();
    }
}

static void audio_tracker_event(uint8_t channel,
                                const struct freak_tracker_cell *cell, void *user)
{
    static const uint16_t notes[] = {0, 262, 294, 330, 349, 392, 440, 494, 523};
    uint16_t *current_tone = (uint16_t *)user;
    (void)channel;
    if (cell->note < sizeof(notes) / sizeof(notes[0])) {
        if (current_tone) *current_tone = notes[cell->note];
        freak_audio_tone(notes[cell->note]);
    }
}

static int audio_number_effect(void)
{
    if (freak_input_key_pressed(FREAK_KEY_1)) return 1;
    if (freak_input_key_pressed(FREAK_KEY_2)) return 2;
    if (freak_input_key_pressed(FREAK_KEY_3)) return 3;
    if (freak_input_key_pressed(FREAK_KEY_4)) return 4;
    if (freak_input_key_pressed(FREAK_KEY_5)) return 5;
    if (freak_input_key_pressed(FREAK_KEY_6)) return 6;
    if (freak_input_key_pressed(FREAK_KEY_7)) return 7;
    if (freak_input_key_pressed(FREAK_KEY_8)) return 8;
    return 0;
}

static uint16_t audio_effect_hertz(int effect, unsigned long age, struct freak_rng *rng)
{
    switch (effect) {
    case 1: return (uint16_t)freak_rng_between(rng, 120, 3000); /* white noise */
    case 2: return (uint16_t)(180 - (age > 20 ? 100 : age * 4)); /* low thump */
    case 3: return (uint16_t)(280 + (age > 28 ? 28 : age) * 45); /* rising chirp */
    case 4: return (uint16_t)(1800 - (age > 30 ? 30 : age) * 48); /* falling chirp */
    case 5: return (uint16_t)(age < 12 ? 523 : (age < 24 ? 659 : 784)); /* chime */
    case 6: return (uint16_t)(900 + ((age * 173) % 1300)); /* zap */
    case 7: return 1400; /* tick */
    case 8: return (uint16_t)freak_rng_between(rng, 80, 900); /* soft burst */
    }
    return 0;
}

void freak_demo_pc_speaker(void)
{
    static const struct freak_sequence_event tune[] = {
        {0,1,0,262},{35,1,0,294},{70,1,0,330},{105,1,0,349},
        {140,1,0,392},{175,1,0,440},{210,1,0,494},{245,1,0,523},
        {280,2,0,0}
    };
    static const struct freak_sequence_pattern tune_patterns[] = {{tune, 9, 315}};
    static const uint8_t tune_order[] = {0};
    static const struct freak_tracker_cell tracker_cells[] = {
        {1,0,0,0},{0,0,0,0}, {3,0,0,0},{0,0,0,0},
        {5,0,0,0},{0,0,0,0}, {8,0,0,0},{0,0,0,0},
        {5,0,0,0},{0,0,0,0}, {3,0,0,0},{0,0,0,0},
        {1,0,0,0},{0,0,0,0}, {0,0,0,1},{0,0,0,0}
    };
    static const struct freak_tracker_song tracker_song = {tracker_cells, 8, 2, 2, 1};
    struct freak_sequence sequence;
    struct freak_tracker tracker;
    struct freak_sound_blaster sound_blaster;
    struct freak_mpu401 mpu;
    struct freak_mixer mixer;
    struct freak_audio_sample sample;
    struct freak_audio_sample noise_sample;
    struct freak_audio_sample square_sample;
    int8_t wave[128];
    int8_t noise[128];
    int8_t square[128];
    int8_t mixed[128];
    unsigned long last_tick, now, elapsed, effect_start, effect_end;
    uint16_t current_tone;
    int index, x, y, sb_found, mpu_found, tracker_mode, effect, active_effect;
    struct freak_rng sound_rng;
    char hardware_text[64];

    freak_rng_seed(&sound_rng, 0x53465831UL);
    for (index = 0; index < 128; ++index) {
        wave[index] = (int8_t)((index < 64 ? index : 127 - index) * 4 - 126);
        noise[index] = (int8_t)(freak_rng_range(&sound_rng, 255) - 127);
        square[index] = (int8_t)(index < 64 ? 90 : -90);
    }
    sample.data = wave; sample.length = 128; sample.loop_start = 0; sample.loop_end = 128;
    sample.sample_rate = 8000; sample.loop = 0;
    noise_sample = sample; noise_sample.data = noise;
    square_sample = sample; square_sample.data = square;
    freak_mixer_init(&mixer, 8000);
    freak_sequence_play_patterns(&sequence, tune_patterns, 1, tune_order, 1, 1);
    freak_tracker_stop(&tracker); tracker_mode = 0;
    sb_found = freak_sb_detect(&sound_blaster);
    mpu_found = freak_mpu401_open(&mpu, 0x330);
    sprintf(hardware_text, "Sound Blaster: %s   MPU-401: %s",
            sb_found ? "ready" : "not found", mpu_found ? "ready" : "not found");
    effect_start = effect_end = 0; active_effect = 0; current_tone = 262;
    last_tick = freak_timer_ticks(); freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        now = freak_timer_ticks(); elapsed = now - last_tick; last_tick = now;
        effect = audio_number_effect();
        if (effect) {
            active_effect = effect;
            effect_start = now;
            effect_end = now + (effect == 7 ? 7 : (effect == 8 ? 36 : 30));
            if (effect == 1 || effect == 8)
                freak_mixer_play(&mixer, &noise_sample, 8000, 185, 128, 0);
            else if (effect == 3 || effect == 6)
                freak_mixer_play(&mixer, &square_sample, 8000, 190, 128, 0);
            else
                freak_mixer_play(&mixer, &sample, 8000, 180, 128, 0);
        }
        if (freak_input_key_pressed(FREAK_KEY_T)) {
            tracker_mode = !tracker_mode;
            if (tracker_mode) { freak_sequence_stop(&sequence); freak_tracker_play(&tracker, &tracker_song); }
            else { freak_tracker_stop(&tracker); freak_sequence_play_patterns(&sequence, tune_patterns, 1, tune_order, 1, 1); }
        }
        if (now < effect_end) freak_audio_tone(audio_effect_hertz(active_effect, now - effect_start, &sound_rng));
        else {
            if (effect_end) { freak_audio_tone(current_tone); effect_end = 0; active_effect = 0; }
            if (tracker_mode) freak_tracker_update(&tracker, elapsed, audio_tracker_event, &current_tone);
            else freak_sequence_update(&sequence, (uint32_t)elapsed, audio_sequence_event, &current_tone);
        }
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_ACCEPT))
            freak_mixer_play(&mixer, &sample, 8000, 210, 128, 0);
        freak_mixer_mix(&mixer, mixed, 128);
        freak_clear_buffer(0, screen);
        for (x = 0; x < 128; ++x) {
            y = 115 - mixed[x] / 3;
            freak_draw_line(screen, 40 + x * 2, 115, 40 + x * 2, y, 80);
        }
        freak_fill_rect(screen, 40, 140, 40 + (int)freak_mixer_active(&mixer) * 28,
                        151, 64);
        freak_font_text(screen, 8, 8, &font, "PC Speaker: music + effects", 15);
        freak_font_text(screen, 8, 18, &font, "Slow C major scale  1-8: sound effects  T: tracker", 15);
        freak_font_text(screen, 8, 28, &font, tracker_mode ? "Tracker pattern playing" : "Event sequencer playing", 15);
        freak_font_text(screen, 8, 38, &font, hardware_text, 15);
        freak_font_text(screen, 8, 170, &font, "1 noise  2 thump  3 rise  4 fall  5 chime  6 zap  7 tick  8 burst", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, screen);
    }
    freak_audio_silence();
    if (sb_found) freak_sb_stop(&sound_blaster);
    freak_mpu401_close(&mpu);
}

