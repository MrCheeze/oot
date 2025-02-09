/*
 * File: z_bg_jya_1flift.c
 * Overlay: ovl_Bg_Jya_1flift
 * Description: Shortcut Elevator used in the vanilla version of the Spirit Temple
 */

#include "z_bg_jya_1flift.h"
#include "assets/objects/object_jya_obj/object_jya_obj.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Jya_1flift_actor_ct(Actor* thisx, PlayState* play);
void Bg_Jya_1flift_actor_dt(Actor* thisx, PlayState* play);
void Bg_Jya_1flift_actor_move(Actor* thisx, PlayState* play2);
void Bg_Jya_1flift_actor_draw(Actor* thisx, PlayState* play);

static void mv_wait_init(BgJya1flift* this);
static void mv_wait(BgJya1flift* this, PlayState* play);
static void mv_stop(BgJya1flift* this, PlayState* play);
static void mv_updown_init(BgJya1flift* this);
static void mv_updown(BgJya1flift* this, PlayState* play);
static void mv_stop_init(BgJya1flift* this);
void mv_oneMorment_init(BgJya1flift* this);
void mv_oneMorment(BgJya1flift* this, PlayState* play);

static u8 J1FL_Make_flag = false;

ActorProfile Bg_Jya_1flift_Profile = {
    /**/ ACTOR_BG_JYA_1FLIFT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_JYA_OBJ,
    /**/ sizeof(BgJya1flift),
    /**/ Bg_Jya_1flift_actor_ct,
    /**/ Bg_Jya_1flift_actor_dt,
    /**/ Bg_Jya_1flift_actor_move,
    /**/ Bg_Jya_1flift_actor_draw,
};

static ColliderCylinderInit ClPipeDt_1flift = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 70, 80, -82, { 0, 0, 0 } },
};

static f32 J1FL_GoalPosY[] = { 443.0f, -50.0f };

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1200, ICHAIN_STOP),
};

static void set_dynaPoly(BgJya1flift* this, PlayState* play, CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, moveFlag);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        // "Warning : move BG login failed"
        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_jya_1flift.c", 179,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

void set_collision_1flift(Actor* thisx, PlayState* play) {
    BgJya1flift* this = (BgJya1flift*)thisx;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->dyna.actor, &ClPipeDt_1flift);
    this->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;
}

void Bg_Jya_1flift_actor_ct(Actor* thisx, PlayState* play) {
    BgJya1flift* this = (BgJya1flift*)thisx;
    // "1 F lift"
    PRINTF("(１Ｆリフト)(flag %d)(room %d)\n", J1FL_Make_flag, play->roomCtx.curRoom.num);
    this->hasInitialized = false;
    if (J1FL_Make_flag) {
        Actor_delete(thisx);
        return;
    }
    set_dynaPoly(this, play, &g1fliftCol, 0);
    ValueSet_process(thisx, value_init);
    set_collision_1flift(thisx, play);
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(thisx->params, 0, 6))) {
        LINK_AGE_IN_YEARS == YEARS_ADULT ? mv_updown_init(this) : mv_stop_init(this);
    } else {
        mv_wait_init(this);
    }
    thisx->room = -1;
    J1FL_Make_flag = true;
    this->hasInitialized = true;
}

void Bg_Jya_1flift_actor_dt(Actor* thisx, PlayState* play) {
    BgJya1flift* this = (BgJya1flift*)thisx;

    if (this->hasInitialized) {
        J1FL_Make_flag = false;
        ClObjPipe_dt(play, &this->collider);
        DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

static void mv_wait_init(BgJya1flift* this) {
    this->actionFunc = mv_wait;
    this->dyna.actor.world.pos.y = J1FL_GoalPosY[0];
}

static void mv_wait(BgJya1flift* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
        mv_updown_init(this);
    }
}

static void mv_stop_init(BgJya1flift* this) {
    this->actionFunc = mv_stop;
    this->dyna.actor.world.pos.y = J1FL_GoalPosY[0];
}

static void mv_stop(BgJya1flift* this, PlayState* play) {
}

static void mv_updown_init(BgJya1flift* this) {
    this->actionFunc = mv_updown;
    this->isMovingDown ^= true;
    this->dyna.actor.velocity.y = 0.0f;
}

static void mv_updown(BgJya1flift* this, PlayState* play) {
    f32 tempVelocity;

    chase_f(&this->dyna.actor.velocity.y, 6.0f, 0.4f);
    if (this->dyna.actor.velocity.y < 1.0f) {
        tempVelocity = 1.0f;
    } else {
        tempVelocity = this->dyna.actor.velocity.y;
    }
    if (fabsf(add_calc(&this->dyna.actor.world.pos.y, (J1FL_GoalPosY[this->isMovingDown]), 0.5f,
                                 tempVelocity, 1.0f)) < 0.001f) {
        this->dyna.actor.world.pos.y = J1FL_GoalPosY[this->isMovingDown];
        mv_oneMorment_init(this);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE3 - SFX_FLAG);
    }
}

void mv_oneMorment_init(BgJya1flift* this) {
    this->actionFunc = mv_oneMorment;
    this->moveDelay = 0;
}

void mv_oneMorment(BgJya1flift* this, PlayState* play) {
    this->moveDelay++;
    if (this->moveDelay >= 21) {
        mv_updown_init(this);
    }
}

void Bg_Jya_1flift_actor_move(Actor* thisx, PlayState* play2) {
    BgJya1flift* this = (BgJya1flift*)thisx;
    PlayState* play = play2;
    s32 tempIsRiding;

    // Room 0 is the first room and 6 is the room that the lift starts on
    if (play->roomCtx.curRoom.num == 6 || play->roomCtx.curRoom.num == 0) {
        this->actionFunc(this, play);
        tempIsRiding = MoveBG_checkRidePlayerStatus(&this->dyna) ? true : false;
        if ((this->actionFunc == mv_updown) || (this->actionFunc == mv_oneMorment)) {
            if (tempIsRiding) {
                changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_ELEVATOR_PLATFORM);
            } else if (!tempIsRiding && this->isLinkRiding) {
                changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_DUNGEON0);
            }
        }
        this->isLinkRiding = tempIsRiding;
        CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->collider);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    } else {
        Actor_delete(thisx);
    }
}

void Bg_Jya_1flift_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, g1fliftDL);
}
