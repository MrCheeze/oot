/*
 * File: z_bg_spot05_soko.c
 * Overlay: ovl_Bg_Spot05_Soko
 * Description: Sacred Forest Meadow Entities
 */

#include "z_bg_spot05_soko.h"
#include "assets/objects/object_spot05_objects/object_spot05_objects.h"

#define FLAGS 0

void Bg_Spot05_Soko_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot05_Soko_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot05_Soko_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot05_Soko_actor_draw(Actor* thisx, PlayState* play);
static void mode_stop(BgSpot05Soko* this, PlayState* play);
static void mode_wait(BgSpot05Soko* this, PlayState* play);
void mode_open(BgSpot05Soko* this, PlayState* play);

ActorProfile Bg_Spot05_Soko_Profile = {
    /**/ ACTOR_BG_SPOT05_SOKO,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_SPOT05_OBJECTS,
    /**/ sizeof(BgSpot05Soko),
    /**/ Bg_Spot05_Soko_actor_ct,
    /**/ Bg_Spot05_Soko_actor_dt,
    /**/ Bg_Spot05_Soko_actor_move,
    /**/ Bg_Spot05_Soko_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static Gfx* shape_model[] = {
    object_spot05_objects_DL_000840,
    object_spot05_objects_DL_001190,
};

void Bg_Spot05_Soko_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad1;
    BgSpot05Soko* this = (BgSpot05Soko*)thisx;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    ValueSet_process(thisx, value_init);
    this->switchFlag = PARAMS_GET_U(thisx->params, 8, 8);
    thisx->params &= 0xFF;
    MoveBG_ct(&this->dyna, 0);
    if (thisx->params == 0) {
        DynaPolyUty_bgdi_SG2KSG(&object_spot05_objects_Col_000918, &colHeader);
        if (LINK_IS_ADULT) {
            Actor_delete(thisx);
        } else {
            this->actionFunc = mode_stop;
        }
    } else {
        DynaPolyUty_bgdi_SG2KSG(&object_spot05_objects_Col_0012C0, &colHeader);
        if (Actor_Environment_sw_Check(play, this->switchFlag) != 0) {
            Actor_delete(thisx);
        } else {
            this->actionFunc = mode_wait;
            thisx->flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        }
    }
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
}

void Bg_Spot05_Soko_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot05Soko* this = (BgSpot05Soko*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mode_stop(BgSpot05Soko* this, PlayState* play) {
}

static void mode_wait(BgSpot05Soko* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, this->switchFlag)) {
        Effect_SE_Info_new(play, &this->dyna.actor.world.pos, 30, NA_SE_EV_METALDOOR_CLOSE);
        Actor_world_to_eye(&this->dyna.actor, 50.0f);
        makeActorAttentionDemo(play, &this->dyna.actor);
        this->actionFunc = mode_open;
        this->dyna.actor.speed = 0.5f;
    }
}

void mode_open(BgSpot05Soko* this, PlayState* play) {
    this->dyna.actor.speed *= 1.5f;
    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 120.0f, this->dyna.actor.speed) !=
        0) {
        Actor_delete(&this->dyna.actor);
    }
}

void Bg_Spot05_Soko_actor_move(Actor* thisx, PlayState* play) {
    BgSpot05Soko* this = (BgSpot05Soko*)thisx;

    this->actionFunc(this, play);
}

void Bg_Spot05_Soko_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, shape_model[thisx->params]);
}
