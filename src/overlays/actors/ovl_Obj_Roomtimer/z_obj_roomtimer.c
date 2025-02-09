/*
 * File: z_obj_roomtimer.c
 * Overlay: ovl_Obj_Roomtimer
 * Description: Starts Timer 1 with a value specified in params
 */

#include "z_obj_roomtimer.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Obj_Roomtimer_actor_ct(Actor* thisx, PlayState* play);
void Obj_Roomtimer_actor_dt(Actor* thisx, PlayState* play);
void Obj_Roomtimer_actor_move(Actor* thisx, PlayState* play);

static void mode_wait(ObjRoomtimer* this, PlayState* play);
static void mode_check(ObjRoomtimer* this, PlayState* play);

ActorProfile Obj_Roomtimer_Profile = {
    /**/ ACTOR_OBJ_ROOMTIMER,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(ObjRoomtimer),
    /**/ Obj_Roomtimer_actor_ct,
    /**/ Obj_Roomtimer_actor_dt,
    /**/ Obj_Roomtimer_actor_move,
    /**/ NULL,
};

void Obj_Roomtimer_actor_ct(Actor* thisx, PlayState* play) {
    ObjRoomtimer* this = (ObjRoomtimer*)thisx;

    this->switchFlag = PARAMS_GET_U(this->actor.params, 10, 6);
    this->actor.params = PARAMS_GET_U(this->actor.params, 0, 10);

    if (this->actor.params != 0x3FF) {
        this->actor.params = CLAMP_MAX(this->actor.params, 600);
    }

    this->actionFunc = mode_wait;
}

void Obj_Roomtimer_actor_dt(Actor* thisx, PlayState* play) {
    ObjRoomtimer* this = (ObjRoomtimer*)thisx;

    if ((this->actor.params != 0x3FF) && (z_common_data.timerSeconds > 0)) {
        z_common_data.timerState = TIMER_STATE_STOP;
    }
}

static void mode_wait(ObjRoomtimer* this, PlayState* play) {
    if (this->actor.params != 0x3FF) {
        event_timer_set(this->actor.params);
    }

    Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
    this->actionFunc = mode_check;
}

static void mode_check(ObjRoomtimer* this, PlayState* play) {
    if (Actor_Environment_no_enemy_Check(play, this->actor.room)) {
        if (this->actor.params != 0x3FF) {
            z_common_data.timerState = TIMER_STATE_STOP;
        }
        Actor_Environment_room_clear_On(play, this->actor.room);
        Actor_Environment_sw_On(play, this->switchFlag);
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        Actor_delete(&this->actor);
        return;
    }

    if ((this->actor.params != 0x3FF) && (z_common_data.timerSeconds == 0)) {
        Nai_FxFlagEntry(NA_SE_OC_ABYSS, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        Game_play_down_restart(play);
        Actor_delete(&this->actor);
    }
}

void Obj_Roomtimer_actor_move(Actor* thisx, PlayState* play) {
    ObjRoomtimer* this = (ObjRoomtimer*)thisx;

    this->actionFunc(this, play);
}
