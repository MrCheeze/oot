/*
 * File: z_eff_ss_g_spk.c
 * Overlay: ovl_Effect_Ss_G_Spk
 * Description: Sparks
 */

#include "z_eff_ss_g_spk.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorB regs[2]
#define rPrimColorA regs[3]
#define rEnvColorR regs[4]
#define rEnvColorG regs[5]
#define rEnvColorB regs[6]
#define rEnvColorA regs[7]
#define rTexIndex regs[8]
#define rScale regs[9]
#define rScaleStep regs[10]

u32 Effect_SS_Spk_ct_ovl(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_Spk_func_proc(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_Spk_func_proc2(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_Spk_disp_mode(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_G_Spk_Profile = {
    EFFECT_SS_G_SPK,
    Effect_SS_Spk_ct_ovl,
};

u32 Effect_SS_Spk_ct_ovl(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsGSpkInitParams* initParams = (EffectSsGSpkInitParams*)initParamsx;

    xyz_t_move(&this->pos, &initParams->pos);
    xyz_t_move(&this->velocity, &initParams->velocity);
    xyz_t_move(&this->accel, &initParams->accel);
    this->gfx = SEGMENTED_TO_VIRTUAL(gEffSparkDL);

    if (initParams->updateMode == 0) {
        this->life = 10;
        this->vec.x = initParams->pos.x - initParams->actor->world.pos.x;
        this->vec.y = initParams->pos.y - initParams->actor->world.pos.y;
        this->vec.z = initParams->pos.z - initParams->actor->world.pos.z;
        this->update = Effect_SS_Spk_func_proc;
    } else {
        this->life = 5;
        this->update = Effect_SS_Spk_func_proc2;
    }

    this->draw = Effect_SS_Spk_disp_mode;
    this->rPrimColorR = initParams->primColor.r;
    this->rPrimColorG = initParams->primColor.g;
    this->rPrimColorB = initParams->primColor.b;
    this->rPrimColorA = initParams->primColor.a;
    this->rEnvColorR = initParams->envColor.r;
    this->rEnvColorG = initParams->envColor.g;
    this->rEnvColorB = initParams->envColor.b;
    this->rEnvColorA = initParams->envColor.a;
    this->rTexIndex = 0;
    this->rScale = initParams->scale;
    this->rScaleStep = initParams->scaleStep;
    this->actor = initParams->actor;

    return 1;
}

void Effect_SS_Spk_disp_mode(PlayState* play, u32 index, EffectSs* this) {
    static void* spark1_txt[] = {
        gEffSpark1Tex,
        gEffSpark2Tex,
        gEffSpark3Tex,
        gEffSpark4Tex,
    };
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTransBillboard;
    Mtx* mtx;
    f32 scale;
    s32 pad;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_g_spk.c", 208);

    scale = this->rScale * 0.0025f;
    Skin_Matrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    Skin_Matrix_SetScale(&mfScale, scale, scale, 1.0f);
    Skin_Matrix_MulMatrix(&mfTrans, &play->billboardMtxF, &mfTransBillboard);
    Skin_Matrix_MulMatrix(&mfTransBillboard, &mfScale, &mfResult);

    mtx = Skin_Matrix_to_Mtx_new(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(spark1_txt[this->rTexIndex]));
        texture_z_cld_poly_xlu(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, this->rEnvColorA);
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
    }

    if (1) {}
    CLOSE_DISPS(gfxCtx, "../z_eff_ss_g_spk.c", 255);
}

void Effect_SS_Spk_func_proc(PlayState* play, u32 index, EffectSs* this) {

    this->accel.x = (fqrand() - 0.5f) * 3.0f;
    this->accel.z = (fqrand() - 0.5f) * 3.0f;

    if (this->actor != NULL) {
        if ((this->actor->category == ACTORCAT_EXPLOSIVE) && (this->actor->update != NULL)) {
            this->pos.x = this->actor->world.pos.x + this->vec.x;
            this->pos.y = this->actor->world.pos.y + this->vec.y;
            this->pos.z = this->actor->world.pos.z + this->vec.z;
        }
    }

    this->vec.x += this->accel.x;
    this->vec.z += this->accel.z;

    this->rTexIndex++;
    this->rTexIndex &= 3;
    this->rScale += this->rScaleStep;
}

// this update mode is unused in the original game
// with this update mode, the sparks don't move randomly in the xz plane, appearing to be on top of each other
void Effect_SS_Spk_func_proc2(PlayState* play, u32 index, EffectSs* this) {
    if (this->actor != NULL) {
        if ((this->actor->category == ACTORCAT_EXPLOSIVE) && (this->actor->update != NULL)) {
            this->pos.x += (sin_s(this->actor->world.rot.y) * this->actor->speed);
            this->pos.z += (cos_s(this->actor->world.rot.y) * this->actor->speed);
        }
    }

    this->rTexIndex++;
    this->rTexIndex &= 3;
    this->rScale += this->rScaleStep;
}
