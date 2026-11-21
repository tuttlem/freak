#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "asset.h"
#include "../dos/vga.h"

static uint16_t read_u16(FILE *file)
{ int a=fgetc(file),b=fgetc(file); return (uint16_t)(a<0||b<0?0:a|(b<<8)); }
static int magic(FILE *file,const char *value)
{ char tag[4]; return fread(tag,1,4,file)==4&&!memcmp(tag,value,4); }
void freak_palette_asset_apply(const uint8_t palette[768])
{ int i;if(!palette)return;for(i=0;i<256;++i)freak_set_palette((uint8_t)i,palette[i*3],palette[i*3+1],palette[i*3+2]); }
int freak_palette_asset_load(const char *name,uint8_t palette[768])
{ FILE *f;if(!name||!palette||(f=fopen(name,"rb"))==0)return 0;if(!magic(f,"FPL1")||fread(palette,1,768,f)!=768){fclose(f);return 0;}fclose(f);return 1; }
void freak_image_asset_init(struct freak_image_asset *a)
{ if(a){a->image.pixels=0;a->image.width=a->image.height=0;a->transparent=0;} }
void freak_image_asset_destroy(struct freak_image_asset *a)
{ if(a&&a->image.pixels)free(a->image.pixels);freak_image_asset_init(a); }
int freak_image_asset_load(struct freak_image_asset *a,const char *name)
{ FILE *f;unsigned long bytes;if(!a||!name)return 0;freak_image_asset_destroy(a);f=fopen(name,"rb");if(!f)return 0;if(!magic(f,"FIM1")){fclose(f);return 0;}a->image.width=read_u16(f);a->image.height=read_u16(f);a->transparent=(uint8_t)fgetc(f);fgetc(f);bytes=(unsigned long)a->image.width*a->image.height;if(!bytes||fread(a->palette,1,768,f)!=768||(a->image.pixels=(uint8_t*)malloc(bytes))==0||fread(a->image.pixels,1,bytes,f)!=bytes){fclose(f);freak_image_asset_destroy(a);return 0;}fclose(f);return 1; }
void freak_image_asset_apply_palette(const struct freak_image_asset *a)
{ if(a)freak_palette_asset_apply(a->palette); }
void freak_sprite_sheet_asset_init(struct freak_sprite_sheet_asset *a)
{ if(a){a->pixels=0;a->frame_width=a->frame_height=a->frame_count=0;a->transparent=0;} }
void freak_sprite_sheet_asset_destroy(struct freak_sprite_sheet_asset *a)
{ if(a&&a->pixels)free(a->pixels);freak_sprite_sheet_asset_init(a); }
int freak_sprite_sheet_asset_load(struct freak_sprite_sheet_asset *a,const char *name)
{ FILE *f;unsigned long bytes;if(!a||!name)return 0;freak_sprite_sheet_asset_destroy(a);f=fopen(name,"rb");if(!f)return 0;if(!magic(f,"FSP1")){fclose(f);return 0;}a->frame_width=read_u16(f);a->frame_height=read_u16(f);a->frame_count=read_u16(f);a->transparent=(uint8_t)fgetc(f);fgetc(f);bytes=(unsigned long)a->frame_width*a->frame_height*a->frame_count;if(!bytes||fread(a->palette,1,768,f)!=768||(a->pixels=(uint8_t*)malloc(bytes))==0||fread(a->pixels,1,bytes,f)!=bytes){fclose(f);freak_sprite_sheet_asset_destroy(a);return 0;}fclose(f);return 1; }
int freak_sprite_sheet_asset_frame(const struct freak_sprite_sheet_asset *a,uint16_t n,struct freak_sprite *s)
{ if(!a||!s||!a->pixels||n>=a->frame_count)return 0;s->pixels=a->pixels+(unsigned long)n*a->frame_width*a->frame_height;s->width=a->frame_width;s->height=a->frame_height;s->transparent=a->transparent;return 1; }
void freak_sprite_sheet_asset_apply_palette(const struct freak_sprite_sheet_asset *a)
{ if(a)freak_palette_asset_apply(a->palette); }
