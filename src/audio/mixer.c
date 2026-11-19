#include "mixer.h"

void freak_mixer_init(struct freak_mixer *mixer, uint16_t output_rate)
{
    unsigned int index;
    if (!mixer) return;
    mixer->output_rate = output_rate ? output_rate : 11025;
    mixer->master_volume = 255;
    for (index = 0; index < FREAK_MIXER_MAX_CHANNELS; ++index)
        mixer->channels[index].active = 0;
}

int freak_mixer_play(struct freak_mixer *mixer,
                     const struct freak_audio_sample *sample,
                     uint16_t frequency, uint8_t volume, uint8_t pan,
                     uint8_t loop)
{
    unsigned int index;
    struct freak_mixer_channel *channel;
    if (!mixer || !sample || !sample->data || !sample->length) return -1;
    for (index = 0; index < FREAK_MIXER_MAX_CHANNELS; ++index)
        if (!mixer->channels[index].active) break;
    if (index == FREAK_MIXER_MAX_CHANNELS) index = 0;
    channel = &mixer->channels[index];
    channel->sample = sample;
    channel->position = 0;
    channel->step = ((uint32_t)(frequency ? frequency : sample->sample_rate) << 16) /
                    mixer->output_rate;
    if (!channel->step) channel->step = 1;
    channel->volume = volume;
    channel->pan = pan;
    channel->loop = loop;
    channel->active = 1;
    return (int)index;
}

void freak_mixer_stop(struct freak_mixer *mixer, int channel)
{
    if (mixer && channel >= 0 && channel < FREAK_MIXER_MAX_CHANNELS)
        mixer->channels[channel].active = 0;
}

void freak_mixer_mix(struct freak_mixer *mixer, int8_t *output,
                     unsigned int frames)
{
    unsigned int frame, index;
    int mixed;
    uint32_t position;
    struct freak_mixer_channel *channel;
    const struct freak_audio_sample *sample;
    if (!mixer || !output) return;
    for (frame = 0; frame < frames; ++frame) {
        mixed = 0;
        for (index = 0; index < FREAK_MIXER_MAX_CHANNELS; ++index) {
            channel = &mixer->channels[index];
            if (!channel->active) continue;
            sample = channel->sample;
            position = channel->position >> 16;
            if (position >= sample->length) {
                if ((channel->loop || sample->loop) &&
                    sample->loop_end > sample->loop_start) {
                    channel->position = sample->loop_start << 16;
                    position = sample->loop_start;
                } else { channel->active = 0; continue; }
            }
            mixed += sample->data[position] * channel->volume / 255;
            channel->position += channel->step;
        }
        mixed = mixed * mixer->master_volume / 255;
        if (mixed > 127) mixed = 127;
        if (mixed < -128) mixed = -128;
        output[frame] = (int8_t)mixed;
    }
}

unsigned int freak_mixer_active(const struct freak_mixer *mixer)
{
    unsigned int index, count;
    if (!mixer) return 0;
    count = 0;
    for (index = 0; index < FREAK_MIXER_MAX_CHANNELS; ++index)
        if (mixer->channels[index].active) ++count;
    return count;
}

void freak_mixer_mix_stereo(struct freak_mixer *mixer, int8_t *output,
                            unsigned int frames)
{
    unsigned int frame,index; int left,right,value; uint32_t position;
    struct freak_mixer_channel *channel; const struct freak_audio_sample *sample;
    if(!mixer||!output)return;
    for(frame=0;frame<frames;++frame){
        left=right=0;
        for(index=0;index<FREAK_MIXER_MAX_CHANNELS;++index){
            channel=&mixer->channels[index];if(!channel->active)continue;sample=channel->sample;position=channel->position>>16;
            if(position>=sample->length){if((channel->loop||sample->loop)&&sample->loop_end>sample->loop_start){channel->position=sample->loop_start<<16;position=sample->loop_start;}else{channel->active=0;continue;}}
            value=sample->data[position]*channel->volume/255;
            left+=value*(255-channel->pan)/255;right+=value*channel->pan/255;channel->position+=channel->step;
        }
        left=left*mixer->master_volume/255;right=right*mixer->master_volume/255;
        if(left>127)left=127;if(left<-128)left=-128;if(right>127)right=127;if(right<-128)right=-128;
        output[frame*2]=(int8_t)left;output[frame*2+1]=(int8_t)right;
    }
}

void freak_mixer_mix_unsigned(struct freak_mixer *mixer, uint8_t *output,
                              unsigned int frames)
{
    unsigned int index;
    if(!mixer||!output)return;
    /* The byte representation is identical aside from the signed bias. */
    freak_mixer_mix(mixer,(int8_t *)output,frames);
    for(index=0;index<frames;++index) output[index]=(uint8_t)((int)output[index]+128);
}
