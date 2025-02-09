/*
 * File: z_oceff_storm.c
 * Overlay: ovl_Oceff_Storm
 * Description: Song of Storms Effect
 */

#include "z_oceff_storm.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void Oceff_Storm_Actor_ct(Actor* thisx, PlayState* play);
void Oceff_Storm_Actor_dt(Actor* thisx, PlayState* play);
void Oceff_Storm_Actor_move(Actor* thisx, PlayState* play);
void Oceff_Storm_Actor_draw(Actor* thisx, PlayState* play);

void Oceff_Storm_Actor_draw2(Actor* thisx, PlayState* play);

static void move_wait(OceffStorm* this, PlayState* play);
static void move_wait2(OceffStorm* this, PlayState* play);

ActorProfile Oceff_Storm_Profile = {
    /**/ ACTOR_OCEFF_STORM,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(OceffStorm),
    /**/ Oceff_Storm_Actor_ct,
    /**/ Oceff_Storm_Actor_dt,
    /**/ Oceff_Storm_Actor_move,
    /**/ Oceff_Storm_Actor_draw,
};

void Oceff_Storm_actor_set_process(OceffStorm* this, OceffStormActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Oceff_Storm_Actor_ct(Actor* thisx, PlayState* play) {
    OceffStorm* this = (OceffStorm*)thisx;
    Oceff_Storm_actor_set_process(this, move_wait);
    this->posYOffAdd = 0;
    this->counter = 0;
    this->primColorAlpha = 0;
    this->vtxAlpha = 0;
    this->actor.scale.y = 0.0f;
    this->actor.scale.z = 0.0f;
    this->actor.scale.x = 0.0f;
    this->posYOff = this->posYOffAdd;

    if (this->actor.params == 1) {
        Oceff_Storm_actor_set_process(this, move_wait2);
        this->actor.draw = Oceff_Storm_Actor_draw2;
    } else {
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_OKARINA_EFFECT, this->actor.world.pos.x,
                    this->actor.world.pos.y - 30.0f, this->actor.world.pos.z, 0, 0, 0, 1);
    }
}

void Oceff_Storm_Actor_dt(Actor* thisx, PlayState* play) {
    OceffStorm* this = (OceffStorm*)thisx;
    Player* player = GET_PLAYER(play);

    magic_cancel_check(play);
    if (z_common_data.nayrusLoveTimer != 0) {
        player->stateFlags3 |= PLAYER_STATE3_RESTORE_NAYRUS_LOVE;
    }
}

static void move_wait(OceffStorm* this, PlayState* play) {
    if (this->counter < 20) {
        this->primColorAlpha = (s8)(this->counter * 5.0f);
    } else if (this->counter > 80) {
        this->primColorAlpha = (s8)((100 - this->counter) * 5.0f);
    } else {
        this->primColorAlpha = 100;
    }

    if (this->counter < 10 || this->counter >= 90) {
        this->vtxAlpha = 0;
    } else {
        if (this->counter <= 65) {
            if (this->vtxAlpha <= 200) {
                this->vtxAlpha += 10;
            }
            this->actor.scale.x = this->actor.scale.z = 0.4f;
            this->actor.scale.y = 0.3f;
        } else if (this->counter > 65) {
            this->vtxAlpha = (90 - this->counter) * 10;
        } else {
            this->vtxAlpha = 255;
            this->actor.scale.x = this->actor.scale.z = 0.4f;
        }
    }

    if (this->counter > 60) {
        this->actor.world.pos.y += this->posYOff * 0.01f;
        this->posYOff += this->posYOffAdd;
        this->posYOffAdd += 10;
    }

    if (this->counter < 100) {
        this->counter++;
    } else {
        Actor_delete(&this->actor);
    }
}

static void move_wait2(OceffStorm* this, PlayState* play) {
    if (this->primColorAlpha < 100) {
        this->primColorAlpha += 5;
    }
    //! @bug Actor_delete is never called so the actor will stay alive forever
}

void Oceff_Storm_Actor_move(Actor* thisx, PlayState* play) {
    OceffStorm* this = (OceffStorm*)thisx;
    Player* player = GET_PLAYER(play);

    this->actor.world.pos = player->actor.world.pos;
    this->actor.shape.rot.y = getRealCameraAngleY(GET_ACTIVE_CAM(play));
    this->actionFunc(this, play);
}

#include "assets/overlays/ovl_Oceff_Storm/z_oceff_storm.c"

void Oceff_Storm_Actor_draw2(Actor* thisx, PlayState* play) {
    u32 scroll = play->state.frames & 0xFFF;
    OceffStorm* this = (OceffStorm*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_oceff_storm.c", 449);

    gDPPipeSync(POLY_XLU_DISP++);

    if (1) {}

    POLY_XLU_DISP = gfx_xlu_rectangle_2c(POLY_XLU_DISP);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_NOISE);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 200, 200, 150, this->primColorAlpha);
    gSPDisplayList(POLY_XLU_DISP++, oceff_storm_model);
    gSPDisplayList(POLY_XLU_DISP++, two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, scroll * 8, scroll * 4, 64,
                                                     64, 1, scroll * 4, scroll * 4, 64, 64));
    gSPTextureRectangle(POLY_XLU_DISP++, 0, 0, SCREEN_WIDTH << 2, SCREEN_HEIGHT << 2, G_TX_RENDERTILE, 0, 0,
                        (s32)(0.13671875 * (1 << 10)), (s32)(-0.13671875 * (1 << 10)));

    CLOSE_DISPS(play->state.gfxCtx, "../z_oceff_storm.c", 477);
}

void Oceff_Storm_Actor_draw(Actor* thisx, PlayState* play) {
    u32 scroll = play->state.frames & 0xFFF;
    OceffStorm* this = (OceffStorm*)thisx;
    Vtx* vtxPtr = efc_ocarina_2_v;

    OPEN_DISPS(play->state.gfxCtx, "../z_oceff_storm.c", 486);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 200, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 150, 150, 0, 128);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_NOISE);

    vtxPtr[0].v.cn[3] = vtxPtr[6].v.cn[3] = vtxPtr[16].v.cn[3] = vtxPtr[25].v.cn[3] = this->vtxAlpha >> 1;
    vtxPtr[10].v.cn[3] = vtxPtr[22].v.cn[3] = this->vtxAlpha;

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_oceff_storm.c", 498);

    gSPDisplayList(POLY_XLU_DISP++, efc_ocarina_2_modelT);
    gSPDisplayList(POLY_XLU_DISP++, two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, scroll * 4, (0 - scroll) * 8,
                                                     32, 32, 1, scroll * 8, (0 - scroll) * 12, 32, 32));
    gSPDisplayList(POLY_XLU_DISP++, efc_ocarina_2_modelT2);

    CLOSE_DISPS(play->state.gfxCtx, "../z_oceff_storm.c", 512);

    Oceff_Storm_Actor_draw2(&this->actor, play);
}
