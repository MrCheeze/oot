/*
 * File: z_en_tp.c
 * Overlay: ovl_En_Tp
 * Description: Electric Tailpasaran
 */

#include "z_en_tp.h"
#include "versions.h"
#include "assets/objects/object_tp/object_tp.h"

#define FLAGS 0

void En_Tp_Actor_ct(Actor* thisx, PlayState* play2);
void En_Tp_Actor_dt(Actor* thisx, PlayState* play);
void En_Tp_move(Actor* thisx, PlayState* play);
void En_Tp_display(Actor* thisx, PlayState* play);

void En_Tp_Actor_mode_wait_init(EnTp* this);
void En_Tp_Actor_mode_wait(EnTp* this, PlayState* play);
void En_Tp_Actor_mode_forward_init(EnTp* this);
void En_Tp_Actor_mode_forward(EnTp* this, PlayState* play);
void En_Tp_Actor_mode_down_init(EnTp* this);
void En_Tp_Actor_mode_down(EnTp* this, PlayState* play);
void En_Tp_Actor_mode_blast_init(EnTp* this);
void En_Tp_Actor_mode_blast(EnTp* this, PlayState* play);
void En_Tp_Actor_mode_rnd_move_init(EnTp* this);
void En_Tp_Actor_mode_rnd_move(EnTp* this, PlayState* play);
void En_Tp_Actor_mode_trap2_init(EnTp* this);
void En_Tp_Actor_mode_trap2(EnTp* this, PlayState* play);
void En_Tp_Actor_mode_div_init(EnTp* this);
void En_Tp_Actor_mode_div(EnTp* this, PlayState* play);

typedef enum TailpasaranAction {
    /* 0 */ TAILPASARAN_ACTION_FRAGMENT_FADE,
    /* 1 */ TAILPASARAN_ACTION_DIE,
    /* 2 */ TAILPASARAN_ACTION_TAIL_FOLLOWHEAD,
    /* 4 */ TAILPASARAN_ACTION_HEAD_WAIT = 4,
    /* 7 */ TAILPASARAN_ACTION_HEAD_APPROACHPLAYER = 7,
    /* 8 */ TAILPASARAN_ACTION_HEAD_TAKEOFF,
    /* 9 */ TAILPASARAN_ACTION_HEAD_BURROWRETURNHOME
} TailpasaranAction;

ActorProfile En_Tp_Profile = {
    /**/ ACTOR_EN_TP,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_TP,
    /**/ sizeof(EnTp),
    /**/ En_Tp_Actor_ct,
    /**/ En_Tp_Actor_dt,
    /**/ En_Tp_move,
    /**/ En_Tp_display,
};

static ColliderJntSphElementInit JntSphElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x03, 0x08 },
            { 0xFFCFFFFF, 0x01, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 4 }, 100 },
    },
};

static ColliderJntSphInit JntSphData = {
    {
        COL_MATERIAL_HIT1,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    JntSphElemData,
};

typedef enum TailpasaranDamageEffect {
    /* 00 */ TAILPASARAN_DMGEFF_NONE,
    /* 01 */ TAILPASARAN_DMGEFF_DEKUNUT,
    /* 14 */ TAILPASARAN_DMGEFF_SHOCKING = 14, // Kills the Tailpasaran but shocks Player
    /* 15 */ TAILPASARAN_DMGEFF_INSULATING     // Kills the Tailpasaran and does not shock Player
} TailpasaranDamageEffect;

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_DEKUNUT),
    /* Deku stick    */ DMG_ENTRY(2, TAILPASARAN_DMGEFF_INSULATING),
    /* Slingshot     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Explosive     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(1, TAILPASARAN_DMGEFF_INSULATING),
    /* Normal arrow  */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, TAILPASARAN_DMGEFF_SHOCKING),
    /* Hookshot      */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Kokiri sword  */ DMG_ENTRY(1, TAILPASARAN_DMGEFF_SHOCKING),
    /* Master sword  */ DMG_ENTRY(2, TAILPASARAN_DMGEFF_SHOCKING),
    /* Giant's Knife */ DMG_ENTRY(4, TAILPASARAN_DMGEFF_SHOCKING),
    /* Fire arrow    */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Ice arrow     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Light arrow   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Unk arrow 1   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Ice magic     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Light magic   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, TAILPASARAN_DMGEFF_SHOCKING),
    /* Giant spin    */ DMG_ENTRY(4, TAILPASARAN_DMGEFF_SHOCKING),
    /* Master spin   */ DMG_ENTRY(2, TAILPASARAN_DMGEFF_SHOCKING),
    /* Kokiri jump   */ DMG_ENTRY(2, TAILPASARAN_DMGEFF_SHOCKING),
    /* Giant jump    */ DMG_ENTRY(8, TAILPASARAN_DMGEFF_SHOCKING),
    /* Master jump   */ DMG_ENTRY(4, TAILPASARAN_DMGEFF_SHOCKING),
    /* Unknown 1     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, TAILPASARAN_DMGEFF_SHOCKING),
    /* Unknown 2     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(lockOnArrowOffset, 10, ICHAIN_STOP),
};

void En_Tp_actor_set_process(EnTp* this, EnTpActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Tp_Actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnTp* this = (EnTp*)thisx;
    EnTp* now;
    EnTp* next;
    s32 i;

    ValueSet_process(&this->actor, value_init);
    this->actor.attentionRangeType = ATTENTION_RANGE_3;
    this->actor.colChkInfo.damageTable = &btl_data;
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 0.14f);
    this->unk_150 = 0;
    this->actor.colChkInfo.health = 1;
    now = this;
    this->alpha = 255;
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &JntSphData, this->colliderItems);

    if (this->actor.params <= TAILPASARAN_HEAD) {
        this->actor.naviEnemyId = NAVI_ENEMY_TAILPASARAN;
        this->timer = 0;
        this->collider.base.acFlags |= AC_HARD;
        this->collider.elements[0].dim.modelSphere.radius = this->collider.elements[0].dim.worldSphere.radius = 8;
        En_Tp_Actor_mode_trap2_init(this);
        this->actor.focus.pos = this->actor.world.pos;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        Actor_set_scale(&this->actor, 1.5f);

        for (i = 0; i <= 6; i++) {
            next = (EnTp*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_TP, this->actor.world.pos.x,
                                      this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0 * i);

            if ((0 * i) != 0) {} // Very fake, but needed to get the s registers right

            if (next != NULL) {
                now->actor.child = &next->actor;
                next->actor.parent = &now->actor;
                next->kiraSpawnTimer = i + 1;
                next->head = this;
                Actor_set_scale(&next->actor, 0.3f);

                if (i == 2) {
                    next->actor.flags |=
                        ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED;
                    next->unk_150 = 1; // Why?
                }

                next->timer = next->unk_15C = i * -5;
                next->horizontalVariation = 6.0f - (i * 0.75f);
                now = next;

                if ((0 * i) != 0) {}
            }
        }
    } else if (this->actor.params == TAILPASARAN_TAIL) {
        En_Tp_Actor_mode_wait_init(this);
    } else {
        En_Tp_Actor_mode_blast_init(this);
    }
}

void En_Tp_Actor_dt(Actor* thisx, PlayState* play) {
    EnTp* this = (EnTp*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

void En_Tp_Actor_mode_wait_init(EnTp* this) {
    this->actionIndex = TAILPASARAN_ACTION_TAIL_FOLLOWHEAD;
    En_Tp_actor_set_process(this, En_Tp_Actor_mode_wait);
}

void En_Tp_Actor_mode_wait(EnTp* this, PlayState* play) {
    s16 angle;
    s16 phase;

    if (this->actor.params == TAILPASARAN_TAIL_DYING) {
        this->actionIndex = TAILPASARAN_ACTION_DIE;

        if (this->actor.parent == NULL) {
            En_Tp_Actor_mode_down_init(this);
        }
    } else {
        if (this->unk_150 != 0) {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        }

        if (this->head->unk_150 != 0) {
            this->actor.speed = this->red = this->actor.velocity.y = this->heightPhase = 0.0f;
            if (this->actor.world.pos.y < this->head->actor.home.pos.y) {
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            }

            this->actor.world.pos = this->actor.parent->prevPos;
        } else {
            add_calc(&this->actor.world.pos.y, this->actor.parent->world.pos.y - 4.0f, 1.0f, 1.0f, 0.0f);
            angle = this->head->actor.shape.rot.y + 0x4000;
            phase = 2000 * (this->head->unk_15C + this->timer);
            this->actor.world.pos.x =
                this->actor.home.pos.x + sin_s(phase) * (sin_s(angle) * this->horizontalVariation);
            this->actor.world.pos.z =
                this->actor.home.pos.z + sin_s(phase) * (cos_s(angle) * this->horizontalVariation);
        }
    }
}

void En_Tp_Actor_mode_forward_init(EnTp* this) {
    this->actionIndex = TAILPASARAN_ACTION_HEAD_APPROACHPLAYER;
    this->timer = 200;
    En_Tp_actor_set_process(this, En_Tp_Actor_mode_forward);
}

void En_Tp_Actor_mode_forward(EnTp* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    add_calc(&this->actor.world.pos.y, player->actor.world.pos.y + 30.0f, 1.0f, 0.5f, 0.0f);
    Nai_FxFlagEntry(NA_SE_EN_TAIL_FLY - SFX_FLAG, &this->actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        if (&player->actor == this->collider.base.at) {
            this->timer = 1;
        }
    }

    if (this->red < 255) {
        this->red += 15;
    }

    if (cosf_table(this->heightPhase) == 0.0f) {
        this->extraHeightVariation = 2.0f * fqrand();
    }

    this->actor.world.pos.y += cosf_table(this->heightPhase) * (2.0f + this->extraHeightVariation);
    this->heightPhase += 0.2f;
    add_calc(&this->actor.speed, 2.5f, 0.1f, 0.2f, 0.0f);
    this->timer--;

    if (this->timer != 0) {
        add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 750, 0);
        this->actor.shape.rot.y = this->actor.world.rot.y;
    } else {
        En_Tp_Actor_mode_div_init(this);
    }
}

void En_Tp_Actor_mode_down_init(EnTp* this) {
    Actor* now;

    this->timer = 2;

    if (this->actor.params <= TAILPASARAN_HEAD) {
        for (now = this->actor.child; now != NULL; now = now->child) {
            now->params = TAILPASARAN_TAIL_DYING;
            now->colChkInfo.health = 0;
        }

        this->timer = 13;
        Actor_SE_set(&this->actor, NA_SE_EN_TAIL_DEAD);
    }
    this->actionIndex = TAILPASARAN_ACTION_DIE;
    En_Tp_actor_set_process(this, En_Tp_Actor_mode_down);
}

/**
 * Spawns effects and smaller tail segment-like fragments
 */
void En_Tp_Actor_mode_down(EnTp* this, PlayState* play) {
    EnTp* now;
    s16 i;
    s32 pad;
    Vec3f effectVelAccel = { 0.0f, 0.5f, 0.0f };
    Vec3f effectPos = { 0.0f, 0.0f, 0.0f };

    this->timer--;

    if (this->timer <= 0) {
        if (this->actor.params == TAILPASARAN_HEAD_DYING) {
            effectPos.x = ((fqrand() - 0.5f) * 15.0f) + this->actor.world.pos.x;
            effectPos.z = ((fqrand() - 0.5f) * 15.0f) + this->actor.world.pos.z;
            effectPos.y = ((fqrand() - 0.5f) * 5.0f) + this->actor.world.pos.y;
            _Effect_SS_Db_ct(play, &effectPos, &effectVelAccel, &effectVelAccel, 100, 0, 255, 255, 255, 255, 0, 0,
                                 255, 1, 9, 1);

            effectPos.x = ((fqrand() - 0.5f) * 15.0f) + this->actor.world.pos.x;
            effectPos.z = ((fqrand() - 0.5f) * 15.0f) + this->actor.world.pos.z;
            effectPos.y = ((fqrand() - 0.5f) * 5.0f) + this->actor.world.pos.y;
            _Effect_SS_Db_ct(play, &effectPos, &effectVelAccel, &effectVelAccel, 100, 0, 255, 255, 255, 255, 0, 0,
                                 255, 1, 9, 1);
            Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x50);
        } else {
#if OOT_VERSION < NTSC_1_1
            for (i = 0; i < 2; i++)
#else
            for (i = 0; i < 1; i++)
#endif
            {
                now =
                    (EnTp*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_TP, this->actor.world.pos.x,
                                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, TAILPASARAN_FRAGMENT);

                if (now != NULL) {
                    Actor_set_scale(&now->actor, this->actor.scale.z * 0.5f);
                    now->red = this->red;
                }
            }
        }

        if (this->actor.child != NULL) {
            this->actor.child->parent = NULL;
            this->actor.child->params = TAILPASARAN_TAIL_DYING;
            this->actor.child->colChkInfo.health = 0;
        }

        this->unk_150 = 2;
        Actor_delete(&this->actor);
    }
}

void En_Tp_Actor_mode_blast_init(EnTp* this) {
    this->actionIndex = TAILPASARAN_ACTION_FRAGMENT_FADE;
    this->actor.world.pos.x += ((fqrand() - 0.5f) * 5.0f);
    this->actor.world.pos.y += ((fqrand() - 0.5f) * 5.0f);
    this->actor.world.pos.z += ((fqrand() - 0.5f) * 5.0f);
    this->actor.velocity.x = (fqrand() - 0.5f) * 1.5f;
    this->actor.velocity.y = (fqrand() - 0.5f) * 1.5f;
    this->actor.velocity.z = (fqrand() - 0.5f) * 1.5f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    En_Tp_actor_set_process(this, En_Tp_Actor_mode_blast);
}

void En_Tp_Actor_mode_blast(EnTp* this, PlayState* play) {
    Actor_position_move(&this->actor);
    this->alpha -= 20;

    if (this->alpha < 20) {
        this->alpha = 0;
        Actor_delete(&this->actor);
    }
}

void En_Tp_Actor_mode_rnd_move_init(EnTp* this) {
    this->timer = (fqrand() * 15.0f) + 40.0f;
    this->actionIndex = TAILPASARAN_ACTION_HEAD_TAKEOFF;
    En_Tp_actor_set_process(this, En_Tp_Actor_mode_rnd_move);
}

/**
 * Flies up and loops around until it makes for Player
 */
void En_Tp_Actor_mode_rnd_move(EnTp* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    add_calc(&this->actor.speed, 2.5f, 0.1f, 0.2f, 0.0f);
    add_calc(&this->actor.world.pos.y, player->actor.world.pos.y + 85.0f + this->horizontalVariation, 1.0f,
                       this->actor.speed * 0.25f, 0.0f);
    Nai_FxFlagEntry(NA_SE_EN_TAIL_FLY - SFX_FLAG, &this->actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        if (&player->actor == this->collider.base.at) {
            this->unk_15C = 1;
        }
    }

    if (this->red != 0) {
        this->red -= 15;
    }

    if (cosf_table(this->heightPhase) == 0.0f) {
        this->extraHeightVariation = fqrand() * 4.0f;
    }

    this->actor.world.pos.y +=
        cosf_table(this->heightPhase) * ((this->actor.speed * 0.25f) + this->extraHeightVariation);
    this->actor.world.rot.y += this->unk_164;
    this->heightPhase += 0.2f;

    if (this->timer != 0) {
        this->timer--;
    }

    add_calc_short_angle2(&this->actor.world.rot.y, search_position_angleY(&this->actor.world.pos, &this->actor.home.pos), 1, 750,
                       0);

    if (this->timer == 0) {
        En_Tp_Actor_mode_forward_init(this);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void En_Tp_Actor_mode_trap2_init(EnTp* this) {
    this->actionIndex = TAILPASARAN_ACTION_HEAD_WAIT;
    this->unk_150 = 0;
    this->actor.shape.rot.x = -0x4000;
    this->timer = 60;
    this->unk_15C = 0;
    this->actor.speed = 0.0f;
    En_Tp_actor_set_process(this, En_Tp_Actor_mode_trap2);
}

/**
 * Awaken and rise from the ground when Player is closer than 200
 */
void En_Tp_Actor_mode_trap2(EnTp* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yaw;

    this->unk_15C--;

    if (this->actor.xzDistToPlayer < 200.0f) {
        if (this->collider.base.atFlags & AT_HIT) {
            this->collider.base.atFlags &= ~AT_HIT;
            if (&player->actor == this->collider.base.at) {
                this->timer = 0;
            }
        }

        if (this->timer != 0) {
            this->timer--;

            add_calc_short_angle2(&this->actor.shape.rot.x, 0, 1, 500, 0);
            add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 1500, 0);

            yaw = search_position_angleY(&this->actor.home.pos, &player->actor.world.pos) + 0x4000;
            add_calc(&this->actor.world.pos.y, this->actor.home.pos.y + 30.0f, 0.3f, 1.0f, 0.3f);
            this->actor.world.pos.x = this->actor.home.pos.x +
                                      (sin_s(2000 * this->unk_15C) * (sin_s(yaw) * this->horizontalVariation));
            this->actor.world.pos.z = this->actor.home.pos.z +
                                      (sin_s(2000 * this->unk_15C) * (cos_s(yaw) * this->horizontalVariation));
        } else {
            this->actor.shape.rot.x = 0;
            this->unk_150 = 1;
            En_Tp_Actor_mode_rnd_move_init(this);
        }
    } else {
        add_calc_short_angle2(&this->actor.shape.rot.x, -0x4000, 1, 500, 0);

        if (add_calc(&this->actor.world.pos.y, this->actor.home.pos.y, 0.3f, 1.5f, 0.3f) == 0.0f) {
            this->timer = 60;
        } else {
            yaw = search_position_angleY(&this->actor.home.pos, &player->actor.world.pos);
            this->actor.world.pos.x =
                this->actor.home.pos.x + (sin_s(2000 * this->unk_15C) * (sin_s(yaw) * 6.0f));
            this->actor.world.pos.z =
                this->actor.home.pos.z + (sin_s(2000 * this->unk_15C) * (cos_s(yaw) * 6.0f));
        }
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->actor.world.pos.y != this->actor.home.pos.y) {
        Nai_FxFlagEntry(NA_SE_EN_TAIL_FLY - SFX_FLAG, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
}

void En_Tp_Actor_mode_div_init(EnTp* this) {
    this->actionIndex = TAILPASARAN_ACTION_HEAD_BURROWRETURNHOME;
    this->timer = 0;
    En_Tp_actor_set_process(this, En_Tp_Actor_mode_div);
}

void En_Tp_Actor_mode_div(EnTp* this, PlayState* play) {
    static Vec3f acc = { 0.0f, -0.5f, 0.0f };
    static Color_RGBA8 prim = { 255, 255, 255, 255 };
    static Color_RGBA8 env = { 150, 150, 150, 0 };
    Vec3f bubbleVelocity;
    Vec3f bubblePos;
    s32 closeToFloor;
    EnTp* now;
    s16 temp_v0; // Required to match, usage can maybe be improved

    closeToFloor = false;
    temp_v0 = this->timer;
    this->unk_15C--;

    if ((temp_v0 != 0) || ((this->actor.home.pos.y - this->actor.world.pos.y) > 60.0f)) {
        this->timer = temp_v0 - 1;
        temp_v0 = this->timer;

        if (temp_v0 == 0) {
            En_Tp_Actor_mode_trap2_init(this);

            for (now = (EnTp*)this->actor.child; now != NULL; now = (EnTp*)now->actor.child) {
                now->unk_15C = now->timer;
            }
        } else {
            if (this->actor.shape.rot.x != -0x4000) {
                this->timer = 80;
                this->actor.velocity.y = 0.0f;
                this->actor.speed = 0.0f;
                this->actor.world.pos = this->actor.home.pos;
                this->actor.shape.rot.x = -0x4000;

                for (now = (EnTp*)this->actor.child; now != NULL; now = (EnTp*)now->actor.child) {
                    now->actor.velocity.y = 0.0f;
                    now->actor.speed = 0.0f;
                    now->actor.world.pos = this->actor.home.pos;
                    now->actor.world.pos.y = this->actor.home.pos.y - 80.0f;
                }
            }

            this->actor.world.pos.y = this->actor.home.pos.y - this->timer;
        }
    } else {
        if (this->actor.shape.rot.x != 0x4000) {
            this->actor.shape.rot.x -= 0x400;
        }

        if (this->red != 0) {
            this->red -= 15;
        }

        this->actor.speed = 2.0f * cos_s(this->actor.shape.rot.x);
        this->actor.velocity.y = sin_s(this->actor.shape.rot.x) * -2.0f;

        if ((this->actor.world.pos.y - this->actor.floorHeight) < 20.0f) {
            closeToFloor = true;
        }

        if (this->actor.world.pos.y != this->actor.home.pos.y) {
            Nai_FxFlagEntry(NA_SE_EN_TAIL_FLY - SFX_FLAG, &this->actor.projectedPos, 4,
                                 &_dummy_one, &_dummy_one, &_dummy_zero_s8);
        }

        if (closeToFloor && ((play->gameplayFrames & 1) != 0)) {
            bubblePos = this->actor.world.pos;
            bubblePos.y = this->actor.floorHeight;

            bubbleVelocity.x = rnd_fx(5.0f);
            bubbleVelocity.y = (fqrand() * 3.5f) + 1.5f;
            bubbleVelocity.z = rnd_fx(5.0f);

            Effect_SS_Dt_Bubble_sc_cl_co_ct(play, &bubblePos, &bubbleVelocity, &acc, &prim,
                                              &env, get_random_timer(100, 50), 20, 0);
        }
    }
}

void En_Tp_damage_proc(EnTp* this, PlayState* play) {
    s32 phi_s2;
    s32 phi_s4;
    EnTp* head; // Can eliminate this and just use now, but they're used differently
    EnTp* now;

    if ((this->collider.base.acFlags & AC_HIT) && (this->actionIndex >= TAILPASARAN_ACTION_TAIL_FOLLOWHEAD)) {
        phi_s4 = phi_s2 = 0;

        if (this->actor.params <= TAILPASARAN_HEAD) {
            phi_s2 = 1;
        }

        this->collider.base.acFlags &= ~AC_HIT;
        Hit_bit_set_sph(&this->actor, &this->collider, true);
        this->damageEffect = this->actor.colChkInfo.damageEffect;

        if (this->actor.colChkInfo.damageEffect != TAILPASARAN_DMGEFF_NONE) {
            if (this->actor.colChkInfo.damageEffect == TAILPASARAN_DMGEFF_DEKUNUT) {
                phi_s4 = 1;
            }

            // Head is invincible
            if (phi_s2 == 0) {
                hp_down(&this->actor);
            }

            if (this->actor.colChkInfo.health == 0) {
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                head = this->head;

                if (head->actor.params <= TAILPASARAN_HEAD) {
                    En_Tp_Actor_mode_down_init(head);
                    head->damageEffect = this->actor.colChkInfo.damageEffect;
                    head->actor.params = TAILPASARAN_HEAD_DYING;
                }
            } else {
                if (phi_s4 != 0) {
                    this->actor.freezeTimer = 80;
                    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
                    if (phi_s2 != 0) {
                        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA,
                                             80);
                    } else {
                        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_XLU,
                                             80);
                    }
                }

                for (now = (EnTp*)this->actor.parent; now != NULL; now = (EnTp*)now->actor.parent) {
                    now->collider.base.acFlags &= ~AC_HIT;

                    if (phi_s4 != 0) {
                        now->actor.freezeTimer = 80;
                        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);

                        if (phi_s2 != 0) {
                            Set_Fog(&now->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA,
                                                 80);
                        } else {
                            Set_Fog(&now->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_XLU,
                                                 80);
                        }
                    }
                }

                for (now = (EnTp*)this->actor.child; now != NULL; now = (EnTp*)now->actor.child) {
                    now->collider.base.acFlags &= ~AC_HIT;
                    if (phi_s4 != 0) {
                        now->actor.freezeTimer = 80;

                        if (phi_s2 != 0) {
                            Set_Fog(&now->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA,
                                                 80);
                        } else {
                            Set_Fog(&now->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_XLU,
                                                 80);
                        }
                    }
                }
            }
        }
    }
}

void En_Tp_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnTp* this = (EnTp*)thisx;
    Vec3f kiraVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f kiraAccel = { 0.0f, -0.6f, 0.0f };
    Vec3f kiraPos;
    Color_RGBA8 kiraPrimColor = { 0, 0, 255, 255 };
    Color_RGBA8 kiraEnvColor = { 0, 0, 0, 0 };
    Player* player = GET_PLAYER(play);
    s16 yawToWall;

    if (player->stateFlags1 & PLAYER_STATE1_26) { // Shielding
        this->damageEffect = TAILPASARAN_DMGEFF_NONE;
    }

    if (this->actor.colChkInfo.health != 0) {
        En_Tp_damage_proc(this, play);
    }

    this->actionFunc(this, play);

    if (this->actor.params <= TAILPASARAN_HEAD) {
        Actor_position_moveF(&this->actor);

        if (this->actionIndex != TAILPASARAN_ACTION_HEAD_BURROWRETURNHOME) {
            Actor_BGcheck2(play, &this->actor, 0.0f, 15.0f, 10.0f,
                                    UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
        }

        // Turn away from wall
        if ((this->actor.speed != 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
            yawToWall = this->actor.wallYaw - this->actor.world.rot.y;

            if (ABS(yawToWall) > 0x4000) {
                if (yawToWall >= 0) {
                    this->actor.world.rot.y -= 500;
                } else {
                    this->actor.world.rot.y += 500;
                }

                this->actor.shape.rot.y = this->actor.world.rot.y;
            }
        }

        this->actor.shape.rot.z += 0x800;

        if (this->actor.shape.rot.z == 0) {
            Nai_FxFlagEntry(NA_SE_EN_TAIL_CRY, &this->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }

        if (this->actionIndex >= TAILPASARAN_ACTION_TAIL_FOLLOWHEAD) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        }
    }

    if (this->actor.params != TAILPASARAN_TAIL_DYING) {
        this->kiraSpawnTimer--;
        this->kiraSpawnTimer &= 7;
    }

    this->actor.focus.pos = this->actor.world.pos;

    if (this->damageEffect == TAILPASARAN_DMGEFF_SHOCKING) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }

    if ((this->kiraSpawnTimer & 7) == 0) {
        kiraPrimColor.r = this->red;
        kiraAccel.x = -this->actor.velocity.x * 0.25f;
        kiraAccel.y = -this->actor.velocity.y * 0.25f;
        kiraAccel.z = -this->actor.velocity.z * 0.25f;
        kiraPos.x = ((fqrand() - 0.5f) * 25.0f) + this->actor.world.pos.x;
        kiraPos.y = ((fqrand() - 0.5f) * 20.0f) + this->actor.world.pos.y;
        kiraPos.z = ((fqrand() - 0.5f) * 25.0f) + this->actor.world.pos.z;
        Effect_SS_KiraKira_ct(play, &kiraPos, &kiraVelocity, &kiraAccel, &kiraPrimColor, &kiraEnvColor);
    }

    if ((this->actionIndex >= TAILPASARAN_ACTION_TAIL_FOLLOWHEAD) && (this->actor.colChkInfo.health != 0)) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void En_Tp_display(Actor* thisx, PlayState* play) {
    s32 pad;
    EnTp* this = (EnTp*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_tp.c", 1451);

    if (this->unk_150 != 2) {
        if ((thisx->params <= TAILPASARAN_HEAD) || (thisx->params == TAILPASARAN_HEAD_DYING)) {
            _texture_z_light_fog_prim(play->state.gfxCtx);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_tp.c", 1459);
            gSPDisplayList(POLY_OPA_DISP++, gTailpasaranHeadDL);

            Matrix_translate(0.0f, 0.0f, 8.0f, MTXMODE_APPLY);
        } else {
            _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->red, 0, 255, this->alpha);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetCombineLERP(POLY_XLU_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT,
                              TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT,
                              TEXEL0, ENVIRONMENT);
            gDPPipeSync(POLY_XLU_DISP++);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gTailpasaranTailSegmentTex));
            gDPPipeSync(POLY_XLU_DISP++);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_tp.c", 1480);
            gSPDisplayList(POLY_XLU_DISP++, gTailpasaranTailSegmentDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_tp.c", 1495);

    if ((thisx->params <= TAILPASARAN_TAIL) || (thisx->params == TAILPASARAN_TAIL_DYING)) {
        CollisionCheck_Uty_convJntSphL2G(0, &this->collider);
    }
}
