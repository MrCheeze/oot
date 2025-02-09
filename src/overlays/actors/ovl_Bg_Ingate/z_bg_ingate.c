/*
 * File: z_bg_ingate.c
 * Overlay: ovl_Bg_Ingate
 * Description: Ingo's Gates (Lon Lon Ranch)
 */

#include "z_bg_ingate.h"
#include "assets/objects/object_ingate/object_ingate.h"

#define FLAGS 0

void Bg_Ingate_actor_ct(Actor* thisx, PlayState* play);
void Bg_Ingate_actor_dt(Actor* thisx, PlayState* play);
void Bg_Ingate_actor_move(Actor* thisx, PlayState* play);
void Bg_Ingate_actor_draw(Actor* thisx, PlayState* play);

void mode_close(BgInGate* this, PlayState* play);
static void mode_wait(BgInGate* this, PlayState* play);

ActorProfile Bg_Ingate_Profile = {
    /**/ ACTOR_BG_INGATE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_INGATE,
    /**/ sizeof(BgInGate),
    /**/ Bg_Ingate_actor_ct,
    /**/ Bg_Ingate_actor_dt,
    /**/ Bg_Ingate_actor_move,
    /**/ Bg_Ingate_actor_draw,
};

void Bg_Ingate_actor_set_process(BgInGate* this, BgInGateActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Bg_Ingate_actor_ct(Actor* thisx, PlayState* play) {
    BgInGate* this = (BgInGate*)thisx;

    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gIngoGateCol, &colHeader);

    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    if ((play->sceneId != SCENE_LON_LON_RANCH || !LINK_IS_ADULT) ||
        (GET_EVENTCHKINF(EVENTCHKINF_EPONA_OBTAINED) && (z_common_data.save.cutsceneIndex != 0xFFF0))) {
        Actor_delete(&this->dyna.actor);
        return;
    }

    Actor_set_scale(&this->dyna.actor, 0.1f);
    if ((PARAMS_GET_U(this->dyna.actor.params, 0, 1) != 0) &&
        (GET_EVENTINF_INGO_RACE_STATE() == INGO_RACE_STATE_TRAPPED_WIN_EPONA)) {
        play->csCtx.curFrame = 0;
        Bg_Ingate_actor_set_process(this, mode_close);
    } else {
        Bg_Ingate_actor_set_process(this, mode_wait);
    }
}

void Bg_Ingate_actor_dt(Actor* thisx, PlayState* play) {
    BgInGate* this = (BgInGate*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void mode_close(BgInGate* this, PlayState* play) {
    s32 phi0;
    s16 phi1;
    s16 csFrames;

    if (play->csCtx.curFrame >= 50) {
        phi0 = 0x4000;
        if (!PARAMS_GET_NOSHIFT(this->dyna.actor.params, 1, 1)) {
            phi0 = -0x4000;
        }
        this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y + phi0;
        Bg_Ingate_actor_set_process(this, mode_wait);
    } else if (play->csCtx.curFrame >= 10) {
        csFrames = play->csCtx.curFrame - 10;
        csFrames *= 400;
        phi1 = csFrames;
        if (csFrames > 0x4000) {
            csFrames = 0x4000;
        }
        csFrames = (sin_s(csFrames) * 16384.0f);
        phi1 = csFrames;
        if (!PARAMS_GET_NOSHIFT(this->dyna.actor.params, 1, 1)) {
            phi1 = -phi1;
        }
        this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y + phi1;
    }
}

static void mode_wait(BgInGate* this, PlayState* play) {
}

void Bg_Ingate_actor_move(Actor* thisx, PlayState* play) {
    BgInGate* this = (BgInGate*)thisx;

    this->actionFunc(this, play);
}

void Bg_Ingate_actor_draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_bg_ingate.c", 240);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_ingate.c", 245);

    gSPDisplayList(POLY_OPA_DISP++, gIngoGateDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_ingate.c", 250);
}
