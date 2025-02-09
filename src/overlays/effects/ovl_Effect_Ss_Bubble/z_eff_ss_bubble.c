/*
 * File: z_eff_ss_bubble.c
 * Overlay: ovl_Effect_Ss_Bubble
 * Description:
 */

#include "z_eff_ss_bubble.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]

u32 Effect_SS2_Bubble_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_Bubble_disp_mode(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_Bubble_func_proc(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Bubble_Profile = {
    EFFECT_SS_BUBBLE,
    Effect_SS2_Bubble_ct,
};

u32 Effect_SS2_Bubble_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsBubbleInitParams* initParams = (EffectSsBubbleInitParams*)initParamsx;

    //! @bug fqrand in the macro means a random number is generated for both parts of the macro.
    // In the base game this works out because both addresses are segment 4, but it may break if
    // the addresses were changed to refer to different segments
    this->gfx = SEGMENTED_TO_VIRTUAL(fqrand() < 0.5f ? gEffBubble1Tex : gEffBubble2Tex);
    this->pos.x = ((fqrand() - 0.5f) * initParams->xzPosRandScale) + initParams->pos.x;
    this->pos.y = (((fqrand() - 0.5f) * initParams->yPosRandScale) + initParams->yPosOffset) + initParams->pos.y;
    this->pos.z = ((fqrand() - 0.5f) * initParams->xzPosRandScale) + initParams->pos.z;
    xyz_t_move(&this->vec, &this->pos);
    this->life = 1;
    this->rScale = (((fqrand() * 0.5f) + 1.0f) * initParams->scale) * 100;
    this->draw = Effect_SS_Bubble_disp_mode;
    this->update = Effect_SS_Bubble_func_proc;

    return 1;
}

void Effect_SS_Bubble_disp_mode(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 scale = this->rScale / 100.0f;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_bubble.c", 154);

    Matrix_translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx, "../z_eff_ss_bubble.c", 167);
    _texture_z_light_fog_prim(gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 150, 150, 150, 0);
    gSPSegment(POLY_OPA_DISP++, 0x08, this->gfx);
    gSPDisplayList(POLY_OPA_DISP++, SEGMENTED_TO_VIRTUAL(gEffBubbleDL));

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_bubble.c", 179);
}

void Effect_SS_Bubble_func_proc(PlayState* play, u32 index, EffectSs* this) {
    WaterBox* waterBox;
    f32 waterSurfaceY = this->pos.y;

    // kill bubble if it's out of range of a water box
    if (!T_BGCheck_WaterSurfaceCheck3(play, &play->colCtx, this->pos.x, this->pos.z, &waterSurfaceY, &waterBox)) {
        this->life = -1;
        return;
    }

    if (waterSurfaceY < this->pos.y) {
        Vec3f ripplePos;

        ripplePos.x = this->pos.x;
        ripplePos.y = waterSurfaceY;
        ripplePos.z = this->pos.z;
        Effect_SS_G_Ripple_ct2(play, &ripplePos, 0, 80, 0);
        this->life = -1;
    } else {
        this->life++;
        this->pos.x = ((fqrand() * 0.5f) - 0.25f) + this->vec.x;
        this->accel.y = (fqrand() - 0.3f) * 0.2f;
        this->pos.z = ((fqrand() * 0.5f) - 0.25f) + this->vec.z;
    }
}
