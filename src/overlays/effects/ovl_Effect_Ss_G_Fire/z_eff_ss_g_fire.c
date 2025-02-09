/*
 * File: z_eff_ss_g_fire.c
 * Overlay: ovl_Effect_Ss_G_Fire
 * Description: Flame Footprints
 */

#include "z_eff_ss_g_fire.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

u32 Effect_SS2_G_Fire_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_G_Fire_disp_mode(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_G_Fire_func_proc(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_G_Fire_Profile = {
    EFFECT_SS_G_FIRE,
    Effect_SS2_G_Fire_ct,
};

u32 Effect_SS2_G_Fire_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsGFireInitParams* initParams = (EffectSsGFireInitParams*)initParamsx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    this->velocity = this->accel = zeroVec;
    this->pos = initParams->pos;
    this->draw = Effect_SS_G_Fire_disp_mode;
    this->update = Effect_SS_G_Fire_func_proc;
    this->gfx = SEGMENTED_TO_VIRTUAL(gEffFireFootprintDL);
    this->life = 8;
    this->flags = 0;
    this->rgScale = 200;
    this->rgTexIdx = 0;
    this->rgTexIdxStep = 50;
    this->rgPrimColorR = 255;
    this->rgPrimColorG = 220;
    this->rgPrimColorB = 80;
    this->rgPrimColorA = 255;
    this->rgEnvColorR = 130;
    this->rgEnvColorG = 30;
    this->rgEnvColorB = 0;
    this->rgEnvColorA = 0;

    return 1;
}

void Effect_SS_G_Fire_disp_mode(PlayState* play, u32 index, EffectSs* this) {
    void* fireFootprintTextures[] = {
        gEffFireFootprint1Tex, gEffFireFootprint2Tex, gEffFireFootprint3Tex, gEffFireFootprint4Tex,
        gEffFireFootprint5Tex, gEffFireFootprint6Tex, gEffFireFootprint7Tex, gEffFireFootprint8Tex,
    };
    s16 texIdx = (this->rgTexIdx / 100) % 7;

    effect_disp_mode_sub(play, this, fireFootprintTextures[texIdx]);
}

void Effect_SS_G_Fire_func_proc(PlayState* play, u32 index, EffectSs* this) {
    this->rgTexIdx += this->rgTexIdxStep;
}
