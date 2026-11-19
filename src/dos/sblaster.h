#ifndef __freak_sblaster_h_
#define __freak_sblaster_h_

#include <stdint.h>

/* Hardware endpoint for the mixer.  DMA memory must be conventional memory
   (and must not cross a 64K DMA boundary); its physical address is supplied
   by the DOS extender/application that owns the buffer. */
struct freak_sound_blaster {
    uint16_t base;
    uint8_t irq;
    uint8_t dma8;
    uint8_t major, minor;
    uint8_t detected;
    uint8_t playing;
};
typedef void (*freak_sb_refill_callback)(uint8_t *buffer, uint16_t bytes,
                                         void *user);
struct freak_sb_dma_stream {
    uint8_t *buffer;
    uint16_t bytes;
    freak_sb_refill_callback refill;
    void *user;
};
struct freak_sb_dma_buffer {
    uint8_t __far *data;
    uint32_t physical_address;
    uint16_t bytes;
    unsigned int segment;
    unsigned int selector;
};

/* Reads BLASTER=Axxx Ixx Dxx when present, then performs the DSP reset/ACK
   handshake.  Defaults to the traditional 220h/IRQ7/DMA1 configuration. */
int freak_sb_detect(struct freak_sound_blaster *card);
void freak_sb_stop(struct freak_sound_blaster *card);
/* Start an 8-bit unsigned PCM auto-init DMA stream.  buffer_bytes is a
   complete ring buffer size (1..65536) and physical_address is its DOS
   physical address. */
int freak_sb_start_dma(struct freak_sound_blaster *card, uint32_t physical_address,
                       uint16_t buffer_bytes, uint16_t rate);
/* Acknowledge the configured SB IRQ after an application ISR has mixed the
   next portion of its ring buffer. */
void freak_sb_acknowledge_irq(const struct freak_sound_blaster *card);
/* Call from the application-installed Sound Blaster IRQ wrapper.  It refills
   one completed auto-init block, then acknowledges the DSP interrupt. */
void freak_sb_stream_init(struct freak_sb_dma_stream *stream, uint8_t *buffer,
                          uint16_t bytes, freak_sb_refill_callback refill,
                          void *user);
void freak_sb_stream_irq(struct freak_sound_blaster *card,
                         struct freak_sb_dma_stream *stream);
/* Allocate/free a conventional-memory buffer suitable for an 8-bit DMA
   transfer. Its backing DOS segment and physical address stay together. */
int freak_sb_dma_buffer_alloc(struct freak_sb_dma_buffer *buffer,
                              uint16_t bytes);
void freak_sb_dma_buffer_free(struct freak_sb_dma_buffer *buffer);

#endif
