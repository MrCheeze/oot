/*
 * File: z_eff_ss_d_fire.c
 * Overlay: ovl_Effect_Ss_D_Fire
 * Description: Dodongo Fire
 */

#include "z_eff_ss_d_fire.h"
#include "assets/objects/object_dodongo/object_dodongo.h"

#define rScale regs[0]
#define rTexIndex regs[1]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rPrimColorA regs[5]
#define rFadeDelay regs[6]
#define rScaleStep regs[9]
#define rObjectSlot regs[10]
#define rYAccelStep regs[11] // has no effect due to how it's implemented

u32 Effect_SS2_DFire_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_SS_Dfire_disp_mode(PlayState* play, u32 index, EffectSs* this);
void Effect_SS_Dfire_func_proc(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_D_Fire_Profile = {
    EFFECT_SS_D_FIRE,
    Effect_SS2_DFire_ct,
};

u32 Effect_SS2_DFire_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsDFireInitParams* initParams = (EffectSsDFireInitParams*)initParamsx;
    s32 objectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_DODONGO);

    if (objectSlot >= 0) {
        this->pos = initParams->pos;
        this->velocity = initParams->velocity;
        this->accel = initParams->accel;

        //! @bug Segment 6 is not set to the required object before setting this display list.
        //! It works out in practice because this effect is spawned from an actor who uses the same object
        //! and previously already set it to segment 6.
        this->gfx = SEGMENTED_TO_VIRTUAL(gDodongoFireDL);

        this->life = initParams->life;
        this->rScale = initParams->scale;
        this->rScaleStep = initParams->scaleStep;
        this->rYAccelStep = 0;
        this->rObjectSlot = objectSlot;
        this->draw = Effect_SS_Dfire_disp_mode;
        this->update = Effect_SS_Dfire_func_proc;
        this->rTexIndex = ((s16)(play->state.frames % 4) ^ 3);
        this->rPrimColorR = 255;
        this->rPrimColorG = 255;
        this->rPrimColorB = 50;
        this->rPrimColorA = initParams->alpha;
        this->rFadeDelay = initParams->fadeDelay;

        return 1;
    }

    return 0;
}

static void* dfire_txt[] = { gDodongoFire0Tex, gDodongoFire1Tex, gDodongoFire2Tex, gDodongoFire3Tex };

void Effect_SS_Dfire_disp_mode(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTransBillboard;
    s32 pad;
    void* objectPtr;
    Mtx* mtx;
    f32 scale;

    objectPtr = play->objectCtx.slots[this->rObjectSlot].segment;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_d_fire.c", 276);

    if (Object_Exchange_bank_check(&play->objectCtx, OBJECT_DODONGO) >= 0) {
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(objectPtr);
        gSPSegment(POLY_XLU_DISP++, 0x06, objectPtr);
        scale = this->rScale / 100.0f;
        Skin_Matrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
        Skin_Matrix_SetScale(&mfScale, scale, scale, 1.0f);
        Skin_Matrix_MulMatrix(&mfTrans, &play->billboardMtxF, &mfTransBillboard);
        Skin_Matrix_MulMatrix(&mfTransBillboard, &mfScale, &mfResult);

        mtx = Skin_Matrix_to_Mtx_new(gfxCtx, &mfResult);

        if (mtx != NULL) {
            gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            texture_z_cld_poly_xlu(gfxCtx);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                            this->rPrimColorA);
            SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(objectPtr);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(dfire_txt[this->rTexIndex]));
            gSPDisplayList(POLY_XLU_DISP++, this->gfx);
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_d_fire.c", 330);
}

void Effect_SS_Dfire_func_proc(PlayState* play, u32 index, EffectSs* this) {
    this->rTexIndex++;
    this->rTexIndex &= 3;
    this->rScale += this->rScaleStep;

    if (this->rFadeDelay >= this->life) {
        this->rPrimColorA -= 5;
        if (this->rPrimColorA < 0) {
            this->rPrimColorA = 0;
        }
    } else {
        this->rPrimColorA += 15;
        if (this->rPrimColorA > 255) {
            this->rPrimColorA = 255;
        }
    }

    if (this->accel.y < 0.0f) {
        this->accel.y += this->rYAccelStep * 0.01f;
    }

    if (this->life <= 0) {
        this->rYAccelStep += 0;
    }
}
