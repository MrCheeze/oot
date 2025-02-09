/*
 * File: z_en_firefly.c
 * Overlay: ovl_En_Firefly
 * Description: Keese (Normal, Fire, Ice)
 */

#include "z_en_firefly.h"
#include "versions.h"
#include "assets/objects/object_firefly/object_firefly.h"
#include "overlays/actors/ovl_Obj_Syokudai/z_obj_syokudai.h"

#define FLAGS \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_IGNORE_QUAKE | ACTOR_FLAG_CAN_ATTACH_TO_ARROW)

void En_Firefly_actor_ct(Actor* thisx, PlayState* play);
void En_Firefly_actor_dt(Actor* thisx, PlayState* play);
void En_Firefly_actor_move(Actor* thisx, PlayState* play2);
void En_Firefly_actor_draw(Actor* thisx, PlayState* play);

void En_Firefly_actor_draw_xlu(Actor* thisx, PlayState* play);

static void mode_fly(EnFirefly* this, PlayState* play);
static void mode_damage(EnFirefly* this, PlayState* play);
static void mode_down(EnFirefly* this, PlayState* play);
static void mode_attack(EnFirefly* this, PlayState* play);
static void mode_reverse(EnFirefly* this, PlayState* play);
static void mode_escape(EnFirefly* this, PlayState* play);
static void mode_stop(EnFirefly* this, PlayState* play);
static void mode_drop(EnFirefly* this, PlayState* play);
static void mode_wait(EnFirefly* this, PlayState* play);
static void mode_start(EnFirefly* this, PlayState* play);

typedef enum KeeseAuraType {
    /* 0 */ KEESE_AURA_NONE,
    /* 1 */ KEESE_AURA_FIRE,
    /* 2 */ KEESE_AURA_ICE
} KeeseAuraType;

ActorProfile En_Firefly_Profile = {
    /**/ ACTOR_EN_FIREFLY,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_FIREFLY,
    /**/ sizeof(EnFirefly),
    /**/ En_Firefly_actor_ct,
    /**/ En_Firefly_actor_dt,
    /**/ En_Firefly_actor_move,
    /**/ En_Firefly_actor_draw,
};

static ColliderJntSphElementInit FireflyAllJntSphElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x01, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 1000, 0 }, 15 }, 100 },
    },
};

static ColliderJntSphInit FireflyAllJntSphData = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    FireflyAllJntSphElemData,
};

static CollisionCheckInfoInit FireflyStatusData = { 1, 10, 10, 30 };

static DamageTable FireflyBtlData = {
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
    /* Fire arrow    */ DMG_ENTRY(2, 0xF),
    /* Ice arrow     */ DMG_ENTRY(4, 0x3),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x2),
    /* Ice magic     */ DMG_ENTRY(4, 0x3),
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
    ICHAIN_VEC3F_DIV1000(scale, 5, ICHAIN_CONTINUE),  ICHAIN_F32_DIV1000(gravity, -500, ICHAIN_CONTINUE),
    ICHAIN_F32(minVelocityY, -4, ICHAIN_CONTINUE),    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 4000, ICHAIN_STOP),
};

void firefly_to_shadowfly(EnFirefly* this) {
    this->actor.params += 2;
    this->collider.elements[0].base.atDmgInfo.effect = 0; // None
    this->auraType = KEESE_AURA_NONE;
    this->onFire = false;
    this->actor.naviEnemyId = NAVI_ENEMY_KEESE;
}

void shadowfly_to_firefly(EnFirefly* this) {
    if (this->actor.params == KEESE_ICE_FLY) {
        this->actor.params = KEESE_FIRE_FLY;
    } else {
        this->actor.params -= 2;
    }
    this->collider.elements[0].base.atDmgInfo.effect = 1; // Fire
    this->auraType = KEESE_AURA_FIRE;
    this->onFire = true;
    this->actor.naviEnemyId = NAVI_ENEMY_FIRE_KEESE;
}

void En_Firefly_actor_ct(Actor* thisx, PlayState* play) {
    EnFirefly* this = (EnFirefly*)thisx;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 25.0f);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gKeeseSkeleton, &gKeeseFlyAnim, this->jointTable, this->morphTable, 28);
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &FireflyAllJntSphData, this->colliderItems);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, &FireflyBtlData, &FireflyStatusData);

    if (PARAMS_GET_NOSHIFT(this->actor.params, 15, 1) != 0) {
        this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
        if (1) {}
        this->actor.draw = En_Firefly_actor_draw_xlu;
        this->actor.params &= 0x7FFF;
    }

    if (this->actor.params <= KEESE_FIRE_PERCH) {
        this->onFire = true;
    } else {
        this->onFire = false;
    }

    if (this->onFire) {
        this->actionFunc = mode_fly;
        this->timer = get_random_timer(20, 60);
        this->actor.shape.rot.x = 0x1554;
        this->auraType = KEESE_AURA_FIRE;
        this->actor.naviEnemyId = NAVI_ENEMY_FIRE_KEESE;
        this->maxAltitude = this->actor.home.pos.y;
    } else {
        if (this->actor.params == KEESE_NORMAL_PERCH) {
            this->actionFunc = mode_wait;
        } else {
            this->actionFunc = mode_fly;
        }

        if (this->actor.params == KEESE_ICE_FLY) {
            this->collider.elements[0].base.atDmgInfo.effect = 2; // Ice
            this->actor.naviEnemyId = NAVI_ENEMY_ICE_KEESE;
        } else {
            this->collider.elements[0].base.atDmgInfo.effect = 0; // Nothing
            this->actor.naviEnemyId = NAVI_ENEMY_KEESE;
        }

        this->maxAltitude = this->actor.home.pos.y + 100.0f;

        if (this->actor.params == KEESE_ICE_FLY) {
            this->auraType = KEESE_AURA_ICE;
        } else {
            this->auraType = KEESE_AURA_NONE;
        }
    }

    this->collider.elements[0].dim.worldSphere.radius = FireflyAllJntSphData.elements[0].dim.modelSphere.radius;
}

void En_Firefly_actor_dt(Actor* thisx, PlayState* play) {
    EnFirefly* this = (EnFirefly*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

static void mode_fly_init(EnFirefly* this) {
    this->timer = get_random_timer(70, 100);
    this->actor.speed = (fqrand() * 1.5f) + 1.5f;
    chase_angle(&this->actor.shape.rot.y, Actor_search_position_angleY(&this->actor, &this->actor.home.pos), 0x300);
    this->targetPitch = ((this->maxAltitude < this->actor.world.pos.y) ? 0xC00 : -0xC00) + 0x1554;
    this->skelAnime.playSpeed = 1.0f;
    this->actionFunc = mode_fly;
}

static void mode_damage_init(EnFirefly* this) {
    this->timer = 40;
    this->actor.velocity.y = 0.0f;
    Skeleton_Info2_init(&this->skelAnime, &gKeeseFlyAnim, 0.5f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, -3.0f);
    Actor_SE_set(&this->actor, NA_SE_EN_FFLY_DEAD);
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 40);
    this->actionFunc = mode_damage;
}

static void mode_down_init(EnFirefly* this) {
    this->timer = 15;
    this->actor.speed = 0.0f;
    this->actionFunc = mode_down;
}

static void mode_reverse_init(EnFirefly* this) {
    this->actor.world.rot.x = 0x7000;
    this->timer = 18;
    this->skelAnime.playSpeed = 1.0f;
    this->actor.speed = 2.5f;
    this->actionFunc = mode_reverse;
}

static void mode_attack_init(EnFirefly* this) {
    this->timer = get_random_timer(70, 100);
    this->skelAnime.playSpeed = 1.0f;
    this->targetPitch = ((this->actor.yDistToPlayer > 0.0f) ? -0xC00 : 0xC00) + 0x1554;
    this->actionFunc = mode_attack;
}

static void mode_escape_init(EnFirefly* this) {
    this->timer = 150;
    this->skelAnime.playSpeed = 1.0f;
    this->targetPitch = 0x954;
    this->actionFunc = mode_escape;
}

static void mode_stop_init(EnFirefly* this) {
    this->timer = 80;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 80);
    this->auraType = KEESE_AURA_NONE;
    this->actor.velocity.y = 0.0f;
    this->skelAnime.playSpeed = 3.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    this->actionFunc = mode_stop;
}

static void mode_drop_init(EnFirefly* this, PlayState* play) {
    s32 i;
    Vec3f iceParticlePos;

    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->auraType = KEESE_AURA_NONE;
    this->actor.speed = 0.0f;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 255);
    Actor_SE_set(&this->actor, NA_SE_EN_FFLY_DEAD);

    for (i = 0; i <= 7; i++) {
        iceParticlePos.x = (i & 1 ? 7.0f : -7.0f) + this->actor.world.pos.x;
        iceParticlePos.y = (i & 2 ? 7.0f : -7.0f) + this->actor.world.pos.y;
        iceParticlePos.z = (i & 4 ? 7.0f : -7.0f) + this->actor.world.pos.z;
        Effect_En_Ice_ct0(play, &this->actor, &iceParticlePos, 150, 150, 150, 250, 235, 245, 255,
                                       (fqrand() * 0.15f) + 0.85f);
    }

    this->actionFunc = mode_drop;
}

static void mode_wait_init(EnFirefly* this) {
    this->timer = 1;
    this->actor.speed = 0.0f;
    this->actionFunc = mode_wait;
}

static void mode_start_init(EnFirefly* this) {
    this->skelAnime.playSpeed = 3.0f;
    this->actor.shape.rot.x = 0x1554;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    this->timer = 50;
    this->actor.speed = 3.0f;
    this->actionFunc = mode_start;
}

s32 check_return_home(EnFirefly* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 distFromHome;

    if (this->actor.params != KEESE_NORMAL_PERCH) {
        return 0;
    }

    if (Actor_search_position_distanceXZ(&player->actor, &this->actor.home.pos) > 300.0f) {
        distFromHome = Actor_search_position_distance(&this->actor, &this->actor.home.pos);

        if (distFromHome < 5.0f) {
            mode_wait_init(this);
            return 1;
        }

        distFromHome *= 0.05f;

        if (distFromHome < 1.0f) {
            this->actor.speed *= distFromHome;
        }

        chase_angle(&this->actor.shape.rot.y, Actor_search_position_angleY(&this->actor, &this->actor.home.pos),
                           0x300);
        chase_angle(&this->actor.shape.rot.x,
                           Actor_search_position_angleX(&this->actor, &this->actor.home.pos) + 0x1554, 0x100);
        return 1;
    }

    return 0;
}

s32 search_obj_syokudai(EnFirefly* this, PlayState* play) {
    ObjSyokudai* findTorch;
    ObjSyokudai* closestTorch;
    f32 torchDist;
    f32 currentMinDist;
    Vec3f flamePos;

    findTorch = (ObjSyokudai*)play->actorCtx.actorLists[ACTORCAT_PROP].head;
    closestTorch = NULL;
    currentMinDist = 35000.0f;

    while (findTorch != NULL) {
        if ((findTorch->actor.id == ACTOR_OBJ_SYOKUDAI) && (findTorch->litTimer != 0)) {
            torchDist = Actor_search_actor_distance(&this->actor, &findTorch->actor);
            if (torchDist < currentMinDist) {
                currentMinDist = torchDist;
                closestTorch = findTorch;
            }
        }
        findTorch = (ObjSyokudai*)findTorch->actor.next;
    }

    if (closestTorch != NULL) {
        flamePos.x = closestTorch->actor.world.pos.x;
        flamePos.y = closestTorch->actor.world.pos.y + 52.0f + 15.0f;
        flamePos.z = closestTorch->actor.world.pos.z;
        if (Actor_search_position_distance(&this->actor, &flamePos) < 15.0f) {
            shadowfly_to_firefly(this);
            return 1;
        } else {
            chase_angle(&this->actor.shape.rot.y, Actor_search_actor_angleY(&this->actor, &closestTorch->actor),
                               0x300);
            chase_angle(&this->actor.shape.rot.x, Actor_search_position_angleX(&this->actor, &flamePos) + 0x1554,
                               0x100);
            return 1;
        }
    }
    return 0;
}

static void mode_fly(EnFirefly* this, PlayState* play) {
    s32 skelanimeUpdated;
    f32 rand;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }
    skelanimeUpdated = Skeleton_Info_frame_check(&this->skelAnime, 0.0f);
    this->actor.speed = (fqrand() * 1.5f) + 1.5f;
    if (this->onFire || (this->actor.params == KEESE_ICE_FLY) ||
        ((check_return_home(this, play) == 0) && (search_obj_syokudai(this, play) == 0))) {
        if (skelanimeUpdated) {
            rand = fqrand();
            if (rand < 0.5f) {
                chase_angle(&this->actor.shape.rot.y,
                                   Actor_search_position_angleY(&this->actor, &this->actor.home.pos), 0x300);
            } else if (rand < 0.8f) {
                this->actor.shape.rot.y += rnd_fx(1536.0f);
            }
            // Climb if too close to ground
            if (this->actor.world.pos.y < (this->actor.floorHeight + 20.0f)) {
                this->targetPitch = 0x954;
                // Descend if above maxAltitude
            } else if (this->maxAltitude < this->actor.world.pos.y) {
                this->targetPitch = 0x2154;
                // Otherwise ascend or descend at random, biased towards ascending
            } else if (0.35f < fqrand()) {
                this->targetPitch = 0x954;
            } else {
                this->targetPitch = 0x2154;
            }
        } else {
            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                this->targetPitch = 0x954;
            } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) ||
                       (this->maxAltitude < this->actor.world.pos.y)) {
                this->targetPitch = 0x2154;
            }
        }
        chase_angle(&this->actor.shape.rot.x, this->targetPitch, 0x100);
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.wallYaw, 2, 0xC00, 0x300);
    }
    if ((this->timer == 0) && (this->actor.xzDistToPlayer < 200.0f) && (mask_check(play) != PLAYER_MASK_SKULL)) {
        mode_attack_init(this);
    }
}

// Fall to the ground after being hit
static void mode_damage(EnFirefly* this, PlayState* play) {
    if (Skeleton_Info_frame_check(&this->skelAnime, 6.0f)) {
        this->skelAnime.playSpeed = 0.0f;
    }
    this->actor.colorFilterTimer = 40;
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_f(&this->actor.speed, 0.0f, 0.5f);
    if (this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW) {
        this->actor.colorFilterTimer = 40;
    } else {
        chase_angle(&this->actor.shape.rot.x, 0x6800, 0x200);
        this->actor.shape.rot.y -= 0x300;
        if (this->timer != 0) {
            this->timer--;
        }
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->timer == 0)) {
            mode_down_init(this);
        }
    }
}

// Hit the ground or burn up, spawn drops
static void mode_down(EnFirefly* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    chase_f(&this->actor.scale.x, 0.0f, 0.00034f);
    this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;
    if (this->timer == 0) {
        Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0xE0);
        Actor_delete(&this->actor);
    }
}

static void mode_attack(EnFirefly* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f preyPos;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }
    chase_f(&this->actor.speed, 4.0f, 0.5f);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.wallYaw, 2, 0xC00, 0x300);
        chase_angle(&this->actor.shape.rot.x, this->targetPitch, 0x100);
    } else if (Actor_player_direction_check(&this->actor, 0x2800)) {
        if (Skeleton_Info_frame_check(&this->skelAnime, 4.0f)) {
            this->skelAnime.playSpeed = 0.0f;
            this->skelAnime.curFrame = 4.0f;
        }
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xC00, 0x300);
        preyPos.x = player->actor.world.pos.x;
        preyPos.y = player->actor.world.pos.y + 20.0f;
        preyPos.z = player->actor.world.pos.z;
        add_calc_short_angle2(&this->actor.shape.rot.x, Actor_search_position_angleX(&this->actor, &preyPos) + 0x1554, 2,
                           0x400, 0x100);
    } else {
        this->skelAnime.playSpeed = 1.5f;
        if (this->actor.xzDistToPlayer > 80.0f) {
            add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xC00, 0x300);
        }
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->targetPitch = 0x954;
        }
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) || (this->maxAltitude < this->actor.world.pos.y)) {
            this->targetPitch = 0x2154;
        } else {
            this->targetPitch = 0x954;
        }
        chase_angle(&this->actor.shape.rot.x, this->targetPitch, 0x100);
    }
    if ((this->timer == 0) || (mask_check(play) == PLAYER_MASK_SKULL)) {
        mode_escape_init(this);
    }
}

// Knockback after hitting player
static void mode_reverse(EnFirefly* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_angle(&this->actor.shape.rot.x, 0, 0x100);
    chase_f(&this->actor.velocity.y, 0.0f, 0.4f);
    if (chase_f(&this->actor.speed, 0.0f, 0.15f)) {
        if (this->timer != 0) {
            this->timer--;
        }
        if (this->timer == 0) {
            mode_escape_init(this);
        }
    }
}

static void mode_escape(EnFirefly* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }
    if (((fabsf(this->actor.world.pos.y - this->maxAltitude) < 10.0f) &&
         (search_position_distanceXZ(&this->actor.world.pos, &this->actor.home.pos) < 20.0f)) ||
        (this->timer == 0)) {
        mode_fly_init(this);
        return;
    }
    chase_f(&this->actor.speed, 3.0f, 0.3f);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->targetPitch = 0x954;
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) || (this->maxAltitude < this->actor.world.pos.y)) {
        this->targetPitch = 0x2154;
    } else {
        this->targetPitch = 0x954;
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.wallYaw, 2, 0xC00, 0x300);
    } else {
        chase_angle(&this->actor.shape.rot.y, Actor_search_position_angleY(&this->actor, &this->actor.home.pos),
                           0x300);
    }
    chase_angle(&this->actor.shape.rot.x, this->targetPitch, 0x100);
}

static void mode_stop(EnFirefly* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_f(&this->actor.speed, 0.0f, 0.5f);
    chase_angle(&this->actor.shape.rot.x, 0x1554, 0x100);
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        if (this->onFire) {
            this->auraType = KEESE_AURA_FIRE;
        } else if (this->actor.params == KEESE_ICE_FLY) {
            this->auraType = KEESE_AURA_ICE;
        }
        mode_fly_init(this);
    }
}

static void mode_drop(EnFirefly* this, PlayState* play) {
#if OOT_VERSION < NTSC_1_1
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) || (this->actor.floorHeight == BGCHECK_Y_MIN))
#else
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.floorHeight == BGCHECK_Y_MIN))
#endif
    {
        this->actor.colorFilterTimer = 0;
        mode_down_init(this);
    } else {
        this->actor.colorFilterTimer = 255;
    }
}

// When perching, sit on collision and flap at random intervals
static void mode_wait(EnFirefly* this, PlayState* play) {
    chase_angle(&this->actor.shape.rot.x, 0, 0x100);

    if (this->timer != 0) {
        Skeleton_Info2_anime_play(&this->skelAnime);
        if (Skeleton_Info_frame_check(&this->skelAnime, 6.0f)) {
            this->timer--;
        }
    } else if (fqrand() < 0.02f) {
        this->timer = 1;
    }

    if (this->actor.xzDistToPlayer < 120.0f) {
        mode_start_init(this);
    }
}

static void mode_start(EnFirefly* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f preyPos;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer < 40) {
        chase_angle(&this->actor.shape.rot.x, -0xAAC, 0x100);
    } else {
        preyPos.x = player->actor.world.pos.x;
        preyPos.y = player->actor.world.pos.y + 20.0f;
        preyPos.z = player->actor.world.pos.z;
        chase_angle(&this->actor.shape.rot.x, Actor_search_position_angleX(&this->actor, &preyPos) + 0x1554,
                           0x100);
        chase_angle(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x300);
    }

    if (this->timer == 0) {
        mode_fly_init(this);
    }
}

void set_icefly_fire_effect(EnFirefly* this, PlayState* play) {
    s32 i;

    for (i = 0; i <= 2; i++) {
        Effect_En_Fire_ct(play, &this->actor, &this->actor.world.pos, 40, 0, 0, i);
    }

    this->auraType = KEESE_AURA_NONE;
}

void En_Firefly_damage_proc(EnFirefly* this, PlayState* play) {
    u8 damageEffect;

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Hit_bit_set(&this->actor, &this->collider.elements[0].base, true);

        if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
            if (hp_down(&this->actor) == 0) {
                Actor_info_finish(play, &this->actor);
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            }

            damageEffect = this->actor.colChkInfo.damageEffect;

            if (damageEffect == 2) { // Din's Fire
                if (this->actor.params == KEESE_ICE_FLY) {
                    this->actor.colChkInfo.health = 0;
                    Actor_info_finish(play, &this->actor);
                    set_icefly_fire_effect(this, play);
                    mode_damage_init(this);
                } else if (!this->onFire) {
                    shadowfly_to_firefly(this);
                    if (this->actionFunc == mode_wait) {
                        mode_fly_init(this);
                    }
                }
            } else if (damageEffect == 3) { // Ice Arrows or Ice Magic
                if (this->actor.params == KEESE_ICE_FLY) {
                    mode_damage_init(this);
                } else {
                    mode_drop_init(this, play);
                }
            } else if (damageEffect == 1) { // Deku Nuts
                if (this->actionFunc != mode_stop) {
                    mode_stop_init(this);
                }
            } else { // Fire Arrows
                if ((damageEffect == 0xF) && (this->actor.params == KEESE_ICE_FLY)) {
                    set_icefly_fire_effect(this, play);
                }
                mode_damage_init(this);
            }
        }
    }
}

void En_Firefly_actor_move(Actor* thisx, PlayState* play2) {
    EnFirefly* this = (EnFirefly*)thisx;
    PlayState* play = play2;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        Actor_SE_set(&this->actor, NA_SE_EN_FFLY_ATTACK);
        if (this->onFire) {
            firefly_to_shadowfly(this);
        }
        if (this->actionFunc != mode_start) {
            mode_reverse_init(this);
        }
    }

    En_Firefly_damage_proc(this, play);

    this->actionFunc(this, play);

    if (!(this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW)) {
        if ((this->actor.colChkInfo.health == 0) || (this->actionFunc == mode_stop)) {
            Actor_position_moveF(&this->actor);
        } else {
            if (this->actionFunc != mode_reverse) {
                this->actor.world.rot.x = 0x1554 - this->actor.shape.rot.x;
            }
            Actor_position_moveF_XY(&this->actor);
        }
    }

    Actor_BGcheck2(play, &this->actor, 10.0f, 10.0f, 15.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
    this->collider.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider.elements[0].dim.worldSphere.center.y = this->actor.world.pos.y + 10.0f;
    this->collider.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;

    if ((this->actionFunc == mode_attack) || (this->actionFunc == mode_start)) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->actor.colChkInfo.health != 0) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (Skeleton_Info_frame_check(&this->skelAnime, 5.0f)) {
            Actor_SE_set(&this->actor, NA_SE_EN_FFLY_FLY);
        }
    }

    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    this->actor.focus.pos.x =
        (10.0f * sin_s(this->actor.shape.rot.x) * sin_s(this->actor.shape.rot.y)) + this->actor.world.pos.x;
    this->actor.focus.pos.y = (10.0f * cos_s(this->actor.shape.rot.x)) + this->actor.world.pos.y;
    this->actor.focus.pos.z =
        (10.0f * sin_s(this->actor.shape.rot.x) * cos_s(this->actor.shape.rot.y)) + this->actor.world.pos.z;
}

s32 en_firefly_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                               Gfx** gfx) {
    EnFirefly* this = (EnFirefly*)thisx;

    if ((this->actor.draw == En_Firefly_actor_draw_xlu) && !play->actorCtx.lensActive) {
        *dList = NULL;
    } else if (limbIndex == 1) {
        pos->y += 2300.0f;
    }
    return false;
}

void en_firefly_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    static Color_RGBA8 fire_prim = { 255, 255, 100, 255 };
    static Color_RGBA8 fire_env = { 255, 50, 0, 0 };
    static Color_RGBA8 ice_prim = { 100, 200, 255, 255 };
    static Color_RGBA8 ice_env = { 0, 0, 255, 0 };
    static Vec3f fire_vec = { 0.0f, 0.5f, 0.0f };
    static Vec3f fire_acc = { 0.0f, 0.5f, 0.0f };
    static Vec3f zero_pos = { 0.0f, 0.0f, 0.0f };
    Vec3f effPos;
    Vec3f* limbDest;
    Color_RGBA8* effPrimColor;
    Color_RGBA8* effEnvColor;
    MtxF mtx;
    s16 effScaleStep;
    s16 effLife;
    EnFirefly* this = (EnFirefly*)thisx;

    if (!this->onFire && (limbIndex == 27)) {
        gSPDisplayList((*gfx)++, gKeeseEyesDL);
    } else {
        if ((this->auraType == KEESE_AURA_FIRE) || (this->auraType == KEESE_AURA_ICE)) {
            if ((limbIndex == 15) || (limbIndex == 21)) {
                if (this->actionFunc != mode_down) {
                    Matrix_get(&mtx);
                    effPos.x = (fqrand() * 5.0f) + mtx.xw;
                    effPos.y = (fqrand() * 5.0f) + mtx.yw;
                    effPos.z = (fqrand() * 5.0f) + mtx.zw;
                    effScaleStep = -40;
                    effLife = 3;
                } else {
                    if (limbIndex == 15) {
                        effPos.x = (sin_s(9100 * this->timer) * this->timer) + this->actor.world.pos.x;
                        effPos.z = (cos_s(9100 * this->timer) * this->timer) + this->actor.world.pos.z;
                    } else {
                        effPos.x = this->actor.world.pos.x - (sin_s(9100 * this->timer) * this->timer);
                        effPos.z = this->actor.world.pos.z - (cos_s(9100 * this->timer) * this->timer);
                    }

                    effPos.y = this->actor.world.pos.y + ((15 - this->timer) * 1.5f);
                    effScaleStep = -5;
                    effLife = 10;
                }

                if (this->auraType == KEESE_AURA_FIRE) {
                    effPrimColor = &fire_prim;
                    effEnvColor = &fire_env;
                } else {
                    effPrimColor = &ice_prim;
                    effEnvColor = &ice_env;
                }

                Effect_SS_Dust_sc_cl_co_nofog_ct(play, &effPos, &fire_vec, &fire_acc, effPrimColor, effEnvColor, 250, effScaleStep,
                              effLife);
            }
        }
    }
    if ((limbIndex == 15) || (limbIndex == 21) || (limbIndex == 10)) {
        if (limbIndex == 15) {
            limbDest = &this->bodyPartsPos[0];
        } else if (limbIndex == 21) {
            limbDest = &this->bodyPartsPos[1];
        } else {
            limbDest = &this->bodyPartsPos[2];
        }

        Matrix_Position(&zero_pos, limbDest);
        limbDest->y -= 5.0f;
    }
}

void En_Firefly_actor_draw(Actor* thisx, PlayState* play) {
    EnFirefly* this = (EnFirefly*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_firefly.c", 1733);
    _texture_z_light_fog_prim(play->state.gfxCtx);

    if (this->onFire) {
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    } else {
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    }

    POLY_OPA_DISP = Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   en_firefly_display1, en_firefly_display2, &this->actor, POLY_OPA_DISP);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_firefly.c", 1763);
}

void En_Firefly_actor_draw_xlu(Actor* thisx, PlayState* play) {
    EnFirefly* this = (EnFirefly*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_firefly.c", 1775);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    if (this->onFire) {
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 0);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 255);
    }

    POLY_XLU_DISP = Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   en_firefly_display1, en_firefly_display2, this, POLY_XLU_DISP);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_firefly.c", 1805);
}
