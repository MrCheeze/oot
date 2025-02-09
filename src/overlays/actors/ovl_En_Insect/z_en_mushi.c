/*
 * File: z_en_insect.c
 * Overlay: ovl_En_Insect
 * Description: Bugs
 */

#include "z_en_insect.h"
#include "global.h"
#include "terminal.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS 0

void En_Mushi_actor_ct(Actor* thisx, PlayState* play2);
void En_Mushi_actor_dt(Actor* thisx, PlayState* play);
void En_Mushi_actor_move(Actor* thisx, PlayState* play);
void En_Mushi_actor_draw(Actor* thisx, PlayState* play);

static void mv_stop_init(EnInsect* this);
static void mv_stop(EnInsect* this, PlayState* play);
void mv_go_init(EnInsect* this);
static void mv_go(EnInsect* this, PlayState* play);
void mv_away_init(EnInsect* this);
static void mv_away(EnInsect* this, PlayState* play);
static void mv_clear_init(EnInsect* this);
static void mv_clear(EnInsect* this, PlayState* play);
void mv_mogu_init(EnInsect* this);
void mv_mogu(EnInsect* this, PlayState* play);
void mv_swim_init(EnInsect* this);
void mv_swim(EnInsect* this, PlayState* play);
void mv_sink_init(EnInsect* this);
void mv_sink(EnInsect* this, PlayState* play);
void mv_beanSearch_init(EnInsect* this);
void mv_beanSearch(EnInsect* this, PlayState* play);

static f32 Bean_search_power = 0.0f;
/**
 * The number of bugs caught this frame.
 */
static s16 Carry_counter = 0;
/**
 * The number of active dropped bugs.
 */
static s16 Bin_counter = 0;

ActorProfile En_Insect_Profile = {
    /**/ ACTOR_EN_INSECT,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnInsect),
    /**/ En_Mushi_actor_ct,
    /**/ En_Mushi_actor_dt,
    /**/ En_Mushi_actor_move,
    /**/ En_Mushi_actor_draw,
};

static ColliderJntSphElementInit CrossSphElemDt_mushi[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 5 }, 100 },
    },
};

static ColliderJntSphInit CrossSphDt_mushi = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_1,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    CrossSphElemDt_mushi,
};

/**
 * The initial flags for a new bug, depending on its type.
 */
static u16 status_data[] = {
    0,
    INSECT_FLAG_0 | INSECT_FLAG_IS_SHORT_LIVED,
    INSECT_FLAG_0 | INSECT_FLAG_1 | INSECT_FLAG_IS_SHORT_LIVED,
    INSECT_FLAG_0 | INSECT_FLAG_1 | INSECT_FLAG_IS_SHORT_LIVED,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 700, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 20, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 600, ICHAIN_STOP),
};

void init_status_mushi(EnInsect* this) {
    this->insectFlags = status_data[PARAMS_GET_U(this->actor.params, 0, 2)];
}

static f32 get_distXZ_2(Vec3f* v1, Vec3f* v2) {
    return SQ(v1->x - v2->x) + SQ(v1->z - v2->z);
}

static s32 check_carryArea(EnInsect* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    Vec3f pos;

    if (this->actor.xzDistToPlayer < 32.0f) {
        pos.x = sin_s(this->actor.yawTowardsPlayer + 0x8000) * 16.0f + player->actor.world.pos.x;
        pos.y = player->actor.world.pos.y;
        pos.z = cos_s(this->actor.yawTowardsPlayer + 0x8000) * 16.0f + player->actor.world.pos.z;

        //! @bug: this check is superfluous: it is automatically satisfied if the coarse check is satisfied. It may have
        //! been intended to check the actor is in front of Player, but yawTowardsPlayer does not depend on Player's
        //! world rotation.
        if (get_distXZ_2(&pos, &this->actor.world.pos) <= SQ(20.0f)) {
            return true;
        }
    }

    return false;
}

void reset_skeleton_mushi(EnInsect* this) {
    Skeleton_Info2_init(&this->skelAnime, &gBugCrawlAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, 0.0f);
}

/**
 * Find the nearest soft dirt patch within 6400 units in the xz plane and the current room
 *
 * @return true if one was found, false otherwise
 */
s32 set_bean_goal_actor(EnInsect* this, PlayState* play) {
    Actor* currentActor = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;
    f32 currentDistanceSq;
    f32 bestDistanceSq = SQ(80.0f);
    s32 ret = false;

    this->soilActor = NULL;

    while (currentActor != NULL) {
        if (currentActor->id == ACTOR_OBJ_MAKEKINSUTA) {
            currentDistanceSq = Math3DLengthSquare2D(this->actor.world.pos.x, this->actor.world.pos.z,
                                                currentActor->world.pos.x, currentActor->world.pos.z);

            if (currentDistanceSq < bestDistanceSq && currentActor->room == this->actor.room) {
                ret = true;
                bestDistanceSq = currentDistanceSq;
                this->soilActor = (ObjMakekinsuta*)currentActor;
            }
        }
        currentActor = currentActor->next;
    }
    return ret;
}

/**
 * Update the crawl sound timer, and play the crawling sound effect when it reaches 0.
 */
void reset_walk_SE_mushi(EnInsect* this) {
    if (this->crawlSoundDelay > 0) {
        this->crawlSoundDelay--;
        return;
    }

    Actor_SE_set(&this->actor, NA_SE_EN_MUSI_WALK);

    this->crawlSoundDelay = 3.0f / CLAMP_MIN(this->skelAnime.playSpeed, 0.1f);
    if (this->crawlSoundDelay < 2) {
        this->crawlSoundDelay = 2;
    }
}

void En_Mushi_actor_ct(Actor* thisx, PlayState* play2) {
    EnInsect* this = (EnInsect*)thisx;
    PlayState* play = play2;
    f32 rand;
    s16 type;
    s32 count;

    ValueSet_process(&this->actor, value_init);
    init_status_mushi(this);

    type = PARAMS_GET_U(this->actor.params, 0, 2);

    Skeleton_Info2_M_ct(play, &this->skelAnime, &gBugSkel, &gBugCrawlAnim, this->jointTable, this->morphTable, 24);
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &CrossSphDt_mushi, &this->colliderItem);

    this->actor.colChkInfo.mass = 30;

    if (this->insectFlags & INSECT_FLAG_0) {
        this->actor.gravity = -0.2f;
        this->actor.minVelocityY = -2.0f;
    }

    if (this->insectFlags & INSECT_FLAG_IS_SHORT_LIVED) {
        this->lifeTimer = get_random_timer(200, 40);
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    }

    if (type == INSECT_TYPE_FIRST_DROPPED || type == INSECT_TYPE_EXTRA_DROPPED) {
        if (set_bean_goal_actor(this, play)) {
            this->insectFlags |= INSECT_FLAG_FOUND_SOIL;
            Bean_search_power = 0.0f;
        }

        if (type == INSECT_TYPE_FIRST_DROPPED) {
            this->actor.world.rot.z = 0;
            this->actor.shape.rot.z = this->actor.world.rot.z;

            for (count = 0; count < 2; count++) {
                Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_INSECT, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, this->actor.shape.rot.x, this->actor.shape.rot.y,
                            this->actor.shape.rot.z, INSECT_TYPE_EXTRA_DROPPED);
            }
        }

        mv_beanSearch_init(this);

        Bin_counter++;
    } else {
        rand = fqrand();

        if (rand < 0.3f) {
            mv_stop_init(this);
        } else if (rand < 0.4f) {
            mv_go_init(this);
        } else {
            mv_away_init(this);
        }
    }
}

void En_Mushi_actor_dt(Actor* thisx, PlayState* play) {
    s16 type;
    EnInsect* this = (EnInsect*)thisx;

    type = PARAMS_GET_U(this->actor.params, 0, 2);
    ClObjJntSph_dt_nzf(play, &this->collider);
    if ((type == INSECT_TYPE_FIRST_DROPPED || type == INSECT_TYPE_EXTRA_DROPPED) && Bin_counter > 0) {
        Bin_counter--;
    }
}

static void mv_stop_init(EnInsect* this) {
    this->actionTimer = get_random_timer(5, 35);
    reset_skeleton_mushi(this);
    this->actionFunc = mv_stop;
    this->insectFlags |= INSECT_FLAG_CRAWLING;
}

static void mv_stop(EnInsect* this, PlayState* play) {
    s32 pad[2];
    s16 type;
    f32 playSpeed;

    type = PARAMS_GET_U(this->actor.params, 0, 2);

    add_calc(&this->actor.speed, 0.0f, 0.1f, 0.5f, 0.0f);

    playSpeed = (fqrand() * 0.8f) + (this->actor.speed * 1.2f);
    this->skelAnime.playSpeed = CLAMP(playSpeed, 0.0f, 1.9f);

    Skeleton_Info2_anime_play(&this->skelAnime);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if (this->actionTimer <= 0) {
        mv_go_init(this);
    }

    if (((this->insectFlags & INSECT_FLAG_IS_SHORT_LIVED) && this->lifeTimer <= 0) ||
        ((type == INSECT_TYPE_FIRST_DROPPED || type == INSECT_TYPE_EXTRA_DROPPED) &&
         (this->insectFlags & INSECT_FLAG_0) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
         Bin_counter >= 4)) {
        mv_mogu_init(this);
    } else if ((this->insectFlags & INSECT_FLAG_0) && (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH)) {
        mv_swim_init(this);
    } else if (this->actor.xzDistToPlayer < 40.0f) {
        mv_away_init(this);
    }
}

void mv_go_init(EnInsect* this) {
    this->actionTimer = get_random_timer(10, 45);
    reset_skeleton_mushi(this);
    this->actionFunc = mv_go;
    this->insectFlags |= INSECT_FLAG_CRAWLING;
}

static void mv_go(EnInsect* this, PlayState* play) {
    s32 pad1;
    s32 pad2;
    s16 yaw;
    s16 type = PARAMS_GET_U(this->actor.params, 0, 2);

    add_calc(&this->actor.speed, 1.5f, 0.1f, 0.5f, 0.0f);

    if (get_distXZ_2(&this->actor.world.pos, &this->actor.home.pos) > 1600.0f ||
        (this->actionTimer < 4)) {
        yaw = search_position_angleY(&this->actor.world.pos, &this->actor.home.pos);
        chase_angle(&this->actor.world.rot.y, yaw, 2000);
    } else if (this->actor.child != NULL && &this->actor != this->actor.child) {
        yaw = search_position_angleY(&this->actor.world.pos, &this->actor.child->world.pos);
        chase_angle(&this->actor.world.rot.y, yaw, 2000);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->skelAnime.playSpeed = CLAMP(this->actor.speed * 1.4f, 0.7f, 1.9f);

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->actionTimer <= 0) {
        mv_stop_init(this);
    }

    if (((this->insectFlags & INSECT_FLAG_IS_SHORT_LIVED) && this->lifeTimer <= 0) ||
        ((type == INSECT_TYPE_FIRST_DROPPED || type == INSECT_TYPE_EXTRA_DROPPED) &&
         (this->insectFlags & INSECT_FLAG_0) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
         Bin_counter >= 4)) {
        mv_mogu_init(this);
    } else if ((this->insectFlags & INSECT_FLAG_0) && (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH)) {
        mv_swim_init(this);
    } else if (this->actor.xzDistToPlayer < 40.0f) {
        mv_away_init(this);
    }
}

void mv_away_init(EnInsect* this) {
    this->actionTimer = get_random_timer(10, 40);
    reset_skeleton_mushi(this);
    this->actionFunc = mv_away;
    this->insectFlags |= INSECT_FLAG_CRAWLING;
}

static void mv_away(EnInsect* this, PlayState* play) {
    s32 pad1;
    s32 pad2;
    s16 pad3;
    s16 frames;
    s16 yaw;
    s16 playerIsClose = this->actor.xzDistToPlayer < 40.0f;

    add_calc(&this->actor.speed, 1.8f, 0.1f, 0.5f, 0.0f);

    if (get_distXZ_2(&this->actor.world.pos, &this->actor.home.pos) > 25600.0f || this->actionTimer < 4) {
        yaw = search_position_angleY(&this->actor.world.pos, &this->actor.home.pos);
        chase_angle(&this->actor.world.rot.y, yaw, 2000);
    } else if (playerIsClose) {
        frames = play->state.frames;
        yaw = this->actor.yawTowardsPlayer + 0x8000;

        if (frames & 0x10) {
            if (frames & 0x20) {
                yaw += 0x2000;
            }
        } else {
            if (frames & 0x20) {
                yaw -= 0x2000;
            }
        }
        if (play) {} // Must be 'play'
        chase_angle(&this->actor.world.rot.y, yaw, 2000);
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->skelAnime.playSpeed = CLAMP(this->actor.speed * 1.6f, 0.8f, 1.9f);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->actionTimer <= 0 || !playerIsClose) {
        mv_stop_init(this);
    } else if ((this->insectFlags & INSECT_FLAG_0) && (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH)) {
        mv_swim_init(this);
    }
}

static void mv_clear_init(EnInsect* this) {
    this->actionTimer = 200;

    Actor_set_scale(&this->actor, 0.001f);

    this->actor.draw = NULL;
    this->actor.speed = 0.0f;

    reset_skeleton_mushi(this);

    this->skelAnime.playSpeed = 0.3f;
    this->actionFunc = mv_clear;
    this->insectFlags &= ~INSECT_FLAG_CRAWLING;
}

static void mv_clear(EnInsect* this, PlayState* play) {
    if (this->actionTimer == 20 && !(this->insectFlags & INSECT_FLAG_IS_SHORT_LIVED)) {
        this->actor.draw = En_Mushi_actor_draw;
    } else if (this->actionTimer == 0) {
        if (this->insectFlags & INSECT_FLAG_IS_SHORT_LIVED) {
            Actor_delete(&this->actor);
        } else {
            Actor_set_scale(&this->actor, 0.01f);
            mv_stop_init(this);
        }
    } else if (this->actionTimer < 20) {
        Actor_set_scale(&this->actor, CLAMP_MAX(this->actor.scale.x + 0.001f, 0.01f));
        Skeleton_Info2_anime_play(&this->skelAnime);
    }
}

void mv_mogu_init(EnInsect* this) {
    this->actionTimer = 60;
    reset_skeleton_mushi(this);
    this->skelAnime.playSpeed = 1.9f;
    Actor_SE_set(&this->actor, NA_SE_EN_MUSI_SINK);
    xyz_t_move(&this->actor.home.pos, &this->actor.world.pos);
    this->actionFunc = mv_mogu;
    this->insectFlags &= ~INSECT_FLAG_CRAWLING;
    this->insectFlags |= INSECT_FLAG_UNCATCHABLE;
}

void mv_mogu(EnInsect* this, PlayState* play) {
    static Vec3f dust_acc = { 0.0f, 0.0f, 0.0f };
    static Vec3f unused = { 0.0f, 0.0f, 0.0f };
    s32 pad[2];
    Vec3f velocity;

    add_calc(&this->actor.speed, 0.0f, 0.1f, 0.5f, 0.0f);
    chase_s(&this->actor.shape.rot.x, 10922, 352);

    Actor_set_scale(&this->actor, CLAMP_MIN(this->actor.scale.x - 0.0002f, 0.001f));

    this->actor.shape.yOffset -= 0.8f;
    this->actor.world.pos.x = fqrand() + this->actor.home.pos.x - 0.5f;
    this->actor.world.pos.z = fqrand() + this->actor.home.pos.z - 0.5f;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->actionTimer > 20 && fqrand() < 0.1f) {
        velocity.x = sin_s(this->actor.shape.rot.y) * -0.6f;
        velocity.y = sin_s(this->actor.shape.rot.x) * 0.6f;
        velocity.z = cos_s(this->actor.shape.rot.y) * -0.6f;
        Effect_SS_Dust_sc_li_ct(play, &this->actor.world.pos, &velocity, &dust_acc, fqrand() * 5.0f + 8.0f,
                      fqrand() * 5.0f + 8.0f);
    }

    if (this->actionTimer <= 0) {
        if ((this->insectFlags & INSECT_FLAG_FOUND_SOIL) && this->soilActor != NULL &&
            Math3DLengthSquare(&this->soilActor->actor.world.pos, &this->actor.world.pos) < SQ(8.0f)) {
            this->soilActor->unk_152 = 1;
        }
        Actor_delete(&this->actor);
    }
}

void mv_swim_init(EnInsect* this) {
    this->actionTimer = get_random_timer(120, 50);
    reset_skeleton_mushi(this);
    this->unk_316 = this->unk_318 = 0;
    this->actionFunc = mv_swim;
    this->insectFlags &= ~INSECT_FLAG_CRAWLING;
}

void mv_swim(EnInsect* this, PlayState* play) {
    f32 temp_f0;
    s16 temp_v1;
    s16 pad;
    s16 type;
    Vec3f ripplePoint;

    type = PARAMS_GET_U(this->actor.params, 0, 2);

    if (this->actionTimer > 80) {
        chase_f(&this->actor.speed, 0.6f, 0.08f);
    } else {
        chase_f(&this->actor.speed, 0.0f, 0.02f);
    }
    this->actor.velocity.y = 0.0f;
    this->actor.world.pos.y += this->actor.depthInWater;
    this->skelAnime.playSpeed = CLAMP(this->actionTimer * 0.018f, 0.1f, 1.9f);

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->actionTimer > 80) {
        this->unk_316 += get_random_timer(-50, 100);
        this->unk_318 += get_random_timer(-300, 600);
    }

    temp_v1 = this->skelAnime.playSpeed * 200.0f;
    this->unk_316 = CLAMP(this->unk_316, -temp_v1, temp_v1);
    this->actor.world.rot.y += this->unk_316;

    temp_v1 = this->skelAnime.playSpeed * 1000.0f;
    this->unk_318 = CLAMP(this->unk_318, -temp_v1, temp_v1);
    this->actor.shape.rot.y += this->unk_318;

    chase_angle(&this->actor.world.rot.x, 0, 3000);
    this->actor.shape.rot.x = this->actor.world.rot.x;

    if (fqrand() < 0.03f) {
        ripplePoint.x = this->actor.world.pos.x;
        ripplePoint.y = this->actor.world.pos.y + this->actor.depthInWater;
        ripplePoint.z = this->actor.world.pos.z;
        Effect_SS_G_Ripple_ct2(play, &ripplePoint, 20, 100, 4);
        Effect_SS_G_Ripple_ct2(play, &ripplePoint, 40, 200, 8);
    }

    if (this->actionTimer <= 0 || ((this->insectFlags & INSECT_FLAG_IS_SHORT_LIVED) && this->lifeTimer <= 0) ||
        ((type == INSECT_TYPE_FIRST_DROPPED || type == INSECT_TYPE_EXTRA_DROPPED) &&
         (this->insectFlags & INSECT_FLAG_0) && Bin_counter >= 4)) {
        mv_sink_init(this);
    } else if (!(this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH)) {
        if (this->insectFlags & INSECT_FLAG_FOUND_SOIL) {
            mv_beanSearch_init(this);
        } else {
            mv_stop_init(this);
        }
    }
}

void mv_sink_init(EnInsect* this) {
    this->actionTimer = 100;
    reset_skeleton_mushi(this);
    this->actor.velocity.y = 0.0f;
    this->actor.speed = 0.0f;
    this->actor.minVelocityY = -0.8f;
    this->actor.gravity = -0.04f;
    this->insectFlags &= ~(INSECT_FLAG_0 | INSECT_FLAG_1);
    this->actionFunc = mv_sink;
    this->insectFlags &= ~INSECT_FLAG_CRAWLING;
    this->insectFlags |= INSECT_FLAG_UNCATCHABLE;
}

void mv_sink(EnInsect* this, PlayState* play) {
    this->actor.shape.rot.x -= 500;
    this->actor.shape.rot.y += 200;
    Actor_set_scale(&this->actor, CLAMP_MIN(this->actor.scale.x - 0.00005f, 0.001f));

    if (this->actor.depthInWater > 5.0f && this->actor.depthInWater < 30.0f && fqrand() < 0.3f) {
        Effect_SS_Bubble_ct(play, &this->actor.world.pos, -5.0f, 5.0f, 5.0f, (fqrand() * 0.04f) + 0.02f);
    }

    if (this->actionTimer <= 0) {
        Actor_delete(&this->actor);
    }
}

void mv_beanSearch_init(EnInsect* this) {
    reset_skeleton_mushi(this);
    this->actionTimer = 100;
    this->unk_324 = 1.5f;
    this->unk_328 = fqrand() * (0xFFFF + 0.5f);
    this->unk_316 = (fqrand() - 0.5f) * 1500.0f;
    this->actor.world.rot.y = fqrand() * (0xFFFF + 0.5f);
    Actor_set_scale(&this->actor, 0.003f);
    this->actionFunc = mv_beanSearch;
    this->insectFlags |= INSECT_FLAG_CRAWLING;
}

void mv_beanSearch(EnInsect* this, PlayState* play) {
    s32 temp_a0;
    s32 sp50;
    f32 phi_f0;
    EnInsect* thisTemp = this;
    s32 temp_a1;
    f32 distanceSq;
    f32 phi_f2;
    s16 type;
    s16 sp38;
    f32 sp34;

    sp50 = 0;
    type = PARAMS_GET_U(this->actor.params, 0, 2);

    if (this->soilActor != NULL) {
        distanceSq = Math3DLengthSquare(&this->actor.world.pos, &this->soilActor->actor.world.pos);
    } else {
        if (this->insectFlags & INSECT_FLAG_FOUND_SOIL) {
            PRINTF_COLOR_WARNING();
            // "warning: target Actor is NULL"
            PRINTF("warning:目標 Actor が NULL (%s %d)\n", "../z_en_mushi.c", 1046);
            PRINTF_RST();
        }
        distanceSq = 40.0f;
    }

    Bean_search_power += 0.99999994f / 300.0f;
    if (Bean_search_power > 1.0f) {
        Bean_search_power = 1.0f;
    }

    if (Bean_search_power > 0.999f) {
        phi_f2 = 0.0f;
    } else {
        if (distanceSq > SQ(30.0f)) {
            phi_f2 = ((1.1f - Bean_search_power) * 100.0f) + 20.0f;
        } else {
            phi_f2 = (1.0f - Bean_search_power) * 10.0f;
        }
    }

    if (this->soilActor != NULL && fqrand() < 0.07f) {
        this->actor.home.pos.x = (fqrand() - 0.5f) * phi_f2 + thisTemp->soilActor->actor.world.pos.x;
        this->actor.home.pos.y = thisTemp->soilActor->actor.world.pos.y;
        this->actor.home.pos.z = (fqrand() - 0.5f) * phi_f2 + thisTemp->soilActor->actor.world.pos.z;
    }

    if (Bean_search_power > 0.999f) {
        this->unk_328 = search_position_angleY(&this->actor.world.pos, &this->actor.home.pos);
        this->unk_324 = fqrand() * 0.6f + 0.6f;
    } else if (fqrand() < 0.07f) {
        if (this->unk_324 > 1.0f) {
            this->unk_324 = 0.1f;
        } else {
            this->unk_324 = fqrand() * 0.8f + 1.0f;
        }

        sp34 = 1.3f - Bean_search_power;
        if (sp34 < 0.0f) {
            sp34 = 0.0f;
        } else {
            if (sp34 > 1.0f) {
                phi_f0 = 1.0f;
            } else {
                phi_f0 = sp34;
            }
            sp34 = phi_f0;
        }

        sp38 = (fqrand() - 0.5f) * 65535.0f * sp34;
        this->unk_328 = search_position_angleY(&this->actor.world.pos, &this->actor.home.pos) + sp38;
    }

    Actor_set_scale(&this->actor, CLAMP_MAX(thisTemp->actor.scale.x + 0.0008f, 0.01f));

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        add_calc(&this->actor.speed, this->unk_324, 0.1f, 0.5f, 0.0f);
        chase_angle(&this->actor.world.rot.y, this->unk_328, 2000);
        sp50 = chase_angle(&this->actor.world.rot.x, 0, 2000);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        this->actor.shape.rot.x = this->actor.world.rot.x;
    } else {
        add_calc(&this->actor.speed, 0.0f, 0.1f, 0.5f, 0.0f);
        this->actor.speed += (fqrand() - 0.5f) * 0.14f;
        this->actor.velocity.y += fqrand() * 0.12f;
        this->actor.world.rot.y += this->unk_316;
        this->actor.shape.rot.y = this->actor.world.rot.y;
        this->actor.shape.rot.x -= 2000;
    }

    phi_f2 = fqrand() * 0.5f + this->actor.speed * 1.3f;
    if (phi_f2 < 0.0f) {
        this->skelAnime.playSpeed = 0.0f;
    } else {
        if (phi_f2 > 1.9f) {
            phi_f0 = 1.9f;
        } else {
            phi_f0 = phi_f2;
        }
        this->skelAnime.playSpeed = phi_f0;
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (!(this->insectFlags & INSECT_FLAG_DROPPED_HAS_LANDED) && (this->insectFlags & INSECT_FLAG_0) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Actor_SE_set(&this->actor, NA_SE_EN_MUSI_LAND);
        this->insectFlags |= INSECT_FLAG_DROPPED_HAS_LANDED;
    }

    if (type == INSECT_TYPE_FIRST_DROPPED && (this->insectFlags & INSECT_FLAG_FOUND_SOIL) &&
        !(this->insectFlags & INSECT_FLAG_7)) {
        if (this->unk_32A >= 15) {
            if (this->soilActor != NULL) {
                if (!(GET_GS_FLAGS(PARAMS_GET_U(this->soilActor->actor.params, 8, 5) - 1) &
                      PARAMS_GET_U(this->soilActor->actor.params, 0, 8))) {
                    Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
                }
            }
            this->insectFlags |= INSECT_FLAG_7;
        } else {
            this->unk_32A++;
        }
    }

    if ((this->insectFlags & INSECT_FLAG_0) && (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH)) {
        mv_swim_init(this);
    } else if (this->insectFlags & INSECT_FLAG_FOUND_SOIL) {
        if (distanceSq < SQ(3.0f)) {
            mv_mogu_init(this);
        } else if (this->actionTimer <= 0 || this->lifeTimer <= 0 ||
                   ((this->insectFlags & INSECT_FLAG_0) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
                    Bin_counter >= 4 && (type == INSECT_TYPE_FIRST_DROPPED || type == INSECT_TYPE_EXTRA_DROPPED))) {
            mv_mogu_init(this);
        } else {
            if (distanceSq < SQ(30.0f)) {
                this->lifeTimer++;
                this->insectFlags |= INSECT_FLAG_SOIL_CLOSE;
            } else {
                this->actionTimer = 100;
            }
        }
    } else if (sp50 != 0) {
        mv_stop_init(this);
    } else if ((type == INSECT_TYPE_FIRST_DROPPED || type == INSECT_TYPE_EXTRA_DROPPED) &&
               (this->insectFlags & INSECT_FLAG_0) && this->lifeTimer <= 0 && this->actionTimer <= 0 &&
               this->actor.floorHeight < BGCHECK_Y_MIN + 10.0f) {
        PRINTF_COLOR_WARNING();
        // "BG missing? To do Actor_delete"
        PRINTF("BG 抜け？ Actor_delete します(%s %d)\n", "../z_en_mushi.c", 1197);
        PRINTF_RST();
        Actor_delete(&this->actor);
    }
}

void En_Mushi_actor_move(Actor* thisx, PlayState* play) {
    EnInsect* this = (EnInsect*)thisx;
    s32 tmp;

    if (this->actor.child != NULL) {
        if (this->actor.child->update == NULL) {
            if (this->actor.child != &this->actor) {
                this->actor.child = NULL;
            }
        }
    }

    if (this->actionTimer > 0) {
        this->actionTimer--;
    }

    if (this->lifeTimer > 0) {
        this->lifeTimer--;
    }

    this->actionFunc(this, play);

    if (this->actor.update != NULL) {
        Actor_position_moveF(&this->actor);
        if (this->insectFlags & INSECT_FLAG_CRAWLING) {
            if (this->insectFlags & INSECT_FLAG_0) {
                if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                    reset_walk_SE_mushi(this);
                }
            } else {
                reset_walk_SE_mushi(this);
            }
        }

        tmp = 0;

        if (this->insectFlags & INSECT_FLAG_0) {
            tmp = UPDBGCHECKINFO_FLAG_2;
        }

        if (this->insectFlags & INSECT_FLAG_1) {
            tmp |= UPDBGCHECKINFO_FLAG_0;
        }

        if (tmp != 0) {
            tmp |= UPDBGCHECKINFO_FLAG_6;
            Actor_BGcheck2(play, &this->actor, 8.0f, 5.0f, 0.0f, tmp);
        }

        if (Actor_carry_check(&this->actor, play)) {
            this->actor.parent = NULL;
            tmp = PARAMS_GET_U(this->actor.params, 0, 2);

            if (tmp == INSECT_TYPE_FIRST_DROPPED || tmp == INSECT_TYPE_EXTRA_DROPPED) {
                Actor_delete(&this->actor);
            } else {
                mv_clear_init(this);
            }
        } else if (this->actor.xzDistToPlayer < 50.0f && this->actionFunc != mv_clear) {
            if (!(this->insectFlags & INSECT_FLAG_SOIL_CLOSE) && this->lifeTimer < 180) {
                CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
            }

            if (!(this->insectFlags & INSECT_FLAG_UNCATCHABLE) && Carry_counter < 4 &&
                check_carryArea(this, play) &&
                // GI_MAX in this case allows the player to catch the actor in a bottle
                Actor_carry_request_set2(&this->actor, play, GI_MAX, 60.0f, 30.0f)) {
                Carry_counter++;
            }
        }

        Actor_world_to_eye(&this->actor, 0.0f);
    }
}

void En_Mushi_actor_draw(Actor* thisx, PlayState* play) {
    EnInsect* this = (EnInsect*)thisx;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, NULL);
    CollisionCheck_Uty_convJntSphL2G(0, &this->collider);
    Carry_counter = 0;
}
