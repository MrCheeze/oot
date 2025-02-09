/*
 * File: z_eff_ss_dead_ds.c
 * Overlay: ovl_Effect_Ss_Dead_Ds
 * Description: Burn mark on the floor
 */

#include "z_eff_ss_dead_ds.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]
#define rTimer regs[1]
#define rRoll regs[2]
#define rPitch regs[3]
#define rYaw regs[4]
#define rAlpha regs[5]
#define rScaleStep regs[9]
#define rAlphaStep regs[10]
#define rHalfOfLife regs[11]

u32 Effect_SS_Dead_Ds_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_Ds_disp_mode(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_Ds_func_proc(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Dead_Ds_Profile = {
    EFFECT_SS_DEAD_DS,
    Effect_SS_Dead_Ds_ct,
};

u32 Effect_SS_Dead_Ds_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsDeadDsInitParams* initParams = (EffectSsDeadDsInitParams*)initParamsx;

    this->pos = initParams->pos;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;
    this->life = initParams->life;
    this->rScaleStep = initParams->scaleStep;
    this->rHalfOfLife = initParams->life / 2;
    this->rAlphaStep = initParams->alpha / this->rHalfOfLife;
    this->draw = Effect_SS_Ds_disp_mode;
    this->update = Effect_SS_Ds_func_proc;
    this->rScale = initParams->scale;
    this->rAlpha = initParams->alpha;
    this->rTimer = 0;

    return 1;
}

void Effect_SS_Ds_disp_mode(PlayState* play, u32 index, EffectSs* this) {
    s32 pad;
    f32 scale;
    s32 pad1;
    s32 pad2;
    MtxF mf;
    f32 yIntersect;
    Vec3f pos;
    CollisionPoly* groundPoly;

    OPEN_DISPS(play->state.gfxCtx, "../z_eff_ss_dead_ds.c", 157);

    scale = this->rScale * 0.01f;
    texture_z_cld_poly_xlu(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, this->rAlpha);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 0);
    pos = this->pos;

    if (this->rTimer == 0) {
        Vec3s rpy;
        Vec3f prevPos;

        prevPos.x = pos.x - this->velocity.x;
        prevPos.y = pos.y - this->velocity.y;
        prevPos.z = pos.z - this->velocity.z;

        if (T_BGCheck_ObjWallCheck2(&play->colCtx, &this->pos, &pos, &prevPos, 1.5f, &groundPoly, 1.0f)) {
            T_Polygon_Ground_Matrix(groundPoly, this->pos.x, this->pos.y, this->pos.z, &mf);
            Matrix_put(&mf);
        } else {
            pos.y++;
            yIntersect = T_BGCheck_ObjGroundCheck(&play->colCtx, &groundPoly, &pos);

            if (groundPoly != NULL) {
                T_Polygon_Ground_Matrix(groundPoly, this->pos.x, yIntersect + 1.5f, this->pos.z, &mf);
                Matrix_put(&mf);
            } else {
                Matrix_translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
                Matrix_get(&mf);
            }
        }

        Matrix_to_rotate2_new(&mf, &rpy, 0);
        this->rRoll = rpy.x;
        this->rPitch = rpy.y;
        this->rYaw = rpy.z;
        this->pos.y = mf.yw;
        this->rTimer++;
    }

    Matrix_translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_rotateXYZ(this->rRoll, this->rPitch, this->rYaw, MTXMODE_APPLY);
    Matrix_rotateX(1.57f, MTXMODE_APPLY);
    Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_eff_ss_dead_ds.c", 246);
    gDPSetCombineLERP(POLY_XLU_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);
    gSPDisplayList(POLY_XLU_DISP++, gLensFlareCircleDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_eff_ss_dead_ds.c", 255);
}

void Effect_SS_Ds_func_proc(PlayState* play, u32 index, EffectSs* this) {
    if (this->life < this->rHalfOfLife) {

        this->rScale += this->rScaleStep;
        if (this->rScale < 0) {
            this->rScale = 0;
        }

        this->rAlpha -= this->rAlphaStep;
        if (this->rAlpha < 0) {
            this->rAlpha = 0;
        }
    }
}
