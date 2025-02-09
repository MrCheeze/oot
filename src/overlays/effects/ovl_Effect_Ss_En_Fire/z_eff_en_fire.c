/*
 * File: z_eff_ss_en_fire.c
 * Overlay: ovl_Effect_Ss_En_Fire
 * Description:
 */

#include "z_eff_ss_en_fire.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define rScaleMax regs[0]
#define rScale regs[1]
#define rLifespan regs[2]
#define rUnused regs[3]
#define rPitch regs[4]
#define rYaw regs[5]
#define rReg6 regs[6]
#define rBodyPart regs[7]
#define rFlags regs[8]
#define rScroll regs[9]

u32 Effect_Ss_En_Fire_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_En_Fire_disp_mode(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_En_Fire_func_proc(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_En_Fire_Profile = {
    EFFECT_SS_EN_FIRE,
    Effect_Ss_En_Fire_ct,
};

u32 Effect_Ss_En_Fire_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsEnFireInitParams* initParams = (EffectSsEnFireInitParams*)initParamsx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    this->pos = initParams->pos;
    this->velocity = this->accel = zeroVec;
    this->life = 20;
    this->rLifespan = this->life;
    this->actor = initParams->actor;
    this->rScroll = fqrand() * 20.0f;
    this->draw = Effect_SS_En_Fire_disp_mode;
    this->update = Effect_SS_En_Fire_func_proc;
    this->rUnused = -15;

    if (initParams->bodyPart < 0) {
        this->rYaw = search_position_angleY(&initParams->actor->world.pos, &initParams->pos) - initParams->actor->shape.rot.y;
        this->rPitch =
            search_position_angleX(&initParams->actor->world.pos, &initParams->pos) - initParams->actor->shape.rot.x;
        this->vec.z = search_position_distance(&initParams->pos, &initParams->actor->world.pos);
    }

    this->rScaleMax = initParams->scale;

    if ((initParams->unk_12 & 0x8000) != 0) {
        this->rScale = initParams->scale;
    } else {
        this->rScale = 0;
    }

    this->rReg6 = initParams->unk_12 & 0x7FFF;
    this->rBodyPart = initParams->bodyPart;
    this->rFlags = initParams->flags;

    return 1;
}

void Effect_SS_En_Fire_disp_mode(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 scale;
    s16 camYaw;
    s32 pad[3];
    s16 intensity;
    s16 redGreen;

    OPEN_DISPS(gfxCtx, "../z_eff_en_fire.c", 169);

    Matrix_translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    camYaw = (getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x8000);
    Matrix_rotateY(BINANG_TO_RAD(camYaw), MTXMODE_APPLY);

    scale = sin_s(this->life * 0x333) * (this->rScale * 0.00005f);
    Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_eff_en_fire.c", 180);

    intensity = this->life - 5;

    if (intensity < 0) {
        intensity = 0;
    }

    redGreen = intensity;
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, redGreen * 12.7f, 0, 0, 0);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x0, 0x80, redGreen * 12.7f, redGreen * 12.7f, 0, 255);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x40, 1, 0,
                                (this->rScroll * -0x14) & 0x1FF, 0x20, 0x80));

    if (((this->rFlags & 0x7FFF) != 0) || (this->life < 18)) {
        gSPDisplayList(POLY_XLU_DISP++, gEffFire2DL);
    } else {
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_en_fire.c", 213);
}

typedef struct FireActorF {
    /* 0x000 */ Actor actor;
    /* 0x14C */ Vec3f firePos[10];
} FireActorF;

typedef struct FireActorS {
    /* 0x000 */ Actor actor;
    /* 0x14C */ Vec3s firePos[10];
} FireActorS;

void Effect_SS_En_Fire_func_proc(PlayState* play, u32 index, EffectSs* this) {

    this->rScroll++;

    if (this->actor != NULL) {
        if (this->actor->colorFilterTimer >= 22) {
            this->life++;
        }
        if (this->actor->update != NULL) {
            add_calc_short_angle2(&this->rScale, this->rScaleMax, 1, this->rScaleMax >> 3, 0);

            if (this->rBodyPart < 0) {
                Matrix_translate(this->actor->world.pos.x, this->actor->world.pos.y, this->actor->world.pos.z,
                                 MTXMODE_NEW);
                Matrix_rotateY(BINANG_TO_RAD(this->rYaw + this->actor->shape.rot.y), MTXMODE_APPLY);
                Matrix_rotateX(BINANG_TO_RAD(this->rPitch + this->actor->shape.rot.x), MTXMODE_APPLY);
                Matrix_Position(&this->vec, &this->pos);
            } else {
                if (this->rFlags & 0x8000) {
                    this->pos.x = ((FireActorS*)this->actor)->firePos[this->rBodyPart].x;
                    this->pos.y = ((FireActorS*)this->actor)->firePos[this->rBodyPart].y;
                    this->pos.z = ((FireActorS*)this->actor)->firePos[this->rBodyPart].z;
                } else {
                    this->pos.x = ((FireActorF*)this->actor)->firePos[this->rBodyPart].x;
                    this->pos.y = ((FireActorF*)this->actor)->firePos[this->rBodyPart].y;
                    this->pos.z = ((FireActorF*)this->actor)->firePos[this->rBodyPart].z;
                }
            }
        } else if (this->rReg6 != 0) {
            this->life = 0;
        } else {
            this->actor = NULL;
        }
    }
}
