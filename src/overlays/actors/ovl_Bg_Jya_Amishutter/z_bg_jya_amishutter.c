/*
 * File: z_bg_jya_amishutter.c
 * Overlay: Bg_Jya_Amishutter
 * Description: Circular metal grate. Lifts up when you get close to it.
 */

#include "z_bg_jya_amishutter.h"
#include "assets/objects/object_jya_obj/object_jya_obj.h"

#define FLAGS 0

void Bg_Jya_Amishutter_actor_ct(Actor* thisx, PlayState* play);
void Bg_Jya_Amishutter_actor_dt(Actor* thisx, PlayState* play);
void Bg_Jya_Amishutter_actor_move(Actor* thisx, PlayState* play);
void Bg_Jya_Amishutter_actor_draw(Actor* thisx, PlayState* play);

void mv_close_init(BgJyaAmishutter* this);
void mv_close(BgJyaAmishutter* this);
static void mv_up_init(BgJyaAmishutter* this);
static void mv_up(BgJyaAmishutter* this);
void mv_open_init(BgJyaAmishutter* this);
void mv_open(BgJyaAmishutter* this);
static void mv_down_init(BgJyaAmishutter* this);
static void mv_down(BgJyaAmishutter* this);

ActorProfile Bg_Jya_Amishutter_Profile = {
    /**/ ACTOR_BG_JYA_AMISHUTTER,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_JYA_OBJ,
    /**/ sizeof(BgJyaAmishutter),
    /**/ Bg_Jya_Amishutter_actor_ct,
    /**/ Bg_Jya_Amishutter_actor_dt,
    /**/ Bg_Jya_Amishutter_actor_move,
    /**/ Bg_Jya_Amishutter_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

static void set_dynaPoly(BgJyaAmishutter* this, PlayState* play, CollisionHeader* collision, s32 flag) {
    s32 pad1;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, flag);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_jya_amishutter.c", 129,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

void Bg_Jya_Amishutter_actor_ct(Actor* thisx, PlayState* play) {
    BgJyaAmishutter* this = (BgJyaAmishutter*)thisx;

    set_dynaPoly(this, play, &gAmishutterCol, 0);
    ValueSet_process(&this->dyna.actor, value_init);
    mv_close_init(this);
}

void Bg_Jya_Amishutter_actor_dt(Actor* thisx, PlayState* play) {
    BgJyaAmishutter* this = (BgJyaAmishutter*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void mv_close_init(BgJyaAmishutter* this) {
    this->actionFunc = mv_close;
}

void mv_close(BgJyaAmishutter* this) {
    if ((this->dyna.actor.xzDistToPlayer < 60.0f) && (fabsf(this->dyna.actor.yDistToPlayer) < 30.0f)) {
        mv_up_init(this);
    }
}

static void mv_up_init(BgJyaAmishutter* this) {
    this->actionFunc = mv_up;
}

static void mv_up(BgJyaAmishutter* this) {
    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 100.0f, 3.0f)) {
        mv_open_init(this);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_STOP);
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_SLIDE - SFX_FLAG);
    }
}

void mv_open_init(BgJyaAmishutter* this) {
    this->actionFunc = mv_open;
}

void mv_open(BgJyaAmishutter* this) {
    if (this->dyna.actor.xzDistToPlayer > 300.0f) {
        mv_down_init(this);
    }
}

static void mv_down_init(BgJyaAmishutter* this) {
    this->actionFunc = mv_down;
}

static void mv_down(BgJyaAmishutter* this) {
    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 3.0f)) {
        mv_close_init(this);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_STOP);
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_SLIDE - SFX_FLAG);
    }
}

void Bg_Jya_Amishutter_actor_move(Actor* thisx, PlayState* play) {
    BgJyaAmishutter* this = (BgJyaAmishutter*)thisx;

    this->actionFunc(this);
}

void Bg_Jya_Amishutter_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gAmishutterDL);
}
