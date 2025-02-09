/*
 * File: z_en_m_fire1.c
 * Overlay: ovl_En_M_Fire1
 * Description: Deku Nut Hitbox
 */

#include "z_en_m_fire1.h"

#define FLAGS 0

void En_M_Fire1_actor_ct(Actor* thisx, PlayState* play);
void En_M_Fire1_actor_dt(Actor* thisx, PlayState* play);
void En_M_Fire1_actor_deku_move(Actor* thisx, PlayState* play);

ActorProfile En_M_Fire1_Profile = {
    /**/ ACTOR_EN_M_FIRE1,
    /**/ ACTORCAT_MISC,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnMFire1),
    /**/ En_M_Fire1_actor_ct,
    /**/ En_M_Fire1_actor_dt,
    /**/ En_M_Fire1_actor_deku_move,
    /**/ NULL,
};

static ColliderCylinderInit AcOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000001, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { 200, 200, 0, { 0 } },
};

void En_M_Fire1_actor_ct(Actor* thisx, PlayState* play) {
    EnMFire1* this = (EnMFire1*)thisx;
    s32 pad;

    if (this->actor.params < 0) {
        Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_ITEMACTION);
    }

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &AcOcInfoData);
}

void En_M_Fire1_actor_dt(Actor* thisx, PlayState* play) {
    EnMFire1* this = (EnMFire1*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void En_M_Fire1_actor_deku_move(Actor* thisx, PlayState* play) {
    EnMFire1* this = (EnMFire1*)thisx;
    s32 pad;

    if (chase_f(&this->timer, 1.0f, 0.2f)) {
        Actor_delete(&this->actor);
    } else {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }
}
