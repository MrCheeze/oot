/*
 * File: z_bg_spot03_taki.c
 * Overlay: ovl_Bg_Spot03_Taki
 * Description: Zora's River Waterfall
 */

#include "z_bg_spot03_taki.h"
#include "assets/objects/object_spot03_object/object_spot03_object.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Spot03_Taki_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot03_Taki_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot03_Taki_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot03_Taki_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgSpot03Taki* this, PlayState* play);

ActorProfile Bg_Spot03_Taki_Profile = {
    /**/ ACTOR_BG_SPOT03_TAKI,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT03_OBJECT,
    /**/ sizeof(BgSpot03Taki),
    /**/ Bg_Spot03_Taki_actor_ct,
    /**/ Bg_Spot03_Taki_actor_dt,
    /**/ Bg_Spot03_Taki_actor_move,
    /**/ Bg_Spot03_Taki_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void func_taki_vtx_chg_w(BgSpot03Taki* this, s32 bufferIndex) {
    s32 i;
    Vtx* vtx = (bufferIndex == 0) ? SEGMENTED_TO_VIRTUAL(object_spot03_object_Vtx_000800)
                                  : SEGMENTED_TO_VIRTUAL(object_spot03_object_Vtx_000990);

    for (i = 0; i < 5; i++) {
        vtx[i + 10].v.cn[3] = this->openingAlpha;
    }
}

void Bg_Spot03_Taki_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot03Taki* this = (BgSpot03Taki*)thisx;
    s16 pad;
    CollisionHeader* colHeader = NULL;

    this->switchFlag = PARAMS_GET_U(this->dyna.actor.params, 0, 6);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&object_spot03_object_Col_000C98, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    ValueSet_process(&this->dyna.actor, value_init);
    this->bufferIndex = 0;
    this->openingAlpha = 255.0f;
    func_taki_vtx_chg_w(this, 0);
    func_taki_vtx_chg_w(this, 1);
    this->actionFunc = mode_wait;
}

void Bg_Spot03_Taki_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot03Taki* this = (BgSpot03Taki*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mode_wait(BgSpot03Taki* this, PlayState* play) {
    if (this->state == WATERFALL_CLOSED) {
        if (Actor_Environment_sw_Check(play, this->switchFlag)) {
            this->state = WATERFALL_OPENING_ANIMATED;
            this->timer = 40;
            makeOnepointDemo(play, 4100, -99, NULL, CAM_ID_MAIN);
        }
    } else if (this->state == WATERFALL_OPENING_IDLE) {
        this->timer--;
        if (this->timer < 0) {
            this->state = WATERFALL_OPENING_ANIMATED;
        }
    } else if (this->state == WATERFALL_OPENING_ANIMATED) {
        if (this->openingAlpha > 0) {
            this->openingAlpha -= 5;
            if (this->openingAlpha <= 0.0f) {
                DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
                this->timer = 400;
                this->state = WATERFALL_OPENED;
                this->openingAlpha = 0;
            }
        }
    } else if (this->state == WATERFALL_OPENED) {
        this->timer--;
        if (this->timer < 0) {
            this->state = WATERFALL_CLOSING;
        }
    } else if (this->state == WATERFALL_CLOSING) {
        if (this->openingAlpha < 255.0f) {
            this->openingAlpha += 5.0f;
            if (this->openingAlpha >= 255.0f) {
                DynaPolygonInfo_clearThrough(play, &play->colCtx.dyna, this->dyna.bgId);
                this->state = WATERFALL_CLOSED;
                this->openingAlpha = 255.0f;
                Actor_Environment_sw_Off(play, this->switchFlag);
            }
        }
    }

    func_taki_vtx_chg_w(this, this->bufferIndex);
}

void Bg_Spot03_Taki_actor_move(Actor* thisx, PlayState* play) {
    BgSpot03Taki* this = (BgSpot03Taki*)thisx;

    this->actionFunc(this, play);
}

void Bg_Spot03_Taki_actor_draw(Actor* thisx, PlayState* play) {
    BgSpot03Taki* this = (BgSpot03Taki*)thisx;
    s32 pad;
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot03_taki.c", 321);

    gameplayFrames = play->gameplayFrames;

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_spot03_taki.c", 325);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, gameplayFrames * 5, 64, 64, 1, 0,
                                gameplayFrames * 5, 64, 64));

    gSPDisplayList(POLY_XLU_DISP++, object_spot03_object_DL_000B20);

    if (this->bufferIndex == 0) {
        gSPVertex(POLY_XLU_DISP++, object_spot03_object_Vtx_000800, 25, 0);
    } else {
        gSPVertex(POLY_XLU_DISP++, object_spot03_object_Vtx_000990, 25, 0);
    }

    gSPDisplayList(POLY_XLU_DISP++, object_spot03_object_DL_000BC0);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames * 1, gameplayFrames * 3, 64, 64, 1,
                                -gameplayFrames, gameplayFrames * 3, 64, 64));

    gSPDisplayList(POLY_XLU_DISP++, object_spot03_object_DL_001580);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot03_taki.c", 358);

    this->bufferIndex = this->bufferIndex == 0;

    if (this->state >= WATERFALL_OPENING_IDLE && this->state <= WATERFALL_OPENED) {
        Na_SetWaterfallSe(&this->dyna.actor.projectedPos, 0.5f);
    } else {
        Na_SetWaterfallSe(&this->dyna.actor.projectedPos, 1.0f);
    }
}
