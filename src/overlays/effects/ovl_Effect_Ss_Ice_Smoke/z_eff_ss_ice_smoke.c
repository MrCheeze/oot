/*
 * File: z_eff_ss_ice_smoke.c
 * Overlay: ovl_Effect_Ss_Ice_Smoke
 * Description: Ice Smoke
 */

#include "z_eff_ss_ice_smoke.h"
#include "assets/objects/object_fz/object_fz.h"

#define rObjectSlot regs[0]
#define rAlpha regs[1]
#define rScale regs[2]

u32 Effect_SS_Ice_Smoke_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_Ice_Smoke_disp(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_Ice_Smoke_move(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Ice_Smoke_Profile = {
    EFFECT_SS_ICE_SMOKE,
    Effect_SS_Ice_Smoke_ct,
};

u32 Effect_SS_Ice_Smoke_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsIceSmokeInitParams* initParams = (EffectSsIceSmokeInitParams*)initParamsx;
    s32 pad;
    s32 objectSlot;

    objectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_FZ);

    if ((objectSlot >= 0) && Object_Exchange_bank_dma_check(&play->objectCtx, objectSlot)) {
        uintptr_t prevSeg6 = SegmentBaseAddress[6];

        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);
        xyz_t_move(&this->pos, &initParams->pos);
        xyz_t_move(&this->velocity, &initParams->velocity);
        xyz_t_move(&this->accel, &initParams->accel);
        this->rObjectSlot = objectSlot;
        this->rAlpha = 0;
        this->rScale = initParams->scale;
        this->life = 50;
        this->draw = Effect_SS_Ice_Smoke_disp;
        this->update = Effect_SS_Ice_Smoke_move;
        SegmentBaseAddress[6] = prevSeg6;

        return 1;
    }

    PRINTF("Effect_SS_Ice_Smoke_ct():バンク Object_Bank_Fzが有りません。\n");

    return 0;
}

void Effect_SS_Ice_Smoke_disp(PlayState* play, u32 index, EffectSs* this) {
    PlayState* play2 = (PlayState*)play;
    void* objectPtr;
    Mtx* mtx;
    f32 scale;
    s32 objectSlot;

    objectPtr = play2->objectCtx.slots[this->rObjectSlot].segment;

    OPEN_DISPS(play->state.gfxCtx, "../z_eff_ss_ice_smoke.c", 155);

    objectSlot = Object_Exchange_bank_check(&play2->objectCtx, OBJECT_FZ);

    if ((objectSlot >= 0) && Object_Exchange_bank_dma_check(&play2->objectCtx, objectSlot)) {
        gDPPipeSync(POLY_XLU_DISP++);
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(objectPtr);
        gSPSegment(POLY_XLU_DISP++, 0x06, objectPtr);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gFreezardSteamStartDL));
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 235, 235, this->rAlpha);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, this->life * 3, this->life * 15, 32, 64, 1, 0,
                                    0, 32, 32));
        Matrix_translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
        Matrix_rotate_scale_exchange(&play2->billboardMtxF);
        scale = this->rScale * 0.0001f;
        Matrix_scale(scale, scale, 1.0f, MTXMODE_APPLY);

        mtx = MATRIX_FINALIZE(play->state.gfxCtx, "../z_eff_ss_ice_smoke.c", 196);

        if (mtx != NULL) {
            if (1) {}
            gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gFreezardSteamDL));
        }
    } else {
        this->life = -1;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_eff_ss_ice_smoke.c", 210);
}

void Effect_SS_Ice_Smoke_move(PlayState* play, u32 index, EffectSs* this) {
    s32 objectSlot;

    objectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_FZ);

    if ((objectSlot >= 0) && Object_Exchange_bank_dma_check(&play->objectCtx, objectSlot)) {
        if (this->rAlpha < 100) {
            this->rAlpha += 10;
        }
    } else {
        this->life = -1;
    }
}
