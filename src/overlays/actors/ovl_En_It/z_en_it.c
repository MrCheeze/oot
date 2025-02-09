/*
 * File: z_en_it.c
 * Overlay: ovl_En_It
 * Description: Dampe's Minigame digging spot hitboxes
 */

#include "z_en_it.h"

#define FLAGS 0

void En_It_actor_ct(Actor* thisx, PlayState* play);
void En_It_actor_dt(Actor* thisx, PlayState* play);
void En_It_actor_move(Actor* thisx, PlayState* play);

static ColliderCylinderInit EnItAtInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_NO_PUSH,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 40, 10, 0, { 0 } },
};

static CollisionCheckInfoInit2 ItStatusData = { 0, 0, 0, 0, MASS_IMMOVABLE };

ActorProfile En_It_Profile = {
    /**/ ACTOR_EN_IT,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnIt),
    /**/ En_It_actor_ct,
    /**/ En_It_actor_dt,
    /**/ En_It_actor_move,
    /**/ NULL,
};

void En_It_actor_ct(Actor* thisx, PlayState* play) {
    EnIt* this = (EnIt*)thisx;

    this->actor.params = 0x0D05;
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnItAtInfoData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, NULL, &ItStatusData);
}

void En_It_actor_dt(Actor* thisx, PlayState* play) {
    EnIt* this = (EnIt*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void En_It_actor_move(Actor* thisx, PlayState* play) {
    EnIt* this = (EnIt*)thisx;
    s32 pad;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}
