#include <stdio.h>
#include "democom.h"
#include "../demoasst.h"

static void image_draw(const struct freak_image_asset *asset, int left, int top) {
    int x,y;
    for(y=0;y<asset->image.height&&top+y<200;++y)
        for(x=0;x<asset->image.width&&left+x<320;++x)
            screen[(top+y)*320+left+x]=asset->image.pixels[y*asset->image.width+x];
}
static uint16_t asset_note_hz(uint8_t note) {
    static const uint16_t semitones[] = {262,277,294,311,330,349,370,392,415,440,466,494};
    int octave;
    if (note < 48 || note > 83) return 0;
    octave = (note - 60) / 12;
    if (octave < 0) return (uint16_t)(semitones[note % 12] >> -octave);
    return (uint16_t)(semitones[note % 12] << octave);
}
static void asset_tracker_tone(uint8_t channel,const struct freak_tracker_cell *cell,void *user) {
    uint16_t *tone=(uint16_t *)user; (void)channel;
    if (cell->note) { *tone=asset_note_hz(cell->note); freak_audio_tone(*tone); }
}
static void model_point(const struct freak_model3d_vertex *vertex, unsigned long tick,
                        int *x, int *y) {
    fixed cy=fixed_cos((unsigned int)(tick*2UL)&1023U),sy=fixed_sin((unsigned int)(tick*2UL)&1023U);
    fixed rx=fixed_mul(vertex->position.x,cy)+fixed_mul(vertex->position.z,sy);
    fixed rz=fixed_mul(vertex->position.z,cy)-fixed_mul(vertex->position.x,sy);
    fixed depth=rz+int_to_fixed(5);
    *x=245+fixed_to_int(fixed_div(fixed_mul(rx,int_to_fixed(58)),depth));
    *y=68-fixed_to_int(fixed_div(fixed_mul(vertex->position.y,int_to_fixed(58)),depth));
}
static void model_draw(const struct freak_model3d *model, unsigned long tick) {
    unsigned int i; int ax,ay,bx,by,cx,cy;
    if (!model) return;
    for(i=0;i+2<model->index_count;i+=3) {
        const struct freak_model3d_vertex *a=&model->vertices[model->indices[i]];
        const struct freak_model3d_vertex *b=&model->vertices[model->indices[i+1]];
        const struct freak_model3d_vertex *c=&model->vertices[model->indices[i+2]];
        model_point(a,tick,&ax,&ay); model_point(b,tick,&bx,&by); model_point(c,tick,&cx,&cy);
        freak_draw_line(screen,ax,ay,bx,by,15); freak_draw_line(screen,bx,by,cx,cy,15); freak_draw_line(screen,cx,cy,ax,ay,15);
    }
}
void freak_demo_assets(void) {
    struct freak_image_asset image; struct freak_sprite_sheet_asset sheet;
    struct freak_font_asset loaded_font; struct freak_sound_bank bank;
    struct freak_tracker_asset tracker; struct freak_imf_asset imf;
    struct freak_tilemap map; uint8_t palette[768];
    struct freak_model3d model; struct freak_model3d_vertex vertices[16];
    uint16_t indices[64]; struct freak_sprite sprite; FILE *file;
    unsigned long last_tick,now; uint16_t tone=0; struct freak_tracker player;
    char text[48]; int ok_image,ok_sheet,ok_font,ok_bank,ok_tracker,ok_imf,ok_model,ok_palette,ok_map,playing;
    freak_image_asset_init(&image); freak_sprite_sheet_asset_init(&sheet);
    freak_font_asset_init(&loaded_font); freak_sound_bank_init(&bank);
    freak_tracker_asset_init(&tracker); freak_imf_asset_init(&imf);
    ok_image=freak_image_asset_load(&image,"TEST\\ASSETS\\SHOWCASE.FIM");
    ok_sheet=freak_sprite_sheet_asset_load(&sheet,"TEST\\ASSETS\\SPRITES.FSP");
    ok_font=freak_font_asset_load(&loaded_font,"TEST\\ASSETS\\UI.FFN");
    ok_bank=freak_sound_bank_load(&bank,"TEST\\ASSETS\\PING.FSB");
    ok_tracker=freak_tracker_asset_load(&tracker,"TEST\\ASSETS\\MIDI.FTR");
    ok_imf=freak_imf_asset_load(&imf,"TEST\\ASSETS\\PING.IMF");
    ok_palette=freak_palette_asset_load("TEST\\ASSETS\\SHOWCASE.FPL",palette);
    ok_map=freak_tilemap_load("TEST\\ASSETS\\LEVEL.FMP",&map);
    file=fopen("TEST\\ASSETS\\CUBE.F3M","rb"); ok_model=file&&freak_model3d_read(file,&model,vertices,16,indices,64); if(file)fclose(file);
    if(ok_image)freak_image_asset_apply_palette(&image);
    playing=0; freak_tracker_stop(&player); last_tick=freak_timer_ticks(); freak_input_clear_events();
    while(1) {
        freak_input_update(); if(freak_input_pressed(&freak_input_default_bindings,FREAK_ACTION_BACK))break;
        now=freak_timer_ticks();
        if(ok_tracker&&freak_input_key_pressed(FREAK_KEY_T)) { playing=!playing; if(playing)freak_tracker_play(&player,&tracker.song); else freak_audio_silence(); }
        if(playing)freak_tracker_update(&player,now-last_tick,asset_tracker_tone,&tone);
        last_tick=now;
        freak_clear_buffer(0,screen);
        if(ok_image)image_draw(&image,8,32);
        if(ok_sheet&&freak_sprite_sheet_asset_frame(&sheet,0,&sprite))freak_sprite_blit_key(screen,120,38,&sprite);
        if(ok_sheet&&freak_sprite_sheet_asset_frame(&sheet,1,&sprite))freak_sprite_blit_key(screen,144,38,&sprite);
        freak_font_text(screen,8,8,&font,"Asset loading: files on disk",15);
        freak_font_text(screen,8,18,&font,"FIM image  FSP sprites  FFN font",15);
        if(ok_font)freak_font_text(screen,190,42,&loaded_font.font,"A",15);
        if(ok_model)model_draw(&model,now);
        sprintf(text,"image:%s sprites:%s font:%s palette:%s",ok_image?"OK":"FAIL",ok_sheet?"OK":"FAIL",ok_font?"OK":"FAIL",ok_palette?"OK":"FAIL"); freak_font_text(screen,8,104,&font,text,15);
        sprintf(text,"sound:%u  MIDI tracker:%s  IMF:%lu",bank.count,ok_tracker?"OK":"FAIL",imf.length); freak_font_text(screen,8,116,&font,text,15);
        sprintf(text,"model:%u vertices  map:%ux%u",ok_model?model.vertex_count:0,ok_map?map.width:0,ok_map?map.height:0); freak_font_text(screen,8,128,&font,text,15);
        freak_font_text(screen,8,160,&font,playing?"MIDI tracker playing on PC speaker":"T: play converted MIDI tracker on PC speaker",15);
        freak_font_text(screen,8,180,&font,"Esc: back   Run make assets before make demo",15);
        freak_wait_vsync(); freak_copy_buffer(freak_vga,screen);
    }
    freak_image_asset_destroy(&image); freak_sprite_sheet_asset_destroy(&sheet);
    freak_font_asset_destroy(&loaded_font); freak_sound_bank_destroy(&bank);
    freak_tracker_asset_destroy(&tracker); freak_imf_asset_destroy(&imf);
    freak_audio_silence();
    if(ok_map)freak_tilemap_destroy(&map);
}
