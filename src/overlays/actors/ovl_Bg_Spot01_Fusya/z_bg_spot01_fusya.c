/*
 * File: z_bg_spot01_fusya.c
 * Overlay: Bg_Spot01_Fusya
 * Description: Windmill Sails
 */

#include "z_bg_spot01_fusya.h"
#include "assets/objects/object_spot01_objects/object_spot01_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Spot01_Fusya_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot01_Fusya_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot01_Fusya_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot01_Fusya_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgSpot01Fusya* this, PlayState* play);

ActorProfile Bg_Spot01_Fusya_Profile = {
    /**/ ACTOR_BG_SPOT01_FUSYA,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT01_OBJECTS,
    /**/ sizeof(BgSpot01Fusya),
    /**/ Bg_Spot01_Fusya_actor_ct,
    /**/ Bg_Spot01_Fusya_actor_dt,
    /**/ Bg_Spot01_Fusya_actor_move,
    /**/ Bg_Spot01_Fusya_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 12800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1300, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1300, ICHAIN_STOP),
};

void Bg_Spot01_Fusya_actor_set_process(BgSpot01Fusya* this, BgSpot01FusyaActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Bg_Spot01_Fusya_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot01Fusya* this = (BgSpot01Fusya*)thisx;

    ValueSet_process(&this->actor, value_init);
    this->unk_154 = 100.0f;
    this->unk_158 = 100.0f;
    this->unk_15C = 0.5f;
    if (!IS_CUTSCENE_LAYER) {
        CLEAR_EVENTCHKINF(EVENTCHKINF_65);
    }
    Bg_Spot01_Fusya_actor_set_process(this, mode_wait);
}

void Bg_Spot01_Fusya_actor_dt(Actor* thisx, PlayState* play) {
}

static void mode_wait(BgSpot01Fusya* this, PlayState* play) {
    f32 temp;
    Actor* thisx = &this->actor;

    if (GET_EVENTCHKINF(EVENTCHKINF_65)) {
        this->unk_158 = 1800.0f;
    }
    thisx->shape.rot.z += this->unk_154;
    temp = ((this->unk_154 - 100.0f) / 1700.0f) + 1.0f;
    Na_SetMotorSe(&thisx->projectedPos, 0x2085, temp);
    add_calc2(&this->unk_154, this->unk_158, this->unk_15C, 100.0f);
}

void Bg_Spot01_Fusya_actor_move(Actor* thisx, PlayState* play) {
    BgSpot01Fusya* this = (BgSpot01Fusya*)thisx;

    this->actionFunc(this, play);
}

void Bg_Spot01_Fusya_actor_draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot01_fusya.c", 210);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_spot01_fusya.c", 214);
    gSPDisplayList(POLY_OPA_DISP++, gKakarikoWindmillSailsDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot01_fusya.c", 219);
}
