/*
 * File: z_en_oe2.c
 * Overlay: ovl_En_Oe2
 * Description: Blue Navi Target Spot
 */

#include "z_en_oe2.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_OE2_Actor_ct(Actor* thisx, PlayState* play);
void En_OE2_Actor_dt(Actor* thisx, PlayState* play);
void En_OE2_Actor_move(Actor* thisx, PlayState* play);
void En_OE2_Actor_draw(Actor* thisx, PlayState* play);

static void matsu(EnOE2* this, PlayState* play);

ActorProfile En_OE2_Profile = {
    /**/ ACTOR_EN_OE2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_OE2,
    /**/ sizeof(EnOE2),
    /**/ En_OE2_Actor_ct,
    /**/ En_OE2_Actor_dt,
    /**/ En_OE2_Actor_move,
    /**/ En_OE2_Actor_draw,
};

void En_OE2_actor_set_process(EnOE2* this, EnOE2ActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_OE2_Actor_ct(Actor* thisx, PlayState* play) {
    EnOE2* this = (EnOE2*)thisx;

    En_OE2_actor_set_process(this, matsu);
}

void En_OE2_Actor_dt(Actor* thisx, PlayState* play) {
}

static void matsu(EnOE2* this, PlayState* play) {
}

void En_OE2_Actor_move(Actor* thisx, PlayState* play) {
}

void En_OE2_Actor_draw(Actor* thisx, PlayState* play) {
}
