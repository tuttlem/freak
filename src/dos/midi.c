#include <conio.h>

#include "midi.h"

static int wait_write(const struct freak_mpu401 *mpu)
{ unsigned int spin; for(spin=0;spin<65535U;++spin) if(!(inp(mpu->base+1)&0x40)) return 1; return 0; }
static int wait_read(const struct freak_mpu401 *mpu)
{ unsigned int spin; for(spin=0;spin<65535U;++spin) if(!(inp(mpu->base+1)&0x80)) return 1; return 0; }
int freak_mpu401_open(struct freak_mpu401 *mpu, uint16_t base)
{
    if(!mpu) return 0; mpu->base=base ? base : 0x330; mpu->available=0;
    if(!wait_write(mpu)) return 0; outp(mpu->base+1,0xff);
    if(!wait_read(mpu) || inp(mpu->base)!=0xfe) return 0;
    if(!wait_write(mpu)) return 0; outp(mpu->base+1,0x3f); mpu->available=1; return 1;
}
void freak_mpu401_close(struct freak_mpu401 *mpu) { if(mpu) mpu->available=0; }
int freak_mpu401_send(struct freak_mpu401 *mpu,uint8_t value)
{ if(!mpu||!mpu->available||!wait_write(mpu))return 0;outp(mpu->base,value);return 1; }
int freak_midi_note_on(struct freak_mpu401 *mpu,uint8_t c,uint8_t n,uint8_t v)
{ return freak_mpu401_send(mpu,(uint8_t)(0x90|(c&15)))&&freak_mpu401_send(mpu,n)&&freak_mpu401_send(mpu,v); }
int freak_midi_note_off(struct freak_mpu401 *mpu,uint8_t c,uint8_t n)
{ return freak_mpu401_send(mpu,(uint8_t)(0x80|(c&15)))&&freak_mpu401_send(mpu,n)&&freak_mpu401_send(mpu,0); }
