/*
 * File: z_en_goma.c
 * Overlay: ovl_En_Goma
 * Description: Gohma Larva
 */

#include "z_en_goma.h"
#include "assets/objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "assets/objects/object_gol/object_gol.h"
#include "overlays/actors/ovl_Boss_Goma/z_boss_goma.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Goma_actor_ct(Actor* thisx, PlayState* play);
void En_Goma_actor_dt(Actor* thisx, PlayState* play);
void En_Goma_actor_move(Actor* thisx, PlayState* play);
void En_Goma_actor_draw(Actor* thisx, PlayState* play);

void mode_away(EnGoma* this, PlayState* play);
void mode_egg(EnGoma* this, PlayState* play);
void mode_egg_roof(EnGoma* this, PlayState* play);
void mode_born(EnGoma* this, PlayState* play);
static void mode_damage(EnGoma* this, PlayState* play);
static void mode_down(EnGoma* this, PlayState* play);
static void mode_after_down(EnGoma* this, PlayState* play);
static void mode_pre_atack(EnGoma* this, PlayState* play);
void mode_after_atack(EnGoma* this, PlayState* play);
static void mode_atack(EnGoma* this, PlayState* play);
static void mode_wait(EnGoma* this, PlayState* play);
static void mode_walk(EnGoma* this, PlayState* play);
static void mode_mahi(EnGoma* this, PlayState* play);
void Ko_Goma_Eye_Control(EnGoma* this, PlayState* play);
void En_Goma_Damage_check(EnGoma* this, PlayState* play);
void mode_egg_hahen(EnGoma* this, PlayState* play);
void egg_break(EnGoma* this, PlayState* play2);
void mode_boss_hahen(EnGoma* this, PlayState* play);

void mode_away_init(EnGoma* this);
void mode_born_init(EnGoma* this, PlayState* play);
static void mode_damage_init(EnGoma* this, PlayState* play);
static void mode_down_init(EnGoma* this);
void mode_after_down_init(EnGoma* this);
static void mode_wait_init(EnGoma* this);
static void mode_walk_init(EnGoma* this);
static void mode_pre_atack_init(EnGoma* this);
void mode_after_atack_init(EnGoma* this);
static void mode_atack_init(EnGoma* this);
static void mode_mahi_init(EnGoma* this, PlayState* play);

ActorProfile En_Goma_Profile = {
    /**/ ACTOR_BOSS_GOMA,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_GOL,
    /**/ sizeof(EnGoma),
    /**/ En_Goma_actor_ct,
    /**/ En_Goma_actor_dt,
    /**/ En_Goma_actor_move,
    /**/ En_Goma_actor_draw,
};

static ColliderCylinderInit EnGomaAtOcInfoData = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFDFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 15, 30, 10, { 0, 0, 0 } },
};

static ColliderCylinderInit EnGomaAcInfoData = {
    {
        COL_MATERIAL_HIT3,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFDFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 15, 30, 10, { 0, 0, 0 } },
};

static u8 en_goma_counter = 0;
static Vec3f zero = { 0.0f, 0.0f, 0.0f };

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_3, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_GOHMA_LARVA, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 20, ICHAIN_STOP),
};

void En_Goma_actor_ct(Actor* thisx, PlayState* play) {
    EnGoma* this = (EnGoma*)thisx;
    s16 params;

    this->eggTimer = fqrand() * 200.0f;
    ValueSet_process(&this->actor, value_init);
    Actor_set_scale(&this->actor, 0.01f);
    params = this->actor.params;

    if (params >= 100) { // piece of boss goma
        Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_BOSS);
        this->actionFunc = mode_boss_hahen;
        this->gomaType = ENGOMA_BOSSLIMB;
        Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 0.0f);
        this->actionTimer = this->actor.params + 150;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    } else if (params >= 10) { // Debris when hatching
        this->actor.gravity = -1.3f;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->actionTimer = 50;
        this->gomaType = ENGOMA_HATCH_DEBRIS;
        this->eggScale = 1.0f;
        this->actor.velocity.y = fqrand() * 5.0f + 5.0f;
        this->actionFunc = mode_egg_hahen;
        this->actor.speed = fqrand() * 2.3f + 1.5f;
        this->actionTimer = 30;
        this->actor.scale.x = fqrand() * 0.005f + 0.01f;
        this->actor.scale.y = fqrand() * 0.005f + 0.01f;
        this->actor.scale.z = fqrand() * 0.005f + 0.01f;
        Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 0.0f);
    } else { // Egg
        Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 40.0f);
        Skeleton_Info2_M_ct(play, &this->skelanime, &gObjectGolSkel, &gObjectGolStandAnim, this->jointTable,
                       this->morphTable, GOMA_LIMB_MAX);
        Skeleton_Info2_init_standard_repeat(&this->skelanime, &gObjectGolStandAnim);
        this->actor.colChkInfo.health = 2;

        if (this->actor.params < 3) { // Spawned by boss
            this->actionFunc = mode_egg;
            this->invincibilityTimer = 10;
            this->actor.speed = 1.5f;
        } else if (this->actor.params == 8 || this->actor.params == 6) {
            this->actionFunc = mode_egg_roof;
            this->spawnNum = en_goma_counter++;
        } else if (this->actor.params == 9 || this->actor.params == 7) {
            this->actionFunc = mode_egg_roof;
        }

        if (this->actor.params >= 8) { // on ceiling
            this->eggYOffset = -1500.0f;
        } else {
            this->eggYOffset = 1500.0f;
        }

        this->gomaType = ENGOMA_EGG;
        this->eggScale = 1.0f;
        this->eggSquishAngle = fqrand() * 1000.0f;
        this->actionTimer = 50;
        ClObjPipe_ct(play, &this->colCyl1);
        ClObjPipe_set5(play, &this->colCyl1, &this->actor, &EnGomaAtOcInfoData);
        ClObjPipe_ct(play, &this->colCyl2);
        ClObjPipe_set5(play, &this->colCyl2, &this->actor, &EnGomaAcInfoData);
    }
}

void En_Goma_actor_dt(Actor* thisx, PlayState* play) {
    EnGoma* this = (EnGoma*)thisx;

    if (this->actor.params < 10) {
        ClObjPipe_dt(play, &this->colCyl1);
        ClObjPipe_dt(play, &this->colCyl2);
    }
}

void mode_away_init(EnGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gObjectGolRunningAnim, 2.0f, 0.0f,
                     Si2_anime_end_frame(&gObjectGolRunningAnim), ANIMMODE_LOOP, -2.0f);
    this->actionFunc = mode_away;
    this->actionTimer = 20;

    if (this->actor.params < 6) {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_BJR_DAM2);
    } else {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_DAM2);
    }
}

void mode_away(EnGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);
    add_calc2(&this->actor.speed, 20.0f / 3.0f, 0.5f, 2.0f);
    adds(&this->actor.world.rot.y, Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor) + 0x8000,
                   3, 2000);
    adds(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, 3000);

    if (this->actionTimer == 0) {
        mode_wait_init(this);
    }
}

void mode_egg(EnGoma* this, PlayState* play) {
    this->actor.gravity = -1.3f;
    this->eggSquishAccel += 0.03f;
    this->eggSquishAngle += 1.0f + this->eggSquishAccel;
    add_calc0(&this->eggSquishAmount, 1.0f, 0.005f);
    add_calc2(&this->eggYOffset, 1500.0f, 1.0f, 150.0f);

    switch (this->hatchState) {
        case 0:
            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                if (this->actor.params < 6) {
                    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_BJR_EGG1);
                } else {
                    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_EGG1);
                }

                if (this->actor.params > 5) {
                    mode_born_init(this, play);
                } else {
                    this->hatchState = 1;
                    this->actionTimer = 3;
                    add_calc2(&this->eggScale, 1.5f, 0.5f, 1.0f);
                }
            }
            break;

        case 1:
            if (this->actionTimer == 0) {
                this->hatchState = 2;
                this->actionTimer = 3;
                add_calc2(&this->eggScale, 0.75f, 0.5f, 1.0f);
                this->actor.velocity.y = 5.0f;
                this->actor.speed = 2.0f;
            } else {
                add_calc2(&this->eggScale, 1.5f, 0.5f, 1.0f);
            }
            break;

        case 2:
            if (this->actionTimer == 0) {
                this->hatchState = 3;
                this->actionTimer = 80;
            } else {
                add_calc2(&this->eggScale, 0.75f, 0.5f, 1.0f);
            }
            break;

        case 3:
            add_calc2(&this->eggScale, 1.0f, 0.1f, 0.1f);
            if (this->actionTimer == 0) {
                mode_born_init(this, play);
            }
            break;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        add_calc0(&this->actor.speed, 0.2f, 0.05f);
    }
    this->eggPitch += (this->actor.speed * 0.1f);
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void mode_egg_roof(EnGoma* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 i;

    this->eggSquishAngle += 1.0f;
    add_calc2(&this->eggSquishAmount, 0.1f, 1.0f, 0.005f);
    if (fabsf(this->actor.world.pos.x - player->actor.world.pos.x) < 100.0f &&
        fabsf(this->actor.world.pos.z - player->actor.world.pos.z) < 100.0f) {
        if (++this->playerDetectionTimer > 9) {
            this->actionFunc = mode_egg;
        }
    } else {
        this->playerDetectionTimer = 0;
    }

    if (!(this->eggTimer & 0xF) && fqrand() < 0.5f) {
        for (i = 0; i < 2; i++) {
            Vec3f vel = { 0.0f, 0.0f, 0.0f };
            Vec3f acc = { 0.0f, -0.5f, 0.0f };
            Vec3f pos;

            pos.x = rnd_fx(30.0f) + this->actor.world.pos.x;
            pos.y = rnd_f(30.0f) + this->actor.world.pos.y;
            pos.z = rnd_fx(30.0f) + this->actor.world.pos.z;
            Effect_Hahen_ct3(play, &pos, &vel, &acc, 0, (s16)(fqrand() * 5.0f) + 10, HAHEN_OBJECT_DEFAULT, 10,
                                NULL);
        }
    }
}

void mode_born_init(EnGoma* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelanime, &gObjectGolJumpHeadbuttAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gObjectGolJumpHeadbuttAnim), ANIMMODE_ONCE, 0.0f);
    this->actionFunc = mode_born;
    Actor_set_scale(&this->actor, 0.005f);
    this->gomaType = ENGOMA_NORMAL;
    this->actionTimer = 5;
    this->actor.shape.rot.y = Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    egg_break(this, play);
    this->eggScale = 1.0f;
    this->actor.speed = 0.0f;
}

void mode_born(EnGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);
    if (this->actionTimer == 0) {
        mode_wait_init(this);
    }
}

static void mode_damage_init(EnGoma* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelanime, &gObjectGolDamagedAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gObjectGolDamagedAnim), ANIMMODE_ONCE, -2.0f);
    this->actionFunc = mode_damage;

    if ((s8)this->actor.colChkInfo.health <= 0) {
        this->actionTimer = 5;
        Actor_info_finish(play, &this->actor);
    } else {
        this->actionTimer = 10;
    }

    this->actor.speed = 20.0f;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer + 0x8000;
    if (this->actor.params < 6) {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_BJR_DAM1);
    } else {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_DAM1);
    }
}

static void mode_damage(EnGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        add_calc0(&this->actor.speed, 1.0f, 2.0f);
    }

    if (this->actionTimer == 0) {
        if ((s8)this->actor.colChkInfo.health <= 0) {
            mode_down_init(this);
        } else {
            mode_away_init(this);
        }
    }
}

static void mode_down_init(EnGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gObjectGolDeathAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gObjectGolDeathAnim),
                     ANIMMODE_ONCE, -2.0f);
    this->actionFunc = mode_down;
    this->actionTimer = 30;

    if (this->actor.params < 6) {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_BJR_DEAD);
    } else {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_DEAD);
    }

    this->invincibilityTimer = 100;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

static void mode_down(EnGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        add_calc0(&this->actor.speed, 1.0f, 2.0f);
    }

    if (this->actionTimer == 17) {
        if (this->actor.params < 6) {
            Actor_SE_set(&this->actor, NA_SE_EN_GOMA_BJR_LAND);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_LAND);
        }
    }

    if (this->actionTimer == 0) {
        mode_after_down_init(this);
    }
}

void mode_after_down_init(EnGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gObjectGolDeadTwitchingAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gObjectGolDeadTwitchingAnim), ANIMMODE_LOOP, -2.0f);
    this->actionFunc = mode_after_down;
    this->actionTimer = 3;
}

static void mode_after_down(EnGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);
    add_calc0(&this->actor.speed, 1.0f, 2.0f);

    if (this->actionTimer == 2) {
        Vec3f accel;
        Vec3f pos;

        pos.x = this->actor.world.pos.x;
        pos.y = (this->actor.world.pos.y + 5.0f) - 10.0f;
        pos.z = this->actor.world.pos.z;
        accel = zero;
        accel.y = 0.03f;
        Effect_k_fire_ct(play, &pos, &zero, &accel, 40, 0);
    }

    if (this->actionTimer == 0 && add_calc(&this->actor.scale.y, 0.0f, 0.5f, 0.00225f, 0.00001f) <= 0.001f) {
        if (this->actor.params < 6) {
            BossGoma* parent = (BossGoma*)this->actor.parent;

            parent->childrenGohmaState[this->actor.params] = -1;
        }
        Nai_FxFlagEntry(NA_SE_EN_EXTINCT, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        Actor_delete(&this->actor);
        Item_Set_Std(play, NULL, &this->actor.world.pos, 0x30);
    }
    this->visualState = 2;
}

static void mode_wait_init(EnGoma* this) {
    f32 lastFrame;

    lastFrame = Si2_anime_end_frame(&gObjectGolStandAnim);
    this->actionTimer = get_random_timer(10, 30);
    Skeleton_Info2_init(&this->skelanime, &gObjectGolStandAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP, -5.0f);
    this->actionFunc = mode_wait;
    this->gomaType = ENGOMA_NORMAL;
}

static void mode_walk_init(EnGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gObjectGolRunningAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gObjectGolRunningAnim), ANIMMODE_LOOP, -5.0f);
    this->actionFunc = mode_walk;
    this->actionTimer = get_random_timer(70, 110);
}

static void mode_pre_atack_init(EnGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gObjectGolPrepareJumpAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gObjectGolPrepareJumpAnim), ANIMMODE_ONCE, -5.0f);
    this->actionFunc = mode_pre_atack;
    this->actionTimer = 30;
}

static void mode_pre_atack(EnGoma* this, PlayState* play) {
    s16 targetAngle;

    Skeleton_Info2_anime_play(&this->skelanime);
    add_calc0(&this->actor.speed, 0.5f, 2.0f);

    targetAngle = Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor);
    adds(&this->actor.world.rot.y, targetAngle, 2, 4000);
    adds(&this->actor.shape.rot.y, targetAngle, 2, 3000);

    if (this->actionTimer == 0) {
        mode_atack_init(this);
    }
    this->visualState = 0;
}

void mode_after_atack_init(EnGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gObjectGolLandFromJumpAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gObjectGolLandFromJumpAnim), ANIMMODE_ONCE, 0.0f);
    this->actionFunc = mode_after_atack;
    this->actionTimer = 10;
}

void mode_after_atack(EnGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        add_calc0(&this->actor.speed, 1.0f, 2.0f);
    }
    if (this->actionTimer == 0) {
        mode_wait_init(this);
    }
}

static void mode_atack_init(EnGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gObjectGolJumpHeadbuttAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gObjectGolJumpHeadbuttAnim), ANIMMODE_ONCE, 0.0f);
    this->actionFunc = mode_atack;
    this->actor.velocity.y = 8.0f;

    if (this->actor.params < 6) {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_BJR_CRY);
    } else {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_CRY);
    }
}

static void mode_atack(EnGoma* this, PlayState* play) {
    this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
    Skeleton_Info2_anime_play(&this->skelanime);
    add_calc2(&this->actor.speed, 10.0f, 0.5f, 5.0f);

    if (this->actor.velocity.y <= 0.0f && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        mode_after_atack_init(this);
        if (this->actor.params < 6) {
            Actor_SE_set(&this->actor, NA_SE_EN_GOMA_BJR_LAND2);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_LAND2);
        }
    }
    this->visualState = 0;
}

static void mode_wait(EnGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);
    add_calc0(&this->actor.speed, 0.5f, 2.0f);
    adds(&this->actor.shape.rot.y, Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor), 2,
                   3000);

    if (this->actionTimer == 0) {
        mode_walk_init(this);
    }
}

static void mode_walk(EnGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);

    if (Skeleton_Info_frame_check(&this->skelanime, 1.0f) || Skeleton_Info_frame_check(&this->skelanime, 5.0f)) {
        if (this->actor.params < 6) {
            Actor_SE_set(&this->actor, NA_SE_EN_GOMA_BJR_WALK);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_WALK);
        }
    }

    add_calc2(&this->actor.speed, 10.0f / 3.0f, 0.5f, 2.0f);
    adds(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 3, 2000);
    adds(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, 3000);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.velocity.y = 0.0f;
    }
    if (this->actor.xzDistToPlayer <= 150.0f) {
        mode_pre_atack_init(this);
    }
}

static void mode_mahi_init(EnGoma* this, PlayState* play) {
    this->actionFunc = mode_mahi;
    this->stunTimer = 100;
    Skeleton_Info2_init_standard_repeat_morf(&this->skelanime, &gObjectGolStandAnim, -5.0f);
    this->actionTimer = (s16)rnd_f(15.0f) + 3;

    if (this->actor.params < 6) {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_BJR_FREEZE);
    } else {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    }
}

static void mode_mahi(EnGoma* this, PlayState* play) {
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 180, COLORFILTER_BUFFLAG_OPA, 2);
    this->visualState = 2;

    if (this->actionTimer != 0) {
        Skeleton_Info2_anime_play(&this->skelanime);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.velocity.y = 0.0f;
        add_calc0(&this->actor.speed, 0.5f, 2.0f);
    }

    if (this->stunTimer == 0) {
        mode_wait_init(this);
    } else if (--this->stunTimer < 30) {
        if (this->stunTimer & 1) {
            this->actor.world.pos.x += 1.5f;
            this->actor.world.pos.z += 1.5f;
        } else {
            this->actor.world.pos.x -= 1.5f;
            this->actor.world.pos.z -= 1.5f;
        }
    }
}

void Ko_Goma_Eye_Control(EnGoma* this, PlayState* play) {
    s16 eyePitch;
    s16 eyeYaw;

    eyeYaw = Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor) - this->actor.shape.rot.y;
    eyePitch = Actor_search_actor_angleX(&this->actor, &GET_PLAYER(play)->actor) - this->actor.shape.rot.x;

    if (eyeYaw > 6000) {
        eyeYaw = 6000;
    }
    if (eyeYaw < -6000) {
        eyeYaw = -6000;
    }
    adds(&this->eyeYaw, eyeYaw, 3, 2000);
    adds(&this->eyePitch, eyePitch, 3, 2000);
}

void En_Goma_Damage_check(EnGoma* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->hurtTimer != 0) {
        this->hurtTimer--;
    } else {
        ColliderElement* acHitElem;
        u8 swordDamage;

        if ((this->colCyl1.base.atFlags & AT_HIT) && this->actionFunc == mode_atack) {
            mode_after_atack_init(this);
            this->actor.speed = 0.0f;
            this->actor.velocity.y = 0.0f;
        }

        if ((this->colCyl2.base.acFlags & AC_HIT) && (s8)this->actor.colChkInfo.health > 0) {
            acHitElem = this->colCyl2.elem.acHitElem;
            this->colCyl2.base.acFlags &= ~AC_HIT;

            if (this->gomaType == ENGOMA_NORMAL) {
                u32 dmgFlags = acHitElem->atDmgInfo.dmgFlags;

                if (dmgFlags & DMG_SHIELD) {
                    if (this->actionFunc == mode_atack) {
                        mode_after_atack_init(this);
                        this->actor.velocity.y = 0.0f;
                        this->actor.speed = -5.0f;
                    } else {
                        static Vec3f mae = { 0.0f, 0.0f, 20.0f };

                        Matrix_rotateY(BINANG_TO_RAD_ALT(player->actor.shape.rot.y), MTXMODE_NEW);
                        Matrix_Position(&mae, &this->shieldKnockbackVel);
                        this->invincibilityTimer = 5;
                    }
                } else if (dmgFlags & DMG_DEKU_NUT) { // stun
                    if (this->actionFunc != mode_mahi) {
                        mode_mahi_init(this, play);
                        this->hurtTimer = 8;
                    }
                } else {
                    swordDamage = GetSwordAP(dmgFlags);

                    if (swordDamage != 0) {
                        DamageEffectSibukiSet(play, &this->actor.focus.pos);
                    } else {
                        swordDamage = 1;
                    }

                    this->actor.colChkInfo.health -= swordDamage;
                    mode_damage_init(this, play);
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 5);
                    this->hurtTimer = 13;
                }
            } else {
                // die if still an egg
                if (this->actor.params <= 5) { //! BossGoma only has 3 children
                    BossGoma* parent = (BossGoma*)this->actor.parent;

                    parent->childrenGohmaState[this->actor.params] = -1;
                }

                egg_break(this, play);
                Actor_delete(&this->actor);
            }
        }
    }
}

void Ko_Goma_Col_Control(EnGoma* this) {
    static f32 gol_col_r[][3] = {
        { 255.0f, 0.0f, 50.0f },
        { 17.0f, 255.0f, 50.0f },
        { 0.0f, 170.0f, 50.0f },
    };

    add_calc2(&this->eyeEnvColor[0], gol_col_r[0][this->visualState], 0.5f, 20.0f);
    add_calc2(&this->eyeEnvColor[1], gol_col_r[1][this->visualState], 0.5f, 20.0f);
    add_calc2(&this->eyeEnvColor[2], gol_col_r[2][this->visualState], 0.5f, 20.0f);
}

static void ground_angle_calc(EnGoma* this) {
    f32 nx;
    f32 ny;
    f32 nz;

    if (this->actor.floorPoly != NULL) {
        nx = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.x);
        ny = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.y);
        nz = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.z);
        adds(&this->slopePitch, RAD_TO_BINANG(-fatan2(-nz * ny, 1.0f)), 1, 1000);
        adds(&this->slopeRoll, RAD_TO_BINANG(fatan2(-nx * ny, 1.0f)), 1, 1000);
    }
}

void En_Goma_actor_move(Actor* thisx, PlayState* play) {
    EnGoma* this = (EnGoma*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);

    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    if (this->invincibilityTimer != 0) {
        this->invincibilityTimer--;
    }

    this->actionFunc(this, play);
    Actor_position_moveF(&this->actor);
    this->actor.world.pos.x += this->shieldKnockbackVel.x;
    this->actor.world.pos.z += this->shieldKnockbackVel.z;
    add_calc0(&this->shieldKnockbackVel.x, 1.0f, 3.0f);
    add_calc0(&this->shieldKnockbackVel.z, 1.0f, 3.0f);

    if (this->actor.params < 10) {
        this->eggTimer++;
        add_calc(&this->actor.scale.x, 0.01f, 0.5f, 0.00075f, 0.000001f);
        add_calc(&this->actor.scale.y, 0.01f, 0.5f, 0.00075f, 0.000001f);
        add_calc(&this->actor.scale.z, 0.01f, 0.5f, 0.00075f, 0.000001f);
        En_Goma_Damage_check(this, play);
        Actor_BGcheck2(play, &this->actor, 50.0f, 50.0f, 100.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
        ground_angle_calc(this);
        Actor_world_to_eye(&this->actor, 20.0f);
        Ko_Goma_Eye_Control(this, play);
        Ko_Goma_Col_Control(this);
        this->visualState = 1;
        if (player->meleeWeaponState != 0) {
            this->colCyl2.dim.radius = 35;
            this->colCyl2.dim.height = 35;
            this->colCyl2.dim.yShift = 0;
        } else {
            this->colCyl2.dim.radius = 15;
            this->colCyl2.dim.height = 30;
            this->colCyl2.dim.yShift = 10;
        }
        if (this->invincibilityTimer == 0) {
            CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colCyl1);
            CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colCyl2);
            CollisionCheck_setOC(play, &play->colChkCtx, &this->colCyl1.base);
            CollisionCheck_setAC(play, &play->colChkCtx, &this->colCyl2.base);
            CollisionCheck_setAT(play, &play->colChkCtx, &this->colCyl1.base);
        }
    }
}

s32 En_Goma_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnGoma* this = (EnGoma*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_goma.c", 1976);
    gDPSetEnvColor(POLY_OPA_DISP++, (s16)this->eyeEnvColor[0], (s16)this->eyeEnvColor[1], (s16)this->eyeEnvColor[2],
                   255);

    if (limbIndex == GOMA_LIMB_EYE_IRIS_ROOT1) {
        if (1) {}
        rot->x += this->eyePitch;
        rot->y += this->eyeYaw;
    } else if (limbIndex == GOMA_LIMB_BODY && this->hurtTimer != 0) {
        gDPSetEnvColor(POLY_OPA_DISP++, (s16)(fqrand() * 255.0f), (s16)(fqrand() * 255.0f),
                       (s16)(fqrand() * 255.0f), 255);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_goma.c", 2011);
    return 0;
}

Gfx* kogoma_mode_2(GraphicsContext* gfxCtx) {
    Gfx* dListHead;
    Gfx* dList;

    dListHead = dList = GRAPH_ALLOC(gfxCtx, sizeof(Gfx) * 4);
    gDPPipeSync(dListHead++);
    gDPSetRenderMode(dListHead++, G_RM_PASS, G_RM_AA_ZB_TEX_EDGE2);
    gSPClearGeometryMode(dListHead++, G_CULL_BACK);
    gSPEndDisplayList(dListHead++);
    return dList;
}

void En_Goma_actor_draw(Actor* thisx, PlayState* play) {
    EnGoma* this = (EnGoma*)thisx;
    s32 y;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_goma.c", 2040);
    _texture_z_light_fog_prim(play->state.gfxCtx);

    switch (this->gomaType) {
        case ENGOMA_NORMAL:
            this->actor.naviEnemyId = NAVI_ENEMY_GOHMA_LARVA;
            Matrix_translate(this->actor.world.pos.x,
                             this->actor.world.pos.y +
                                 ((this->actor.shape.yOffset * this->actor.scale.y) + play->mainCamera.quakeOffset.y),
                             this->actor.world.pos.z, MTXMODE_NEW);
            Matrix_rotateX(BINANG_TO_RAD_ALT(this->slopePitch), MTXMODE_APPLY);
            Matrix_rotateZ(BINANG_TO_RAD_ALT(this->slopeRoll), MTXMODE_APPLY);
            Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.shape.rot.y), MTXMODE_APPLY);
            Matrix_rotateX(BINANG_TO_RAD_ALT(this->actor.shape.rot.x), MTXMODE_APPLY);
            Matrix_rotateZ(BINANG_TO_RAD_ALT(this->actor.shape.rot.z), MTXMODE_APPLY);
            Matrix_scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Si2_draw(play, this->skelanime.skeleton, this->skelanime.jointTable, En_Goma_draw_sub, NULL,
                              this);
            break;

        case ENGOMA_EGG:
            this->actor.naviEnemyId = NAVI_ENEMY_GOHMA_EGG;
            y = (s16)(sinf((this->eggTimer * 5.0f * 3.1415f) / 180.0f) * 31.9f);
            y = (s16)(y + 31);
            gSPSegment(POLY_OPA_DISP++, 0x08, tex_scroll(play->state.gfxCtx, 0, y));
            Matrix_push();
            Matrix_scale(this->eggScale, 1.0f / this->eggScale, this->eggScale, MTXMODE_APPLY);
            Matrix_rotateY(this->eggSquishAngle * 0.15f, MTXMODE_APPLY);
            Matrix_rotateZ(this->eggSquishAngle * 0.1f, MTXMODE_APPLY);
            Matrix_scale(0.95f - this->eggSquishAmount, this->eggSquishAmount + 1.05f, 0.95f - this->eggSquishAmount,
                         MTXMODE_APPLY);
            Matrix_rotateZ(-(this->eggSquishAngle * 0.1f), MTXMODE_APPLY);
            Matrix_rotateY(-(this->eggSquishAngle * 0.15f), MTXMODE_APPLY);
            Matrix_translate(0.0f, this->eggYOffset, 0.0f, MTXMODE_APPLY);
            Matrix_rotateX(this->eggPitch, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_goma.c", 2101);
            gSPDisplayList(POLY_OPA_DISP++, gObjectGolEggDL);
            Matrix_pull();
            break;

        case ENGOMA_HATCH_DEBRIS:
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_goma.c", 2107);
            gSPDisplayList(POLY_OPA_DISP++, gBrownFragmentDL);
            break;

        case ENGOMA_BOSSLIMB:
            if (this->bossLimbDL != NULL) {
                gSPSegment(POLY_OPA_DISP++, 0x08, kogoma_mode_2(play->state.gfxCtx));
                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_goma.c", 2114);
                gSPDisplayList(POLY_OPA_DISP++, this->bossLimbDL);
            }
            break;
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_goma.c", 2119);
}

void mode_egg_hahen(EnGoma* this, PlayState* play) {
    this->actor.shape.rot.y += 2500;
    this->actor.shape.rot.x += 3500;
    if (this->actionTimer == 0) {
        Actor_delete(&this->actor);
    }
}

void egg_break(EnGoma* this, PlayState* play2) {
    PlayState* play = play2;
    s16 i;

    if (this->actor.params < 6) {
        Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EN_GOMA_BJR_EGG2);
    } else {
        Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EN_GOMA_EGG2);
    }

    for (i = 0; i < 15; i++) {
        Actor_info_make_child_actor(
            &play->actorCtx, &this->actor, play, ACTOR_EN_GOMA, rnd_fx(10.0f) + this->actor.world.pos.x,
            rnd_fx(10.0f) + this->actor.world.pos.y + 15.0f,
            rnd_fx(10.0f) + this->actor.world.pos.z, 0, rnd_fx(0x10000 - 0.01f), 0, i + 10);
    }
}

void mode_boss_hahen(EnGoma* this, PlayState* play) {
    Vec3f vel = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 1.0f, 0.0f };
    Color_RGBA8 primColor = { 255, 255, 255, 255 };
    Color_RGBA8 envColor = { 0, 100, 255, 255 };
    Vec3f pos;

    this->actor.world.pos.y -= 5.0f;
    Actor_BGcheck2(play, &this->actor, 50.0f, 50.0f, 100.0f, UPDBGCHECKINFO_FLAG_2);
    this->actor.world.pos.y += 5.0f;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.velocity.y = 0.0f;
    } else if (this->actionTimer < 250) {
        this->actor.shape.rot.y += 2000;
    }

    if (this->actionTimer == 250) {
        this->actor.gravity = -1.0f;
    }

    if (this->actionTimer < 121) {
        if (add_calc(&this->actor.scale.y, 0.0f, 1.0f, 0.00075f, 0) <= 0.001f) {
            Actor_delete(&this->actor);
        }
        this->actor.scale.x = this->actor.scale.z = this->actor.scale.y;
    }

    if (this->actionTimer % 8 == 0 && this->actionTimer != 0) {
        pos.x = rnd_fx(20.0f) + this->actor.world.pos.x;
        pos.y = rnd_fx(10.0f) + this->actor.world.pos.y;
        pos.z = rnd_fx(20.0f) + this->actor.world.pos.z;
        Effect_SS_Dust_sc_cl_co_ct(play, &pos, &vel, &accel, &primColor, &envColor, 500, 10, 10);
    }
}
