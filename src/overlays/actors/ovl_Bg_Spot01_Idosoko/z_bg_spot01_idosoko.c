/*
 * File: z_bg_spot01_idosoko.c
 * Overlay: ovl_Bg_Spot01_Idosoko
 * Description: Stone that blocks the entrance to Bottom of the Well
 */

#include "z_bg_spot01_idosoko.h"
#include "assets/objects/object_spot01_matoya/object_spot01_matoya.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Spot01_Idosoko_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot01_Idosoko_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot01_Idosoko_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot01_Idosoko_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgSpot01Idosoko* this, PlayState* play);

ActorProfile Bg_Spot01_Idosoko_Profile = {
    /**/ ACTOR_BG_SPOT01_IDOSOKO,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT01_MATOYA,
    /**/ sizeof(BgSpot01Idosoko),
    /**/ Bg_Spot01_Idosoko_actor_ct,
    /**/ Bg_Spot01_Idosoko_actor_dt,
    /**/ Bg_Spot01_Idosoko_actor_move,
    /**/ Bg_Spot01_Idosoko_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Spot01_Idosoko_actor_set_process(BgSpot01Idosoko* this, BgSpot01IdosokoActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Bg_Spot01_Idosoko_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgSpot01Idosoko* this = (BgSpot01Idosoko*)thisx;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    ValueSet_process(&this->dyna.actor, value_init);
    DynaPolyUty_bgdi_SG2KSG(&gKakarikoBOTWStoneCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    if (!LINK_IS_ADULT) {
        Actor_delete(&this->dyna.actor);
    } else {
        Bg_Spot01_Idosoko_actor_set_process(this, mode_wait);
    }
}

void Bg_Spot01_Idosoko_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot01Idosoko* this = (BgSpot01Idosoko*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mode_wait(BgSpot01Idosoko* this, PlayState* play) {
}

void Bg_Spot01_Idosoko_actor_move(Actor* thisx, PlayState* play) {
    BgSpot01Idosoko* this = (BgSpot01Idosoko*)thisx;

    this->actionFunc(this, play);
}

void Bg_Spot01_Idosoko_actor_draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot01_idosoko.c", 162);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_spot01_idosoko.c", 166);
    gSPDisplayList(POLY_OPA_DISP++, gKakarikoBOTWStoneDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot01_idosoko.c", 171);
}
