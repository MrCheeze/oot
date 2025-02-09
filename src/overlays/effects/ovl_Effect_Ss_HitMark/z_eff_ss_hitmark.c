/*
 * File: z_eff_ss_hitmark.c
 * Overlay: ovl_Effect_Ss_HitMark
 * Description: Hit Marks
 */

#include "z_eff_ss_hitmark.h"
#include "global.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define rTexIndex regs[0]
#define rType regs[1]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rEnvColorR regs[5]
#define rEnvColorG regs[6]
#define rEnvColorB regs[7]
#define rScale regs[8]

u32 Effect_SS2_HitMark_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_HitMark_disp(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_HitMark_proc(PlayState* play, u32 index, EffectSs* this);

static Color_RGB8 ColorTable[] = {
    { 255, 255, 255 }, { 255, 255, 0 }, { 255, 255, 255 }, { 255, 0, 0 },   { 255, 200, 100 }, { 200, 150, 0 },
    { 150, 100, 0 },   { 100, 50, 0 },  { 255, 255, 255 }, { 255, 0, 0 },   { 255, 255, 0 },   { 255, 0, 0 },
    { 255, 255, 255 }, { 0, 255, 200 }, { 255, 255, 255 }, { 150, 0, 255 },
};

static void* hit_mark_txt[] = {
    gEffHitMark1Tex,  gEffHitMark2Tex,  gEffHitMark3Tex,  gEffHitMark4Tex,  gEffHitMark5Tex,  gEffHitMark6Tex,
    gEffHitMark7Tex,  gEffHitMark8Tex,  gEffHitMark9Tex,  gEffHitMark10Tex, gEffHitMark11Tex, gEffHitMark12Tex,
    gEffHitMark13Tex, gEffHitMark14Tex, gEffHitMark15Tex, gEffHitMark16Tex, gEffHitMark17Tex, gEffHitMark18Tex,
    gEffHitMark19Tex, gEffHitMark20Tex, gEffHitMark21Tex, gEffHitMark22Tex, gEffHitMark23Tex, gEffHitMark24Tex,
    gEffHitMark1Tex,  gEffHitMark2Tex,  gEffHitMark3Tex,  gEffHitMark4Tex,  gEffHitMark5Tex,  gEffHitMark6Tex,
    gEffHitMark7Tex,  gEffHitMark8Tex,
};

EffectSsProfile Effect_Ss_HitMark_Profile = {
    EFFECT_SS_HITMARK,
    Effect_SS2_HitMark_ct,
};

u32 Effect_SS2_HitMark_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    s32 colorIdx;
    EffectSsHitMarkInitParams* initParams = (EffectSsHitMarkInitParams*)initParamsx;
    this->pos = initParams->pos;
    this->gfx = SEGMENTED_TO_VIRTUAL(gEffHitMarkDL);

    if (initParams->type == EFFECT_HITMARK_DUST) {
        this->life = 16;
    } else {
        this->life = 8;
    }

    this->draw = Effect_SS_HitMark_disp;
    this->update = Effect_SS_HitMark_proc;
    colorIdx = initParams->type * 4;
    this->rTexIndex = 0;
    this->rType = initParams->type;
    this->rPrimColorR = ColorTable[colorIdx].r;
    this->rPrimColorG = ColorTable[colorIdx].g;
    this->rPrimColorB = ColorTable[colorIdx].b;
    this->rEnvColorR = ColorTable[colorIdx + 1].r;
    this->rEnvColorG = ColorTable[colorIdx + 1].g;
    this->rEnvColorB = ColorTable[colorIdx + 1].b;
    this->rScale = initParams->scale;

    return 1;
}

void Effect_SS_HitMark_disp(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTransBillboard;
    Mtx* mtx;
    f32 scale;
    s32 pad;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_hitmark.c", 297);

    Skin_Matrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    scale = this->rScale / 100.0f;
    Skin_Matrix_SetScale(&mfScale, scale, scale, 1.0f);
    Skin_Matrix_MulMatrix(&mfTrans, &play->billboardMtxF, &mfTransBillboard);
    Skin_Matrix_MulMatrix(&mfTransBillboard, &mfScale, &mfResult);
    gSPMatrix(POLY_XLU_DISP++, &Mtx_clear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    mtx = Skin_Matrix_to_Mtx_new(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(hit_mark_txt[(this->rType * 8) + (this->rTexIndex)]));
        texture_z_cld_poly_xlu_nd(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, 0);
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
    }
    CLOSE_DISPS(gfxCtx, "../z_eff_ss_hitmark.c", 341);
}

void Effect_SS_HitMark_proc(PlayState* play, u32 index, EffectSs* this) {
    s32 colorIdx;

    if (this->rType == EFFECT_HITMARK_DUST) {
        this->rTexIndex = (15 - this->life) / 2;
    } else {
        this->rTexIndex = 7 - this->life;
    }

    if (this->rTexIndex != 0) {
        colorIdx = this->rType * 4 + 2;
        this->rPrimColorR = Effect_SS_Uty_short_interpolation(this->rPrimColorR, ColorTable[colorIdx].r, this->life + 1);
        this->rPrimColorG = Effect_SS_Uty_short_interpolation(this->rPrimColorG, ColorTable[colorIdx].g, this->life + 1);
        this->rPrimColorB = Effect_SS_Uty_short_interpolation(this->rPrimColorB, ColorTable[colorIdx].b, this->life + 1);
        this->rEnvColorR = Effect_SS_Uty_short_interpolation(this->rEnvColorR, ColorTable[colorIdx + 1].r, this->life + 1);
        this->rEnvColorG = Effect_SS_Uty_short_interpolation(this->rEnvColorG, ColorTable[colorIdx + 1].g, this->life + 1);
        this->rEnvColorB = Effect_SS_Uty_short_interpolation(this->rEnvColorB, ColorTable[colorIdx + 1].b, this->life + 1);
    }
}
