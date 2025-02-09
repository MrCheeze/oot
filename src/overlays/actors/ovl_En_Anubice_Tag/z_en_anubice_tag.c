/*
 * File: z_en_anubice_tag.c
 * Overlay: ovl_En_Anubice_Tag
 * Description: Spawns and manages the Anubis enemy
 */

#include "z_en_anubice_tag.h"
#include "terminal.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Anubice_Tag_actor_ct(Actor* thisx, PlayState* play);
void En_Anubice_Tag_actor_dt(Actor* thisx, PlayState* play);
void En_Anubice_Tag_actor_move(Actor* thisx, PlayState* play);
void En_Anubice_Tag_actor_disp(Actor* thisx, PlayState* play);

static void mode_move_init(EnAnubiceTag* this, PlayState* play);
static void mode_move(EnAnubiceTag* this, PlayState* play);

ActorProfile En_Anubice_Tag_Profile = {
    /**/ ACTOR_EN_ANUBICE_TAG,
    /**/ ACTORCAT_SWITCH,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnAnubiceTag),
    /**/ En_Anubice_Tag_actor_ct,
    /**/ En_Anubice_Tag_actor_dt,
    /**/ En_Anubice_Tag_actor_move,
    /**/ En_Anubice_Tag_actor_disp,
};

void En_Anubice_Tag_actor_ct(Actor* thisx, PlayState* play) {
    EnAnubiceTag* this = (EnAnubiceTag*)thisx;

    PRINTF("\n\n");
    // "Anubis control tag generated"
    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ アヌビス制御タグ発生 ☆☆☆☆☆ %d\n" VT_RST, this->actor.params);

    if (this->actor.params < -1) {
        this->actor.params = 0;
    }
    if (this->actor.params != 0) {
        this->extraTriggerRange = this->actor.params * 40.0f;
    }
    this->actionFunc = mode_move_init;
}

void En_Anubice_Tag_actor_dt(Actor* thisx, PlayState* play) {
}

static void mode_move_init(EnAnubiceTag* this, PlayState* play) {
    this->anubis = (EnAnubice*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_ANUBICE,
                                                  this->actor.world.pos.x, this->actor.world.pos.y,
                                                  this->actor.world.pos.z, 0, this->actor.yawTowardsPlayer, 0, 0);

    if (this->anubis != NULL) {
        this->actionFunc = mode_move;
    }
}

static void mode_move(EnAnubiceTag* this, PlayState* play) {
    EnAnubice* anubis;
    Vec3f offset;

    if (this->anubis != NULL) {
        anubis = this->anubis;
        if (anubis->actor.update == NULL) {
            return;
        }
    } else {
        Actor_delete(&this->actor);
        return;
    }

    if (anubis->deathTimer != 0) {
        Actor_delete(&this->actor);
        return;
    }

    if (this->actor.xzDistToPlayer < (200.0f + this->extraTriggerRange)) {
        if (!anubis->isPlayerOutOfRange && !anubis->isKnockedback) {
            anubis->isMirroringPlayer = true;
            offset.x = -sin_s(this->actor.yawTowardsPlayer) * this->actor.xzDistToPlayer;
            offset.z = -cos_s(this->actor.yawTowardsPlayer) * this->actor.xzDistToPlayer;
            add_calc2(&anubis->actor.world.pos.x, this->actor.world.pos.x + offset.x, 0.3f, 10.0f);
            add_calc2(&anubis->actor.world.pos.z, this->actor.world.pos.z + offset.z, 0.3f, 10.0f);
        }
    } else if (anubis->isMirroringPlayer) {
        anubis->isPlayerOutOfRange = true;
    }
}

void En_Anubice_Tag_actor_move(Actor* thisx, PlayState* play) {
    EnAnubiceTag* this = (EnAnubiceTag*)thisx;

    this->actionFunc(this, play);
}

void En_Anubice_Tag_actor_disp(Actor* thisx, PlayState* play) {
    EnAnubiceTag* this = (EnAnubiceTag*)thisx;

    if (DEBUG_FEATURES && BREG(0) != 0) {
        Debug_Display_new(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 0, 255, 4, play->state.gfxCtx);
    }
}
