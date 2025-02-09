/*
 * File: z_eff_ss_dt_bubble.c
 * Overlay: ovl_Effect_Ss_Dt_Bubble
 * Description: Bubbles (a random mix of translucent and opaque)
 */

#include "z_eff_ss_dt_bubble.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorB regs[2]
#define rPrimColorA regs[3]
#define rEnvColorR regs[4]
#define rEnvColorG regs[5]
#define rEnvColorB regs[6]
#define rEnvColorA regs[7]
#define rRandXZ regs[8]
#define rScale regs[9]
#define rLifespan regs[10]

u32 Effect_Ss_Dt_Bubble_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_Dt_Bubble_disp_mode(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_Dt_Bubble_func_proc(PlayState* play, u32 index, EffectSs* this);

static Color_RGBA8 dt_bubble_prim[] = { { 255, 255, 100, 255 }, { 150, 255, 255, 255 }, { 100, 255, 255, 255 } };
static Color_RGBA8 dt_bubble_env[] = { { 170, 0, 0, 255 }, { 0, 100, 0, 255 }, { 0, 0, 255, 255 } };

EffectSsProfile Effect_Ss_Dt_Bubble_Profile = {
    EFFECT_SS_DT_BUBBLE,
    Effect_Ss_Dt_Bubble_ct,
};

u32 Effect_Ss_Dt_Bubble_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsDtBubbleInitParams* initParams = (EffectSsDtBubbleInitParams*)initParamsx;

    //! @bug fqrand in the macro means a random number is generated for both parts of the macro.
    // In the base game this works out because both addresses are segment 4, but it may break if
    // the addresses were changed to refer to different segments
    this->gfx = SEGMENTED_TO_VIRTUAL(fqrand() < 0.5f ? gEffBubble1Tex : gEffBubble2Tex);
    this->pos = initParams->pos;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;
    this->life = initParams->life;

    if (!initParams->customColor) {
        this->rPrimColorR = dt_bubble_prim[initParams->colorProfile].r;
        this->rPrimColorG = dt_bubble_prim[initParams->colorProfile].g;
        this->rPrimColorB = dt_bubble_prim[initParams->colorProfile].b;
        this->rPrimColorA = dt_bubble_prim[initParams->colorProfile].a;
        this->rEnvColorR = dt_bubble_env[initParams->colorProfile].r;
        this->rEnvColorG = dt_bubble_env[initParams->colorProfile].g;
        this->rEnvColorB = dt_bubble_env[initParams->colorProfile].b;
        this->rEnvColorA = dt_bubble_env[initParams->colorProfile].a;
    } else {
        this->rPrimColorR = initParams->primColor.r;
        this->rPrimColorG = initParams->primColor.g;
        this->rPrimColorB = initParams->primColor.b;
        this->rPrimColorA = initParams->primColor.a;
        this->rEnvColorR = initParams->envColor.r;
        this->rEnvColorG = initParams->envColor.g;
        this->rEnvColorB = initParams->envColor.b;
        this->rEnvColorA = initParams->envColor.a;
    }

    this->rRandXZ = initParams->randXZ;
    this->rScale = initParams->scale;
    this->rLifespan = initParams->life;
    this->draw = Effect_SS_Dt_Bubble_disp_mode;
    this->update = Effect_SS_Dt_Bubble_func_proc;

    return 1;
}

void Effect_SS_Dt_Bubble_disp_mode(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 scale;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_dt_bubble.c", 201);

    scale = this->rScale * 0.004f;
    Matrix_translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_eff_ss_dt_bubble.c", 213);
    texture_z_light_prim_xlu_disp(gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                    (this->rPrimColorA * this->life) / this->rLifespan);
    gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB,
                   (this->rEnvColorA * this->life) / this->rLifespan);
    gSPSegment(POLY_XLU_DISP++, 0x08, this->gfx);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gEffBubbleDL));

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_dt_bubble.c", 236);
}

void Effect_SS_Dt_Bubble_func_proc(PlayState* play, u32 index, EffectSs* this) {
    f32 rand;

    if (this->rRandXZ == 1) {
        rand = fqrand();
        this->pos.x += (rand * 2.0f) - 1.0f;

        rand = fqrand();
        this->pos.z += (rand * 2.0f) - 1.0f;
    }
}
