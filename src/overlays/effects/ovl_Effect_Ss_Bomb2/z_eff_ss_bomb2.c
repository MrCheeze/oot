/*
 * File: z_eff_ss_bomb2.c
 * Overlay: ovl_Effect_Ss_Bomb2
 * Description: Bomb Blast
 */

#include "z_eff_ss_bomb2.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]
#define rTexIndex regs[1]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rPrimColorA regs[5]
#define rEnvColorR regs[6]
#define rEnvColorG regs[7]
#define rEnvColorB regs[8]
#define rScaleStep regs[9]
#define rDepth regs[10]

u32 Effect_SS_Bomb2_ct_ovl(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_Bomb2_disp_mode(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_Bomb2_disp_mode2(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_Bomb2_func_proc(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Bomb2_Profile = {
    EFFECT_SS_BOMB2,
    Effect_SS_Bomb2_ct_ovl,
};

static EffectSsDrawFunc disp_func_tbl[] = {
    Effect_SS_Bomb2_disp_mode,
    Effect_SS_Bomb2_disp_mode2,
};

u32 Effect_SS_Bomb2_ct_ovl(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {

    EffectSsBomb2InitParams* initParams = (EffectSsBomb2InitParams*)initParamsx;

    xyz_t_move(&this->pos, &initParams->pos);
    xyz_t_move(&this->velocity, &initParams->velocity);
    xyz_t_move(&this->accel, &initParams->accel);
    this->gfx = SEGMENTED_TO_VIRTUAL(gEffBombExplosion1DL);
    this->life = 24;
    this->update = Effect_SS_Bomb2_func_proc;
    this->draw = disp_func_tbl[initParams->drawMode];
    this->rScale = initParams->scale;
    this->rScaleStep = initParams->scaleStep;
    this->rPrimColorR = 255;
    this->rPrimColorG = 255;
    this->rPrimColorB = 255;
    this->rPrimColorA = 255;
    this->rEnvColorR = 0;
    this->rEnvColorG = 0;
    this->rEnvColorB = 200;

    return 1;
}

// unused in the original game. looks like EffectSsBomb but with color
void Effect_SS_Bomb2_disp_mode(PlayState* play, u32 index, EffectSs* this) {
    static void* bomb2_txt[] = {
        gEffBombExplosion1Tex, gEffBombExplosion2Tex, gEffBombExplosion3Tex, gEffBombExplosion4Tex,
        gEffBombExplosion5Tex, gEffBombExplosion6Tex, gEffBombExplosion7Tex, gEffBombExplosion8Tex,
    };
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTransBillboard;
    Mtx* mtx;
    s32 pad;
    f32 scale;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_bomb2.c", 298);

    scale = this->rScale * 0.01f;
    Skin_Matrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    Skin_Matrix_SetScale(&mfScale, scale, scale, 1.0f);
    Skin_Matrix_MulMatrix(&mfTrans, &play->billboardMtxF, &mfTransBillboard);
    Skin_Matrix_MulMatrix(&mfTransBillboard, &mfScale, &mfResult);

    mtx = Skin_Matrix_to_Mtx_new(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        texture_z_cld_poly_xlu(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                        this->rPrimColorA);
        gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, 0);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(bomb2_txt[this->rTexIndex]));
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
    }

    if (1) {}
    CLOSE_DISPS(gfxCtx, "../z_eff_ss_bomb2.c", 345);
}

void Effect_SS_Bomb2_disp_mode2(PlayState* play, u32 index, EffectSs* this) {
    static void* bomb2_txt[] = {
        gEffBombExplosion1Tex, gEffBombExplosion2Tex, gEffBombExplosion3Tex, gEffBombExplosion4Tex,
        gEffBombExplosion5Tex, gEffBombExplosion6Tex, gEffBombExplosion7Tex, gEffBombExplosion8Tex,
    };
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTransBillboard;
    MtxF mtx2F;
    Mtx* mtx2;
    Mtx* mtx;
    s32 pad[3];
    f32 scale;
    f32 depth;
    f32 layer2Scale = 0.925f;
    s32 i;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_bomb2.c", 386);

    depth = this->rDepth;
    scale = this->rScale * 0.01f;
    Skin_Matrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    Skin_Matrix_SetScale(&mfScale, scale, scale, 1.0f);
    if (1) {}
    Skin_Matrix_MulMatrix(&mfTrans, &play->billboardMtxF, &mfTransBillboard);
    Skin_Matrix_MulMatrix(&mfTransBillboard, &mfScale, &mfResult);

    mtx = Skin_Matrix_to_Mtx_new(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        mtx2 = Skin_Matrix_to_Mtx_new(gfxCtx, &mfResult);

        if (mtx2 != NULL) {
            texture_z_cld_poly_xlu(gfxCtx);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                            this->rPrimColorA);
            gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, 0);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(bomb2_txt[this->rTexIndex]));
            gSPDisplayList(POLY_XLU_DISP++, gEffBombExplosion2DL);
            gSPDisplayList(POLY_XLU_DISP++, gEffBombExplosion3DL);

            Matrix_MtxtoMtxF(mtx2, &mtx2F);
            Matrix_put(&mtx2F);

            for (i = 1; i >= 0; i--) {
                Matrix_translate(0.0f, 0.0f, depth, MTXMODE_APPLY);
                Matrix_rotateZ((this->life * 0.02f) + 180.0f, MTXMODE_APPLY);
                Matrix_scale(layer2Scale, layer2Scale, layer2Scale, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_eff_ss_bomb2.c", 448);
                gSPDisplayList(POLY_XLU_DISP++, gEffBombExplosion3DL);
                layer2Scale -= 0.15f;
            }
        }
    }

    if (1) {}
    CLOSE_DISPS(gfxCtx, "../z_eff_ss_bomb2.c", 456);
}

void Effect_SS_Bomb2_func_proc(PlayState* play, u32 index, EffectSs* this) {
    s32 divisor;

    this->rTexIndex = (23 - this->life) / 3;
    this->rScale += this->rScaleStep;

    if (this->rScaleStep == 30) {
        this->rDepth += 4.0f;
    } else {
        this->rDepth += 2.0f;
    }

    if ((this->life < 23) && (this->life > 13)) {
        divisor = this->life - 13;
        this->rPrimColorR = Effect_SS_Uty_short_interpolation(this->rPrimColorR, 255, divisor);
        this->rPrimColorG = Effect_SS_Uty_short_interpolation(this->rPrimColorG, 255, divisor);
        this->rPrimColorB = Effect_SS_Uty_short_interpolation(this->rPrimColorB, 150, divisor);
        this->rPrimColorA = Effect_SS_Uty_short_interpolation(this->rPrimColorA, 255, divisor);
        this->rEnvColorR = Effect_SS_Uty_short_interpolation(this->rEnvColorR, 150, divisor);
        this->rEnvColorG = Effect_SS_Uty_short_interpolation(this->rEnvColorG, 0, divisor);
        this->rEnvColorB = Effect_SS_Uty_short_interpolation(this->rEnvColorB, 0, divisor);
    } else if ((this->life < 14) && (this->life > -1)) {
        divisor = this->life + 1;
        this->rPrimColorR = Effect_SS_Uty_short_interpolation(this->rPrimColorR, 50, divisor);
        this->rPrimColorG = Effect_SS_Uty_short_interpolation(this->rPrimColorG, 50, divisor);
        this->rPrimColorB = Effect_SS_Uty_short_interpolation(this->rPrimColorB, 50, divisor);
        this->rPrimColorA = Effect_SS_Uty_short_interpolation(this->rPrimColorA, 150, divisor);
        this->rEnvColorR = Effect_SS_Uty_short_interpolation(this->rEnvColorR, 10, divisor);
        this->rEnvColorG = Effect_SS_Uty_short_interpolation(this->rEnvColorG, 10, divisor);
        this->rEnvColorB = Effect_SS_Uty_short_interpolation(this->rEnvColorB, 10, divisor);
    }
}
