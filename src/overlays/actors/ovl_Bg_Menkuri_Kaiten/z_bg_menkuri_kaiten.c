/*
 * File: z_bg_menkuri_kaiten.c
 * Overlay: Bg_Menkuri_Kaiten
 * Description: Large rotating stone ring used in Gerudo Training Grounds and Forest Temple.
 */

#include "z_bg_menkuri_kaiten.h"
#include "assets/objects/object_menkuri_objects/object_menkuri_objects.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Menkuri_Kaiten_actor_ct(Actor* thisx, PlayState* play);
void Bg_Menkuri_Kaiten_actor_dt(Actor* thisx, PlayState* play);
void Bg_Menkuri_Kaiten_actor_move(Actor* thisx, PlayState* play);
void Bg_Menkuri_Kaiten_actor_draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Menkuri_Kaiten_Profile = {
    /**/ ACTOR_BG_MENKURI_KAITEN,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_MENKURI_OBJECTS,
    /**/ sizeof(BgMenkuriKaiten),
    /**/ Bg_Menkuri_Kaiten_actor_ct,
    /**/ Bg_Menkuri_Kaiten_actor_dt,
    /**/ Bg_Menkuri_Kaiten_actor_move,
    /**/ Bg_Menkuri_Kaiten_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Menkuri_Kaiten_actor_ct(Actor* thisx, PlayState* play) {
    BgMenkuriKaiten* this = (BgMenkuriKaiten*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
    DynaPolyUty_bgdi_SG2KSG(&gGTGRotatingRingPlatformCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
}

void Bg_Menkuri_Kaiten_actor_dt(Actor* thisx, PlayState* play) {
    BgMenkuriKaiten* this = (BgMenkuriKaiten*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void Bg_Menkuri_Kaiten_actor_move(Actor* thisx, PlayState* play) {
    BgMenkuriKaiten* this = (BgMenkuriKaiten*)thisx;

    if (!Actor_Environment_sw_Check(play, this->dyna.actor.params) && MoveBG_checkOverPlayerStatus(&this->dyna)) {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
        this->dyna.actor.shape.rot.y += 0x80;
    }
}

void Bg_Menkuri_Kaiten_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gGTGRotatingRingPlatformDL);
}
