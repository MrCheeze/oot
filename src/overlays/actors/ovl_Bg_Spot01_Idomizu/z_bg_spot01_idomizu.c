/*
 * File: z_bg_spot01_idomizu.c
 * Overlay: ovl_Bg_Spot01_Idomizu
 * Description: Kakariko Village Well Water
 */

#include "z_bg_spot01_idomizu.h"
#include "assets/objects/object_spot01_objects/object_spot01_objects.h"

#define FLAGS ACTOR_FLAG_DRAW_CULLING_DISABLED

void Bg_Spot01_Idomizu_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot01_Idomizu_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot01_Idomizu_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot01_Idomizu_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgSpot01Idomizu* this, PlayState* play);

ActorProfile Bg_Spot01_Idomizu_Profile = {
    /**/ ACTOR_BG_SPOT01_IDOMIZU,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT01_OBJECTS,
    /**/ sizeof(BgSpot01Idomizu),
    /**/ Bg_Spot01_Idomizu_actor_ct,
    /**/ Bg_Spot01_Idomizu_actor_dt,
    /**/ Bg_Spot01_Idomizu_actor_move,
    /**/ Bg_Spot01_Idomizu_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Spot01_Idomizu_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot01Idomizu* this = (BgSpot01Idomizu*)thisx;

    ValueSet_process(&this->actor, value_init);
    if (GET_EVENTCHKINF(EVENTCHKINF_DRAINED_WELL) || LINK_AGE_IN_YEARS == YEARS_ADULT) {
        this->waterHeight = -550.0f;
    } else {
        this->waterHeight = 52.0f;
    }
    this->actionFunc = mode_wait;
    this->actor.world.pos.y = this->waterHeight;
}

void Bg_Spot01_Idomizu_actor_dt(Actor* thisx, PlayState* play) {
}

static void mode_wait(BgSpot01Idomizu* this, PlayState* play) {
    if (GET_EVENTCHKINF(EVENTCHKINF_DRAINED_WELL)) {
        this->waterHeight = -550.0f;
    }
    play->colCtx.colHeader->waterBoxes[0].ySurface = this->actor.world.pos.y;
    if (this->waterHeight < this->actor.world.pos.y) {
        Nai_FxFlagEntry(NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
    add_calc2(&this->actor.world.pos.y, this->waterHeight, 1.0f, 2.0f);
}

void Bg_Spot01_Idomizu_actor_move(Actor* thisx, PlayState* play) {
    BgSpot01Idomizu* this = (BgSpot01Idomizu*)thisx;

    this->actionFunc(this, play);
}

void Bg_Spot01_Idomizu_actor_draw(Actor* thisx, PlayState* play) {
    u32 frames;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot01_idomizu.c", 228);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_spot01_idomizu.c", 232);

    frames = play->state.frames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 127 - frames % 128, frames & 0x7F, 32, 32, 1,
                                frames % 128, frames & 0x7F, 32, 32));

    gSPDisplayList(POLY_XLU_DISP++, gKakarikoWellWaterDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot01_idomizu.c", 244);
}
