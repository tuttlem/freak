#ifndef __freak_midi_h_
#define __freak_midi_h_

#include <stdint.h>

struct freak_mpu401 { uint16_t base; uint8_t available; };

/* Probe an MPU-401 compatible interface and switch it into UART mode. */
int freak_mpu401_open(struct freak_mpu401 *mpu, uint16_t base);
void freak_mpu401_close(struct freak_mpu401 *mpu);
int freak_mpu401_send(struct freak_mpu401 *mpu, uint8_t value);
int freak_midi_note_on(struct freak_mpu401 *mpu, uint8_t channel, uint8_t note, uint8_t velocity);
int freak_midi_note_off(struct freak_mpu401 *mpu, uint8_t channel, uint8_t note);

#endif
