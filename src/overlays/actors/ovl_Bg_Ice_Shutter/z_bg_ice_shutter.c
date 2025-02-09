/*
 * File: z_bg_ice_shutter.c
 * Overlay: ovl_Bg_Ice_Shutter
 * Description: Vertical Ice Bars (Doors) in Ice Cavern
 */

#include "z_bg_ice_shutter.h"
#include "assets/objects/object_ice_objects/object_ice_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Ice_Shutter_actor_ct(Actor* thisx, PlayState* play);
void Bg_Ice_Shutter_actor_dt(Actor* thisx, PlayState* play);
void Bg_Ice_Shutter_actor_move(Actor* thisx, PlayState* play);
void Bg_Ice_Shutter_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgIceShutter* this, PlayState* play);
static void mode_wait2(BgIceShutter* this, PlayState* play);
static void mode_move(BgIceShutter* this, PlayState* play);

ActorProfile Bg_Ice_Shutter_Profile = {
    /**/ ACTOR_BG_ICE_SHUTTER,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_ICE_OBJECTS,
    /**/ sizeof(BgIceShutter),
    /**/ Bg_Ice_Shutter_actor_ct,
    /**/ Bg_Ice_Shutter_actor_dt,
    /**/ Bg_Ice_Shutter_actor_move,
    /**/ Bg_Ice_Shutter_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void ice_shutter_set_pos(BgIceShutter* this) {
    f32 sp24;

    sp24 = sin_s(this->dyna.actor.shape.rot.x) * this->dyna.actor.velocity.y;
    this->dyna.actor.world.pos.y =
        (cos_s(this->dyna.actor.shape.rot.x) * this->dyna.actor.velocity.y) + this->dyna.actor.home.pos.y;
    this->dyna.actor.world.pos.x = (sin_s(this->dyna.actor.shape.rot.y) * sp24) + this->dyna.actor.home.pos.x;
    this->dyna.actor.world.pos.z = (cos_s(this->dyna.actor.shape.rot.y) * sp24) + this->dyna.actor.home.pos.z;
}

void Bg_Ice_Shutter_actor_ct(Actor* thisx, PlayState* play) {
    BgIceShutter* this = (BgIceShutter*)thisx;
    f32 sp24;
    CollisionHeader* colHeader;
    s32 sp28;

    colHeader = NULL;
    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, 0);
    sp28 = PARAMS_GET_U(this->dyna.actor.params, 0, 8);
    this->dyna.actor.params = PARAMS_GET_U(this->dyna.actor.params, 8, 8);
    DynaPolyUty_bgdi_SG2KSG(&object_ice_objects_Col_002854, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    if (sp28 == 2) {
        this->dyna.actor.shape.rot.x = -0x4000;
    }

    if (sp28 != 1) {
        if (Actor_Environment_room_clear_Check(play, this->dyna.actor.room)) {
            Actor_delete(&this->dyna.actor);
        } else {
            this->actionFunc = mode_wait;
        }

    } else {
        if (Actor_Environment_sw_Check(play, this->dyna.actor.params)) {
            Actor_delete(&this->dyna.actor);
        } else {
            this->actionFunc = mode_wait2;
        }
    }

    if (sp28 == 2) {
        f32 temp_f6 = sin_s(this->dyna.actor.shape.rot.x) * 50.0f;

        this->dyna.actor.focus.pos.x =
            (sin_s(this->dyna.actor.shape.rot.y) * temp_f6) + this->dyna.actor.home.pos.x;
        this->dyna.actor.focus.pos.y = this->dyna.actor.home.pos.y;
        this->dyna.actor.focus.pos.z =
            this->dyna.actor.home.pos.z + (cos_s(this->dyna.actor.shape.rot.y) * temp_f6);
    } else {
        Actor_world_to_eye(&this->dyna.actor, 50.0f);
    }
}

void Bg_Ice_Shutter_actor_dt(Actor* thisx, PlayState* play) {
    BgIceShutter* this = (BgIceShutter*)thisx;
    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mode_wait(BgIceShutter* this, PlayState* play) {
    if (Actor_Environment_no_enemy_Check(play, this->dyna.actor.room)) {
        Actor_Environment_room_clear_On(play, this->dyna.actor.room);
        Effect_SE_Info_new(play, &this->dyna.actor.world.pos, 30, NA_SE_EV_SLIDE_DOOR_OPEN);
        this->actionFunc = mode_move;
        if (this->dyna.actor.shape.rot.x == 0) {
            makeActorAttentionDemo(play, &this->dyna.actor);
        }
    }
}

static void mode_wait2(BgIceShutter* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, this->dyna.actor.params)) {
        Effect_SE_Info_new(play, &this->dyna.actor.world.pos, 30, NA_SE_EV_SLIDE_DOOR_OPEN);
        this->actionFunc = mode_move;
        makeActorAttentionDemo(play, &this->dyna.actor);
    }
}

static void mode_move(BgIceShutter* this, PlayState* play) {
    chase_f(&this->dyna.actor.speed, 30.0f, 2.0f);
    if (chase_f(&this->dyna.actor.velocity.y, 210.0f, this->dyna.actor.speed)) {
        Actor_delete(&this->dyna.actor);
        return;
    }

    ice_shutter_set_pos(this);
}

void Bg_Ice_Shutter_actor_move(Actor* thisx, PlayState* play) {
    BgIceShutter* this = (BgIceShutter*)thisx;

    this->actionFunc(this, play);
}

void Bg_Ice_Shutter_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, object_ice_objects_DL_002740);
}
