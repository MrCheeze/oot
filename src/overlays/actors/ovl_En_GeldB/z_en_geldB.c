/*
 * File: z_en_geldb.c
 * Overlay: ovl_En_GeldB
 * Description: Gerudo fighter
 */

#include "z_en_geldb.h"
#include "assets/objects/object_geldb/object_geldb.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

typedef enum EnGeldBAction {
    /*  0 */ GELDB_WAIT,
    /*  1 */ GELDB_DEFEAT,
    /*  2 */ GELDB_DAMAGED,
    /*  3 */ GELDB_JUMP,
    /*  4 */ GELDB_ROLL_BACK,
    /*  5 */ GELDB_READY,
    /*  6 */ GELDB_BLOCK,
    /*  7 */ GELDB_SLASH,
    /*  8 */ GELDB_ADVANCE,
    /*  9 */ GELDB_PIVOT,
    /* 10 */ GELDB_CIRCLE,
    /* 11 */ GELDB_UNUSED,
    /* 12 */ GELDB_SPIN_ATTACK,
    /* 13 */ GELDB_SIDESTEP,
    /* 14 */ GELDB_ROLL_FORWARD,
    /* 15 */ GELDB_STUNNED,
    /* 16 */ GELDB_SPIN_DODGE
} EnGeldBAction;

void En_GeldB_Actor_ct(Actor* thisx, PlayState* play);
void En_GeldB_Actor_dt(Actor* thisx, PlayState* play);
void En_GeldB_move(Actor* thisx, PlayState* play);
void En_GeldB_display(Actor* thisx, PlayState* play);

static s32 Shot_def(PlayState* play, EnGeldB* this);

static void mode_before_drop_init(EnGeldB* this);
static void mode_wait_init(EnGeldB* this);
static void mode_forward_init(EnGeldB* this, PlayState* play);
static void mode_search_rot_init(EnGeldB* this);
void mode_oi_jump_init(EnGeldB* this);
static void mode_move_to_player_front_init(EnGeldB* this);
void mode_esc_init(EnGeldB* this, PlayState* play);
static void mode_kiru_init(EnGeldB* this);
void mode_jump_attack_init(EnGeldB* this);
static void mode_back_jump_init(EnGeldB* this);
static void mode_jump_move_init(EnGeldB* this);
static void mode_defense_init(EnGeldB* this);
static void mode_side_step_init(EnGeldB* this, PlayState* play);
static void mode_down_init(EnGeldB* this);

static void mode_before_drop(EnGeldB* this, PlayState* play);
void mode_ret_jump(EnGeldB* this, PlayState* play);
static void mode_wait(EnGeldB* this, PlayState* play);
static void mode_forward(EnGeldB* this, PlayState* play);
void mode_oi_jump(EnGeldB* this, PlayState* play);
static void mode_search_rot(EnGeldB* this, PlayState* play);
static void mode_move_to_player_front(EnGeldB* this, PlayState* play);
void mode_esc(EnGeldB* this, PlayState* play);
static void mode_kiru(EnGeldB* this, PlayState* play);
void mode_jump_attack(EnGeldB* this, PlayState* play);
static void mode_back_jump(EnGeldB* this, PlayState* play);
static void mode_paralyze(EnGeldB* this, PlayState* play);
static void mode_damage(EnGeldB* this, PlayState* play);
static void mode_jump_move(EnGeldB* this, PlayState* play);
static void mode_defense(EnGeldB* this, PlayState* play);
static void mode_side_step(EnGeldB* this, PlayState* play);
static void mode_down(EnGeldB* this, PlayState* play);

ActorProfile En_GeldB_Profile = {
    /**/ ACTOR_EN_GELDB,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_GELDB,
    /**/ sizeof(EnGeldB),
    /**/ En_GeldB_Actor_ct,
    /**/ En_GeldB_Actor_dt,
    /**/ En_GeldB_move,
    /**/ En_GeldB_display,
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
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 50, 0, { 0, 0, 0 } },
};

static ColliderTrisElementInit AcInfoShieldDataElem[2] = {
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFC1FFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -10.0f, 14.0f, 2.0f }, { -10.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFC1FFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -10.0f, -6.0f, 2.0f }, { 9.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
};

static ColliderTrisInit AcInfoShieldData = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    AcInfoShieldDataElem,
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
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL | ATELEM_UNK7,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum EnGeldBDamageEffects {
    /* 0x0 */ GELDB_DMG_NORMAL,
    /* 0x1 */ GELDB_DMG_STUN,
    /* 0x6 */ GELDB_DMG_UNK_6 = 0x6,
    /* 0xD */ GELDB_DMG_UNK_D = 0xD,
    /* 0xE */ GELDB_DMG_UNK_E,
    /* 0xF */ GELDB_DMG_FREEZE
} EnGeldBDamageEffects;

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, GELDB_DMG_STUN),
    /* Deku stick    */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Slingshot     */ DMG_ENTRY(1, GELDB_DMG_NORMAL),
    /* Explosive     */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Boomerang     */ DMG_ENTRY(0, GELDB_DMG_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Hammer swing  */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Hookshot      */ DMG_ENTRY(0, GELDB_DMG_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, GELDB_DMG_NORMAL),
    /* Master sword  */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Giant's Knife */ DMG_ENTRY(4, GELDB_DMG_NORMAL),
    /* Fire arrow    */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Ice arrow     */ DMG_ENTRY(2, GELDB_DMG_FREEZE),
    /* Light arrow   */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Unk arrow 1   */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Unk arrow 2   */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Unk arrow 3   */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Fire magic    */ DMG_ENTRY(4, GELDB_DMG_UNK_E),
    /* Ice magic     */ DMG_ENTRY(0, GELDB_DMG_UNK_6),
    /* Light magic   */ DMG_ENTRY(3, GELDB_DMG_UNK_D),
    /* Shield        */ DMG_ENTRY(0, GELDB_DMG_NORMAL),
    /* Mirror Ray    */ DMG_ENTRY(0, GELDB_DMG_NORMAL),
    /* Kokiri spin   */ DMG_ENTRY(1, GELDB_DMG_NORMAL),
    /* Giant spin    */ DMG_ENTRY(4, GELDB_DMG_NORMAL),
    /* Master spin   */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Kokiri jump   */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Giant jump    */ DMG_ENTRY(8, GELDB_DMG_NORMAL),
    /* Master jump   */ DMG_ENTRY(4, GELDB_DMG_NORMAL),
    /* Unknown 1     */ DMG_ENTRY(4, GELDB_DMG_NORMAL),
    /* Unblockable   */ DMG_ENTRY(0, GELDB_DMG_NORMAL),
    /* Hammer jump   */ DMG_ENTRY(4, GELDB_DMG_NORMAL),
    /* Unknown 2     */ DMG_ENTRY(0, GELDB_DMG_NORMAL),
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -3000, ICHAIN_STOP),
};

static Vec3f local_eye = { 1100.0f, -700.0f, 0.0f };

void En_GeldB_Actor_set_process(EnGeldB* this, EnGeldBActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_GeldB_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EffectBlureInit1 blureInit;
    EnGeldB* this = (EnGeldB*)thisx;

    ValueSet_process(thisx, value_init);
    thisx->colChkInfo.damageTable = &btl_data;
    Shape_Info_init(&thisx->shape, 0.0f, Actor_shadow_foot, 0.0f);
    this->actor.colChkInfo.mass = MASS_HEAVY;
    thisx->colChkInfo.health = 20;
    thisx->colChkInfo.cylRadius = 50;
    thisx->colChkInfo.cylHeight = 100;
    thisx->naviEnemyId = NAVI_ENEMY_GERUDO_THIEF;
    this->keyFlag = PARAMS_GET_NOSHIFT(thisx->params, 8, 8);
    thisx->params &= 0xFF;
    this->blinkState = 0;
    this->unkFloat = 10.0f;
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGerudoRedSkel, &gGerudoRedNeutralAnim, this->jointTable,
                       this->morphTable, GELDB_LIMB_MAX);
    ClObjPipe_ct(play, &this->bodyCollider);
    ClObjPipe_set5(play, &this->bodyCollider, thisx, &OcInfoData);
    ClObjTris_ct(play, &this->blockCollider);
    ClObjTris_set5_nzm(play, &this->blockCollider, thisx, &AcInfoShieldData, this->blockElements);
    ClObjSwrd_ct(play, &this->swordCollider);
    ClObjSwrd_set5(play, &this->swordCollider, thisx, &AtInfoData);
    blureInit.p1StartColor[0] = blureInit.p1StartColor[1] = blureInit.p1StartColor[2] = blureInit.p1StartColor[3] =
        blureInit.p2StartColor[0] = blureInit.p2StartColor[1] = blureInit.p2StartColor[2] = blureInit.p1EndColor[0] =
            blureInit.p1EndColor[1] = blureInit.p1EndColor[2] = blureInit.p2EndColor[0] = blureInit.p2EndColor[1] =
                blureInit.p2EndColor[2] = 255;
    blureInit.p2StartColor[3] = 64;
    blureInit.p1EndColor[3] = blureInit.p2EndColor[3] = 0;
    blureInit.elemDuration = 8;
    blureInit.unkFlag = 0;
    blureInit.calcMode = 2;

    EffectAdd(play, &this->blureIndex, EFFECT_BLURE1, 0, 0, &blureInit);
    Actor_set_scale(thisx, 0.012499999f);
    mode_before_drop_init(this);
    if ((this->keyFlag != 0) && Actor_Environment_item_Check(play, this->keyFlag >> 8)) {
        Actor_delete(thisx);
    }
}

void En_GeldB_Actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGeldB* this = (EnGeldB*)thisx;

    Na_StopMiddleBossBgm();
    EffectFreeIndex(play, this->blureIndex);
    ClObjTris_dt_nzf(play, &this->blockCollider);
    ClObjPipe_dt(play, &this->bodyCollider);
    ClObjSwrd_dt(play, &this->swordCollider);
}

static s32 zf_br2(PlayState* play, EnGeldB* this, s16 arg2) {
    Player* player = GET_PLAYER(play);
    Actor* thisx = &this->actor;
    s16 angleToWall;
    s16 angleToLink;

    angleToWall = thisx->wallYaw - thisx->shape.rot.y;
    angleToWall = ABS(angleToWall);
    angleToLink = thisx->yawTowardsPlayer - thisx->shape.rot.y;
    angleToLink = ABS(angleToLink);

    if (PlayerSwingCheck(play, thisx, 100.0f, 0x2710, 0x3E80, thisx->shape.rot.y)) {
        if (player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_START) {
            mode_esc_init(this, play);
            return true;
        } else if (play->gameplayFrames & 1) {
            mode_defense_init(this);
            return true;
        }
    }
    if (PlayerSwingCheck(play, thisx, 100.0f, 0x5DC0, 0x2AA8, thisx->shape.rot.y)) {
        thisx->shape.rot.y = thisx->world.rot.y = thisx->yawTowardsPlayer;
        if ((thisx->bgCheckFlags & BGCHECKFLAG_WALL) && (ABS(angleToWall) < 0x2EE0) &&
            (thisx->xzDistToPlayer < 90.0f)) {
            mode_jump_move_init(this);
            return true;
        } else if (player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_START) {
            mode_esc_init(this, play);
            return true;
        } else if ((thisx->xzDistToPlayer < 90.0f) && (play->gameplayFrames & 1)) {
            mode_defense_init(this);
            return true;
        } else {
            mode_back_jump_init(this);
            return true;
        }
    } else {
        Actor* bomb = ActorSearch(play, thisx, -1, ACTORCAT_EXPLOSIVE, 80.0f);

        if (bomb != NULL) {
            thisx->shape.rot.y = thisx->world.rot.y = thisx->yawTowardsPlayer;
            if (((thisx->bgCheckFlags & BGCHECKFLAG_WALL) && (angleToWall < 0x2EE0)) ||
                (bomb->id == ACTOR_EN_BOM_CHU)) {
                if ((bomb->id == ACTOR_EN_BOM_CHU) && (Actor_search_actor_distance(thisx, bomb) < 80.0f) &&
                    ((s16)(thisx->shape.rot.y - (bomb->world.rot.y - 0x8000)) < 0x3E80)) {
                    mode_jump_move_init(this);
                    return true;
                } else {
                    mode_side_step_init(this, play);
                    return true;
                }
            } else {
                mode_back_jump_init(this);
                return true;
            }
        }
    }

    if (arg2) {
        if (angleToLink >= 0x1B58) {
            mode_side_step_init(this, play);
            return true;
        } else {
            s16 angleToFacingLink = player->actor.shape.rot.y - thisx->shape.rot.y;

            if ((thisx->xzDistToPlayer <= 45.0f) && !Anc_Fight_ham_Check(play, thisx) &&
                ((play->gameplayFrames & 7) || (ABS(angleToFacingLink) < 0x38E0))) {
                mode_kiru_init(this);
                return true;
            } else {
                mode_move_to_player_front_init(this);
                return true;
            }
        }
    }
    return false;
}

static void mode_before_drop_init(EnGeldB* this) {
    Skeleton_Info2_init_standard_speedset_stop(&this->skelAnime, &gGerudoRedJumpAnim, 0.0f);
    this->actor.world.pos.y = this->actor.home.pos.y + 120.0f;
    this->timer = 10;
    this->invisible = true;
    this->action = GELDB_WAIT;
    this->actor.bgCheckFlags &= ~(BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH);
    this->actor.gravity = -2.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    En_GeldB_Actor_set_process(this, mode_before_drop);
}

static void mode_before_drop(EnGeldB* this, PlayState* play) {
    if ((this->invisible && !Actor_Environment_sw_Check(play, this->actor.home.rot.z)) || this->actor.xzDistToPlayer > 300.0f) {
        this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        this->actor.world.pos.y = this->actor.floorHeight + 120.0f;
    } else {
        this->invisible = false;
        this->actor.shape.shadowScale = 90.0f;
        Na_StartMiddleBossBgm(NA_BGM_MINI_BOSS);
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_DOWN);
        this->skelAnime.playSpeed = 1.0f;
        this->actor.world.pos.y = this->actor.floorHeight;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.focus.pos = this->actor.world.pos;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND_TOUCH;
        this->actor.velocity.y = 0.0f;
        _dust_ground_set(play, &this->actor, &this->leftFootPos, 3.0f, 2, 2.0f, 0, 0, false);
        _dust_ground_set(play, &this->actor, &this->rightFootPos, 3.0f, 2, 2.0f, 0, 0, false);
    }
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_wait_init(this);
    }
}

void mode_ret_jump_init(EnGeldB* this) {
    f32 lastFrame = Si2_anime_end_frame(&gGerudoRedJumpAnim);

    Skeleton_Info2_init(&this->skelAnime, &gGerudoRedJumpAnim, -2.0f, lastFrame, 0.0f, ANIMMODE_ONCE_INTERP, -4.0f);
    this->timer = 20;
    this->invisible = false;
    this->action = GELDB_WAIT;
    this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    En_GeldB_Actor_set_process(this, mode_ret_jump);
}

void mode_ret_jump(EnGeldB* this, PlayState* play) {
    if (this->skelAnime.curFrame == 10.0f) {
        Actor_SE_set(&this->actor, NA_SE_EN_STAL_JUMP);
    }
    if (this->skelAnime.curFrame == 2.0f) {
        this->actor.gravity = 0.0f;
        _dust_ground_set(play, &this->actor, &this->leftFootPos, 3.0f, 2, 2.0f, 0, 0, false);
        _dust_ground_set(play, &this->actor, &this->rightFootPos, 3.0f, 2, 2.0f, 0, 0, false);
    }
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        add_calc(&this->actor.world.pos.y, this->actor.floorHeight + 300.0f, 1.0f, 20.5f, 0.0f);
        this->timer--;
        if (this->timer == 0) {
            Actor_delete(&this->actor);
        }
    }
}

static void mode_wait_init(EnGeldB* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGerudoRedNeutralAnim, -4.0f);
    this->action = GELDB_READY;
    this->timer = fqrand() * 10.0f + 5.0f;
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    En_GeldB_Actor_set_process(this, mode_wait);
}

static void mode_wait(EnGeldB* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    s16 angleToLink;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->lookTimer != 0) {
        angleToLink = this->actor.yawTowardsPlayer - this->actor.shape.rot.y - this->headRot.y;
        if (ABS(angleToLink) > 0x2000) {
            this->lookTimer--;
            return;
        }
        this->lookTimer = 0;
    }
    angleToLink = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    if (!Shot_def(play, this)) {
        if (this->unkTimer != 0) {
            this->unkTimer--;

            if (ABS(angleToLink) >= 0x1FFE) {
                return;
            }
            this->unkTimer = 0;
        } else if (zf_br2(play, this, 0)) {
            return;
        }
        angleToLink = player->actor.shape.rot.y - this->actor.shape.rot.y;
        if ((this->actor.xzDistToPlayer < 100.0f) && (player->meleeWeaponState != 0) && (ABS(angleToLink) >= 0x1F40)) {
            this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            mode_move_to_player_front_init(this);
        } else if (--this->timer == 0) {
            if (Actor_player_direction_check(&this->actor, 30 * 0x10000 / 360)) {
                if ((210.0f > this->actor.xzDistToPlayer) && (this->actor.xzDistToPlayer > 150.0f) &&
                    (fqrand() < 0.3f)) {
                    if (Anc_Fight_ham_Check(play, &this->actor) || (fqrand() > 0.5f) ||
                        (ABS(angleToLink) < 0x38E0)) {
                        mode_oi_jump_init(this);
                    } else {
                        mode_jump_attack_init(this);
                    }
                } else if (fqrand() > 0.3f) {
                    mode_forward_init(this, play);
                } else {
                    mode_move_to_player_front_init(this);
                }
            } else {
                mode_search_rot_init(this);
            }
            if ((play->gameplayFrames & 0x5F) == 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_GERUDOFT_BREATH);
            }
        }
    }
}

static void mode_forward_init(EnGeldB* this, PlayState* play) {
    f32 lastFrame = Si2_anime_end_frame(&gGerudoRedWalkAnim);

    Skeleton_Info2_init(&this->skelAnime, &gGerudoRedWalkAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP, -4.0f);
    this->action = GELDB_ADVANCE;
    En_GeldB_Actor_set_process(this, mode_forward);
}

static void mode_forward(EnGeldB* this, PlayState* play) {
    s32 prevFrame;
    s32 beforeCurFrame;
    s32 absPlaySpeed;
    s16 facingAngletoLink;
    Player* player = GET_PLAYER(play);

    if (!Shot_def(play, this)) {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0x2EE, 0);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (this->actor.xzDistToPlayer <= 40.0f) {
            add_calc(&this->actor.speed, -8.0f, 1.0f, 1.5f, 0.0f);
        } else if (this->actor.xzDistToPlayer > 55.0f) {
            add_calc(&this->actor.speed, 8.0f, 1.0f, 1.5f, 0.0f);
        } else {
            add_calc(&this->actor.speed, 0.0f, 1.0f, 6.65f, 0.0f);
        }
        this->skelAnime.playSpeed = this->actor.speed / 8.0f;
        facingAngletoLink = player->actor.shape.rot.y - this->actor.shape.rot.y;
        facingAngletoLink = ABS(facingAngletoLink);
        if ((this->actor.xzDistToPlayer < 150.0f) && (player->meleeWeaponState != 0) && (facingAngletoLink >= 0x1F40)) {
            this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            if (fqrand() > 0.7f) {
                mode_move_to_player_front_init(this);
                return;
            }
        }

        prevFrame = (s32)this->skelAnime.curFrame;
        Skeleton_Info2_anime_play(&this->skelAnime);
        beforeCurFrame = (s32)(this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed));
        absPlaySpeed = (s32)(f32)ABS(this->skelAnime.playSpeed);

        if (!Actor_player_direction_check(&this->actor, 0x11C7)) {
            if (fqrand() > 0.5f) {
                mode_move_to_player_front_init(this);
            } else {
                mode_wait_init(this);
            }
        } else if (this->actor.xzDistToPlayer < 90.0f) {
            if (!Anc_Fight_ham_Check(play, &this->actor) &&
                (fqrand() > 0.03f || (this->actor.xzDistToPlayer <= 45.0f && facingAngletoLink < 0x38E0))) {
                mode_kiru_init(this);
            } else if (Anc_Fight_ham_Check(play, &this->actor) && (fqrand() > 0.5f)) {
                mode_back_jump_init(this);
            } else {
                mode_move_to_player_front_init(this);
            }
        }
        if (!zf_br2(play, this, 0)) {
            if ((this->actor.xzDistToPlayer < 210.0f) && (this->actor.xzDistToPlayer > 150.0f) &&
                Actor_player_direction_check(&this->actor, 0x71C)) {
                if (Anc_Fight_My_Check(play, &this->actor)) {
                    if (fqrand() > 0.5f) {
                        mode_oi_jump_init(this);
                    } else {
                        mode_jump_attack_init(this);
                    }
                } else {
                    mode_move_to_player_front_init(this);
                    return;
                }
            }
            if ((play->gameplayFrames & 0x5F) == 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_GERUDOFT_BREATH);
            }
            if (prevFrame != (s32)this->skelAnime.curFrame) {
                s32 afterPrevFrame = absPlaySpeed + prevFrame;

                if (((beforeCurFrame < 0) && (afterPrevFrame > 0)) || ((beforeCurFrame < 4) && (afterPrevFrame > 4))) {
                    Actor_SE_set(&this->actor, NA_SE_EN_MUSI_LAND);
                }
            }
        }
    }
}

void mode_oi_jump_init(EnGeldB* this) {
    f32 lastFrame = Si2_anime_end_frame(&gGerudoRedFlipAnim);

    Skeleton_Info2_init(&this->skelAnime, &gGerudoRedFlipAnim, -1.0f, lastFrame, 0.0f, ANIMMODE_ONCE, -3.0f);
    this->timer = 0;
    this->invisible = true;
    this->action = GELDB_ROLL_FORWARD;
    this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    this->actor.speed = 10.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_STAL_JUMP);
    En_GeldB_Actor_set_process(this, mode_oi_jump);
}

void mode_oi_jump(EnGeldB* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 facingAngleToLink = player->actor.shape.rot.y - this->actor.shape.rot.y;

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->invisible = false;
        this->actor.speed = 0.0f;
        if (!Actor_player_direction_check(&this->actor, 0x1554)) {
            mode_wait_init(this);
            this->timer = (fqrand() * 5.0f) + 5.0f;
            if (ABS(facingAngleToLink) < 0x38E0) {
                this->lookTimer = 20;
            }
        } else if (!Anc_Fight_ham_Check(play, &this->actor) &&
                   (fqrand() > 0.5f || (ABS(facingAngleToLink) < 0x3FFC))) {
            mode_kiru_init(this);
        } else {
            mode_forward_init(this, play);
        }
    }
    if ((play->gameplayFrames & 0x5F) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_GERUDOFT_BREATH);
    }
}

static void mode_search_rot_init(EnGeldB* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGerudoRedSidestepAnim, -4.0f);
    this->action = GELDB_PIVOT;
    En_GeldB_Actor_set_process(this, mode_search_rot);
}

static void mode_search_rot(EnGeldB* this, PlayState* play) {
    s16 angleToLink;
    s16 turnRate;
    f32 playSpeed;

    if (!Shot_def(play, this) && !zf_br2(play, this, 0)) {
        angleToLink = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        turnRate = (angleToLink > 0) ? ((angleToLink * 0.25f) + 2000.0f) : ((angleToLink * 0.25f) - 2000.0f);
        this->actor.world.rot.y = this->actor.shape.rot.y += turnRate;
        if (angleToLink > 0) {
            playSpeed = turnRate * 0.5f;
            playSpeed = CLAMP_MAX(playSpeed, 1.0f);
        } else {
            playSpeed = turnRate * 0.5f;
            playSpeed = CLAMP_MIN(playSpeed, -1.0f);
        }
        this->skelAnime.playSpeed = -playSpeed;
        Skeleton_Info2_anime_play(&this->skelAnime);
        if (Actor_player_direction_check(&this->actor, 30 * 0x10000 / 360)) {
            if (fqrand() > 0.8f) {
                mode_move_to_player_front_init(this);
            } else {
                mode_forward_init(this, play);
            }
        }
        if ((play->gameplayFrames & 0x5F) == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_GERUDOFT_BREATH);
        }
    }
}

static void mode_move_to_player_front_init(EnGeldB* this) {
    f32 lastFrame = Si2_anime_end_frame(&gGerudoRedSidestepAnim);

    Skeleton_Info2_init(&this->skelAnime, &gGerudoRedSidestepAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP, 0.0f);
    this->actor.speed = rnd_fx(12.0f);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->skelAnime.playSpeed = -this->actor.speed * 0.5f;
    this->timer = fqrand() * 30.0f + 30.0f;
    this->action = GELDB_CIRCLE;
    this->approachRate = 0.0f;
    En_GeldB_Actor_set_process(this, mode_move_to_player_front);
}

static void mode_move_to_player_front(EnGeldB* this, PlayState* play) {
    s16 angleBehindLink;
    s16 phi_v1;
    s32 afterPrevFrame;
    s32 prevFrame;
    s32 pad;
    s32 beforeCurFrame;
    Player* player = GET_PLAYER(play);

    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA0, 1);
    if (!Shot_def(play, this) && !zf_br2(play, this, 0)) {
        this->actor.world.rot.y = this->actor.shape.rot.y + 0x3A98;
        angleBehindLink = player->actor.shape.rot.y + 0x8000;
        if (sin_s(angleBehindLink - this->actor.shape.rot.y) >= 0.0f) {
            this->actor.speed -= 0.25f;
            if (this->actor.speed < -8.0f) {
                this->actor.speed = -8.0f;
            }
        } else if (sin_s(angleBehindLink - this->actor.shape.rot.y) < 0.0f) {
            this->actor.speed += 0.25f;
            if (this->actor.speed > 8.0f) {
                this->actor.speed = 8.0f;
            }
        }
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) ||
            !BG_point_check(&this->actor, play, this->actor.speed, this->actor.shape.rot.y + 0x3E80)) {
            if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                if (this->actor.speed >= 0.0f) {
                    phi_v1 = this->actor.shape.rot.y + 0x3E80;
                } else {
                    phi_v1 = this->actor.shape.rot.y - 0x3E80;
                }
                phi_v1 = this->actor.wallYaw - phi_v1;
            } else {
                this->actor.speed *= -0.8f;
                phi_v1 = 0;
            }
            if (ABS(phi_v1) > 0x4000) {
                this->actor.speed *= -0.8f;
                if (this->actor.speed < 0.0f) {
                    this->actor.speed -= 0.5f;
                } else {
                    this->actor.speed += 0.5f;
                }
            }
        }
        if (this->actor.xzDistToPlayer <= 45.0f) {
            add_calc(&this->approachRate, -4.0f, 1.0f, 1.5f, 0.0f);
        } else if (this->actor.xzDistToPlayer > 40.0f) {
            add_calc(&this->approachRate, 4.0f, 1.0f, 1.5f, 0.0f);
        } else {
            add_calc(&this->approachRate, 0.0f, 1.0f, 6.65f, 0.0f);
        }
        if (this->approachRate != 0.0f) {
            this->actor.world.pos.x += sin_s(this->actor.shape.rot.y) * this->approachRate;
            this->actor.world.pos.z += cos_s(this->actor.shape.rot.y) * this->approachRate;
        }
        if (ABS(this->approachRate) < ABS(this->actor.speed)) {
            this->skelAnime.playSpeed = -this->actor.speed * 0.5f;
        } else {
            this->skelAnime.playSpeed = -this->approachRate * 0.5f;
        }
        this->skelAnime.playSpeed = CLAMP(this->skelAnime.playSpeed, -3.0f, 3.0f);

        prevFrame = (s32)this->skelAnime.curFrame;
        Skeleton_Info2_anime_play(&this->skelAnime);
        beforeCurFrame = (s32)(this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed));
        afterPrevFrame = (s32)ABS(this->skelAnime.playSpeed) + prevFrame;

        if ((prevFrame != (s32)this->skelAnime.curFrame) &&
            (((beforeCurFrame < 0) && (afterPrevFrame > 0)) || ((beforeCurFrame < 5) && (afterPrevFrame > 5)))) {
            Actor_SE_set(&this->actor, NA_SE_EN_MUSI_LAND);
        }

        if ((play->gameplayFrames & 0x5F) == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_GERUDOFT_BREATH);
        }
        if ((cos_s(angleBehindLink - this->actor.shape.rot.y) < -0.85f) &&
            !Anc_Fight_ham_Check(play, &this->actor) && (this->actor.xzDistToPlayer <= 45.0f)) {
            mode_kiru_init(this);
        } else if (--this->timer == 0) {
            if (Anc_Fight_ham_Check(play, &this->actor) && (fqrand() > 0.5f)) {
                mode_back_jump_init(this);
            } else {
                mode_wait_init(this);
            }
        }
    }
}

void mode_esc_init(EnGeldB* this, PlayState* play) {
    s16 sp3E;
    Player* player = GET_PLAYER(play);
    f32 lastFrame = Si2_anime_end_frame(&gGerudoRedSidestepAnim);

    Skeleton_Info2_init(&this->skelAnime, &gGerudoRedSidestepAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP, 0.0f);
    sp3E = player->actor.shape.rot.y;
    if (sin_s(sp3E - this->actor.shape.rot.y) > 0.0f) {
        this->actor.speed = -10.0f;
    } else if (sin_s(sp3E - this->actor.shape.rot.y) < 0.0f) {
        this->actor.speed = 10.0f;
    } else if (fqrand() > 0.5f) {
        this->actor.speed = 10.0f;
    } else {
        this->actor.speed = -10.0f;
    }
    this->skelAnime.playSpeed = -this->actor.speed * 0.5f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->timer = 6;
    this->approachRate = 0.0f;
    this->unkFloat = 0.0f;
    this->action = GELDB_SPIN_DODGE;

    En_GeldB_Actor_set_process(this, mode_esc);
}

void mode_esc(EnGeldB* this, PlayState* play) {
    s16 phi_v1;
    s32 prevFrame;
    s32 pad;
    s32 beforeCurFrame;
    s32 afterPrevFrame;

    this->actor.world.rot.y = this->actor.yawTowardsPlayer + 0x3A98;
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) ||
        !BG_point_check(&this->actor, play, this->actor.speed, this->actor.shape.rot.y + 0x3E80)) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            if (this->actor.speed >= 0.0f) {
                phi_v1 = this->actor.shape.rot.y + 0x3E80;
            } else {
                phi_v1 = this->actor.shape.rot.y - 0x3E80;
            }
            phi_v1 = this->actor.wallYaw - phi_v1;
        } else {
            this->actor.speed *= -0.8f;
            phi_v1 = 0;
        }
        if (ABS(phi_v1) > 0x4000) {
            mode_jump_move_init(this);
            return;
        }
    }
    if (this->actor.xzDistToPlayer <= 45.0f) {
        add_calc(&this->approachRate, -4.0f, 1.0f, 1.5f, 0.0f);
    } else if (this->actor.xzDistToPlayer > 40.0f) {
        add_calc(&this->approachRate, 4.0f, 1.0f, 1.5f, 0.0f);
    } else {
        add_calc(&this->approachRate, 0.0f, 1.0f, 6.65f, 0.0f);
    }
    if (this->approachRate != 0.0f) {
        this->actor.world.pos.x += sin_s(this->actor.yawTowardsPlayer) * this->approachRate;
        this->actor.world.pos.z += cos_s(this->actor.yawTowardsPlayer) * this->approachRate;
    }
    if (ABS(this->approachRate) < ABS(this->actor.speed)) {
        this->skelAnime.playSpeed = -this->actor.speed * 0.5f;
    } else {
        this->skelAnime.playSpeed = -this->approachRate * 0.5f;
    }
    this->skelAnime.playSpeed = CLAMP(this->skelAnime.playSpeed, -3.0f, 3.0f);

    prevFrame = (s32)this->skelAnime.curFrame;
    Skeleton_Info2_anime_play(&this->skelAnime);
    beforeCurFrame = (s32)(this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed));
    afterPrevFrame = (s32)ABS(this->skelAnime.playSpeed) + prevFrame;

    if ((prevFrame != (s32)this->skelAnime.curFrame) &&
        (((beforeCurFrame < 0) && (afterPrevFrame > 0)) || ((beforeCurFrame < 5) && (afterPrevFrame > 5)))) {
        Actor_SE_set(&this->actor, NA_SE_EN_MUSI_LAND);
    }

    if ((play->gameplayFrames & 0x5F) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_GERUDOFT_BREATH);
    }
    this->timer--;
    if (this->timer == 0) {
        this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
        if (!Shot_def(play, this)) {
            if (!Anc_Fight_ham_Check(play, &this->actor) && (this->actor.xzDistToPlayer <= 70.0f)) {
                mode_kiru_init(this);
            } else {
                mode_back_jump_init(this);
            }
        }
    } else {
        if (this->actor.speed >= 0.0f) {
            this->actor.shape.rot.y += 0x4000;
        } else {
            this->actor.shape.rot.y -= 0x4000;
        }
    }
}

static void mode_kiru_init(EnGeldB* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gGerudoRedSlashAnim);
    this->swordCollider.base.atFlags &= ~AT_BOUNCED;
    this->action = GELDB_SLASH;
    this->spinAttackState = 0;
    this->actor.speed = 0.0f;
    Nai_StopObjFlagFx(&this->actor.projectedPos, NA_SE_EN_GERUDOFT_BREATH);
    En_GeldB_Actor_set_process(this, mode_kiru);
}

static void mode_kiru(EnGeldB* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 angleFacingLink = player->actor.shape.rot.y - this->actor.shape.rot.y;
    s16 angleToLink = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    angleFacingLink = ABS(angleFacingLink);
    angleToLink = ABS(angleToLink);

    this->actor.speed = 0.0f;
    if ((s32)this->skelAnime.curFrame == 1) {
        Actor_SE_set(&this->actor, NA_SE_EN_GERUDOFT_ATTACK);
        this->swordState = 1;
    } else if ((s32)this->skelAnime.curFrame == 6) {
        this->swordState = -1;
    }
    if (this->swordCollider.base.atFlags & AT_BOUNCED) {
        this->swordState = -1;
        this->swordCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        mode_back_jump_init(this);
    } else if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (!Actor_player_direction_check(&this->actor, 0x1554)) {
            mode_wait_init(this);
            this->timer = (fqrand() * 5.0f) + 5.0f;
            if (angleToLink > 0x4000) {
                this->lookTimer = 20;
            }
        } else if (fqrand() > 0.7f || (this->actor.xzDistToPlayer >= 120.0f)) {
            mode_wait_init(this);
            this->timer = (fqrand() * 5.0f) + 5.0f;
        } else {
            this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            if (fqrand() > 0.7f) {
                mode_side_step_init(this, play);
            } else if (angleFacingLink <= 0x2710) {
                if (angleToLink > 0x3E80) {
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                    mode_move_to_player_front_init(this);
                } else {
                    zf_br2(play, this, 1);
                }
            } else {
                mode_move_to_player_front_init(this);
            }
        }
    }
}

void mode_jump_attack_init(EnGeldB* this) {
    f32 lastFrame = Si2_anime_end_frame(&gGerudoRedSpinAttackAnim);

    Skeleton_Info2_init(&this->skelAnime, &gGerudoRedSpinAttackAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE_INTERP, 0.0f);
    this->swordCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
    this->action = GELDB_SPIN_ATTACK;
    this->spinAttackState = 0;
    this->actor.speed = 0.0f;
    En_GeldB_Actor_set_process(this, mode_jump_attack);
}

void mode_jump_attack(EnGeldB* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 angleFacingLink;
    s16 angleToLink;

    if (this->spinAttackState < 2) {
        if (this->swordCollider.base.atFlags & AT_BOUNCED) {
            this->swordCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
            this->spinAttackState = 1;
            this->skelAnime.playSpeed = 1.5f;
        } else if (this->swordCollider.base.atFlags & AT_HIT) {
            this->swordCollider.base.atFlags &= ~AT_HIT;
            if (&player->actor == this->swordCollider.base.at) {
                Actor_player_power_damage_set(play, &this->actor, 6.0f, this->actor.yawTowardsPlayer, 6.0f);
                this->spinAttackState = 2;
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_24);
                message_set(play, 0x6003, &this->actor);
                this->timer = 30;
                this->actor.speed = 0.0f;
                Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_YOUNG_LAUGH);
                return;
            }
        }
    }
    if ((s32)this->skelAnime.curFrame < 9) {
        this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    } else if ((s32)this->skelAnime.curFrame == 13) {
        _dust_ground_set(play, &this->actor, &this->leftFootPos, 3.0f, 2, 2.0f, 0, 0, false);
        _dust_ground_set(play, &this->actor, &this->rightFootPos, 3.0f, 2, 2.0f, 0, 0, false);
        this->swordState = 1;
        this->actor.speed = 10.0f;
        Actor_SE_set(&this->actor, NA_SE_EN_GERUDOFT_ATTACK);
    } else if ((s32)this->skelAnime.curFrame == 21) {
        this->actor.speed = 0.0f;
    } else if ((s32)this->skelAnime.curFrame == 24) {
        this->swordState = -1;
    }
    if (Skeleton_Info2_anime_play(&this->skelAnime) && (this->spinAttackState < 2)) {
        if (!Actor_player_direction_check(&this->actor, 0x1554)) {
            mode_wait_init(this);
            this->timer = (fqrand() * 5.0f) + 5.0f;
            this->lookTimer = 46;
        } else if (this->spinAttackState != 0) {
            mode_back_jump_init(this);
        } else if (fqrand() > 0.7f || (this->actor.xzDistToPlayer >= 120.0f)) {
            mode_wait_init(this);
            this->timer = (fqrand() * 5.0f) + 5.0f;
        } else {
            this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            if (fqrand() > 0.7f) {
                mode_side_step_init(this, play);
            } else {
                angleFacingLink = player->actor.shape.rot.y - this->actor.shape.rot.y;
                angleFacingLink = ABS(angleFacingLink);
                if (angleFacingLink <= 0x2710) {
                    angleToLink = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
                    angleToLink = ABS(angleToLink);
                    if (angleToLink > 0x3E80) {
                        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                        mode_move_to_player_front_init(this);
                    } else {
                        zf_br2(play, this, 1);
                    }
                } else {
                    mode_move_to_player_front_init(this);
                }
            }
        }
    }
}

static void mode_back_jump_init(EnGeldB* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGerudoRedFlipAnim, -3.0f);
    this->timer = 0;
    this->invisible = true;
    this->action = GELDB_ROLL_BACK;
    this->actor.speed = -8.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_STAL_JUMP);
    this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    En_GeldB_Actor_set_process(this, mode_back_jump);
}

static void mode_back_jump(EnGeldB* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (!Anc_Fight_ham_Check(play, &this->actor) && (this->actor.xzDistToPlayer < 170.0f) &&
            (this->actor.xzDistToPlayer > 140.0f) && (fqrand() < 0.2f)) {
            mode_jump_attack_init(this);
        } else if (play->gameplayFrames & 1) {
            mode_side_step_init(this, play);
        } else {
            mode_wait_init(this);
        }
    }
    if ((play->state.frames & 0x5F) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_GERUDOFT_BREATH);
    }
}

static void mode_paralyze_init(EnGeldB* this) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = 0.0f;
    }
    if ((this->damageEffect != GELDB_DMG_FREEZE) || (this->action == GELDB_SPIN_ATTACK)) {
        Skeleton_Info2_init_standard_speedset_stop(&this->skelAnime, &gGerudoRedDamageAnim, 0.0f);
    }
    if (this->damageEffect == GELDB_DMG_FREEZE) {
        this->iceTimer = 36;
    }
    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    this->action = GELDB_STUNNED;
    En_GeldB_Actor_set_process(this, mode_paralyze);
}

static void mode_paralyze(EnGeldB* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->actor.speed = 0.0f;
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->actor.speed < 0.0f) {
            this->actor.speed += 0.05f;
        }
        this->invisible = false;
    }
    if ((this->actor.colorFilterTimer == 0) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (this->actor.colChkInfo.health == 0) {
            mode_down_init(this);
        } else {
            zf_br2(play, this, 1);
        }
    }
}

static void mode_damage_init(EnGeldB* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGerudoRedDamageAnim, -4.0f);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->invisible = false;
        this->actor.speed = -4.0f;
    } else {
        this->invisible = true;
    }
    this->lookTimer = 0;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    Actor_SE_set(&this->actor, NA_SE_EN_GERUDOFT_DAMAGE);
    this->action = GELDB_DAMAGED;
    En_GeldB_Actor_set_process(this, mode_damage);
}

static void mode_damage(EnGeldB* this, PlayState* play) {
    s16 angleToWall;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->actor.speed = 0.0f;
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->actor.speed < 0.0f) {
            this->actor.speed += 0.05f;
        }
        this->invisible = false;
    }
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0x1194, 0);
    if (!Shot_def(play, this) && !zf_br2(play, this, 0) &&
        Skeleton_Info2_anime_play(&this->skelAnime) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        angleToWall = this->actor.wallYaw - this->actor.shape.rot.y;
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) && (ABS(angleToWall) < 0x2EE0) &&
            (this->actor.xzDistToPlayer < 90.0f)) {
            mode_jump_move_init(this);
        } else if (!Shot_def(play, this)) {
            if ((this->actor.xzDistToPlayer <= 45.0f) && !Anc_Fight_ham_Check(play, &this->actor) &&
                (play->gameplayFrames & 7)) {
                mode_kiru_init(this);
            } else {
                mode_back_jump_init(this);
            }
        }
    }
}

static void mode_jump_move_init(EnGeldB* this) {
    f32 lastFrame = Si2_anime_end_frame(&gGerudoRedFlipAnim);

    Skeleton_Info2_init(&this->skelAnime, &gGerudoRedFlipAnim, -1.0f, lastFrame, 0.0f, ANIMMODE_ONCE, -3.0f);
    this->timer = 0;
    this->invisible = false;
    this->action = GELDB_JUMP;
    this->actor.speed = 6.5f;
    this->actor.velocity.y = 15.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_STAL_JUMP);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    En_GeldB_Actor_set_process(this, mode_jump_move);
}

static void mode_jump_move(EnGeldB* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA0, 1);
    if (this->actor.velocity.y >= 5.0f) {
        suna_set(play, &this->leftFootPos);
        suna_set(play, &this->rightFootPos);
    }
    if (Skeleton_Info2_anime_play(&this->skelAnime) &&
        (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH))) {
        this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
        this->actor.shape.rot.x = 0;
        this->actor.speed = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.world.pos.y = this->actor.floorHeight;
        if (!Anc_Fight_ham_Check(play, &this->actor)) {
            mode_kiru_init(this);
        } else {
            mode_wait_init(this);
        }
    }
}

static void mode_defense_init(EnGeldB* this) {
    f32 lastFrame = Si2_anime_end_frame(&gGerudoRedBlockAnim);

    if (this->swordState != 0) {
        this->swordState = -1;
    }
    this->actor.speed = 0.0f;
    this->action = GELDB_BLOCK;
    this->timer = (s32)rnd_fx(10.0f) + 10;
    Skeleton_Info2_init(&this->skelAnime, &gGerudoRedBlockAnim, 0.0f, 0.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    En_GeldB_Actor_set_process(this, mode_defense);
}

static void mode_defense(EnGeldB* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    s16 angleToLink;
    s16 angleFacingLink;

    if (this->timer != 0) {
        this->timer--;
    } else {
        this->skelAnime.playSpeed = 1.0f;
    }
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        angleToLink = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        if ((ABS(angleToLink) <= 0x4000) && (this->actor.xzDistToPlayer < 40.0f) &&
            (ABS(this->actor.yDistToPlayer) < 50.0f)) {
            if (PlayerSwingCheck(play, &this->actor, 100.0f, 0x2710, 0x4000, this->actor.shape.rot.y)) {
                if (player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_START) {
                    mode_esc_init(this, play);
                } else if (play->gameplayFrames & 1) {
                    mode_defense_init(this);
                } else {
                    mode_back_jump_init(this);
                }
            } else {
                angleFacingLink = player->actor.shape.rot.y - this->actor.shape.rot.y;
                if (!Anc_Fight_ham_Check(play, &this->actor) &&
                    ((play->gameplayFrames & 1) || (ABS(angleFacingLink) < 0x38E0))) {
                    mode_kiru_init(this);
                } else {
                    mode_move_to_player_front_init(this);
                }
            }
        } else {
            mode_move_to_player_front_init(this);
        }
    } else if ((this->timer == 0) &&
               PlayerSwingCheck(play, &this->actor, 100.0f, 0x2710, 0x4000, this->actor.shape.rot.y)) {
        if (player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_START) {
            mode_esc_init(this, play);
        } else if (!Shot_def(play, this)) {
            if (play->gameplayFrames & 1) {
                if ((this->actor.xzDistToPlayer < 100.0f) && (fqrand() > 0.7f)) {
                    mode_jump_move_init(this);
                } else {
                    mode_back_jump_init(this);
                }
            } else {
                mode_defense_init(this);
            }
        }
    }
}

static void mode_side_step_init(EnGeldB* this, PlayState* play) {
    s16 playerRotY;
    Player* player;
    f32 lastFrame = Si2_anime_end_frame(&gGerudoRedSidestepAnim);

    Skeleton_Info2_init(&this->skelAnime, &gGerudoRedSidestepAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP, 0.0f);
    player = GET_PLAYER(play);
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA0, 1);
    playerRotY = player->actor.shape.rot.y;
    if (sin_s(playerRotY - this->actor.shape.rot.y) > 0.0f) {
        this->actor.speed = -6.0f;
    } else if (sin_s(playerRotY - this->actor.shape.rot.y) < 0.0f) {
        this->actor.speed = 6.0f;
    } else {
        this->actor.speed = rnd_fx(12.0f);
    }
    this->skelAnime.playSpeed = -this->actor.speed * 0.5f;
    this->approachRate = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF;
    this->timer = fqrand() * 10.0f + 5.0f;
    this->action = GELDB_SIDESTEP;
    En_GeldB_Actor_set_process(this, mode_side_step);
}

static void mode_side_step(EnGeldB* this, PlayState* play) {
    s16 behindLinkAngle;
    s16 phi_v1;
    Player* player = GET_PLAYER(play);
    s32 prevFrame;
    s32 beforeCurFrame;
    f32 absPlaySpeed;

    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 1);
    behindLinkAngle = player->actor.shape.rot.y + 0x8000;
    if (sin_s(behindLinkAngle - this->actor.shape.rot.y) > 0.0f) {
        this->actor.speed += 0.125f;
    } else if (sin_s(behindLinkAngle - this->actor.shape.rot.y) <= 0.0f) {
        this->actor.speed -= 0.125f;
    }

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) ||
        !BG_point_check(&this->actor, play, this->actor.speed, this->actor.shape.rot.y + 0x3E80)) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            if (this->actor.speed >= 0.0f) {
                phi_v1 = this->actor.shape.rot.y + 0x3E80;
            } else {
                phi_v1 = this->actor.shape.rot.y - 0x3E80;
            }
            phi_v1 = this->actor.wallYaw - phi_v1;
        } else {
            this->actor.speed *= -0.8f;
            phi_v1 = 0;
        }
        if (ABS(phi_v1) > 0x4000) {
            this->actor.speed *= -0.8f;
            if (this->actor.speed < 0.0f) {
                this->actor.speed -= 0.5f;
            } else {
                this->actor.speed += 0.5f;
            }
        }
    }
    if (this->actor.speed >= 0.0f) {
        this->actor.world.rot.y = this->actor.shape.rot.y + 0x3E80;
    } else {
        this->actor.world.rot.y = this->actor.shape.rot.y - 0x3E80;
    }
    if (this->actor.xzDistToPlayer <= 45.0f) {
        add_calc(&this->approachRate, -4.0f, 1.0f, 1.5f, 0.0f);
    } else if (this->actor.xzDistToPlayer > 40.0f) {
        add_calc(&this->approachRate, 4.0f, 1.0f, 1.5f, 0.0f);
    } else {
        add_calc(&this->approachRate, 0.0f, 1.0f, 6.65f, 0.0f);
    }
    if (this->approachRate != 0.0f) {
        this->actor.world.pos.x += sin_s(this->actor.shape.rot.y) * this->approachRate;
        this->actor.world.pos.z += cos_s(this->actor.shape.rot.y) * this->approachRate;
    }
    if (ABS(this->approachRate) < ABS(this->actor.speed)) {
        this->skelAnime.playSpeed = -this->actor.speed * 0.5f;
    } else {
        this->skelAnime.playSpeed = -this->approachRate * 0.5f;
    }
    this->skelAnime.playSpeed = CLAMP(this->skelAnime.playSpeed, -3.0f, 3.0f);

    prevFrame = (s32)this->skelAnime.curFrame;
    Skeleton_Info2_anime_play(&this->skelAnime);
    beforeCurFrame = (s32)(this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed));
    absPlaySpeed = ((void)0, ABS(this->skelAnime.playSpeed)); // Needed to match for some reason

    if (!Shot_def(play, this) && !zf_br2(play, this, 0)) {
        if (--this->timer == 0) {
            s16 angleFacingPlayer = player->actor.shape.rot.y - this->actor.shape.rot.y;

            angleFacingPlayer = ABS(angleFacingPlayer);
            if (angleFacingPlayer >= 0x3A98) {
                mode_wait_init(this);
                this->timer = (fqrand() * 5.0f) + 1.0f;
            } else {
                Player* player2 = GET_PLAYER(play);
                s16 angleFacingPlayer2 = player2->actor.shape.rot.y - this->actor.shape.rot.y;

                this->actor.world.rot.y = this->actor.shape.rot.y;
                if ((this->actor.xzDistToPlayer <= 45.0f) && !Anc_Fight_ham_Check(play, &this->actor) &&
                    (!(play->gameplayFrames & 3) || (ABS(angleFacingPlayer2) < 0x38E0))) {
                    mode_kiru_init(this);
                } else if ((210.0f > this->actor.xzDistToPlayer) && (this->actor.xzDistToPlayer > 150.0f) &&
                           !(play->gameplayFrames & 1)) {
                    if (Anc_Fight_ham_Check(play, &this->actor) || (fqrand() > 0.5f) ||
                        (ABS(angleFacingPlayer2) < 0x38E0)) {
                        mode_oi_jump_init(this);
                    } else {
                        mode_jump_attack_init(this);
                    }
                } else {
                    mode_forward_init(this, play);
                }
            }
        }
        if ((prevFrame != (s32)this->skelAnime.curFrame) &&
            (((beforeCurFrame < 0) && (((s32)absPlaySpeed + prevFrame) > 0)) ||
             ((beforeCurFrame < 5) && (((s32)absPlaySpeed + prevFrame) > 5)))) {
            Actor_SE_set(&this->actor, NA_SE_EN_MUSI_LAND);
        }
        if ((play->gameplayFrames & 0x5F) == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_GERUDOFT_BREATH);
        }
    }
}

static void mode_down_init(EnGeldB* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGerudoRedDefeatAnim, -4.0f);
    this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->invisible = false;
        this->actor.speed = -6.0f;
    } else {
        this->invisible = true;
    }
    this->action = GELDB_DEFEAT;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_SE_set(&this->actor, NA_SE_EN_GERUDOFT_DEAD);
    En_GeldB_Actor_set_process(this, mode_down);
}

static void mode_down(EnGeldB* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->actor.speed = 0.0f;
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        add_calc(&this->actor.speed, 0.0f, 1.0f, 0.5f, 0.0f);
        this->invisible = false;
    }
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_ret_jump_init(this);
    } else if ((s32)this->skelAnime.curFrame == 10) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_DOWN);
        Na_StopMiddleBossBgm();
    }
}

static void Face_To_Player(EnGeldB* this, PlayState* play) {
    if ((this->action == GELDB_READY) && (this->lookTimer != 0)) {
        this->headRot.y = sin_s(this->lookTimer * 0x1068) * 8920.0f;
    } else if (this->action != GELDB_STUNNED) {
        if ((this->action != GELDB_SLASH) && (this->action != GELDB_SPIN_ATTACK)) {
            add_calc_short_angle2(&this->headRot.y, this->actor.yawTowardsPlayer - this->actor.shape.rot.y, 1, 0x1F4, 0);
            this->headRot.y = CLAMP(this->headRot.y, -0x256F, 0x256F);
        } else {
            this->headRot.y = 0;
        }
    }
}

static void damage_proc(EnGeldB* this, PlayState* play) {
    s32 pad;
    EnItem00* key;

    if (this->blockCollider.base.acFlags & AC_BOUNCED) {
        this->blockCollider.base.acFlags &= ~AC_BOUNCED;
        this->bodyCollider.base.acFlags &= ~AC_HIT;
    } else if ((this->bodyCollider.base.acFlags & AC_HIT) && (this->action >= GELDB_READY) &&
               (this->spinAttackState < 2)) {
        this->bodyCollider.base.acFlags &= ~AC_HIT;
        if (this->actor.colChkInfo.damageEffect != GELDB_DMG_UNK_6) {
            this->damageEffect = this->actor.colChkInfo.damageEffect;
            Hit_bit_set(&this->actor, &this->bodyCollider.elem, true);
            Nai_StopObjFlagFx(&this->actor.projectedPos, NA_SE_EN_GERUDOFT_BREATH);
            if ((this->actor.colChkInfo.damageEffect == GELDB_DMG_STUN) ||
                (this->actor.colChkInfo.damageEffect == GELDB_DMG_FREEZE)) {
                if (this->action != GELDB_STUNNED) {
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 80);
                    hp_down(&this->actor);
                    mode_paralyze_init(this);
                }
            } else {
                Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                if (hp_down(&this->actor) == 0) {
                    if (this->keyFlag != 0) {
                        key = Item_set0(play, &this->actor.world.pos, this->keyFlag | ITEM00_SMALL_KEY);
                        if (key != NULL) {
                            key->actor.world.rot.y = search_position_angleY(&key->actor.world.pos, &this->actor.home.pos);
                            key->actor.speed = 6.0f;
                            Nai_FxFlagEntry(NA_SE_SY_TRE_BOX_APPEAR, &_dummy_zero_f, 4,
                                                 &_dummy_one, &_dummy_one,
                                                 &_dummy_zero_s8);
                        }
                    }
                    mode_down_init(this);
                    Actor_info_finish(play, &this->actor);
                } else {
                    mode_damage_init(this);
                }
            }
        }
    }
}

void En_GeldB_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGeldB* this = (EnGeldB*)thisx;

    damage_proc(this, play);
    if (this->actor.colChkInfo.damageEffect != GELDB_DMG_UNK_6) {
        Actor_position_moveF(&this->actor);
        Actor_BGcheck2(play, &this->actor, 15.0f, 30.0f, 60.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                    UPDBGCHECKINFO_FLAG_4);
        this->actionFunc(this, play);
        this->actor.focus.pos = this->actor.world.pos;
        this->actor.focus.pos.y += 40.0f;
        Face_To_Player(this, play);
    }
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->bodyCollider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->bodyCollider.base);
    if ((this->action >= GELDB_READY) && (this->spinAttackState < 2) &&
        ((this->actor.colorFilterTimer == 0) || !(this->actor.colorFilterParams & 0x4000))) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->bodyCollider.base);
    }
    if ((this->action == GELDB_BLOCK) && (this->skelAnime.curFrame == 0.0f)) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->blockCollider.base);
    }
    if (this->swordState > 0) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->swordCollider.base);
    }
    if (this->blinkState == 0) {
        if ((fqrand() < 0.1f) && ((play->gameplayFrames % 4) == 0)) {
            this->blinkState++;
        }
    } else {
        this->blinkState = (this->blinkState + 1) & 3;
    }
}

s32 gelb_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnGeldB* this = (EnGeldB*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_geldB.c", 2507);
    if (limbIndex == GELDB_LIMB_NECK) {
        rot->z += this->headRot.x;
        rot->x += this->headRot.y;
        rot->y += this->headRot.z;
    } else if (limbIndex == GELDB_LIMB_HEAD) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 80, 60, 10, 255);
    } else if ((limbIndex == GELDB_LIMB_R_SWORD) || (limbIndex == GELDB_LIMB_L_SWORD)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 140, 170, 230, 255);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
    } else {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 140, 0, 0, 255);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_geldB.c", 2529);
    return false;
}

void gelb_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f local_foot = { 300.0f, 0.0f, 0.0f };
    static Vec3f sword_top = { 0.0f, -3000.0f, 0.0f };
    static Vec3f sword_root = { 400.0f, 0.0f, 0.0f };
    static Vec3f sword_top2 = { 1600.0f, -4000.0f, 0.0f };
    static Vec3f sword_root2 = { -3000.0f, -2000.0f, 1300.0f };
    static Vec3f sword_top3 = { -3000.0f, -2000.0f, -1300.0f };
    static Vec3f sword_root3 = { 1000.0f, 1000.0f, 0.0f };
    static Vec3f dam_root = { 0.0f, 0.0f, 0.0f };
    Vec3f swordTip;
    Vec3f swordHilt;
    EnGeldB* this = (EnGeldB*)thisx;
    s32 bodyPart = -1;

    if (limbIndex == GELDB_LIMB_R_SWORD) {
        Matrix_Position(&sword_top2, &this->swordCollider.dim.quad[1]);
        Matrix_Position(&sword_root2, &this->swordCollider.dim.quad[0]);
        Matrix_Position(&sword_top3, &this->swordCollider.dim.quad[3]);
        Matrix_Position(&sword_root3, &this->swordCollider.dim.quad[2]);
        CollisionCheck_Uty_setSword4Pos(&this->swordCollider, &this->swordCollider.dim.quad[0],
                                 &this->swordCollider.dim.quad[1], &this->swordCollider.dim.quad[2],
                                 &this->swordCollider.dim.quad[3]);
        Matrix_Position(&sword_top, &swordTip);
        Matrix_Position(&sword_root, &swordHilt);

        if ((this->swordState < 0) || ((this->action != GELDB_SLASH) && (this->action != GELDB_SPIN_ATTACK))) {
            EffectBlure_space_add(Effect_GetEffectMemoryPointer(this->blureIndex));
            this->swordState = 0;
        } else if (this->swordState > 0) {
            EffectBlure_edge_add(Effect_GetEffectMemoryPointer(this->blureIndex), &swordTip, &swordHilt);
        }
    } else {
        Actor_foot_shadow_pos_set(&this->actor, limbIndex, GELDB_LIMB_L_FOOT, &local_foot, GELDB_LIMB_R_FOOT, &local_foot);
    }

    if (limbIndex == GELDB_LIMB_L_FOOT) {
        Matrix_Position(&local_foot, &this->leftFootPos);
    } else if (limbIndex == GELDB_LIMB_R_FOOT) {
        Matrix_Position(&local_foot, &this->rightFootPos);
    }

    if (this->iceTimer != 0) {
        switch (limbIndex) {
            case GELDB_LIMB_NECK:
                bodyPart = 0;
                break;
            case GELDB_LIMB_L_SWORD:
                bodyPart = 1;
                break;
            case GELDB_LIMB_R_SWORD:
                bodyPart = 2;
                break;
            case GELDB_LIMB_L_UPPER_ARM:
                bodyPart = 3;
                break;
            case GELDB_LIMB_R_UPPER_ARM:
                bodyPart = 4;
                break;
            case GELDB_LIMB_TORSO:
                bodyPart = 5;
                break;
            case GELDB_LIMB_WAIST:
                bodyPart = 6;
                break;
            case GELDB_LIMB_L_FOOT:
                bodyPart = 7;
                break;
            case GELDB_LIMB_R_FOOT:
                bodyPart = 8;
                break;
            default:
                break;
        }

        if (bodyPart >= 0) {
            Vec3f limbPos;

            Matrix_Position(&dam_root, &limbPos);
            this->bodyPartsPos[bodyPart].x = limbPos.x;
            this->bodyPartsPos[bodyPart].y = limbPos.y;
            this->bodyPartsPos[bodyPart].z = limbPos.z;
        }
    }
}

void En_GeldB_display(Actor* thisx, PlayState* play) {
    static Vec3f shield_data[3] = {
        { -3000.0f, 6000.0f, 1600.0f },
        { -3000.0f, 0.0f, 1600.0f },
        { 3000.0f, 6000.0f, 1600.0f },
    };
    static Vec3f shield_data2[3] = {
        { -3000.0f, 0.0f, 1600.0f },
        { 3000.0f, 0.0f, 1600.0f },
        { 3000.0f, 6000.0f, 1600.0f },
    };
    static void* eye_txt[] = { gGerudoRedEyeOpenTex, gGerudoRedEyeHalfTex, gGerudoRedEyeShutTex,
                                   gGerudoRedEyeHalfTex };
    s32 pad;
    EnGeldB* this = (EnGeldB*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_geldB.c", 2672);

    if ((this->spinAttackState >= 2) && Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->spinAttackState == 2) {
            Skeleton_Info2_init(&this->skelAnime, &gGerudoRedSpinAttackAnim, 0.5f, 0.0f, 12.0f, ANIMMODE_ONCE_INTERP,
                             4.0f);
            this->spinAttackState++;
            thisx->world.rot.y = thisx->shape.rot.y = thisx->yawTowardsPlayer;
        } else {
            this->timer--;
            if (this->timer == 0) {
                if ((INV_CONTENT(ITEM_HOOKSHOT) == ITEM_NONE) || (INV_CONTENT(ITEM_LONGSHOT) == ITEM_NONE)) {
                    play->nextEntranceIndex = ENTR_GERUDO_VALLEY_1;
                } else if (GET_EVENTCHKINF(EVENTCHKINF_C7)) {
                    play->nextEntranceIndex = ENTR_GERUDOS_FORTRESS_18;
                } else {
                    play->nextEntranceIndex = ENTR_GERUDOS_FORTRESS_17;
                }
                play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_BLACK, TCS_FAST);
                play->transitionTrigger = TRANS_TRIGGER_START;
            }
        }
    }

    if ((this->action != GELDB_WAIT) || !this->invisible) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->blinkState]));
        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              gelb_display1, gelb_display2, this);
        if (this->action == GELDB_BLOCK) {
            s32 i;
            Vec3f blockTrisVtx0[3];
            Vec3f blockTrisVtx1[3];

            for (i = 0; i < 3; i++) {
                Matrix_Position(&shield_data[i], &blockTrisVtx0[i]);
                Matrix_Position(&shield_data2[i], &blockTrisVtx1[i]);
            }
            CollisionCheck_Uty_setTrisPos(&this->blockCollider, 0, &blockTrisVtx0[0], &blockTrisVtx0[1], &blockTrisVtx0[2]);
            CollisionCheck_Uty_setTrisPos(&this->blockCollider, 1, &blockTrisVtx1[0], &blockTrisVtx1[1], &blockTrisVtx1[2]);
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
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_geldB.c", 2744);
}

static s32 Shot_def(PlayState* play, EnGeldB* this) {
    Actor* actor = ShotVsMyCheck(play, &this->actor, 800.0f);

    if (actor != NULL) {
        s16 angleToFacing;
        f32 dist;

        angleToFacing = Actor_search_actor_angleY(&this->actor, actor) - this->actor.shape.rot.y;
        this->actor.world.rot.y = (u16)this->actor.shape.rot.y & 0xFFFF;
        dist = Actor_search_position_distance(&this->actor, &actor->world.pos);
        //! @bug
        // Actor_search_position_distance already sqrtfs the distance, so this actually checks for a
        // distance of 360000. Also it's a double calculation because no f on sqrt.
        if ((ABS(angleToFacing) < 0x2EE0) && (sqrt(dist) < 600.0)) {
            if (actor->id == ACTOR_ARMS_HOOK) {
                mode_jump_move_init(this);
            } else {
                mode_defense_init(this);
            }
        } else {
            this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF;
            if ((ABS(angleToFacing) < 0x2000) || (ABS(angleToFacing) > 0x5FFF)) {
                mode_side_step_init(this, play);
                this->actor.speed *= 3.0f;
            } else if (ABS(angleToFacing) < 0x5FFF) {
                mode_back_jump_init(this);
            }
        }
        return true;
    }
    return false;
}
