/*
 * File: z_en_dog.c
 * Overlay: ovl_En_Dog
 * Description: Dog
 */

#include "z_en_dog.h"
#include "assets/objects/object_dog/object_dog.h"

#define FLAGS 0

void En_Dog_actor_ct(Actor* thisx, PlayState* play);
void En_Dog_actor_dt(Actor* thisx, PlayState* play);
void En_Dog_actor_move(Actor* thisx, PlayState* play);
void En_Dog_actor_draw(Actor* thisx, PlayState* play);

void walk(EnDog* this, PlayState* play);
static void wait(EnDog* this, PlayState* play);
void player_chase(EnDog* this, PlayState* play);
void run_away(EnDog* this, PlayState* play);
void end_run_away(EnDog* this, PlayState* play);
void sit(EnDog* this, PlayState* play);

ActorProfile En_Dog_Profile = {
    /**/ ACTOR_EN_DOG,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DOG,
    /**/ sizeof(EnDog),
    /**/ En_Dog_actor_ct,
    /**/ En_Dog_actor_dt,
    /**/ En_Dog_actor_move,
    /**/ En_Dog_actor_draw,
};

static ColliderCylinderInit DogCoInfoData = {
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
    { 16, 20, 0, { 0 } },
};

static CollisionCheckInfoInit2 DogStatusData = { 0, 0, 0, 0, 50 };

typedef enum EnDogAnimation {
    /* 0 */ ENDOG_ANIM_0,
    /* 1 */ ENDOG_ANIM_1,
    /* 2 */ ENDOG_ANIM_2,
    /* 3 */ ENDOG_ANIM_3,
    /* 4 */ ENDOG_ANIM_4,
    /* 5 */ ENDOG_ANIM_5,
    /* 6 */ ENDOG_ANIM_6,
    /* 7 */ ENDOG_ANIM_7
} EnDogAnimation;

static AnimationInfo anime_ct_data[] = {
    { &gDogWalkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gDogWalkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -6.0f },
    { &gDogRunAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -6.0f },
    { &gDogBarkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -6.0f },
    { &gDogSitAnim, 1.0f, 0.0f, 4.0f, ANIMMODE_ONCE, -6.0f },
    { &gDogSitAnim, 1.0f, 5.0f, 25.0f, ANIMMODE_LOOP_PARTIAL, -6.0f },
    { &gDogBowAnim, 1.0f, 0.0f, 6.0f, ANIMMODE_ONCE, -6.0f },
    { &gDogBow2Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -6.0f },
};

typedef enum DogBehavior {
    /* 0x00 */ DOG_WALK,
    /* 0x01 */ DOG_RUN,
    /* 0x02 */ DOG_BARK,
    /* 0x03 */ DOG_SIT,
    /* 0x04 */ DOG_SIT_2,
    /* 0x05 */ DOG_BOW,
    /* 0x06 */ DOG_BOW_2
} DogBehavior;

void SE_set_walk(EnDog* this) {
    AnimationHeader* walk = &gDogWalkAnim;

    if (this->skelAnime.animation == walk) {
        if ((this->skelAnime.curFrame == 1.0f) || (this->skelAnime.curFrame == 7.0f)) {
            Actor_SE_set(&this->actor, NA_SE_EV_CHIBI_WALK);
        }
    }
}

void SE_set_run(EnDog* this) {
    AnimationHeader* run = &gDogRunAnim;

    if (this->skelAnime.animation == run) {
        if ((this->skelAnime.curFrame == 2.0f) || (this->skelAnime.curFrame == 4.0f)) {
            Actor_SE_set(&this->actor, NA_SE_EV_CHIBI_WALK);
        }
    }
}

void SE_set_bark(EnDog* this) {
    AnimationHeader* bark = &gDogBarkAnim;

    if (this->skelAnime.animation == bark) {
        if ((this->skelAnime.curFrame == 13.0f) || (this->skelAnime.curFrame == 19.0f)) {
            Actor_SE_set(&this->actor, NA_SE_EV_SMALL_DOG_BARK);
        }
    }
}

s32 dog_chg_anime(EnDog* this) {
    s32 animation;

    if (this->behavior != this->nextBehavior) {
        if (this->nextBehavior == DOG_SIT_2) {
            this->nextBehavior = DOG_SIT;
        }
        if (this->nextBehavior == DOG_BOW_2) {
            this->nextBehavior = DOG_BOW;
        }

        this->behavior = this->nextBehavior;
        switch (this->behavior) {
            case DOG_WALK:
                animation = ENDOG_ANIM_1;
                break;
            case DOG_RUN:
                animation = ENDOG_ANIM_2;
                break;
            case DOG_BARK:
                animation = ENDOG_ANIM_3;
                break;
            case DOG_SIT:
                animation = ENDOG_ANIM_4;
                break;
            case DOG_BOW:
                animation = ENDOG_ANIM_6;
                break;
        }
        npc_anime_ct(&this->skelAnime, anime_ct_data, animation);
    }

    switch (this->behavior) {
        case DOG_SIT:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, anime_ct_data, ENDOG_ANIM_5);
                this->behavior = this->nextBehavior = DOG_SIT_2;
            }
            break;
        case DOG_BOW:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, anime_ct_data, ENDOG_ANIM_7);
                this->behavior = this->nextBehavior = DOG_BOW_2;
            }
            break;
        case DOG_WALK:
            SE_set_walk(this);
            break;
        case DOG_RUN:
            SE_set_run(this);
            break;
        case DOG_BARK:
            SE_set_bark(this);
            break;
    }
    return 0;
}

static s8 hitcheck(EnDog* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        return 2;
    }

    if (play->sceneId == SCENE_MARKET_DAY) {
        return 0;
    }

    if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
        this->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;
        if (z_common_data.dogParams != 0) {
            return 0;
        }
        z_common_data.dogParams = PARAMS_GET_S(this->actor.params, 0, 15);
        return 1;
    }

    return 0;
}

static s32 get_next_path_point(EnDog* this, PlayState* play) {
    s32 change;

    if (this->path == NULL) {
        return 0;
    }

    if (this->reverse) {
        change = -1;
    } else {
        change = 1;
    }

    this->waypoint += change;

    if (this->reverse) {
        if (this->waypoint < 0) {
            this->waypoint = this->path->count - 1;
        }
    } else {
        if ((this->path->count - 1) < this->waypoint) {
            this->waypoint = 0;
        }
    }

    return 1;
}

s32 dog_path_move(EnDog* this, PlayState* play) {
    s16 targetYaw;
    f32 waypointDistSq;

    waypointDistSq = path_move(&this->actor, this->path, this->waypoint, &targetYaw);
    add_calc_short_angle2(&this->actor.world.rot.y, targetYaw, 10, 1000, 1);

    if ((waypointDistSq > 0.0f) && (waypointDistSq < 1000.0f)) {
        return get_next_path_point(this, play);
    } else {
        return 0;
    }
}

void En_Dog_actor_ct(Actor* thisx, PlayState* play) {
    EnDog* this = (EnDog*)thisx;
    s16 followingDog;
    s32 pad;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 24.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gDogSkel, NULL, this->jointTable, this->morphTable, 13);
    npc_anime_ct(&this->skelAnime, anime_ct_data, ENDOG_ANIM_0);

    if (!PARAMS_GET_NOSHIFT(this->actor.params, 15, 1)) {
        this->actor.params = (this->actor.params & ~(0xF << 8)) | ((PARAMS_GET_S(this->actor.params, 8, 4) + 1) << 8);
    }

    followingDog = ((z_common_data.dogParams & 0x0F00) >> 8);
    if (followingDog == PARAMS_GET_S(this->actor.params, 8, 4) && !PARAMS_GET_NOSHIFT(this->actor.params, 15, 1)) {
        Actor_delete(&this->actor);
        return;
    }

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &DogCoInfoData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, NULL, &DogStatusData);
    Actor_set_scale(&this->actor, 0.0075f);
    this->waypoint = 0;
    this->actor.gravity = -1.0f;
    this->path = get_path_data(play, PARAMS_GET_S(this->actor.params, 4, 4), 0xF);

    switch (play->sceneId) {
        case SCENE_MARKET_NIGHT:
            if ((!z_common_data.dogIsLost) && PARAMS_GET_S(this->actor.params, 8, 4) == 1) {
                Actor_delete(&this->actor);
            }
            break;
        case SCENE_DOG_LADY_HOUSE: // Richard's Home
            if (!PARAMS_GET_NOSHIFT(this->actor.params, 15, 1)) {
                if (!z_common_data.dogIsLost) {
                    this->nextBehavior = DOG_SIT;
                    this->actionFunc = sit;
                    this->actor.speed = 0.0f;
                    return;
                } else {
                    Actor_delete(&this->actor);
                    return;
                }
            }
            break;
    }

    if (PARAMS_GET_NOSHIFT(this->actor.params, 15, 1)) {
        this->nextBehavior = DOG_WALK;
        this->actionFunc = player_chase;
    } else {
        this->nextBehavior = DOG_SIT;
        this->actionFunc = wait;
    }
}

void En_Dog_actor_dt(Actor* thisx, PlayState* play) {
    EnDog* this = (EnDog*)thisx;
    ClObjPipe_dt(play, &this->collider);
}

void walk(EnDog* this, PlayState* play) {
    s32 behaviors[] = { DOG_SIT, DOG_BOW, DOG_BARK };
    s32 unused[] = { 40, 80, 20 };
    f32 speedXZ;
    s32 frame;

    if (hitcheck(this, play) == 1) {
        this->actionFunc = player_chase;
    }

    if (DECR(this->behaviorTimer) != 0) {
        if (this->nextBehavior == DOG_WALK) {
            speedXZ = 1.0f;
        } else {
            speedXZ = 4.0f;
        }
        add_calc(&this->actor.speed, speedXZ, 0.4f, 1.0f, 0.0f);
        dog_path_move(this, play);
        this->actor.shape.rot = this->actor.world.rot;

        // Used to change between two text boxes for Richard's owner in the Market Day scene
        // depending on where he is on his path. En_Hy checks these event flags.
        if (this->waypoint < 9) {
            // Richard is close to her, text says something about his coat
            SET_EVENTINF(EVENTINF_30);
        } else {
            // Richard is far, text says something about running fast
            CLEAR_EVENTINF(EVENTINF_30);
        }
    } else {
        frame = play->state.frames % 3;
        this->nextBehavior = behaviors[frame];
        // no clue why they're using the behavior id to calculate timer. possibly meant to use the unused array?
        this->behaviorTimer = get_random_timer(60, behaviors[frame]);
        this->actionFunc = wait;
    }
}

static void wait(EnDog* this, PlayState* play) {
    if (hitcheck(this, play) == 1) {
        this->actionFunc = player_chase;
    }

    if (DECR(this->behaviorTimer) == 0) {
        this->behaviorTimer = get_random_timer(200, 100);
        if (play->state.frames % 2) {
            this->nextBehavior = DOG_WALK;
        } else {
            this->nextBehavior = DOG_RUN;
        }

        if (this->nextBehavior == DOG_RUN) {
            this->behaviorTimer /= 2;
        }
        this->actionFunc = walk;
    }
    add_calc(&this->actor.speed, 0.0f, 0.4f, 1.0f, 0.0f);
}

void player_chase(EnDog* this, PlayState* play) {
    f32 speedXZ;

    if (z_common_data.dogParams == 0) {
        this->nextBehavior = DOG_SIT;
        this->actionFunc = sit;
        this->actor.speed = 0.0f;
        return;
    }

    if (this->actor.xzDistToPlayer > 400.0f) {
        if (this->nextBehavior != DOG_SIT && this->nextBehavior != DOG_SIT_2) {
            this->nextBehavior = DOG_BOW;
        }
        z_common_data.dogParams = 0;
        speedXZ = 0.0f;
    } else if (this->actor.xzDistToPlayer > 100.0f) {
        this->nextBehavior = DOG_RUN;
        speedXZ = 4.0f;
    } else if (this->actor.xzDistToPlayer < 40.0f) {
        if (this->nextBehavior != DOG_BOW && this->nextBehavior != DOG_BOW_2) {
            this->nextBehavior = DOG_BOW;
        }
        speedXZ = 0.0f;
    } else {
        this->nextBehavior = DOG_WALK;
        speedXZ = 1.0f;
    }

    add_calc2(&this->actor.speed, speedXZ, 0.6f, 1.0f);

    if (!(this->actor.xzDistToPlayer > 400.0f)) {
        add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 10, 1000, 1);
        this->actor.shape.rot = this->actor.world.rot;
    }
}

void run_away(EnDog* this, PlayState* play) {
    if (this->actor.xzDistToPlayer < 200.0f) {
        add_calc2(&this->actor.speed, 4.0f, 0.6f, 1.0f);
        add_calc_short_angle2(&this->actor.world.rot.y, (this->actor.yawTowardsPlayer ^ 0x8000), 10, 1000, 1);
    } else {
        this->actionFunc = end_run_away;
    }
    this->actor.shape.rot = this->actor.world.rot;
}

void end_run_away(EnDog* this, PlayState* play) {
    s16 rotTowardLink;
    s16 prevRotY;
    f32 absAngleDiff;

    // if the dog is more than 200 units away from Link, turn to face him then wait
    if (200.0f <= this->actor.xzDistToPlayer) {
        this->nextBehavior = DOG_WALK;

        add_calc2(&this->actor.speed, 1.0f, 0.6f, 1.0f);

        rotTowardLink = this->actor.yawTowardsPlayer;
        prevRotY = this->actor.world.rot.y;
        add_calc_short_angle2(&this->actor.world.rot.y, rotTowardLink, 10, 1000, 1);

        absAngleDiff = this->actor.world.rot.y;
        absAngleDiff -= prevRotY;
        absAngleDiff = fabsf(absAngleDiff);
        if (absAngleDiff < 200.0f) {
            this->nextBehavior = DOG_SIT;
            this->actionFunc = sit;
            this->actor.speed = 0.0f;
        }
    } else {
        this->nextBehavior = DOG_RUN;
        this->actionFunc = run_away;
    }
    this->actor.shape.rot = this->actor.world.rot;
}

void sit(EnDog* this, PlayState* play) {
    this->unusedAngle = (this->actor.yawTowardsPlayer - this->actor.shape.rot.y);

    // If another dog is following Link and he gets within 200 units of waiting dog, run away
    if ((z_common_data.dogParams != 0) && (this->actor.xzDistToPlayer < 200.0f)) {
        this->nextBehavior = DOG_RUN;
        this->actionFunc = run_away;
    }
}

void En_Dog_actor_move(Actor* thisx, PlayState* play) {
    EnDog* this = (EnDog*)thisx;
    s32 pad;

    dog_chg_anime(this);
    Skeleton_Info2_anime_play(&this->skelAnime);
    Actor_BGcheck2(play, &this->actor, this->collider.dim.radius, this->collider.dim.height * 0.5f, 0.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    Actor_position_moveF(&this->actor);
    this->actionFunc(this, play);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
}

void En_Dog_actor_draw(Actor* thisx, PlayState* play) {
    EnDog* this = (EnDog*)thisx;
    Color_RGBA8 colors[] = { { 255, 255, 200, 0 }, { 150, 100, 50, 0 } };

    OPEN_DISPS(play->state.gfxCtx, "../z_en_dog.c", 972);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, colors[PARAMS_GET_S(this->actor.params, 0, 4)].r,
                   colors[PARAMS_GET_S(this->actor.params, 0, 4)].g, colors[PARAMS_GET_S(this->actor.params, 0, 4)].b,
                   colors[PARAMS_GET_S(this->actor.params, 0, 4)].a);

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_dog.c", 994);
}
