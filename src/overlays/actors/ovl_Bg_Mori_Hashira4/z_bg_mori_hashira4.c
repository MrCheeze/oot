/*
 * File: z_bg_mori_hashira4
 * Overlay: ovl_Bg_Mori_Hashira4
 * Description: Forest Temple gates and rotating pillars
 */

#include "z_bg_mori_hashira4.h"
#include "assets/objects/object_mori_objects/object_mori_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Mori_Hashira4_actor_ct(Actor* thisx, PlayState* play);
void Bg_Mori_Hashira4_actor_dt(Actor* thisx, PlayState* play);
void Bg_Mori_Hashira4_actor_move(Actor* thisx, PlayState* play);
void Bg_Mori_Hashira4_actor_draw(Actor* thisx, PlayState* play);

static void mvSet_dmaWait(BgMoriHashira4* this);
static void mv_dmaWait(BgMoriHashira4* this, PlayState* play);
static void mvSet_stop(BgMoriHashira4* this);
static void mv_stop(BgMoriHashira4* this, PlayState* play);
void mv_ori_wait(BgMoriHashira4* this, PlayState* play);
void mv_ori_move(BgMoriHashira4* this, PlayState* play);

ActorProfile Bg_Mori_Hashira4_Profile = {
    /**/ ACTOR_BG_MORI_HASHIRA4,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_MORI_OBJECTS,
    /**/ sizeof(BgMoriHashira4),
    /**/ Bg_Mori_Hashira4_actor_ct,
    /**/ Bg_Mori_Hashira4_actor_dt,
    /**/ Bg_Mori_Hashira4_actor_move,
    /**/ NULL,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 700, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

static Gfx* shape_model[] = { gMoriHashiraPlatformsDL, gMoriHashiraGateDL };

static s16 button_count; // seems to be unused

static void set_moveProc(BgMoriHashira4* this, BgMoriHashira4ActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

static void set_dynaPoly(BgMoriHashira4* this, PlayState* play, CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, moveFlag);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        // "Warning : move BG login failed"
        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_mori_hashira4.c", 155,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

void Bg_Mori_Hashira4_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriHashira4* this = (BgMoriHashira4*)thisx;

    this->switchFlag = PARAMS_GET_U(this->dyna.actor.params, 8, 6);
    this->dyna.actor.params &= 0xFF;

    if (this->dyna.actor.params == 0) {
        set_dynaPoly(this, play, &gMoriHashira1Col, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
    } else {
        set_dynaPoly(this, play, &gMoriHashira2Col, 0);
    }
    ValueSet_process(&this->dyna.actor, value_init);
    this->moriTexObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_MORI_TEX);
    if (this->moriTexObjectSlot < 0) {
        Actor_delete(&this->dyna.actor);
        // "Bank danger!"
        PRINTF("Error : バンク危険！(arg_data 0x%04x)(%s %d)\n", this->dyna.actor.params, "../z_bg_mori_hashira4.c",
               196);
        return;
    }
    if ((this->dyna.actor.params != 0) && Actor_Environment_sw_Check(play, this->switchFlag)) {
        Actor_delete(&this->dyna.actor);
        return;
    }
    Actor_world_to_eye(&this->dyna.actor, 50.0f);
    mvSet_dmaWait(this);
    // "(4 pillars of the Forest Temple) Bank danger"
    PRINTF("(森の神殿 ４本柱)(arg_data 0x%04x)\n", this->dyna.actor.params);
    button_count = 0;
}

void Bg_Mori_Hashira4_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriHashira4* this = (BgMoriHashira4*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mvSet_dmaWait(BgMoriHashira4* this) {
    set_moveProc(this, mv_dmaWait);
}

static void mv_dmaWait(BgMoriHashira4* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->moriTexObjectSlot)) {
        this->gateTimer = 0;
        if (this->dyna.actor.params == 0) {
            mvSet_stop(this);
        } else {
            set_moveProc(this, mv_ori_wait);
        }
        this->dyna.actor.draw = Bg_Mori_Hashira4_actor_draw;
    }
}

static void mvSet_stop(BgMoriHashira4* this) {
    set_moveProc(this, mv_stop);
}

static void mv_stop(BgMoriHashira4* this, PlayState* play) {
    this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y += 0x96;
    Actor_SE_set(&this->dyna.actor, NA_SE_EV_ROLL_STAND_2 - SFX_FLAG);
}

void mv_ori_wait(BgMoriHashira4* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, this->switchFlag) || (this->gateTimer != 0)) {
        this->gateTimer++;
        if (this->gateTimer > 30) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_OPEN);
            set_moveProc(this, mv_ori_move);
            makeOnepointDemo(play, 6010, 20, &this->dyna.actor, CAM_ID_MAIN);
            button_count++;
        }
    }
}

void mv_ori_move(BgMoriHashira4* this, PlayState* play) {
    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 120.0f, 10.0f)) {
        Actor_delete(&this->dyna.actor);
    }
}

void Bg_Mori_Hashira4_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriHashira4* this = (BgMoriHashira4*)thisx;

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void Bg_Mori_Hashira4_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriHashira4* this = (BgMoriHashira4*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_mori_hashira4.c", 339);
    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, play->objectCtx.slots[this->moriTexObjectSlot].segment);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_mori_hashira4.c", 344);

    gSPDisplayList(POLY_OPA_DISP++, shape_model[this->dyna.actor.params]);
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_mori_hashira4.c", 348);
}
