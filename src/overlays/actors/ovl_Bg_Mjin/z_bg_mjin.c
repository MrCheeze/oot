/*
 * File: z_bg_mjin.c
 * Overlay: ovl_Bg_Mjin
 * Description: Warp Pad
 */

#include "z_bg_mjin.h"
#include "assets/objects/object_mjin/object_mjin.h"
#include "assets/objects/object_mjin_wind/object_mjin_wind.h"
#include "assets/objects/object_mjin_soul/object_mjin_soul.h"
#include "assets/objects/object_mjin_dark/object_mjin_dark.h"
#include "assets/objects/object_mjin_ice/object_mjin_ice.h"
#include "assets/objects/object_mjin_flame/object_mjin_flame.h"
#include "assets/objects/object_mjin_flash/object_mjin_flash.h"
#include "assets/objects/object_mjin_oka/object_mjin_oka.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Mjin_actor_ct(Actor* thisx, PlayState* play);
void Bg_Mjin_actor_dt(Actor* thisx, PlayState* play);
void Bg_Mjin_actor_move(Actor* thisx, PlayState* play);
void Bg_Mjin_actor_draw(Actor* thisx, PlayState* play);

static void move_dma_wait(BgMjin* this, PlayState* play);
static void move_wait(BgMjin* this, PlayState* play);

ActorProfile Bg_Mjin_Profile = {
    /**/ ACTOR_BG_MJIN,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(BgMjin),
    /**/ Bg_Mjin_actor_ct,
    /**/ Bg_Mjin_actor_dt,
    /**/ Bg_Mjin_actor_move,
    /**/ NULL,
};

extern UNK_TYPE D_06000000;

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 400, ICHAIN_STOP),
};

static s16 mjin_texture_bank[] = { OBJECT_MJIN_FLASH, OBJECT_MJIN_DARK, OBJECT_MJIN_FLAME,
                            OBJECT_MJIN_ICE,   OBJECT_MJIN_SOUL, OBJECT_MJIN_WIND };

void Bg_Mjin_actor_set_process(BgMjin* this, BgMjinActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Bg_Mjin_actor_ct(Actor* thisx, PlayState* play) {
    BgMjin* this = (BgMjin*)thisx;
    s8 objectSlot;

    ValueSet_process(thisx, value_init);
    objectSlot = Object_Exchange_bank_check(&play->objectCtx, (thisx->params != 0 ? OBJECT_MJIN : OBJECT_MJIN_OKA));
    this->requiredObjectSlot = objectSlot;
    if (objectSlot < 0) {
        Actor_delete(thisx);
    } else {
        Bg_Mjin_actor_set_process(this, move_dma_wait);
    }
}

void Bg_Mjin_actor_dt(Actor* thisx, PlayState* play) {
    BgMjin* this = (BgMjin*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void move_dma_wait(BgMjin* this, PlayState* play) {
    CollisionHeader* colHeader;
    CollisionHeader* collision;

    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        colHeader = NULL;
        this->dyna.actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->dyna.actor.objectSlot = this->requiredObjectSlot;
        Actor_set_segment(play, &this->dyna.actor);
        MoveBG_ct(&this->dyna, 0);
        collision = this->dyna.actor.params != 0 ? &gWarpPadCol : &gOcarinaWarpPadCol;
        DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
        this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
        Bg_Mjin_actor_set_process(this, move_wait);
        this->dyna.actor.draw = Bg_Mjin_actor_draw;
    }
}

static void move_wait(BgMjin* this, PlayState* play) {
}

void Bg_Mjin_actor_move(Actor* thisx, PlayState* play) {
    BgMjin* this = (BgMjin*)thisx;

    this->actionFunc(this, play);
}

void Bg_Mjin_actor_draw(Actor* thisx, PlayState* play) {
    BgMjin* this = (BgMjin*)thisx;
    Gfx* dlist;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_mjin.c", 250);

    if (thisx->params != 0) {
        s32 objectSlot = Object_Exchange_bank_check(&play->objectCtx, mjin_texture_bank[thisx->params - 1]);

        if (objectSlot >= 0) {
            SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);
        }

        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(&D_06000000));
        dlist = gWarpPadBaseDL;
    } else {
        dlist = gOcarinaWarpPadDL;
    }

    _texture_z_light_fog_prim(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_mjin.c", 285);
    gSPDisplayList(POLY_OPA_DISP++, dlist);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_mjin.c", 288);
}
