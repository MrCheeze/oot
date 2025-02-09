/*
 * File: z_bg_spot18_shutter.c
 * Overlay: Bg_Spot18_Shutter
 * Description:
 */

#include "z_bg_spot18_shutter.h"
#include "assets/objects/object_spot18_obj/object_spot18_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Spot18_Shutter_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot18_Shutter_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot18_Shutter_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot18_Shutter_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgSpot18Shutter* this, PlayState* play);
static void mode_door_wait(BgSpot18Shutter* this, PlayState* play);
void mode_door_wait2(BgSpot18Shutter* this, PlayState* play);
static void mode_door_up(BgSpot18Shutter* this, PlayState* play);
void mode_door_slide(BgSpot18Shutter* this, PlayState* play);

ActorProfile Bg_Spot18_Shutter_Profile = {
    /**/ ACTOR_BG_SPOT18_SHUTTER,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_SPOT18_OBJ,
    /**/ sizeof(BgSpot18Shutter),
    /**/ Bg_Spot18_Shutter_actor_ct,
    /**/ Bg_Spot18_Shutter_actor_dt,
    /**/ Bg_Spot18_Shutter_actor_move,
    /**/ Bg_Spot18_Shutter_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Spot18_Shutter_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgSpot18Shutter* this = (BgSpot18Shutter*)thisx;
    s32 param = PARAMS_GET_U(this->dyna.actor.params, 8, 1);
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, 0);
    ValueSet_process(&this->dyna.actor, value_init);

    if (param == 0) {
        if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
            if (GET_INFTABLE(INFTABLE_109)) {
                this->actionFunc = mode_wait;
                this->dyna.actor.world.pos.y += 180.0f;
            } else {
                this->actionFunc = mode_door_wait2;
            }
        } else {
            if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
                this->actionFunc = mode_wait;
                this->dyna.actor.world.pos.y += 180.0f;
            } else {
                this->actionFunc = mode_door_wait;
            }
        }
    } else {
        if (GET_INFTABLE(INFTABLE_109)) {
            this->dyna.actor.world.pos.x += 125.0f * cos_s(this->dyna.actor.world.rot.y);
            this->dyna.actor.world.pos.z -= 125.0f * sin_s(this->dyna.actor.world.rot.y);
            this->actionFunc = mode_wait;
        } else {
            this->actionFunc = mode_door_wait2;
        }
    }

    DynaPolyUty_bgdi_SG2KSG(&gGoronCityDoorCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
}

void Bg_Spot18_Shutter_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot18Shutter* this = (BgSpot18Shutter*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mode_wait(BgSpot18Shutter* this, PlayState* play) {
}

static void mode_door_wait(BgSpot18Shutter* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
        Actor_world_to_eye(&this->dyna.actor, 70.0f);
        makeActorAttentionDemo(play, &this->dyna.actor);
        this->actionFunc = mode_door_up;
    }
}

void mode_door_wait2(BgSpot18Shutter* this, PlayState* play) {
    if (GET_INFTABLE(INFTABLE_109)) {
        Actor_world_to_eye(&this->dyna.actor, 70.0f);
        if (PARAMS_GET_U(this->dyna.actor.params, 8, 1) == 0) {
            this->actionFunc = mode_door_up;
        } else {
            this->actionFunc = mode_door_slide;
            makeOnepointDemo(play, 4221, 140, &this->dyna.actor, CAM_ID_MAIN);
        }
    }
}

static void mode_door_up(BgSpot18Shutter* this, PlayState* play) {
    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 180.0f, 1.44f)) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_STONEDOOR_STOP);
        this->actionFunc = mode_wait;
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_STONE_STATUE_OPEN - SFX_FLAG);
    }
}

void mode_door_slide(BgSpot18Shutter* this, PlayState* play) {
    f32 sin = sin_s(this->dyna.actor.world.rot.y);
    f32 cos = cos_s(this->dyna.actor.world.rot.y);
    s32 flag = true;

    flag &= chase_f(&this->dyna.actor.world.pos.x, this->dyna.actor.home.pos.x + (125.0f * cos), fabsf(cos));
    flag &= chase_f(&this->dyna.actor.world.pos.z, this->dyna.actor.home.pos.z - (125.0f * sin), fabsf(sin));

    if (flag) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_STONEDOOR_STOP);
        this->actionFunc = mode_wait;
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_STONE_STATUE_OPEN - SFX_FLAG);
    }
}

void Bg_Spot18_Shutter_actor_move(Actor* thisx, PlayState* play) {
    BgSpot18Shutter* this = (BgSpot18Shutter*)thisx;

    this->actionFunc(this, play);
}

void Bg_Spot18_Shutter_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gGoronCityDoorDL);
}
