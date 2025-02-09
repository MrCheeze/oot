/*
 * File: z_bg_spot12_saku.c
 * Overlay: ovl_Bg_Spot12_Saku
 * Description:
 */

#include "z_bg_spot12_saku.h"
#include "assets/objects/object_spot12_obj/object_spot12_obj.h"

#define FLAGS 0

void Bg_Spot12_Saku_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot12_Saku_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot12_Saku_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot12_Saku_actor_draw(Actor* thisx, PlayState* play);

static void mv_wait_init(BgSpot12Saku* this);
static void mv_wait(BgSpot12Saku* this, PlayState* play);
static void mv_slide_init(BgSpot12Saku* this);
static void mv_slide(BgSpot12Saku* this, PlayState* play);
static void mv_end_init(BgSpot12Saku* this);
static void mv_end(BgSpot12Saku* this, PlayState* play);

ActorProfile Bg_Spot12_Saku_Profile = {
    /**/ ACTOR_BG_SPOT12_SAKU,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT12_OBJ,
    /**/ sizeof(BgSpot12Saku),
    /**/ Bg_Spot12_Saku_actor_ct,
    /**/ Bg_Spot12_Saku_actor_dt,
    /**/ Bg_Spot12_Saku_actor_move,
    /**/ Bg_Spot12_Saku_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

static void set_dynaPoly(BgSpot12Saku* this, PlayState* play, CollisionHeader* collision, s32 flags) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, flags);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_spot12_saku.c", 140,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

void Bg_Spot12_Saku_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot12Saku* this = (BgSpot12Saku*)thisx;

    set_dynaPoly(this, play, &gGerudoFortressGTGShutterCol, 0);
    ValueSet_process(&this->dyna.actor, value_init);
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
        mv_end_init(this);
    } else {
        mv_wait_init(this);
    }
}

void Bg_Spot12_Saku_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot12Saku* this = (BgSpot12Saku*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mv_wait_init(BgSpot12Saku* this) {
    this->actionFunc = mv_wait;
    this->dyna.actor.scale.x = 0.1f;
    this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x;
    this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z;
}

static void mv_wait(BgSpot12Saku* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
        mv_slide_init(this);
        this->timer = 20;
        makeOnepointDemo(play, 4170, -99, &this->dyna.actor, CAM_ID_MAIN);
    }
}

static void mv_slide_init(BgSpot12Saku* this) {
    if (this->timer == 0) {
        this->actionFunc = mv_slide;
    }
}

static void mv_slide(BgSpot12Saku* this, PlayState* play) {
    f32 temp_ret = add_calc(&this->dyna.actor.scale.x, 0.001f / 0.14f, 0.16f, 0.0022f, 0.001f);
    f32 temp_f18 = ((0.1f - this->dyna.actor.scale.x) * 840.0f);

    this->dyna.actor.world.pos.x =
        this->dyna.actor.home.pos.x - (sin_s(this->dyna.actor.shape.rot.y + 0x4000) * temp_f18);
    this->dyna.actor.world.pos.z =
        this->dyna.actor.home.pos.z - (cos_s(this->dyna.actor.shape.rot.y + 0x4000) * temp_f18);
    if (fabsf(temp_ret) < 0.0001f) {
        mv_end_init(this);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_BRIDGE_OPEN_STOP);
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_METALGATE_OPEN - SFX_FLAG);
    }
}

static void mv_end_init(BgSpot12Saku* this) {
    this->actionFunc = mv_end;
    this->dyna.actor.scale.x = 0.001f / 0.14f;
    this->dyna.actor.world.pos.x =
        this->dyna.actor.home.pos.x - (sin_s(this->dyna.actor.shape.rot.y + 0x4000) * 78.0f);
    this->dyna.actor.world.pos.z =
        this->dyna.actor.home.pos.z - (cos_s(this->dyna.actor.shape.rot.y + 0x4000) * 78.0f);
}

static void mv_end(BgSpot12Saku* this, PlayState* play) {
}

void Bg_Spot12_Saku_actor_move(Actor* thisx, PlayState* play) {
    BgSpot12Saku* this = (BgSpot12Saku*)thisx;

    if (this->timer > 0) {
        this->timer--;
    }
    this->actionFunc(this, play);
}

void Bg_Spot12_Saku_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gGerudoFortressGTGShutterDL);
}
