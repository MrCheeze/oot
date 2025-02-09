/*
 * File: z_eff_ss_g_ripple.c
 * Overlay: ovl_Effect_Ss_G_Ripple
 * Description: Water Ripple
 */

#include "z_eff_ss_g_ripple.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define rWaterBoxNum regs[0]
#define rRadius regs[1]
#define rRadiusMax regs[2]
#define rPrimColorR regs[3]
#define rPrimColorG regs[4]
#define rPrimColorB regs[5]
#define rPrimColorA regs[6]
#define rEnvColorR regs[7]
#define rEnvColorG regs[8]
#define rEnvColorB regs[9]
#define rEnvColorA regs[10]
#define rLifespan regs[11]

u32 Effect_SS2_G_Ripple_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_G_Ripple_disp_mode(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_G_Ripple_func_proc(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_G_Ripple_Profile = {
    EFFECT_SS_G_RIPPLE,
    Effect_SS2_G_Ripple_ct,
};

u32 Effect_SS2_G_Ripple_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    s32 pad;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    WaterBox* waterBox;
    EffectSsGRippleInitParams* initParams = (EffectSsGRippleInitParams*)initParamsx;

    waterBox = NULL;
    this->velocity = this->accel = zeroVec;
    this->pos = initParams->pos;
    this->gfx = SEGMENTED_TO_VIRTUAL(gEffWaterRippleDL);
    this->life = initParams->life + 20;
    this->flags = 0;
    this->draw = Effect_SS_G_Ripple_disp_mode;
    this->update = Effect_SS_G_Ripple_func_proc;
    this->rRadius = initParams->radius;
    this->rRadiusMax = initParams->radiusMax;
    this->rLifespan = initParams->life;
    this->rPrimColorR = 255;
    this->rPrimColorG = 255;
    this->rPrimColorB = 255;
    this->rPrimColorA = 255;
    this->rEnvColorR = 255;
    this->rEnvColorG = 255;
    this->rEnvColorB = 255;
    this->rEnvColorA = 255;
    this->rWaterBoxNum = T_BGCheck_WaterGetIndex(play, &play->colCtx, &initParams->pos, 3.0f, &waterBox);

    return 1;
}

void effect_disp_mode_sub_non_softsprite(PlayState* play2, EffectSs* this, void* segment) {
    PlayState* play = play2;
    f32 radius;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    Mtx* mtx;
    f32 yPos;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_g_ripple.c", 199);

    radius = this->rRadius * 0.0025f;

    if ((this->rWaterBoxNum != -1) && (this->rWaterBoxNum < play->colCtx.colHeader->numWaterBoxes)) {
        yPos = (this->rWaterBoxNum + play->colCtx.colHeader->waterBoxes)->ySurface;
    } else {
        yPos = this->pos.y;
    }

    Skin_Matrix_SetTranslate(&mfTrans, this->pos.x, yPos, this->pos.z);
    Skin_Matrix_SetScale(&mfScale, radius, radius, radius);
    Skin_Matrix_MulMatrix(&mfTrans, &mfScale, &mfResult);

    mtx = Skin_Matrix_to_Mtx_new(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        texture_z_cld_poly_xlu(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                        this->rPrimColorA);
        gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, this->rEnvColorA);
        gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
        gDPSetColorDither(POLY_XLU_DISP++, G_CD_NOISE);
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_g_ripple.c", 247);
}

void Effect_SS_G_Ripple_disp_mode(PlayState* play, u32 index, EffectSs* this) {
    if (this->rLifespan == 0) {
        effect_disp_mode_sub_non_softsprite(play, this, gEffWaterRippleTex);
    }
}

void Effect_SS_G_Ripple_func_proc(PlayState* play, u32 index, EffectSs* this) {
    f32 radius;
    f32 primAlpha;
    f32 envAlpha;

    if (DECR(this->rLifespan) == 0) {
        radius = this->rRadius;
        add_calc(&radius, this->rRadiusMax, 0.2f, 30.0f, 1.0f);
        this->rRadius = radius;

        primAlpha = this->rPrimColorA;
        envAlpha = this->rEnvColorA;

        add_calc(&primAlpha, 0.0f, 0.2f, 15.0f, 7.0f);
        add_calc(&envAlpha, 0.0f, 0.2f, 15.0f, 7.0f);

        this->rPrimColorA = primAlpha;
        this->rEnvColorA = envAlpha;
    }
}
