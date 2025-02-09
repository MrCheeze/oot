/*
 * File: z_eff_ss_bomb.c
 * Overlay: ovl_Effect_Ss_Bomb
 * Description: Bomb Blast. Unused in the orignal game.
 */

#include "z_eff_ss_bomb.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define EFFSSBOMB_LIFESPAN 20

#define rScale regs[0]
#define rTexIndex regs[1]

u32 Effect_SS_Bomb_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_Bomb_disp_mode(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_Bomb_func_proc(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Bomb_Profile = {
    EFFECT_SS_BOMB,
    Effect_SS_Bomb_ct,
};

u32 Effect_SS_Bomb_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsBombInitParams* initParams = (EffectSsBombInitParams*)initParamsx;

    xyz_t_move(&this->pos, &initParams->pos);
    xyz_t_move(&this->velocity, &initParams->velocity);
    xyz_t_move(&this->accel, &initParams->accel);
    this->gfx = SEGMENTED_TO_VIRTUAL(gEffBombExplosion1DL);
    this->life = EFFSSBOMB_LIFESPAN;
    this->draw = Effect_SS_Bomb_disp_mode;
    this->update = Effect_SS_Bomb_func_proc;
    this->rScale = 100;
    this->rTexIndex = 0;

    return 1;
}

static void* bomb1_txt[] = {
    gEffBombExplosion1Tex,
    gEffBombExplosion2Tex,
    gEffBombExplosion3Tex,
    gEffBombExplosion4Tex,
};

void Effect_SS_Bomb_disp_mode(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTransBillboard;
    Mtx* mtx;
    s32 pad;
    f32 scale;
    s16 intensity;

    if (1) {}

    OPEN_DISPS(gfxCtx, "../z_eff_ss_bomb.c", 168);

    scale = this->rScale / 100.0f;

    Skin_Matrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    Skin_Matrix_SetScale(&mfScale, scale, scale, 1.0f);
    if (1) {}
    Skin_Matrix_MulMatrix(&mfTrans, &play->billboardMtxF, &mfTransBillboard);
    Skin_Matrix_MulMatrix(&mfTransBillboard, &mfScale, &mfResult);

    gSPMatrix(POLY_XLU_DISP++, &Mtx_clear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    mtx = Skin_Matrix_to_Mtx_new(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(bomb1_txt[this->rTexIndex]));
        gDPPipeSync(POLY_XLU_DISP++);
        texture_z_cld_poly_xlu_nd(gfxCtx);
        intensity = this->life * ((f32)255 / EFFSSBOMB_LIFESPAN);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, intensity, intensity, intensity, intensity);
        gDPPipeSync(POLY_XLU_DISP++);
        //! @bug env color is not set but used in gEffBombExplosion1DL
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
        gDPPipeSync(POLY_XLU_DISP++);
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_bomb.c", 214);
}

void Effect_SS_Bomb_func_proc(PlayState* play, u32 index, EffectSs* this) {
    if ((this->life <= EFFSSBOMB_LIFESPAN) && (this->life > (EFFSSBOMB_LIFESPAN - ARRAY_COUNT(bomb1_txt)))) {
        this->rTexIndex = (EFFSSBOMB_LIFESPAN - this->life);
    } else {
        this->rScale += 0;
        this->rTexIndex = ARRAY_COUNT(bomb1_txt) - 1;
    }

    this->accel.x = ((fqrand() * 0.4f) - 0.2f);
    this->accel.z = ((fqrand() * 0.4f) - 0.2f);
}
