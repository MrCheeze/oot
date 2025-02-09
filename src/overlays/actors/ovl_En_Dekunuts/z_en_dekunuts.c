/*
 * File: z_en_dekunuts.c
 * Overlay: ovl_En_Dekunuts
 * Description: Mad Scrub
 */

#include "z_en_dekunuts.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "assets/objects/object_dekunuts/object_dekunuts.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

#define DEKUNUTS_FLOWER 10

void En_Dekunuts_actor_ct(Actor* thisx, PlayState* play);
void En_Dekunuts_actor_dt(Actor* thisx, PlayState* play);
void En_Dekunuts_actor_move(Actor* thisx, PlayState* play);
void En_Dekunuts_actor_draw(Actor* thisx, PlayState* play);

static void mode_head_up_init(EnDekunuts* this);
static void mode_head_up(EnDekunuts* this, PlayState* play);
static void mode_look_around(EnDekunuts* this, PlayState* play);
static void mode_attack_wait(EnDekunuts* this, PlayState* play);
static void mode_attack(EnDekunuts* this, PlayState* play);
static void mode_head_down(EnDekunuts* this, PlayState* play);
static void mode_jump(EnDekunuts* this, PlayState* play);
static void mode_escape(EnDekunuts* this, PlayState* play);
void mode_rest(EnDekunuts* this, PlayState* play);
static void mode_damage(EnDekunuts* this, PlayState* play);
static void mode_stop(EnDekunuts* this, PlayState* play);
static void mode_down(EnDekunuts* this, PlayState* play);

ActorProfile En_Dekunuts_Profile = {
    /**/ ACTOR_EN_DEKUNUTS,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_DEKUNUTS,
    /**/ sizeof(EnDekunuts),
    /**/ En_Dekunuts_actor_ct,
    /**/ En_Dekunuts_actor_dt,
    /**/ En_Dekunuts_actor_move,
    /**/ En_Dekunuts_actor_draw,
};

static ColliderCylinderInit DekunutsAcOcPipeData = {
    {
        COL_MATERIAL_HIT6,
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
    { 18, 32, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit DekunutsStatusData = { 1, 18, 32, MASS_IMMOVABLE };

static DamageTable DekunutsBtlData = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(1, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(4, 0x2),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(4, 0x2),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_MAD_SCRUB, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2600, ICHAIN_STOP),
};

void En_Dekunuts_actor_ct(Actor* thisx, PlayState* play) {
    EnDekunuts* this = (EnDekunuts*)thisx;
    s32 pad;

    ValueSet_process(&this->actor, value_init);
    if (thisx->params == DEKUNUTS_FLOWER) {
        thisx->flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
    } else {
        Shape_Info_init(&thisx->shape, 0.0f, Actor_shadow_circle, 35.0f);
        Skeleton_Info2_M_ct(play, &this->skelAnime, &gDekuNutsSkel, &gDekuNutsStandAnim, this->jointTable, this->morphTable,
                       25);
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, &this->actor, &DekunutsAcOcPipeData);
        CollisionCheck_Status_set2(&thisx->colChkInfo, &DekunutsBtlData, &DekunutsStatusData);
        this->shotsPerRound = PARAMS_GET_U(thisx->params, 8, 8);
        thisx->params &= 0xFF;
        if ((this->shotsPerRound == 0xFF) || (this->shotsPerRound == 0)) {
            this->shotsPerRound = 1;
        }
        mode_head_up_init(this);
        Actor_info_make_child_actor(&play->actorCtx, thisx, play, ACTOR_EN_DEKUNUTS, thisx->world.pos.x, thisx->world.pos.y,
                           thisx->world.pos.z, 0, thisx->world.rot.y, 0, DEKUNUTS_FLOWER);
    }
}

void En_Dekunuts_actor_dt(Actor* thisx, PlayState* play) {
    EnDekunuts* this = (EnDekunuts*)thisx;

    if (this->actor.params != DEKUNUTS_FLOWER) {
        ClObjPipe_dt(play, &this->collider);
    }
}

static void mode_head_up_init(EnDekunuts* this) {
    Skeleton_Info2_init_standard_speedset_stop(&this->skelAnime, &gDekuNutsUpAnim, 0.0f);
    this->animFlagAndTimer = get_random_timer(100, 50);
    this->collider.dim.height = 5;
    xyz_t_move(&this->actor.world.pos, &this->actor.home.pos);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = mode_head_up;
}

static void mode_look_around_init(EnDekunuts* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gDekuNutsLookAroundAnim);
    this->animFlagAndTimer = 2;
    this->actionFunc = mode_look_around;
}

static void mode_attack_init(EnDekunuts* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gDekuNutsSpitAnim);
    this->animFlagAndTimer = this->shotsPerRound;
    this->actionFunc = mode_attack;
}

static void mode_attack_wait_init(EnDekunuts* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gDekuNutsStandAnim, -3.0f);
    if (this->actionFunc == mode_attack) {
        this->animFlagAndTimer = 2 | 0x1000; // sets timer and flag
    } else {
        this->animFlagAndTimer = 1;
    }
    this->actionFunc = mode_attack_wait;
}

static void mode_head_down_init(EnDekunuts* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gDekuNutsBurrowAnim, -5.0f);
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_DOWN);
    this->actionFunc = mode_head_down;
}

static void mode_jump_init(EnDekunuts* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gDekuNutsUnburrowAnim, -3.0f);
    this->collider.dim.height = 37;
    this->actor.colChkInfo.mass = 50;
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_DAMAGE);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = mode_jump;
}

static void mode_escape_init(EnDekunuts* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gDekuNutsRunAnim);
    this->animFlagAndTimer = 2;
    this->playWalkSfx = false;
    this->collider.base.acFlags |= AC_ON;
    this->actionFunc = mode_escape;
}

void mode_rest_init(EnDekunuts* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gDekuNutsGaspAnim);
    this->animFlagAndTimer = 3;
    this->actor.speed = 0.0f;
    if (this->runAwayCount != 0) {
        this->runAwayCount--;
    }
    this->actionFunc = mode_rest;
}

static void mode_damage_init(EnDekunuts* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gDekuNutsDamageAnim, -3.0f);
    if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & (DMG_ARROW | DMG_SLINGSHOT)) {
        this->actor.world.rot.y = this->collider.base.ac->world.rot.y;
    } else {
        this->actor.world.rot.y = Actor_search_actor_angleY(&this->actor, this->collider.base.ac) + 0x8000;
    }
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = mode_damage;
    this->actor.speed = 10.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_DAMAGE);
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_CUTBODY);
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA,
                         Si2_anime_end_frame(&gDekuNutsDamageAnim));
}

static void mode_stop_init(EnDekunuts* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gDekuNutsDamageAnim, -3.0f);
    this->animFlagAndTimer = 5;
    this->actionFunc = mode_stop;
    this->actor.speed = 0.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA,
                         Si2_anime_end_frame(&gDekuNutsDamageAnim) * this->animFlagAndTimer);
}

static void mode_down_init(EnDekunuts* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gDekuNutsDieAnim);
    this->actionFunc = mode_down;
    this->actor.speed = 0.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_DEAD);
}

static void mode_head_up(EnDekunuts* this, PlayState* play) {
    s32 hasSlowPlaybackSpeed = false;

    if (this->skelAnime.playSpeed < 0.5f) {
        hasSlowPlaybackSpeed = true;
    }
    if (hasSlowPlaybackSpeed && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 9.0f)) {
        this->collider.base.acFlags |= AC_ON;
    } else if (Skeleton_Info_frame_check(&this->skelAnime, 8.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_UP);
    }

    this->collider.dim.height = ((CLAMP(this->skelAnime.curFrame, 9.0f, 12.0f) - 9.0f) * 9.0f) + 5.0f;
    if (!hasSlowPlaybackSpeed && (this->actor.xzDistToPlayer < 120.0f)) {
        mode_head_down_init(this);
    } else if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->actor.xzDistToPlayer < 120.0f) {
            mode_head_down_init(this);
        } else if ((this->animFlagAndTimer == 0) && (this->actor.xzDistToPlayer > 320.0f)) {
            mode_look_around_init(this);
        } else {
            mode_attack_wait_init(this);
        }
    }
    if (hasSlowPlaybackSpeed &&
        ((this->actor.xzDistToPlayer > 160.0f) && (fabsf(this->actor.yDistToPlayer) < 120.0f)) &&
        ((this->animFlagAndTimer == 0) || (this->actor.xzDistToPlayer < 480.0f))) {
        this->skelAnime.playSpeed = 1.0f;
    }
}

static void mode_look_around(EnDekunuts* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }
    if ((this->actor.xzDistToPlayer < 120.0f) || (this->animFlagAndTimer == 0)) {
        mode_head_down_init(this);
    }
}

static void mode_attack_wait(EnDekunuts* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }
    if (!(this->animFlagAndTimer & 0x1000)) {
        adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    }
    if (this->animFlagAndTimer == 0x1000) {
        if ((this->actor.xzDistToPlayer > 480.0f) || (this->actor.xzDistToPlayer < 120.0f)) {
            mode_head_down_init(this);
        } else {
            mode_attack_init(this);
        }
    } else if (this->animFlagAndTimer == 0) {
        mode_attack_init(this);
    }
}

static void mode_attack(EnDekunuts* this, PlayState* play) {
    Vec3f spawnPos;

    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_attack_wait_init(this);
    } else if (Skeleton_Info_frame_check(&this->skelAnime, 6.0f)) {
        spawnPos.x = this->actor.world.pos.x + (sin_s(this->actor.shape.rot.y) * 23.0f);
        spawnPos.y = this->actor.world.pos.y + 12.0f;
        spawnPos.z = this->actor.world.pos.z + (cos_s(this->actor.shape.rot.y) * 23.0f);
        if (Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_NUTSBALL, spawnPos.x, spawnPos.y, spawnPos.z,
                        this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z, 0) != NULL) {
            Actor_SE_set(&this->actor, NA_SE_EN_NUTS_THROW);
        }
    } else if ((this->animFlagAndTimer > 1) && Skeleton_Info_frame_check(&this->skelAnime, 12.0f)) {
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gDekuNutsSpitAnim, -3.0f);
        if (this->animFlagAndTimer != 0) {
            this->animFlagAndTimer--;
        }
    }
}

static void mode_head_down(EnDekunuts* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_head_up_init(this);
    } else {
        this->collider.dim.height = ((3.0f - CLAMP(this->skelAnime.curFrame, 1.0f, 3.0f)) * 12.0f) + 5.0f;
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 4.0f)) {
        this->collider.base.acFlags &= ~AC_ON;
    }
    add_calc2(&this->actor.world.pos.x, this->actor.home.pos.x, 0.5f, 3.0f);
    add_calc2(&this->actor.world.pos.z, this->actor.home.pos.z, 0.5f, 3.0f);
}

static void mode_jump(EnDekunuts* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->runDirection = this->actor.yawTowardsPlayer + 0x8000;
        this->runAwayCount = 3;
        mode_escape_init(this);
    }
    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
}

static void mode_escape(EnDekunuts* this, PlayState* play) {
    s16 diffRotInit;
    s16 diffRot;
    f32 phi_f0;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }
    if (this->playWalkSfx) {
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_WALK);
        this->playWalkSfx = false;
    } else {
        this->playWalkSfx = true;
    }

    chase_f(&this->actor.speed, 7.5f, 1.0f);
    if (add_calc_short_angle2(&this->actor.world.rot.y, this->runDirection, 1, 0xE38, 0xB6) == 0) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
            this->runDirection = Actor_search_position_angleY(&this->actor, &this->actor.home.pos);
        } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            this->runDirection = this->actor.wallYaw;
        } else if (this->runAwayCount == 0) {
            diffRotInit = Actor_search_position_angleY(&this->actor, &this->actor.home.pos);
            diffRot = diffRotInit - this->actor.yawTowardsPlayer;
            if (ABS(diffRot) > 0x2000) {
                this->runDirection = diffRotInit;
            } else {
                phi_f0 = (diffRot >= 0.0f) ? 1.0f : -1.0f;
                this->runDirection = (phi_f0 * -0x2000) + this->actor.yawTowardsPlayer;
            }
        } else {
            this->runDirection = this->actor.yawTowardsPlayer + 0x8000;
        }
    }

    this->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
    if ((this->runAwayCount == 0) && Actor_search_position_distanceXZ(&this->actor, &this->actor.home.pos) < 20.0f &&
        fabsf(this->actor.world.pos.y - this->actor.home.pos.y) < 2.0f) {
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        this->actor.speed = 0.0f;
        mode_head_down_init(this);
    } else if (this->animFlagAndTimer == 0) {
        mode_rest_init(this);
    }
}

void mode_rest(EnDekunuts* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }
    if (this->animFlagAndTimer == 0) {
        mode_escape_init(this);
    }
}

static void mode_damage(EnDekunuts* this, PlayState* play) {
    chase_f(&this->actor.speed, 0.0f, 1.0f);
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_down_init(this);
    }
}

static void mode_stop(EnDekunuts* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f)) {
        if (this->animFlagAndTimer != 0) {
            this->animFlagAndTimer--;
        }
        if (this->animFlagAndTimer == 0) {
            mode_escape_init(this);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_NUTS_FAINT);
        }
    }
}

static void mode_down(EnDekunuts* this, PlayState* play) {
    static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };

    s32 pad;
    Vec3f effectPos;

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        effectPos.x = this->actor.world.pos.x;
        effectPos.y = this->actor.world.pos.y + 18.0f;
        effectPos.z = this->actor.world.pos.z;
        _Effect_SS_Db_ct(play, &effectPos, &zero_vec, &zero_vec, 200, 0, 255, 255, 255, 255, 150,
                             150, 150, 1, 13, 1);
        effectPos.y = this->actor.world.pos.y + 10.0f;
        Effect_Hahen_Kakusan_ct3(play, &effectPos, 3.0f, 0, 12, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x30);
        if (this->actor.child != NULL) {
            Actor_info_part_chg(play, &play->actorCtx, this->actor.child, ACTORCAT_PROP);
        }
        Actor_delete(&this->actor);
    }
}

void En_Dekunuts_damage_proc(EnDekunuts* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Hit_bit_set(&this->actor, &this->collider.elem, true);
        if (this->actor.colChkInfo.mass == 50) {
            if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
                if (this->actor.colChkInfo.damageEffect != 1) {
                    if (this->actor.colChkInfo.damageEffect == 2) {
                        Effect_Fcircle_ct(play, &this->actor, &this->actor.world.pos, 40, 50);
                    }
                    mode_damage_init(this);
                    if (hp_down(&this->actor) == 0) {
                        Actor_info_finish(play, &this->actor);
                    }
                } else if (this->actionFunc != mode_stop) {
                    mode_stop_init(this);
                }
            }
        } else {
            mode_jump_init(this);
        }
    } else if ((this->actor.colChkInfo.mass == MASS_IMMOVABLE) && (play->actorCtx.unk_02 != 0)) {
        mode_jump_init(this);
    }
}

void En_Dekunuts_actor_move(Actor* thisx, PlayState* play) {
    EnDekunuts* this = (EnDekunuts*)thisx;
    s32 pad;

    if (this->actor.params != DEKUNUTS_FLOWER) {
        En_Dekunuts_damage_proc(this, play);
        this->actionFunc(this, play);
        Actor_position_moveF(&this->actor);
        Actor_BGcheck2(play, &this->actor, 20.0f, this->collider.dim.radius, this->collider.dim.height,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                    UPDBGCHECKINFO_FLAG_4);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        if (this->collider.base.acFlags & AC_ON) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        if (this->actionFunc == mode_head_up) {
            Actor_world_to_eye(&this->actor, this->skelAnime.curFrame);
        } else if (this->actionFunc == mode_head_down) {
            Actor_world_to_eye(&this->actor,
                           20.0f - ((this->skelAnime.curFrame * 20.0f) / Si2_anime_end_frame(&gDekuNutsBurrowAnim)));
        } else {
            Actor_world_to_eye(&this->actor, 20.0f);
        }
    }
}

s32 en_dekunuts_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnDekunuts* this = (EnDekunuts*)thisx;
    f32 x;
    f32 y;
    f32 z;
    f32 curFrame;

    if ((limbIndex == 7) && (this->actionFunc == mode_attack)) {
        curFrame = this->skelAnime.curFrame;
        if (curFrame <= 6.0f) {
            x = 1.0f - (curFrame * 0.0833f);
            z = 1.0f + (curFrame * 0.1167f);
            y = 1.0f + (curFrame * 0.1167f);
        } else if (curFrame <= 7.0f) {
            curFrame -= 6.0f;
            x = 0.5f + curFrame;
            z = 1.7f - (curFrame * 0.7f);
            y = 1.7f - (curFrame * 0.7f);
        } else if (curFrame <= 10.0f) {
            x = 1.5f - ((curFrame - 7.0f) * 0.1667f);
            z = 1.0f;
            y = 1.0f;
        } else {
            return false;
        }
        Matrix_scale(x, y, z, MTXMODE_APPLY);
    }
    return false;
}

void En_Dekunuts_actor_draw(Actor* thisx, PlayState* play) {
    EnDekunuts* this = (EnDekunuts*)thisx;

    if (this->actor.params == DEKUNUTS_FLOWER) {
        Cheap_gfx_display(play, gDekuNutsFlowerDL);
    } else {
        Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, en_dekunuts_display1, NULL,
                          this);
    }
}
