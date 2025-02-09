/*
 * File: z_en_vase.c
 * Overlay: ovl_En_Vase
 * Description: An unused, orange pot based on ALTTP. Lacks collision.
 */

#include "z_en_vase.h"
#include "assets/objects/object_vase/object_vase.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_vase_Actor_ct(Actor* thisx, PlayState* play);
void En_vase_Actor_dt(Actor* thisx, PlayState* play);
void En_vase_display(Actor* thisx, PlayState* play);

ActorProfile En_Vase_Profile = {
    /**/ ACTOR_EN_VASE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_VASE,
    /**/ sizeof(EnVase),
    /**/ En_vase_Actor_ct,
    /**/ En_vase_Actor_dt,
    /**/ Cheap_non_move,
    /**/ En_vase_display,
};

void En_vase_Actor_ct(Actor* thisx, PlayState* play) {
    EnVase* this = (EnVase*)thisx;

    Actor_set_scale(&this->actor, 0.01f);
    this->actor.focus.pos = this->actor.world.pos;
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 6.0f);
}

void En_vase_Actor_dt(Actor* thisx, PlayState* play) {
}

void En_vase_display(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gUnusedVaseDL);
}
