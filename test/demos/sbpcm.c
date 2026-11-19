#include <stdio.h>

#include "../democtx.h"

static void fill_scale(struct freak_sb_dma_buffer *buffer)
{
    static const uint16_t notes[] = {262,294,330,349,392,440,494,523};
    uint32_t phase = 0;
    unsigned int index, note, note_bytes, local;
    int value, amplitude;

    if (!buffer || !buffer->data) return;
    note_bytes = buffer->bytes / 8;
    for (index = 0; index < buffer->bytes; ++index) {
        note = index / note_bytes;
        local = index % note_bytes;
        if (local == 0) phase = 0;
        phase += notes[note];
        if (phase >= 8000U) phase -= 8000U;
        value = phase < 4000U ? (int)(phase * 254U / 4000U) - 127 :
                                127 - (int)((phase - 4000U) * 254U / 4000U);
        amplitude = 42;
        if (local < 96U) amplitude = (int)local * 42 / 96;
        else if (local > note_bytes - 96U)
            amplitude = (int)(note_bytes - local) * 42 / 96;
        buffer->data[index] = (uint8_t)(128 + value * amplitude / 127);
    }
}

void freak_demo_sound_blaster(const struct freak_demo_context *context)
{
    struct freak_sound_blaster card;
    struct freak_sb_dma_buffer buffer;
    int detected, playing;
    char status[64];

    buffer.data = 0; buffer.physical_address = 0; buffer.bytes = 0;
    buffer.segment = buffer.selector = 0;
    detected = freak_sb_detect(&card); playing = 0;
    if (!detected)
        sprintf(status, "No Sound Blaster DSP found (BLASTER / port %03X)", card.base);
    else if (!freak_sb_dma_buffer_alloc(&buffer, 16384))
        sprintf(status, "Sound Blaster found; conventional DMA buffer unavailable");
    else {
        fill_scale(&buffer);
        playing = freak_sb_start_dma(&card, buffer.physical_address, buffer.bytes, 8000);
        sprintf(status, playing ? "Playing 8-bit PCM through Sound Blaster" :
                                  "Sound Blaster found; DMA start failed");
    }
    freak_input_clear_events();
    while (1) {
        freak_input_update();
        if (freak_input_pressed(&freak_input_default_bindings, FREAK_ACTION_BACK)) break;
        freak_clear_buffer(0, context->screen);
        freak_font_text(context->screen, 8, 8, context->font, "Sound Blaster: PCM DMA", 15);
        freak_font_text(context->screen, 8, 20, context->font, status, playing ? 10 : 12);
        freak_font_text(context->screen, 8, 40, context->font, "Looping 2-second C major PCM scale at 8 kHz", 15);
        freak_font_text(context->screen, 8, 52, context->font, "This demo does not use the PC speaker.", 15);
        freak_font_text(context->screen, 8, 180, context->font, "Esc: back", 15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga, context->screen);
    }
    if (playing) freak_sb_stop(&card);
    freak_sb_dma_buffer_free(&buffer);
}
