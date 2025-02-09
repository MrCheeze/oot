/*
 * File: z_eff_ss_fire_tail.c
 * Overlay: ovl_Effect_Ss_Fire_Tail
 * Description: Fire (burned by something)
 */

#include "z_eff_ss_fire_tail.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]
#define rLifespan regs[1]
#define rReg2 regs[2]
#define rReg3 regs[3]
#define rPrimColorR regs[4]
#define rPrimColorG regs[5]
#define rPrimColorB regs[6]
#define rEnvColorR regs[7]
#define rEnvColorG regs[8]
#define rEnvColorB regs[9]
#define rReg10 regs[10]
#define rBodyPart regs[11]
#define rType regs[12]

u32 Effect_Ss2_Fire_Tail_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_Fire_Tail_disp_mode(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_Fire_Tail_func_proc(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Fire_Tail_Profile = {
    EFFECT_SS_FIRE_TAIL,
    Effect_Ss2_Fire_Tail_ct,
};

u32 Effect_Ss2_Fire_Tail_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsFireTailInitParams* initParams = (EffectSsFireTailInitParams*)initParamsx;

    this->pos = initParams->pos;
    this->vec = initParams->unk_14;
    this->velocity.x = 0.0f;
    this->velocity.y = 0.0f;
    this->velocity.z = 0.0f;
    this->accel.x = 0.0f;
    this->accel.y = 0.0f;
    this->accel.z = 0.0f;
    this->life = initParams->life;
    this->actor = initParams->actor;
    this->draw = Effect_SS_Fire_Tail_disp_mode;
    this->update = Effect_SS_Fire_Tail_func_proc;
    this->rScale = initParams->scale * 1000.0f;
    this->rLifespan = initParams->life;
    this->rReg2 = -0xA;
    this->rReg3 = -0xF;
    if (initParams->unk_20 == 0) {
        initParams->unk_20 = 1;
    }
    this->rReg10 = initParams->unk_20;
    this->rPrimColorR = initParams->primColor.r;
    this->rPrimColorG = initParams->primColor.g;
    this->rPrimColorB = initParams->primColor.b;
    this->rEnvColorR = initParams->envColor.r;
    this->rEnvColorG = initParams->envColor.g;
    this->rEnvColorB = initParams->envColor.b;
    this->rBodyPart = initParams->bodyPart;
    this->rType = initParams->type;

    return 1;
}

void Effect_SS_Fire_Tail_disp_mode(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;
    s16 yaw;
    Vec3f scale;
    f32 temp1;
    f32 temp2;
    f32 dist;

    OPEN_DISPS(gfxCtx, "../z_eff_fire_tail.c", 182);

    scale.x = scale.y = scale.z = 0.0f;

    if (this->actor != NULL) {

        this->vec = this->actor->velocity;

        if (this->rBodyPart < 0) {
            Matrix_translate(this->pos.x + this->actor->world.pos.x, this->pos.y + this->actor->world.pos.y,
                             this->pos.z + this->actor->world.pos.z, MTXMODE_NEW);
        } else {
            Player* player = GET_PLAYER(play);
            s16 bodyPart = this->rBodyPart;

            this->pos.x =
                player->bodyPartsPos[bodyPart].x - (sin_s(getRealCameraAngleY(GET_ACTIVE_CAM(play))) * 5.0f);
            this->pos.y = player->bodyPartsPos[bodyPart].y;
            this->pos.z =
                player->bodyPartsPos[bodyPart].z - (cos_s(getRealCameraAngleY(GET_ACTIVE_CAM(play))) * 5.0f);

            Matrix_translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
        }
    } else {
        Matrix_translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    }

    yaw = search_position_angleY(&scale, &this->vec) - getRealCameraAngleY(GET_ACTIVE_CAM(play));
    temp1 = fabsf(cos_s(yaw));
    temp2 = sin_s(yaw);
    dist = search_position_distanceXZ(&scale, &this->vec) / (this->rReg10 * 0.1f);
    Matrix_rotateY(BINANG_TO_RAD((s16)(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x8000)), MTXMODE_APPLY);
    Matrix_rotateZ(DEG_TO_RAD(temp2 * this->rReg2 * dist), MTXMODE_APPLY);
    temp2 = 1.0f - ((f32)(this->life + 1) / this->rLifespan);
    temp2 = 1.0f - SQ(temp2);
    scale.x = scale.y = scale.z = temp2 * (this->rScale * 0.000010000001f);
    Matrix_scale(scale.x, scale.y, scale.z, MTXMODE_APPLY);
    temp1 = (this->rReg3 * 0.01f * temp1 * dist) + 1.0f;

    if (temp1 < 0.1f) {
        temp1 = 0.1f;
    }

    Matrix_scale(1.0f, temp1, 1.0f / temp1, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_eff_fire_tail.c", 238);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, 0);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 64, 1, 0,
                                (play->state.frames * -0x14) & 0x1FF, 32, 128));

    if (this->rType != 0) {
        gSPDisplayList(POLY_XLU_DISP++, gEffFire2DL);
    } else {
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_fire_tail.c", 273);
}

void Effect_SS_Fire_Tail_func_proc(PlayState* play, u32 index, EffectSs* this) {
    this->rScale *= 0.9f;
}
