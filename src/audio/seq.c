#include "seq.h"
static void set_pattern(struct freak_sequence *s,unsigned int order_index)
{
    unsigned int pattern;
    pattern=s->order ? s->order[order_index] : order_index;
    if(pattern>=s->pattern_count){s->playing=0;return;}
    s->order_index=order_index;s->events=s->patterns[pattern].events;s->count=s->patterns[pattern].count;s->index=0;s->tick=0;
}
void freak_sequence_play(struct freak_sequence *s,const struct freak_sequence_event *e,unsigned int c,uint8_t l)
{if(!s)return;s->events=e;s->count=c;s->index=0;s->tick=0;s->loop=l;s->playing=e!=0&&c!=0;s->patterns=0;s->order=0;s->pattern_count=s->order_count=s->order_index=0;}
void freak_sequence_stop(struct freak_sequence *s){if(s)s->playing=0;}
void freak_sequence_play_patterns(struct freak_sequence *s,const struct freak_sequence_pattern *p,unsigned int pc,const uint8_t *o,unsigned int oc,uint8_t l)
{if(!s)return;s->patterns=p;s->order=o;s->pattern_count=pc;s->order_count=oc;s->loop=l;s->playing=p!=0&&pc!=0&&oc!=0;set_pattern(s,0);}
void freak_sequence_update(struct freak_sequence *s,uint32_t elapsed,freak_sequence_callback cb,void *user)
{
 uint32_t length;unsigned int next;
 if(!s||!s->playing)return;s->tick+=elapsed;
 while(s->playing&&s->index<s->count&&s->events[s->index].tick<=s->tick){if(cb)cb(&s->events[s->index],user);++s->index;}
 if(!s->patterns){if(s->index>=s->count){if(s->loop){s->index=0;s->tick=0;}else s->playing=0;}return;}
 length=s->patterns[s->order?s->order[s->order_index]:s->order_index].length;
 if(s->tick<length)return;next=s->order_index+1;
 if(next>=s->order_count){if(!s->loop){s->playing=0;return;}next=0;}set_pattern(s,next);
}
