/*
 * File: z_bg_ddan_jd.c
 * Overlay: ovl_Bg_Ddan_Jd
 * Description: Rising stone platform (Dodongo's Cavern)
 */

#include "z_bg_ddan_jd.h"

#include "ichain.h"
#include "one_point_cutscene.h"
#include "rand.h"
#include "sfx.h"
#include "z_lib.h"
#include "z64play.h"

#include "assets/objects/object_ddan_objects/object_ddan_objects.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Ddan_Jd_actor_ct(Actor* thisx, PlayState* play);
void Bg_Ddan_Jd_actor_dt(Actor* thisx, PlayState* play);
void Bg_Ddan_Jd_actor_move(Actor* thisx, PlayState* play);
void Bg_Ddan_Jd_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgDdanJd* this, PlayState* play);
static void mode_move(BgDdanJd* this, PlayState* play);

ActorProfile Bg_Ddan_Jd_Profile = {
    /**/ ACTOR_BG_DDAN_JD,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_DDAN_OBJECTS,
    /**/ sizeof(BgDdanJd),
    /**/ Bg_Ddan_Jd_actor_ct,
    /**/ Bg_Ddan_Jd_actor_dt,
    /**/ Bg_Ddan_Jd_actor_move,
    /**/ Bg_Ddan_Jd_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

typedef enum BgDdanJdState {
    /* 0 */ STATE_GO_BOTTOM,
    /* 1 */ STATE_GO_MIDDLE_FROM_BOTTOM,
    /* 2 */ STATE_GO_MIDDLE_FROM_TOP,
    /* 3 */ STATE_GO_TOP
} BgDdanJdState;

#define MOVE_HEIGHT_MIDDLE 140.0f
#define MOVE_HEIGHT_TOP 700.0f

#define IDLE_FRAMES 100

// Since ySpeed is used to determine if the platform should rise to the top of the dungeon, these must be assigned
// different values in order for the shortcut to work correctly
#define DEFAULT_Y_SPEED 1
#define SHORTCUT_Y_SPEED 5

void Bg_Ddan_Jd_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgDdanJd* this = (BgDdanJd*)thisx;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyUty_bgdi_SG2KSG(&gDodongoRisingPlatformCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->idleTimer = IDLE_FRAMES;
    this->state = STATE_GO_BOTTOM;

    // Missing check for actor.params < 0x40. This will cause inconsistent behavior if params >= 0x40 and the bound
    // switch state is turned on while in the same room, as the shortcut behavior won't become enabled until the actor
    // is reloaded.
    if (Actor_Environment_sw_Check(play, this->dyna.actor.params)) {
        this->ySpeed = SHORTCUT_Y_SPEED;
    } else {
        this->ySpeed = DEFAULT_Y_SPEED;
    }
    this->actionFunc = mode_wait;
}

void Bg_Ddan_Jd_actor_dt(Actor* thisx, PlayState* play) {
    BgDdanJd* this = (BgDdanJd*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mode_wait(BgDdanJd* this, PlayState* play) {
    if (this->idleTimer != 0) {
        this->idleTimer--;
    }

    // if this is the platform that rises all the way to the top, and the switch state has just changed to on
    if (this->ySpeed == DEFAULT_Y_SPEED && this->dyna.actor.params < 0x40 &&
        Actor_Environment_sw_Check(play, this->dyna.actor.params)) {
        this->ySpeed = SHORTCUT_Y_SPEED;
        this->state = STATE_GO_MIDDLE_FROM_BOTTOM;
        this->idleTimer = 0;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + MOVE_HEIGHT_MIDDLE;
        makeOnepointDemo(play, 3060, -99, &this->dyna.actor, CAM_ID_MAIN);
    }
    if (this->idleTimer == 0) {
        this->idleTimer = IDLE_FRAMES;
        if (this->state == STATE_GO_BOTTOM) {
            this->state = STATE_GO_MIDDLE_FROM_BOTTOM;
            this->targetY = this->dyna.actor.home.pos.y + MOVE_HEIGHT_MIDDLE;
        } else if (this->state == STATE_GO_MIDDLE_FROM_BOTTOM) {
            // If the platform has been activated as a shortcut
            if (this->ySpeed != DEFAULT_Y_SPEED) {
                this->state = STATE_GO_TOP;
                this->targetY = this->dyna.actor.home.pos.y + MOVE_HEIGHT_TOP;
            } else {
                this->state = STATE_GO_BOTTOM;
                this->targetY = this->dyna.actor.home.pos.y;
            }
        } else if (this->state == STATE_GO_MIDDLE_FROM_TOP) {
            // If the platform has been activated as a shortcut
            if (this->ySpeed != DEFAULT_Y_SPEED) {
                this->state = STATE_GO_TOP;
                this->targetY = this->dyna.actor.home.pos.y + MOVE_HEIGHT_TOP;
            } else {
                this->state = STATE_GO_BOTTOM;
                this->targetY = this->dyna.actor.home.pos.y;
            }
        } else if (this->state == STATE_GO_TOP) {
            this->state = STATE_GO_MIDDLE_FROM_TOP;
            this->targetY = this->dyna.actor.home.pos.y + MOVE_HEIGHT_MIDDLE;
        }
        this->actionFunc = mode_move;
    }
}

// Handles dust particles and sfx when moving
void bg_ddan_jd_set_dust(BgDdanJd* this, PlayState* play) {
    Vec3f dustPos;

    // Generate random dust particles at the platform's base.
    dustPos.y = this->dyna.actor.home.pos.y;
    if (play->gameplayFrames & 1) {
        dustPos.x = this->dyna.actor.world.pos.x + 65.0f;
        dustPos.z = rnd_fx(110.0f) + this->dyna.actor.world.pos.z;
        dust_fly_set2(play, &dustPos, 5.0f, 1, 20, 60, 1);
        dustPos.x = this->dyna.actor.world.pos.x - 65.0f;
        dustPos.z = rnd_fx(110.0f) + this->dyna.actor.world.pos.z;
        dust_fly_set2(play, &dustPos, 5.0f, 1, 20, 60, 1);
    } else {
        dustPos.x = rnd_fx(110.0f) + this->dyna.actor.world.pos.x;
        dustPos.z = this->dyna.actor.world.pos.z + 65.0f;
        dust_fly_set2(play, &dustPos, 5.0f, 1, 20, 60, 1);
        dustPos.x = rnd_fx(110.0f) + this->dyna.actor.world.pos.x;
        dustPos.z = this->dyna.actor.world.pos.z - 65.0f;
        dust_fly_set2(play, &dustPos, 5.0f, 1, 20, 60, 1);
    }
    if (this->ySpeed == SHORTCUT_Y_SPEED) {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
    }
}

// Implements the platform's movement state
static void mode_move(BgDdanJd* this, PlayState* play) {
    // if this is the platform that rises all the way to the top, and the switch state has just changed to on
    if (this->ySpeed == DEFAULT_Y_SPEED && this->dyna.actor.params < 0x40 &&
        Actor_Environment_sw_Check(play, this->dyna.actor.params)) {
        this->ySpeed = SHORTCUT_Y_SPEED;
        this->state = STATE_GO_MIDDLE_FROM_BOTTOM;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + MOVE_HEIGHT_MIDDLE;
        this->idleTimer = 0;
        this->actionFunc = mode_wait;
        makeOnepointDemo(play, 3060, -99, &this->dyna.actor, CAM_ID_MAIN);
    } else if (chase_f(&this->dyna.actor.world.pos.y, this->targetY, this->ySpeed)) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_PILLAR_MOVE_STOP);
        this->actionFunc = mode_wait;
    }
    bg_ddan_jd_set_dust(this, play);
}

void Bg_Ddan_Jd_actor_move(Actor* thisx, PlayState* play) {
    BgDdanJd* this = (BgDdanJd*)thisx;

    this->actionFunc(this, play);
}

void Bg_Ddan_Jd_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gDodongoRisingPlatformDL);
}
