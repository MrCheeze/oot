/*
 * File: z_eff_ss_extra.c
 * Overlay: ovl_Effect_Ss_Extra
 * Description: Minigame Score Points
 */

#include "z_eff_ss_extra.h"
#include "assets/objects/object_yabusame_point/object_yabusame_point.h"

#define rObjectSlot regs[0]
#define rTimer regs[1]
#define rScoreIdx regs[2]
#define rScale regs[3]

u32 Effect_Ss2_Extra_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_Extra_disp(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_Extra_move(PlayState* play, u32 index, EffectSs* this);

static s16 score_data[] = { 30, 60, 100 };

EffectSsProfile Effect_Ss_Extra_Profile = {
    EFFECT_SS_EXTRA,
    Effect_Ss2_Extra_ct,
};

u32 Effect_Ss2_Extra_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsExtraInitParams* initParams = (EffectSsExtraInitParams*)initParamsx;
    s32 pad;
    s32 objectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_YABUSAME_POINT);

    if ((objectSlot >= 0) && Object_Exchange_bank_dma_check(&play->objectCtx, objectSlot)) {
        uintptr_t oldSeg6 = SegmentBaseAddress[6];

        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);
        this->pos = initParams->pos;
        this->velocity = initParams->velocity;
        this->accel = initParams->accel;
        this->draw = Effect_SS_Extra_disp;
        this->update = Effect_SS_Extra_move;
        this->life = 50;
        this->rScoreIdx = initParams->scoreIdx;
        this->rScale = initParams->scale;
        this->rTimer = 5;
        this->rObjectSlot = objectSlot;
        SegmentBaseAddress[6] = oldSeg6;

        return 1;
    }

    return 0;
}

static void* point_txt[] = {
    object_yabusame_point_Tex_000000,
    object_yabusame_point_Tex_000480,
    object_yabusame_point_Tex_000900,
};

void Effect_SS_Extra_disp(PlayState* play, u32 index, EffectSs* this) {
    s32 pad;
    f32 scale = this->rScale / 100.0f;
    void* objectPtr = play->objectCtx.slots[this->rObjectSlot].segment;

    OPEN_DISPS(play->state.gfxCtx, "../z_eff_ss_extra.c", 168);

    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(objectPtr);
    gSPSegment(POLY_XLU_DISP++, 0x06, objectPtr);
    Matrix_translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    Matrix_rotate_scale_exchange(&play->billboardMtxF);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_eff_ss_extra.c", 186);
    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(point_txt[this->rScoreIdx]));
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_yabusame_point_DL_000DC0));

    CLOSE_DISPS(play->state.gfxCtx, "../z_eff_ss_extra.c", 194);
}

void Effect_SS_Extra_move(PlayState* play, u32 index, EffectSs* this) {
    if (this->rTimer != 0) {
        this->rTimer--;
    } else {
        this->velocity.y = 0.0f;
    }

    if (this->rTimer == 1) {
        play->interfaceCtx.unk_23C = score_data[this->rScoreIdx];
    }
}
