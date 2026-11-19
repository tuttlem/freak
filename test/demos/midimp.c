#include <stdio.h>

#include "../democtx.h"

void freak_demo_midi(const struct freak_demo_context *context)
{
    static const uint8_t notes[] = {60,62,64,65,67,69,71,72};
    struct freak_mpu401 mpu;
    unsigned long last_note, now;
    unsigned int note = 0;
    int available;
    char status[64];

    available = freak_mpu401_open(&mpu, 0x330);
    sprintf(status, available ? "MPU-401 UART responds; sending C major scale" :
                                "No MPU-401 MIDI interface found at 330h");
    if (available) freak_midi_note_on(&mpu, 0, notes[note], 52);
    last_note = freak_timer_ticks(); freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        now = freak_timer_ticks();
        if (available && now - last_note >= 35) {
            freak_midi_note_off(&mpu, 0, notes[note]);
            note = (note + 1) % (sizeof(notes) / sizeof(notes[0]));
            freak_midi_note_on(&mpu, 0, notes[note], 52);
            last_note = now;
        }
        freak_clear_buffer(0, context->screen);
        freak_font_text(context->screen, 8, 8, context->font, "MIDI: MPU-401 UART", 15);
        freak_font_text(context->screen, 8, 20, context->font, status, available ? 10 : 12);
        freak_font_text(context->screen, 8, 40, context->font, "Channel 1, gentle C major scale", 15);
        freak_font_text(context->screen, 8, 52, context->font, "A host MIDI synth/device is required for audible sound.", 15);
        freak_font_text(context->screen, 8, 180, context->font, "Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, context->screen);
    }
    if (available) freak_midi_note_off(&mpu, 0, notes[note]);
    freak_mpu401_close(&mpu);
}
