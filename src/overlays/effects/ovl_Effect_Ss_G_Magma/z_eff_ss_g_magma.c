/*
 * File: z_eff_ss_g_magma.c
 * Overlay: ovl_Effect_Ss_G_Magma
 * Description: Magma Bubbles
 */

#include "z_eff_ss_g_magma.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

u32 Effect_SS2_G_Magma_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_G_Magma_disp_mode(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_G_Magma_func_proc(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_G_Magma_Profile = {
    EFFECT_SS_G_MAGMA,
    Effect_SS2_G_Magma_ct,
};

u32 Effect_SS2_G_Magma_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsGMagmaInitParams* initParams = (EffectSsGMagmaInitParams*)initParamsx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    this->velocity = this->accel = zeroVec;
    this->pos = initParams->pos;
    this->draw = Effect_SS_G_Magma_disp_mode;
    this->update = Effect_SS_G_Magma_func_proc;
    this->gfx = SEGMENTED_TO_VIRTUAL(gEffMagmaBubbleDL);
    this->life = 16;
    this->rgScale = (s16)(fqrand() * 100.0f) + 200;
    this->rgTexIdx = 0;
    this->rgTexIdxStep = 50;
    this->rgPrimColorR = 255;
    this->rgPrimColorG = 255;
    this->rgPrimColorB = 0;
    this->rgPrimColorA = 255;
    this->rgEnvColorR = 255;
    this->rgEnvColorG = 0;
    this->rgEnvColorB = 0;
    this->rgEnvColorA = 0;

    return 1;
}

static void* magma_txt[] = {
    gEffMagmaBubble1Tex, gEffMagmaBubble2Tex, gEffMagmaBubble3Tex, gEffMagmaBubble4Tex,
    gEffMagmaBubble5Tex, gEffMagmaBubble6Tex, gEffMagmaBubble7Tex, gEffMagmaBubble8Tex,
};

void Effect_SS_G_Magma_disp_mode(PlayState* play, u32 index, EffectSs* this) {
    s16 texIdx = this->rgTexIdx / 100;

    if (texIdx > 7) {
        texIdx = 7;
    }

    effect_disp_mode_sub(play, this, magma_txt[texIdx]);
}

void Effect_SS_G_Magma_func_proc(PlayState* play, u32 index, EffectSs* this) {
    this->rgTexIdx += this->rgTexIdxStep;
}
