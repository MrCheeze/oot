/*
 * File: z_bg_spot00_break.c
 * Overlay: ovl_Bg_Spot00_Break
 * Description: Broken drawbridge in Hyrule Field.
 */

#include "z_bg_spot00_break.h"
#include "assets/objects/object_spot00_break/object_spot00_break.h"

#define FLAGS 0

void Bg_Spot00_Break_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot00_Break_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot00_Break_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot00_Break_actor_draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Spot00_Break_Profile = {
    /**/ ACTOR_BG_SPOT00_BREAK,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_SPOT00_BREAK,
    /**/ sizeof(BgSpot00Break),
    /**/ Bg_Spot00_Break_actor_ct,
    /**/ Bg_Spot00_Break_actor_dt,
    /**/ Bg_Spot00_Break_actor_move,
    /**/ Bg_Spot00_Break_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void Bg_Spot00_Break_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot00Break* this = (BgSpot00Break*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, 0);

    if (this->dyna.actor.params == 1) {
        DynaPolyUty_bgdi_SG2KSG(&gBarbedWireFenceCol, &colHeader);
    } else {
        DynaPolyUty_bgdi_SG2KSG(&gBrokenDrawbridgeCol, &colHeader);
    }

    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    if (!LINK_IS_ADULT) {
        Actor_delete(&this->dyna.actor);
    }
}

void Bg_Spot00_Break_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot00Break* this = (BgSpot00Break*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void Bg_Spot00_Break_actor_move(Actor* thisx, PlayState* play) {
}

void Bg_Spot00_Break_actor_draw(Actor* thisx, PlayState* play) {
    BgSpot00Break* this = (BgSpot00Break*)thisx;

    if (this->dyna.actor.params == 1) {
        Cheap_gfx_display(play, gBarbedWireFenceDL);
    } else {
        Cheap_gfx_display(play, gBrokenDrawbridgeDL);
    }
}
