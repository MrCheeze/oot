/*
 * File: z_obj_elevator.c
 * Overlay: Obj_Elevator
 * Description: Stone Elevator
 */

#include "z_obj_elevator.h"
#include "assets/objects/object_d_elevator/object_d_elevator.h"

#define FLAGS 0

void Obj_Elevator_actor_ct(Actor* thisx, PlayState* play);
void Obj_Elevator_actor_dt(Actor* thisx, PlayState* play);
void Obj_Elevator_actor_move(Actor* thisx, PlayState* play);
void Obj_Elevator_actor_draw(Actor* thisx, PlayState* play);

static void mv_stop_init(ObjElevator* this);
static void mv_stop(ObjElevator* this, PlayState* play);
static void mv_updown_init(ObjElevator* this);
static void mv_updown(ObjElevator* this, PlayState* play);

ActorProfile Obj_Elevator_Profile = {
    /**/ ACTOR_OBJ_ELEVATOR,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_D_ELEVATOR,
    /**/ sizeof(ObjElevator),
    /**/ Obj_Elevator_actor_ct,
    /**/ Obj_Elevator_actor_dt,
    /**/ Obj_Elevator_actor_move,
    /**/ Obj_Elevator_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 600, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 2000, ICHAIN_STOP),
};

static f32 scale_init[] = { 0.1f, 0.05f };

static void set_moveProc(ObjElevator* this, ObjElevatorActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

static void set_dynaPoly(ObjElevator* this, PlayState* play, CollisionHeader* collision, s32 flag) {
    s16 pad1;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, flag);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_obj_elevator.c", 136,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

void Obj_Elevator_actor_ct(Actor* thisx, PlayState* play) {
    ObjElevator* this = (ObjElevator*)thisx;
    f32 temp_f0;

    set_dynaPoly(this, play, &object_d_elevator_Col_000360, DYNA_TRANSFORM_POS);
    Actor_set_scale(thisx, scale_init[PARAMS_GET_U(thisx->params, 0, 1)]);
    ValueSet_process(thisx, value_init);
    temp_f0 = PARAMS_GET_U(thisx->params, 8, 4);
    this->unk_16C = temp_f0 + temp_f0;
    mv_stop_init(this);
    PRINTF("(Dungeon Elevator)(arg_data 0x%04x)\n", thisx->params);
}

void Obj_Elevator_actor_dt(Actor* thisx, PlayState* play) {
    ObjElevator* this = (ObjElevator*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mv_stop_init(ObjElevator* this) {
    set_moveProc(this, mv_stop);
}

static void mv_stop(ObjElevator* this, PlayState* play) {
    f32 sub;
    Actor* thisx = &this->dyna.actor;

    if ((this->dyna.interactFlags & DYNA_INTERACT_PLAYER_ON_TOP) && !(this->unk_170 & DYNA_INTERACT_PLAYER_ON_TOP)) {
        sub = thisx->world.pos.y - thisx->home.pos.y;
        if (fabsf(sub) < 0.1f) {
            this->unk_168 = thisx->home.pos.y + (PARAMS_GET_U(thisx->params, 12, 4)) * 80.0f;
        } else {
            this->unk_168 = thisx->home.pos.y;
        }
        mv_updown_init(this);
    }
}

static void mv_updown_init(ObjElevator* this) {
    set_moveProc(this, mv_updown);
}

static void mv_updown(ObjElevator* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    if (fabsf(add_calc(&thisx->world.pos.y, this->unk_168, 1.0f, this->unk_16C, 0.0f)) < 0.001f) {
        Actor_SE_set(thisx, NA_SE_EV_FOOT_SWITCH);
        mv_stop_init(this);
    } else {
        Actor_SE_set(thisx, NA_SE_EV_STONE_STATUE_OPEN - SFX_FLAG);
    }
}

void Obj_Elevator_actor_move(Actor* thisx, PlayState* play) {
    ObjElevator* this = (ObjElevator*)thisx;

    if (this->actionFunc) {
        this->actionFunc(this, play);
    }
    this->unk_170 = this->dyna.interactFlags;
}

void Obj_Elevator_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, object_d_elevator_DL_000180);
}
