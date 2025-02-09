/*
 * File: z_eff_ss_kirakira.c
 * Overlay: ovl_Effect_Ss_KiraKira
 * Description: Sparkles
 */

#include "z_eff_ss_kirakira.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define rRotSpeed regs[0]
#define rYaw regs[1]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rPrimColorA regs[5]
#define rEnvColorR regs[6]
#define rEnvColorG regs[7]
#define rEnvColorB regs[8]
#define rEnvColorA regs[9]
#define rAlphaStep regs[10]
#define rScale regs[11]
#define rLifespan regs[12]

u32 Effect_SS2_KiraKira_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS2_KiraKira_disp_mode(PlayState* play, u32 index, EffectSs* this);
void Effect_SS2_KiraKira_func_proc(PlayState* play, u32 index, EffectSs* this);
void Effect_SS2_KiraKira_func_soul_proc(PlayState* play, u32 index, EffectSs* this);
void Effect_SS2_KiraKira_func_okarina_proc(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_KiraKira_Profile = {
    EFFECT_SS_KIRAKIRA,
    Effect_SS2_KiraKira_ct,
};

u32 Effect_SS2_KiraKira_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsKiraKiraInitParams* initParams = (EffectSsKiraKiraInitParams*)initParamsx;

    this->pos = initParams->pos;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;

    if ((this->life = initParams->life) < 0) {
        this->life = -this->life;
        this->gfx = SEGMENTED_TO_VIRTUAL(gEffSparklesDL);
        this->update = Effect_SS2_KiraKira_func_okarina_proc;
        this->rEnvColorA = initParams->scale;
        this->rScale = 0;
    } else {
        this->gfx = SEGMENTED_TO_VIRTUAL(gEffSparklesDL);

        if (initParams->updateMode == 0) {
            this->update = Effect_SS2_KiraKira_func_proc;
        } else {
            this->update = Effect_SS2_KiraKira_func_soul_proc;
        }

        this->rEnvColorA = initParams->envColor.a;
        this->rScale = initParams->scale;
    }

    this->draw = Effect_SS2_KiraKira_disp_mode;
    this->rRotSpeed = initParams->rotSpeed;
    this->rYaw = initParams->yaw;
    this->rPrimColorR = initParams->primColor.r;
    this->rPrimColorG = initParams->primColor.g;
    this->rPrimColorB = initParams->primColor.b;
    this->rPrimColorA = initParams->primColor.a;
    this->rEnvColorR = initParams->envColor.r;
    this->rEnvColorG = initParams->envColor.g;
    this->rEnvColorB = initParams->envColor.b;
    this->rAlphaStep = initParams->alphaStep;
    this->rLifespan = initParams->life;

    return 1;
}

void Effect_SS2_KiraKira_disp_mode(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx;
    f32 scale;
    s32 pad;
    MtxF mfTrans;
    MtxF mfRotY;
    MtxF mfScale;
    MtxF mfTransBillboard;
    MtxF mfTransBillboardRotY;
    MtxF mfResult;
    Mtx* mtx;

    scale = this->rScale / 10000.0f;
    gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_kirakira.c", 257);

    Skin_Matrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    Skin_Matrix_SetRotateXyz_s(&mfRotY, 0, 0, this->rYaw);
    Skin_Matrix_SetScale(&mfScale, scale, scale, 1.0f);
    Skin_Matrix_MulMatrix(&mfTrans, &play->billboardMtxF, &mfTransBillboard);
    Skin_Matrix_MulMatrix(&mfTransBillboard, &mfRotY, &mfTransBillboardRotY);
    Skin_Matrix_MulMatrix(&mfTransBillboardRotY, &mfScale, &mfResult);
    gSPMatrix(POLY_XLU_DISP++, &Mtx_clear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    mtx = Skin_Matrix_to_Mtx_new(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        texture_z_light_prim_xlu_disp(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                        (((s8)((55.0f / this->rLifespan) * this->life) + 200)));
        gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, this->rEnvColorA);
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_kirakira.c", 301);
}

void Effect_SS2_KiraKira_func_proc(PlayState* play, u32 index, EffectSs* this) {
    this->accel.x = (fqrand() * 0.4f) - 0.2f;
    this->accel.z = (fqrand() * 0.4f) - 0.2f;
    this->rEnvColorA += this->rAlphaStep;

    if (this->rEnvColorA < 0) {
        this->rEnvColorA = 0;
        this->rAlphaStep = -this->rAlphaStep;
    } else if (this->rEnvColorA > 255) {
        this->rEnvColorA = 255;
        this->rAlphaStep = -this->rAlphaStep;
    }

    this->rYaw += this->rRotSpeed;
}

void Effect_SS2_KiraKira_func_soul_proc(PlayState* play, u32 index, EffectSs* this) {
    this->velocity.x *= 0.95f;
    this->velocity.z *= 0.95f;
    this->accel.x = rnd_fx(0.2f);
    this->accel.z = rnd_fx(0.2f);
    this->rEnvColorA += this->rAlphaStep;

    if (this->rEnvColorA < 0) {
        this->rEnvColorA = 0;
        this->rAlphaStep = -this->rAlphaStep;
    } else if (this->rEnvColorA > 255) {
        this->rEnvColorA = 255;
        this->rAlphaStep = -this->rAlphaStep;
    }

    this->rYaw += this->rRotSpeed;
}

void Effect_SS2_KiraKira_func_okarina_proc(PlayState* play, u32 index, EffectSs* this) {
    this->rScale = this->rEnvColorA * sin_s((32768.0f / this->rLifespan) * this->life);
}
