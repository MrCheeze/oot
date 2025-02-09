/*
 *  File: z_bg_jya_goroiwa.c
 *  Overlay: ovl_Bg_Jya_Goroiwa
 *  Description: Rolling Boulder
 *  moves very slowly in some cases
 */

#include "z_bg_jya_goroiwa.h"
#include "assets/objects/object_goroiwa/object_goroiwa.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Jya_Goroiwa_actor_ct(Actor* thisx, PlayState* play);
void Bg_Jya_Goroiwa_actor_dt(Actor* thisx, PlayState* play);
void Bg_Jya_Goroiwa_actor_move(Actor* thisx, PlayState* play);
void Bg_Jya_Goroiwa_actor_draw(Actor* thisx, PlayState* play);

static void mv_stop(BgJyaGoroiwa* this, PlayState* play);
static void mv_walk(BgJyaGoroiwa* this, PlayState* play);

static void mv_stop_init(BgJyaGoroiwa* this);
static void mv_walk_init(BgJyaGoroiwa* this);
static void set_angle_goroiwa(BgJyaGoroiwa* this);
static void set_collisionPos_goroiwa(BgJyaGoroiwa* this);

ActorProfile Bg_Jya_Goroiwa_Profile = {
    /**/ ACTOR_BG_JYA_GOROIWA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GOROIWA,
    /**/ sizeof(BgJyaGoroiwa),
    /**/ Bg_Jya_Goroiwa_actor_ct,
    /**/ Bg_Jya_Goroiwa_actor_dt,
    /**/ Bg_Jya_Goroiwa_actor_move,
    /**/ Bg_Jya_Goroiwa_actor_draw,
};

static ColliderJntSphElementInit ClSphElemDt_goroiwa[] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 58 }, 100 },
    },
};

static ColliderJntSphInit ClSphDt_goroiwa = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    ClSphElemDt_goroiwa,
};

static CollisionCheckInfoInit StatusDt_goroiwa = { 1, 15, 0, MASS_HEAVY };

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

static void set_collisionPos_goroiwa(BgJyaGoroiwa* this) {
    Sphere16* worldSphere = &this->collider.elements[0].dim.worldSphere;

    worldSphere->center.x = this->actor.world.pos.x;
    worldSphere->center.y = this->actor.world.pos.y + 59.5f;
    worldSphere->center.z = this->actor.world.pos.z;
}

static void set_collision_goroiwa(BgJyaGoroiwa* this, PlayState* play) {
    s32 pad;

    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &ClSphDt_goroiwa, &this->colliderItem);
    set_collisionPos_goroiwa(this);
    this->collider.elements[0].dim.worldSphere.radius = 58;
}

static void set_angle_goroiwa(BgJyaGoroiwa* this) {
    f32 xDiff = this->actor.world.pos.x - this->actor.prevPos.x;

    this->actor.shape.rot.z -= 0x10000 / (119 * M_PI) * xDiff;
}

void Bg_Jya_Goroiwa_actor_ct(Actor* thisx, PlayState* play) {
    BgJyaGoroiwa* this = (BgJyaGoroiwa*)thisx;

    ValueSet_process(&this->actor, value_init);
    set_collision_goroiwa(this, play);
    this->actor.shape.rot.x = this->actor.shape.rot.y = this->actor.shape.rot.z = 0;
    CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &StatusDt_goroiwa);
    Shape_Info_init(&this->actor.shape, 595.0f, Actor_shadow_circle, 9.0f);
    this->actor.shape.shadowAlpha = 128;
    mv_walk_init(this);
}

void Bg_Jya_Goroiwa_actor_dt(Actor* thisx, PlayState* play) {
    BgJyaGoroiwa* this = (BgJyaGoroiwa*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

static void mv_walk_init(BgJyaGoroiwa* this) {
    this->actionFunc = mv_walk;
    this->collider.base.atFlags |= AT_ON;
    this->hasHit = false;
    this->speedFactor = 1.0f;
}

static void mv_walk(BgJyaGoroiwa* this, PlayState* play) {
    Actor* thisx = &this->actor;
    s16 relYawTowardsPlayer;
    f32 speedXZBaseSq = (-100.0f - thisx->world.pos.y) * 2.5f;
    f32 posYfac;

    if (speedXZBaseSq < 0.01f) {
        speedXZBaseSq = 0.01f;
    }

    thisx->speed = sqrtf(speedXZBaseSq) * this->speedFactor;
    thisx->velocity.x = sin_s(thisx->world.rot.y) * thisx->speed;
    thisx->velocity.z = cos_s(thisx->world.rot.y) * thisx->speed;

    thisx->world.pos.x += thisx->velocity.x;
    thisx->world.pos.z += thisx->velocity.z;

    if ((thisx->world.pos.x > 1466.0f) && (thisx->world.pos.x < 1673.0f)) {
        thisx->world.pos.y = -129.5f;
    } else {
        posYfac = 1569.0f - thisx->world.pos.x;
        posYfac = fabsf(posYfac) - 103.0f;
        thisx->world.pos.y = ((35.0f / 92.0f) * posYfac) - 129.5f;
    }

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~(AT_ON | AT_HIT);

        relYawTowardsPlayer = thisx->yawTowardsPlayer - thisx->world.rot.y;
        if ((relYawTowardsPlayer > -0x4000) && (relYawTowardsPlayer < 0x4000)) {
            thisx->world.rot.y += 0x8000;
        }

        Actor_player_power_damage_AT_set(play, thisx, 2.0f, thisx->yawTowardsPlayer, 0.0f, 0);
        player_SE_set(GET_PLAYER(play), NA_SE_PL_BODY_HIT);

        this->yOffsetSpeed = 10.0f;
        this->speedFactor = 0.5f;
        this->hasHit = true;
    }

    if (this->hasHit) {
        this->yOffsetSpeed -= 1.5f;
        thisx->shape.yOffset += this->yOffsetSpeed * 10.0f;
        if (thisx->shape.yOffset < 595.0f) {
            thisx->shape.yOffset = 595.0f;
            mv_stop_init(this);
        }
    } else {
        chase_f(&this->speedFactor, 1.0f, 0.04f);
    }

    if (thisx->world.pos.x > 1745.0f) {
        thisx->world.rot.y = -0x4000;
    } else if (thisx->world.pos.x < 1393.0f) {
        thisx->world.rot.y = 0x4000;
    }

    Actor_SE_set(thisx, NA_SE_EV_BIGBALL_ROLL - SFX_FLAG);
}

static void mv_stop_init(BgJyaGoroiwa* this) {
    this->actionFunc = mv_stop;
    this->waitTimer = 0;
}

static void mv_stop(BgJyaGoroiwa* this, PlayState* play) {
    this->waitTimer++;
    if (this->waitTimer > 60) {
        mv_walk_init(this);
        this->speedFactor = 0.1f;
    }
}

void Bg_Jya_Goroiwa_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgJyaGoroiwa* this = (BgJyaGoroiwa*)thisx;
    Player* player = GET_PLAYER(play);
    s32 bgId;
    Vec3f checkPos;

    if (!(player->stateFlags1 & (PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_28 | PLAYER_STATE1_29))) {
        this->actionFunc(this, play);
        set_angle_goroiwa(this);
        checkPos.x = this->actor.world.pos.x;
        checkPos.y = this->actor.world.pos.y + 59.5f;
        checkPos.z = this->actor.world.pos.z;
        this->actor.floorHeight =
            T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &checkPos);
        set_collisionPos_goroiwa(this);
        if (this->collider.base.atFlags & AT_ON) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        }
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void Bg_Jya_Goroiwa_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gRollingRockDL);
}
