#include <stdio.h>
#include <malloc.h>
#include "tracker.h"

void freak_tracker_asset_init(struct freak_tracker_asset *a)
{ if(a){a->cells=0;a->song.cells=0;a->song.rows=0;a->song.channels=0;a->song.ticks_per_row=0;a->song.loop=0;} }
void freak_tracker_asset_destroy(struct freak_tracker_asset *a)
{ if(!a)return;if(a->cells)free(a->cells);freak_tracker_asset_init(a); }
int freak_tracker_asset_load(struct freak_tracker_asset *a,const char *name)
{
 FILE *f;unsigned char h[9];unsigned long total;
 if(!a||!name)return 0;freak_tracker_asset_destroy(a);f=fopen(name,"rb");if(!f)return 0;
 if(fread(h,1,9,f)!=9||h[0]!='F'||h[1]!='T'||h[2]!='R'||h[3]!='1'){fclose(f);return 0;}
 a->song.rows=(uint16_t)(h[4]|((uint16_t)h[5]<<8));a->song.channels=h[6];a->song.ticks_per_row=h[7];a->song.loop=h[8];
 if(!a->song.rows||!a->song.channels||a->song.channels>FREAK_TRACKER_MAX_CHANNELS||!a->song.ticks_per_row){freak_tracker_asset_init(a);fclose(f);return 0;}
 total=(unsigned long)a->song.rows*a->song.channels*sizeof(*a->cells);a->cells=(struct freak_tracker_cell *)malloc(total);
 if(!a->cells||fread(a->cells,1,total,f)!=total){freak_tracker_asset_destroy(a);fclose(f);return 0;}fclose(f);a->song.cells=a->cells;return 1;
}
void freak_tracker_play(struct freak_tracker *t,const struct freak_tracker_song *s){if(!t)return;t->song=s;t->row=0;t->tick=0;t->playing=s!=0&&s->cells!=0&&s->channels>0;}
void freak_tracker_stop(struct freak_tracker *t){if(t)t->playing=0;}
void freak_tracker_update(struct freak_tracker *t,unsigned long ticks,freak_tracker_callback cb,void *user){uint8_t c;const struct freak_tracker_cell *cell;if(!t||!t->playing)return;while(ticks--){if(++t->tick<t->song->ticks_per_row)continue;t->tick=0;for(c=0;c<t->song->channels;++c){cell=&t->song->cells[t->row*t->song->channels+c];if((cell->note||cell->effect)&&cb)cb(c,cell,user);}if(++t->row>=t->song->rows){if(t->song->loop)t->row=0;else{t->playing=0;break;}}}}
