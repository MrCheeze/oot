
/*
 * File: z_en_reeba.c
 * Overlay: ovl_En_Reeba
 * Description: Leever
 */

#include "z_en_reeba.h"
#include "overlays/actors/ovl_En_Encount1/z_en_encount1.h"
#include "terminal.h"
#include "assets/objects/object_reeba/object_reeba.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_LOCK_ON_DISABLED)

void En_Reeba_Actor_ct(Actor* thisx, PlayState* play);
void En_Reeba_Actor_dt(Actor* thisx, PlayState* play);
void En_Reeba_move(Actor* thisx, PlayState* play2);
void En_Reeba_display(Actor* thisx, PlayState* play);

void mode_wake_init(EnReeba* this, PlayState* play);
void mode_wake(EnReeba* this, PlayState* play);
static void mode_move(EnReeba* this, PlayState* play);
static void mode_down_init(EnReeba* this, PlayState* play);
static void mode_down(EnReeba* this, PlayState* play);
void mode_boss_move_init(EnReeba* this, PlayState* play);
void mode_boss_move(EnReeba* this, PlayState* play);
void mode_buruburu(EnReeba* this, PlayState* play);
static void mode_damage(EnReeba* this, PlayState* play);
static void mode_dead(EnReeba* this, PlayState* play);
void mode_special_damage(EnReeba* this, PlayState* play);
static void mode_dead_wait(EnReeba* this, PlayState* play);

typedef enum LeeverDamageEffect {
    /* 0x00 */ LEEVER_DMGEFF_NONE, // used by anything that cant kill the Leever
    /* 0x01 */ LEEVER_DMGEFF_UNK,  // used by "unknown 1" attack
    /* 0x03 */ LEEVER_DMGEFF_ICE = 3,
    /* 0x0B */ LEEVER_DMGEFF_UNUSED = 11, // not used in the damage table, but still checked for.
    /* 0x0C */ LEEVER_DMGEFF_BOOMERANG,
    /* 0x0D */ LEEVER_DMGEFF_HOOKSHOT,
    /* 0x0E */ LEEVER_DMGEFF_OTHER
} LeeverDamageEffect;

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, LEEVER_DMGEFF_NONE),
    /* Deku stick    */ DMG_ENTRY(2, LEEVER_DMGEFF_OTHER),
    /* Slingshot     */ DMG_ENTRY(1, LEEVER_DMGEFF_OTHER),
    /* Explosive     */ DMG_ENTRY(2, LEEVER_DMGEFF_OTHER),
    /* Boomerang     */ DMG_ENTRY(1, LEEVER_DMGEFF_BOOMERANG),
    /* Normal arrow  */ DMG_ENTRY(2, LEEVER_DMGEFF_OTHER),
    /* Hammer swing  */ DMG_ENTRY(2, LEEVER_DMGEFF_OTHER),
    /* Hookshot      */ DMG_ENTRY(2, LEEVER_DMGEFF_HOOKSHOT),
    /* Kokiri sword  */ DMG_ENTRY(1, LEEVER_DMGEFF_OTHER),
    /* Master sword  */ DMG_ENTRY(4, LEEVER_DMGEFF_OTHER),
    /* Giant's Knife */ DMG_ENTRY(6, LEEVER_DMGEFF_OTHER),
    /* Fire arrow    */ DMG_ENTRY(2, LEEVER_DMGEFF_OTHER),
    /* Ice arrow     */ DMG_ENTRY(4, LEEVER_DMGEFF_ICE),
    /* Light arrow   */ DMG_ENTRY(2, LEEVER_DMGEFF_OTHER),
    /* Unk arrow 1   */ DMG_ENTRY(2, LEEVER_DMGEFF_OTHER),
    /* Unk arrow 2   */ DMG_ENTRY(2, LEEVER_DMGEFF_OTHER),
    /* Unk arrow 3   */ DMG_ENTRY(2, LEEVER_DMGEFF_OTHER),
    /* Fire magic    */ DMG_ENTRY(0, LEEVER_DMGEFF_NONE),
    /* Ice magic     */ DMG_ENTRY(4, LEEVER_DMGEFF_ICE),
    /* Light magic   */ DMG_ENTRY(0, LEEVER_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, LEEVER_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, LEEVER_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(2, LEEVER_DMGEFF_OTHER),
    /* Giant spin    */ DMG_ENTRY(8, LEEVER_DMGEFF_OTHER),
    /* Master spin   */ DMG_ENTRY(4, LEEVER_DMGEFF_OTHER),
    /* Kokiri jump   */ DMG_ENTRY(2, LEEVER_DMGEFF_OTHER),
    /* Giant jump    */ DMG_ENTRY(8, LEEVER_DMGEFF_OTHER),
    /* Master jump   */ DMG_ENTRY(4, LEEVER_DMGEFF_OTHER),
    /* Unknown 1     */ DMG_ENTRY(0, LEEVER_DMGEFF_UNK),
    /* Unblockable   */ DMG_ENTRY(0, LEEVER_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(0, LEEVER_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, LEEVER_DMGEFF_NONE),
};

ActorProfile En_Reeba_Profile = {
    /**/ ACTOR_EN_REEBA,
    /**/ ACTORCAT_MISC,
    /**/ FLAGS,
    /**/ OBJECT_REEBA,
    /**/ sizeof(EnReeba),
    /**/ En_Reeba_Actor_ct,
    /**/ En_Reeba_Actor_dt,
    /**/ En_Reeba_move,
    /**/ En_Reeba_display,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_HIT5,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x08, 0x08 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 40, 0, { 0, 0, 0 } },
};

void En_Reeba_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnReeba* this = (EnReeba*)thisx;
    s32 surfaceType;

    this->actor.naviEnemyId = NAVI_ENEMY_LEEVER;
    this->actor.attentionRangeType = ATTENTION_RANGE_3;
    this->actor.gravity = -3.5f;
    this->actor.focus.pos = this->actor.world.pos;
    Skeleton_Info2_M_ct(play, &this->skelanime, &object_reeba_Skel_001EE8, &object_reeba_Anim_0001E4, this->jointTable,
                   this->morphTable, 18);
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.colChkInfo.health = 4;
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
    this->type = this->actor.params;
    this->scale = 0.04f;

    if (this->type != LEEVER_TYPE_SMALL) {
        this->scale *= 1.5f;
        this->collider.dim.radius = 35;
        this->collider.dim.height = 45;
        // "Reeba Boss Appears %f"
        PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ リーバぼす登場 ☆☆☆☆☆ %f\n" VT_RST, this->scale);
        this->actor.colChkInfo.health = 20;
        this->collider.elem.atDmgInfo.effect = 4;
        this->collider.elem.atDmgInfo.damage = 16;
        Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
    }

    this->actor.shape.yOffset = this->yOffsetTarget = this->scale * -27500.0f;
    Shape_Info_init(&this->actor.shape, this->actor.shape.yOffset, Actor_shadow_circle, 0.0f);
    this->actor.colChkInfo.damageTable = &btl_data;
    Actor_BGcheck2(play, &this->actor, 35.0f, 60.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);

    surfaceType = T_BGCheck_getAttributeCode_ai(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);

    if ((surfaceType != FLOOR_TYPE_4) && (surfaceType != FLOOR_TYPE_7)) {
        Actor_delete(&this->actor);
        return;
    }

    this->actionfunc = mode_wake_init;
}

void En_Reeba_Actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnReeba* this = (EnReeba*)thisx;

    ClObjPipe_dt(play, &this->collider);

    if (this->actor.parent != NULL) {
        EnEncount1* spawner = (EnEncount1*)this->actor.parent;

        if (spawner->actor.update != NULL) {
            if (spawner->curNumSpawn > 0) {
                spawner->curNumSpawn--;
            }
            if (this->type != LEEVER_TYPE_SMALL) {
                spawner->bigLeever = NULL;
                spawner->timer = 600;
            }
        }
    }
}

void mode_wake_init(EnReeba* this, PlayState* play) {
    f32 frames = Si2_anime_end_frame(&object_reeba_Anim_0001E4);
    Player* player = GET_PLAYER(play);
    s16 absPlayerSpeedXZ;

    Skeleton_Info2_init(&this->skelanime, &object_reeba_Anim_0001E4, 2.0f, 0.0f, frames, ANIMMODE_LOOP, -10.0f);

    absPlayerSpeedXZ = fabsf(player->speedXZ);
    this->waitTimer = 20 - absPlayerSpeedXZ * 2;

    if (this->waitTimer < 0) {
        this->waitTimer = 2;
    }
    if (this->waitTimer > 20) {
        this->waitTimer = 20;
    }

    this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.world.pos.y = this->actor.floorHeight;

    if (this->type != LEEVER_TYPE_SMALL) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIVA_BIG_APPEAR);
    } else {
        Actor_SE_set(&this->actor, NA_SE_EN_RIVA_APPEAR);
    }

    this->actionfunc = mode_wake;
}

void mode_wake(EnReeba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 playerSpeedXZ;

    Skeleton_Info2_anime_play(&this->skelanime);

    if ((play->gameplayFrames % 4) == 0) {
        _dust_ground_set(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1, 8.0f,
                                 500, 10, true);
    }

    if (this->waitTimer == 0) {
        add_calc2(&this->actor.shape.shadowScale, 12.0f, 1.0f, 1.0f);
        if (this->actor.shape.yOffset < 0.0f) {
            add_calc0(&this->actor.shape.yOffset, 1.0f, this->yOffsetStep);
            add_calc2(&this->yOffsetStep, 300.0f, 1.0f, 5.0f);
        } else {
            this->yOffsetStep = 0.0f;
            this->actor.shape.yOffset = 0.0f;
            playerSpeedXZ = player->speedXZ;

            switch (this->aimType) {
                case 0:
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                    break;
                case 1:
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer + (800.0f * playerSpeedXZ);
                    break;
                case 2:
                case 3:
                    this->actor.world.rot.y =
                        this->actor.yawTowardsPlayer +
                        (player->actor.shape.rot.y - this->actor.yawTowardsPlayer) * (playerSpeedXZ * 0.15f);
                    break;
                case 4:
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer - (800.0f * playerSpeedXZ);
                    break;
            }

            if (this->type != LEEVER_TYPE_SMALL) {
                this->actionfunc = mode_boss_move_init;
            } else {
                this->moveTimer = 130;
                this->actor.speed = rnd_f(4.0f) + 6.0f;
                this->actionfunc = mode_move;
            }
        }
    }
}

static void mode_move(EnReeba* this, PlayState* play) {
    s32 surfaceType;

    Skeleton_Info2_anime_play(&this->skelanime);

    if (this->actor.shape.shadowScale < 12.0f) {
        add_calc2(&this->actor.shape.shadowScale, 12.0f, 3.0f, 1.0f);
    }

    surfaceType = T_BGCheck_getAttributeCode_ai(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);

    if ((surfaceType != FLOOR_TYPE_4) && (surfaceType != FLOOR_TYPE_7)) {
        this->actor.speed = 0.0f;
        this->actionfunc = mode_down_init;
    } else if ((this->moveTimer == 0) || (this->actor.xzDistToPlayer < 30.0f) ||
               (this->actor.xzDistToPlayer > 400.0f) || (this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
        this->actionfunc = mode_down_init;
    } else if (this->sfxTimer == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIVA_MOVE);
        this->sfxTimer = 10;
    }
}

void mode_boss_move_init(EnReeba* this, PlayState* play) {
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE;
    this->actionfunc = mode_boss_move;
}

void mode_boss_move(EnReeba* this, PlayState* play) {
    f32 accel;
    s16 yawDiff;
    s16 yaw;
    s32 surfaceType;

    Skeleton_Info2_anime_play(&this->skelanime);

    if (this->actor.shape.shadowScale < 12.0f) {
        add_calc2(&this->actor.shape.shadowScale, 12.0f, 3.0f, 1.0f);
    }

    surfaceType = T_BGCheck_getAttributeCode_ai(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);

    if (((surfaceType != FLOOR_TYPE_4) && (surfaceType != FLOOR_TYPE_7)) || (this->actor.xzDistToPlayer > 400.0f) ||
        (this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
        this->actionfunc = mode_down_init;
    } else {
        if ((this->actor.xzDistToPlayer < 70.0f) && (this->bigLeeverTimer == 0)) {
            this->bigLeeverTimer = 30;
        }

        accel = (this->actor.xzDistToPlayer - 20.0f) / ((fqrand() * 50.0f) + 150.0f);
        this->actor.speed += accel * 1.8f;

        if (this->actor.speed >= 3.0f) {
            this->actor.speed = 3.0f;
        }
        if (this->actor.speed < -3.0f) {
            this->actor.speed = -3.0f;
        }

        yawDiff = (this->bigLeeverTimer == 0) ? this->actor.yawTowardsPlayer : -this->actor.yawTowardsPlayer;
        yawDiff -= this->actor.world.rot.y;
        yaw = (yawDiff > 0) ? ((yawDiff / 31.0f) + 10.0f) : ((yawDiff / 31.0f) - 10.0f);
        this->actor.world.rot.y += yaw * 2.0f;

        if (this->sfxTimer == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_RIVA_MOVE);
            this->sfxTimer = 20;
        }
    }
}

void mode_handou_move(EnReeba* this, PlayState* play) {
    add_calc0(&this->actor.speed, 1.0f, 0.3f);

    if (this->moveTimer == 0) {
        if (this->type != LEEVER_TYPE_SMALL) {
            this->actionfunc = mode_boss_move_init;
        } else {
            this->actionfunc = mode_down_init;
        }
    }
}

static void mode_down_init(EnReeba* this, PlayState* play) {
    this->stunType = LEEVER_STUN_NONE;
    Actor_SE_set(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
    this->actionfunc = mode_down;
}

static void mode_down(EnReeba* this, PlayState* play) {
    add_calc0(&this->actor.shape.shadowScale, 1.0f, 0.3f);
    add_calc0(&this->actor.speed, 0.1f, 0.3f);
    Skeleton_Info2_anime_play(&this->skelanime);

    if ((this->yOffsetTarget + 10.0f) <= this->actor.shape.yOffset) {
        if ((play->gameplayFrames % 4) == 0) {
            _dust_ground_set(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1, 8.0f,
                                     500, 10, true);
        }

        add_calc2(&this->actor.shape.yOffset, this->yOffsetTarget, 1.0f, this->yOffsetStep);
        add_calc2(&this->yOffsetStep, 300.0f, 1.0f, 5.0f);
    } else {
        Actor_delete(&this->actor);
    }
}

static void mode_damage_init(EnReeba* this, PlayState* play) {
    this->damagedTimer = 14;
    this->actor.speed = -8.0f;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
    this->actionfunc = mode_damage;
}

static void mode_damage(EnReeba* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);

    if (this->actor.speed < 0.0f) {
        this->actor.speed += 1.0f;
    }

    if (this->damagedTimer == 0) {
        if (this->type != LEEVER_TYPE_SMALL) {
            this->bigLeeverTimer = 30;
            this->actionfunc = mode_boss_move_init;
        } else {
            this->actionfunc = mode_down_init;
        }
    }
}

void mode_special_damage_init(EnReeba* this, PlayState* play) {
    this->waitTimer = 14;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->actor.speed = -8.0f;
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
    this->actionfunc = mode_special_damage;
}

void mode_special_damage(EnReeba* this, PlayState* play) {
    if (this->waitTimer != 0) {
        if (this->actor.speed < 0.0f) {
            this->actor.speed += 1.0f;
        }
        return;
    } else {
        this->actor.speed = 0.0f;

        if ((this->stunType == LEEVER_STUN_OTHER) || (this->actor.colChkInfo.health != 0)) {
            Vec3f pos;
            f32 scale;

            if (this->stunType == LEEVER_STUN_ICE) {
                pos.x = this->actor.world.pos.x + rnd_fx(20.0f);
                pos.y = this->actor.world.pos.y + rnd_fx(20.0f);
                pos.z = this->actor.world.pos.z + rnd_fx(20.0f);
                scale = 3.0f;

                if (this->type != LEEVER_TYPE_SMALL) {
                    scale = 6.0f;
                }

                Effect_En_Ice_ct0(play, &this->actor, &pos, 150, 150, 150, 250, 235, 245, 255, scale);
            }

            this->waitTimer = 66;
            this->actionfunc = mode_buruburu;
            return;
        }

        this->waitTimer = 30;
        this->actionfunc = mode_dead_wait;
    }
}

static void mode_dead_wait(EnReeba* this, PlayState* play) {
    Vec3f pos;
    f32 scale;

    if (this->waitTimer != 0) {
        if ((this->stunType == LEEVER_STUN_ICE) && ((this->waitTimer & 0xF) == 0)) {
            pos.x = this->actor.world.pos.x + rnd_fx(20.0f);
            pos.y = this->actor.world.pos.y + rnd_fx(20.0f);
            pos.z = this->actor.world.pos.z + rnd_fx(20.0f);

            scale = 3.0f;
            if (this->type != LEEVER_TYPE_SMALL) {
                scale = 6.0f;
            }

            Effect_En_Ice_ct0(play, &this->actor, &pos, 150, 150, 150, 250, 235, 245, 255, scale);
        }
    } else {
        Actor_SE_set(&this->actor, NA_SE_EN_RIVA_DEAD);
        Actor_info_finish(play, &this->actor);
        this->actionfunc = mode_dead;
    }
}

static void mode_dead_init(EnReeba* this, PlayState* play) {
    this->actor.speed = -8.0f;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
    this->waitTimer = 14;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionfunc = mode_dead;
}

static void mode_dead(EnReeba* this, PlayState* play) {
    Vec3f pos;
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };

    if (this->waitTimer != 0) {
        if (this->actor.speed < 0.0f) {
            this->actor.speed += 1.0f;
        }
    } else {
        this->actor.speed = 0.0f;
        add_calc0(&this->scale, 0.1f, 0.01f);

        if (this->scale < 0.01f) {
            pos.x = this->actor.world.pos.x;
            pos.y = this->actor.world.pos.y;
            pos.z = this->actor.world.pos.z;

            velocity.y = 4.0f;

            _Effect_SS_Db_ct(play, &pos, &velocity, &accel, 120, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9, true);

            if (this->type == LEEVER_TYPE_SMALL) {
                Item_Set_Std(play, &this->actor, &pos, 0xE0);
            } else {
                Item_Set_Std(play, &this->actor, &pos, 0xC0);
            }

            if (this->actor.parent != NULL) {
                EnEncount1* spawner = (EnEncount1*)this->actor.parent;

                if ((spawner->actor.update != NULL) && this->type == LEEVER_TYPE_SMALL) {
                    if (spawner->killCount < 10) {
                        spawner->killCount++;
                    }
                    // "How many are dead?"
                    PRINTF("\n\n");
                    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 何匹ＤＥＡＤ？ ☆☆☆☆☆%d\n" VT_RST, spawner->killCount);
                    PRINTF("\n\n");
                }

                Actor_delete(&this->actor);
            }
        }
    }
}

void mode_buruburu(EnReeba* this, PlayState* play) {
    if (this->waitTimer < 37) {
        this->actor.shape.rot.x = rnd_fx(3000.0f);
        this->actor.shape.rot.z = rnd_fx(3000.0f);

        if (this->waitTimer == 0) {
            if (this->type != LEEVER_TYPE_SMALL) {
                this->actionfunc = mode_boss_move_init;
            } else {
                this->actionfunc = mode_down_init;
            }
        }
    }
}

void En_Reeba_damage_proc(EnReeba* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;

        if ((this->actionfunc != mode_dead) && (this->actionfunc != mode_damage)) {
            this->actor.shape.rot.x = this->actor.shape.rot.z = 0;
            this->stunType = LEEVER_STUN_NONE;

            switch (this->actor.colChkInfo.damageEffect) {
                case LEEVER_DMGEFF_UNUSED:
                case LEEVER_DMGEFF_BOOMERANG:
                    if ((this->actor.colChkInfo.health > 1) && (this->stunType != LEEVER_STUN_OTHER)) {
                        this->stunType = LEEVER_STUN_OTHER;
                        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
                        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA,
                                             80);
                        this->actionfunc = mode_special_damage_init;
                        break;
                    }
                    FALLTHROUGH;
                case LEEVER_DMGEFF_HOOKSHOT:
                    if ((this->actor.colChkInfo.health > 2) && (this->stunType != LEEVER_STUN_OTHER)) {
                        this->stunType = LEEVER_STUN_OTHER;
                        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA,
                                             80);
                        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
                        this->actionfunc = mode_special_damage_init;
                        break;
                    }
                    FALLTHROUGH;
                case LEEVER_DMGEFF_OTHER:
                    this->unkDamageField = 6;
                    hp_down(&this->actor);
                    if (this->actor.colChkInfo.health == 0) {
                        Actor_SE_set(&this->actor, NA_SE_EN_RIVA_DEAD);
                        Actor_info_finish(play, &this->actor);
                        this->actionfunc = mode_dead_init;
                    } else {
                        if (this->actionfunc == mode_buruburu) {
                            this->actor.shape.rot.x = this->actor.shape.rot.z = 0;
                        }
                        Actor_SE_set(&this->actor, NA_SE_EN_RIVA_DAMAGE);
                        this->actionfunc = mode_damage_init;
                    }
                    break;

                case LEEVER_DMGEFF_ICE:
                    hp_down(&this->actor);
                    this->unkDamageField = 2;
                    this->stunType = LEEVER_STUN_ICE;
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 80);
                    this->actionfunc = mode_special_damage_init;
                    break;

                case LEEVER_DMGEFF_UNK:
                    if (this->stunType != LEEVER_STUN_OTHER) {
                        this->stunType = LEEVER_STUN_OTHER;
                        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA,
                                             80);
                        this->actionfunc = mode_special_damage_init;
                    }
                    break;

                default:
                    break;
            }
        }
    }
}

void En_Reeba_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnReeba* this = (EnReeba*)thisx;
    Player* player = GET_PLAYER(play);

    En_Reeba_damage_proc(this, play);
    this->actionfunc(this, play);
    Actor_set_scale(&this->actor, this->scale);

    if (this->bigLeeverTimer != 0) {
        this->bigLeeverTimer--;
    }

    if (this->moveTimer != 0) {
        this->moveTimer--;
    }

    if (this->waitTimer != 0) {
        this->waitTimer--;
    }

    if (this->sfxTimer != 0) {
        this->sfxTimer--;
    }

    if (this->damagedTimer != 0) {
        this->damagedTimer--;
    }

    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 35.0f, 60.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);

    if (this->collider.base.atFlags & AT_BOUNCED) {
        this->collider.base.atFlags &= ~AT_BOUNCED;

        if ((this->actionfunc == mode_move) || (this->actionfunc == mode_boss_move)) {
            this->actor.speed = 8.0f;
            this->actor.world.rot.y *= -1.0f;
            this->moveTimer = 14;
            this->actionfunc = mode_handou_move;
            return;
        }
    }

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        if ((this->collider.base.at == &player->actor) && this->type == LEEVER_TYPE_SMALL &&
            (this->actionfunc != mode_down)) {
            this->actionfunc = mode_down_init;
        }
    }

    this->actor.focus.pos = this->actor.world.pos;

    if (this->type == LEEVER_TYPE_SMALL) {
        this->actor.focus.pos.y += 15.0f;
    } else {
        this->actor.focus.pos.y += 30.0f;
    }

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);

    if ((this->actor.shape.yOffset >= -700.0f) && (this->actor.colChkInfo.health > 0) &&
        (this->actionfunc != mode_down)) {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);

        if (!(this->actor.shape.yOffset < 0.0f)) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);

            if ((this->actionfunc == mode_move) || (this->actionfunc == mode_boss_move)) {
                CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
            }
        }
    }
}

void En_Reeba_display(Actor* thisx, PlayState* play) {
    s32 pad;
    EnReeba* this = (EnReeba*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_reeba.c", 1062);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    if (this->type != LEEVER_TYPE_SMALL) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0x0, 0x01, 155, 55, 255, 255);
    } else {
        gDPSetPrimColor(POLY_OPA_DISP++, 0x0, 0x01, 255, 255, 255, 255);
    }

    Si2_draw(play, this->skelanime.skeleton, this->skelanime.jointTable, NULL, NULL, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_reeba.c", 1088);

    if (DEBUG_FEATURES && BREG(0) != 0) {
        Vec3f debugPos;

        debugPos.x = (sin_s(this->actor.world.rot.y) * 30.0f) + this->actor.world.pos.x;
        debugPos.y = this->actor.world.pos.y + 20.0f;
        debugPos.z = (cos_s(this->actor.world.rot.y) * 30.0f) + this->actor.world.pos.z;
        Debug_Display_new(debugPos.x, debugPos.y, debugPos.z, this->actor.world.rot.x, this->actor.world.rot.y,
                               this->actor.world.rot.z, 1.0f, 1.0f, 1.0f, 255, 0, 0, 255, 4, play->state.gfxCtx);
    }
}
