#ifndef __freak_seq_h_
#define __freak_seq_h_
#include <stdint.h>
struct freak_sequence_event { uint32_t tick; uint8_t type; uint8_t channel; uint16_t value; };
struct freak_sequence_pattern {
    const struct freak_sequence_event *events;
    unsigned int count;
    uint32_t length;
};
typedef void (*freak_sequence_callback)(const struct freak_sequence_event *event, void *user);
struct freak_sequence {
    const struct freak_sequence_event *events;
    unsigned int count, index;
    uint32_t tick;
    uint8_t loop, playing;
    const struct freak_sequence_pattern *patterns;
    const uint8_t *order;
    unsigned int pattern_count, order_count, order_index;
};
void freak_sequence_play(struct freak_sequence *sequence, const struct freak_sequence_event *events, unsigned int count, uint8_t loop);
void freak_sequence_stop(struct freak_sequence *sequence);
/* Pattern order is optional and loops as a complete musical phrase.  Events
   are deliberately device-neutral: callbacks can drive audio, palette,
   camera, or demo-effect parameters from one timeline. */
void freak_sequence_play_patterns(struct freak_sequence *sequence,
                                  const struct freak_sequence_pattern *patterns,
                                  unsigned int pattern_count,
                                  const uint8_t *order, unsigned int order_count,
                                  uint8_t loop);
void freak_sequence_update(struct freak_sequence *sequence, uint32_t elapsed_ticks, freak_sequence_callback callback, void *user);
#endif
