/*
 * File: z_eff_ss_lightning.c
 * Overlay: ovl_Effect_Ss_Lightning
 * Description: Lightning
 */

#include "z_eff_ss_lightning.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorB regs[2]
#define rPrimColorA regs[3]
#define rEnvColorR regs[4]
#define rEnvColorG regs[5]
#define rEnvColorB regs[6]
#define rEnvColorA regs[7]
#define rNumBolts regs[8]
#define rScale regs[9]
#define rYaw regs[10]
#define rLifespan regs[11]

u32 Effect_SS_Lightning_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_Lightning_disp_mode(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_Lightning_func_proc(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Lightning_Profile = {
    EFFECT_SS_LIGHTNING,
    Effect_SS_Lightning_ct,
};

u32 Effect_SS_Lightning_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsLightningInitParams* initParams = (EffectSsLightningInitParams*)initParamsx;

    this->pos = initParams->pos;
    this->gfx = SEGMENTED_TO_VIRTUAL(gEffLightningDL);
    this->life = initParams->life;
    this->draw = Effect_SS_Lightning_disp_mode;
    this->update = Effect_SS_Lightning_func_proc;
    this->rPrimColorR = initParams->primColor.r;
    this->rPrimColorG = initParams->primColor.g;
    this->rPrimColorB = initParams->primColor.b;
    this->rPrimColorA = initParams->primColor.a;
    this->rEnvColorR = initParams->envColor.r;
    this->rEnvColorG = initParams->envColor.g;
    this->rEnvColorB = initParams->envColor.b;
    this->rEnvColorA = initParams->envColor.a;
    this->rNumBolts = initParams->numBolts;
    this->rScale = initParams->scale;
    this->rYaw = initParams->yaw;
    this->rLifespan = initParams->life;

    return 1;
}

void Effect_SS_Lightning_next_ct(PlayState* play, Vec3f* pos, s16 yaw, EffectSs* this) {
    EffectSs newLightning;

    EffectSoftSprite2_ElementDestructClear(&newLightning);
    newLightning = *this;
    newLightning.pos = *pos;
    newLightning.rNumBolts--;
    newLightning.rYaw = yaw;
    newLightning.life = newLightning.rLifespan;

    EffectSoftSprite2_add(play, &newLightning);
}

void Effect_SS_Lightning_disp_mode(PlayState* play, u32 index, EffectSs* this) {
    static void* lightning_txt[] = {
        gEffLightning1Tex, gEffLightning2Tex, gEffLightning3Tex, gEffLightning4Tex,
        gEffLightning5Tex, gEffLightning6Tex, gEffLightning7Tex, gEffLightning8Tex,
    };
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfResult;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfRotate;
    MtxF mfTransBillboard;
    MtxF mfTransBillboardRotate;
    Mtx* mtx;
    f32 yScale;
    s16 texIdx;
    f32 xzScale;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_lightning.c", 233);

    yScale = this->rScale * 0.01f;
    texIdx = this->rLifespan - this->life;

    if (texIdx > 7) {
        texIdx = 7;
    }

    Skin_Matrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    xzScale = yScale * 0.6f;
    Skin_Matrix_SetScale(&mfScale, xzScale, yScale, xzScale);
    Skin_Matrix_SetRotateXyz_s(&mfRotate, this->vec.x, this->vec.y, this->rYaw);
    Skin_Matrix_MulMatrix(&mfTrans, &play->billboardMtxF, &mfTransBillboard);
    Skin_Matrix_MulMatrix(&mfTransBillboard, &mfRotate, &mfTransBillboardRotate);
    Skin_Matrix_MulMatrix(&mfTransBillboardRotate, &mfScale, &mfResult);

    gSPMatrix(POLY_XLU_DISP++, &Mtx_clear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    mtx = Skin_Matrix_to_Mtx_new(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        texture_z_cld_poly_xlu_nd(gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(lightning_txt[texIdx]));
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                        this->rPrimColorA);
        gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, this->rEnvColorA);
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_lightning.c", 281);
}

void Effect_SS_Lightning_func_proc(PlayState* play, u32 index, EffectSs* this) {
    s32 pad;
    Vec3f pos;
    s16 yaw;
    f32 scale;

    if ((this->rNumBolts != 0) && ((this->life + 1) == this->rLifespan)) {

        yaw = this->rYaw + (((fqrand() < 0.5f) ? -1 : 1) * ((s16)((fqrand() * 3640.0f)) + 0xE38));

        scale = (this->rScale * 0.01f) * 80.0f;
        pos.y = this->pos.y + (sin_s(this->rYaw - 0x4000) * scale);

        scale = cos_s(this->rYaw - 0x4000) * scale;
        pos.x = this->pos.x - (cos_s(getCameraAngleY(GET_ACTIVE_CAM(play))) * scale);
        pos.z = this->pos.z + (sin_s(getCameraAngleY(GET_ACTIVE_CAM(play))) * scale);

        Effect_SS_Lightning_next_ct(play, &pos, yaw, this);

        if (fqrand() < 0.1f) {
            Effect_SS_Lightning_next_ct(play, &pos, (this->rYaw * 2) - yaw, this);
        }
    }
}
