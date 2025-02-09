/*
 * File: z_bg_hidan_syoku.c
 * Overlay: ovl_Bg_Hidan_Syoku
 * Description: Stone Elevator in the Fire Temple
 */

#include "z_bg_hidan_syoku.h"
#include "assets/objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Hidan_Syoku_actor_ct(Actor* thisx, PlayState* play);
void Bg_Hidan_Syoku_actor_dt(Actor* thisx, PlayState* play);
void Bg_Hidan_Syoku_actor_move(Actor* thisx, PlayState* play);
void Bg_Hidan_Syoku_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgHidanSyoku* this, PlayState* play);
static void mode_up(BgHidanSyoku* this, PlayState* play);
static void mode_stop(BgHidanSyoku* this, PlayState* play);

ActorProfile Bg_Hidan_Syoku_Profile = {
    /**/ ACTOR_BG_HIDAN_SYOKU,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HIDAN_OBJECTS,
    /**/ sizeof(BgHidanSyoku),
    /**/ Bg_Hidan_Syoku_actor_ct,
    /**/ Bg_Hidan_Syoku_actor_dt,
    /**/ Bg_Hidan_Syoku_actor_move,
    /**/ Bg_Hidan_Syoku_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Hidan_Syoku_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHidanSyoku* this = (BgHidanSyoku*)thisx;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyUty_bgdi_SG2KSG(&gFireTempleFlareDancerPlatformCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->actionFunc = mode_wait;
    this->dyna.actor.home.pos.y += 540.0f;
}

void Bg_Hidan_Syoku_actor_dt(Actor* thisx, PlayState* play) {
    BgHidanSyoku* this = (BgHidanSyoku*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mode_stop_init(BgHidanSyoku* this) {
    this->timer = 60;
    Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
    this->actionFunc = mode_stop;
}

static void mode_wait(BgHidanSyoku* this, PlayState* play) {
    if (Actor_Environment_room_clear_Check(play, this->dyna.actor.room) && MoveBG_checkRidePlayerStatus(&this->dyna)) {
        this->timer = 140;
        this->actionFunc = mode_up;
    }
}

static void mode_up(BgHidanSyoku* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    this->dyna.actor.world.pos.y = (cosf(this->timer * (M_PI / 140)) * 540.0f) + this->dyna.actor.home.pos.y;
    if (this->timer == 0) {
        mode_stop_init(this);
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE3 - SFX_FLAG);
    }
}

static void mode_down(BgHidanSyoku* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - (cosf(this->timer * (M_PI / 140)) * 540.0f);
    if (this->timer == 0) {
        mode_stop_init(this);
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE3 - SFX_FLAG);
    }
}

static void mode_stop(BgHidanSyoku* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        this->timer = 140;
        if (this->dyna.actor.world.pos.y < this->dyna.actor.home.pos.y) {
            this->actionFunc = mode_up;
        } else {
            this->actionFunc = mode_down;
        }
    }
}

void Bg_Hidan_Syoku_actor_move(Actor* thisx, PlayState* play) {
    BgHidanSyoku* this = (BgHidanSyoku*)thisx;

    this->actionFunc(this, play);
    if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
        if (this->unk_168 == 0) {
            this->unk_168 = 3;
        }
        changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_ELEVATOR_PLATFORM);
    } else if (!MoveBG_checkRidePlayerStatus(&this->dyna)) {
        if (this->unk_168 != 0) {
            changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_DUNGEON0);
        }
        this->unk_168 = 0;
    }
}

void Bg_Hidan_Syoku_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gFireTempleFlareDancerPlatformDL);
}
