#include <conio.h>
#include <i86.h>
#include <stdlib.h>
#include <ctype.h>

#include "sblaster.h"

#define DMA_MASK       0x0a
#define DMA_MODE       0x0b
#define DMA_CLEAR      0x0c

static unsigned long parse_number(const char **text, unsigned int base)
{
    unsigned long value = 0; int digit;
    while (**text == ' ') ++*text;
    for (;;) {
        digit = toupper((unsigned char)**text);
        if (digit >= '0' && digit <= '9') digit -= '0';
        else if (digit >= 'A' && digit <= 'F') digit = digit - 'A' + 10;
        else break;
        if ((unsigned int)digit >= base) break;
        value = value * base + (unsigned int)digit; ++*text;
    }
    return value;
}

static void parse_blaster(struct freak_sound_blaster *card)
{
    const char *text = getenv("BLASTER");
    char key;
    if (!text) return;
    while (*text) {
        while (*text == ' ') ++text;
        key = (char)toupper((unsigned char)*text++);
        if (key == 'A') card->base = (uint16_t)parse_number(&text, 16);
        else if (key == 'I') card->irq = (uint8_t)parse_number(&text, 10);
        else if (key == 'D') card->dma8 = (uint8_t)parse_number(&text, 10);
        else while (*text && *text != ' ') ++text;
    }
}

static int dsp_read(const struct freak_sound_blaster *card)
{
    unsigned int spin;
    for (spin = 0; spin < 65535U; ++spin)
        if (inp(card->base + 0x0e) & 0x80) return inp(card->base + 0x0a);
    return -1;
}

static int dsp_write(const struct freak_sound_blaster *card, uint8_t value)
{
    unsigned int spin;
    for (spin = 0; spin < 65535U; ++spin)
        if (!(inp(card->base + 0x0c) & 0x80)) { outp(card->base + 0x0c, value); return 1; }
    return 0;
}

int freak_sb_detect(struct freak_sound_blaster *card)
{
    int value;
    if (!card) return 0;
    card->base = 0x220; card->irq = 7; card->dma8 = 1;
    card->major = card->minor = card->detected = card->playing = 0;
    parse_blaster(card);
    outp(card->base + 6, 1); outp(card->base + 6, 0);
    value = dsp_read(card);
    if (value != 0xaa) return 0;
    card->detected = 1;
    if (dsp_write(card, 0xe1)) {
        value = dsp_read(card); if (value >= 0) card->major = (uint8_t)value;
        value = dsp_read(card); if (value >= 0) card->minor = (uint8_t)value;
    }
    return 1;
}

void freak_sb_stop(struct freak_sound_blaster *card)
{
    if (!card || !card->detected) return;
    dsp_write(card, 0xda);       /* exit auto-init */
    dsp_write(card, 0xd0);       /* halt DMA */
    dsp_write(card, 0xd3);       /* speaker off */
    card->playing = 0;
}

int freak_sb_start_dma(struct freak_sound_blaster *card, uint32_t address,
                       uint16_t buffer_bytes, uint16_t rate)
{
    uint16_t count, offset; uint8_t channel, page;
    if (!card || !card->detected || !buffer_bytes || !rate || card->dma8 > 3) return 0;
    /* ISA's 8-bit controller cannot cross a 64K physical boundary. */
    if (((address & 0xffffUL) + (uint32_t)buffer_bytes) > 0x10000UL) return 0;
    channel = card->dma8; count = (uint16_t)(buffer_bytes - 1); offset = (uint16_t)address;
    page = (uint8_t)(address >> 16);
    outp(DMA_MASK, 4 + channel); outp(DMA_CLEAR, 0);
    outp((uint16_t)(channel << 1), offset & 0xff); outp((uint16_t)(channel << 1), offset >> 8);
    outp((uint16_t)((channel << 1) + 1), count & 0xff); outp((uint16_t)((channel << 1) + 1), count >> 8);
    outp(channel == 0 ? 0x87 : (channel == 1 ? 0x83 : (channel == 2 ? 0x81 : 0x82)), page);
    outp(DMA_MODE, 0x58 + channel); /* single-cycle, auto-init, memory -> device */
    outp(DMA_MASK, channel);
    if (!dsp_write(card, 0xd1) || !dsp_write(card, 0x40) ||
        !dsp_write(card, (uint8_t)(256UL - 1000000UL / rate)) ||
        !dsp_write(card, 0x48) || !dsp_write(card, count & 0xff) ||
        !dsp_write(card, count >> 8) || !dsp_write(card, 0x1c)) return 0;
    card->playing = 1; return 1;
}

void freak_sb_acknowledge_irq(const struct freak_sound_blaster *card)
{
    if (card && card->detected) inp(card->base + 0x0e);
}

void freak_sb_stream_init(struct freak_sb_dma_stream *stream,uint8_t *buffer,
                          uint16_t bytes,freak_sb_refill_callback refill,void *user)
{
    if(!stream)return;stream->buffer=buffer;stream->bytes=bytes;
    stream->refill=refill;stream->user=user;
}
void freak_sb_stream_irq(struct freak_sound_blaster *card,
                         struct freak_sb_dma_stream *stream)
{
    if(stream&&stream->refill&&stream->buffer&&stream->bytes)
        stream->refill(stream->buffer,stream->bytes,stream->user);
    freak_sb_acknowledge_irq(card);
}

int freak_sb_dma_buffer_alloc(struct freak_sb_dma_buffer *buffer,uint16_t bytes)
{
    union REGS input,output;
    uint32_t base,aligned;
    unsigned int paragraphs;
    if(!buffer||!bytes)return 0;
    buffer->data=0;buffer->physical_address=0;buffer->bytes=0;buffer->segment=buffer->selector=0;
    /* DPMI 0100 returns AX=real-mode segment and DX=protected selector.
       Keep both: the former is what ISA DMA sees; the latter addresses it. */
    paragraphs=(unsigned int)(((uint32_t)bytes+65535UL+15UL)/16UL);
    input.w.ax=0x0100;input.w.bx=paragraphs;
    int386(0x31,&input,&output);
    if(output.x.cflag)return 0;
    buffer->segment=output.w.ax;buffer->selector=output.w.dx;
    base=(uint32_t)buffer->segment<<4;
    aligned=(base+0xffffUL)&0xffff0000UL;
    buffer->data=(uint8_t __far *)MK_FP(buffer->selector,(unsigned int)(aligned-base));
    buffer->physical_address=aligned;buffer->bytes=bytes;
    return 1;
}
void freak_sb_dma_buffer_free(struct freak_sb_dma_buffer *buffer)
{
    union REGS input,output;
    if(!buffer)return;
    if(buffer->selector){input.w.ax=0x0101;input.w.dx=buffer->selector;int386(0x31,&input,&output);}
    buffer->data=0;buffer->physical_address=0;buffer->bytes=0;buffer->segment=buffer->selector=0;
}
