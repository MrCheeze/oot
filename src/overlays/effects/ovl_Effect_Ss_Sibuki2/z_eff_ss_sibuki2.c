/*
 * File: z_eff_ss_sibuki2.c
 * Overlay: ovl_Effect_Ss_Sibuki2
 * Description: Unfinished and unused bubble effect
 */

#include "z_eff_ss_sibuki2.h"
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

u32 Effect_Ss2_Sibuki2_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_Sibuki2_disp(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_Sibuki2_move(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Sibuki2_Profile = {
    EFFECT_SS_SIBUKI2,
    Effect_Ss2_Sibuki2_ct,
};

u32 Effect_Ss2_Sibuki2_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsSibuki2InitParams* initParams = (EffectSsSibuki2InitParams*)initParamsx;

    this->pos = initParams->pos;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;
    this->life = 10;
    this->draw = Effect_SS_Sibuki2_disp;
    this->update = Effect_SS_Sibuki2_move;
    this->rScale = initParams->scale;
    this->rPrimColorR = 255;
    this->rPrimColorG = 255;
    this->rPrimColorB = 255;
    this->rPrimColorA = 255;
    this->rEnvColorR = 100;
    this->rEnvColorG = 100;
    this->rEnvColorB = 100;
    this->rEnvColorA = 255;
    this->rTexIndex = 0;

    return 1;
}

void Effect_SS_Sibuki2_disp(PlayState* play, u32 index, EffectSs* this) {
    static void* sibuki2_txt[] = {
        gEffUnusedBubbles1Tex, gEffUnusedBubbles1Tex, gEffUnusedBubbles2Tex,
        gEffUnusedBubbles3Tex, gEffUnusedBubbles4Tex, gEffUnusedBubbles5Tex,
        gEffUnusedBubbles6Tex, gEffUnusedBubbles7Tex, gEffUnusedBubbles8Tex,
    };
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 scale = this->rScale / 100.0f;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_sibuki2.c", 158);

    Matrix_translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_eff_ss_sibuki2.c", 171);
    _texture_z_light_fog_prim(gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB, this->rPrimColorA);
    gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, this->rEnvColorA);
    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sibuki2_txt[this->rTexIndex]));
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gEffUnusedBubblesDL));

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_sibuki2.c", 198);
}

void Effect_SS_Sibuki2_move(PlayState* play, u32 index, EffectSs* this) {
    if (this->rTexIndex < 8) {
        this->rTexIndex++;
    }
}
