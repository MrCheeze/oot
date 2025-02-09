/*
 * File: z_en_arow_trap.c
 * Overlay: ovl_En_Arow_Trap
 * Description: An unused trap that reflects arrows.
 */

#include "z_en_arow_trap.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_atp_Actor_ct(Actor* thisx, PlayState* play);
void En_atp_Actor_dt(Actor* thisx, PlayState* play);
void En_atp_move(Actor* thisx, PlayState* play);

ActorProfile En_Arow_Trap_Profile = {
    /**/ ACTOR_EN_AROW_TRAP,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnArowTrap),
    /**/ En_atp_Actor_ct,
    /**/ En_atp_Actor_dt,
    /**/ En_atp_move,
    /**/ NULL,
};

void En_atp_Actor_ct(Actor* thisx, PlayState* play) {
    EnArowTrap* this = (EnArowTrap*)thisx;

    Actor_set_scale(&this->actor, 0.01);
    this->unk_14C = 0;
    this->attackTimer = 80;
    this->actor.focus.pos = this->actor.world.pos;
}

void En_atp_Actor_dt(Actor* thisx, PlayState* play) {
}

void En_atp_move(Actor* thisx, PlayState* play) {
    EnArowTrap* this = (EnArowTrap*)thisx;

    if (this->actor.xzDistToPlayer <= 400) {
        this->attackTimer--;

        if (this->attackTimer == 0) {
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_ARROW, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, this->actor.shape.rot.x, this->actor.shape.rot.y,
                        this->actor.shape.rot.z, ARROW_NORMAL_SILENT);
            this->attackTimer = 80;
        }
    }
}
