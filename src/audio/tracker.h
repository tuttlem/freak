#ifndef __freak_tracker_h_
#define __freak_tracker_h_
#include <stdint.h>
#define FREAK_TRACKER_MAX_CHANNELS 8
struct freak_tracker_cell { uint8_t note, instrument, volume, effect; };
struct freak_tracker_song { const struct freak_tracker_cell *cells; uint16_t rows; uint8_t channels, ticks_per_row, loop; };
typedef void (*freak_tracker_callback)(uint8_t channel,const struct freak_tracker_cell *cell,void *user);
struct freak_tracker { const struct freak_tracker_song *song; uint16_t row; uint8_t tick, playing; };
/* FTR1 is Freak's compact tracker asset: a 9-byte header (FTR1, rows,
   channels, ticks-per-row, loop) followed by row-major four-byte cells.
   It is a deliberately conversion-friendly MOD/S3M/XM-style target format. */
struct freak_tracker_asset {
    struct freak_tracker_song song;
    struct freak_tracker_cell *cells;
};
void freak_tracker_asset_init(struct freak_tracker_asset *asset);
void freak_tracker_asset_destroy(struct freak_tracker_asset *asset);
int freak_tracker_asset_load(struct freak_tracker_asset *asset,const char *filename);
void freak_tracker_play(struct freak_tracker *tracker,const struct freak_tracker_song *song);
void freak_tracker_stop(struct freak_tracker *tracker);
void freak_tracker_update(struct freak_tracker *tracker,unsigned long ticks,freak_tracker_callback callback,void *user);
#endif
