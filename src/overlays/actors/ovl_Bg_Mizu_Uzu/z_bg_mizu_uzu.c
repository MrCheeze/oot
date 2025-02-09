/*
 * File: z_bg_mizu_uzu.c
 * Overlay: ovl_Bg_Mizu_Uzu
 * Description: Water Noise
 */

#include "z_bg_mizu_uzu.h"
#include "assets/objects/object_mizu_objects/object_mizu_objects.h"

#define FLAGS 0

void Bg_Mizu_Uzu_actor_ct(Actor* thisx, PlayState* play);
void Bg_Mizu_Uzu_actor_dt(Actor* thisx, PlayState* play);
void Bg_Mizu_Uzu_actor_move(Actor* thisx, PlayState* play);
void Bg_Mizu_Uzu_actor_draw(Actor* thisx, PlayState* play);

static void mode_rotate(BgMizuUzu* this, PlayState* play);

ActorProfile Bg_Mizu_Uzu_Profile = {
    /**/ ACTOR_BG_MIZU_UZU,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_MIZU_OBJECTS,
    /**/ sizeof(BgMizuUzu),
    /**/ Bg_Mizu_Uzu_actor_ct,
    /**/ Bg_Mizu_Uzu_actor_dt,
    /**/ Bg_Mizu_Uzu_actor_move,
    /**/ Bg_Mizu_Uzu_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Mizu_Uzu_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMizuUzu* this = (BgMizuUzu*)thisx;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gObjectMizuObjectsUzuCol_0074EC, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->actionFunc = mode_rotate;
}

void Bg_Mizu_Uzu_actor_dt(Actor* thisx, PlayState* play) {
    BgMizuUzu* this = (BgMizuUzu*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mode_rotate(BgMizuUzu* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    if (GET_PLAYER(play)->currentBoots == PLAYER_BOOTS_IRON) {
        DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
    } else {
        DynaPolygonInfo_clearThrough(play, &play->colCtx.dyna, this->dyna.bgId);
    }
    Actor_SE_set(thisx, NA_SE_EV_WATER_CONVECTION - SFX_FLAG);
    thisx->shape.rot.y += 0x1C0;
}

void Bg_Mizu_Uzu_actor_move(Actor* thisx, PlayState* play) {
    BgMizuUzu* this = (BgMizuUzu*)thisx;

    this->actionFunc(this, play);
}

void Bg_Mizu_Uzu_actor_draw(Actor* thisx, PlayState* play) {
}
