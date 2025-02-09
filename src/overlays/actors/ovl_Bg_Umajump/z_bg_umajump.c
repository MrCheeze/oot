/*
 * File: z_bg_umajump.c
 * Overlay: ovl_Bg_Umajump
 * Description: Hoppable horse fence
 */

#include "z_bg_umajump.h"
#include "assets/objects/object_umajump/object_umajump.h"

#define FLAGS 0

void Bg_Umajump_actor_ct(Actor* thisx, PlayState* play);
void Bg_Umajump_actor_dt(Actor* thisx, PlayState* play);
void Bg_Umajump_actor_move(Actor* thisx, PlayState* play);
void Bg_Umajump_actor_draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Umajump_Profile = {
    /**/ ACTOR_BG_UMAJUMP,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_UMAJUMP,
    /**/ sizeof(BgUmaJump),
    /**/ Bg_Umajump_actor_ct,
    /**/ Bg_Umajump_actor_dt,
    /**/ Bg_Umajump_actor_move,
    /**/ Bg_Umajump_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Umajump_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgUmaJump* this = (BgUmaJump*)thisx;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gJumpableHorseFenceCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    if (this->dyna.actor.params == 1) {
        if (!(event_check(EVENTCHKINF_EPONA_OBTAINED) || R_DEBUG_FORCE_EPONA_OBTAINED)) {
            Actor_delete(&this->dyna.actor);
            return;
        }
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED;
    }
}

void Bg_Umajump_actor_dt(Actor* thisx, PlayState* play) {
    BgUmaJump* this = (BgUmaJump*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void Bg_Umajump_actor_move(Actor* thisx, PlayState* play) {
}

void Bg_Umajump_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gJumpableHorseFenceDL);
}
