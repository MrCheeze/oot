/*
 * File: z_bg_spot15_saku.c
 * Overlay: ovl_Bg_Spot15_Saku
 * Description: Hyrule Castle Gate
 */

#include "z_bg_spot15_saku.h"
#include "assets/objects/object_spot15_obj/object_spot15_obj.h"

#define FLAGS 0

void Bg_Spot15_Saku_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot15_Saku_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot15_Saku_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot15_Saku_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgSpot15Saku* this, PlayState* play);
static void mode_move(BgSpot15Saku* this, PlayState* play);
static void mode_stop(BgSpot15Saku* this, PlayState* play);

ActorProfile Bg_Spot15_Saku_Profile = {
    /**/ ACTOR_BG_SPOT15_SAKU,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_SPOT15_OBJ,
    /**/ sizeof(BgSpot15Saku),
    /**/ Bg_Spot15_Saku_actor_ct,
    /**/ Bg_Spot15_Saku_actor_dt,
    /**/ Bg_Spot15_Saku_actor_move,
    /**/ Bg_Spot15_Saku_actor_draw,
};

void Bg_Spot15_Saku_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgSpot15Saku* this = (BgSpot15Saku*)thisx;
    s32 pad2;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gLonLonCorralFenceCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->dyna.actor.scale.x = 0.1f;
    this->dyna.actor.scale.y = 0.1f;
    this->dyna.actor.scale.z = 0.1f;
    this->unk_170.x = this->dyna.actor.world.pos.x;
    this->unk_170.y = this->dyna.actor.world.pos.y;
    this->unk_170.z = this->dyna.actor.world.pos.z;
    if (GET_INFTABLE(INFTABLE_71)) {
        this->dyna.actor.world.pos.z = 2659.0f;
    }
    this->actionFunc = mode_wait;
}

void Bg_Spot15_Saku_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot15Saku* this = (BgSpot15Saku*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mode_wait(BgSpot15Saku* this, PlayState* play) {
    if (this->unk_168 && !GET_INFTABLE(INFTABLE_71)) {
        this->timer = 2;
        this->actionFunc = mode_move;
    }
}

static void mode_move(BgSpot15Saku* this, PlayState* play) {
    if (this->timer == 0) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_METALGATE_OPEN - SFX_FLAG);
        this->dyna.actor.world.pos.z -= 2.0f;
        if (this->dyna.actor.world.pos.z < 2660.0f) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_BRIDGE_OPEN_STOP);
            this->timer = 30;
            this->actionFunc = mode_stop;
        }
    }
}

static void mode_stop(BgSpot15Saku* this, PlayState* play) {
    if (this->timer == 0) {
        this->unk_168 = 0;
        this->actionFunc = mode_wait;
    }
}

void Bg_Spot15_Saku_actor_move(Actor* thisx, PlayState* play) {
    BgSpot15Saku* this = (BgSpot15Saku*)thisx;

    if (this->timer != 0) {
        this->timer--;
    }

    this->actionFunc(this, play);
}

void Bg_Spot15_Saku_actor_draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot15_saku.c", 259);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_spot15_saku.c", 263);
    gSPDisplayList(POLY_XLU_DISP++, gLonLonCorralFenceDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot15_saku.c", 268);
}
