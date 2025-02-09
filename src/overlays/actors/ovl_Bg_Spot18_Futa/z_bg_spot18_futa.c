/*
 * File: z_bg_spot18_futa.c
 * Overlay: ovl_Bg_Spot18_Futa
 * Description: The lid to the spinning goron vase.
 */

#include "z_bg_spot18_futa.h"
#include "assets/objects/object_spot18_obj/object_spot18_obj.h"

#define FLAGS 0

void Bg_Spot18_Futa_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot18_Futa_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot18_Futa_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot18_Futa_actor_draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Spot18_Futa_Profile = {
    /**/ ACTOR_BG_SPOT18_FUTA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_SPOT18_OBJ,
    /**/ sizeof(BgSpot18Futa),
    /**/ Bg_Spot18_Futa_actor_ct,
    /**/ Bg_Spot18_Futa_actor_dt,
    /**/ Bg_Spot18_Futa_actor_move,
    /**/ Bg_Spot18_Futa_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

void Bg_Spot18_Futa_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot18Futa* this = (BgSpot18Futa*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gGoronCityVaseLidCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    ValueSet_process(&this->dyna.actor, value_init);
}

void Bg_Spot18_Futa_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot18Futa* this = (BgSpot18Futa*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void Bg_Spot18_Futa_actor_move(Actor* thisx, PlayState* play) {
    BgSpot18Futa* this = (BgSpot18Futa*)thisx;
    s32 iVar1;

    if (this->dyna.actor.parent == NULL) {
        iVar1 = chase_f(&this->dyna.actor.scale.x, 0, 0.005);

        if (iVar1 != 0) {
            Actor_delete(&this->dyna.actor);
        } else {
            this->dyna.actor.scale.z = this->dyna.actor.scale.x;
            this->dyna.actor.scale.y = this->dyna.actor.scale.x;
        }
    }
}

void Bg_Spot18_Futa_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gGoronCityVaseLidDL);
}
