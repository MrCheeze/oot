/*
 * File: z_bg_pushbox.c
 * Overlay: ovl_Bg_Pushbox
 * Description: Unused (and non functional) pushable block
 */

#include "z_bg_pushbox.h"
#include "assets/objects/object_pu_box/object_pu_box.h"

#define FLAGS 0

void Bg_Pushbox_actor_ct(Actor* thisx, PlayState* play);
void Bg_Pushbox_actor_dt(Actor* thisx, PlayState* play);
void Bg_Pushbox_actor_move(Actor* thisx, PlayState* play);
void Bg_Pushbox_actor_draw(Actor* thisx, PlayState* play);

static void move_wait(BgPushbox* this, PlayState* play);

ActorProfile Bg_Pushbox_Profile = {
    /**/ ACTOR_BG_PUSHBOX,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    //! @bug fixing this actor would involve using OBJECT_PU_BOX
    /**/ OBJECT_GAMEPLAY_DANGEON_KEEP,
    /**/ sizeof(BgPushbox),
    /**/ Bg_Pushbox_actor_ct,
    /**/ Bg_Pushbox_actor_dt,
    /**/ Bg_Pushbox_actor_move,
    /**/ Bg_Pushbox_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_STOP),
};

void Bg_Pushbox_actor_set_process(BgPushbox* this, BgPushboxActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Bg_Pushbox_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgPushbox* this = (BgPushbox*)thisx;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gBlockSmallCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    Shape_Info_init(&this->dyna.actor.shape, 0.0f, NULL, 0.0f);
    Bg_Pushbox_actor_set_process(this, move_wait);
}

void Bg_Pushbox_actor_dt(Actor* thisx, PlayState* play) {
    BgPushbox* this = (BgPushbox*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void move_wait(BgPushbox* this, PlayState* play) {
    this->dyna.actor.speed += this->dyna.unk_150 * 0.2f;
    this->dyna.actor.speed = CLAMP(this->dyna.actor.speed, -1.0f, 1.0f);
    chase_f(&this->dyna.actor.speed, 0.0f, 0.2f);
    this->dyna.actor.world.rot.y = this->dyna.unk_158;
    Actor_position_moveF(&this->dyna.actor);
    Actor_BGcheck2(play, &this->dyna.actor, 20.0f, 40.0f, 40.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);
}

void Bg_Pushbox_actor_move(Actor* thisx, PlayState* play) {
    BgPushbox* this = (BgPushbox*)thisx;

    this->actionFunc(this, play);
    MoveBG_Actor_power_clear(&this->dyna);
}

void Bg_Pushbox_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_pushbox.c", 263);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_pushbox.c", 269);
    gSPDisplayList(POLY_OPA_DISP++, gBlockSmallDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_pushbox.c", 272);
}
