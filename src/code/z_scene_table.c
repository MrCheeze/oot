#include "global.h"
#include "quake.h"
#include "versions.h"
#include "z64frame_advance.h"
#if PLATFORM_N64
#include "n64dd.h"
#endif

#include "assets/scenes/indoors/miharigoya/miharigoya_scene.h"
#include "assets/scenes/indoors/souko/souko_scene.h"

#include "assets/scenes/overworld/spot00/spot00_scene.h"
#include "assets/scenes/overworld/spot00/spot00_room_0.h"
#include "assets/scenes/overworld/spot01/spot01_scene.h"
#include "assets/scenes/overworld/spot07/spot07_scene.h"
#include "assets/scenes/overworld/spot12/spot12_scene.h"
#include "assets/scenes/overworld/spot16/spot16_scene.h"
#include "assets/scenes/overworld/spot16/spot16_room_0.h"
#include "assets/scenes/overworld/spot18/spot18_scene.h"
#include "assets/scenes/overworld/spot20/spot20_scene.h"

#include "assets/scenes/dungeons/Bmori1/Bmori1_scene.h"
#include "assets/scenes/dungeons/MIZUsin/MIZUsin_scene.h"
#include "assets/scenes/dungeons/ddan/ddan_scene.h"
#include "assets/scenes/dungeons/gerudoway/gerudoway_scene.h"
#include "assets/scenes/dungeons/ice_doukutu/ice_doukutu_scene.h"
#include "assets/scenes/dungeons/jyasinzou/jyasinzou_scene.h"
#include "assets/scenes/dungeons/men/men_scene.h"
#include "assets/scenes/dungeons/ydan/ydan_scene.h"

#include "overlays/actors/ovl_Bg_Dodoago/z_bg_dodoago.h"

void scene_proc_non(PlayState* play);
void scene_proc_spot00(PlayState* play);
void scene_proc_spot01(PlayState* play);
void scene_proc_spot03(PlayState* play);
void scene_proc_spot04(PlayState* play);
void scene_proc_spot06(PlayState* play);
void scene_proc_spot07(PlayState* play);
void scene_proc_spot08(PlayState* play);
void scene_proc_spot09(PlayState* play);
void scene_proc_spot10(PlayState* play);
void scene_proc_spot11(PlayState* play);
void scene_proc_spot12(PlayState* play);
void scene_proc_spot13(PlayState* play);
void scene_proc_spot15(PlayState* play);
void scene_proc_spot16(PlayState* play);
void scene_proc_spot17(PlayState* play);
void scene_proc_spot18(PlayState* play);
void scene_proc_spot20(PlayState* play);
void scene_proc_fire(PlayState* play);
void scene_proc_ydan(PlayState* play);
void scene_proc_ddan(PlayState* play);
void scene_proc_bdan(PlayState* play);
void scene_proc_morisin(PlayState* play);
void scene_proc_mizusin(PlayState* play);
void scene_proc_hakadan(PlayState* play);
void scene_proc_jyasinzou(PlayState* play);
void scene_proc_ganontika(PlayState* play);
void scene_proc_men(PlayState* play);
void scene_proc_ydan_boss(PlayState* play);
void scene_proc_mizusin_boss(PlayState* play);
void scene_proc_tokinoma(PlayState* play);
void scene_proc_kakusiana(PlayState* play);
void scene_proc_kenjya(PlayState* play);
void scene_proc_yizumi(PlayState* play);
void scene_proc_syatekijyou(PlayState* play);
void scene_proc_hairaruniwa(PlayState* play);
void scene_proc_ganontou(PlayState* play);
void scene_proc_ice_doukutu(PlayState* play);
void scene_proc_ganon_final(PlayState* play);
void scene_proc_hakaana2(PlayState* play);
void scene_proc_gerudoway(PlayState* play);
void scene_proc_bowling(PlayState* play);
void scene_proc_hakaana_ouke(PlayState* play);
void scene_proc_hylia_labo(PlayState* play);
void scene_proc_souko(PlayState* play);
void scene_proc_miharigoya(PlayState* play);
void scene_proc_mahouya(PlayState* play);
void scene_proc_wtscr1_only(PlayState* play);
void scene_proc_tatehikari(PlayState* play);
void scene_proc_besitu(PlayState* play);
void scene_proc_turibori(PlayState* play);
void scene_proc_ganon_sonogo(PlayState* play);
void scene_proc_ganontikasonogo(PlayState* play);

// Entrance Table definition
#define DEFINE_ENTRANCE(_0, sceneId, spawn, continueBgm, displayTitleCard, endTransType, startTransType) \
    { sceneId, spawn,                                                                                    \
      (((continueBgm) ? ENTRANCE_INFO_CONTINUE_BGM_FLAG : 0) |                                           \
       ((displayTitleCard) ? ENTRANCE_INFO_DISPLAY_TITLE_CARD_FLAG : 0) |                                \
       (((endTransType) << ENTRANCE_INFO_END_TRANS_TYPE_SHIFT) & ENTRANCE_INFO_END_TRANS_TYPE_MASK) |    \
       (((startTransType) << ENTRANCE_INFO_START_TRANS_TYPE_SHIFT) & ENTRANCE_INFO_START_TRANS_TYPE_MASK)) },

EntranceInfo scene_status_data[] = {
#include "tables/entrance_table.h"
};

#undef DEFINE_ENTRANCE

// Linker symbol declarations (used in the table below)
#define DEFINE_SCENE(name, title, _2, _3, _4, _5) \
    DECLARE_ROM_SEGMENT(name)                     \
    DECLARE_ROM_SEGMENT(title)

#include "tables/scene_table.h"

#undef DEFINE_SCENE

// Scene Table definition
#define DEFINE_SCENE(name, title, _2, drawConfig, unk_10, unk_12) \
    { ROM_FILE(name), ROM_FILE(title), unk_10, drawConfig, unk_12, 0 },

// Handle `none` as a special case for scenes without a title card
#define _noneSegmentRomStart NULL
#define _noneSegmentRomEnd NULL

SceneTableEntry scene_data_status[] = {
#include "tables/scene_table.h"
};

#undef _noneSegmentRomStart
#undef _noneSegmentRomEnd

#undef DEFINE_SCENE

Gfx dl_anime_default[] = {
    gsSPSegment(0x08, dl_noop),
    gsSPSegment(0x09, dl_noop),
    gsSPSegment(0x0A, dl_noop),
    gsSPSegment(0x0B, dl_noop),
    gsSPSegment(0x0C, dl_noop),
    gsSPSegment(0x0D, dl_noop),
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0, 128, 128, 128, 128),
    gsDPSetEnvColor(128, 128, 128, 128),
    gsSPEndDisplayList(),
};

#if PLATFORM_N64 // call_scene_proc is at end of file in GC/iQue versions

SceneDrawConfigFunc scene_proc[SDC_MAX] = {
    scene_proc_non,                     // SDC_DEFAULT
    scene_proc_spot00,                 // SDC_HYRULE_FIELD
    scene_proc_spot01,             // SDC_KAKARIKO_VILLAGE
    scene_proc_spot03,                  // SDC_ZORAS_RIVER
    scene_proc_spot04,                // SDC_KOKIRI_FOREST
    scene_proc_spot06,                   // SDC_LAKE_HYLIA
    scene_proc_spot07,                 // SDC_ZORAS_DOMAIN
    scene_proc_spot08,               // SDC_ZORAS_FOUNTAIN
    scene_proc_spot09,                // SDC_GERUDO_VALLEY
    scene_proc_spot10,                   // SDC_LOST_WOODS
    scene_proc_spot11,              // SDC_DESERT_COLOSSUS
    scene_proc_spot12,             // SDC_GERUDOS_FORTRESS
    scene_proc_spot13,            // SDC_HAUNTED_WASTELAND
    scene_proc_spot15,                // SDC_HYRULE_CASTLE
    scene_proc_spot16,          // SDC_DEATH_MOUNTAIN_TRAIL
    scene_proc_spot17,         // SDC_DEATH_MOUNTAIN_CRATER
    scene_proc_spot18,                   // SDC_GORON_CITY
    scene_proc_spot20,                 // SDC_LON_LON_RANCH
    scene_proc_fire,                  // SDC_FIRE_TEMPLE
    scene_proc_ydan,                    // SDC_DEKU_TREE
    scene_proc_ddan,              // SDC_DODONGOS_CAVERN
    scene_proc_bdan,                    // SDC_JABU_JABU
    scene_proc_morisin,                // SDC_FOREST_TEMPLE
    scene_proc_mizusin,                 // SDC_WATER_TEMPLE
    scene_proc_hakadan,         // SDC_SHADOW_TEMPLE_AND_WELL
    scene_proc_jyasinzou,                // SDC_SPIRIT_TEMPLE
    scene_proc_ganontika,          // SDC_INSIDE_GANONS_CASTLE
    scene_proc_men,        // SDC_GERUDO_TRAINING_GROUND
    scene_proc_ydan_boss,                // SDC_DEKU_TREE_BOSS
    scene_proc_mizusin_boss,             // SDC_WATER_TEMPLE_BOSS
    scene_proc_tokinoma,                // SDC_TEMPLE_OF_TIME
    scene_proc_kakusiana,                     // SDC_GROTTOS
    scene_proc_kenjya,           // SDC_CHAMBER_OF_THE_SAGES
    scene_proc_yizumi,          // SDC_GREAT_FAIRYS_FOUNTAIN
    scene_proc_syatekijyou,             // SDC_SHOOTING_GALLERY
    scene_proc_hairaruniwa,       // SDC_CASTLE_COURTYARD_GUARDS
    scene_proc_ganontou,         // SDC_OUTSIDE_GANONS_CASTLE
    scene_proc_ice_doukutu,                   // SDC_ICE_CAVERN
    scene_proc_ganon_final, // SDC_GANONS_TOWER_COLLAPSE_EXTERIOR
    scene_proc_hakaana2,              // SDC_FAIRYS_FOUNTAIN
    scene_proc_gerudoway,              // SDC_THIEVES_HIDEOUT
    scene_proc_bowling,         // SDC_BOMBCHU_BOWLING_ALLEY
    scene_proc_hakaana_ouke,            // SDC_ROYAL_FAMILYS_TOMB
    scene_proc_hylia_labo,          // SDC_LAKESIDE_LABORATORY
    scene_proc_souko,             // SDC_LON_LON_BUILDINGS
    scene_proc_miharigoya,            // SDC_MARKET_GUARD_HOUSE
    scene_proc_mahouya,            // SDC_POTION_SHOP_GRANNY
    scene_proc_wtscr1_only,                   // SDC_CALM_WATER
    scene_proc_tatehikari,       // SDC_GRAVE_EXIT_LIGHT_SHINING
    scene_proc_besitu,                      // SDC_BESITU
    scene_proc_turibori,                 // SDC_FISHING_POND
    scene_proc_ganon_sonogo, // SDC_GANONS_TOWER_COLLAPSE_INTERIOR
    scene_proc_ganontikasonogo,  // SDC_INSIDE_GANONS_CASTLE_COLLAPSE
};

void call_scene_proc(PlayState* play) {
    if ((B_80121220 != NULL) && (B_80121220->unk_6C != NULL)) {
        B_80121220->unk_6C(play, scene_proc);
        return;
    }

    scene_proc[play->sceneDrawConfig](play);
}

#endif

void scene_proc_non(PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 4725);

    gSPDisplayList(POLY_OPA_DISP++, dl_anime_default);
    gSPDisplayList(POLY_XLU_DISP++, dl_anime_default);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 4735);
}

void* iriguti_txt[] = {
    gDekuTreeDayEntranceTex,
    gDekuTreeNightEntranceTex,
};

void scene_proc_ydan(PlayState* play) {
    u32 gameplayFrames = play->gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 4763);

    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - (gameplayFrames % 128),
                                (gameplayFrames * 1) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                32, 32));

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    gSPSegment(POLY_OPA_DISP++, 0x08,
               SEGMENTED_TO_VIRTUAL(iriguti_txt[((void)0, z_common_data.save.nightFlag)]));

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 4783);
}

void scene_proc_ydan_boss(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 4845);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (gameplayFrames * 2) % 256, 0, 64, 32, 1, 0,
                                (gameplayFrames * 2) % 128, 64, 32));

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 4859);
}

void* ddan_iriguti_txt[] = {
    gDCDayEntranceTex,
    gDCNightEntranceTex,
};
void* ddan_byou_txt[] = {
    gDCLavaFloor1Tex, gDCLavaFloor2Tex, gDCLavaFloor3Tex, gDCLavaFloor4Tex,
    gDCLavaFloor5Tex, gDCLavaFloor6Tex, gDCLavaFloor7Tex, gDCLavaFloor8Tex,
};

void scene_proc_ddan(PlayState* play) {
    u32 gameplayFrames;
    s32 pad;
    Gfx* displayListHead = GRAPH_ALLOC(play->state.gfxCtx, 2 * sizeof(Gfx[3]));

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 4905);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_OPA_DISP++, 0x08,
               SEGMENTED_TO_VIRTUAL(ddan_iriguti_txt[((void)0, z_common_data.save.nightFlag)]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(ddan_byou_txt[(s32)(gameplayFrames & 14) >> 1]));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (gameplayFrames * 1) % 256, 0, 64, 32, 1, 0,
                                (gameplayFrames * 1) % 128, 64, 32));
    gSPSegment(POLY_OPA_DISP++, 0x0A,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (gameplayFrames * 1) % 128, 32, 32, 1, 0,
                                (gameplayFrames * 2) % 128, 32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    gSPSegment(POLY_OPA_DISP++, 0x0B, displayListHead);
    gDPPipeSync(displayListHead++);
    gDPSetEnvColor(displayListHead++, 255, 255, 255, play->roomCtx.drawParams[BGDODOAGO_EYE_LEFT]);
    gSPEndDisplayList(displayListHead++);

    gSPSegment(POLY_OPA_DISP++, 0x0C, displayListHead);
    gDPPipeSync(displayListHead++);
    gDPSetEnvColor(displayListHead++, 255, 255, 255, play->roomCtx.drawParams[BGDODOAGO_EYE_RIGHT]);
    gSPEndDisplayList(displayListHead++);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 4956);
}

void scene_proc_tokinoma(PlayState* play) {
    f32 temp;
    Gfx* displayListHead = GRAPH_ALLOC(play->state.gfxCtx, 18 * sizeof(Gfx));

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5069);

    temp = play->roomCtx.drawParams[0] / 255.0f;

    gSPSegment(POLY_XLU_DISP++, 0x08, displayListHead);
    gSPSegment(POLY_OPA_DISP++, 0x08, displayListHead);
    gDPSetPrimColor(displayListHead++, 0, 0, 255 - (u8)(185.0f * temp), 255 - (u8)(145.0f * temp),
                    255 - (u8)(105.0f * temp), 255);
    gSPEndDisplayList(displayListHead++);

    gSPSegment(POLY_XLU_DISP++, 0x09, displayListHead);
    gSPSegment(POLY_OPA_DISP++, 0x09, displayListHead);
    gDPSetPrimColor(displayListHead++, 0, 0, 76 + (u8)(6.0f * temp), 76 + (u8)(34.0f * temp), 76 + (u8)(74.0f * temp),
                    255);
    gSPEndDisplayList(displayListHead++);

    gSPSegment(POLY_OPA_DISP++, 0x0A, displayListHead);
    gSPSegment(POLY_XLU_DISP++, 0x0A, displayListHead);
    gDPPipeSync(displayListHead++);
    gDPSetEnvColor(displayListHead++, 0, 0, 0, play->roomCtx.drawParams[0]);
    gSPEndDisplayList(displayListHead++);

    gSPSegment(POLY_OPA_DISP++, 0x0B, displayListHead);
    gSPSegment(POLY_XLU_DISP++, 0x0B, displayListHead);
    gDPSetPrimColor(displayListHead++, 0, 0, 89 + (u8)(166.0f * temp), 89 + (u8)(166.0f * temp),
                    89 + (u8)(166.0f * temp), 255);
    gDPPipeSync(displayListHead++);
    gDPSetEnvColor(displayListHead++, 0, 0, 0, play->roomCtx.drawParams[0]);
    gSPEndDisplayList(displayListHead++);

    gSPSegment(POLY_OPA_DISP++, 0x0C, displayListHead);
    gSPSegment(POLY_XLU_DISP++, 0x0C, displayListHead);
    gDPSetPrimColor(displayListHead++, 0, 0, 255 + (u8)(179.0f * temp), 255 + (u8)(179.0f * temp),
                    255 + (u8)(179.0f * temp), 255);
    gDPPipeSync(displayListHead++);
    gDPSetEnvColor(displayListHead++, 0, 0, 0, play->roomCtx.drawParams[0]);
    gSPEndDisplayList(displayListHead++);

    gSPSegment(POLY_OPA_DISP++, 0x0D, displayListHead);
    gSPSegment(POLY_XLU_DISP++, 0x0D, displayListHead);
    gDPPipeSync(displayListHead++);
    gDPSetEnvColor(displayListHead++, 0, 0, 0, play->roomCtx.drawParams[1]);
    gSPEndDisplayList(displayListHead);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5145);

    if (z_common_data.sceneLayer == 5) {
        E_lf_flag = true;
        E_lf_position.x = -20.0f;
        E_lf_position.y = 1220.0f;
        E_lf_position.z = -684.0f;
        E_lf_wscale = 10;
        E_lf_alpha = 8.0f;
        E_lf_rectalpha = 200;
    }
}

void scene_proc_kakusiana(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5171);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08, tex_scroll2(play->state.gfxCtx, 0, (gameplayFrames * 1) % 64, 256, 16));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - (gameplayFrames % 128),
                                (gameplayFrames * 1) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                32, 32));
    gSPSegment(POLY_OPA_DISP++, 0x0A,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 32, 1, 0,
                                127 - (gameplayFrames * 1) % 128, 32, 32));
    gSPSegment(POLY_OPA_DISP++, 0x0B, tex_scroll2(play->state.gfxCtx, 0, (gameplayFrames * 1) % 128, 32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x0C,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (gameplayFrames * 50) % 2048, 8, 512, 1, 0,
                                (gameplayFrames * 60) % 2048, 8, 512));
    gSPSegment(
        POLY_OPA_DISP++, 0x0D,
        two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 64, 1, 0, (gameplayFrames * 1) % 128, 32, 32));

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5212);
}

void scene_proc_kenjya(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5226);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08, tex_scroll2(play->state.gfxCtx, 0, (gameplayFrames * 2) % 256, 64, 64));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gSPSegment(POLY_OPA_DISP++, 0x0A,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 1) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - (gameplayFrames * 1) % 128,
                                (gameplayFrames * 1) % 256, 32, 64, 1, 0, 0, 32, 128));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5264);
}

void scene_proc_yizumi(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5278);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 3) % 256, 32, 64, 1, gameplayFrames % 128, (gameplayFrames * 3) % 256,
                                32, 64));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 3) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 3) % 128,
                                32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5301);
}

void scene_proc_tatehikari(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5317);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08, tex_scroll2(play->state.gfxCtx, 0, gameplayFrames % 64, 256, 16));

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5330);
}

void scene_proc_hakaana2(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5346);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 3) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 3) % 128,
                                32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x09, tex_scroll2(play->state.gfxCtx, 0, gameplayFrames % 64, 256, 16));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5367);
}

void scene_proc_hakadan(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5384);

    gameplayFrames = play->gameplayFrames;

    if (play->sceneId == SCENE_SHADOW_TEMPLE_BOSS) {
        gSPSegment(POLY_OPA_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (gameplayFrames * 2) % 128, 0, 32, 32, 1,
                                    (gameplayFrames * 2) % 128, 0, 32, 32));
    } else {
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (gameplayFrames * 2) % 128, 0, 32, 32, 1,
                                    (gameplayFrames * 2) % 128, 0, 32, 32));
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5416);
}

void* gway_iriguti_txt[] = {
    gThievesHideoutDayEntranceTex,
    gThievesHideoutNightEntranceTex,
};

void scene_proc_gerudoway(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5490);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_OPA_DISP++, 0x09, tex_scroll2(play->state.gfxCtx, 0, (gameplayFrames * 3) % 128, 32, 32));

    gSPSegment(POLY_XLU_DISP++, 0x08,
               SEGMENTED_TO_VIRTUAL(gway_iriguti_txt[((void)0, z_common_data.save.nightFlag)]));

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5507);
}

void* mizusin_iriguti_txt[] = {
    gWaterTempleDayEntranceTex,
    gWaterTempleNightEntranceTex,
};

void scene_proc_mizusin(PlayState* play) {
    u32 gameplayFrames;
    s32 spB0;
    s32 spAC;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5535);

    spB0 = (play->roomCtx.drawParams[1] >> 8) & 0xFF;
    spAC = play->roomCtx.drawParams[1] & 0xFF;
    gameplayFrames = play->gameplayFrames;

#if !OOT_MQ
    gSPSegment(POLY_XLU_DISP++, 0x06,
               SEGMENTED_TO_VIRTUAL(mizusin_iriguti_txt[((void)0, z_common_data.save.nightFlag)]));
#else
    gSPSegment(POLY_XLU_DISP++, 0x08,
               SEGMENTED_TO_VIRTUAL(mizusin_iriguti_txt[((void)0, z_common_data.save.nightFlag)]));
#endif

    if (spB0 == 1) {
        gSPSegment(POLY_OPA_DISP++, 0x08,
                   two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames * 1, 0, 32, 32, 1, 0, 0,
                                            32, 32, 0, 0, 0, spAC));
    } else if (spB0 < 1) {
        gSPSegment(POLY_OPA_DISP++, 0x08,
                   two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames * 1, 0, 32, 32, 1, 0, 0,
                                            32, 32, 0, 0, 0, 255));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08,
                   two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames * 1, 0, 32, 32, 1, 0, 0,
                                            32, 32, 0, 0, 0, 160));
    }

    if (spB0 == 2) {
        gSPSegment(POLY_OPA_DISP++, 0x09,
                   two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames * 1, 0, 32, 32, 1, 0, 0,
                                            32, 32, 0, 0, 0, spAC));
    } else if (spB0 < 2) {
        gSPSegment(POLY_OPA_DISP++, 0x09,
                   two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames * 1, 0, 32, 32, 1, 0, 0,
                                            32, 32, 0, 0, 0, 255));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x09,
                   two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames * 1, 0, 32, 32, 1, 0, 0,
                                            32, 32, 0, 0, 0, 160));
    }

    if (spB0 != 0) {
        gSPSegment(POLY_OPA_DISP++, 0x0A,
                   two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames * 1, 0, 32, 32, 1, 0, 0,
                                            32, 32, 0, 0, 0, 160));
        gSPSegment(POLY_OPA_DISP++, 0x0B,
                   two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames * 3, 0, 32, 32, 1, 0, 0,
                                            32, 32, 0, 0, 0, 180));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x0A,
                   two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, (gameplayFrames * 1) % 128, 0, 32, 32,
                                            1, 0, 0, 32, 32, 0, 0, 0, 160 + (s32)((spAC / 200.0f) * 95.0f)));
        gSPSegment(POLY_OPA_DISP++, 0x0B,
                   two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames * 3, 0, 32, 32, 1, 0, 0,
                                            32, 32, 0, 0, 0, 185 + (s32)((spAC / 200.0f) * 70.0f)));
    }

    gSPSegment(POLY_XLU_DISP++, 0x0C,
               two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames * 1, gameplayFrames * 1, 32,
                                        32, 1, 0, 127 - (gameplayFrames * 1), 32, 32, 0, 0, 0, 128));
    gSPSegment(POLY_XLU_DISP++, 0x0D,
               two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames * 4, 0, 32, 32, 1,
                                        gameplayFrames * 4, 0, 32, 32, 0, 0, 0, 128));

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5644);
}

void scene_proc_mizusin_boss(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5791);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_OPA_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames * 1, 0, 32, 32, 1, 0, 0, 32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, play->roomCtx.drawParams[0]);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 145);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5808);
}

void scene_proc_syatekijyou(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5822);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_OPA_DISP++, 0x08, tex_scroll2(play->state.gfxCtx, 0, gameplayFrames % 64, 4, 16));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5836);
}

void scene_proc_hairaruniwa(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5850);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 3) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 3) % 128,
                                32, 32));

    if (play->sceneId == SCENE_CASTLE_COURTYARD_GUARDS_DAY) {
        gSPSegment(POLY_XLU_DISP++, 0x09, tex_scroll2(play->state.gfxCtx, 0, (gameplayFrames * 10) % 256, 32, 64));
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5876);
}

void scene_proc_ganontou(PlayState* play) {
    u32 gameplayFrames;
    s8 sp83;

    sp83 = coss(play->gameplayFrames * 1500) >> 8;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5894);

    gameplayFrames = play->gameplayFrames;

    if (play->sceneId == SCENE_OUTSIDE_GANONS_CASTLE) {
        gSPSegment(POLY_XLU_DISP++, 0x09, tex_scroll2(play->state.gfxCtx, 0, (gameplayFrames * 1) % 256, 64, 64));
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 255 - (gameplayFrames * 1) % 256, 64, 64, 1,
                                    0, (gameplayFrames * 1) % 256, 64, 64));
    }

    gSPSegment(POLY_OPA_DISP++, 0x0B,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 255 - (gameplayFrames * 1) % 128,
                                (gameplayFrames * 1) % 128, 32, 32, 1, (gameplayFrames * 1) % 128,
                                (gameplayFrames * 1) % 128, 32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    sp83 = (sp83 >> 1) + 192;
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, sp83, sp83, sp83, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5930);
}

// Screen Shake for Ganon's Tower Collapse
void ganon_quake_proc(PlayState* play) {
    s32 quakeIndex;

    if (play->gameplayFrames % 128 == 13) {
        quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_2);
        setSpeedQuake(quakeIndex, 10000);
        setScaleQuake(quakeIndex, 4, 0, 0, 0);
        setTimerQuake(quakeIndex, 127);
    }

    if ((play->gameplayFrames % 64 == 0) && (fqrand() > 0.6f)) {
        quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
        setSpeedQuake(quakeIndex, 32000.0f + (fqrand() * 3000.0f));
        setScaleQuake(quakeIndex, 10.0f - (fqrand() * 9.0f), 0, 0, 0);
        setTimerQuake(quakeIndex, 48.0f - (fqrand() * 15.0f));
    }
}

void scene_proc_ganon_final(PlayState* play) {
    u32 gameplayFrames;
    s8 sp7B;

    sp7B = coss((play->gameplayFrames * 1500) & 0xFFFF) >> 8;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 5968);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_OPA_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (gameplayFrames * 1) % 512, 64, 128, 1, 0,
                                511 - (gameplayFrames * 1) % 512, 64, 128));
    gSPSegment(POLY_OPA_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (gameplayFrames * 1) % 256, 32, 64, 1, 0,
                                255 - (gameplayFrames * 1) % 256, 32, 64));
    gSPSegment(POLY_XLU_DISP++, 0x0A,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (gameplayFrames * 20) % 2048, 16, 512, 1, 0,
                                (gameplayFrames * 30) % 2048, 16, 512));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    sp7B = (sp7B >> 1) + 192;
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, sp7B, sp7B, sp7B, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6004);

    if (Actor_Environment_sw_Check(play, 0x37)) {
        if ((play->sceneId == SCENE_GANON_BOSS) || (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) ||
            (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) ||
            (play->sceneId == SCENE_INSIDE_GANONS_CASTLE_COLLAPSE)) {
            ganon_quake_proc(play);
        }
    }
}

void* ice_doukutu_iriguti_txt[] = {
    gIceCavernDayEntranceTex,
    gIceCavernNightEntranceTex,
};

void scene_proc_ice_doukutu(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6042);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               SEGMENTED_TO_VIRTUAL(ice_doukutu_iriguti_txt[((void)0, z_common_data.save.nightFlag)]));
    gSPSegment(POLY_OPA_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 1) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x0A,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 1) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6076);
}

void scene_proc_hakaana_ouke(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6151);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08, tex_scroll2(play->state.gfxCtx, 0, (gameplayFrames * 1) % 64, 256, 16));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (gameplayFrames * 60) % 2048, 8, 512, 1, 0,
                                (gameplayFrames * 50) % 2048, 8, 512));
    gSPSegment(POLY_OPA_DISP++, 0x0A,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - (gameplayFrames * 1) % 128, 0, 32, 32, 1,
                                (gameplayFrames * 1) % 128, 0, 32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x0B,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 1023 - (gameplayFrames * 6) % 1024, 16, 256, 1,
                                0, 1023 - (gameplayFrames * 3) % 1024, 16, 256));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6187);
}

void scene_proc_hylia_labo(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6201);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(
        POLY_OPA_DISP++, 0x08,
        two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 32, 1, 0, (gameplayFrames * 1) % 128, 32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x0A,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 1) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x09, tex_scroll2(play->state.gfxCtx, 0, 255 - (gameplayFrames * 10) % 256, 32, 64));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6232);
}

void scene_proc_wtscr1_only(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6249);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 1) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6264);
}

void* men_iriguti_txt[] = {
    gGTGDayEntranceTex,
    gGTGNightEntranceTex,
};

void scene_proc_men(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6290);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               SEGMENTED_TO_VIRTUAL(men_iriguti_txt[((void)0, z_common_data.save.nightFlag)]));
    gSPSegment(POLY_OPA_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 1) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x0A,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 1) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6320);
}

Gfx* two_tex_scroll_prim(GraphicsContext* gfxCtx, s32 tile1, u32 x1, u32 y1, s32 width1, s32 height1, s32 tile2,
                               u32 x2, u32 y2, s32 width2, s32 height2, s32 r, s32 g, s32 b, s32 a) {
    Gfx* displayList = GRAPH_ALLOC(gfxCtx, 10 * sizeof(Gfx));

    x1 %= 512 << 2;
    y1 %= 512 << 2;
    x2 %= 512 << 2;
    y2 %= 512 << 2;

    gDPTileSync(displayList);
    gDPSetTileSize(displayList + 1, tile1, x1, y1, x1 + ((width1 - 1) << 2), y1 + ((height1 - 1) << 2));
    gDPTileSync(displayList + 2);
    gDPSetTileSize(displayList + 3, tile2, x2, y2, x2 + ((width2 - 1) << 2), y2 + ((height2 - 1) << 2));
    gDPSetPrimColor(displayList + 4, 0, 0, r, g, b, a);
    gSPEndDisplayList(displayList + 5);

    return displayList;
}

void scene_proc_turibori(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6433);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll_prim(play->state.gfxCtx, 0, 127 - gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                         32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128, 32, 32, 255, 255,
                                         255, play->roomCtx.drawParams[0] + 127));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6449);
}

void scene_proc_bowling(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6463);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08, tex_scroll2(play->state.gfxCtx, 127 - (gameplayFrames * 4) % 128, 0, 32, 32));
    gSPSegment(POLY_OPA_DISP++, 0x09, tex_scroll2(play->state.gfxCtx, 0, (gameplayFrames * 5) % 64, 16, 16));
    gSPSegment(POLY_OPA_DISP++, 0x0A, tex_scroll2(play->state.gfxCtx, 0, 63 - (gameplayFrames * 2) % 64, 16, 16));
    gSPSegment(POLY_XLU_DISP++, 0x0B,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 127 - (gameplayFrames * 3) % 128, 32, 32, 1, 0,
                                0, 32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6491);
}

void* souko_iriguti_txt[] = {
    gLonLonHouseDayEntranceTex,
    gLonLonHouseNightEntranceTex,
};

void scene_proc_souko(PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6515);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               SEGMENTED_TO_VIRTUAL(souko_iriguti_txt[((void)0, z_common_data.save.nightFlag)]));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6528);
}

void* miharigoya_modoL_txt[] = {
    gGuardHouseOutSideView1DayTex,
    gGuardHouseOutSideView1NightTex,
};
void* miharigoya_modoR_txt[] = {
    gGuardHouseOutSideView2DayTex,
    gGuardHouseOutSideView2NightTex,
};

void scene_proc_miharigoya(PlayState* play) {
    s32 var;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6560);

    if (LINK_IS_ADULT) {
        var = 1;
    } else {
        var = z_common_data.save.nightFlag;
    }

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(miharigoya_modoR_txt[var]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(miharigoya_modoL_txt[var]));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6581);
}

void scene_proc_mahouya(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6595);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_OPA_DISP++, 0x08, tex_scroll2(play->state.gfxCtx, 0, (gameplayFrames * 3) % 128, 32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 1023 - (gameplayFrames * 3) % 1024, 16, 256, 1,
                                0, 1023 - (gameplayFrames * 6) % 1024, 16, 256));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6615);
}

void* mori_iriguti_txt[] = {
    gForestTempleDayEntranceTex,
    gForestTempleNightEntranceTex,
};

void scene_proc_morisin(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6640);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               SEGMENTED_TO_VIRTUAL(mori_iriguti_txt[((void)0, z_common_data.save.nightFlag)]));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 1) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                32, 32));
    gSPSegment(POLY_OPA_DISP++, 0x0A,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 1) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6671);
}

void* jyasin_iriguti_txt[] = {
    gSpiritTempleDayEntranceTex,
    gSpiritTempleNightEntranceTex,
};

void scene_proc_jyasinzou(PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6752);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               SEGMENTED_TO_VIRTUAL(jyasin_iriguti_txt[((void)0, z_common_data.save.nightFlag)]));

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6762);
}

void scene_proc_spot00(PlayState* play) {
    u32 gameplayFrames;
    Gfx* displayListHead;

    displayListHead = GRAPH_ALLOC(play->state.gfxCtx, 3 * sizeof(Gfx));

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6814);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 3) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 3) % 128,
                                32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 10) % 128, 32, 32, 1, gameplayFrames % 128,
                                (gameplayFrames * 10) % 128, 32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    gSPSegment(POLY_XLU_DISP++, 0x0A, displayListHead);

    if ((z_common_data.save.dayTime > CLOCK_TIME(7, 0)) && (z_common_data.save.dayTime <= CLOCK_TIME(18, 30))) {
        gSPEndDisplayList(displayListHead++);
    } else {
        if (z_common_data.save.dayTime > CLOCK_TIME(18, 30)) {
            if (play->roomCtx.drawParams[0] != 255) {
                chase_s(&play->roomCtx.drawParams[0], 255, 5);
            }
        } else if (z_common_data.save.dayTime >= CLOCK_TIME(6, 0)) {
            if (play->roomCtx.drawParams[0] != 0) {
                chase_s(&play->roomCtx.drawParams[0], 0, 10);
            }
        }

        gDPSetPrimColor(displayListHead++, 0, 0, 255, 255, 255, play->roomCtx.drawParams[0]);
        gSPDisplayList(displayListHead++, spot00_room_0DL_012B20);
        gSPEndDisplayList(displayListHead++);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6866);
}

void* spot01_mado_txt[] = {
    gKakarikoVillageDayWindowTex,
    gKakarikoVillageNightWindowTex,
};

void scene_proc_spot01(PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6890);

    gSPSegment(POLY_OPA_DISP++, 0x08,
               SEGMENTED_TO_VIRTUAL(spot01_mado_txt[((void)0, z_common_data.save.nightFlag)]));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6903);
}

void scene_proc_spot03(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6917);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 6) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 6) % 128,
                                32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 3) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 3) % 128,
                                32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x0A,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 1) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6948);
}

void scene_proc_spot04(PlayState* play) {
    u32 gameplayFrames;
    u8 spA3;
    u16 spA0;
    Gfx* displayListHead;

    spA3 = 128;
    spA0 = 500;
    displayListHead = GRAPH_ALLOC(play->state.gfxCtx, 6 * sizeof(Gfx));

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 6965);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 1) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 10) % 128, 32, 32, 1, gameplayFrames % 128,
                                (gameplayFrames * 10) % 128, 32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    if (z_common_data.sceneLayer == 4) {
        spA3 = 255 - (u8)play->roomCtx.drawParams[0];
    } else if (z_common_data.sceneLayer == 6) {
        spA0 = play->roomCtx.drawParams[0] + 500;
    } else if (
#if OOT_VERSION < PAL_1_0
        !IS_CUTSCENE_LAYER && GET_EVENTCHKINF(EVENTCHKINF_07)
#else
        (!IS_CUTSCENE_LAYER || LINK_IS_ADULT) && GET_EVENTCHKINF(EVENTCHKINF_07)
#endif
    ) {
        spA0 = 2150;
    }

    gSPSegment(POLY_OPA_DISP++, 0x0A, displayListHead);
    gDPPipeSync(displayListHead++);
    gDPSetEnvColor(displayListHead++, 128, 128, 128, spA3);
    gSPEndDisplayList(displayListHead++);

    gSPSegment(POLY_XLU_DISP++, 0x0B, displayListHead);
    gSPSegment(POLY_OPA_DISP++, 0x0B, displayListHead);
    gDPPipeSync(displayListHead++);
    gDPSetEnvColor(displayListHead++, 128, 128, 128, spA0 * 0.1f);
    gSPEndDisplayList(displayListHead++);

    gSPSegment(POLY_OPA_DISP++, 0x0C,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (s16)(-play->roomCtx.drawParams[0] * 0.02f), 32,
                                16, 1, 0, (s16)(-play->roomCtx.drawParams[0] * 0.02f), 32, 16));

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7044);
}

void scene_proc_spot06(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7058);

    if (IS_CUTSCENE_LAYER || (LINK_IS_ADULT && !GET_EVENTCHKINF(EVENTCHKINF_RESTORED_LAKE_HYLIA))) {
        play->roomCtx.drawParams[0] = 87;
    }

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_OPA_DISP++, 0x08,
               two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames, gameplayFrames, 32, 32, 1,
                                        0, 0, 32, 32, 0, 0, 0, play->roomCtx.drawParams[0] + 168));
    gSPSegment(POLY_OPA_DISP++, 0x09,
               two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, -gameplayFrames, -gameplayFrames, 32, 32,
                                        1, 0, 0, 16, 64, 0, 0, 0, play->roomCtx.drawParams[0] + 168));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7097);
}

void* spot07_iriguti_txt[] = {
    gZorasDomainDayEntranceTex,
    gZorasDomainNightEntranceTex,
};

void scene_proc_spot07(PlayState* play) {
    u32 gameplayFrames;
    u32 var;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7123);

    gameplayFrames = play->gameplayFrames;
    var = 127 - (gameplayFrames * 1) % 128;
    if (LINK_IS_ADULT) {
        var = 0;
    }
    gSPSegment(POLY_OPA_DISP++, 0x0C,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 64, 32, 1, 0, var, 64, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               SEGMENTED_TO_VIRTUAL(spot07_iriguti_txt[((void)0, z_common_data.save.nightFlag)]));

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7147);
}

void scene_proc_spot08(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7161);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(
        POLY_OPA_DISP++, 0x08,
        two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (gameplayFrames * 1) % 128, 0, 32, 32, 1, 0, 0, 32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 255 - (gameplayFrames * 2) % 256, 64, 64, 1, 0,
                                255 - (gameplayFrames * 2) % 256, 64, 64));
    gSPSegment(POLY_XLU_DISP++, 0x0A,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (gameplayFrames * 1) % 128, 32, 32, 1, 0,
                                (gameplayFrames * 1) % 128, 32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7192);
}

void scene_proc_spot09(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7206);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (gameplayFrames * 3) % 1024, 32, 256, 1, 0,
                                (gameplayFrames * 3) % 1024, 32, 256));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (gameplayFrames * 1) % 256, 64, 64, 1, 0,
                                (gameplayFrames * 1) % 256, 64, 64));
    gSPSegment(POLY_XLU_DISP++, 0x0A,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (gameplayFrames * 2) % 128, 32, 32, 1, 0,
                                (gameplayFrames * 2) % 128, 32, 32));
    gSPSegment(POLY_OPA_DISP++, 0x0B,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 32, 1, 0,
                                127 - (gameplayFrames * 3) % 128, 32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x0C,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (gameplayFrames * 1) % 128, 32, 32, 1, 0,
                                (gameplayFrames * 1) % 128, 32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x0D,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (gameplayFrames * 1) % 64, 16, 16, 1, 0,
                                (gameplayFrames * 1) % 64, 16, 16));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7260);
}

void scene_proc_spot10(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7274);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames % 128, 0, 32, 16, 1,
                                gameplayFrames % 128, 0, 32, 16));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128, gameplayFrames % 128,
                                32, 32, 1, gameplayFrames % 128, gameplayFrames % 128, 32, 32));

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    if ((play->roomCtx.drawParams[0] == 0) && (INV_CONTENT(ITEM_COJIRO) == ITEM_COJIRO)) {
        if (play->roomCtx.drawParams[1] == 50) {
            player_SE_set(GET_PLAYER(play), NA_SE_EV_CHICKEN_CRY_M);
            play->roomCtx.drawParams[0] = 1;
        }
        play->roomCtx.drawParams[1]++;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7309);
}

void scene_proc_spot11(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7323);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(
        POLY_OPA_DISP++, 0x08,
        two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 32, 1, 0, 127 - gameplayFrames % 128, 32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7339);
}

void* spot12_rou_txt[] = {
    gGerudoFortressNightWallTex,
    gGerudoFortressDayWallTex,
};

void scene_proc_spot12(PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7363);

    gSPSegment(POLY_OPA_DISP++, 0x08,
               SEGMENTED_TO_VIRTUAL(spot12_rou_txt[((void)0, z_common_data.save.nightFlag)]));

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7371);
}

void scene_proc_spot13(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7385);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_OPA_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, gameplayFrames % 128, 32, 32, 1, 0,
                                gameplayFrames % 128, 32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, gameplayFrames % 128, 32, 32, 1, 0,
                                gameplayFrames % 128, 32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7409);
}

void scene_proc_spot15(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7423);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 10) % 128, 32, 32, 1, gameplayFrames % 128,
                                (gameplayFrames * 10) % 128, 32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 3) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 3) % 128,
                                32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7443);
}

void scene_proc_spot16(PlayState* play) {
    Gfx* displayListHead = GRAPH_ALLOC(play->state.gfxCtx, 3 * sizeof(Gfx));

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7461);

    gSPSegment(POLY_XLU_DISP++, 0x08, displayListHead);

    if ((z_common_data.save.dayTime > CLOCK_TIME(7, 0)) && (z_common_data.save.dayTime <= CLOCK_TIME(18, 0))) {
        gSPEndDisplayList(displayListHead++);
    } else {
        if (z_common_data.save.dayTime > CLOCK_TIME(18, 0)) {
            if (play->roomCtx.drawParams[0] != 255) {
                chase_s(&play->roomCtx.drawParams[0], 255, 5);
            }
        } else if (z_common_data.save.dayTime >= CLOCK_TIME(6, 0)) {
            if (play->roomCtx.drawParams[0] != 0) {
                chase_s(&play->roomCtx.drawParams[0], 0, 10);
            }
        }

        gDPSetPrimColor(displayListHead++, 0, 0, 255, 255, 255, play->roomCtx.drawParams[0]);
        gSPDisplayList(displayListHead++, spot16_room_0DL_00AA48);
        gSPEndDisplayList(displayListHead++);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7495);
}

void scene_proc_spot17(PlayState* play) {
    s8 sp6F = coss((play->gameplayFrames * 1500) & 0xFFFF) >> 8;
    s8 sp6E = coss((play->gameplayFrames * 1500) & 0xFFFF) >> 8;
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7512);

    gameplayFrames = play->gameplayFrames;
    sp6F = (sp6F >> 1) + 192;
    sp6E = (sp6E >> 1) + 192;

    gSPSegment(POLY_OPA_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, gameplayFrames % 128, 32, 32, 1, 0,
                                gameplayFrames % 128, 32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, sp6F, sp6E, 255, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7530);
}

void* spot18_iriguti_txt[] = {
    gGoronCityDayEntranceTex,
    gGoronCityNightEntranceTex,
};

void scene_proc_spot18(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7555);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_OPA_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 127 - gameplayFrames % 128, 32, 32, 1,
                                gameplayFrames % 128, 0, 32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               SEGMENTED_TO_VIRTUAL(spot18_iriguti_txt[((void)0, z_common_data.save.nightFlag)]));

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7578);
}

void* spot20_mado_txt[] = {
    gLonLonRanchDayWindowTex,
    gLonLonRangeNightWindowsTex,
};

void scene_proc_spot20(PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7602);

    gSPSegment(POLY_OPA_DISP++, 0x08,
               SEGMENTED_TO_VIRTUAL(spot20_mado_txt[((void)0, z_common_data.save.nightFlag)]));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7615);
}

void scene_proc_fire(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7630);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_OPA_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 127 - gameplayFrames % 128, 32, 32, 1,
                                127 - gameplayFrames % 128, 0, 32, 32));
    gSPSegment(POLY_OPA_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (gameplayFrames * 3) % 128,
                                127 - (gameplayFrames * 6) % 128, 32, 32, 1, (gameplayFrames * 6) % 128,
                                127 - (gameplayFrames * 3) % 128, 32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 64);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 64);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7653);
}

f32 sin_cnt = 0.0f;

void scene_proc_bdan(PlayState* play) {
    static s16 c1 = 538;
    static s16 c2 = 4272;
    u32 gameplayFrames;
    f32 temp;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7712);

    gameplayFrames = play->gameplayFrames;
    if (play->sceneId == SCENE_JABU_JABU) {
        gSPSegment(POLY_OPA_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames % 128,
                                    (gameplayFrames * 2) % 128, 32, 32, 1, 127 - gameplayFrames % 128,
                                    (gameplayFrames * 2) % 128, 32, 32));
        gSPSegment(POLY_OPA_DISP++, 0x0B,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 255 - (gameplayFrames * 4) % 256, 32, 64, 1,
                                    0, 255 - (gameplayFrames * 4) % 256, 32, 64));
    } else {
        gSPSegment(
            POLY_OPA_DISP++, 0x08,
            tex_scroll2(play->state.gfxCtx, (127 - (gameplayFrames * 1)) % 128, (gameplayFrames * 1) % 128, 32, 32));
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    if (_Game_play_isPause(play) != true) {

        c1 += 1820;
        c2 += 1820;

        temp = 0.020000001f;
        stretchViewRotate(&play->view,
                                      ((360.00018f / 65535.0f) * (M_PI / 180.0f)) * temp * cos_s(c1),
                                      ((360.00018f / 65535.0f) * (M_PI / 180.0f)) * temp * sin_s(c1),
                                      ((360.00018f / 65535.0f) * (M_PI / 180.0f)) * temp * sin_s(c2));
        stretchViewScale(&play->view, 1.f + (0.79999995f * temp * sin_s(c2)),
                                1.f + (0.39999998f * temp * cos_s(c2)),
                                1.f + (1 * temp * cos_s(c1)));
        stretchViewSpeed(&play->view, 0.95f);

        switch (play->roomCtx.drawParams[0]) {
            case 0:
                break;
            case 1:
                if (play->roomCtx.drawParams[1] < 1200) {
                    play->roomCtx.drawParams[1] += 200;
                } else {
                    play->roomCtx.drawParams[0]++;
                }
                break;
            case 2:
                if (play->roomCtx.drawParams[1] > 0) {
                    play->roomCtx.drawParams[1] -= 30;
                } else {
                    play->roomCtx.drawParams[1] = 0;
                    play->roomCtx.drawParams[0] = 0;
                }
                break;
        }

        sin_cnt += 0.15f + (play->roomCtx.drawParams[1] * 0.001f);
    }

    if (play->roomCtx.curRoom.num == 2) {
        Matrix_scale(1.0f, sinf(sin_cnt) * 0.8f, 1.0f, MTXMODE_NEW);
    } else {
        Matrix_scale(1.005f, sinf(sin_cnt) * 0.8f, 1.005f, MTXMODE_NEW);
    }

    gSPSegment(POLY_OPA_DISP++, 0x0D, MATRIX_FINALIZE(play->state.gfxCtx, "../z_scene_table.c", 7809));

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7811);
}

void scene_proc_ganontika(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7825);

    gameplayFrames = play->gameplayFrames;

    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 1) % 512, 32, 128, 1, gameplayFrames % 128,
                                (gameplayFrames * 1) % 512, 32, 128));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 1) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                32, 32));
    gSPSegment(POLY_OPA_DISP++, 0x0A,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - gameplayFrames % 128,
                                (gameplayFrames * 1) % 128, 32, 32, 1, gameplayFrames % 128, (gameplayFrames * 1) % 128,
                                32, 32));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7852);
}

void scene_proc_ganontikasonogo(PlayState* play) {
    ganon_quake_proc(play);
}

void scene_proc_ganon_sonogo(PlayState* play) {
    ganon_quake_proc(play);
}

void scene_proc_besitu(PlayState* play) {
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7893);

    gameplayFrames = play->gameplayFrames;
    gSPSegment(POLY_OPA_DISP++, 0x08, tex_scroll2(play->state.gfxCtx, 127 - (gameplayFrames * 2) % 128, 0, 32, 64));
    gSPSegment(POLY_OPA_DISP++, 0x09, tex_scroll2(play->state.gfxCtx, 0, (gameplayFrames * 2) % 512, 128, 128));

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);

    CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 7910);
}

#if !PLATFORM_N64 // call_scene_proc is at beginning of file in N64 versions

SceneDrawConfigFunc scene_proc[SDC_MAX] = {
    scene_proc_non,                     // SDC_DEFAULT
    scene_proc_spot00,                 // SDC_HYRULE_FIELD
    scene_proc_spot01,             // SDC_KAKARIKO_VILLAGE
    scene_proc_spot03,                  // SDC_ZORAS_RIVER
    scene_proc_spot04,                // SDC_KOKIRI_FOREST
    scene_proc_spot06,                   // SDC_LAKE_HYLIA
    scene_proc_spot07,                 // SDC_ZORAS_DOMAIN
    scene_proc_spot08,               // SDC_ZORAS_FOUNTAIN
    scene_proc_spot09,                // SDC_GERUDO_VALLEY
    scene_proc_spot10,                   // SDC_LOST_WOODS
    scene_proc_spot11,              // SDC_DESERT_COLOSSUS
    scene_proc_spot12,             // SDC_GERUDOS_FORTRESS
    scene_proc_spot13,            // SDC_HAUNTED_WASTELAND
    scene_proc_spot15,                // SDC_HYRULE_CASTLE
    scene_proc_spot16,          // SDC_DEATH_MOUNTAIN_TRAIL
    scene_proc_spot17,         // SDC_DEATH_MOUNTAIN_CRATER
    scene_proc_spot18,                   // SDC_GORON_CITY
    scene_proc_spot20,                 // SDC_LON_LON_RANCH
    scene_proc_fire,                  // SDC_FIRE_TEMPLE
    scene_proc_ydan,                    // SDC_DEKU_TREE
    scene_proc_ddan,              // SDC_DODONGOS_CAVERN
    scene_proc_bdan,                    // SDC_JABU_JABU
    scene_proc_morisin,                // SDC_FOREST_TEMPLE
    scene_proc_mizusin,                 // SDC_WATER_TEMPLE
    scene_proc_hakadan,         // SDC_SHADOW_TEMPLE_AND_WELL
    scene_proc_jyasinzou,                // SDC_SPIRIT_TEMPLE
    scene_proc_ganontika,          // SDC_INSIDE_GANONS_CASTLE
    scene_proc_men,        // SDC_GERUDO_TRAINING_GROUND
    scene_proc_ydan_boss,                // SDC_DEKU_TREE_BOSS
    scene_proc_mizusin_boss,             // SDC_WATER_TEMPLE_BOSS
    scene_proc_tokinoma,                // SDC_TEMPLE_OF_TIME
    scene_proc_kakusiana,                     // SDC_GROTTOS
    scene_proc_kenjya,           // SDC_CHAMBER_OF_THE_SAGES
    scene_proc_yizumi,          // SDC_GREAT_FAIRYS_FOUNTAIN
    scene_proc_syatekijyou,             // SDC_SHOOTING_GALLERY
    scene_proc_hairaruniwa,       // SDC_CASTLE_COURTYARD_GUARDS
    scene_proc_ganontou,         // SDC_OUTSIDE_GANONS_CASTLE
    scene_proc_ice_doukutu,                   // SDC_ICE_CAVERN
    scene_proc_ganon_final, // SDC_GANONS_TOWER_COLLAPSE_EXTERIOR
    scene_proc_hakaana2,              // SDC_FAIRYS_FOUNTAIN
    scene_proc_gerudoway,              // SDC_THIEVES_HIDEOUT
    scene_proc_bowling,         // SDC_BOMBCHU_BOWLING_ALLEY
    scene_proc_hakaana_ouke,            // SDC_ROYAL_FAMILYS_TOMB
    scene_proc_hylia_labo,          // SDC_LAKESIDE_LABORATORY
    scene_proc_souko,             // SDC_LON_LON_BUILDINGS
    scene_proc_miharigoya,            // SDC_MARKET_GUARD_HOUSE
    scene_proc_mahouya,            // SDC_POTION_SHOP_GRANNY
    scene_proc_wtscr1_only,                   // SDC_CALM_WATER
    scene_proc_tatehikari,       // SDC_GRAVE_EXIT_LIGHT_SHINING
    scene_proc_besitu,                      // SDC_BESITU
    scene_proc_turibori,                 // SDC_FISHING_POND
    scene_proc_ganon_sonogo, // SDC_GANONS_TOWER_COLLAPSE_INTERIOR
    scene_proc_ganontikasonogo,  // SDC_INSIDE_GANONS_CASTLE_COLLAPSE
};

void call_scene_proc(PlayState* play) {
#if DEBUG_FEATURES
    if (R_HREG_MODE == HREG_MODE_SCENE_CONFIG) {
        if (R_SCENE_CONFIG_INIT != HREG_MODE_SCENE_CONFIG) {
            R_SCENE_CONFIG_INIT = HREG_MODE_SCENE_CONFIG;
            R_SCENE_CONFIG_DRAW_DEFAULT_DLIST = 1;
            R_SCENE_CONFIG_DRAW_SCENE_CONFIG = 1;

            // these regs are not used in this mode
            HREG(83) = 0;
            HREG(84) = 0;
            HREG(85) = 0;
            HREG(86) = 0;
            HREG(87) = 0;
            HREG(88) = 0;
            HREG(89) = 0;
            HREG(91) = 0;
            HREG(92) = 0;
            HREG(93) = 0;
            HREG(94) = 0;
        }

        OPEN_DISPS(play->state.gfxCtx, "../z_scene_table.c", 8104);

        if (R_SCENE_CONFIG_DRAW_DEFAULT_DLIST == 1) {
            gSPDisplayList(POLY_OPA_DISP++, dl_anime_default);
            gSPDisplayList(POLY_XLU_DISP++, dl_anime_default);
        }

        CLOSE_DISPS(play->state.gfxCtx, "../z_scene_table.c", 8109);

        if (R_SCENE_CONFIG_DRAW_SCENE_CONFIG == 1) {
            scene_proc[play->sceneDrawConfig](play);
        }
    } else {
        scene_proc[play->sceneDrawConfig](play);
    }
#else
    scene_proc[play->sceneDrawConfig](play);
#endif
}

#endif
