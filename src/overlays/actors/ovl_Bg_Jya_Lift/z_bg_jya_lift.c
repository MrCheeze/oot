/*
 * File: z_bg_jya_lift.c
 * Overlay: ovl_Bg_Jya_Lift
 * Description: Chain Platform (Spirit Temple)
 */

#include "z_bg_jya_lift.h"
#include "assets/objects/object_jya_obj/object_jya_obj.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Jya_Lift_actor_ct(Actor* thisx, PlayState* play);
void Bg_Jya_Lift_actor_dt(Actor* thisx, PlayState* play);
void Bg_Jya_Lift_actor_move(Actor* thisx, PlayState* play2);
void Bg_Jya_Lift_actor_draw(Actor* thisx, PlayState* play);

static void mv_end_init(BgJyaLift* this);
static void mv_wait_init(BgJyaLift* this);
static void mv_wait(BgJyaLift* this, PlayState* play);
static void mv_down_init(BgJyaLift* this);
static void mv_down(BgJyaLift* this, PlayState* play);

static s16 JL_Exist_flag = false;

ActorProfile Bg_Jya_Lift_Profile = {
    /**/ ACTOR_BG_JYA_LIFT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_JYA_OBJ,
    /**/ sizeof(BgJyaLift),
    /**/ Bg_Jya_Lift_actor_ct,
    /**/ Bg_Jya_Lift_actor_dt,
    /**/ Bg_Jya_Lift_actor_move,
    /**/ Bg_Jya_Lift_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 2500, ICHAIN_STOP),
};

static void set_dynaPoly(BgJyaLift* this, PlayState* play, CollisionHeader* collisionHeader, s32 moveFlag) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, moveFlag);
    DynaPolyUty_bgdi_SG2KSG(collisionHeader, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
}

void Bg_Jya_Lift_actor_ct(Actor* thisx, PlayState* play) {
    BgJyaLift* this = (BgJyaLift*)thisx;

    this->isSpawned = false;
    if (JL_Exist_flag) {
        Actor_delete(thisx);
        return;
    }

    // "Goddess lift CT"
    PRINTF("女神リフト CT\n");
    set_dynaPoly(this, play, &gLiftCol, 0);
    ValueSet_process(thisx, value_init);
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(thisx->params, 0, 6))) {
        mv_end_init(this);
    } else {
        mv_wait_init(this);
    }
    thisx->room = -1;
    JL_Exist_flag = true;
    this->isSpawned = true;
}

void Bg_Jya_Lift_actor_dt(Actor* thisx, PlayState* play) {
    BgJyaLift* this = (BgJyaLift*)thisx;

    if (this->isSpawned) {

        // "Goddess Lift DT"
        PRINTF("女神リフト DT\n");
        JL_Exist_flag = false;
        DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

static void mv_wait_init(BgJyaLift* this) {
    this->actionFunc = mv_wait;
    this->dyna.actor.world.pos.y = 1613.0f;
    this->moveDelay = 0;
}

static void mv_wait(BgJyaLift* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6)) || (this->moveDelay > 0)) {
        this->moveDelay++;
        if (this->moveDelay >= 20) {
            makeOnepointDemo(play, 3430, -99, &this->dyna.actor, CAM_ID_MAIN);
            mv_down_init(this);
        }
    }
}

static void mv_down_init(BgJyaLift* this) {
    this->actionFunc = mv_down;
}

static void mv_down(BgJyaLift* this, PlayState* play) {
    f32 distFromBottom;
    f32 tempVelocity;

    add_calc(&this->dyna.actor.velocity.y, 4.0f, 0.1f, 1.0f, 0.0f);
    tempVelocity = (this->dyna.actor.velocity.y < 0.2f) ? 0.2f : this->dyna.actor.velocity.y;
    distFromBottom = add_calc(&this->dyna.actor.world.pos.y, 973.0f, 0.1f, tempVelocity, 0.2f);
    if ((this->dyna.actor.world.pos.y < 1440.0f) && (1440.0f <= this->dyna.actor.prevPos.y)) {
        restartCameraStoped(GET_ACTIVE_CAM(play));
    }
    if (fabsf(distFromBottom) < 0.001f) {
        mv_end_init(this);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_ELEVATOR_STOP);
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_BRIDGE_OPEN - SFX_FLAG);
    }
}

static void mv_end_init(BgJyaLift* this) {
    this->actionFunc = NULL;
    this->dyna.actor.world.pos.y = 973.0f;
}

void Bg_Jya_Lift_actor_move(Actor* thisx, PlayState* play2) {
    BgJyaLift* this = (BgJyaLift*)thisx;
    PlayState* play = play2;

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
    if ((this->dyna.interactFlags & DYNA_INTERACT_PLAYER_ABOVE) && !(this->unk_16B & DYNA_INTERACT_PLAYER_ABOVE)) {
        changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_DIRECTED_YAW);
    } else if (!(this->dyna.interactFlags & DYNA_INTERACT_PLAYER_ABOVE) &&
               (this->unk_16B & DYNA_INTERACT_PLAYER_ABOVE) &&
               (play->cameraPtrs[CAM_ID_MAIN]->setting == CAM_SET_DIRECTED_YAW)) {
        changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_DUNGEON0);
    }
    this->unk_16B = this->dyna.interactFlags;

    // Spirit Temple room 5 is the main room with the statue room 25 is directly above room 5
    if ((play->roomCtx.curRoom.num != 5) && (play->roomCtx.curRoom.num != 25)) {
        Actor_delete(thisx);
    }
}

void Bg_Jya_Lift_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gLiftDL);
}
