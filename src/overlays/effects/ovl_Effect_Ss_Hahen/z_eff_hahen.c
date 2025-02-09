/*
 * File: z_eff_ss_hahen.c
 * Overlay: ovl_Effect_Ss_Hahen
 * Description: Fragments
 */

#include "z_eff_ss_hahen.h"

#include "libc64/qrand.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "segmented_address.h"
#include "sys_matrix.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define rPitch regs[0]
#define rYaw regs[1]
#define rUnused regs[2]
#define rScale regs[3]
#define rObjId regs[4]
#define rObjectSlot regs[5]
#define rMinLife regs[6]

u32 Effect_Ss_Hahen_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_Haka_Tubo_Hahen_disp(PlayState* play, u32 index, EffectSs* this);
void Effect_Hahen_disp(PlayState* play, u32 index, EffectSs* this);
void Effect_Hahen_move(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Hahen_Profile = {
    EFFECT_SS_HAHEN,
    Effect_Ss_Hahen_ct,
};

static void check_shape_bank(EffectSs* this, PlayState* play) {
    if (((this->rObjectSlot = Object_Exchange_bank_check(&play->objectCtx, this->rObjId)) < 0) ||
        !Object_Exchange_bank_dma_check(&play->objectCtx, this->rObjectSlot)) {
        this->life = -1;
        this->draw = NULL;
    }
}

u32 Effect_Ss_Hahen_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsHahenInitParams* initParams = (EffectSsHahenInitParams*)initParamsx;

    this->pos = initParams->pos;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;
    this->life = 200;

    if (initParams->dList != NULL) {
        this->gfx = initParams->dList;
        this->rObjId = initParams->objId;
        check_shape_bank(this, play);
    } else {
        this->gfx = SEGMENTED_TO_VIRTUAL(gEffFragments1DL);
        this->rObjId = -1;
    }

    if ((this->rObjId == OBJECT_HAKA_OBJECTS) && (this->gfx == gEffFragments2DL)) {
        this->draw = Effect_Haka_Tubo_Hahen_disp;
    } else {
        this->draw = Effect_Hahen_disp;
    }

    this->update = Effect_Hahen_move;
    this->rUnused = initParams->unused;
    this->rScale = initParams->scale;
    this->rPitch = fqrand() * 314.0f;
    this->rYaw = fqrand() * 314.0f;
    this->rMinLife = 200 - initParams->life;

    return 1;
}

void Effect_Hahen_disp(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;
    f32 scale = this->rScale * 0.001f;

    OPEN_DISPS(gfxCtx, "../z_eff_hahen.c", 208);

    if (this->rObjId != -1) {
        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->rObjectSlot].segment);
    }

    Matrix_translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_rotateY(this->rYaw * 0.01f, MTXMODE_APPLY);
    Matrix_rotateX(this->rPitch * 0.01f, MTXMODE_APPLY);
    Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx, "../z_eff_hahen.c", 228);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, this->gfx);

    CLOSE_DISPS(gfxCtx, "../z_eff_hahen.c", 236);
}

// in the original game this function is hardcoded to be used only by the skull pots in Shadow Temple
void Effect_Haka_Tubo_Hahen_disp(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;
    f32 scale = this->rScale * 0.001f;

    OPEN_DISPS(gfxCtx, "../z_eff_hahen.c", 253);

    if (this->rObjId != -1) {
        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->rObjectSlot].segment);
    }

    Matrix_translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_rotateY(this->rYaw * 0.01f, MTXMODE_APPLY);
    Matrix_rotateX(this->rPitch * 0.01f, MTXMODE_APPLY);
    Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx, "../z_eff_hahen.c", 271);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    gDPSetCombineLERP(POLY_OPA_DISP++, SHADE, 0, PRIMITIVE, 0, SHADE, 0, PRIMITIVE, 0, SHADE, 0, PRIMITIVE, 0, SHADE, 0,
                      PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0x0, 0x01, 100, 100, 120, 255);
    gSPDisplayList(POLY_OPA_DISP++, this->gfx);

    CLOSE_DISPS(gfxCtx, "../z_eff_hahen.c", 288);
}

void Effect_Hahen_move(PlayState* play, u32 index, EffectSs* this) {
    Player* player = GET_PLAYER(play);

    this->rPitch += 55;
    this->rYaw += 10;

    if ((this->pos.y <= player->actor.floorHeight) && (this->life < this->rMinLife)) {
        this->life = 0;
    }

    if (this->rObjId != -1) {
        check_shape_bank(this, play);
    }
}
