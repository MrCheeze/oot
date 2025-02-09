/*
 * File: z_bg_jya_kanaami.c
 * Overlay: ovl_Bg_Jya_Kanaami
 * Description: Climbable grating/bridge (Spirit Temple)
 */

#include "z_bg_jya_kanaami.h"
#include "assets/objects/object_jya_obj/object_jya_obj.h"
#include "quake.h"

#define FLAGS 0

void Bg_Jya_Kanaami_actor_ct(Actor* thisx, PlayState* play);
void Bg_Jya_Kanaami_actor_dt(Actor* thisx, PlayState* play);
void Bg_Jya_Kanaami_actor_move(Actor* thisx, PlayState* play);
void Bg_Jya_Kanaami_actor_draw(Actor* thisx, PlayState* play);

static void mv_stop_init(BgJyaKanaami* this);
static void mv_stop(BgJyaKanaami* this, PlayState* play);
void mv_fall_init(BgJyaKanaami* this);
void mv_fall(BgJyaKanaami* this, PlayState* play);
static void mv_end_init(BgJyaKanaami* this);

ActorProfile Bg_Jya_Kanaami_Profile = {
    /**/ ACTOR_BG_JYA_KANAAMI,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_JYA_OBJ,
    /**/ sizeof(BgJyaKanaami),
    /**/ Bg_Jya_Kanaami_actor_ct,
    /**/ Bg_Jya_Kanaami_actor_dt,
    /**/ Bg_Jya_Kanaami_actor_move,
    /**/ Bg_Jya_Kanaami_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 700, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

static void set_dynaPoly(BgJyaKanaami* this, PlayState* play, CollisionHeader* collision, s32 flag) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, flag);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_jya_kanaami.c", 145,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

void Bg_Jya_Kanaami_actor_ct(Actor* thisx, PlayState* play) {
    BgJyaKanaami* this = (BgJyaKanaami*)thisx;

    set_dynaPoly(this, play, &gKanaamiCol, 0);
    ValueSet_process(&this->dyna.actor, value_init);
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
        mv_end_init(this);
    } else {
        mv_stop_init(this);
    }
    PRINTF("(jya 金網)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void Bg_Jya_Kanaami_actor_dt(Actor* thisx, PlayState* play) {
    BgJyaKanaami* this = (BgJyaKanaami*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mv_stop_init(BgJyaKanaami* this) {
    this->actionFunc = mv_stop;
    this->unk_16A = 0;
}

static void mv_stop(BgJyaKanaami* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6)) || this->unk_16A > 0) {
        if (this->dyna.actor.world.pos.x > -1000.0f && this->unk_16A == 0) {
            makeOnepointDemo(play, 3450, -99, &this->dyna.actor, CAM_ID_MAIN);
        }
        this->unk_16A++;
        if (this->unk_16A >= 10) {
            mv_fall_init(this);
        }
    }
}

void mv_fall_init(BgJyaKanaami* this) {
    this->actionFunc = mv_fall;
    this->unk_168 = 0;
}

void mv_fall(BgJyaKanaami* this, PlayState* play) {
    s32 pad[2];
    s32 quakeIndex;

    this->unk_168 += 0x20;
    if (chase_angle(&this->dyna.actor.world.rot.x, 0x4000, this->unk_168)) {
        mv_end_init(this);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_TRAP_BOUND);

        quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
        setSpeedQuake(quakeIndex, 25000);
        setScaleQuake(quakeIndex, 2, 0, 0, 0);
        setTimerQuake(quakeIndex, 16);
    }
}

static void mv_end_init(BgJyaKanaami* this) {
    this->actionFunc = NULL;
    this->dyna.actor.world.rot.x = 0x4000;
}

void Bg_Jya_Kanaami_actor_move(Actor* thisx, PlayState* play) {
    BgJyaKanaami* this = (BgJyaKanaami*)thisx;

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
    this->dyna.actor.shape.rot.x = this->dyna.actor.world.rot.x;
}

void Bg_Jya_Kanaami_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gKanaamiDL);
}
