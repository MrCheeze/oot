/*
 * File: z_en_fish.c
 * Overlay: ovl_En_Fish
 * Description: Fish
 */

#include "z_en_fish.h"
#include "global.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "terminal.h"

#define FLAGS 0

void En_Sakana_actor_ct(Actor* thisx, PlayState* play);
void En_Sakana_actor_dt(Actor* thisx, PlayState* play2);
void En_Sakana_actor_move(Actor* thisx, PlayState* play);
void En_Sakana_actor_draw(Actor* thisx, PlayState* play);

static void mvSet_stop(EnFish* this);
static void mv_stop(EnFish* this, PlayState* play);
void mvSet_go(EnFish* this);
static void mv_go(EnFish* this, PlayState* play);
void mvSet_away(EnFish* this);
static void mv_away(EnFish* this, PlayState* play);
void mvSet_pet(EnFish* this);
void mv_pet(EnFish* this, PlayState* play);
static void mvSet_drop(EnFish* this);
static void mv_drop(EnFish* this, PlayState* play);
void mvSet_pichipichi(EnFish* this);
void mv_pichipichi(EnFish* this, PlayState* play);
void mvSet_taisan(EnFish* this);
void mv_taisan(EnFish* this, PlayState* play);
void mvSet_slow(EnFish* this);
void mv_slow(EnFish* this, PlayState* play);

// Used in the cutscene functions
static Actor* DEMO_actor = NULL;
static f32 DEMO_offset_posY = 0.0f;
static f32 DEMO_offset_spdY = 0.0f;

static ColliderJntSphElementInit CrossSphElemDt_sakana[1] = {
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

static ColliderJntSphInit CrossSphDt_sakana = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    CrossSphElemDt_sakana,
};

ActorProfile En_Fish_Profile = {
    /**/ ACTOR_EN_FISH,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnFish),
    /**/ En_Sakana_actor_ct,
    /**/ En_Sakana_actor_dt,
    /**/ En_Sakana_actor_move,
    /**/ En_Sakana_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 900, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 40, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 700, ICHAIN_STOP),
};

static f32 get_distXZ_2(Vec3f* v1, Vec3f* v2) {
    return SQ(v1->x - v2->x) + SQ(v1->z - v2->z);
}

void init_skelton_swim(EnFish* this) {
    Skeleton_Info2_init(&this->skelAnime, &gFishInWaterAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gFishInWaterAnim),
                     ANIMMODE_LOOP_INTERP, 2.0f);
}

void init_skelton_jump(EnFish* this) {
    Skeleton_Info2_init(&this->skelAnime, &gFishOutOfWaterAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gFishOutOfWaterAnim),
                     ANIMMODE_LOOP_INTERP, 2.0f);
}

void set_clearMode(EnFish* this) {
    this->respawnTimer = 400;
    Actor_set_scale(&this->actor, 0.001f);
    this->actor.draw = NULL;
}

void demoMode_ct(EnFish* this) {
    Actor* thisx = &this->actor;

    if (DEMO_actor == NULL) {
        DEMO_actor = thisx;
        Actor_set_scale(thisx, 0.01f);
        thisx->draw = En_Sakana_actor_draw;
        thisx->shape.rot.x = 0;
        thisx->shape.rot.y = -0x6410;
        thisx->shape.rot.z = 0x4000;
        thisx->shape.yOffset = 600.0f;
        DEMO_offset_posY = 10.0f;
        DEMO_offset_spdY = 0.0f;
        thisx->flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        init_skelton_jump(this);
    }
}

void demoMode_dt(EnFish* this) {
    DEMO_actor = NULL;
    DEMO_offset_posY = 0.0f;
    DEMO_offset_spdY = 0.0f;
}

void En_Sakana_actor_ct(Actor* thisx, PlayState* play) {
    EnFish* this = (EnFish*)thisx;
    s16 params = this->actor.params;

    ValueSet_process(&this->actor, value_init);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gFishSkel, &gFishInWaterAnim, this->jointTable, this->morphTable, 7);
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &CrossSphDt_sakana, this->colliderItems);
    this->actor.colChkInfo.mass = 50;
    this->slowPhase = fqrand() * (0xFFFF + 0.5f);
    this->fastPhase = fqrand() * (0xFFFF + 0.5f);

    if (params == FISH_DROPPED) {
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 8.0f);
        mvSet_drop(this);
    } else if (params == FISH_SWIMMING_UNIQUE) {
        mvSet_slow(this);
    } else {
        mvSet_stop(this);
    }
}

void En_Sakana_actor_dt(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnFish* this = (EnFish*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

void yuragi_updown(EnFish* this) {
    this->actor.shape.yOffset += (sin_s(this->slowPhase) * 10.0f + sin_s(this->fastPhase) * 5.0f);
    this->actor.shape.yOffset = CLAMP(this->actor.shape.yOffset, -200.0f, 200.0f);
}

static s32 check_carryArea(EnFish* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    Vec3f sp1C;

    if (this->actor.xzDistToPlayer < 32.0f) {
        sp1C.x = (sin_s(this->actor.yawTowardsPlayer + 0x8000) * 16.0f) + player->actor.world.pos.x;
        sp1C.y = player->actor.world.pos.y;
        sp1C.z = (cos_s(this->actor.yawTowardsPlayer + 0x8000) * 16.0f) + player->actor.world.pos.z;

        //! @bug: this check is superfluous: it is automatically satisfied if the coarse check is satisfied. It may have
        //! been intended to check the actor is in front of Player, but yawTowardsPlayer does not depend on Player's
        //! world rotation.
        if (get_distXZ_2(&sp1C, &this->actor.world.pos) <= SQ(20.0f)) {
            return true;
        }
    }

    return false;
}

s32 check_away(EnFish* this, PlayState* play) {
    return (this->actor.xzDistToPlayer < 60.0f);
}

// Respawning type functions

static void mvSet_stop(EnFish* this) {
    this->actor.gravity = 0.0f;
    this->actor.minVelocityY = 0.0f;
    this->timer = get_random_timer(5, 35);
    this->unk_250 = 0;
    init_skelton_swim(this);
    this->actionFunc = mv_stop;
}

static void mv_stop(EnFish* this, PlayState* play) {
    yuragi_updown(this);
    add_calc(&this->actor.speed, 0.0f, 0.05f, 0.3f, 0.0f);
    this->skelAnime.playSpeed = CLAMP_MAX(this->actor.speed * 1.4f + 0.8f, 2.0f);
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->timer <= 0) {
        mvSet_go(this);
    } else if (&this->actor == this->actor.child) {
        mvSet_pet(this);
    } else if (check_away(this, play)) {
        mvSet_away(this);
    }
}

// The three following actionfunctions also turn the yaw to home if the fish is too far from it.

void mvSet_go(EnFish* this) {
    this->actor.gravity = 0.0f;
    this->actor.minVelocityY = 0.0f;
    this->timer = get_random_timer(15, 45);
    this->unk_250 = 0;
    init_skelton_swim(this);
    this->actionFunc = mv_go;
}

static void mv_go(EnFish* this, PlayState* play) {
    s32 pad;

    yuragi_updown(this);
    add_calc(&this->actor.speed, 1.8f, 0.08f, 0.4f, 0.0f);

    if ((get_distXZ_2(&this->actor.world.pos, &this->actor.home.pos) > SQ(80.0f)) || (this->timer < 4)) {
        chase_s3(&this->actor.world.rot.y, search_position_angleY(&this->actor.world.pos, &this->actor.home.pos),
                          3000);
    } else if ((this->actor.child != NULL) && (&this->actor != this->actor.child)) {
        chase_s3(&this->actor.world.rot.y,
                          search_position_angleY(&this->actor.world.pos, &this->actor.child->world.pos), 3000);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->skelAnime.playSpeed = CLAMP_MAX(this->actor.speed * 1.5f + 0.8f, 4.0f);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->timer <= 0) {
        mvSet_stop(this);
    } else if (&this->actor == this->actor.child) {
        mvSet_pet(this);
    } else if (check_away(this, play)) {
        mvSet_away(this);
    }
}

void mvSet_away(EnFish* this) {
    this->actor.gravity = 0.0f;
    this->actor.minVelocityY = 0.0f;
    this->timer = get_random_timer(10, 40);
    this->unk_250 = 0;
    init_skelton_swim(this);
    this->actionFunc = mv_away;
}

static void mv_away(EnFish* this, PlayState* play) {
    s32 pad;
    s16 pad2;
    s16 frames;
    s16 yaw;
    s16 playerClose;

    yuragi_updown(this);
    playerClose = check_away(this, play);
    add_calc(&this->actor.speed, 4.2f, 0.08f, 1.4f, 0.0f);

    if (get_distXZ_2(&this->actor.world.pos, &this->actor.home.pos) > SQ(160.0f)) {
        yaw = search_position_angleY(&this->actor.world.pos, &this->actor.home.pos);
        chase_s3(&this->actor.world.rot.y, yaw, 3000);
    } else if ((this->actor.child != NULL) && (&this->actor != this->actor.child)) {
        yaw = search_position_angleY(&this->actor.world.pos, &this->actor.child->world.pos);
        chase_s3(&this->actor.world.rot.y, yaw, 2000);
    } else if (playerClose) {
        yaw = this->actor.yawTowardsPlayer + 0x8000;
        frames = play->state.frames;

        if (frames & 0x10) {
            if (frames & 0x20) {
                yaw += 0x2000;
            }
        } else {
            if (frames & 0x20) {
                yaw -= 0x2000;
            }
        }
        if (play) {}
        chase_s3(&this->actor.world.rot.y, yaw, 2000);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->skelAnime.playSpeed = CLAMP_MAX(this->actor.speed * 1.5f + 0.8f, 4.0f);

    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((this->timer <= 0) || !playerClose) {
        mvSet_stop(this);
    } else if (&this->actor == this->actor.child) {
        mvSet_pet(this);
    }
}

void mvSet_pet(EnFish* this) {
    this->actor.gravity = 0.0f;
    this->actor.minVelocityY = 0.0f;
    init_skelton_swim(this);
    this->timer = get_random_timer(10, 40);
    this->unk_250 = 0;
    this->actionFunc = mv_pet;
}

void mv_pet(EnFish* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 pad2;
    Vec3f sp38;
    s16 yaw;
    s16 temp_a0_2;

    yuragi_updown(this);
    add_calc(&this->actor.speed, 1.8f, 0.1f, 0.5f, 0.0f);

    if (get_distXZ_2(&this->actor.world.pos, &this->actor.home.pos) > SQ(80.0f)) {
        yaw = search_position_angleY(&this->actor.world.pos, &this->actor.home.pos);
        chase_s3(&this->actor.world.rot.y, yaw, 3000);
    } else {
        if ((s16)play->state.frames & 0x40) {
            temp_a0_2 = (this->actor.yawTowardsPlayer + 0x9000);
        } else {
            temp_a0_2 = (this->actor.yawTowardsPlayer + 0x7000);
        }

        sp38.x = player->actor.world.pos.x + (sin_s(temp_a0_2) * 20.0f);
        sp38.y = player->actor.world.pos.y;
        sp38.z = player->actor.world.pos.z + (cos_s(temp_a0_2) * 20.0f);

        yaw = search_position_angleY(&this->actor.world.pos, &sp38);
        chase_s3(&this->actor.world.rot.y, yaw, 3000);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->skelAnime.playSpeed = CLAMP_MAX((this->actor.speed * 1.5f) + 0.8f, 4.0f);

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->timer <= 0) {
        mvSet_stop(this);
    }
}

// Dropped type functions

static void mvSet_drop(EnFish* this) {
    this->actor.gravity = -1.0f;
    this->actor.minVelocityY = -10.0f;
    this->actor.shape.yOffset = 0.0f;
    init_skelton_jump(this);
    this->unk_250 = UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2;
    this->actionFunc = mv_drop;
    this->timer = 300;
}

static void mv_drop(EnFish* this, PlayState* play) {
    add_calc(&this->actor.speed, 0.0f, 0.1f, 0.1f, 0.0f);
    chase_s3(&this->actor.world.rot.x, 0x4000, 100);
    chase_s3(&this->actor.world.rot.z, -0x4000, 100);
    this->actor.shape.rot.x = this->actor.world.rot.x;
    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->actor.shape.rot.z = this->actor.world.rot.z;
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->timer = 400;
        mvSet_pichipichi(this);
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
        mvSet_taisan(this);
    } else if ((this->timer <= 0) && (this->actor.params == FISH_DROPPED) &&
               (this->actor.floorHeight < BGCHECK_Y_MIN + 10.0f)) {
        PRINTF_COLOR_WARNING();
        // "BG missing? Running Actor_delete"
        PRINTF("BG 抜け？ Actor_delete します(%s %d)\n", "../z_en_sakana.c", 822);
        PRINTF_RST();
        Actor_delete(&this->actor);
    }
}

/**
 * If the fish is on a floor, this function is looped back to by mv_pichipichi to set a new flopping
 * height and whether the sound should play again.
 */
void mvSet_pichipichi(EnFish* this) {
    s32 pad;
    f32 randomFloat;
    s32 playSfx;

    this->actor.gravity = -1.0f;
    this->actor.minVelocityY = -10.0f;
    randomFloat = fqrand();

    if (randomFloat < 0.1f) {
        this->actor.velocity.y = (fqrand() * 3.0f) + 2.5f;
        playSfx = true;
    } else if (randomFloat < 0.2f) {
        this->actor.velocity.y = (fqrand() * 1.2f) + 0.2f;
        playSfx = true;
    } else {
        this->actor.velocity.y = 0.0f;

        if (fqrand() < 0.2f) {
            playSfx = true;
        } else {
            playSfx = false;
        }
    }

    this->actor.shape.yOffset = 300.0f;
    init_skelton_jump(this);
    this->actionFunc = mv_pichipichi;
    this->unk_250 = UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2;

    if (playSfx && (this->actor.draw != NULL)) {
        Actor_SE_set(&this->actor, NA_SE_EV_FISH_LEAP);
    }
}

void mv_pichipichi(EnFish* this, PlayState* play) {
    s32 pad;
    s16 frames = play->state.frames;
    s16 targetXRot;

    add_calc(&this->actor.speed, fqrand() * 0.2f, 0.1f, 0.1f, 0.0f);

    targetXRot = (s16)((((frames >> 5) & 2) | ((frames >> 2) & 1)) << 0xB) * 0.3f;

    if (frames & 4) {
        targetXRot = -targetXRot;
    }

    chase_s3(&this->actor.world.rot.x, targetXRot, 4000);
    chase_s3(&this->actor.world.rot.z, 0x4000, 1000);
    this->actor.world.rot.y +=
        (s16)(((sin_s(this->slowPhase) * 2000.0f) + (sin_s(this->fastPhase) * 1000.0f)) * fqrand());
    this->actor.shape.rot = this->actor.world.rot;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->timer <= 0) {
        Actor_delete(&this->actor);
        return;
    }

    if (this->timer <= 60) {
        // Blink when about to disappear
        if (frames & 4) {
            this->actor.draw = En_Sakana_actor_draw;
        } else {
            this->actor.draw = NULL;
        }
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
        mvSet_taisan(this);
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        mvSet_pichipichi(this);
    }
}

void mvSet_taisan(EnFish* this) {
    this->actor.home.pos = this->actor.world.pos;
    this->actor.gravity = 0.0f;
    this->actor.minVelocityY = 0.0f;
    this->actor.shape.yOffset = 0.0f;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->timer = 200;
    init_skelton_swim(this);
    this->actionFunc = mv_taisan;
    this->unk_250 = UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2;
}

void mv_taisan(EnFish* this, PlayState* play) {
    s32 pad;

    add_calc(&this->actor.speed, 2.8f, 0.1f, 0.4f, 0.0f);

    // If touching wall or not in water, turn back and slow down for one frame.
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || !(this->actor.bgCheckFlags & BGCHECKFLAG_WATER)) {
        this->actor.home.rot.y = search_position_angleY(&this->actor.world.pos, &this->actor.home.pos);
        this->actor.speed *= 0.5f;
    }

    chase_s3(&this->actor.world.rot.x, 0, 1500);
    chase_s3(&this->actor.world.rot.y, this->actor.home.rot.y, 3000);
    chase_s3(&this->actor.world.rot.z, 0, 1000);

    this->actor.shape.rot = this->actor.world.rot;

    // Raise if on a floor.
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        chase_f(&this->actor.world.pos.y, this->actor.home.pos.y - 4.0f, 2.0f);
    } else {
        chase_f(&this->actor.world.pos.y, this->actor.home.pos.y - 10.0f, 2.0f);
    }

    // Shrink when close to disappearing.
    if (this->timer < 100) {
        Actor_set_scale(&this->actor, this->actor.scale.x * 0.982f);
    }

    this->skelAnime.playSpeed = CLAMP_MAX((this->actor.speed * 1.5f) + 1.0f, 4.0f);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->timer <= 0) {
        Actor_delete(&this->actor);
    }
}

// Unique type functions

void mvSet_slow(EnFish* this) {
    this->actor.gravity = 0.0f;
    this->actor.minVelocityY = 0.0f;
    this->timer = get_random_timer(5, 35);
    this->unk_250 = 0;
    init_skelton_swim(this);
    this->actionFunc = mv_slow;
}

void mv_slow(EnFish* this, PlayState* play) {
    static f32 slow_data[] = { 0.0f, 0.04f, 0.09f };
    static f32 speedMoving[] = { 0.5f, 0.1f, 0.15f };
    f32 playSpeed;
    u32 frames = play->gameplayFrames;
    f32* speed;
    s32 pad2;
    f32 extraPlaySpeed;
    s32 pad3;

    if (this->actor.xzDistToPlayer < 60.0f) {
        if (this->timer < 12) {
            speed = speedMoving;
        } else {
            speed = slow_data;
        }
    } else {
        if (this->timer < 4) {
            speed = speedMoving;
        } else {
            speed = slow_data;
        }
    }

    yuragi_updown(this);
    add_calc(&this->actor.speed, speed[0], speed[1], speed[2], 0.0f);

    extraPlaySpeed = 0.0f;

    if ((get_distXZ_2(&this->actor.world.pos, &this->actor.home.pos) > SQ(15.0f))) {
        if (!chase_angle(&this->actor.world.rot.y, search_position_angleY(&this->actor.world.pos, &this->actor.home.pos),
                                200)) {
            extraPlaySpeed = 0.5f;
        }
    } else if ((this->timer < 4) && !chase_angle(&this->actor.world.rot.y, frames * 0x80, 100)) {
        extraPlaySpeed = 0.5f;
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    playSpeed = (this->actor.speed * 1.2f) + 0.2f + extraPlaySpeed;
    this->skelAnime.playSpeed = CLAMP(playSpeed, 1.5f, 0.5);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->timer <= 0) {
        this->timer = get_random_timer(5, 80);
    }
}

// Cutscene functions

void demo_pichi(EnFish* this, PlayState* play) {
    f32 sp24 = sin_s(this->slowPhase);
    f32 sp20 = sin_s(this->fastPhase);

    DEMO_offset_posY += DEMO_offset_spdY;

    if (DEMO_offset_posY <= 1.0f) {
        DEMO_offset_posY = 1.0f;

        if (fqrand() < 0.1f) {
            DEMO_offset_spdY = (fqrand() * 3.0f) + 2.0f;
            Actor_SE_set(&this->actor, NA_SE_EV_FISH_LEAP);
        } else {
            DEMO_offset_spdY = 0.0f;
        }
    } else {
        DEMO_offset_spdY -= 0.4f;
    }

    this->skelAnime.playSpeed = ((sp24 + sp20) * 0.5f) + 2.0f;
    Skeleton_Info2_anime_play(&this->skelAnime);
}

void demo_vacuum(EnFish* this, PlayState* play) {
    s32 pad;
    f32 sp28 = sin_s(this->slowPhase);
    f32 sp24 = sin_s(this->fastPhase);

    this->actor.shape.rot.x -= 500;
    this->actor.shape.rot.z += 100;
    chase_f(&DEMO_offset_posY, 0.0f, 1.0f);
    this->skelAnime.playSpeed = ((sp28 + sp24) * 0.5f) + 2.0f;
    Skeleton_Info2_anime_play(&this->skelAnime);
}

void moveSub_demo(EnFish* this, PlayState* play) {
    s32 pad;
    s32 pad2;
    CsCmdActorCue* cue = play->csCtx.actorCues[1];
    Vec3f startPos;
    Vec3f endPos;
    f32 lerp;
    s32 bgId;

    if (play) {}

    if (cue == NULL) {
        // "Warning : DEMO ended without dousa (action) 3 termination being called"
        PRINTF("Warning : dousa 3 消滅 が呼ばれずにデモが終了した(%s %d)(arg_data 0x%04x)\n", "../z_en_sakana.c", 1169,
               this->actor.params);
        demoMode_dt(this);
        Actor_delete(&this->actor);
        return;
    }

    this->slowPhase += 0x111;
    this->fastPhase += 0x500;

    switch (cue->id) {
        case 1:
            demo_pichi(this, play);
            break;
        case 2:
            demo_vacuum(this, play);
            break;
        case 3:
            // "DEMO fish termination"
            PRINTF("デモ魚消滅\n");
            demoMode_dt(this);
            Actor_delete(&this->actor);
            return;
        default:
            // "Improper DEMO action"
            PRINTF("不正なデモ動作(%s %d)(arg_data 0x%04x)\n", "../z_en_sakana.c", 1200, this->actor.params);
            break;
    }

    startPos.x = cue->startPos.x;
    startPos.y = cue->startPos.y;
    startPos.z = cue->startPos.z;

    endPos.x = cue->endPos.x;
    endPos.y = cue->endPos.y;
    endPos.z = cue->endPos.z;

    lerp = get_parcent(cue->endFrame, cue->startFrame, play->csCtx.curFrame);

    this->actor.world.pos.x = (endPos.x - startPos.x) * lerp + startPos.x;
    this->actor.world.pos.y = (endPos.y - startPos.y) * lerp + startPos.y + DEMO_offset_posY;
    this->actor.world.pos.z = (endPos.z - startPos.z) * lerp + startPos.z;

    this->actor.floorHeight =
        T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &this->actor.world.pos);
}

// Update functions and Draw

void moveSub_normal(EnFish* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
    }

    this->slowPhase += 0x111;
    this->fastPhase += 0x500;

    if ((this->actor.child != NULL) && (this->actor.child->update == NULL) && (&this->actor != this->actor.child)) {
        this->actor.child = NULL;
    }

    if ((this->actionFunc == NULL) || (this->actionFunc(this, play), (this->actor.update != NULL))) {
        Actor_position_moveF(&this->actor);

        if (this->unk_250 != 0) {
            Actor_BGcheck2(play, &this->actor, 17.5f, 4.0f, 0.0f, this->unk_250);
        }

        if (this->actor.xzDistToPlayer < 70.0f) {
            CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        }

        Actor_world_to_eye(&this->actor, this->actor.shape.yOffset * 0.01f);

        if (Actor_carry_check(&this->actor, play)) {
            this->actor.parent = NULL;

            if (this->actor.params == FISH_DROPPED) {
                Actor_delete(&this->actor);
                return;
            }

            set_clearMode(this);
        } else if (check_carryArea(this, play)) {
            // GI_MAX in this case allows the player to catch the actor in a bottle
            Actor_carry_request_set2(&this->actor, play, GI_MAX, 80.0f, 20.0f);
        }
    }
}

void moveSub_clear(EnFish* this, PlayState* play) {
    if (this->actor.params == FISH_SWIMMING_UNIQUE) {
        Actor_delete(&this->actor);
        return;
    }

    if ((this->actor.child != NULL) && (this->actor.child->update == NULL) && (&this->actor != this->actor.child)) {
        this->actor.child = NULL;
    }

    if ((this->actionFunc == NULL) || (this->actionFunc(this, play), (this->actor.update != NULL))) {
        Actor_position_moveF(&this->actor);

        if (this->respawnTimer == 20) {
            this->actor.draw = En_Sakana_actor_draw;
        } else if (this->respawnTimer == 0) {
            Actor_set_scale(&this->actor, 0.01f);
        } else if (this->respawnTimer < 20) {
            Actor_set_scale(&this->actor, CLAMP_MAX(this->actor.scale.x + 0.001f, 0.01f));
        }
    }
}

void En_Sakana_actor_move(Actor* thisx, PlayState* play) {
    EnFish* this = (EnFish*)thisx;

    if ((DEMO_actor == NULL) && (this->actor.params == FISH_DROPPED) && (play->csCtx.state != CS_STATE_IDLE) &&
        (play->csCtx.actorCues[1] != NULL)) {
        demoMode_ct(this);
    }

    if ((DEMO_actor != NULL) && (&this->actor == DEMO_actor)) {
        moveSub_demo(this, play);
    } else if (this->respawnTimer > 0) {
        this->respawnTimer--;
        moveSub_clear(this, play);
    } else {
        moveSub_normal(this, play);
    }
}

void En_Sakana_actor_draw(Actor* thisx, PlayState* play) {
    EnFish* this = (EnFish*)thisx;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, NULL);
    CollisionCheck_Uty_convJntSphL2G(0, &this->collider);
}
