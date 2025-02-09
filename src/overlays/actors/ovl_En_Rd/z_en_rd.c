/*
 * File: z_en_rd.c
 * Overlay: ovl_En_Rd
 * Description: Redead and Gibdo
 */

#include "z_en_rd.h"

#include "libc64/qrand.h"
#include "attributes.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "rumble.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"

#include "global.h"

#include "assets/objects/object_rd/object_rd.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

void En_Rd_Actor_ct(Actor* thisx, PlayState* play);
void En_Rd_Actor_dt(Actor* thisx, PlayState* play);
void En_Rd_move(Actor* thisx, PlayState* play);
void En_Rd_display(Actor* thisx, PlayState* play);

void En_Rd_Actor_mode_wait_init(EnRd* this);
void En_Rd_Actor_mode_wait2_init(EnRd* this);
void En_Rd_Actor_mode_gohome_init(EnRd* this, PlayState* play);
void En_Rd_Actor_mode_gomother_init(EnRd* this);
void En_Rd_Actor_mode_attack_init(EnRd* this);
void En_Rd_Actor_mode_find_init(EnRd* this);
void En_Rd_Actor_mode_stand_init(EnRd* this);
void En_Rd_Actor_mode_shit_init(EnRd* this);
void En_Rd_Actor_mode_wait(EnRd* this, PlayState* play);
void En_Rd_Actor_mode_wait2(EnRd* this, PlayState* play);
void En_Rd_Actor_mode_forward(EnRd* this, PlayState* play);
void En_Rd_Actor_mode_gohome(EnRd* this, PlayState* play);
void En_Rd_Actor_mode_gomother(EnRd* this, PlayState* play);
void En_Rd_Actor_mode_attack(EnRd* this, PlayState* play);
void En_Rd_Actor_mode_find(EnRd* this, PlayState* play);
void En_Rd_Actor_mode_stand(EnRd* this, PlayState* play);
void En_Rd_Actor_mode_shit(EnRd* this, PlayState* play);
void En_Rd_Actor_mode_damage(EnRd* this, PlayState* play);
void En_Rd_Actor_mode_down(EnRd* this, PlayState* play);
void En_Rd_Actor_mode_paralyze(EnRd* this, PlayState* play);

typedef enum EnRdAction {
    /*  0 */ REDEAD_ACTION_IDLE,
    /*  1 */ REDEAD_ACTION_STUNNED,
    /*  2 */ REDEAD_ACTION_WALK_TO_HOME,
    /*  3 */ REDEAD_ACTION_WALK_TO_PARENT,
    /*  4 */ REDEAD_ACTION_WALK_TO_PLAYER_OR_RELEASE_GRAB,
    /*  5 */ REDEAD_ACTION_STAND_UP,
    /*  6 */ REDEAD_ACTION_CROUCH,
    /*  7 */ REDEAD_ACTION_ATTEMPT_PLAYER_FREEZE,
    /*  8 */ REDEAD_ACTION_GRAB,
    /*  9 */ REDEAD_ACTION_DAMAGED,
    /* 10 */ REDEAD_ACTION_DEAD,
    /* 11 */ REDEAD_ACTION_RISE_FROM_COFFIN
} EnRdAction;

typedef enum EnRdGrabState {
    /* 0 */ REDEAD_GRAB_START,
    /* 1 */ REDEAD_GRAB_INITIAL_DAMAGE,
    /* 2 */ REDEAD_GRAB_ATTACK,
    /* 3 */ REDEAD_GRAB_RELEASE,
    /* 4 */ REDEAD_GRAB_END
} EnRdGrabState;

ActorProfile En_Rd_Profile = {
    /**/ ACTOR_EN_RD,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_RD,
    /**/ sizeof(EnRd),
    /**/ En_Rd_Actor_ct,
    /**/ En_Rd_Actor_dt,
    /**/ En_Rd_move,
    /**/ En_Rd_display,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

typedef enum EnRdDamageEffect {
    /* 0x0 */ REDEAD_DMGEFF_NONE,              // Does not interact with the Gibdo/Redead at all
    /* 0x1 */ REDEAD_DMGEFF_HOOKSHOT,          // Stuns the Gibdo/Redead
    /* 0x6 */ REDEAD_DMGEFF_ICE_MAGIC = 0x6,   // Does not interact with the Gibdo/Redead at all
    /* 0xD */ REDEAD_DMGEFF_LIGHT_MAGIC = 0xD, // Stuns the Gibdo/Redead
    /* 0xE */ REDEAD_DMGEFF_FIRE_MAGIC,        // Applies a fire effect
    /* 0xF */ REDEAD_DMGEFF_DAMAGE             // Deals damage without stunning or applying an effect
} EnRdDamageEffect;

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, REDEAD_DMGEFF_NONE),
    /* Deku stick    */ DMG_ENTRY(2, REDEAD_DMGEFF_DAMAGE),
    /* Slingshot     */ DMG_ENTRY(0, REDEAD_DMGEFF_NONE),
    /* Explosive     */ DMG_ENTRY(0, REDEAD_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(0, REDEAD_DMGEFF_NONE),
    /* Normal arrow  */ DMG_ENTRY(0, REDEAD_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, REDEAD_DMGEFF_DAMAGE),
    /* Hookshot      */ DMG_ENTRY(0, REDEAD_DMGEFF_HOOKSHOT),
    /* Kokiri sword  */ DMG_ENTRY(1, REDEAD_DMGEFF_DAMAGE),
    /* Master sword  */ DMG_ENTRY(2, REDEAD_DMGEFF_DAMAGE),
    /* Giant's Knife */ DMG_ENTRY(4, REDEAD_DMGEFF_DAMAGE),
    /* Fire arrow    */ DMG_ENTRY(0, REDEAD_DMGEFF_NONE),
    /* Ice arrow     */ DMG_ENTRY(0, REDEAD_DMGEFF_NONE),
    /* Light arrow   */ DMG_ENTRY(0, REDEAD_DMGEFF_NONE),
    /* Unk arrow 1   */ DMG_ENTRY(0, REDEAD_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(0, REDEAD_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(0, REDEAD_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(4, REDEAD_DMGEFF_FIRE_MAGIC),
    /* Ice magic     */ DMG_ENTRY(0, REDEAD_DMGEFF_ICE_MAGIC),
    /* Light magic   */ DMG_ENTRY(3, REDEAD_DMGEFF_LIGHT_MAGIC),
    /* Shield        */ DMG_ENTRY(0, REDEAD_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, REDEAD_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, REDEAD_DMGEFF_DAMAGE),
    /* Giant spin    */ DMG_ENTRY(4, REDEAD_DMGEFF_DAMAGE),
    /* Master spin   */ DMG_ENTRY(2, REDEAD_DMGEFF_DAMAGE),
    /* Kokiri jump   */ DMG_ENTRY(2, REDEAD_DMGEFF_DAMAGE),
    /* Giant jump    */ DMG_ENTRY(8, REDEAD_DMGEFF_DAMAGE),
    /* Master jump   */ DMG_ENTRY(4, REDEAD_DMGEFF_DAMAGE),
    /* Unknown 1     */ DMG_ENTRY(0, REDEAD_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, REDEAD_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, REDEAD_DMGEFF_DAMAGE),
    /* Unknown 2     */ DMG_ENTRY(0, REDEAD_DMGEFF_NONE),
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -3500, ICHAIN_STOP),
};

void En_Rd_actor_set_process(EnRd* this, EnRdActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Rd_Actor_ct(Actor* thisx, PlayState* play) {
    EnRd* this = (EnRd*)thisx;

    ValueSet_process(thisx, value_init);
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    this->actor.colChkInfo.damageTable = &btl_data;
    Shape_Info_init(&thisx->shape, 0.0f, NULL, 0.0f);
    this->upperBodyYRotation = this->headYRotation = 0;
    this->actor.focus.pos = thisx->world.pos;
    this->actor.focus.pos.y += 50.0f;
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.colChkInfo.health = 8;
    this->alpha = this->unk_31D = 255;
    this->rdFlags = REDEAD_GET_RDFLAGS(thisx);

    if (PARAMS_GET_NOSHIFT(this->actor.params, 7, 1)) {
        this->actor.params |= 0xFF00;
    } else {
        this->actor.params &= 0xFF;
    }

    if (this->actor.params >= REDEAD_TYPE_DOES_NOT_MOURN) {
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gRedeadSkel, &gGibdoRedeadIdleAnim, this->jointTable,
                           this->morphTable, REDEAD_GIBDO_LIMB_MAX);
        this->actor.naviEnemyId = NAVI_ENEMY_REDEAD;
    } else {
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGibdoSkel, &gGibdoRedeadIdleAnim, this->jointTable,
                           this->morphTable, REDEAD_GIBDO_LIMB_MAX);
        this->actor.naviEnemyId = NAVI_ENEMY_GIBDO;
    }

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, thisx, &OcInfoData);

    if (this->actor.params >= REDEAD_TYPE_GIBDO) {
        En_Rd_Actor_mode_wait_init(this);
    } else {
        En_Rd_Actor_mode_wait2_init(this);
    }

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->actor.params == REDEAD_TYPE_INVISIBLE) {
        this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
    }
}

void En_Rd_Actor_dt(Actor* thisx, PlayState* play) {
    EnRd* this = (EnRd*)thisx;

    if (z_common_data.sunsSongState != SUNSSONG_INACTIVE) {
        z_common_data.sunsSongState = SUNSSONG_INACTIVE;
    }

    ClObjPipe_dt(play, &this->collider);
}

/**
 * The `parent` pointer is updated for all currently loaded Redeads depending
 * on the `shouldMourn` variable.
 *
 * If `shouldMourn` is true, the parent of all other Redeads is set to this
 * instance so they will mourn this Redead.
 *
 * If `shouldMourn` is false, the parent of all other Redeads is cleared so that
 * they stop mourning. This is done when the dead Redead starts fading away.
 */
void Rd_M_set(PlayState* play, Actor* thisx, s32 shouldMourn) {
    Actor* enemyIterator = play->actorCtx.actorLists[ACTORCAT_ENEMY].head;

    while (enemyIterator != NULL) {
        if ((enemyIterator->id != ACTOR_EN_RD) || (enemyIterator == thisx) ||
            (enemyIterator->params < REDEAD_TYPE_DOES_NOT_MOURN_IF_WALKING)) {
            enemyIterator = enemyIterator->next;
            continue;
        }

        if (shouldMourn) {
            enemyIterator->parent = thisx;
        } else if (enemyIterator->parent == thisx) {
            enemyIterator->parent = NULL;
        }

        enemyIterator = enemyIterator->next;
    }
}

void En_Rd_Actor_mode_wait_init(EnRd* this) {
    if (this->actor.params != REDEAD_TYPE_CRYING) {
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGibdoRedeadIdleAnim, -6.0f);
    } else {
        Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gGibdoRedeadSobbingAnim);
    }

    this->action = REDEAD_ACTION_IDLE;
    this->timer = (fqrand() * 10.0f) + 5.0f;
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    En_Rd_actor_set_process(this, En_Rd_Actor_mode_wait);
}

void En_Rd_Actor_mode_wait(EnRd* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc_short_angle2(&this->headYRotation, 0, 1, 0x64, 0);
    add_calc_short_angle2(&this->upperBodyYRotation, 0, 1, 0x64, 0);

    if ((this->actor.params == REDEAD_TYPE_CRYING) && (this->skelAnime.curFrame == 0.0f)) {
        if (fqrand() >= 0.5f) {
            Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gGibdoRedeadSobbingAnim);
        } else {
            Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gGibdoRedeadWipingTearsAnim);
        }
    } else {
        this->timer--;
        if (this->timer == 0) {
            // This resets the idle animation back to its first frame, making the
            // Redead/Gibdo appear to "judder" in place.
            this->timer = (fqrand() * 10.0f) + 10.0f;
            this->skelAnime.curFrame = 0.0f;
        }
    }

    if (this->actor.parent != NULL) {
        if (!this->isMourning) {
            if (this->actor.params != REDEAD_TYPE_CRYING) {
                En_Rd_Actor_mode_gomother_init(this);
            } else {
                En_Rd_Actor_mode_stand_init(this);
            }
        }
    } else {
        if (this->isMourning) {
            if (this->actor.params != REDEAD_TYPE_CRYING) {
                En_Rd_Actor_mode_find_init(this);
            } else {
                En_Rd_Actor_mode_stand_init(this);
            }
        }

        this->isMourning = false;
        if ((this->actor.xzDistToPlayer <= 150.0f) && player_big_SE_action_check(play)) {
            if ((this->actor.params != REDEAD_TYPE_CRYING) && !this->isMourning) {
                En_Rd_Actor_mode_find_init(this);
            } else {
                En_Rd_Actor_mode_stand_init(this);
            }
        }
    }

    if ((play->gameplayFrames & 0x5F) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_REDEAD_CRY);
    }
}

void En_Rd_Actor_mode_wait2_init(EnRd* this) {
    Skeleton_Info2_init(&this->skelAnime, &gGibdoRedeadIdleAnim, 0, 0, Si2_anime_end_frame(&gGibdoRedeadIdleAnim),
                     ANIMMODE_LOOP, -6.0f);
    this->action = REDEAD_ACTION_RISE_FROM_COFFIN;
    this->timer = 6;
    this->actor.shape.rot.x = -0x4000;
    this->actor.gravity = 0.0f;
    this->actor.shape.yOffset = 0.0f;
    this->actor.speed = 0.0f;
    En_Rd_actor_set_process(this, En_Rd_Actor_mode_wait2);
}

void En_Rd_Actor_mode_wait2(EnRd* this, PlayState* play) {
    if (this->actor.shape.rot.x != -0x4000) {
        add_calc_short_angle2(&this->actor.shape.rot.x, 0, 1, 0x7D0, 0);
        if (add_calc(&this->actor.world.pos.y, this->actor.home.pos.y, 0.3f, 2.0f, 0.3f) == 0.0f) {
            this->actor.gravity = -3.5f;
            En_Rd_Actor_mode_wait_init(this);
        }
    } else {
        if (this->actor.world.pos.y == this->actor.home.pos.y) {
            Actor_SE_set(&this->actor, NA_SE_EN_REDEAD_CRY);
        }

        if (add_calc(&this->actor.world.pos.y, this->actor.home.pos.y + 50.0f, 0.3f, 2.0f, 0.3f) == 0.0f) {
            if (this->timer != 0) {
                this->timer--;
                add_calc(&this->actor.speed, 6.0f, 0.3f, 1.0f, 0.3f);
            } else if (add_calc(&this->actor.speed, 0.0f, 0.3f, 1.0f, 0.3f) == 0.0f) {
                add_calc_short_angle2(&this->actor.shape.rot.x, 0, 1, 0x7D0, 0);
            }
        }
    }
}

void En_Rd_Actor_mode_forward_init(EnRd* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gGibdoRedeadWalkAnim, 1.0f, 4.0f, Si2_anime_end_frame(&gGibdoRedeadWalkAnim),
                     ANIMMODE_LOOP_INTERP, -4.0f);
    this->actor.speed = 0.4f;
    this->action = REDEAD_ACTION_WALK_TO_PLAYER_OR_RELEASE_GRAB;
    En_Rd_actor_set_process(this, En_Rd_Actor_mode_forward);
}

void En_Rd_Actor_mode_forward(EnRd* this, PlayState* play) {
    Vec3f D_80AE4918 = { 0.0f, 0.0f, 0.0f };
    Color_RGBA8 D_80AE4924 = { 200, 200, 255, 255 };
    Color_RGBA8 D_80AE4928 = { 0, 0, 255, 0 };
    Player* player = GET_PLAYER(play);
    s32 pad;
    s16 yaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y - this->headYRotation - this->upperBodyYRotation;

    this->skelAnime.playSpeed = this->actor.speed;
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA, 0);
    add_calc_short_angle2(&this->headYRotation, 0, 1, 0x64, 0);
    add_calc_short_angle2(&this->upperBodyYRotation, 0, 1, 0x64, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (Actor_search_position_distance(&player->actor, &this->actor.home.pos) >= 150.0f) {
        En_Rd_Actor_mode_gohome_init(this, play);
    }

    if ((ABS(yaw) < 0x1554) && (Actor_search_actor_distance(&this->actor, &player->actor) <= 150.0f)) {
        if (!(player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_18 |
                                     PLAYER_STATE1_19 | PLAYER_STATE1_21)) &&
            !(player->stateFlags2 & PLAYER_STATE2_7)) {
            if (this->playerStunWaitTimer == 0) {
                if (!(this->rdFlags & 0x80)) {
                    player->actor.freezeTimer = 40;

                    // `player->actor.freezeTimer` gets set above which will prevent Player from updating.
                    // Because of this, he cannot update things related to Z-Targeting.
                    // If Player can't update, `player->zTargetActiveTimer` won't update, which means
                    // the Attention system will not be notified of a new actor lock-on occurring.
                    // So, no reticle will appear. But the camera will still focus on the actor.
                    to_anchor_set(play, &this->actor);

                    // This is redundant, `autoLockOnActor` gets set by `to_anchor_set` above
                    GET_PLAYER(play)->autoLockOnActor = &this->actor;

                    z_vibctl2_vib_setQ(this->actor.xzDistToPlayer, 255, 20, 150);
                }

                this->playerStunWaitTimer = 60;
                Actor_SE_set(&this->actor, NA_SE_EN_REDEAD_AIM);
            }
        } else {
            En_Rd_Actor_mode_gohome_init(this, play);
        }
    }

    if (this->grabWaitTimer != 0) {
        this->grabWaitTimer--;
    }

    if (!this->grabWaitTimer && (Actor_search_actor_distance(&this->actor, &player->actor) <= 45.0f) &&
        Actor_player_direction_check(&this->actor, 0x38E3)) {
        player->actor.freezeTimer = 0;
        if (play->grabPlayer(play, player)) {
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            En_Rd_Actor_mode_attack_init(this);
        }
    } else if (this->actor.params > REDEAD_TYPE_DOES_NOT_MOURN_IF_WALKING) {
        if (this->actor.parent != NULL) {
            En_Rd_Actor_mode_gomother_init(this);
        } else {
            this->isMourning = false;
        }
    }

    if ((this->skelAnime.curFrame == 10.0f) || (this->skelAnime.curFrame == 22.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_WALK);
    } else if ((play->gameplayFrames & 0x5F) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_REDEAD_CRY);
    }
}

void En_Rd_Actor_mode_gohome_init(EnRd* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gGibdoRedeadWalkAnim, 0.5f, 0, Si2_anime_end_frame(&gGibdoRedeadWalkAnim),
                     ANIMMODE_LOOP_INTERP, -4.0f);
    this->action = REDEAD_ACTION_WALK_TO_HOME;
    En_Rd_actor_set_process(this, En_Rd_Actor_mode_gohome);
}

void En_Rd_Actor_mode_gohome(EnRd* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    s16 targetY = Actor_search_position_angleY(&this->actor, &this->actor.home.pos);

    if (Actor_search_position_distance(&this->actor, &this->actor.home.pos) >= 5.0f) {
        add_calc_short_angle2(&this->actor.shape.rot.y, targetY, 1, 0x1C2, 0);
    } else {
        this->actor.speed = 0.0f;
        if (add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.home.rot.y, 1, 0x1C2, 0) == 0) {
            if (this->actor.params != REDEAD_TYPE_CRYING) {
                En_Rd_Actor_mode_wait_init(this);
            } else {
                En_Rd_Actor_mode_shit_init(this);
            }
        }
    }

    add_calc_short_angle2(&this->headYRotation, 0, 1, 0x64, 0);
    add_calc_short_angle2(&this->upperBodyYRotation, 0, 1, 0x64, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (!(player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_18 |
                                 PLAYER_STATE1_19 | PLAYER_STATE1_21)) &&
        !(player->stateFlags2 & PLAYER_STATE2_7) &&
        (Actor_search_position_distance(&player->actor, &this->actor.home.pos) < 150.0f)) {
        this->actor.attentionRangeType = ATTENTION_RANGE_0;
        En_Rd_Actor_mode_forward_init(this, play);
    } else if (this->actor.params > REDEAD_TYPE_DOES_NOT_MOURN_IF_WALKING) {
        if (this->actor.parent != NULL) {
            En_Rd_Actor_mode_gomother_init(this);
        } else {
            this->isMourning = false;
        }
    }

    if (this->skelAnime.curFrame == 10.0f || this->skelAnime.curFrame == 22.0f) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_WALK);
    } else if ((play->gameplayFrames & 0x5F) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_REDEAD_CRY);
    }
}

void En_Rd_Actor_mode_gomother_init(EnRd* this) {
    Skeleton_Info2_init(&this->skelAnime, &gGibdoRedeadWalkAnim, 0.5f, 0, Si2_anime_end_frame(&gGibdoRedeadWalkAnim),
                     ANIMMODE_LOOP_INTERP, -4.0f);
    this->action = REDEAD_ACTION_WALK_TO_PARENT;
    this->isMourning = true;
    En_Rd_actor_set_process(this, En_Rd_Actor_mode_gomother);
}

/**
 * When a Redead or Gibdo dies, it sets itself to be the parent for all other
 * Redeads in the area that are capable of mourning. This function will make
 * these Redeads walk over to the corpse and stand near until it begins to
 * fade away.
 */
void En_Rd_Actor_mode_gomother(EnRd* this, PlayState* play) {
    s32 pad;
    s16 targetY;
    Vec3f parentPos;

    if (this->actor.parent != NULL) {
        parentPos = this->actor.parent->world.pos;

        targetY = Actor_search_position_angleY(&this->actor, &parentPos);

        add_calc_short_angle2(&this->actor.shape.rot.y, targetY, 1, 0xFA, 0);

        if (Actor_search_position_distance(&this->actor, &parentPos) >= 45.0f) {
            this->actor.speed = 0.4f;
        } else {
            this->actor.speed = 0.0f;

            if (this->actor.params != REDEAD_TYPE_CRYING) {
                En_Rd_Actor_mode_wait_init(this);
            } else {
                En_Rd_Actor_mode_shit_init(this);
            }
        }

        add_calc_short_angle2(&this->headYRotation, 0, 1, 0x64, 0);
        add_calc_short_angle2(&this->upperBodyYRotation, 0, 1, 0x64, 0);
    } else {
        En_Rd_Actor_mode_forward_init(this, play);
    }

    this->actor.world.rot.y = this->actor.shape.rot.y;
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->skelAnime.curFrame == 10.0f || this->skelAnime.curFrame == 22.0f) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_WALK);
    } else if ((play->gameplayFrames & 0x5F) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_REDEAD_CRY);
    }
}

void En_Rd_Actor_mode_attack_init(EnRd* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gGibdoRedeadGrabStartAnim);
    this->timer = this->grabState = 0;
    this->grabDamageTimer = 200;
    this->action = REDEAD_ACTION_GRAB;
    this->actor.speed = 0.0f;
    En_Rd_actor_set_process(this, En_Rd_Actor_mode_attack);
}

void En_Rd_Actor_mode_attack(EnRd* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->grabState++;
    }

    switch (this->grabState) {
        case REDEAD_GRAB_INITIAL_DAMAGE:
            Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gGibdoRedeadGrabAttackAnim);
            this->grabState++;
            play->damagePlayer(play, -8);
            z_vibctl2_vib_setQ(this->actor.xzDistToPlayer, 255, 1, 12);
            this->grabDamageTimer = 20;
            FALLTHROUGH;
        case REDEAD_GRAB_START:
            add_calc_short_angle2(&this->headYRotation, 0, 1, 0x5DC, 0);
            add_calc_short_angle2(&this->upperBodyYRotation, 0, 1, 0x5DC, 0);
            FALLTHROUGH;
        case REDEAD_GRAB_ATTACK:
            if (!(player->stateFlags2 & PLAYER_STATE2_7)) {
                Skeleton_Info2_init(&this->skelAnime, &gGibdoRedeadGrabEndAnim, 0.5f, 0.0f,
                                 Si2_anime_end_frame(&gGibdoRedeadGrabEndAnim), ANIMMODE_ONCE_INTERP, 0.0f);
                this->grabState++;
                this->action = REDEAD_ACTION_WALK_TO_PLAYER_OR_RELEASE_GRAB;
                return;
            }

            if (!LINK_IS_ADULT) {
                add_calc(&this->actor.shape.yOffset, -1500.0f, 1.0f, 150.0f, 0.0f);
            }

            add_calc(&this->actor.world.pos.x,
                               (sin_s(player->actor.shape.rot.y) * -25.0f) + player->actor.world.pos.x, 1.0f, 10.0f,
                               0.0f);
            add_calc(&this->actor.world.pos.y, player->actor.world.pos.y, 1.0f, 10.0f, 0.0f);
            add_calc(&this->actor.world.pos.z,
                               (cos_s(player->actor.shape.rot.y) * -25.0f) + player->actor.world.pos.z, 1.0f, 10.0f,
                               0.0f);
            add_calc_short_angle2(&this->actor.shape.rot.y, player->actor.shape.rot.y, 1, 0x1770, 0);

            if (this->skelAnime.curFrame == 0.0f) {
                Actor_SE_set(&this->actor, NA_SE_EN_REDEAD_ATTACK);
            }

            this->grabDamageTimer--;
            if (this->grabDamageTimer == 0) {
                play->damagePlayer(play, -8);
                z_vibctl2_vib_setQ(this->actor.xzDistToPlayer, 240, 1, 12);
                this->grabDamageTimer = 20;
                player_SE_set(player, NA_SE_VO_LI_DAMAGE_S + player->ageProperties->unk_92);
            }
            break;

        case REDEAD_GRAB_RELEASE:
            if (!LINK_IS_ADULT) {
                add_calc(&this->actor.shape.yOffset, 0, 1.0f, 400.0f, 0.0f);
            }
            break;

        case REDEAD_GRAB_END:
            if (!LINK_IS_ADULT) {
                add_calc(&this->actor.shape.yOffset, 0, 1.0f, 400.0f, 0.0f);
            }
            this->actor.attentionRangeType = ATTENTION_RANGE_0;
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->playerStunWaitTimer = 0xA;
            this->grabWaitTimer = 0xF;
            En_Rd_Actor_mode_forward_init(this, play);
            break;
    }
}

void En_Rd_Actor_mode_find_init(EnRd* this) {
    Skeleton_Info2_init(&this->skelAnime, &gGibdoRedeadLookBackAnim, 0.0f, 0.0f,
                     Si2_anime_end_frame(&gGibdoRedeadLookBackAnim), ANIMMODE_ONCE, 0.0f);
    this->action = REDEAD_ACTION_ATTEMPT_PLAYER_FREEZE;
    En_Rd_actor_set_process(this, En_Rd_Actor_mode_find);
}

void En_Rd_Actor_mode_find(EnRd* this, PlayState* play) {
    Vec3f D_80AE492C = { 0.0f, 0.0f, 0.0f };
    Color_RGBA8 D_80AE4938 = { 200, 200, 255, 255 };
    Color_RGBA8 D_80AE493C = { 0, 0, 255, 0 };
    Player* player = GET_PLAYER(play);
    s16 yaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y - this->headYRotation - this->upperBodyYRotation;

    if (ABS(yaw) < 0x2008) {
        if (!(this->rdFlags & 0x80)) {
            player->actor.freezeTimer = 60;
            z_vibctl2_vib_setQ(this->actor.xzDistToPlayer, 255, 20, 150);

            // The same note mentioned with this function call in `En_Rd_Actor_mode_forward` applies here too
            to_anchor_set(play, &this->actor);
        }

        Actor_SE_set(&this->actor, NA_SE_EN_REDEAD_AIM);
        En_Rd_Actor_mode_forward_init(this, play);
    }
}

void En_Rd_Actor_mode_stand_init(EnRd* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGibdoRedeadStandUpAnim, -4.0f);
    this->action = REDEAD_ACTION_STAND_UP;
    En_Rd_actor_set_process(this, En_Rd_Actor_mode_stand);
}

void En_Rd_Actor_mode_stand(EnRd* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->actor.parent != NULL) {
            En_Rd_Actor_mode_gomother_init(this);
        } else {
            En_Rd_Actor_mode_find_init(this);
        }
    }
}

void En_Rd_Actor_mode_shit_init(EnRd* this) {
    Skeleton_Info2_init(&this->skelAnime, &gGibdoRedeadStandUpAnim, -1.0f,
                     Si2_anime_end_frame(&gGibdoRedeadStandUpAnim), 0.0f, ANIMMODE_ONCE, -4.0f);
    this->action = REDEAD_ACTION_CROUCH;
    En_Rd_actor_set_process(this, En_Rd_Actor_mode_shit);
}

void En_Rd_Actor_mode_shit(EnRd* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        En_Rd_Actor_mode_wait_init(this);
    }
}

void En_Rd_Actor_mode_damage_init(EnRd* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGibdoRedeadDamageAnim, -6.0f);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = -2.0f;
    }

    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_SE_set(&this->actor, NA_SE_EN_REDEAD_DAMAGE);
    this->action = REDEAD_ACTION_DAMAGED;
    En_Rd_actor_set_process(this, En_Rd_Actor_mode_damage);
}

void En_Rd_Actor_mode_damage(EnRd* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actor.speed < 0.0f) {
        this->actor.speed += 0.15f;
    }

    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    add_calc_short_angle2(&this->headYRotation, 0, 1, 0x12C, 0);
    add_calc_short_angle2(&this->upperBodyYRotation, 0, 1, 0x12C, 0);
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->actor.world.rot.y = this->actor.shape.rot.y;

        if (this->actor.parent != NULL) {
            En_Rd_Actor_mode_gomother_init(this);
        } else if (Actor_search_position_distance(&player->actor, &this->actor.home.pos) >= 150.0f) {
            En_Rd_Actor_mode_gohome_init(this, play);
        } else {
            En_Rd_Actor_mode_forward_init(this, play);
        }

        this->unk_31D = 0xFF;
    }
}

void En_Rd_Actor_mode_down_init(EnRd* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGibdoRedeadDeathAnim, -1.0f);
    this->action = REDEAD_ACTION_DEAD;
    this->timer = 300;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.speed = 0.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_REDEAD_DEAD);
    En_Rd_actor_set_process(this, En_Rd_Actor_mode_down);
}

void En_Rd_Actor_mode_down(EnRd* this, PlayState* play) {
    if (this->actor.category != ACTORCAT_PROP) {
        Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
    }

    add_calc_short_angle2(&this->headYRotation, 0, 1, 0x7D0, 0);
    add_calc_short_angle2(&this->upperBodyYRotation, 0, 1, 0x7D0, 0);

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->timer == 0) {
            if (!Actor_Environment_sw_Check(play, this->rdFlags & 0x7F)) {
                Actor_Environment_sw_On(play, this->rdFlags & 0x7F);
            }

            if (this->alpha != 0) {
                if (this->alpha == 180) {
                    Rd_M_set(play, &this->actor, false);
                }

                this->actor.scale.y -= 0.000075f;
                this->alpha -= 5;
            } else {
                Actor_delete(&this->actor);
            }
        } else {
            this->timer--;
        }
    } else if (((s32)this->skelAnime.curFrame == 33) || ((s32)this->skelAnime.curFrame == 40)) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_DOWN);
    }
}

void En_Rd_Actor_mode_paralyze_init(EnRd* this) {
    this->action = REDEAD_ACTION_STUNNED;
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (z_common_data.sunsSongState != SUNSSONG_INACTIVE) {
        this->stunnedBySunsSong = true;
        this->sunsSongStunTimer = 600;
        Actor_SE_set(&this->actor, NA_SE_EN_LIGHT_ARROW_HIT);
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_GRAY, COLORFILTER_INTENSITY_FLAG | 200,
                             COLORFILTER_BUFFLAG_OPA, 255);
    } else if (this->damageEffect == REDEAD_DMGEFF_HOOKSHOT) {
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 200, COLORFILTER_BUFFLAG_OPA, 80);
    } else {
        Actor_SE_set(&this->actor, NA_SE_EN_LIGHT_ARROW_HIT);
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_GRAY, 200, COLORFILTER_BUFFLAG_OPA, 80);
    }

    En_Rd_actor_set_process(this, En_Rd_Actor_mode_paralyze);
}

void En_Rd_Actor_mode_paralyze(EnRd* this, PlayState* play) {
    if (this->stunnedBySunsSong && (this->sunsSongStunTimer != 0)) {
        this->sunsSongStunTimer--;
        if (this->sunsSongStunTimer >= 255) {
            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_GRAY, 200, COLORFILTER_BUFFLAG_OPA, 255);
        }

        if (this->sunsSongStunTimer == 0) {
            this->stunnedBySunsSong = false;
            z_common_data.sunsSongState = SUNSSONG_INACTIVE;
        }
    }

    if (this->actor.colorFilterTimer == 0) {
        if (this->actor.colChkInfo.health == 0) {
            Rd_M_set(play, &this->actor, true);
            En_Rd_Actor_mode_down_init(this);
            Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x90);
        } else {
            En_Rd_Actor_mode_damage_init(this);
        }
    }
}

void En_Rd_Face_To_Player(EnRd* this, PlayState* play) {
    s16 headAngleTemp = this->actor.yawTowardsPlayer - (s16)(this->actor.shape.rot.y + this->upperBodyYRotation);
    s16 upperBodyAngle = CLAMP(headAngleTemp, -500, 500);
    s16 headAngle;

    headAngleTemp -= this->headYRotation;
    headAngle = CLAMP(headAngleTemp, -500, 500);

    if ((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y) >= 0) {
        this->upperBodyYRotation += ABS(upperBodyAngle);
        this->headYRotation += ABS(headAngle);
    } else {
        this->upperBodyYRotation -= ABS(upperBodyAngle);
        this->headYRotation -= ABS(headAngle);
    }

    this->upperBodyYRotation = CLAMP(this->upperBodyYRotation, -0x495F, 0x495F);
    this->headYRotation = CLAMP(this->headYRotation, -0x256F, 0x256F);
}

void En_Rd_damage_proc(EnRd* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    if ((z_common_data.sunsSongState != SUNSSONG_INACTIVE) && (this->actor.shape.rot.x == 0) &&
        !this->stunnedBySunsSong && (this->action != REDEAD_ACTION_DAMAGED) && (this->action != REDEAD_ACTION_DEAD) &&
        (this->action != REDEAD_ACTION_STUNNED)) {
        En_Rd_Actor_mode_paralyze_init(this);
        return;
    }

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        this->damageEffect = this->actor.colChkInfo.damageEffect;

        if (this->action != REDEAD_ACTION_RISE_FROM_COFFIN) {
            Hit_bit_set(&this->actor, &this->collider.elem, true);
            if (player->unk_844 != 0) {
                this->unk_31D = player->unk_845;
            }

            if ((this->damageEffect != REDEAD_DMGEFF_NONE) && (this->damageEffect != REDEAD_DMGEFF_ICE_MAGIC)) {
                if (((this->damageEffect == REDEAD_DMGEFF_HOOKSHOT) ||
                     (this->damageEffect == REDEAD_DMGEFF_LIGHT_MAGIC)) &&
                    (this->action != REDEAD_ACTION_STUNNED)) {
                    hp_down(&this->actor);
                    En_Rd_Actor_mode_paralyze_init(this);
                    return;
                }

                this->stunnedBySunsSong = false;
                this->sunsSongStunTimer = 0;

                if (this->damageEffect == REDEAD_DMGEFF_FIRE_MAGIC) {
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 80);
                    this->fireTimer = 40;
                } else {
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                }

                hp_down(&this->actor);
                if (this->actor.colChkInfo.health == 0) {
                    Rd_M_set(play, &this->actor, true);
                    En_Rd_Actor_mode_down_init(this);
                    Item_Set_Std(play, NULL, &this->actor.world.pos, 0x90);
                } else {
                    En_Rd_Actor_mode_damage_init(this);
                }
            }
        }
    }
}

void En_Rd_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnRd* this = (EnRd*)thisx;
    Player* player = GET_PLAYER(play);
    s32 pad2;

    En_Rd_damage_proc(this, play);

    if (z_common_data.sunsSongState != SUNSSONG_INACTIVE && !this->stunnedBySunsSong) {
        z_common_data.sunsSongState = SUNSSONG_INACTIVE;
    }

    if (this->damageEffect != REDEAD_DMGEFF_ICE_MAGIC &&
        ((this->action != REDEAD_ACTION_RISE_FROM_COFFIN) || (this->damageEffect != REDEAD_DMGEFF_FIRE_MAGIC))) {
        if (this->playerStunWaitTimer != 0) {
            this->playerStunWaitTimer--;
        }

        this->actionFunc(this, play);
        if (this->action != REDEAD_ACTION_GRAB && this->actor.speed != 0.0f) {
            Actor_position_moveF(&this->actor);
        }

        if ((this->actor.shape.rot.x == 0) && (this->action != REDEAD_ACTION_GRAB) && (this->actor.speed != 0.0f)) {
            Actor_BGcheck2(play, &this->actor, 30.0f, 20.0f, 35.0f,
                                    UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                        UPDBGCHECKINFO_FLAG_4);
        }

        if (this->action == REDEAD_ACTION_ATTEMPT_PLAYER_FREEZE) {
            En_Rd_Face_To_Player(this, play);
        }
    }

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 50.0f;

    if ((this->actor.colChkInfo.health > 0) && (this->action != REDEAD_ACTION_GRAB)) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        if ((this->action != REDEAD_ACTION_DAMAGED) || ((player->unk_844 != 0) && (player->unk_845 != this->unk_31D))) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

s32 en_rd_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnRd* this = (EnRd*)thisx;

    if (limbIndex == REDEAD_GIBDO_LIMB_HEAD_ROOT) {
        rot->y += this->headYRotation;
    } else if (limbIndex == REDEAD_GIBDO_LIMB_UPPER_BODY_ROOT) {
        rot->y += this->upperBodyYRotation;
    }

    return false;
}

void en_rd_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    Vec3f D_80AE4940 = { 300.0f, 0.0f, 0.0f };
    EnRd* this = (EnRd*)thisx;
    s32 idx = -1;
    Vec3f destPos;

    if ((this->fireTimer != 0) || ((this->actor.colorFilterTimer != 0) && (this->actor.colorFilterParams & 0x4000))) {
        switch (limbIndex) {
            case REDEAD_GIBDO_LIMB_HEAD:
                idx = 0;
                break;

            case REDEAD_GIBDO_LIMB_ROOT:
                idx = 1;
                break;

            case REDEAD_GIBDO_LIMB_RIGHT_HAND:
                idx = 2;
                break;

            case REDEAD_GIBDO_LIMB_LEFT_HAND:
                idx = 3;
                break;

            case REDEAD_GIBDO_LIMB_TORSO:
                idx = 4;
                break;

            case REDEAD_GIBDO_LIMB_PELVIS:
                idx = 5;
                break;

            case REDEAD_GIBDO_LIMB_RIGHT_SHIN:
                idx = 6;
                break;

            case REDEAD_GIBDO_LIMB_LEFT_SHIN:
                idx = 7;
                break;

            case REDEAD_GIBDO_LIMB_RIGHT_FOOT:
                idx = 8;
                break;

            case REDEAD_GIBDO_LIMB_LEFT_FOOT:
                idx = 9;
                break;
        }

        if (idx >= 0) {
            Matrix_Position(&D_80AE4940, &destPos);
            this->firePos[idx].x = destPos.x;
            this->firePos[idx].y = destPos.y;
            this->firePos[idx].z = destPos.z;
        }
    }
}

void En_Rd_display(Actor* thisx, PlayState* play) {
    static Vec3f local_foot = { 300.0f, 0.0f, 0.0f };
    static Vec3f shadow_scale = { 0.25f, 0.25f, 0.25f };
    s32 pad;
    EnRd* this = (EnRd*)thisx;
    Vec3f thisPos = thisx->world.pos;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_rd.c", 1679);

    if (this->alpha == 255) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, this->alpha);
        gSPSegment(POLY_OPA_DISP++, 8, &Actor_change_render_mode[2]);
        POLY_OPA_DISP =
            Si2_draw2_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               en_rd_display1, en_rd_display2, this, POLY_OPA_DISP);

        Shadow_draw(&thisPos, &shadow_scale, 255, play);

        if (this->fireTimer != 0) {
            thisx->colorFilterTimer++;
            this->fireTimer--;
            if (this->fireTimer % 4 == 0) {
                Effect_En_Fire_ct_s(play, thisx, &this->firePos[this->fireTimer >> 2], 0x4B, 0, 0,
                                          (this->fireTimer >> 2));
            }
        }
    } else {
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
        gSPSegment(POLY_XLU_DISP++, 8, &Actor_change_render_mode[0]);
        POLY_XLU_DISP =
            Si2_draw2_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               en_rd_display1, NULL, this, POLY_XLU_DISP);

        Shadow_draw(&thisPos, &shadow_scale, this->alpha, play);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_rd.c", 1735);
}
