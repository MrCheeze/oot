/*
 * File: z_bg_bdan_objects.c
 * Overlay: ovl_Bg_Bdan_Objects
 * Description: Lord Jabu-Jabu Objects
 */

#include "z_bg_bdan_objects.h"

#include "ichain.h"
#include "one_point_cutscene.h"
#include "quake.h"
#include "rumble.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64audio.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/object_bdan_objects/object_bdan_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef enum BgBdanObjectsPropertyGetter {
    JABU_OBJECTS_GET_PROP_CAM_SETTING_NORMAL0 = 0,
    JABU_OBJECTS_GET_PROP_CAM_SETTING_DUNGEON1 = 3,
    JABU_OBJECTS_GET_PROP_WATCHED_BIGOCTO_INTRO_CUTSCENE = 4
} BgBdanObjectsPropertyGetter;
typedef enum BgBdanObjectsPropertySetter {
    JABU_OBJECTS_SET_PROP_CAM_SETTING_NORMAL1 = 1,
    JABU_OBJECTS_SET_PROP_CAM_SETTING_DUNGEON0 = 2,
    JABU_OBJECTS_SET_PROP_WATCHED_BIGOCTO_INTRO_CUTSCENE = 4
} BgBdanObjectsPropertySetter;

void Bg_Bdan_Objects_actor_ct(Actor* thisx, PlayState* play);
void Bg_Bdan_Objects_actor_dt(Actor* thisx, PlayState* play);
void Bg_Bdan_Objects_actor_move(Actor* thisx, PlayState* play);
void Bg_Bdan_Objects_actor_draw(Actor* thisx, PlayState* play);

static void mode_event_wait(BgBdanObjects* this, PlayState* play);
void mode_event_up(BgBdanObjects* this, PlayState* play);
void mode_event_wait2(BgBdanObjects* this, PlayState* play);
void mode_event_door(BgBdanObjects* this, PlayState* play);
void mode_event_wait3(BgBdanObjects* this, PlayState* play);
static void mode_wait(BgBdanObjects* this, PlayState* play);
static void mode_after_down(BgBdanObjects* this, PlayState* play);
void mode_after_wait(BgBdanObjects* this, PlayState* play);
void mode_after_up(BgBdanObjects* this, PlayState* play);
static void mode_stop(BgBdanObjects* this, PlayState* play);
void mode_ere_updown(BgBdanObjects* this, PlayState* play);
static void mode_water_wait(BgBdanObjects* this, PlayState* play);
static void mode_water_move(BgBdanObjects* this, PlayState* play);
static void mode_water_stop(BgBdanObjects* this, PlayState* play);
void mode_lift_wait(BgBdanObjects* this, PlayState* play);
static void mode_lift_drop(BgBdanObjects* this, PlayState* play);

ActorProfile Bg_Bdan_Objects_Profile = {
    /**/ ACTOR_BG_BDAN_OBJECTS,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_BDAN_OBJECTS,
    /**/ sizeof(BgBdanObjects),
    /**/ Bg_Bdan_Objects_actor_ct,
    /**/ Bg_Bdan_Objects_actor_dt,
    /**/ Bg_Bdan_Objects_actor_move,
    /**/ Bg_Bdan_Objects_actor_draw,
};

static ColliderCylinderInit BdanTogeAtPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x04 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_HARD,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { 0x00BB, 0x0050, 0x0000, { 0 } },
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static Gfx* shape_model[] = {
    gJabuObjectsLargeRotatingSpikePlatformDL,
    gJabuElevatorPlatformDL,
    gJabuWaterDL,
    gJabuFallingPlatformDL,
};

s32 Bg_Bdan_Objects_Get_Contact_Ru1(BgBdanObjects* this, s32 arg1) {
    switch (arg1) {
        case JABU_OBJECTS_GET_PROP_CAM_SETTING_NORMAL0:
            return this->cameraSetting == CAM_SET_NORMAL0;
        case JABU_OBJECTS_GET_PROP_WATCHED_BIGOCTO_INTRO_CUTSCENE:
            return GET_INFTABLE(INFTABLE_146);
        case JABU_OBJECTS_GET_PROP_CAM_SETTING_DUNGEON1:
            return this->cameraSetting == CAM_SET_DUNGEON1;
        default:
            PRINTF("Bg_Bdan_Objects_Get_Contact_Ru1\nそんな受信モードは無い%d!!!!!!!!\n", arg1);
            return -1;
    }
}

void Bg_Bdan_Objects_Set_Contact_Ru1(BgBdanObjects* this, s32 arg1) {
    switch (arg1) {
        case JABU_OBJECTS_SET_PROP_CAM_SETTING_NORMAL1:
            this->cameraSetting = CAM_SET_NORMAL1;
            break;
        case JABU_OBJECTS_SET_PROP_CAM_SETTING_DUNGEON0:
            this->cameraSetting = CAM_SET_DUNGEON0;
            break;
        case JABU_OBJECTS_SET_PROP_WATCHED_BIGOCTO_INTRO_CUTSCENE:
            SET_INFTABLE(INFTABLE_146);
            break;
        default:
            PRINTF("Bg_Bdan_Objects_Set_Contact_Ru1\nそんな送信モードは無い%d!!!!!!!!\n", arg1);
    }
}

void Bg_Bdan_Objects_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgBdanObjects* this = (BgBdanObjects*)thisx;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    this->var.switchFlag = PARAMS_GET_U(thisx->params, 8, 6);
    thisx->params &= 0xFF;
    if (thisx->params == JABU_OBJECTS_TYPE_WATERBOX_HEIGHT_CHANGER) {
        thisx->flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED;
        play->colCtx.colHeader->waterBoxes[7].ySurface = thisx->world.pos.y;
        this->actionFunc = mode_water_wait;
        return;
    }
    if (thisx->params == JABU_OBJECTS_TYPE_BIG_OCTO_PLATFORM) {
        DynaPolyUty_bgdi_SG2KSG(&gJabuBigOctoPlatformCol, &colHeader);
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, &this->dyna.actor, &BdanTogeAtPipeData);
        thisx->world.pos.y += -79.0f;
        if (Actor_Environment_room_clear_Check(play, thisx->room)) {
            Actor_Environment_sw_On(play, this->var.switchFlag);
            this->actionFunc = mode_after_down;
        } else {
            if (Bg_Bdan_Objects_Get_Contact_Ru1(this, JABU_OBJECTS_GET_PROP_WATCHED_BIGOCTO_INTRO_CUTSCENE)) {
                if (Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_EN_BIGOKUTA, thisx->home.pos.x,
                                       thisx->home.pos.y, thisx->home.pos.z, 0, thisx->shape.rot.y + 0x8000, 0,
                                       3) != NULL) {
                    thisx->child->world.pos.z = thisx->child->home.pos.z + 263.0f;
                }
                thisx->world.rot.y = 0;
                this->actionFunc = mode_wait;
                thisx->world.pos.y = thisx->home.pos.y + -70.0f;
            } else {
                Actor_Environment_sw_On(play, this->var.switchFlag);
                this->timer = 0;
                this->actionFunc = mode_event_wait;
            }
        }
    } else {
        if (thisx->params == JABU_OBJECTS_TYPE_SMALL_AUTO_ELEVATOR) {
            DynaPolyUty_bgdi_SG2KSG(&gJabuElevatorCol, &colHeader);
            this->timer = 512;
            this->var.camChangeTimer = 0;
            this->actionFunc = mode_ere_updown;
        } else { // JABU_OBJECTS_TYPE_FALLING_PLATFORM
            DynaPolyUty_bgdi_SG2KSG(&gJabuLoweringPlatformCol, &colHeader);
            if (Actor_Environment_sw_Check(play, this->var.switchFlag)) {
                this->actionFunc = mode_stop;
                thisx->world.pos.y = thisx->home.pos.y - 400.0f;
            } else {
                this->actionFunc = mode_lift_wait;
            }
        }
    }
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
}

void Bg_Bdan_Objects_actor_dt(Actor* thisx, PlayState* play) {
    BgBdanObjects* this = (BgBdanObjects*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    if (thisx->params == JABU_OBJECTS_TYPE_BIG_OCTO_PLATFORM) {
        ClObjPipe_dt(play, &this->collider);
    }
}

/**
 * This actor will never escape this actionFunc on its own. It relies on
 * En_Ru1 (Ruto) to change its cameraSetting to NORMAL0, which allows the
 * state machine to proceed.
 */
static void mode_event_wait(BgBdanObjects* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    if (Bg_Bdan_Objects_Get_Contact_Ru1(this, JABU_OBJECTS_GET_PROP_CAM_SETTING_NORMAL0)) {
        if (this->dyna.actor.xzDistToPlayer < 250.0f) {
            Bg_Bdan_Objects_Set_Contact_Ru1(this, JABU_OBJECTS_SET_PROP_CAM_SETTING_NORMAL1);
            this->timer = 20;
            makeOnepointDemo(play, 3070, -99, &this->dyna.actor, CAM_ID_MAIN);
            player->actor.world.pos.x = -1130.0f;
            player->actor.world.pos.y = -1025.0f;
            player->actor.world.pos.z = -3300.0f;
            z_vibctl2_vib_setQ(0.0f, 255, 20, 150);
        }
    } else if (this->timer != 0) {
        if (this->timer != 0) {
            this->timer--;
        }
        if (this->timer == 0) {
            this->actionFunc = mode_event_up;
        }
    }

    if (!Game_play_demo_mode_check(play) && !Bg_Bdan_Objects_Get_Contact_Ru1(this, JABU_OBJECTS_GET_PROP_CAM_SETTING_NORMAL0)) {
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + -79.0f;
    } else {
        this->dyna.actor.world.pos.y = (this->dyna.actor.home.pos.y + -79.0f) - 5.0f;
    }
}

void mode_event_up(BgBdanObjects* this, PlayState* play) {
    if (add_calc(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 500.0f, 0.5f, 7.5f, 1.0f) <
        0.1f) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_BUYOSTAND_STOP_A);
        this->actionFunc = mode_event_wait2;
        this->timer = 30;
        Bg_Bdan_Objects_Set_Contact_Ru1(this, JABU_OBJECTS_SET_PROP_CAM_SETTING_DUNGEON0);
        z_vibctl2_vib_setQ(0.0f, 255, 20, 150);
    } else {
        if (this->timer != 0) {
            this->timer--;
        }
        if (this->timer == 0) {
            z_vibctl2_vib_setQ(0.0f, 120, 20, 10);
            this->timer = 11;
        }
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_BUYOSTAND_RISING - SFX_FLAG);
    }
}

/**
 * Again, this actionFunc is inescapable until En_Ru1 (Ruto) sets this
 * actor's cameraSetting to DUNGEON1.
 */
void mode_event_wait2(BgBdanObjects* this, PlayState* play) {
    s32 quakeIndex;

    if (this->timer != 0) {
        this->timer--;
        if (this->timer == 0) {
            quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_1);
            setSpeedQuake(quakeIndex, 0x3A98);
            setScaleQuake(quakeIndex, 0, 1, 250, 1);
            setTimerQuake(quakeIndex, 10);
        }
    }

    if (Bg_Bdan_Objects_Get_Contact_Ru1(this, JABU_OBJECTS_GET_PROP_CAM_SETTING_DUNGEON1)) {
        Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_EN_BIGOKUTA, this->dyna.actor.world.pos.x,
                           this->dyna.actor.world.pos.y + 140.0f, this->dyna.actor.world.pos.z, 0,
                           this->dyna.actor.shape.rot.y + 0x8000, 0, 0);
        Bg_Bdan_Objects_Set_Contact_Ru1(this, JABU_OBJECTS_SET_PROP_WATCHED_BIGOCTO_INTRO_CUTSCENE);
        this->timer = 10;
        this->actionFunc = mode_event_door;
        restartCameraStoped(GET_ACTIVE_CAM(play));
    }
}

void mode_event_down(BgBdanObjects* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->dyna.actor.velocity.y += 0.5f;
    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + -70.0f,
                     this->dyna.actor.velocity.y)) {
        this->dyna.actor.world.rot.y = 0;
        this->timer = 60;
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_BUYOSTAND_STOP_U);
        this->dyna.actor.child->world.pos.y = this->dyna.actor.world.pos.y + 140.0f;
        this->actionFunc = mode_event_wait3;
        makeOnepointDemo(play, 3080, -99, this->dyna.actor.child, CAM_ID_MAIN);
        player->actor.world.pos.x = -1130.0f;
        player->actor.world.pos.y = -1025.0f;
        player->actor.world.pos.z = -3500.0f;
        player->actor.shape.rot.y = 0x7530;
        player->actor.world.rot.y = player->actor.shape.rot.y;
        z_vibctl2_vib_setQ(0.0f, 255, 30, 150);
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_BUYOSTAND_FALL - SFX_FLAG);
        if (this->timer != 0) {
            this->timer--;
        }
        if (this->timer == 0) {
            z_vibctl2_vib_setQ(0.0f, 120, 20, 10);
            this->timer = 11;
        }
        if (this->dyna.actor.child != NULL) {
            this->dyna.actor.child->world.pos.y = this->dyna.actor.world.pos.y + 140.0f;
        }
    }
}

void mode_event_door(BgBdanObjects* this, PlayState* play) {
    this->timer--;

    if (this->timer == 0) {
        Actor_Environment_sw_Off(play, this->var.switchFlag);
    } else if (this->timer == -40) {
        this->timer = 0;
        this->actionFunc = mode_event_down;
    }
}

void mode_event_wait3(BgBdanObjects* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if ((this->timer == 0) && (this->dyna.actor.child != NULL)) {
        if (this->dyna.actor.child->params == 2) {
            this->actionFunc = mode_wait;
        } else if (this->dyna.actor.child->params == 0) {
            this->dyna.actor.child->params = 1;
        }
    }
}

static void mode_wait(BgBdanObjects* this, PlayState* play) {
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, &this->collider);
    CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    if (Actor_Environment_room_clear_Check(play, this->dyna.actor.room)) {
        Actor_Environment_sw_On(play, this->var.switchFlag);
        this->dyna.actor.home.rot.y = (s16)(this->dyna.actor.shape.rot.y + 0x2000) & 0xC000;
        this->actionFunc = mode_after_down;
    } else {
        this->dyna.actor.shape.rot.y += this->dyna.actor.world.rot.y;
        Na_SetMotorSe(&this->dyna.actor.projectedPos, NA_SE_EV_ROLL_STAND - SFX_FLAG,
                      ABS(this->dyna.actor.world.rot.y) / 512.0f);
    }
}

static void mode_after_down(BgBdanObjects* this, PlayState* play) {
    s32 cond = chase_angle(&this->dyna.actor.shape.rot.y, this->dyna.actor.home.rot.y, 0x200);

    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + -125.0f, 3.0f)) {
        if (cond) {
            this->actionFunc = mode_after_wait;
        }
    }
}

void mode_after_wait(BgBdanObjects* this, PlayState* play) {
    if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
        if (this->dyna.actor.xzDistToPlayer < 120.0f) {
            this->actionFunc = mode_after_up;
            makeOnepointDemo(play, 3090, -99, &this->dyna.actor, CAM_ID_MAIN);
        }
    }
}

void mode_after_up(BgBdanObjects* this, PlayState* play) {
    if (add_calc(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 965.0f, 0.5f, 15.0f, 0.2f) <
        0.01f) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_BUYOSTAND_STOP_A);
        this->actionFunc = mode_stop;
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_BUYOSTAND_RISING - SFX_FLAG);
    }
}

static void mode_stop(BgBdanObjects* this, PlayState* play) {
}

void mode_ere_updown(BgBdanObjects* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->var.camChangeTimer == 0) {
        if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
            this->cameraSetting = play->cameraPtrs[CAM_ID_MAIN]->setting;
            changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_NORMAL2);
            clearCameraFlag(play->cameraPtrs[CAM_ID_MAIN], CAM_STATE_CHECK_BG);
            this->var.camChangeTimer = 10;
        }
    } else {
        changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_NORMAL2);
        if (!MoveBG_checkRidePlayerStatus(&this->dyna)) {
            if (this->var.camChangeTimer != 0) {
                this->var.camChangeTimer--;
            }
        }
        if (this->var.camChangeTimer == 0) {
            if (1) {}
            changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], this->cameraSetting);
            setCameraFlag(play->cameraPtrs[CAM_ID_MAIN], CAM_STATE_CHECK_BG);
        }
    }
    this->dyna.actor.world.pos.y =
        this->dyna.actor.home.pos.y - (sinf(this->timer * (M_PI / 256.0f)) * 471.24f); // pi * 150
    if (this->timer == 0) {
        this->timer = 512;
    }
}

static void mode_water_wait(BgBdanObjects* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, this->var.switchFlag)) {
        this->timer = 100;
        this->actionFunc = mode_water_move;
    }
}

static void mode_water_move(BgBdanObjects* this, PlayState* play) {
    if (this->timer == 0) {
        if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 0.5f)) {
            Actor_Environment_sw_Off(play, this->var.switchFlag);
            this->actionFunc = mode_water_wait;
        }
        Actor_fix_level_SE_set(&this->dyna.actor, NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
    } else {
        if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 75.0f, 0.5f)) {
            this->actionFunc = mode_water_stop;
        }
        Actor_fix_level_SE_set(&this->dyna.actor, NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
    }
    play->colCtx.colHeader->waterBoxes[7].ySurface = this->dyna.actor.world.pos.y;
}

static void mode_water_stop(BgBdanObjects* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    Actor_timer_level_SE_set(&this->dyna.actor, this->timer); // play ticking sound effect
    if (this->timer == 0) {
        this->actionFunc = mode_water_move;
    }
}

void mode_lift_wait(BgBdanObjects* this, PlayState* play) {
    if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
        Actor_Environment_sw_On(play, this->var.switchFlag);
        this->timer = 50;
        this->actionFunc = mode_lift_drop;
        this->dyna.actor.home.pos.y -= 200.0f;
        makeOnepointDemo(play, 3100, 51, &this->dyna.actor, CAM_ID_MAIN);
    }
}

static void mode_lift_drop(BgBdanObjects* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - (cosf(this->timer * (M_PI / 50.0f)) * 200.0f);

    if (this->timer == 0) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_BUYOSTAND_STOP_U);
        this->actionFunc = mode_stop;
        // Using `CAM_ID_NONE` here defaults to the active camera
        Gama_play_copy_camera_position(play, CAM_ID_MAIN, CAM_ID_NONE);
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_BUYOSTAND_FALL - SFX_FLAG);
    }
}

void Bg_Bdan_Objects_actor_move(Actor* thisx, PlayState* play) {
    BgBdanObjects* this = (BgBdanObjects*)thisx;

    Actor_world_to_eye(thisx, 50.0f);
    this->actionFunc(this, play);
}

void Bg_Bdan_Objects_actor_draw(Actor* thisx, PlayState* play) {
    BgBdanObjects* this = (BgBdanObjects*)thisx;

    if (thisx->params == JABU_OBJECTS_TYPE_BIG_OCTO_PLATFORM) {
        if (this->actionFunc == mode_event_wait) {
            if (((thisx->home.pos.y + -79.0f) - 5.0f) < thisx->world.pos.y) {
                Matrix_translate(0.0f, -50.0f, 0.0f, MTXMODE_APPLY);
            }
        }
    }

    if (thisx->params == JABU_OBJECTS_TYPE_WATERBOX_HEIGHT_CHANGER) {
        Cheap_gfx_display_xlu(play, gJabuWaterDL);
    } else {
        Cheap_gfx_display(play, shape_model[thisx->params]);
    }
}
