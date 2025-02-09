/*
 * File: z_bg_spot12_gate.c
 * Overlay: ovl_Bg_Spot12_Gate
 * Description: Haunted Wasteland Gate
 */

#include "z_bg_spot12_gate.h"
#include "assets/objects/object_spot12_obj/object_spot12_obj.h"
#include "quake.h"

#define FLAGS 0

void Bg_Spot12_Gate_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot12_Gate_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot12_Gate_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot12_Gate_actor_draw(Actor* thisx, PlayState* play);

static void mv_wait_init(BgSpot12Gate* this);
static void mv_wait(BgSpot12Gate* this, PlayState* play);
void mv_wait2_init(BgSpot12Gate* this);
void mv_wait2(BgSpot12Gate* this, PlayState* play);
static void mv_up_init(BgSpot12Gate* this);
static void mv_up(BgSpot12Gate* this, PlayState* play);
static void mv_end_init(BgSpot12Gate* this);
static void mv_end(BgSpot12Gate* this, PlayState* play);

ActorProfile Bg_Spot12_Gate_Profile = {
    /**/ ACTOR_BG_SPOT12_GATE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT12_OBJ,
    /**/ sizeof(BgSpot12Gate),
    /**/ Bg_Spot12_Gate_actor_ct,
    /**/ Bg_Spot12_Gate_actor_dt,
    /**/ Bg_Spot12_Gate_actor_move,
    /**/ Bg_Spot12_Gate_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 2500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1200, ICHAIN_STOP),
};

static void set_dynaPoly(BgSpot12Gate* this, PlayState* play, CollisionHeader* collision, s32 flags) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, flags);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_spot12_gate.c", 145,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

void Bg_Spot12_Gate_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot12Gate* this = (BgSpot12Gate*)thisx;

    set_dynaPoly(this, play, &gGerudoFortressWastelandGateCol, 0);
    ValueSet_process(&this->dyna.actor, value_init);

    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
        mv_end_init(this);
    } else {
        mv_wait_init(this);
    }
}

void Bg_Spot12_Gate_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot12Gate* this = (BgSpot12Gate*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mv_wait_init(BgSpot12Gate* this) {
    this->actionFunc = mv_wait;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
}

static void mv_wait(BgSpot12Gate* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
        mv_wait2_init(this);
        makeOnepointDemo(play, 4160, -99, &this->dyna.actor, CAM_ID_MAIN);
    }
}

void mv_wait2_init(BgSpot12Gate* this) {
    this->actionFunc = mv_wait2;
    this->unk_168 = 40;
}

void mv_wait2(BgSpot12Gate* this, PlayState* play) {
    if (this->unk_168 <= 0) {
        mv_up_init(this);
    }
}

static void mv_up_init(BgSpot12Gate* this) {
    this->actionFunc = mv_up;
}

static void mv_up(BgSpot12Gate* this, PlayState* play) {
    s32 pad;

    chase_f(&this->dyna.actor.velocity.y, 1.6f, 0.03f);
    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 200.0f,
                     this->dyna.actor.velocity.y)) {
        mv_end_init(this);

        {
            s32 quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);

            setSpeedQuake(quakeIndex, -0x3CB0);
            setScaleQuake(quakeIndex, 3, 0, 0, 0);
            setTimerQuake(quakeIndex, 12);
        }

        Actor_SE_set(&this->dyna.actor, NA_SE_EV_BRIDGE_OPEN_STOP);
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_METALGATE_OPEN - SFX_FLAG);
    }
}

static void mv_end_init(BgSpot12Gate* this) {
    this->actionFunc = mv_end;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 200.0f;
}

static void mv_end(BgSpot12Gate* this, PlayState* play) {
}

void Bg_Spot12_Gate_actor_move(Actor* thisx, PlayState* play) {
    BgSpot12Gate* this = (BgSpot12Gate*)thisx;

    if (this->unk_168 > 0) {
        this->unk_168--;
    }
    this->actionFunc(this, play);
}

void Bg_Spot12_Gate_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gGerudoFortressWastelandGateDL);
}
