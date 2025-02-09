/*
 * File: z_bg_hidan_fslift.c
 * Overlay: ovl_Bg_Hidan_Fslift
 * Description: Hookshot Elevator
 */

#include "z_bg_hidan_fslift.h"

#include "ichain.h"
#include "sfx.h"
#include "z_lib.h"
#include "z64play.h"

#include "assets/objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Hidan_Fslift_actor_ct(Actor* thisx, PlayState* play);
void Bg_Hidan_Fslift_actor_dt(Actor* thisx, PlayState* play);
void Bg_Hidan_Fslift_actor_move(Actor* thisx, PlayState* play);
void Bg_Hidan_Fslift_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgHidanFslift* this, PlayState* play);
static void mode_down(BgHidanFslift* this, PlayState* play);
static void mode_up(BgHidanFslift* this, PlayState* play);

ActorProfile Bg_Hidan_Fslift_Profile = {
    /**/ ACTOR_BG_HIDAN_FSLIFT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HIDAN_OBJECTS,
    /**/ sizeof(BgHidanFslift),
    /**/ Bg_Hidan_Fslift_actor_ct,
    /**/ Bg_Hidan_Fslift_actor_dt,
    /**/ Bg_Hidan_Fslift_actor_move,
    /**/ Bg_Hidan_Fslift_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 300, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 350, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_STOP),
};

void Bg_Hidan_Fslift_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad1;
    BgHidanFslift* this = (BgHidanFslift*)thisx;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyUty_bgdi_SG2KSG(&gFireTempleHookshotElevatorCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
    if (Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_OBJ_HSBLOCK, this->dyna.actor.world.pos.x,
                           this->dyna.actor.world.pos.y + 40.0f, this->dyna.actor.world.pos.z + -28.0f, 0, 0, 0,
                           2) == NULL) {
        Actor_delete(&this->dyna.actor);
        return;
    }
    this->actionFunc = mode_wait;
}

void hidan_fslift_set_fs_pos(BgHidanFslift* this) {
    if (this->dyna.actor.child != NULL && this->dyna.actor.child->update != NULL) {
        this->dyna.actor.child->world.pos.x = this->dyna.actor.world.pos.x;
        this->dyna.actor.child->world.pos.y = this->dyna.actor.world.pos.y + 40.0f;
        this->dyna.actor.child->world.pos.z = this->dyna.actor.world.pos.z + -28.0f;
    } else {
        this->dyna.actor.child = NULL;
    }
}

void Bg_Hidan_Fslift_actor_dt(Actor* thisx, PlayState* play) {
    BgHidanFslift* this = (BgHidanFslift*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mode_wait_init(BgHidanFslift* this) {
    this->timer = 40;
    this->actionFunc = mode_wait;
}

static void mode_wait(BgHidanFslift* this, PlayState* play) {
    s32 nearHomePos;

    if (this->timer) {
        this->timer--;
    }

    if (this->timer == 0) {
        nearHomePos = false;
        if ((this->dyna.actor.world.pos.y - this->dyna.actor.home.pos.y) < 0.5f) {
            nearHomePos = true;
        }
        if (MoveBG_checkOverPlayerStatus(&this->dyna) && nearHomePos) {
            this->actionFunc = mode_up;
        } else if (!nearHomePos) {
            this->actionFunc = mode_down;
        }
    }
}

static void mode_down(BgHidanFslift* this, PlayState* play) {
    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 4.0f)) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        mode_wait_init(this);
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE3 - SFX_FLAG);
    }
    hidan_fslift_set_fs_pos(this);
}

static void mode_up(BgHidanFslift* this, PlayState* play) {
    if (MoveBG_checkOverPlayerStatus(&this->dyna)) {
        if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 790.0f, 4.0f)) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
            mode_wait_init(this);
        } else {
            Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE3 - SFX_FLAG);
        }
    } else {
        mode_wait_init(this);
    }
    hidan_fslift_set_fs_pos(this);
}

void Bg_Hidan_Fslift_actor_move(Actor* thisx, PlayState* play) {
    BgHidanFslift* this = (BgHidanFslift*)thisx;

    this->actionFunc(this, play);
    if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
        if (this->cameraSetting == CAM_SET_NONE) {
            this->cameraSetting = CAM_SET_DUNGEON0;
        }
        changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_ELEVATOR_PLATFORM);
    } else if (!MoveBG_checkRidePlayerStatus(&this->dyna)) {
        if (this->cameraSetting != CAM_SET_NONE) {
            // Given the values that get set to `cameraSetting`, it seems likely that it was intended to be
            // passed to the function call below. But instead `CAM_SET_DUNGEON0` is used directly.
            changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_DUNGEON0);
        }
        this->cameraSetting = CAM_SET_NONE;
    }
}

void Bg_Hidan_Fslift_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gFireTempleHookshotElevatorDL);
}
