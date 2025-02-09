/*
 * File: z_door_toki.c
 * Overlay: ovl_Door_Toki
 * Description: Door of Time Collision
 */

#include "z_door_toki.h"
#include "assets/objects/object_toki_objects/object_toki_objects.h"

#define FLAGS 0

void Door_Toki_actor_ct(Actor* thisx, PlayState* play);
void Door_Toki_actor_dt(Actor* thisx, PlayState* play);
void Door_Toki_actor_move(Actor* thisx, PlayState* play);

ActorProfile Door_Toki_Profile = {
    /**/ ACTOR_DOOR_TOKI,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_TOKI_OBJECTS,
    /**/ sizeof(DoorToki),
    /**/ Door_Toki_actor_ct,
    /**/ Door_Toki_actor_dt,
    /**/ Door_Toki_actor_move,
    /**/ NULL,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void Door_Toki_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    DoorToki* this = (DoorToki*)thisx;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gDoorTokiCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
}

void Door_Toki_actor_dt(Actor* thisx, PlayState* play) {
    DoorToki* this = (DoorToki*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void Door_Toki_actor_move(Actor* thisx, PlayState* play) {
    DoorToki* this = (DoorToki*)thisx;

    if (GET_EVENTCHKINF(EVENTCHKINF_OPENED_DOOR_OF_TIME)) {
        DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
    } else {
        DynaPolygonInfo_clearThrough(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}
