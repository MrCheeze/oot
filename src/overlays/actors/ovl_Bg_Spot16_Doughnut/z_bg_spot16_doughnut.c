/*
 * File: z_bg_spot16_doughnut.c
 * Overlay: ovl_Bg_Spot16_Doughnut
 * Description: Death Mountain cloud circle
 */

#include "z_bg_spot16_doughnut.h"
#include "assets/objects/object_efc_doughnut/object_efc_doughnut.h"
#include "terminal.h"

#define FLAGS 0

void Bg_Spot16_Doughnut_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot16_Doughnut_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot16_Doughnut_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot16_Doughnut_actor_draw(Actor* thisx, PlayState* play);

void Bg_Spot16_Doughnut_actor_move_komatu(Actor* thisx, PlayState* play);
void Bg_Spot16_Doughnut_actor_draw_komatu(Actor* thisx, PlayState* play);

ActorProfile Bg_Spot16_Doughnut_Profile = {
    /**/ ACTOR_BG_SPOT16_DOUGHNUT,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_EFC_DOUGHNUT,
    /**/ sizeof(BgSpot16Doughnut),
    /**/ Bg_Spot16_Doughnut_actor_ct,
    /**/ Bg_Spot16_Doughnut_actor_dt,
    /**/ Bg_Spot16_Doughnut_actor_move,
    /**/ Bg_Spot16_Doughnut_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 5500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 5000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 5000, ICHAIN_STOP),
};

static s16 sc[] = {
    0, 0, 70, 210, 300,
};

void Bg_Spot16_Doughnut_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot16Doughnut* this = (BgSpot16Doughnut*)thisx;
    s32 params;

    ValueSet_process(&this->actor, value_init);
    Actor_set_scale(&this->actor, 0.1f);
    this->fireFlag = 0;
    this->envColorAlpha = 255;
    params = this->actor.params;
    if (params == 1 || params == 2 || params == 3 || params == 4) {
        Actor_set_scale(&this->actor, sc[this->actor.params] * 1.0e-4f);
        this->actor.draw = Bg_Spot16_Doughnut_actor_draw_komatu;
        this->actor.update = Bg_Spot16_Doughnut_actor_move_komatu;
    } else {
        // Scales this actor for scenes where it is featured in the background,
        // Death Mountain itself falls into the default case.
        switch (play->sceneId) {
            case SCENE_KAKARIKO_VILLAGE:
                Actor_set_scale(&this->actor, 0.04f);
                break;
            case SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY:
            case SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT:
            case SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS:
                Actor_set_scale(&this->actor, 0.018f);
                break;
            default:
                Actor_set_scale(&this->actor, 0.1f);
                break;
        }
        PRINTF(VT_FGCOL(CYAN) "%f" VT_RST "\n", this->actor.scale.x);
        if (!LINK_IS_ADULT || GET_EVENTCHKINF(EVENTCHKINF_2F)) {
            this->fireFlag &= ~1;
        } else {
            this->fireFlag |= 1;
        }
        PRINTF("(ｓｐｏｔ１６ ドーナツ雲)(arg_data 0x%04x)\n", this->actor.params);
    }
}

void Bg_Spot16_Doughnut_actor_dt(Actor* thisx, PlayState* play) {
}

void Bg_Spot16_Doughnut_actor_move(Actor* thisx, PlayState* play) {
    BgSpot16Doughnut* this = (BgSpot16Doughnut*)thisx;

    if (!(this->fireFlag & 1)) {
        this->actor.shape.rot.y -= 0x20;
        if (this->envColorAlpha < 255) {
            this->envColorAlpha += 5;
        } else {
            this->envColorAlpha = 255;
        }
    } else if (play->csCtx.state != CS_STATE_IDLE && play->csCtx.actorCues[2] != NULL &&
               play->csCtx.actorCues[2]->id == 2) {
        if (this->envColorAlpha >= 6) {
            this->envColorAlpha -= 5;
        } else {
            this->envColorAlpha = 0;
            this->fireFlag &= ~1;
        }
    }
}

// Update function for outwardly expanding and dissipating
void Bg_Spot16_Doughnut_actor_move_komatu(Actor* thisx, PlayState* play) {
    BgSpot16Doughnut* this = (BgSpot16Doughnut*)thisx;

    if (this->envColorAlpha >= 6) {
        this->envColorAlpha -= 5;
    } else {
        Actor_delete(&this->actor);
    }
    this->actor.shape.rot.y -= 0x20;
    Actor_set_scale(&this->actor, this->actor.scale.x + 0.0019999998f);
}

void Bg_Spot16_Doughnut_actor_draw(Actor* thisx, PlayState* play) {
    BgSpot16Doughnut* this = (BgSpot16Doughnut*)thisx;
    u32 scroll = play->gameplayFrames & 0xFFFF;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot16_doughnut.c", 210);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_spot16_doughnut.c", 213);
    if (this->fireFlag & 1) {
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, scroll * (-1), 0, 16, 32, 1, scroll,
                                    scroll * (-2), 16, 32));
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, this->envColorAlpha);
        gSPDisplayList(POLY_XLU_DISP++, gDeathMountainCloudCircleFieryDL);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, this->envColorAlpha);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
        gSPDisplayList(POLY_XLU_DISP++, gDeathMountainCloudCircleNormalDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot16_doughnut.c", 238);
}

// Draw function for outwardly expanding and dissipating
void Bg_Spot16_Doughnut_actor_draw_komatu(Actor* thisx, PlayState* play) {
    BgSpot16Doughnut* this = (BgSpot16Doughnut*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot16_doughnut.c", 245);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_spot16_doughnut.c", 248);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, this->envColorAlpha);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
    gSPDisplayList(POLY_XLU_DISP++, gDeathMountainCloudCircleNormalDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot16_doughnut.c", 256);
}
