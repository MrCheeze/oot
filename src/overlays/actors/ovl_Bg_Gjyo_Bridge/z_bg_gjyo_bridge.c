/*
 * File: z_bg_gjyo_bridge.c
 * Overlay: ovl_Bg_Gjyo_Bridge
 * Description: Rainbow Bridge outside Ganon's Castle
 */

#include "z_bg_gjyo_bridge.h"

#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "segmented_address.h"
#include "sys_matrix.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/object_gjyo_objects/object_gjyo_objects.h"
#include "assets/scenes/overworld/ganon_tou/ganon_tou_scene.h"

#define FLAGS 0

void Bg_Gjyo_Bridge_actor_ct(Actor* thisx, PlayState* play);
void Bg_Gjyo_Bridge_actor_dt(Actor* thisx, PlayState* play);
void Bg_Gjyo_Bridge_actor_move(Actor* thisx, PlayState* play);
void Bg_Gjyo_Bridge_actor_draw(Actor* thisx, PlayState* play);

static void mode_stop(BgGjyoBridge* this, PlayState* play);
static void mode_wait(BgGjyoBridge* this, PlayState* play);
static void mode_wait2(BgGjyoBridge* this, PlayState* play);

ActorProfile Bg_Gjyo_Bridge_Profile = {
    /**/ ACTOR_BG_GJYO_BRIDGE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GJYO_OBJECTS,
    /**/ sizeof(BgGjyoBridge),
    /**/ Bg_Gjyo_Bridge_actor_ct,
    /**/ Bg_Gjyo_Bridge_actor_dt,
    /**/ Bg_Gjyo_Bridge_actor_move,
    /**/ Bg_Gjyo_Bridge_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 800, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Gjyo_Bridge_actor_ct(Actor* thisx, PlayState* play) {
    BgGjyoBridge* this = (BgGjyoBridge*)thisx;
    s32 pad;
    CollisionHeader* colHeader;

    colHeader = NULL;

    ValueSet_process(thisx, value_init);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gRainbowBridgeCol, &colHeader);

    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);

    if (GET_EVENTCHKINF(EVENTCHKINF_CREATED_RAINBOW_BRIDGE)) {
        this->actionFunc = mode_stop;
    } else {
        this->dyna.actor.draw = NULL;
        DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
        this->actionFunc = mode_wait;
    }
}

void Bg_Gjyo_Bridge_actor_dt(Actor* thisx, PlayState* play) {
    BgGjyoBridge* this = (BgGjyoBridge*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mode_stop(BgGjyoBridge* this, PlayState* play) {
}

static void mode_wait(BgGjyoBridge* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) && CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) &&
        (INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT) && (player->actor.world.pos.x > -70.0f) &&
        (player->actor.world.pos.x < 300.0f) && (player->actor.world.pos.y > 1340.0f) &&
        (player->actor.world.pos.z > 1340.0f) && (player->actor.world.pos.z < 1662.0f) && !Game_play_demo_mode_check(play)) {
        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gRainbowBridgeCs);
        z_common_data.cutsceneTrigger = 1;
        this->actionFunc = mode_wait2;
    }
}

static void mode_wait2(BgGjyoBridge* this, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[2] != NULL) &&
        (play->csCtx.actorCues[2]->id == 2)) {
        this->dyna.actor.draw = Bg_Gjyo_Bridge_actor_draw;
        DynaPolygonInfo_clearThrough(play, &play->colCtx.dyna, this->dyna.bgId);
        SET_EVENTCHKINF(EVENTCHKINF_CREATED_RAINBOW_BRIDGE);
    }
}

void Bg_Gjyo_Bridge_actor_move(Actor* thisx, PlayState* play) {
    BgGjyoBridge* this = (BgGjyoBridge*)thisx;

    this->actionFunc(this, play);
}

void Bg_Gjyo_Bridge_actor_draw(Actor* thisx, PlayState* play) {
    PlayState* play2 = (PlayState*)play;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_gjyo_bridge.c", 260);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gSPSegment(
        POLY_XLU_DISP++, 8,
        tex_scroll2(play->state.gfxCtx, play2->gameplayFrames & 127, play2->gameplayFrames * -3 & 127, 32, 32));

    gSPSegment(POLY_XLU_DISP++, 9,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, -play2->gameplayFrames & 127, 32, 32, 1, 0,
                                play2->gameplayFrames & 127, 32, 32));

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_gjyo_bridge.c", 281);

    gSPDisplayList(POLY_XLU_DISP++, gRainbowBridgeDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_gjyo_bridge.c", 285);
}
