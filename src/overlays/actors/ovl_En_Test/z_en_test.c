/*
 * File: z_en_test.c
 * Overlay: ovl_En_Test
 * Description: Stalfos
 */

#include "z_en_test.h"

#include "libc64/qrand.h"
#include "attributes.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64audio.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"

#include "global.h"
#include "assets/objects/object_sk2/object_sk2.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Test_Actor_ct(Actor* thisx, PlayState* play);
void En_Test_Actor_dt(Actor* thisx, PlayState* play);
void En_Test_move(Actor* thisx, PlayState* play);
void En_Test_display(Actor* thisx, PlayState* play);

void En_Test_Actor_mode_before_drop_init(EnTest* this);
void En_Test_Actor_mode_before_drop2_init(EnTest* this);
void En_Test_Actor_mode_back_jump_init(EnTest* this);
void En_Test_Actor_mode_kiru_end_init(EnTest* this);
void En_Test_Actor_mode_kiru2_init(EnTest* this);
void En_Test_Actor_mode_jump_attack_init(EnTest* this);
void En_Test_Actor_mode_forward_init(EnTest* this);
void En_Test_Actor_mode_move_to_player_front_init(EnTest* this);
void En_Test_Actor_mode_kiru_init(EnTest* this);
void En_Test_Actor_mode_search_rot_init(EnTest* this);
void En_Test_Actor_mode_defend_out_init(EnTest* this);
void En_Test_Actor_mode_hirumu_init(EnTest* this);
void En_Test_Actor_mode_back_damage_init(EnTest* this);
void En_Test_Actor_mode_front_damage_init(EnTest* this);
void En_Test_Actor_mode_defend_in_init(EnTest* this);
void En_Test_Actor_mode_side_step_init(EnTest* this, PlayState* play);

void En_Test_Actor_mode_before_drop(EnTest* this, PlayState* play);
void En_Test_Actor_mode_before_drop2(EnTest* this, PlayState* play);
void En_Test_Actor_mode_drop(EnTest* this, PlayState* play);
void En_Test_Actor_mode_landing(EnTest* this, PlayState* play);
void En_Test_Actor_mode_nobi(EnTest* this, PlayState* play);
void En_Test_Actor_mode_wait(EnTest* this, PlayState* play);
void En_Test_Actor_mode_forward(EnTest* this, PlayState* play);
void En_Test_Actor_mode_search_rot(EnTest* this, PlayState* play);
void En_Test_Actor_mode_move_to_player_front(EnTest* this, PlayState* play);
void En_Test_Actor_mode_kiru(EnTest* this, PlayState* play);
void En_Test_Actor_mode_kiru_end(EnTest* this, PlayState* play);
void En_Test_Actor_mode_kiru2(EnTest* this, PlayState* play);
void En_Test_Actor_mode_back_jump(EnTest* this, PlayState* play);
void En_Test_Actor_mode_jump_attack(EnTest* this, PlayState* play);
void En_Test_Actor_mode_jump_move(EnTest* this, PlayState* play);
void En_Test_Actor_mode_defend_in(EnTest* this, PlayState* play);
void En_Test_Actor_mode_defend_out(EnTest* this, PlayState* play);
void En_Test_Actor_mode_front_damage(EnTest* this, PlayState* play);
void En_Test_Actor_mode_back_damage(EnTest* this, PlayState* play);
void En_Test_Actor_mode_paralyze(EnTest* this, PlayState* play);
void En_Test_Actor_mode_side_step(EnTest* this, PlayState* play);
void En_Test_Actor_mode_rev(EnTest* this, PlayState* play);
void En_Test_Actor_mode_front_down(EnTest* this, PlayState* play);
void En_Test_Actor_mode_back_down(EnTest* this, PlayState* play);
void En_Test_Actor_mode_hirumu(EnTest* this, PlayState* play);
void En_Test_Actor_mode_break(EnTest* this, PlayState* play);
void En_Test_Actor_mode_front_down_init(EnTest* this, PlayState* play);

static s32 Shot_def(PlayState* play, EnTest* this);

static u8 skel_joint_status[] = {
    false, // STALFOS_LIMB_NONE
    false, // STALFOS_LIMB_ROOT
    false, // STALFOS_LIMB_UPPERBODY_ROOT
    false, // STALFOS_LIMB_CORE_LOWER_ROOT
    true,  // STALFOS_LIMB_CORE_UPPER_ROOT
    true,  // STALFOS_LIMB_NECK_ROOT
    true,  // STALFOS_LIMB_HEAD_ROOT
    true,  // STALFOS_LIMB_7
    true,  // STALFOS_LIMB_8
    true,  // STALFOS_LIMB_JAW_ROOT
    true,  // STALFOS_LIMB_JAW
    true,  // STALFOS_LIMB_HEAD
    true,  // STALFOS_LIMB_NECK_UPPER
    true,  // STALFOS_LIMB_NECK_LOWER
    true,  // STALFOS_LIMB_CORE_UPPER
    true,  // STALFOS_LIMB_CHEST
    true,  // STALFOS_LIMB_SHOULDER_R_ROOT
    true,  // STALFOS_LIMB_SHOULDER_ARMOR_R_ROOT
    true,  // STALFOS_LIMB_SHOULDER_ARMOR_R
    true,  // STALFOS_LIMB_SHOULDER_L_ROOT
    true,  // STALFOS_LIMB_SHOULDER_ARMOR_L_ROOT
    true,  // STALFOS_LIMB_SHOULDER_ARMOR_L
    true,  // STALFOS_LIMB_ARM_L_ROOT
    true,  // STALFOS_LIMB_UPPERARM_L_ROOT
    true,  // STALFOS_LIMB_FOREARM_L_ROOT
    true,  // STALFOS_LIMB_HAND_L_ROOT
    true,  // STALFOS_LIMB_HAND_L
    true,  // STALFOS_LIMB_SHIELD
    true,  // STALFOS_LIMB_FOREARM_L
    true,  // STALFOS_LIMB_UPPERARM_L
    true,  // STALFOS_LIMB_ARM_R_ROOT
    true,  // STALFOS_LIMB_UPPERARM_R_ROOT
    true,  // STALFOS_LIMB_FOREARM_R_ROOT
    true,  // STALFOS_LIMB_HAND_R_ROOT
    true,  // STALFOS_LIMB_SWORD
    true,  // STALFOS_LIMB_HAND_R
    true,  // STALFOS_LIMB_FOREARM_R
    true,  // STALFOS_LIMB_UPPERARM_R
    true,  // STALFOS_LIMB_CORE_LOWER
    false, // STALFOS_LIMB_LOWERBODY_ROOT
    false, // STALFOS_LIMB_WAIST_ROOT
    false, // STALFOS_LIMB_LEGS_ROOT
    false, // STALFOS_LIMB_LEG_L_ROOT
    false, // STALFOS_LIMB_THIGH_L_ROOT
    false, // STALFOS_LIMB_LOWERLEG_L_ROOT
    false, // STALFOS_LIMB_ANKLE_L_ROOT
    false, // STALFOS_LIMB_ANKLE_L
    false, // STALFOS_LIMB_FOOT_L_ROOT
    false, // STALFOS_LIMB_FOOT_L
    false, // STALFOS_LIMB_LOWERLEG_L
    false, // STALFOS_LIMB_THIGH_L
    false, // STALFOS_LIMB_LEG_R_ROOT
    false, // STALFOS_LIMB_THIGH_R_ROOT
    false, // STALFOS_LIMB_LOWERLEG_R_ROOT
    false, // STALFOS_LIMB_ANKLE_R_ROOT
    false, // STALFOS_LIMB_ANKLE_R
    false, // STALFOS_LIMB_FOOT_R_ROOT
    false, // STALFOS_LIMB_FOOT_R
    false, // STALFOS_LIMB_LOWERLEG_R
    false, // STALFOS_LIMB_THIGH_R
    false, // STALFOS_LIMB_WAIST
};

ActorProfile En_Test_Profile = {
    /**/ ACTOR_EN_TEST,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_SK2,
    /**/ sizeof(EnTest),
    /**/ En_Test_Actor_ct,
    /**/ En_Test_Actor_dt,
    /**/ En_Test_move,
    /**/ En_Test_display,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 25, 65, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit OcInfoData2 = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFC1FFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 20, 70, -50, { 0, 0, 0 } },
};

static ColliderQuadInit AtInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL | ATELEM_UNK7,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum StalfosDamageEffect {
    /* 0x0 */ STALFOS_DMGEFF_NORMAL,
    /* 0x1 */ STALFOS_DMGEFF_STUN,
    /* 0x6 */ STALFOS_DMGEFF_FIREMAGIC = 6,
    /* 0xD */ STALFOS_DMGEFF_SLING = 0xD,
    /* 0xE */ STALFOS_DMGEFF_LIGHT,
    /* 0xF */ STALFOS_DMGEFF_FREEZE
} StalfosDamageEffect;

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, STALFOS_DMGEFF_STUN),
    /* Deku stick    */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Slingshot     */ DMG_ENTRY(1, STALFOS_DMGEFF_SLING),
    /* Explosive     */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Boomerang     */ DMG_ENTRY(0, STALFOS_DMGEFF_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Hammer swing  */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Hookshot      */ DMG_ENTRY(0, STALFOS_DMGEFF_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, STALFOS_DMGEFF_NORMAL),
    /* Master sword  */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Giant's Knife */ DMG_ENTRY(4, STALFOS_DMGEFF_NORMAL),
    /* Fire arrow    */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Ice arrow     */ DMG_ENTRY(4, STALFOS_DMGEFF_FREEZE),
    /* Light arrow   */ DMG_ENTRY(2, STALFOS_DMGEFF_LIGHT),
    /* Unk arrow 1   */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Unk arrow 2   */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Unk arrow 3   */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Fire magic    */ DMG_ENTRY(0, STALFOS_DMGEFF_FIREMAGIC),
    /* Ice magic     */ DMG_ENTRY(3, STALFOS_DMGEFF_FREEZE),
    /* Light magic   */ DMG_ENTRY(0, STALFOS_DMGEFF_LIGHT),
    /* Shield        */ DMG_ENTRY(0, STALFOS_DMGEFF_NORMAL),
    /* Mirror Ray    */ DMG_ENTRY(0, STALFOS_DMGEFF_NORMAL),
    /* Kokiri spin   */ DMG_ENTRY(1, STALFOS_DMGEFF_NORMAL),
    /* Giant spin    */ DMG_ENTRY(4, STALFOS_DMGEFF_NORMAL),
    /* Master spin   */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Kokiri jump   */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Giant jump    */ DMG_ENTRY(8, STALFOS_DMGEFF_NORMAL),
    /* Master jump   */ DMG_ENTRY(4, STALFOS_DMGEFF_NORMAL),
    /* Unknown 1     */ DMG_ENTRY(0, STALFOS_DMGEFF_NORMAL),
    /* Unblockable   */ DMG_ENTRY(0, STALFOS_DMGEFF_NORMAL),
    /* Hammer jump   */ DMG_ENTRY(4, STALFOS_DMGEFF_NORMAL),
    /* Unknown 2     */ DMG_ENTRY(0, STALFOS_DMGEFF_NORMAL),
};

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_STALFOS, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 500, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 15, ICHAIN_CONTINUE),
    ICHAIN_F32(scale.y, 0, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1500, ICHAIN_STOP),
};

void En_Test_actor_set_process(EnTest* this, EnTestActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Test_Actor_ct(Actor* thisx, PlayState* play) {
    EffectBlureInit1 slashBlure;
    EnTest* this = (EnTest*)thisx;

    ValueSet_process(&this->actor, value_init);

    Skeleton_Info2_M_ct(play, &this->skelAnime, &gStalfosSkel, &gStalfosMiddleGuardAnim, this->jointTable, this->morphTable,
                   STALFOS_LIMB_MAX);
    Skeleton_Info2_M_ct(play, &this->upperSkelanime, &gStalfosSkel, &gStalfosMiddleGuardAnim, this->upperJointTable,
                   this->upperMorphTable, STALFOS_LIMB_MAX);

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_foot, 90.0f);

    this->actor.colChkInfo.cylRadius = 40;
    this->actor.colChkInfo.cylHeight = 100;
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 45.0f;
    this->actor.colChkInfo.damageTable = &btl_data;

    ClObjPipe_ct(play, &this->bodyCollider);
    ClObjPipe_set5(play, &this->bodyCollider, &this->actor, &OcInfoData);

    ClObjPipe_ct(play, &this->shieldCollider);
    ClObjPipe_set5(play, &this->shieldCollider, &this->actor, &OcInfoData2);

    ClObjSwrd_ct(play, &this->swordCollider);
    ClObjSwrd_set5(play, &this->swordCollider, &this->actor, &AtInfoData);

    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.colChkInfo.health = 10;

    slashBlure.p1StartColor[0] = slashBlure.p1StartColor[1] = slashBlure.p1StartColor[2] = slashBlure.p1StartColor[3] =
        slashBlure.p2StartColor[0] = slashBlure.p2StartColor[1] = slashBlure.p2StartColor[2] =
            slashBlure.p1EndColor[0] = slashBlure.p1EndColor[1] = slashBlure.p1EndColor[2] = slashBlure.p2EndColor[0] =
                slashBlure.p2EndColor[1] = slashBlure.p2EndColor[2] = 255;

    slashBlure.p1EndColor[3] = 0;
    slashBlure.p2EndColor[3] = 0;
    slashBlure.p2StartColor[3] = 64;

    slashBlure.elemDuration = 4;
    slashBlure.unkFlag = 0;
    slashBlure.calcMode = 2;

    EffectAdd(play, &this->effectIndex, EFFECT_BLURE1, 0, 0, &slashBlure);

    if (this->actor.params != STALFOS_TYPE_CEILING) {
        En_Test_Actor_mode_before_drop_init(this);
    } else {
        En_Test_Actor_mode_before_drop2_init(this);
    }

    if (this->actor.params == STALFOS_TYPE_INVISIBLE) {
        this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
    }
}

void En_Test_Actor_dt(Actor* thisx, PlayState* play) {
    EnTest* this = (EnTest*)thisx;

    if ((this->actor.params != STALFOS_TYPE_2) &&
        !ActorSearch(play, &this->actor, ACTOR_EN_TEST, ACTORCAT_ENEMY, 8000.0f)) {
        Na_StopMiddleBossBgm();
    }

    EffectFreeIndex(play, this->effectIndex);
    ClObjPipe_dt(play, &this->shieldCollider);
    ClObjPipe_dt(play, &this->bodyCollider);
    ClObjSwrd_dt(play, &this->swordCollider);
}

/**
 * If test_branch failed to pick a new action, this function will unconditionally pick
 * a new action as a last resort
 */
void branch0(EnTest* this, PlayState* play) {
    switch ((u32)(fqrand() * 10.0f)) {
        case 0:
        case 1:
        case 5:
        case 6:
            if ((this->actor.xzDistToPlayer < 220.0f) && (this->actor.xzDistToPlayer > 170.0f) &&
                Actor_player_direction_check(&this->actor, 0x71C) && Anc_Fight_My_Check(play, &this->actor)) {
                En_Test_Actor_mode_jump_attack_init(this);
                break;
            }
            FALLTHROUGH;
        case 8:
            En_Test_Actor_mode_forward_init(this);
            break;

        case 3:
        case 4:
        case 7:
            En_Test_Actor_mode_side_step_init(this, play);
            break;

        case 2:
        case 9:
        case 10:
            En_Test_Actor_mode_defend_in_init(this);
            break;
    }
}

void test_branch(EnTest* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s16 yawDiff = player->actor.shape.rot.y - this->actor.shape.rot.y;

    yawDiff = ABS(yawDiff);

    if (yawDiff >= 0x61A8) {
        switch ((u32)(fqrand() * 10.0f)) {
            case 0:
            case 3:
            case 7:
                En_Test_Actor_mode_defend_in_init(this);
                break;

            case 1:
            case 5:
            case 6:
            case 8:
                En_Test_Actor_mode_side_step_init(this, play);
                break;

            case 2:
            case 4:
            case 9:
                if (this->actor.params != STALFOS_TYPE_CEILING) {
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                    En_Test_Actor_mode_back_jump_init(this);
                }
                break;
        }
    } else if (yawDiff <= 0x3E80) {
        if (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) > 0x3E80) {
            if (((play->gameplayFrames % 2) != 0) && (this->actor.params != STALFOS_TYPE_CEILING)) {
                this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                En_Test_Actor_mode_back_jump_init(this);
            } else if ((this->actor.xzDistToPlayer < 220.0f) && (this->actor.xzDistToPlayer > 170.0f)) {
                if (Actor_player_direction_check(&this->actor, 0x71C) && !Anc_Fight_My_Check(play, &this->actor)) {
                    En_Test_Actor_mode_jump_attack_init(this);
                }
            } else {
                En_Test_Actor_mode_forward_init(this);
            }
        } else {
            if (this->actor.xzDistToPlayer < 110.0f) {
                if (fqrand() > 0.2f) {
                    if (player->stateFlags1 & PLAYER_STATE1_HOSTILE_LOCK_ON) {
                        if (this->actor.isLockedOn) {
                            En_Test_Actor_mode_kiru_init(this);
                        } else {
                            En_Test_Actor_mode_side_step_init(this, play);
                        }
                    } else {
                        En_Test_Actor_mode_kiru_init(this);
                    }
                }
            } else {
                branch0(this, play);
            }
        }
    } else {
        branch0(this, play);
    }
}

void En_Test_Actor_mode_before_drop_init(EnTest* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gStalfosMiddleGuardAnim);
    this->unk_7C8 = 0;
    this->timer = 15;
    this->actor.scale.y = 0.0f;
    this->actor.world.pos.y = this->actor.home.pos.y - 3.5f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    En_Test_actor_set_process(this, En_Test_Actor_mode_before_drop);
}

void En_Test_Actor_mode_before_drop(EnTest* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((this->timer == 0) && (ABS(this->actor.yDistToPlayer) < 150.0f)) {
        this->unk_7C8 = 3;
        En_Test_actor_set_process(this, En_Test_Actor_mode_nobi);
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        this->actor.shape.rot.y = this->actor.yawTowardsPlayer;

        if (this->actor.params != STALFOS_TYPE_2) {
            Na_StartMiddleBossBgm(NA_BGM_MINI_BOSS);
        }
    } else {
        if (this->timer != 0) {
            this->timer--;
        }

        this->actor.world.pos.y = this->actor.home.pos.y - 3.5f;
    }
}

void En_Test_Actor_mode_before_drop2_init(EnTest* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gStalfosMiddleGuardAnim);
    this->unk_7C8 = 0;
    this->actor.world.pos.y = this->actor.home.pos.y + 150.0f;
    Actor_set_scale(&this->actor, 0.0f);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    En_Test_actor_set_process(this, En_Test_Actor_mode_before_drop2);
}

void En_Test_Actor_mode_before_drop2(EnTest* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->actor.world.pos.y = this->actor.home.pos.y + 150.0f;

    if ((this->actor.xzDistToPlayer < 200.0f) && (ABS(this->actor.yDistToPlayer) < 450.0f)) {
        En_Test_actor_set_process(this, En_Test_Actor_mode_drop);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        Actor_set_scale(&this->actor, 0.015f);
    }
}

void En_Test_Actor_mode_wait_init(EnTest* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gStalfosMiddleGuardAnim);
    this->unk_7C8 = 0xA;
    this->timer = (fqrand() * 10.0f) + 5.0f;
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    En_Test_actor_set_process(this, En_Test_Actor_mode_wait);
}

void En_Test_Actor_mode_wait(EnTest* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (!Shot_def(play, this)) {
        yawDiff = player->actor.shape.rot.y - this->actor.shape.rot.y;

        if (this->actor.xzDistToPlayer < 100.0f) {
            if ((player->meleeWeaponState != 0) && (ABS(yawDiff) >= 0x1F40)) {
                this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;

                if (fqrand() > 0.7f && player->meleeWeaponAnimation != PLAYER_MWA_JUMPSLASH_START) {
                    En_Test_Actor_mode_back_jump_init(this);
                } else {
                    En_Test_Actor_mode_side_step_init(this, play);
                }
                return;
            }
        }

        if (this->timer != 0) {
            this->timer--;
        } else {
            if (Actor_player_direction_check(&this->actor, 0x1555)) {
                if ((this->actor.xzDistToPlayer < 220.0f) && (this->actor.xzDistToPlayer > 160.0f) &&
                    (fqrand() < 0.3f)) {
                    if (Anc_Fight_My_Check(play, &this->actor)) {
                        En_Test_Actor_mode_jump_attack_init(this);
                    } else {
                        En_Test_Actor_mode_side_step_init(this, play);
                    }
                } else {
                    if (fqrand() > 0.3f) {
                        En_Test_Actor_mode_forward_init(this);
                    } else {
                        En_Test_Actor_mode_side_step_init(this, play);
                    }
                }
            } else {
                if (fqrand() > 0.7f) {
                    En_Test_Actor_mode_search_rot_init(this);
                } else {
                    test_branch(this, play);
                }
            }
        }
    }
}

void En_Test_Actor_mode_drop(EnTest* this, PlayState* play) {
    Skeleton_Info2_init_standard_speedset_stop(&this->skelAnime, &gStalfosLandFromLeapAnim, 0.0f);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->actor.world.pos.y <= this->actor.floorHeight) {
        this->skelAnime.playSpeed = 1.0f;
        this->unk_7C8 = 0xC;
        this->timer = this->unk_7E4 * 0.15f;
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_DOWN);
        En_Test_actor_set_process(this, En_Test_Actor_mode_landing);
    }
}

void En_Test_Actor_mode_landing(EnTest* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        En_Test_Actor_mode_wait_init(this);
        this->timer = (fqrand() * 10.0f) + 5.0f;
    }
}

void En_Test_Actor_mode_forward_init(EnTest* this) {
    Skeleton_Info2_init(&this->upperSkelanime, &gStalfosBlockWithShieldAnim, 2.0f, 0.0f,
                     Si2_anime_end_frame(&gStalfosBlockWithShieldAnim), 2, 2.0f);
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gStalfosSlowAdvanceAnim);
    this->timer = (s16)(fqrand() * 5.0f);
    this->unk_7C8 = 0xD;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    En_Test_actor_set_process(this, En_Test_Actor_mode_forward);
}

void En_Test_Actor_mode_forward(EnTest* this, PlayState* play) {
    s32 pad;
    f32 checkDist = 0.0f;
    s32 absPlaySpeed;
    s32 prevFrame;
    s32 beforeCurFrame;
    f32 playSpeed;
    Player* player = GET_PLAYER(play);
    s16 yawDiff;

    if (!Shot_def(play, this)) {
        this->timer++;

        if (Anc_Fight_ham_Check(play, &this->actor)) {
            checkDist = 150.0f;
        }

        if (this->actor.xzDistToPlayer <= (80.0f + checkDist)) {
            add_calc(&this->actor.speed, -5.0f, 1.0f, 0.8f, 0.0f);
        } else if (this->actor.xzDistToPlayer > (110.0f + checkDist)) {
            add_calc(&this->actor.speed, 5.0f, 1.0f, 0.8f, 0.0f);
        }

        if (this->actor.speed >= 5.0f) {
            this->actor.speed = 5.0f;
        } else if (this->actor.speed < -5.0f) {
            this->actor.speed = -5.0f;
        }

        if ((this->actor.params == STALFOS_TYPE_CEILING) &&
            !BG_point_check(&this->actor, play, this->actor.speed, this->actor.world.rot.y)) {
            this->actor.speed *= -1.0f;
        }

        if (ABS(this->actor.speed) < 3.0f) {
            s32 pad;

            Skeleton_Info2_init(&this->skelAnime, &gStalfosSlowAdvanceAnim, 0.0f, this->skelAnime.curFrame,
                             Si2_anime_end_frame(&gStalfosSlowAdvanceAnim), 0, -6.0f);
            playSpeed = this->actor.speed * 10.0f;
        } else {
            Skeleton_Info2_init(&this->skelAnime, &gStalfosFastAdvanceAnim, 0.0f, this->skelAnime.curFrame,
                             Si2_anime_end_frame(&gStalfosFastAdvanceAnim), 0, -4.0f);
            playSpeed = this->actor.speed * 10.0f * 0.02f;
        }

        if (this->actor.speed >= 0.0f) {
            if (this->unk_7DE == 0) {
                this->unk_7DE++;
            }

            playSpeed = CLAMP_MAX(playSpeed, 2.5f);
            this->skelAnime.playSpeed = playSpeed;
        } else {
            playSpeed = CLAMP_MIN(playSpeed, -2.5f);
            this->skelAnime.playSpeed = playSpeed;
        }

        yawDiff = player->actor.shape.rot.y - this->actor.shape.rot.y;

        if ((this->actor.xzDistToPlayer < 100.0f) && (player->meleeWeaponState != 0)) {
            if (ABS(yawDiff) >= 0x1F40) {
                this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;

                if ((fqrand() > 0.7f) && (player->meleeWeaponAnimation != PLAYER_MWA_JUMPSLASH_START)) {
                    En_Test_Actor_mode_back_jump_init(this);
                } else {
                    En_Test_Actor_mode_defend_in_init(this);
                }

                return;
            }
        }

        prevFrame = (s32)this->skelAnime.curFrame;
        Skeleton_Info2_anime_play(&this->skelAnime);
        beforeCurFrame = (s32)(this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed));
        absPlaySpeed = (s32)(f32)ABS(this->skelAnime.playSpeed);

        if ((s32)this->skelAnime.curFrame != prevFrame) {
            s32 afterPrevFrame = absPlaySpeed + prevFrame;

            if (((afterPrevFrame > 1) && (beforeCurFrame < 1)) || ((beforeCurFrame < 7) && (afterPrevFrame > 7))) {
                Actor_SE_set(&this->actor, NA_SE_EN_STAL_WALK);
            }
        }

        if ((this->timer % 32) == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_STAL_WARAU);
            this->timer += (s16)(fqrand() * 5.0f);
        }

        if ((this->actor.xzDistToPlayer < 220.0f) && (this->actor.xzDistToPlayer > 160.0f) &&
            (Actor_player_direction_check(&this->actor, 0x71C))) {
            if (Anc_Fight_My_Check(play, &this->actor)) {
                if (fqrand() < 0.1f) {
                    En_Test_Actor_mode_jump_attack_init(this);
                    return;
                }
            } else if (player->heldItemAction != PLAYER_IA_NONE) {
                if (this->actor.isLockedOn) {
                    if ((play->gameplayFrames % 2) != 0) {
                        En_Test_Actor_mode_side_step_init(this, play);
                        return;
                    }

                    test_branch(this, play);
                } else {
                    En_Test_Actor_mode_move_to_player_front_init(this);
                }
            }
        }

        if (fqrand() < 0.4f) {
            this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        }

        if (!Actor_player_direction_check(&this->actor, 0x11C7)) {
            En_Test_Actor_mode_wait_init(this);
            this->timer = (fqrand() * 10.0f) + 10.0f;
            return;
        }

        if (this->actor.xzDistToPlayer < 110.0f) {
            if (fqrand() > 0.2f) {
                if (player->stateFlags1 & PLAYER_STATE1_HOSTILE_LOCK_ON) {
                    if (this->actor.isLockedOn) {
                        En_Test_Actor_mode_kiru_init(this);
                    } else {
                        En_Test_Actor_mode_side_step_init(this, play);
                    }
                } else {
                    En_Test_Actor_mode_kiru_init(this);
                }
            } else {
                En_Test_Actor_mode_defend_in_init(this);
            }
        } else if (fqrand() < 0.1f) {
            this->actor.speed = 5.0f;
        }
    }
}

// a variation of sidestep
void En_Test_Actor_mode_search_rot_init(EnTest* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gStalfosSidestepAnim);
    this->unk_7C8 = 0xE;
    En_Test_actor_set_process(this, En_Test_Actor_mode_search_rot);
}

// a variation of sidestep
void En_Test_Actor_mode_search_rot(EnTest* this, PlayState* play) {
    s16 yawDiff;
    s16 yawChange;
    f32 playSpeed;
    s32 prevFrame;
    s32 beforeCurFrame;
    s32 afterPrevFrame;
    s32 absPlaySpeed;

    if (!Shot_def(play, this)) {
        yawDiff = this->actor.yawTowardsPlayer;
        yawDiff -= this->actor.shape.rot.y;

        if (yawDiff > 0) {
            yawChange = (yawDiff / 42.0f) + 300.0f;
            this->actor.shape.rot.y += yawChange * 2;
        } else {
            yawChange = (yawDiff / 42.0f) - 300.0f;
            this->actor.shape.rot.y += yawChange * 2;
        }

        this->actor.world.rot.y = this->actor.shape.rot.y;

        if (yawDiff > 0) {
            playSpeed = yawChange * 0.02f;
            playSpeed = CLAMP_MAX(playSpeed, 1.0f);
            this->skelAnime.playSpeed = playSpeed;
        } else {
            playSpeed = yawChange * 0.02f;
            playSpeed = CLAMP_MIN(playSpeed, -1.0f);
            this->skelAnime.playSpeed = playSpeed;
        }

        prevFrame = (s32)this->skelAnime.curFrame;
        Skeleton_Info2_anime_play(&this->skelAnime);
        beforeCurFrame = (s32)(this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed));
        absPlaySpeed = (s32)(f32)ABS(this->skelAnime.playSpeed);

        if ((s32)this->skelAnime.curFrame != prevFrame) {
            afterPrevFrame = absPlaySpeed + prevFrame;

            if (((afterPrevFrame > 2) && (beforeCurFrame <= 0)) || ((beforeCurFrame < 7) && (afterPrevFrame >= 9))) {
                Actor_SE_set(&this->actor, NA_SE_EN_STAL_WALK);
            }
        }

        if (Actor_player_direction_check(&this->actor, 0x71C)) {
            if (fqrand() > 0.8f) {
                if ((fqrand() > 0.7f)) {
                    En_Test_Actor_mode_move_to_player_front_init(this);
                } else {
                    test_branch(this, play);
                }
            } else {
                En_Test_Actor_mode_forward_init(this);
            }
        }
    }
}

// a variation of sidestep
void En_Test_Actor_mode_move_to_player_front_init(EnTest* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gStalfosSidestepAnim);
    this->unk_7C8 = 0xF;
    this->actor.speed = (fqrand() > 0.5f) ? -0.5f : 0.5f;
    this->timer = (s16)((fqrand() * 15.0f) + 25.0f);
    this->unk_7EC = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    En_Test_actor_set_process(this, En_Test_Actor_mode_move_to_player_front);
}

// a variation of sidestep
void En_Test_Actor_mode_move_to_player_front(EnTest* this, PlayState* play) {
    s16 playerYaw180;
    s32 pad;
    s32 prevFrame;
    s32 beforeCurFrame;
    s16 yawDiff;
    Player* player = GET_PLAYER(play);
    f32 checkDist = 0.0f;
    s16 newYaw;
    s32 absPlaySpeed;

    if (!Shot_def(play, this)) {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA0, 1);
        this->actor.world.rot.y = this->actor.shape.rot.y + 0x3E80;
        playerYaw180 = player->actor.shape.rot.y + 0x8000;

        if (this->actor.speed >= 0.0f) {
            if (this->actor.speed < 6.0f) {
                this->actor.speed += 0.5f;
            } else {
                this->actor.speed = 6.0f;
            }
        } else {
            if (this->actor.speed > -6.0f) {
                this->actor.speed -= 0.5f;
            } else {
                this->actor.speed = -6.0f;
            }
        }

        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) ||
            ((this->actor.params == STALFOS_TYPE_CEILING) &&
             !BG_point_check(&this->actor, play, this->actor.speed, this->actor.world.rot.y))) {
            if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                if (this->actor.speed >= 0.0f) {
                    newYaw = this->actor.shape.rot.y + 0x3FFF;
                } else {
                    newYaw = this->actor.shape.rot.y - 0x3FFF;
                }

                newYaw = this->actor.wallYaw - newYaw;
            } else {
                this->actor.speed *= -0.8f;
                newYaw = 0;
            }

            if (ABS(newYaw) > 0x4000) {
                this->actor.speed *= -0.8f;

                if (this->actor.speed < 0.0f) {
                    this->actor.speed -= 0.5f;
                } else {
                    this->actor.speed += 0.5f;
                }
            }
        }

        if (Anc_Fight_ham_Check(play, &this->actor)) {
            checkDist = 200.0f;
        }

        if (this->actor.xzDistToPlayer <= (80.0f + checkDist)) {
            add_calc(&this->unk_7EC, -2.5f, 1.0f, 0.8f, 0.0f);
        } else if (this->actor.xzDistToPlayer > (110.0f + checkDist)) {
            add_calc(&this->unk_7EC, 2.5f, 1.0f, 0.8f, 0.0f);
        } else {
            add_calc(&this->unk_7EC, 0.0f, 1.0f, 6.65f, 0.0f);
        }

        if (this->unk_7EC != 0.0f) {
            this->actor.world.pos.x += sin_s(this->actor.shape.rot.y) * this->unk_7EC;
            this->actor.world.pos.z += cos_s(this->actor.shape.rot.y) * this->unk_7EC;
        }

        this->skelAnime.playSpeed = this->actor.speed * 0.5f;

        prevFrame = (s32)this->skelAnime.curFrame;
        Skeleton_Info2_anime_play(&this->skelAnime);
        beforeCurFrame = (s32)(this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed));
        absPlaySpeed = (s32)(f32)ABS(this->skelAnime.playSpeed);

        if ((s32)this->skelAnime.curFrame != prevFrame) {
            s32 afterPrevFrame = absPlaySpeed + prevFrame;

            if (((afterPrevFrame > 1) && (beforeCurFrame < 1)) || ((beforeCurFrame < 7) && (afterPrevFrame > 7))) {
                Actor_SE_set(&this->actor, NA_SE_EN_STAL_WALK);
            }
        }

        if ((play->gameplayFrames & 95) == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_STAL_WARAU);
        }

        yawDiff = playerYaw180 - this->actor.shape.rot.y;
        yawDiff = ABS(yawDiff);

        if ((yawDiff > 0x6800) || (this->timer == 0)) {
            test_branch(this, play);
        } else if (this->timer != 0) {
            this->timer--;
        }
    }
}

void En_Test_Actor_mode_kiru_init(EnTest* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gStalfosDownSlashAnim);
    Nai_StopObjFlagFx(&this->actor.projectedPos, NA_SE_EN_STAL_WARAU);
    this->swordCollider.base.atFlags &= ~AT_BOUNCED;
    this->unk_7C8 = 0x10;
    this->actor.speed = 0.0f;
    En_Test_actor_set_process(this, En_Test_Actor_mode_kiru);
    this->swordCollider.elem.atDmgInfo.damage = 16;

    if (this->unk_7DE != 0) {
        this->unk_7DE = 3;
    }
}

void En_Test_Actor_mode_kiru(EnTest* this, PlayState* play) {
    this->actor.speed = 0.0f;

    if ((s32)this->skelAnime.curFrame < 4) {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 0);
    }

    if ((s32)this->skelAnime.curFrame == 7) {
        Actor_SE_set(&this->actor, NA_SE_EN_STAL_SAKEBI);
    }

    if ((this->skelAnime.curFrame > 7.0f) && (this->skelAnime.curFrame < 11.0f)) {
        this->swordState = 1;
    } else {
        this->swordState = 0;
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if ((play->gameplayFrames % 2) != 0) {
            En_Test_Actor_mode_kiru_end_init(this);
        } else {
            En_Test_Actor_mode_kiru2_init(this);
        }
    }
}

void En_Test_Actor_mode_kiru_end_init(EnTest* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gStalfosRecoverFromDownSlashAnim);
    this->unk_7C8 = 0x12;
    this->actor.speed = 0.0f;
    En_Test_actor_set_process(this, En_Test_Actor_mode_kiru_end);
}

void En_Test_Actor_mode_kiru_end(EnTest* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff;

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->swordCollider.base.atFlags & AT_HIT) {
            this->swordCollider.base.atFlags &= ~AT_HIT;
            if (this->actor.params != STALFOS_TYPE_CEILING) {
                En_Test_Actor_mode_back_jump_init(this);
                return;
            }
        }

        if (fqrand() > 0.7f) {
            En_Test_Actor_mode_wait_init(this);
            this->timer = (fqrand() * 5.0f) + 5.0f;
            return;
        }

        this->actor.world.rot.y = Actor_search_actor_angleY(&this->actor, &player->actor);

        if (fqrand() > 0.7f) {
            if (this->actor.params != STALFOS_TYPE_CEILING) {
                En_Test_Actor_mode_back_jump_init(this);
                return;
            }
        }

        yawDiff = player->actor.shape.rot.y - this->actor.shape.rot.y;

        if (ABS(yawDiff) <= 0x2710) {
            yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

            if ((ABS(yawDiff) > 0x3E80) && (this->actor.params != STALFOS_TYPE_CEILING)) {
                this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                En_Test_Actor_mode_back_jump_init(this);
            } else if (player->stateFlags1 & PLAYER_STATE1_HOSTILE_LOCK_ON) {
                if (this->actor.isLockedOn) {
                    En_Test_Actor_mode_kiru_init(this);
                } else if ((play->gameplayFrames % 2) != 0) {
                    En_Test_Actor_mode_side_step_init(this, play);
                } else {
                    En_Test_Actor_mode_back_jump_init(this);
                }
            } else {
                En_Test_Actor_mode_kiru_init(this);
            }
        } else {
            En_Test_Actor_mode_side_step_init(this, play);
        }
    }
}

void En_Test_Actor_mode_kiru2_init(EnTest* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gStalfosUpSlashAnim);
    this->swordCollider.base.atFlags &= ~AT_BOUNCED;
    this->unk_7C8 = 0x11;
    this->swordCollider.elem.atDmgInfo.damage = 16;
    this->actor.speed = 0.0f;
    En_Test_actor_set_process(this, En_Test_Actor_mode_kiru2);

    if (this->unk_7DE != 0) {
        this->unk_7DE = 3;
    }
}

void En_Test_Actor_mode_kiru2(EnTest* this, PlayState* play) {
    this->actor.speed = 0.0f;

    if ((s32)this->skelAnime.curFrame == 2) {
        Actor_SE_set(&this->actor, NA_SE_EN_STAL_SAKEBI);
    }

    if ((this->skelAnime.curFrame > 1.0f) && (this->skelAnime.curFrame < 8.0f)) {
        this->swordState = 1;
    } else {
        this->swordState = 0;
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        En_Test_Actor_mode_kiru_init(this);
    }
}

void En_Test_Actor_mode_back_jump_init(EnTest* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gStalfosJumpBackwardsAnim);
    Actor_SE_set(&this->actor, NA_SE_EN_STAL_JUMP);
    this->unk_7C8 = 0x14;
    this->timer = 5;
    En_Test_actor_set_process(this, En_Test_Actor_mode_back_jump);

    if (this->unk_7DE != 0) {
        this->unk_7DE = 3;
    }

    if (this->actor.params != STALFOS_TYPE_CEILING) {
        this->actor.speed = -11.0f;
    } else {
        this->actor.speed = -7.0f;
    }
}

void En_Test_Actor_mode_back_jump(EnTest* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 1);

    if (this->timer == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_STAL_WARAU);
    } else {
        this->timer--;
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (!Shot_def(play, this)) {
            if (this->actor.xzDistToPlayer <= 100.0f) {
                if (Actor_player_direction_check(&this->actor, 0x1555)) {
                    En_Test_Actor_mode_kiru_init(this);
                } else {
                    En_Test_Actor_mode_wait_init(this);
                    this->timer = (fqrand() * 5.0f) + 5.0f;
                }
            } else {
                if ((this->actor.xzDistToPlayer <= 220.0f) && Actor_player_direction_check(&this->actor, 0xE38)) {
                    En_Test_Actor_mode_jump_attack_init(this);
                } else {
                    En_Test_Actor_mode_wait_init(this);
                    this->timer = (fqrand() * 5.0f) + 5.0f;
                }
            }
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        }
    } else if (this->skelAnime.curFrame == (this->skelAnime.endFrame - 4.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
    }
}

void En_Test_Actor_mode_jump_attack_init(EnTest* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gStalfosJumpAnim);
    Nai_StopObjFlagFx(&this->actor.projectedPos, NA_SE_EN_STAL_WARAU);
    this->timer = 0;
    this->unk_7C8 = 0x17;
    this->actor.velocity.y = 10.0f;
    this->actor.speed = 8.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_STAL_JUMP);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->swordCollider.base.atFlags &= ~AT_BOUNCED;
    En_Test_actor_set_process(this, En_Test_Actor_mode_jump_attack);
    this->swordCollider.elem.atDmgInfo.damage = 32;

    if (this->unk_7DE != 0) {
        this->unk_7DE = 3;
    }
}

void En_Test_Actor_mode_jump_attack(EnTest* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->timer == 0) {
            Skeleton_Info2_init_standard_stop(&this->skelAnime, &gStalfosJumpslashAnim);
            this->timer = 1;
            this->swordState = 1;
            Actor_SE_set(&this->actor, NA_SE_EN_STAL_SAKEBI);
            Actor_SE_set(&this->actor, NA_SE_EN_STAL_JUMP);
        } else {
            this->actor.speed = 0.0f;
            En_Test_Actor_mode_wait_init(this);
        }
    }

    if ((this->timer != 0) && (this->skelAnime.curFrame >= 5.0f)) {
        this->swordState = 0;
    }

    if (this->actor.world.pos.y <= this->actor.floorHeight) {
        if (this->actor.speed != 0.0f) {
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
        }

        this->actor.world.pos.y = this->actor.floorHeight;
        this->actor.velocity.y = 0.0f;
        this->actor.speed = 0.0f;
    }
}

void En_Test_Actor_mode_jump_move_init(EnTest* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gStalfosJumpAnim);
    this->timer = 0;
    this->unk_7C8 = 4;
    this->actor.velocity.y = 14.0f;
    this->actor.speed = 6.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_STAL_JUMP);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    En_Test_actor_set_process(this, En_Test_Actor_mode_jump_move);
}

void En_Test_Actor_mode_jump_move(EnTest* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA0, 1);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->actor.world.pos.y <= this->actor.floorHeight) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
        this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
        this->actor.world.pos.y = this->actor.floorHeight;
        this->unk_7E4 = -(s32)this->actor.velocity.y;

        if (this->unk_7E4 == 0) {
            this->unk_7E4 = 1;
        }

        this->actor.velocity.y = 0.0f;
        this->actor.speed = 0.0f;
        this->unk_7C8 = 0xC;
        this->timer = 4;
        Skeleton_Info2_init(&this->skelAnime, &gStalfosLandFromLeapAnim, 0.0f, 0.0f, 0.0f, 2, 0.0f);
        En_Test_actor_set_process(this, En_Test_Actor_mode_landing);
    }
}

void En_Test_Actor_mode_defend_in_init(EnTest* this) {
    Skeleton_Info2_init(&this->skelAnime, &gStalfosBlockWithShieldAnim, 2.0f, 0.0f,
                     Si2_anime_end_frame(&gStalfosBlockWithShieldAnim), 2, 2.0f);
    this->unk_7C8 = 0x15;
    this->actor.speed = 0.0f;
    this->timer = (fqrand() * 10.0f) + 11.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->unk_7DE = 5;
    En_Test_actor_set_process(this, En_Test_Actor_mode_defend_in);
}

void En_Test_Actor_mode_defend_in(EnTest* this, PlayState* play) {
    add_calc(&this->actor.speed, 0.0f, 1.0f, 0.5f, 0.0f);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) > 0x3E80) &&
        (this->actor.params != STALFOS_TYPE_CEILING) && ((play->gameplayFrames % 2) != 0)) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        En_Test_Actor_mode_back_jump_init(this);
    }

    if (this->timer == 0) {
        En_Test_Actor_mode_defend_out_init(this);
    } else {
        this->timer--;
    }
}

void En_Test_Actor_mode_defend_out_init(EnTest* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gStalfosMiddleGuardAnim, -4.0f);
    this->unk_7C8 = 0x16;
    En_Test_actor_set_process(this, En_Test_Actor_mode_defend_out);
}

void En_Test_Actor_mode_defend_out(EnTest* this, PlayState* play) {
    add_calc(&this->actor.speed, 0.0f, 1.0f, 1.5f, 0.0f);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->skelAnime.morphWeight == 0.0f) {
        this->actor.speed = 0.0f;
        this->unk_7DE = 0;

        if (!Shot_def(play, this)) {
            if (this->actor.xzDistToPlayer < 500.0f) {
                test_branch(this, play);
            } else {
                En_Test_Actor_mode_side_step_init(this, play);
            }
        }
    }
}

void En_Test_Actor_mode_front_damage_init(EnTest* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gStalfosFlinchFromHitFrontAnim);
    Actor_SE_set(&this->actor, NA_SE_EN_STAL_DAMAGE);
    this->unk_7C8 = 8;
    this->actor.speed = -2.0f;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
    En_Test_actor_set_process(this, En_Test_Actor_mode_front_damage);
}

void En_Test_Actor_mode_front_damage(EnTest* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    add_calc(&this->actor.speed, 0.0f, 1.0f, 0.1f, 0.0f);

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->actor.speed = 0.0f;

        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) &&
            ((ABS((s16)(this->actor.wallYaw - this->actor.shape.rot.y)) < 0x38A4) &&
             (this->actor.xzDistToPlayer < 80.0f))) {
            En_Test_Actor_mode_jump_move_init(this);
        } else if (!Shot_def(play, this)) {
            test_branch(this, play);
        } else {
            return;
        }
    }

    if (player->meleeWeaponState != 0) {
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) &&
            ((ABS((s16)(this->actor.wallYaw - this->actor.shape.rot.y)) < 0x38A4) &&
             (this->actor.xzDistToPlayer < 80.0f))) {
            En_Test_Actor_mode_jump_move_init(this);
        } else if ((fqrand() > 0.7f) && (this->actor.params != STALFOS_TYPE_CEILING) &&
                   (player->meleeWeaponAnimation != PLAYER_MWA_JUMPSLASH_START)) {
            En_Test_Actor_mode_back_jump_init(this);
        } else {
            En_Test_Actor_mode_defend_in_init(this);
        }

        this->unk_7C8 = 8;
    }
}

void En_Test_Actor_mode_back_damage_init(EnTest* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gStalfosFlinchFromHitBehindAnim);
    Actor_SE_set(&this->actor, NA_SE_EN_STAL_DAMAGE);
    this->unk_7C8 = 9;
    this->actor.speed = -2.0f;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
    En_Test_actor_set_process(this, En_Test_Actor_mode_back_damage);
}

void En_Test_Actor_mode_back_damage(EnTest* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    add_calc(&this->actor.speed, 0.0f, 1.0f, 0.1f, 0.0f);

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->actor.speed = 0.0f;

        if (!Shot_def(play, this)) {
            test_branch(this, play);
        } else {
            return;
        }
    }

    if (player->meleeWeaponState != 0) {
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) &&
            ((ABS((s16)(this->actor.wallYaw - this->actor.shape.rot.y)) < 0x38A4) &&
             (this->actor.xzDistToPlayer < 80.0f))) {
            En_Test_Actor_mode_jump_move_init(this);
        } else if ((fqrand() > 0.7f) && (this->actor.params != STALFOS_TYPE_CEILING) &&
                   (player->meleeWeaponAnimation != PLAYER_MWA_JUMPSLASH_START)) {
            En_Test_Actor_mode_back_jump_init(this);
        } else {
            En_Test_Actor_mode_defend_in_init(this);
        }

        this->unk_7C8 = 8;
    }
}

void En_Test_Actor_mode_paralyze_init(EnTest* this) {
    this->unk_7C8 = 0xB;
    this->unk_7DE = 0;
    this->swordState = 0;
    this->skelAnime.playSpeed = 0.0f;
    this->actor.speed = -4.0f;

    if (this->lastDamageEffect == STALFOS_DMGEFF_LIGHT) {
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_GRAY, 120, COLORFILTER_BUFFLAG_OPA, 80);
    } else {
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 80);

        if (this->lastDamageEffect == STALFOS_DMGEFF_FREEZE) {
            this->iceTimer = 36;
        } else {
            Skeleton_Info2_init_standard_speedset_stop(&this->skelAnime, &gStalfosFlinchFromHitFrontAnim, 0.0f);
        }
    }

    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    En_Test_actor_set_process(this, En_Test_Actor_mode_paralyze);
}

void En_Test_Actor_mode_paralyze(EnTest* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    add_calc(&this->actor.speed, 0.0f, 1.0f, 1.0f, 0.0f);

    if (this->actor.colorFilterTimer == 0) {
        if (this->actor.colChkInfo.health == 0) {
            En_Test_Actor_mode_front_down_init(this, play);
        } else if (player->meleeWeaponState != 0) {
            if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) &&
                ((ABS((s16)(this->actor.wallYaw - this->actor.shape.rot.y)) < 0x38A4) &&
                 (this->actor.xzDistToPlayer < 80.0f))) {
                En_Test_Actor_mode_jump_move_init(this);
            } else if ((fqrand() > 0.7f) && (player->meleeWeaponAnimation != PLAYER_MWA_JUMPSLASH_START)) {
                En_Test_Actor_mode_back_jump_init(this);
            } else {
                En_Test_Actor_mode_defend_in_init(this);
            }

            this->unk_7C8 = 8;
        } else {
            this->actor.speed = 0.0f;
            if (!Shot_def(play, this)) {
                test_branch(this, play);
            }
        }
    }
}

// a variation of sidestep
void En_Test_Actor_mode_side_step_init(EnTest* this, PlayState* play) {
    if (Anc_Fight_ham_Check(play, &this->actor)) {
        En_Test_Actor_mode_move_to_player_front_init(this);
        return;
    }

    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gStalfosSidestepAnim, -2.0f);
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA0, 1);
    this->actor.speed = ((play->gameplayFrames % 2) != 0) ? -4.0f : 4.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF;
    this->timer = (fqrand() * 20.0f) + 20.0f;
    this->unk_7C8 = 0x18;
    En_Test_actor_set_process(this, En_Test_Actor_mode_side_step);
    this->unk_7EC = 0.0f;
}

// a variation of sidestep
void En_Test_Actor_mode_side_step(EnTest* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 pad1;
    s32 prevFrame;
    s32 beforeCurFrame;
    s32 pad2;
    f32 checkDist = 0.0f;
    s16 newYaw;
    f32 absPlaySpeed;

    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA0, 1);

    if (this->unk_7DE == 0) {
        this->unk_7DE++;
    }

    if (this->actor.speed >= 0.0f) {
        if (this->actor.speed < 6.0f) {
            this->actor.speed += 0.125f;
        } else {
            this->actor.speed = 6.0f;
        }
    } else {
        if (this->actor.speed > -6.0f) {
            this->actor.speed -= 0.125f;
        } else {
            this->actor.speed = -6.0f;
        }
    }

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) ||
        ((this->actor.params == STALFOS_TYPE_CEILING) &&
         !BG_point_check(&this->actor, play, this->actor.speed, this->actor.shape.rot.y + 0x3FFF))) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            if (this->actor.speed >= 0.0f) {
                newYaw = (this->actor.shape.rot.y + 0x3FFF);
            } else {
                newYaw = (this->actor.shape.rot.y - 0x3FFF);
            }

            newYaw = this->actor.wallYaw - newYaw;
        } else {
            this->actor.speed *= -0.8f;
            newYaw = 0;
        }

        if (ABS(newYaw) > 0x4000) {
            this->actor.speed *= -0.8f;

            if (this->actor.speed < 0.0f) {
                this->actor.speed -= 0.5f;
            } else {
                this->actor.speed += 0.5f;
            }
        }
    }

    this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF;

    if (Anc_Fight_ham_Check(play, &this->actor)) {
        checkDist = 200.0f;
    }

    if (this->actor.xzDistToPlayer <= (80.0f + checkDist)) {
        add_calc(&this->unk_7EC, -2.5f, 1.0f, 0.8f, 0.0f);
    } else if (this->actor.xzDistToPlayer > (110.0f + checkDist)) {
        add_calc(&this->unk_7EC, 2.5f, 1.0f, 0.8f, 0.0f);
    } else {
        add_calc(&this->unk_7EC, 0.0f, 1.0f, 6.65f, 0.0f);
    }

    if (this->unk_7EC != 0.0f) {
        this->actor.world.pos.x += (sin_s(this->actor.shape.rot.y) * this->unk_7EC);
        this->actor.world.pos.z += (cos_s(this->actor.shape.rot.y) * this->unk_7EC);
    }

    this->skelAnime.playSpeed = this->actor.speed * 0.5f;

    prevFrame = (s32)this->skelAnime.curFrame;
    Skeleton_Info2_anime_play(&this->skelAnime);
    beforeCurFrame = (s32)(this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed));
    absPlaySpeed = ABS(this->skelAnime.playSpeed);

    if ((this->timer % 32) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_STAL_WARAU);
    }
    if ((s32)this->skelAnime.curFrame != prevFrame) {
        s32 afterPrevFrame = (s32)absPlaySpeed + prevFrame;

        if (((afterPrevFrame > 1) && (beforeCurFrame < 1)) || ((beforeCurFrame < 7) && (afterPrevFrame > 7))) {
            Actor_SE_set(&this->actor, NA_SE_EN_STAL_WALK);
        }
    }

    if (this->timer == 0) {
        if (Anc_Fight_ham_Check(play, &this->actor)) {
            En_Test_Actor_mode_wait_init(this);
        } else if (Anc_Fight_My_Check(play, &this->actor)) {
            if (!Shot_def(play, this)) {
                test_branch(this, play);
            }
        } else if (player->heldItemAction != PLAYER_IA_NONE) {
            if ((play->gameplayFrames % 2) != 0) {
                En_Test_Actor_mode_wait_init(this);
            } else {
                En_Test_Actor_mode_forward_init(this);
            }
        } else {
            En_Test_Actor_mode_forward_init(this);
        }

    } else {
        this->timer--;
    }
}

void En_Test_Actor_mode_rev_init(EnTest* this, PlayState* play) {
    Actor_SE_set(&this->actor, NA_SE_EN_STAL_DAMAGE);
    this->unk_7C8 = 2;
    Part_Break_init(&this->bodyBreak, 60, play);
    this->actor.home.rot.x = 0;

    if (this->swordState >= 0) {
        EffectBlure_space_add(Effect_GetEffectMemoryPointer(this->effectIndex));
        this->swordState = -1;
    }

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    if (this->actor.params == STALFOS_TYPE_5) {
        Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
    }

    En_Test_actor_set_process(this, En_Test_Actor_mode_rev);
}

void En_Test_Actor_mode_rev(EnTest* this, PlayState* play) {
    if (this->actor.child == NULL) {
        if (this->actor.home.rot.x == 0) {
            this->actor.home.rot.x = this->bodyBreak.count;
        }

        if (Part_break(&this->actor, &this->bodyBreak, play, this->actor.params + 8)) {
            this->actor.child = &this->actor;
        }
    } else {
        if (this->actor.home.rot.x == 0) {
            this->actor.colChkInfo.health = 10;

            if (this->actor.params == STALFOS_TYPE_4) {
                this->actor.params = -1;
            } else {
                Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
            }

            this->actor.child = NULL;
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            En_Test_Actor_mode_back_jump_init(this);
        } else if ((this->actor.params == STALFOS_TYPE_5) &&
                   !ActorSearch(play, &this->actor, ACTOR_EN_TEST, ACTORCAT_ENEMY, 8000.0f)) {
            Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0xD0);

            if (this->actor.parent != NULL) {
                this->actor.parent->home.rot.z--;
            }

            Actor_delete(&this->actor);
        }
    }
}

void En_Test_Actor_mode_front_down_init(EnTest* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gStalfosFallOverBackwardsAnim);
    Actor_SE_set(&this->actor, NA_SE_EN_STAL_DEAD);
    this->unk_7DE = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.colorFilterTimer = 0;
    this->actor.speed = 0.0f;

    if (this->actor.params <= STALFOS_TYPE_CEILING) {
        this->unk_7C8 = 5;
        En_Test_actor_set_process(this, En_Test_Actor_mode_front_down);
    } else {
        En_Test_Actor_mode_rev_init(this, play);
    }
}

void En_Test_Actor_mode_front_down(EnTest* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->timer = (fqrand() * 10.0f) + 10.0f;
        this->unk_7C8 = 7;
        En_Test_actor_set_process(this, En_Test_Actor_mode_break);
        Part_Break_init(&this->bodyBreak, 60, play);
    }

    if ((s32)this->skelAnime.curFrame == 15) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_DOWN);
    }
}

void En_Test_Actor_mode_back_down_init(EnTest* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gStalfosFallOverForwardsAnim);
    Actor_SE_set(&this->actor, NA_SE_EN_STAL_DEAD);
    this->unk_7C8 = 6;
    this->actor.colorFilterTimer = 0;
    this->unk_7DE = 0;
    this->actor.speed = 0.0f;

    if (this->actor.params <= STALFOS_TYPE_CEILING) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        En_Test_actor_set_process(this, En_Test_Actor_mode_back_down);
    } else {
        En_Test_Actor_mode_rev_init(this, play);
    }
}

void En_Test_Actor_mode_back_down(EnTest* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->timer = (fqrand() * 10.0f) + 10.0f;
        this->unk_7C8 = 7;
        En_Test_actor_set_process(this, En_Test_Actor_mode_break);
        Part_Break_init(&this->bodyBreak, 60, play);
    }

    if (((s32)this->skelAnime.curFrame == 10) || ((s32)this->skelAnime.curFrame == 25)) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_DOWN);
    }
}

void En_Test_Actor_mode_hirumu_init(EnTest* this) {
    this->swordState = 0;
    this->skelAnime.movementFlags = ANIM_FLAG_UPDATE_Y;
    this->unk_7C8 = 0x13;
    this->skelAnime.playSpeed = -1.0f;
    this->skelAnime.startFrame = this->skelAnime.curFrame;
    this->skelAnime.endFrame = 0.0f;
    En_Test_actor_set_process(this, En_Test_Actor_mode_hirumu);
}

void En_Test_Actor_mode_hirumu(EnTest* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (fqrand() > 0.7f) {
            En_Test_Actor_mode_wait_init(this);
            this->timer = (fqrand() * 5.0f) + 5.0f;
        } else if (((play->gameplayFrames % 2) != 0) && (this->actor.params != STALFOS_TYPE_CEILING)) {
            En_Test_Actor_mode_back_jump_init(this);
        } else {
            En_Test_Actor_mode_side_step_init(this, play);
        }
    }
}

void En_Test_Actor_mode_nobi(EnTest* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->actor.scale.y < 0.015f) {
        this->actor.scale.y += 0.002f;
        this->actor.world.pos.y = this->actor.home.pos.y - 3.5f;
    } else {
        this->actor.world.pos.y = this->actor.home.pos.y;
        En_Test_Actor_mode_back_jump_init(this);
    }
}

void En_Test_Actor_mode_break(EnTest* this, PlayState* play) {
    this->actor.params = STALFOS_TYPE_1;

    if (Part_break(&this->actor, &this->bodyBreak, play, this->actor.params)) {
        Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0xD0);

        if (this->actor.parent != NULL) {
            this->actor.parent->home.rot.z--;
        }

        Actor_delete(&this->actor);
    }
}

void En_Test_Face_To_Player(EnTest* this, PlayState* play) {
    s16 lookAngle = this->actor.yawTowardsPlayer;

    lookAngle -= (s16)(this->headRot.y + this->actor.shape.rot.y);

    this->headRotOffset.y = CLAMP(lookAngle, -0x7D0, 0x7D0);
    this->headRot.y += this->headRotOffset.y;
    this->headRot.y = CLAMP(this->headRot.y, -0x382F, 0x382F);
}

void En_Test_damage_proc(EnTest* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->shieldCollider.base.acFlags & AC_BOUNCED) {
        this->shieldCollider.base.acFlags &= ~AC_BOUNCED;
        this->bodyCollider.base.acFlags &= ~AC_HIT;

        if (this->unk_7C8 >= 0xA) {
            this->actor.speed = -4.0f;
        }
    } else if (this->bodyCollider.base.acFlags & AC_HIT) {
        this->bodyCollider.base.acFlags &= ~AC_HIT;

        if ((this->actor.colChkInfo.damageEffect != STALFOS_DMGEFF_SLING) &&
            (this->actor.colChkInfo.damageEffect != STALFOS_DMGEFF_FIREMAGIC)) {
            this->lastDamageEffect = this->actor.colChkInfo.damageEffect;
            if (this->swordState >= 1) {
                this->swordState = 0;
            }
            this->unk_7DC = player->unk_845;
            this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            Hit_bit_set(&this->actor, &this->bodyCollider.elem, false);
            Nai_StopObjFlagFx(&this->actor.projectedPos, NA_SE_EN_STAL_WARAU);

            if ((this->actor.colChkInfo.damageEffect == STALFOS_DMGEFF_STUN) ||
                (this->actor.colChkInfo.damageEffect == STALFOS_DMGEFF_FREEZE) ||
                (this->actor.colChkInfo.damageEffect == STALFOS_DMGEFF_LIGHT)) {
                if (this->unk_7C8 != 0xB) {
                    hp_down(&this->actor);
                    En_Test_Actor_mode_paralyze_init(this);
                }
            } else {
                if (Actor_player_direction_check(&this->actor, 0x4000)) {
                    if (hp_down(&this->actor) == 0) {
                        Actor_info_finish(play, &this->actor);
                        En_Test_Actor_mode_front_down_init(this, play);
                    } else {
                        En_Test_Actor_mode_front_damage_init(this);
                    }
                } else if (hp_down(&this->actor) == 0) {
                    En_Test_Actor_mode_back_down_init(this, play);
                    Actor_info_finish(play, &this->actor);
                } else {
                    En_Test_Actor_mode_back_damage_init(this);
                }
            }
        }
    }
}

void En_Test_move(Actor* thisx, PlayState* play) {
    EnTest* this = (EnTest*)thisx;
    f32 oldWeight;
    u32 floorProperty;
    s32 pad;

    En_Test_damage_proc(this, play);

    if (this->actor.colChkInfo.damageEffect != STALFOS_DMGEFF_FIREMAGIC) {
        Actor_position_moveF(&this->actor);
        Actor_BGcheck2(play, &this->actor, 75.0f, 30.0f, 30.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                    UPDBGCHECKINFO_FLAG_4);

        if (this->actor.params == STALFOS_TYPE_1) {
            if (this->actor.world.pos.y <= this->actor.home.pos.y) {
                this->actor.world.pos.y = this->actor.home.pos.y;
                this->actor.velocity.y = 0.0f;
            }

            if (this->actor.floorHeight <= this->actor.home.pos.y) {
                this->actor.floorHeight = this->actor.home.pos.y;
            }
        } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            floorProperty = T_BGCheck_GroundAutoJumpType_ai(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);

            if ((floorProperty == FLOOR_PROPERTY_5) || (floorProperty == FLOOR_PROPERTY_12) ||
                T_BGCheck_getAttributeCode_ai(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) == FLOOR_TYPE_9) {
                Actor_delete(&this->actor);
                return;
            }
        }

        this->actionFunc(this, play);

        switch (this->unk_7DE) {
            case 0:
                break;

            case 1:
                Skeleton_Info2_init(&this->upperSkelanime, &gStalfosBlockWithShieldAnim, 2.0f, 0.0f,
                                 Si2_anime_end_frame(&gStalfosBlockWithShieldAnim), 2, 2.0f);
                Skeleton_Proc_Add_init(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                              this->upperSkelanime.jointTable, skel_joint_status);
                this->unk_7DE++;
                break;

            case 2:
                Skeleton_Info2_anime_play(&this->upperSkelanime);
                Skeleton_Info_anime_add(&this->skelAnime, this->skelAnime.jointTable,
                                             this->upperSkelanime.jointTable, skel_joint_status);
                break;

            case 3:
                this->unk_7DE++;
                this->upperSkelanime.morphWeight = 4.0f;
                FALLTHROUGH;
            case 4:
                oldWeight = this->upperSkelanime.morphWeight;
                this->upperSkelanime.morphWeight -= 1.0f;

                if (this->upperSkelanime.morphWeight <= 0.0f) {
                    this->unk_7DE = 0;
                }

                Si_morf_joint(this->skelAnime.limbCount, this->upperSkelanime.jointTable,
                                           this->upperSkelanime.jointTable, this->skelAnime.jointTable,
                                           1.0f - (this->upperSkelanime.morphWeight / oldWeight));
                Skeleton_Info_anime_add(&this->skelAnime, this->skelAnime.jointTable,
                                             this->upperSkelanime.jointTable, skel_joint_status);
                break;
        }

        if ((this->actor.colorFilterTimer == 0) && (this->actor.colChkInfo.health != 0)) {
            if ((this->unk_7C8 != 0x10) && (this->unk_7C8 != 0x17)) {
                En_Test_Face_To_Player(this, play);
            } else {
                add_calc_short_angle2(&this->headRot.y, 0, 1, 0x3E8, 0);
            }
        }
    }

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->bodyCollider);

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 45.0f;

    if ((this->actor.colChkInfo.health > 0) || (this->actor.colorFilterTimer != 0)) {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->bodyCollider.base);

        if ((this->unk_7C8 >= 0xA) &&
            ((this->actor.colorFilterTimer == 0) || !(this->actor.colorFilterParams & 0x4000))) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->bodyCollider.base);
        }

        if (this->unk_7DE != 0) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->shieldCollider.base);
        }
    }

    if (this->swordState >= 1) {
        if (!(this->swordCollider.base.atFlags & AT_BOUNCED)) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->swordCollider.base);
        } else {
            this->swordCollider.base.atFlags &= ~AT_BOUNCED;
            En_Test_Actor_mode_hirumu_init(this);
        }
    }

    if (this->actor.params == STALFOS_TYPE_INVISIBLE) {
        if (play->actorCtx.lensActive) {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_REACT_TO_LENS;
            this->actor.shape.shadowDraw = Actor_shadow_foot;
        } else {
            this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_REACT_TO_LENS);
            this->actor.shape.shadowDraw = NULL;
        }
    }
}

s32 en_test_display1(PlayState* play2, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnTest* this = (EnTest*)thisx;
    PlayState* play = (PlayState*)play2;

    if (limbIndex == STALFOS_LIMB_HEAD_ROOT) {
        rot->x += this->headRot.y;
        rot->y -= this->headRot.x;
        rot->z += this->headRot.z;
    } else if (limbIndex == STALFOS_LIMB_HEAD) {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_test.c", 3582);

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 80 + ABS((s16)(sin_s(play->gameplayFrames * 2000) * 175.0f)), 0, 0, 255);

        CLOSE_DISPS(play->state.gfxCtx, "../z_en_test.c", 3587);
    }

    if ((this->actor.params == STALFOS_TYPE_INVISIBLE) &&
        !CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) {
        *dList = NULL;
    }

    return false;
}

void en_test_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f local_eye = { 1100.0f, -700.0f, 0.0f };
    static Vec3f local_foot = { 300.0f, 0.0f, 0.0f };
    static Vec3f sword_top = { 3400.0f, 0.0f, 0.0f };
    static Vec3f sword_root = { 0.0f, 0.0f, 0.0f };
    static Vec3f sword_top2 = { 7000.0f, 1000.0f, 0.0f };
    static Vec3f sword_root2 = { 3000.0f, -2000.0f, -1000.0f };
    static Vec3f sword_top3 = { 3000.0f, -2000.0f, 1000.0f };
    static Vec3f sword_root3 = { -1300.0f, 1100.0f, 0.0f };
    static Vec3f shield_data = { -3000.0f, 1900.0f, 800.0f };
    static Vec3f unused3 = { -3000.0f, -1100.0f, 800.0f };
    static Vec3f unused4 = { 1900.0f, 1900.0f, 800.0f };
    static Vec3f shield_data2 = { -3000.0f, -1100.0f, 800.0f };
    static Vec3f unused6 = { 1900.0f, -1100.0f, 800.0f };
    static Vec3f unused7 = { 1900.0f, 1900.0f, 800.0f };
    s32 bodyPart = -1;
    Vec3f sp70;
    Vec3f sp64;
    EnTest* this = (EnTest*)thisx;
    s32 pad;
    Vec3f sp50;

    Part_Break_Get(&this->bodyBreak, limbIndex, 0, 60, 60, dList, BODYBREAK_OBJECT_SLOT_DEFAULT);

    if (limbIndex == STALFOS_LIMB_SWORD) {
        Matrix_Position(&sword_top2, &this->swordCollider.dim.quad[1]);
        Matrix_Position(&sword_root2, &this->swordCollider.dim.quad[0]);
        Matrix_Position(&sword_top3, &this->swordCollider.dim.quad[3]);
        Matrix_Position(&sword_root3, &this->swordCollider.dim.quad[2]);

        CollisionCheck_Uty_setSword4Pos(&this->swordCollider, &this->swordCollider.dim.quad[0],
                                 &this->swordCollider.dim.quad[1], &this->swordCollider.dim.quad[2],
                                 &this->swordCollider.dim.quad[3]);

        Matrix_Position(&sword_top, &sp70);
        Matrix_Position(&sword_root, &sp64);

        if ((this->swordState >= 1) && ((this->actor.params != STALFOS_TYPE_INVISIBLE) || play->actorCtx.lensActive)) {
            EffectBlure_edge_add(Effect_GetEffectMemoryPointer(this->effectIndex), &sp70, &sp64);
        } else if (this->swordState >= 0) {
            EffectBlure_space_add(Effect_GetEffectMemoryPointer(this->effectIndex));
            this->swordState = -1;
        }

    } else if ((limbIndex == STALFOS_LIMB_SHIELD) && (this->unk_7DE != 0)) {
        Matrix_Position(&sword_root, &sp64);

        this->shieldCollider.dim.pos.x = sp64.x;
        this->shieldCollider.dim.pos.y = sp64.y;
        this->shieldCollider.dim.pos.z = sp64.z;
    } else {
        Actor_foot_shadow_pos_set(&this->actor, limbIndex, STALFOS_LIMB_FOOT_L, &local_foot, STALFOS_LIMB_ANKLE_R, &local_foot);

        if ((limbIndex == STALFOS_LIMB_FOOT_L) || (limbIndex == STALFOS_LIMB_ANKLE_R)) {
            if ((this->unk_7C8 == 0x15) || (this->unk_7C8 == 0x16)) {
                if (this->actor.speed != 0.0f) {
                    Matrix_Position(&local_foot, &sp64);
                    _dust_ground_set(play, &this->actor, &sp64, 10.0f, 1, 8.0f, 100, 15, false);
                }
            }
        }
    }

    if (this->iceTimer != 0) {
        switch (limbIndex) {
            case STALFOS_LIMB_HEAD:
                bodyPart = 0;
                break;
            case STALFOS_LIMB_CHEST:
                bodyPart = 1;
                break;
            case STALFOS_LIMB_SWORD:
                bodyPart = 2;
                break;
            case STALFOS_LIMB_SHIELD:
                bodyPart = 3;
                break;
            case STALFOS_LIMB_UPPERARM_R:
                bodyPart = 4;
                break;
            case STALFOS_LIMB_UPPERARM_L:
                bodyPart = 5;
                break;
            case STALFOS_LIMB_WAIST:
                bodyPart = 6;
                break;
            case STALFOS_LIMB_FOOT_L:
                bodyPart = 7;
                break;
            case STALFOS_LIMB_FOOT_R:
                bodyPart = 8;
                break;
        }

        if (bodyPart >= 0) {
            Matrix_Position(&sword_root, &sp50);

            this->bodyPartsPos[bodyPart].x = sp50.x;
            this->bodyPartsPos[bodyPart].y = sp50.y;
            this->bodyPartsPos[bodyPart].z = sp50.z;
        }
    }
}

void En_Test_display(Actor* thisx, PlayState* play) {
    EnTest* this = (EnTest*)thisx;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Actor_HiliteReflect_set_init(&this->actor, play, 1);

    if ((thisx->params <= STALFOS_TYPE_CEILING) || (thisx->child == NULL)) {
        Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, en_test_display1,
                          en_test_display2, this);
    }

    if (this->iceTimer != 0) {
        thisx->colorFilterTimer++;
        this->iceTimer--;

        if ((this->iceTimer % 4) == 0) {
            s32 iceIndex = this->iceTimer >> 2;

            Effect_En_Ice_ct1(play, thisx, &this->bodyPartsPos[iceIndex], 150, 150, 150, 250, 235, 245,
                                           255, 1.5f);
        }
    }
}

// a variation of sidestep
void En_Test_Actor_mode_side_step_init2(EnTest* this, f32 xzSpeed) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gStalfosSidestepAnim, -2.0f);
    this->actor.speed = xzSpeed;
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF;
    this->timer = (fqrand() * 20.0f) + 15.0f;
    this->unk_7C8 = 0x18;
    En_Test_actor_set_process(this, En_Test_Actor_mode_side_step);
}

/**
 * Check if a projectile actor is within 300 units and react accordingly.
 * Returns true if the projectile test passes and a new action is performed.
 */
static s32 Shot_def(PlayState* play, EnTest* this) {
    Actor* projectileActor;
    s16 yawToProjectile;
    s16 wallYawDiff;
    s16 touchingWall;
    s16 directionFlag;

    projectileActor = ShotVsMyCheck(play, &this->actor, 300.0f);

    if (projectileActor != NULL) {
        yawToProjectile = Actor_search_actor_angleY(&this->actor, projectileActor) - (u16)this->actor.shape.rot.y;

        if ((u8)(this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
            wallYawDiff = ((u16)this->actor.wallYaw - (u16)this->actor.shape.rot.y);
            touchingWall = true;
        } else {
            touchingWall = false;
        }

        if (search_position_distance(&this->actor.world.pos, &projectileActor->world.pos) < 200.0f) {
            if (Anc_Fight_My_Check(play, &this->actor) && (projectileActor->id == ACTOR_ARMS_HOOK)) {
                En_Test_Actor_mode_jump_move_init(this);
            } else if (ABS(yawToProjectile) < 0x2000) {
                En_Test_Actor_mode_defend_in_init(this);
            } else if (ABS(yawToProjectile) < 0x6000) {
                En_Test_Actor_mode_back_jump_init(this);
            } else {
                En_Test_Actor_mode_jump_move_init(this);
            }

            return true;
        }

        if (Anc_Fight_My_Check(play, &this->actor) && (projectileActor->id == ACTOR_ARMS_HOOK)) {
            En_Test_Actor_mode_jump_move_init(this);
            return true;
        }

        if ((ABS(yawToProjectile) < 0x2000) || (ABS(yawToProjectile) > 0x6000)) {
            directionFlag = play->gameplayFrames % 2;

            if (touchingWall && (wallYawDiff > 0x2000) && (wallYawDiff < 0x6000)) {
                directionFlag = false;
            } else if (touchingWall && (wallYawDiff < -0x2000) && (wallYawDiff > -0x6000)) {
                directionFlag = true;
            }

            if (directionFlag) {
                En_Test_Actor_mode_side_step_init2(this, 4.0f);
            } else {
                En_Test_Actor_mode_side_step_init2(this, -4.0f);
            }
        } else if (ABS(yawToProjectile) < 0x6000) {
            directionFlag = play->gameplayFrames % 2;

            if (touchingWall && (ABS(wallYawDiff) > 0x6000)) {
                directionFlag = false;
            } else if (touchingWall && (ABS(wallYawDiff) < 0x2000)) {
                directionFlag = true;
            }

            if (directionFlag) {
                En_Test_Actor_mode_back_jump_init(this);
            } else {
                En_Test_Actor_mode_jump_move_init(this);
            }
        }

        return true;
    }

    return false;
}
