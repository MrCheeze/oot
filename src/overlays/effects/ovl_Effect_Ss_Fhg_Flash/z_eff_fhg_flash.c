/*
 * File: z_eff_ss_fhg_flash.c
 * Overlay: ovl_Effect_Ss_Fhg_Flash
 * Description: Shock and Light Ball Effect
 */

#include "z_eff_ss_fhg_flash.h"
#include "overlays/actors/ovl_Boss_Ganondrof/z_boss_ganondrof.h"
#include "assets/objects/object_fhg/object_fhg.h"

#define rAlpha regs[0]
#define rObjectSlot regs[2]
#define rXZRot regs[3]
#define rParam regs[4]
#define rScale regs[8]

u32 Effect_Ss_Fhg_Flash_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_fhg_flash_disp(PlayState* play, u32 index, EffectSs* this);
void Effect_fhg_flash_move(PlayState* play, u32 index, EffectSs* this);
void Effect_fhg_flash_mini_disp(PlayState* play, u32 index, EffectSs* this);
void Effect_fhg_flash_mini_move(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Fhg_Flash_Profile = {
    EFFECT_SS_FHG_FLASH,
    Effect_Ss_Fhg_Flash_ct,
};

// Should eventually come from assets/overlays/ovl_Effect_Ss_Fhg_Flash/ovl_Effect_Ss_Fhg_Flash.h
//! TODO: investigate having ZAPD forward declare static variables
static Gfx gnf_inazumaMINI2_modelT[15];

u32 Effect_Ss_Fhg_Flash_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsFhgFlashInitParams* initParams = (EffectSsFhgFlashInitParams*)initParamsx;
    s32 pad;
    s32 objectSlot;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f farAwayVec = { 0.0f, -1000.0f, 0.0f };
    uintptr_t prevSeg6;

    if (initParams->type == FHGFLASH_LIGHTBALL) {
        objectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_FHG);

        if ((objectSlot >= 0) && Object_Exchange_bank_dma_check(&play->objectCtx, objectSlot)) {
            prevSeg6 = SegmentBaseAddress[6];
            SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);
            this->rObjectSlot = objectSlot;
            this->pos = initParams->pos;
            this->velocity = initParams->velocity;
            this->accel = initParams->accel;
            this->rParam = initParams->param;
            this->life = 100;
            this->rScale = initParams->scale;
            this->rAlpha = 255;
            this->draw = Effect_fhg_flash_disp;
            this->update = Effect_fhg_flash_move;
            this->gfx = SEGMENTED_TO_VIRTUAL(gPhantomEnergyBallDL);
            SegmentBaseAddress[6] = prevSeg6;
        } else {
            PRINTF("Effect_Ss_Fhg_Flash_ct():pffd->modeエラー\n");
            return 0;
        }
    } else {
        this->actor = initParams->actor;
        this->velocity = this->accel = zeroVec;
        this->life = (s16)(fqrand() * 10.0f) + 111;
        this->rScale = (s16)rnd_f(initParams->scale) + initParams->scale;
        this->rAlpha = 255;
        this->draw = Effect_fhg_flash_mini_disp;
        this->update = Effect_fhg_flash_mini_move;
        this->rParam = initParams->param;

        if (initParams->param != FHGFLASH_SHOCK_NO_ACTOR) {
            this->pos = farAwayVec; // Set the initial position to where the effect cannot be seen
            this->gfx = SEGMENTED_TO_VIRTUAL(gnf_inazumaMINI2_modelT);
        } else {
            this->pos = initParams->pos;
            this->gfx = SEGMENTED_TO_VIRTUAL(gnf_inazumaMINI2_modelT);
        }
    }
    return 1;
}

static Color_RGB8 col[] = {
    { 165, 255, 61 }, { 0, 255, 255 }, { 255, 40, 0 }, { 255, 255, 0 }, { 0, 0, 255 },
    { 255, 0, 255 },  { 255, 150, 0 }, { 0, 0, 0 },    { 0, 0, 0 },
};

void Effect_fhg_flash_disp(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;
    f32 scale;
    void* objectPtr;

    scale = this->rScale / 100.0f;
    objectPtr = play->objectCtx.slots[this->rObjectSlot].segment;

    OPEN_DISPS(gfxCtx, "../z_eff_fhg_flash.c", 268);

    Matrix_translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(objectPtr);
    gSPSegment(POLY_XLU_DISP++, 0x06, objectPtr);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, this->rAlpha);
    gDPSetEnvColor(POLY_XLU_DISP++, col[this->rParam].r, col[this->rParam].g,
                   col[this->rParam].b, 0);
    gDPPipeSync(POLY_XLU_DISP++);
    Matrix_rotate_scale_exchange(&play->billboardMtxF);
    Matrix_rotateZ((this->rXZRot / (f32)0x8000) * 3.1416f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_eff_fhg_flash.c", 326);
    gSPDisplayList(POLY_XLU_DISP++, this->gfx);

    CLOSE_DISPS(gfxCtx, "../z_eff_fhg_flash.c", 330);
}

void Effect_fhg_flash_mini_disp(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;
    f32 scale;

    scale = this->rScale / 100.0f;

    OPEN_DISPS(gfxCtx, "../z_eff_fhg_flash.c", 346);

    Matrix_translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_scale(scale, scale, scale, MTXMODE_APPLY);

    if (this->rParam != FHGFLASH_SHOCK_NO_ACTOR) {
        _texture_decal_shadow(play->state.gfxCtx);
        Matrix_rotateX((this->rXZRot / (f32)0x8000) * 1.1416f, MTXMODE_APPLY);
        gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_AA_ZB_XLU_DECAL2);
    } else {
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_AA_ZB_XLU_SURF2);
    }

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, this->rAlpha);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 155, 0);
    Matrix_rotateZ((this->rXZRot / (f32)0x8000) * 3.1416f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_eff_fhg_flash.c", 395);
    gSPDisplayList(POLY_XLU_DISP++, this->gfx);

    CLOSE_DISPS(gfxCtx, "../z_eff_fhg_flash.c", 399);
}

void Effect_fhg_flash_move(PlayState* play, u32 index, EffectSs* this) {
    s16 rand = (fqrand() * 20000.0f);

    this->rXZRot = (this->rXZRot + rand) + 0x4000;

    if (this->rScale > 0) {
        this->rScale -= 10;

        if (this->rScale <= 0) {
            this->rScale = 0;
            this->life = 0;
        }
    }

    if (this->rAlpha > 0) {
        this->rAlpha -= 10;

        if (this->rAlpha <= 0) {
            this->rAlpha = 0;
        }
    }
}

void Effect_fhg_flash_mini_move(PlayState* play, u32 index, EffectSs* this) {
    s16 randBodyPart;
    s16 rotStep = fqrand() * 20000.0f;

    this->rXZRot += rotStep + 0x4000;

    if (this->rParam == FHGFLASH_SHOCK_PLAYER) {
        Player* player = GET_PLAYER(play);

        randBodyPart = rnd_f(PLAYER_BODYPART_MAX - 0.1f);
        this->pos.x = player->bodyPartsPos[randBodyPart].x + rnd_fx(10.0f);
        this->pos.y = player->bodyPartsPos[randBodyPart].y + rnd_fx(15.0f);
        this->pos.z = player->bodyPartsPos[randBodyPart].z + rnd_fx(10.0f);
    } else if (this->rParam == FHGFLASH_SHOCK_PG) {
        BossGanondrof* phantomGanon = (BossGanondrof*)this->actor;

        randBodyPart = rnd_f(23.9f);
        this->pos.x = phantomGanon->bodyPartsPos[randBodyPart].x + rnd_fx(15.0f);
        this->pos.y = phantomGanon->bodyPartsPos[randBodyPart].y + rnd_fx(20.0f);
        this->pos.z = phantomGanon->bodyPartsPos[randBodyPart].z + rnd_fx(15.0f);
    }

    if (this->life < 100) {
        s32 pad;

        this->rAlpha -= 50;

        if (this->rAlpha < 0) {
            this->rAlpha = 0;
            this->life = 0;
        }
    }
}

#include "assets/overlays/ovl_Effect_Ss_Fhg_Flash/z_eff_fhg_flash.c"
