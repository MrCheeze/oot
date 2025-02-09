/*
 * File: z_bg_gate_shutter.c
 * Overlay: Bg_Gate_Shutter
 * Description: Death Mountain Trail Gate
 */

#include "z_bg_gate_shutter.h"

#include "gfx.h"
#include "gfx_setupdl.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "terminal.h"
#include "z_lib.h"
#include "z64play.h"

#include "assets/objects/object_spot01_matoyab/object_spot01_matoyab.h"

#define FLAGS 0

void Bg_Gate_Shutter_actor_ct(Actor* thisx, PlayState* play);
void Bg_Gate_Shutter_actor_dt(Actor* thisx, PlayState* play);
void Bg_Gate_Shutter_actor_move(Actor* thisx, PlayState* play);
void Bg_Gate_Shutter_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgGateShutter* this, PlayState* play);
static void mode_move(BgGateShutter* this, PlayState* play);
static void mode_stop(BgGateShutter* this, PlayState* play);
void mode_out(BgGateShutter* this, PlayState* play);

ActorProfile Bg_Gate_Shutter_Profile = {
    /**/ ACTOR_BG_GATE_SHUTTER,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_SPOT01_MATOYAB,
    /**/ sizeof(BgGateShutter),
    /**/ Bg_Gate_Shutter_actor_ct,
    /**/ Bg_Gate_Shutter_actor_dt,
    /**/ Bg_Gate_Shutter_actor_move,
    /**/ Bg_Gate_Shutter_actor_draw,
};

void Bg_Gate_Shutter_actor_ct(Actor* thisx, PlayState* play) {
    BgGateShutter* this = (BgGateShutter*)thisx;
    s32 pad[2];
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gKakarikoGuardGateCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
    this->somePos.x = thisx->world.pos.x;
    this->somePos.y = thisx->world.pos.y;
    this->somePos.z = thisx->world.pos.z;
    if ((GET_INFTABLE(INFTABLE_76) || GET_EVENTCHKINF(EVENTCHKINF_45)) && (play->sceneId == SCENE_KAKARIKO_VILLAGE)) {
        thisx->world.pos.x = -89.0f;
        thisx->world.pos.z = -1375.0f;
    }
    thisx->scale.x = 1.0f;
    thisx->scale.y = 1.0f;
    thisx->scale.z = 1.0f;
    PRINTF("\n\n");
    PRINTF(VT_FGCOL(GREEN) " ☆☆☆☆☆ 柵でたなぁ ☆☆☆☆☆ \n" VT_RST);
    this->actionFunc = mode_wait;
}

void Bg_Gate_Shutter_actor_dt(Actor* thisx, PlayState* play) {
    BgGateShutter* this = (BgGateShutter*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mode_wait(BgGateShutter* this, PlayState* play) {
    if (this->openingState == 1 && !GET_INFTABLE(INFTABLE_76)) {
        this->unk_178 = 2;
        this->actionFunc = mode_move;
    } else if (this->openingState == 2) {
        this->unk_178 = 2;
        this->actionFunc = mode_move;
    } else if (this->openingState < 0) {
        this->unk_178 = 2;
        this->actionFunc = mode_out;
    }
}

static void mode_move(BgGateShutter* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    if (this->unk_178 == 0) {
        Actor_SE_set(thisx, NA_SE_EV_METALGATE_OPEN - SFX_FLAG);
        thisx->world.pos.x -= 2.0f;
        add_calc2(&thisx->world.pos.z, -1375.0f, 0.8f, 0.3f);
        if (thisx->world.pos.x < -89.0f) {
            Actor_SE_set(thisx, NA_SE_EV_BRIDGE_OPEN_STOP);
            this->unk_178 = 0x1E;
            this->actionFunc = mode_stop;
        }
    }
}

static void mode_stop(BgGateShutter* this, PlayState* play) {
    if (this->unk_178 == 0) {
        this->openingState = 0;
        this->actionFunc = mode_wait;
    }
}

void mode_out(BgGateShutter* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    if (this->unk_178 == 0) {
        Actor_SE_set(thisx, NA_SE_EV_METALGATE_OPEN - SFX_FLAG);
        thisx->world.pos.x += 2.0f;
        add_calc2(&thisx->world.pos.z, -1350.0f, 0.8f, 0.3f);
        if (thisx->world.pos.x > 90.0f) {
            thisx->world.pos.x = 91.0f;
            Actor_SE_set(thisx, NA_SE_EV_BRIDGE_OPEN_STOP);
            this->unk_178 = 30;
            this->actionFunc = mode_stop;
        }
    }
}

void Bg_Gate_Shutter_actor_move(Actor* thisx, PlayState* play) {
    BgGateShutter* this = (BgGateShutter*)thisx;

    if (this->unk_178 != 0) {
        this->unk_178--;
    }
    this->actionFunc(this, play);
}

void Bg_Gate_Shutter_actor_draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_bg_gate_shutter.c", 323);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_gate_shutter.c", 328);
    gSPDisplayList(POLY_OPA_DISP++, gKakarikoGuardGateDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_gate_shutter.c", 333);
}
