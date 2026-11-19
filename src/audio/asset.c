#include <stdio.h>
#include <malloc.h>

#include "asset.h"

static uint16_t read_u16(FILE *file) { int a=fgetc(file), b=fgetc(file); return (uint16_t)(a<0||b<0 ? 0 : a|(b<<8)); }
static uint32_t read_u32(FILE *file) { uint32_t a=read_u16(file), b=read_u16(file); return a|(b<<16); }
static int write_u16(FILE *file,uint16_t value) { return fputc(value&255,file)!=EOF&&fputc(value>>8,file)!=EOF; }
static int write_u32(FILE *file,uint32_t value) { return write_u16(file,(uint16_t)value)&&write_u16(file,(uint16_t)(value>>16)); }

void freak_audio_sample_init(struct freak_audio_sample *sample)
{
    if (!sample) return;
    sample->data=0; sample->length=0; sample->loop_start=sample->loop_end=0;
    sample->sample_rate=0; sample->loop=0;
}

void freak_audio_sample_destroy(struct freak_audio_sample *sample)
{
    if (!sample) return;
    if(sample->data) free(sample->data); freak_audio_sample_init(sample);
}
int freak_audio_sample_resample(struct freak_audio_sample *sample,uint16_t output_rate)
{
    int8_t *data; uint32_t length,index,source;
    if(!sample||!sample->data||!sample->sample_rate||!output_rate) return 0;
    if(sample->sample_rate==output_rate) return 1;
    length=((uint32_t)sample->length*output_rate+sample->sample_rate-1)/sample->sample_rate;
    if(!length||(data=(int8_t *)malloc(length))==0) return 0;
    for(index=0;index<length;++index){source=(uint32_t)index*sample->sample_rate/output_rate;if(source>=sample->length)source=sample->length-1;data[index]=sample->data[source];}
    free(sample->data); sample->data=data; sample->length=length;
    if(sample->loop){sample->loop_start=(uint32_t)sample->loop_start*output_rate/sample->sample_rate;sample->loop_end=(uint32_t)sample->loop_end*output_rate/sample->sample_rate;if(sample->loop_end>length)sample->loop_end=length;}
    sample->sample_rate=output_rate; return 1;
}
void freak_audio_sample_normalise(struct freak_audio_sample *sample,uint8_t peak)
{
    uint32_t index; int value,maximum=0;
    if(!sample||!sample->data||!peak)return;
    for(index=0;index<sample->length;++index){value=sample->data[index];if(value<0)value=-value;if(value>maximum)maximum=value;}
    if(!maximum)return;
    for(index=0;index<sample->length;++index){value=(int)sample->data[index]*peak/maximum;if(value>127)value=127;if(value<-128)value=-128;sample->data[index]=(int8_t)value;}
}
int freak_audio_sample_save_raw(const struct freak_audio_sample *sample,const char *filename,uint8_t unsigned_pcm)
{
    FILE *file;uint32_t index;int value;
    if(!sample||!sample->data||!filename)return 0;file=fopen(filename,"wb");if(!file)return 0;
    for(index=0;index<sample->length;++index){value=sample->data[index];if(unsigned_pcm)value+=128;if(fputc(value,file)==EOF){fclose(file);return 0;}}
    fclose(file);return 1;
}
void freak_sound_bank_init(struct freak_sound_bank *bank){if(bank){bank->entries=0;bank->count=0;}}
void freak_sound_bank_destroy(struct freak_sound_bank *bank){uint16_t i;if(!bank)return;for(i=0;i<bank->count;++i)freak_audio_sample_destroy(&bank->entries[i].sample);if(bank->entries)free(bank->entries);freak_sound_bank_init(bank);}
int freak_sound_bank_load(struct freak_sound_bank *bank,const char *filename)
{
    FILE *file;char magic[4];uint16_t count,i;uint32_t bytes,j;int value;
    if(!bank||!filename)return 0;freak_sound_bank_destroy(bank);file=fopen(filename,"rb");if(!file)return 0;
    if(fread(magic,1,4,file)!=4||magic[0]!='F'||magic[1]!='S'||magic[2]!='B'||magic[3]!='K'){fclose(file);return 0;}count=read_u16(file);
    bank->entries=(struct freak_sound_bank_entry *)malloc((unsigned long)count*sizeof(*bank->entries));if(!bank->entries){fclose(file);return 0;}bank->count=count;
    for(i=0;i<count;++i){if(fread(bank->entries[i].name,1,13,file)!=13){freak_sound_bank_destroy(bank);fclose(file);return 0;}freak_audio_sample_init(&bank->entries[i].sample);bank->entries[i].sample.sample_rate=read_u16(file);bytes=read_u32(file);bank->entries[i].sample.loop_start=read_u32(file);bank->entries[i].sample.loop_end=read_u32(file);value=fgetc(file);bank->entries[i].sample.loop=(uint8_t)(value>0);if(!bytes||(bank->entries[i].sample.data=(int8_t *)malloc(bytes))==0){freak_sound_bank_destroy(bank);fclose(file);return 0;}for(j=0;j<bytes;++j){value=fgetc(file);if(value==EOF){freak_sound_bank_destroy(bank);fclose(file);return 0;}bank->entries[i].sample.data[j]=(int8_t)value;}bank->entries[i].sample.length=bytes;}
    fclose(file);return 1;
}
int freak_sound_bank_save(const struct freak_sound_bank *bank,const char *filename)
{
 FILE *file;uint16_t i;uint32_t j;const struct freak_audio_sample *s;
 if(!bank||!filename)return 0;file=fopen(filename,"wb");if(!file)return 0;
 if(fwrite("FSBK",1,4,file)!=4||!write_u16(file,bank->count)){fclose(file);return 0;}
 for(i=0;i<bank->count;++i){s=&bank->entries[i].sample;if(!s->data||!s->length||fwrite(bank->entries[i].name,1,13,file)!=13||!write_u16(file,s->sample_rate)||!write_u32(file,s->length)||!write_u32(file,s->loop_start)||!write_u32(file,s->loop_end)||fputc(s->loop,file)==EOF){fclose(file);return 0;}for(j=0;j<s->length;++j)if(fputc(s->data[j],file)==EOF){fclose(file);return 0;}}
 fclose(file);return 1;
}
void freak_audio_sample_set_loop(struct freak_audio_sample *sample, uint32_t start, uint32_t end)
{
    if (!sample || start >= end || end > sample->length) return;
    sample->loop_start=start; sample->loop_end=end; sample->loop=1;
}
int freak_audio_sample_load_raw(struct freak_audio_sample *sample, const char *filename,
                                uint16_t sample_rate, uint8_t unsigned_pcm)
{
    FILE *file; long length; uint32_t index; int value;
    if (!sample || !filename) return 0;
    freak_audio_sample_destroy(sample); file=fopen(filename,"rb"); if (!file) return 0;
    fseek(file,0,SEEK_END); length=ftell(file); fseek(file,0,SEEK_SET);
    if (length<1) { fclose(file); return 0; }
    sample->data=(int8_t *)malloc((unsigned long)length); if (!sample->data) { fclose(file); return 0; }
    for(index=0;index<(uint32_t)length;++index) { value=fgetc(file); sample->data[index]=(int8_t)(unsigned_pcm ? value-128 : value); }
    fclose(file); sample->length=(uint32_t)length; sample->sample_rate=sample_rate; sample->loop=0; return 1;
}
int freak_audio_sample_load_wav(struct freak_audio_sample *sample, const char *filename)
{
    FILE *file; char tag[4]; uint32_t chunk_size,data_size=0,frames,index,rate=0; uint16_t format=0,channels=0,bits=0; int value,other;
    if (!sample || !filename) return 0;
    freak_audio_sample_destroy(sample); file=fopen(filename,"rb"); if(!file) return 0;
    if(fread(tag,1,4,file)!=4||tag[0]!='R'||tag[1]!='I'||tag[2]!='F'||tag[3]!='F'){fclose(file);return 0;}
    read_u32(file); if(fread(tag,1,4,file)!=4||tag[0]!='W'||tag[1]!='A'||tag[2]!='V'||tag[3]!='E'){fclose(file);return 0;}
    while(!data_size&&fread(tag,1,4,file)==4){chunk_size=read_u32(file);if(tag[0]=='f'&&tag[1]=='m'&&tag[2]=='t'&&tag[3]==' '){format=read_u16(file);channels=read_u16(file);rate=read_u32(file);read_u32(file);read_u16(file);bits=read_u16(file);if(chunk_size>16)fseek(file,(long)(chunk_size-16),SEEK_CUR);}else if(tag[0]=='d'&&tag[1]=='a'&&tag[2]=='t'&&tag[3]=='a')data_size=chunk_size;else fseek(file,(long)chunk_size,SEEK_CUR);if(chunk_size&1)fseek(file,1,SEEK_CUR);}
    if(format!=1||(channels!=1&&channels!=2)||(bits!=8&&bits!=16)||!data_size){fclose(file);return 0;}
    frames=data_size/(channels*(bits/8)); sample->data=(int8_t *)malloc(frames);if(!sample->data){fclose(file);return 0;}
    for(index=0;index<frames;++index){if(bits==8){value=fgetc(file)-128;if(channels==2){other=fgetc(file)-128;value=(value+other)/2;}}else{value=(int16_t)read_u16(file)>>8;if(channels==2){other=(int16_t)read_u16(file)>>8;value=(value+other)/2;}}sample->data[index]=(int8_t)value;}
    fclose(file);sample->length=frames;sample->sample_rate=(uint16_t)rate;sample->loop=0;return 1;
}
