/*
 * File: z_eff_ss_stick.c
 * Overlay: ovl_Effect_Ss_Stick
 * Description: Broken stick as child, broken sword as adult
 */

#include "z_eff_ss_stick.h"
#include "assets/objects/object_link_boy/object_link_boy.h"
#include "assets/objects/object_link_child/object_link_child.h"

#define rObjectSlot regs[0]
#define rYaw regs[1]

u32 Effect_Ss2_Stick_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_Stick_disp_mode(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_Stick_func_proc(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Stick_Profile = {
    EFFECT_SS_STICK,
    Effect_Ss2_Stick_ct,
};

typedef struct StickDrawInfo {
    /* 0x00 */ s16 objectId;
    /* 0x04 */ Gfx* displayList;
} StickDrawInfo;

u32 Effect_Ss2_Stick_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    StickDrawInfo drawInfo[] = {
        { OBJECT_LINK_BOY, gLinkAdultBrokenGiantsKnifeBladeDL }, // adult, broken sword
        { OBJECT_LINK_CHILD, gLinkChildLinkDekuStickDL },        // child, broken stick
    };
    StickDrawInfo* ageInfoEntry = z_common_data.save.linkAge + drawInfo;
    EffectSsStickInitParams* initParams = (EffectSsStickInitParams*)initParamsx;

    this->rObjectSlot = Object_Exchange_bank_check(&play->objectCtx, ageInfoEntry->objectId);
    this->gfx = ageInfoEntry->displayList;
    this->vec = this->pos = initParams->pos;
    this->rYaw = initParams->yaw;
    this->velocity.x = sin_s(initParams->yaw) * 6.0f;
    this->velocity.z = cos_s(initParams->yaw) * 6.0f;
    this->life = 20;
    this->draw = Effect_SS_Stick_disp_mode;
    this->update = Effect_SS_Stick_func_proc;
    this->velocity.y = 26.0f;
    this->accel.y = -4.0f;

    return 1;
}

void Effect_SS_Stick_disp_mode(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_stick.c", 153);

    Matrix_translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);

    if (!LINK_IS_ADULT) {
        Matrix_scale(0.01f, 0.0025f, 0.01f, MTXMODE_APPLY);
        Matrix_rotateXYZ(0, this->rYaw, 0, MTXMODE_APPLY);
    } else {
        Matrix_scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
        Matrix_rotateXYZ(0, this->rYaw, play->state.frames * 10000, MTXMODE_APPLY);
    }

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx, "../z_eff_ss_stick.c", 176);
    _texture_z_light_fog_prim(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->rObjectSlot].segment);
    gSPSegment(POLY_OPA_DISP++, 0x0C, set_cull_back_model);
    gSPDisplayList(POLY_OPA_DISP++, this->gfx);

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_stick.c", 188);
}

void Effect_SS_Stick_func_proc(PlayState* play, u32 index, EffectSs* this) {
}
