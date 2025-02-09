/*
 * File: z_eff_ss_sibuki.c
 * Overlay: ovl_Effect_Ss_Sibuki
 * Description: Bubbles (only used by gohma and gohmalings in the original game)
 */

#include "z_eff_ss_sibuki.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorB regs[2]
#define rPrimColorA regs[3]
#define rEnvColorR regs[4]
#define rEnvColorG regs[5]
#define rEnvColorB regs[6]
#define rEnvColorA regs[7]
#define rMoveDelay regs[8]
#define rDirection regs[9]
#define rScale regs[10]

u32 Effect_Ss2_Sibuki_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_Sibuki_disp(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_Sibuki_move(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Sibuki_Profile = {
    EFFECT_SS_SIBUKI,
    Effect_Ss2_Sibuki_ct,
};

u32 Effect_Ss2_Sibuki_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsSibukiInitParams* initParams = (EffectSsSibukiInitParams*)initParamsx;

    this->pos = initParams->pos;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;

    if (KREG(2) != 0) {
        this->gfx = SEGMENTED_TO_VIRTUAL(gEffBubble2Tex);
    } else {
        this->gfx = SEGMENTED_TO_VIRTUAL(gEffBubble1Tex);
    }

    this->life = ((s16)((fqrand() * (500.0f + KREG(64))) * 0.01f)) + KREG(65) + 10;
    this->rMoveDelay = initParams->moveDelay + 1;
    this->draw = Effect_SS_Sibuki_disp;
    this->update = Effect_SS_Sibuki_move;
    this->rDirection = initParams->direction;
    this->rScale = initParams->scale;
    this->rPrimColorR = 100;
    this->rPrimColorG = 100;
    this->rPrimColorB = 100;
    this->rPrimColorA = 100;
    this->rEnvColorR = 255;
    this->rEnvColorG = 255;
    this->rEnvColorB = 255;
    this->rEnvColorA = 255;

    return 1;
}

void Effect_SS_Sibuki_disp(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 scale = this->rScale / 100.0f;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_sibuki.c", 163);

    Matrix_translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx, "../z_eff_ss_sibuki.c", 176);
    _texture_z_light_fog_prim(gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB, this->rPrimColorA);
    gDPSetEnvColor(POLY_OPA_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, this->rEnvColorA);
    gSPSegment(POLY_OPA_DISP++, 0x08, this->gfx);
    gSPDisplayList(POLY_OPA_DISP++, SEGMENTED_TO_VIRTUAL(gEffBubbleDL));

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_sibuki.c", 198);
}

void Effect_SS_Sibuki_move(PlayState* play, u32 index, EffectSs* this) {
    Player* player = GET_PLAYER(play);
    s32 pad[2];
    f32 xzVelScale;

    if (this->pos.y <= player->actor.floorHeight) {
        this->life = 0;
    }

    if (this->rMoveDelay != 0) {
        this->rMoveDelay--;

        if (this->rMoveDelay == 0) {
            s16 yaw = getCameraAngleY(Gama_play_get_camera(play, CAM_ID_MAIN));

            xzVelScale = ((200.0f + KREG(20)) * 0.01f) + ((0.1f * fqrand()) * (KREG(23) + 20.0f));

            if (this->rDirection != 0) {
                xzVelScale *= -1.0f;
            }

            this->velocity.x = cos_s(yaw) * xzVelScale;
            this->velocity.z = -sin_s(yaw) * xzVelScale;

            this->velocity.y = ((700.0f + KREG(21)) * 0.01f) + ((0.1f * fqrand()) * (KREG(24) + 20.0f));
            this->accel.y = ((-100.0f + KREG(22)) * 0.01f) + ((0.1f * fqrand()) * KREG(25));

            if (KREG(3) != 0) {
                this->velocity.x *= (KREG(3) * 0.01f);
                this->velocity.y *= (KREG(3) * 0.01f);
                this->velocity.z *= (KREG(3) * 0.01f);
                this->accel.y *= (KREG(4) * 0.01f);
            }
        }
    } else {
        if (this->rScale != 0) {
            this->rScale = (this->rScale - KREG(26)) - 3;
        }
    }
}
