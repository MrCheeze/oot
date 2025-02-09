/*
 * File: z_en_goroiwa.c
 * Overlay: ovl_En_Goroiwa
 * Description: Rolling boulders
 */

#include "z_en_goroiwa.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_goroiwa/object_goroiwa.h"
#include "quake.h"
#include "terminal.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef s32 (*EnGoroiwaUnkFunc1)(EnGoroiwa* this, PlayState* play);
typedef void (*EnGoroiwaUnkFunc2)(EnGoroiwa* this);

#define ENGOROIWA_ENABLE_AT (1 << 0)
#define ENGOROIWA_ENABLE_OC (1 << 1)
#define ENGOROIWA_PLAYER_IN_THE_WAY (1 << 2)
#define ENGOROIWA_RETAIN_ROT_SPEED (1 << 3)
#define ENGOROIWA_IN_WATER (1 << 4)

#define ENGOROIWA_LOOPMODE_ONEWAY 0
/* same as ENGOROIWA_LOOPMODE_ONEWAY but display rock fragments as if the boulder broke at the end of the path*/
#define ENGOROIWA_LOOPMODE_ONEWAY_BREAK 1
#define ENGOROIWA_LOOPMODE_ROUNDTRIP 3

void En_Goroiwa_actor_ct(Actor* thisx, PlayState* play);
void En_Goroiwa_actor_dt(Actor* thisx, PlayState* play2);
void En_Goroiwa_actor_move(Actor* thisx, PlayState* play);
void En_Goroiwa_actor_draw(Actor* thisx, PlayState* play);

static void mv_walk_init(EnGoroiwa* this);
static void mv_walk(EnGoroiwa* this, PlayState* play);
void mv_bump_init(EnGoroiwa* this);
void mv_bump(EnGoroiwa* this, PlayState* play);
void mv_oneMoment_init(EnGoroiwa* this);
void mv_oneMoment(EnGoroiwa* this, PlayState* play);
static void mv_up_init(EnGoroiwa* this);
static void mv_up(EnGoroiwa* this, PlayState* play);
static void mv_down_init(EnGoroiwa* this);
static void mv_down(EnGoroiwa* this, PlayState* play);

ActorProfile En_Goroiwa_Profile = {
    /**/ ACTOR_EN_GOROIWA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GOROIWA,
    /**/ sizeof(EnGoroiwa),
    /**/ En_Goroiwa_actor_ct,
    /**/ En_Goroiwa_actor_dt,
    /**/ En_Goroiwa_actor_move,
    /**/ En_Goroiwa_actor_draw,
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

static CollisionCheckInfoInit StatusDt_goroiwa = { 0, 12, 60, MASS_HEAVY };

static f32 G_speedData[] = { 10.0f, 9.2f };

#if DEBUG_FEATURES
#define EN_GOROIWA_SPEED(this) (R_EN_GOROIWA_SPEED * 0.01f)
#else
#define EN_GOROIWA_SPEED(this) G_speedData[(this)->isInKokiri]
#endif

static void set_collisionPos_goroiwa(EnGoroiwa* this) {
    static f32 y_offset[] = { 0.0f, 59.5f };
    Sphere16* worldSphere = &this->collider.elements[0].dim.worldSphere;

    worldSphere->center.x = this->actor.world.pos.x;
    worldSphere->center.y = this->actor.world.pos.y + y_offset[PARAMS_GET_U(this->actor.params, 10, 1)];
    worldSphere->center.z = this->actor.world.pos.z;
}

static void set_collision_goroiwa(EnGoroiwa* this, PlayState* play) {
    s32 pad;

    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &ClSphDt_goroiwa, this->colliderItems);
    set_collisionPos_goroiwa(this);
    this->collider.elements[0].dim.worldSphere.radius = 58;
}

void reset_collision_flag(EnGoroiwa* this, u8 setFlags) {
    this->stateFlags &= ~(ENGOROIWA_ENABLE_AT | ENGOROIWA_ENABLE_OC);
    this->stateFlags |= setFlags;
}

static s32 get_unitVec(Vec3f* ret, Vec3f* a) {
    f32 magnitude = Math3DVecLength(a);
    f32 scale;

    if (magnitude < 0.001f) {
        return false;
    }

    scale = 1.0f / magnitude;

    ret->x = a->x * scale;
    ret->y = a->y * scale;
    ret->z = a->z * scale;

    return true;
}

void set_speed_type(EnGoroiwa* this, PlayState* play) {
    if (play->sceneId == SCENE_KOKIRI_FOREST) {
        this->isInKokiri = true;
#if DEBUG_FEATURES
        R_EN_GOROIWA_SPEED = 920;
#endif
    } else {
        this->isInKokiri = false;
#if DEBUG_FEATURES
        R_EN_GOROIWA_SPEED = 1000;
#endif
    }
}

void set_direct_goroiwa(EnGoroiwa* this, PlayState* play) {
    Path* path = &play->pathList[PARAMS_GET_U(this->actor.params, 0, 8)];
    Vec3s* nextPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->nextWaypoint;
    Vec3f nextPosF;

    nextPosF.x = nextPos->x;
    nextPosF.y = nextPos->y;
    nextPosF.z = nextPos->z;

    this->actor.world.rot.y = search_position_angleY(&this->actor.world.pos, &nextPosF);
}

void get_old_path_vector(EnGoroiwa* this, PlayState* play, Vec3f* dest) {
    s16 loopMode = PARAMS_GET_U(this->actor.params, 8, 2);
    Path* path = &play->pathList[PARAMS_GET_U(this->actor.params, 0, 8)];
    s16 prevWaypoint = this->currentWaypoint - this->pathDirection;
    Vec3s* prevPointPos;
    Vec3s* currentPointPos;

    if (prevWaypoint < 0) {
        if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY || loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK) {
            prevWaypoint = this->endWaypoint;
        } else if (loopMode == ENGOROIWA_LOOPMODE_ROUNDTRIP) {
            prevWaypoint = 1;
        }
    } else if (prevWaypoint > this->endWaypoint) {
        if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY || loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK) {
            prevWaypoint = 0;
        } else if (loopMode == ENGOROIWA_LOOPMODE_ROUNDTRIP) {
            prevWaypoint = this->endWaypoint - 1;
        }
    }

    currentPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->currentWaypoint;
    prevPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + prevWaypoint;
    dest->x = currentPointPos->x - prevPointPos->x;
    dest->y = currentPointPos->x - prevPointPos->y;
    dest->z = currentPointPos->x - prevPointPos->z;
}

void revise_pathIdx_goroiwa(EnGoroiwa* this) {
    s16 loopMode = PARAMS_GET_U(this->actor.params, 8, 2);

    if (this->nextWaypoint < 0) {
        if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY || loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK) {
            this->currentWaypoint = this->endWaypoint;
            this->nextWaypoint = this->endWaypoint - 1;
            this->pathDirection = -1;
        } else if (loopMode == ENGOROIWA_LOOPMODE_ROUNDTRIP) {
            this->currentWaypoint = 0;
            this->nextWaypoint = 1;
            this->pathDirection = 1;
        }
    } else if (this->nextWaypoint > this->endWaypoint) {
        if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY || loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK) {
            this->currentWaypoint = 0;
            this->nextWaypoint = 1;
            this->pathDirection = 1;
        } else if (loopMode == ENGOROIWA_LOOPMODE_ROUNDTRIP) {
            this->currentWaypoint = this->endWaypoint;
            this->nextWaypoint = this->endWaypoint - 1;
            this->pathDirection = -1;
        }
    }
}

void set_nextPathIndex_goroiwa(EnGoroiwa* this) {
    this->currentWaypoint = this->nextWaypoint;
    this->nextWaypoint += this->pathDirection;
    revise_pathIdx_goroiwa(this);
}

void set_revPathIndex_goroiwa(EnGoroiwa* this) {
    this->pathDirection *= -1;
    this->currentWaypoint = this->nextWaypoint;
    this->nextWaypoint += this->pathDirection;
}

void init_PathIndex_goroiwa(EnGoroiwa* this, PlayState* play) {
    this->endWaypoint = play->pathList[PARAMS_GET_U(this->actor.params, 0, 8)].count - 1;
    this->currentWaypoint = 0;
    this->nextWaypoint = 1;
    this->pathDirection = 1;
}

void set_specifyPos_goroiwa(EnGoroiwa* this, PlayState* play, s32 waypoint) {
    Path* path = &play->pathList[PARAMS_GET_U(this->actor.params, 0, 8)];
    Vec3s* pointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + waypoint;

    this->actor.world.pos.x = pointPos->x;
    this->actor.world.pos.y = pointPos->y;
    this->actor.world.pos.z = pointPos->z;
}

void init_angle_goroiwa(EnGoroiwa* this) {
    this->prevUnitRollAxis.x = 1.0f;
    this->rollRotSpeed = 1.0f;
}

s32 check_updownPathPos(EnGoroiwa* this, PlayState* play) {
    s32 pad;
    Path* path = &play->pathList[PARAMS_GET_U(this->actor.params, 0, 8)];
    Vec3s* nextPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->nextWaypoint;
    Vec3s* currentPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->currentWaypoint;

    if (nextPointPos->x == currentPointPos->x && nextPointPos->z == currentPointPos->z) {
#if DEBUG_FEATURES
        if (nextPointPos->y == currentPointPos->y) {
            // "Error: Invalid path data (points overlap)"
            PRINTF("Error : レールデータ不正(点が重なっている)");
            PRINTF("(%s %d)(arg_data 0x%04x)\n", "../z_en_gr.c", 559, this->actor.params);
        }
#endif

        if (nextPointPos->y > currentPointPos->y) {
            return 1;
        } else {
            return -1;
        }
    }

    return 0;
}

void effect_down_dust(PlayState* play, Vec3f* pos) {
    static Vec3f vec_w = { 0.0f, 0.0f, 0.0f };
    static Vec3f acc_w = { 0.0f, 0.3f, 0.0f };
    Vec3f randPos;
    s32 i;
    s16 angle = 0;

    for (i = 0; i < 8; i++) {
        angle += 0x4E20;
        randPos.x = pos->x + (47.0f * (fqrand() * 0.5f + 0.5f)) * sin_s(angle);
        randPos.y = pos->y + (fqrand() - 0.5f) * 40.0f;
        randPos.z = pos->z + ((47.0f * (fqrand() * 0.5f + 0.5f))) * cos_s(angle);
        Effect_SS_Dust_sc_li_ct(play, &randPos, &vec_w, &acc_w, (s16)(fqrand() * 30.0f) + 100, 80);
        Effect_SS_Dust_sc_li_ct(play, &randPos, &vec_w, &acc_w, (s16)(fqrand() * 20.0f) + 80, 80);
    }
}

void effect_down_water(PlayState* play, Vec3f* contactPos) {
    Vec3f splashPos;
    s32 i;
    s16 angle = 0;

    for (i = 0; i < 11; i++) {
        angle += 0x1746;
        splashPos.x = contactPos->x + (sin_s(angle) * 55.0f);
        splashPos.y = contactPos->y;
        splashPos.z = contactPos->z + (cos_s(angle) * 55.0f);
        Effect_SS_G_Splash_sc_cl_ct(play, &splashPos, NULL, NULL, 0, 350);
    }

    Effect_SS_G_Ripple_ct2(play, contactPos, 300, 700, 0);
    Effect_SS_G_Ripple_ct2(play, contactPos, 500, 900, 4);
    Effect_SS_G_Ripple_ct2(play, contactPos, 500, 1300, 8);
}

s32 set_pos_goroiwa(EnGoroiwa* this, PlayState* play) {
    Path* path;
    s32 result;
    s32 pad;
    Vec3s* nextPointPos;

    chase_f(&this->actor.speed, EN_GOROIWA_SPEED(this), 0.3f);
    Actor_position_speed_set(&this->actor);
    path = &play->pathList[PARAMS_GET_U(this->actor.params, 0, 8)];
    nextPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->nextWaypoint;
    result = true;
    result &= chase_f(&this->actor.world.pos.x, nextPointPos->x, fabsf(this->actor.velocity.x));
    result &= chase_f(&this->actor.world.pos.z, nextPointPos->z, fabsf(this->actor.velocity.z));
    this->actor.world.pos.y += this->actor.velocity.y;
    return result;
}

s32 set_posPointToPoint(EnGoroiwa* this, PlayState* play) {
    Path* path = &play->pathList[PARAMS_GET_U(this->actor.params, 0, 8)];
    s32 pad;
    Vec3s* nextPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->nextWaypoint;
    Vec3s* currentPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->currentWaypoint;
    s32 nextPointReached;
    Vec3f posDiff;
    Vec3f nextPointPosF;

    nextPointPosF.x = nextPointPos->x;
    nextPointPosF.y = nextPointPos->y;
    nextPointPosF.z = nextPointPos->z;
    chase_f(&this->actor.speed, EN_GOROIWA_SPEED(this), 0.3f);
    if (Math3DLengthSquare(&nextPointPosF, &this->actor.world.pos) < SQ(5.0f)) {
        xyz_t_sub(&nextPointPosF, &this->actor.world.pos, &posDiff);
    } else {
        posDiff.x = nextPointPosF.x - currentPointPos->x;
        posDiff.y = nextPointPosF.y - currentPointPos->y;
        posDiff.z = nextPointPosF.z - currentPointPos->z;
    }
    get_unitVec(&this->actor.velocity, &posDiff);
    this->actor.velocity.x *= this->actor.speed;
    this->actor.velocity.y *= this->actor.speed;
    this->actor.velocity.z *= this->actor.speed;
    nextPointReached = true;
    nextPointReached &= chase_f(&this->actor.world.pos.x, nextPointPosF.x, fabsf(this->actor.velocity.x));
    nextPointReached &= chase_f(&this->actor.world.pos.y, nextPointPosF.y, fabsf(this->actor.velocity.y));
    nextPointReached &= chase_f(&this->actor.world.pos.z, nextPointPosF.z, fabsf(this->actor.velocity.z));
    return nextPointReached;
}

s32 set_posUp(EnGoroiwa* this, PlayState* play) {
    s32 pad;
    Path* path = &play->pathList[PARAMS_GET_U(this->actor.params, 0, 8)];
    Vec3s* nextPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->nextWaypoint;

    chase_f(&this->actor.velocity.y, EN_GOROIWA_SPEED(this) * 0.5f, 0.18f);
    this->actor.world.pos.x = nextPointPos->x;
    this->actor.world.pos.z = nextPointPos->z;
    return chase_f(&this->actor.world.pos.y, nextPointPos->y, fabsf(this->actor.velocity.y));
}

s32 set_posDown(EnGoroiwa* this, PlayState* play) {
    s32 pad;
    Path* path = &play->pathList[PARAMS_GET_U(this->actor.params, 0, 8)];
    Vec3s* nextPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->nextWaypoint;
    f32 nextPointY;
    f32 thisY;
    f32 yDistToFloor;
    s32 quakeIndex;
    CollisionPoly* floorPoly;
    Vec3f checkPos;
    f32 floorY;
    s32 pad2;
    s32 floorBgId;
    Vec3f dustPos;
    WaterBox* waterBox;
    f32 ySurface;
    Vec3f waterHitPos;

    nextPointY = nextPointPos->y;
    chase_f(&this->actor.velocity.y, -14.0f, 1.0f);
    this->actor.world.pos.x = nextPointPos->x;
    this->actor.world.pos.z = nextPointPos->z;
    thisY = this->actor.world.pos.y;
    if (1) {}
    this->actor.world.pos.y += this->actor.velocity.y;
    if (this->actor.velocity.y < 0.0f && this->actor.world.pos.y <= nextPointY) {
        if (this->bounceCount == 0) {
            if (this->actor.xzDistToPlayer < 600.0f) {
                quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
                setSpeedQuake(quakeIndex, -0x3CB0);
                setScaleQuake(quakeIndex, 3, 0, 0, 0);
                setTimerQuake(quakeIndex, 7);
            }
            this->rollRotSpeed = 0.0f;
            if (!(this->stateFlags & ENGOROIWA_IN_WATER)) {
                checkPos.x = this->actor.world.pos.x;
                checkPos.y = this->actor.world.pos.y + 50.0f;
                checkPos.z = this->actor.world.pos.z;
                floorY =
                    T_BGCheck_ObjGroundCheck_aiac2(play, &play->colCtx, &floorPoly, &floorBgId, &this->actor, &checkPos);
                yDistToFloor = floorY - (this->actor.world.pos.y - 59.5f);
                if (fabsf(yDistToFloor) < 15.0f) {
                    dustPos.x = this->actor.world.pos.x;
                    dustPos.y = floorY + 10.0f;
                    dustPos.z = this->actor.world.pos.z;
                    effect_down_dust(play, &dustPos);
                }
            }
        }
        if (this->bounceCount >= 1) {
            return true;
        }
        this->bounceCount++;
        this->actor.velocity.y *= -0.3f;
        this->actor.world.pos.y = nextPointY - ((this->actor.world.pos.y - nextPointY) * 0.3f);
    }
    if (this->bounceCount == 0 &&
        T_BGCheck_WaterSurfaceCheck2(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &ySurface,
                                &waterBox) &&
        this->actor.world.pos.y <= ySurface) {
        this->stateFlags |= ENGOROIWA_IN_WATER;
        if (ySurface < thisY) {
            waterHitPos.x = this->actor.world.pos.x;
            waterHitPos.y = ySurface;
            waterHitPos.z = this->actor.world.pos.z;
            effect_down_water(play, &waterHitPos);
            this->actor.velocity.y *= 0.2f;
        }
        if (this->actor.velocity.y < -8.0f) {
            this->actor.velocity.y = -8.0f;
        }
    }
    return false;
}

static void set_angle_goroiwa(EnGoroiwa* this, PlayState* play) {
    static Vec3f vec_base_Y = { 0.0f, 1.0f, 0.0f };
    s32 pad;
    Vec3f* rollAxisPtr;
    f32 rollAngleDiff;
    Vec3f rollAxis;
    Vec3f unitRollAxis;
    MtxF mtx;

    if (this->stateFlags & ENGOROIWA_RETAIN_ROT_SPEED) {
        rollAngleDiff = this->prevRollAngleDiff;
    } else {
        this->prevRollAngleDiff = Math3DLength(&this->actor.world.pos, &this->actor.prevPos) * (1.0f / 59.5f);
        rollAngleDiff = this->prevRollAngleDiff;
    }
    rollAngleDiff *= this->rollRotSpeed;
    rollAxisPtr = &rollAxis;
    if (this->stateFlags & ENGOROIWA_RETAIN_ROT_SPEED) {
        Vec3f unusedDiff;

        /*
         * get_old_path_vector has no side effects and its result goes unused,
         * its result was probably meant to be used instead of the actor's vec_w in the
         * Math3DVectorProduct2Vec call.
         */
        get_old_path_vector(this, play, &unusedDiff);
        Math3DVectorProduct2Vec(&vec_base_Y, &this->actor.velocity, rollAxisPtr);
    } else {
        Math3DVectorProduct2Vec(&vec_base_Y, &this->actor.velocity, rollAxisPtr);
    }

    if (get_unitVec(&unitRollAxis, rollAxisPtr)) {
        this->prevUnitRollAxis = unitRollAxis;
    } else {
        unitRollAxis = this->prevUnitRollAxis;
    }

    Matrix_rotateVector(rollAngleDiff, &unitRollAxis, MTXMODE_NEW);
    Matrix_rotateY(BINANG_TO_RAD(this->actor.shape.rot.y), MTXMODE_APPLY);
    Matrix_rotateX(BINANG_TO_RAD(this->actor.shape.rot.x), MTXMODE_APPLY);
    Matrix_rotateZ(BINANG_TO_RAD(this->actor.shape.rot.z), MTXMODE_APPLY);
    Matrix_get(&mtx);
    Matrix_to_rotate_new(&mtx, &this->actor.shape.rot, 0);
}

void set_nextWalk(EnGoroiwa* this, PlayState* play) {
    s16 loopMode = PARAMS_GET_U(this->actor.params, 8, 2);

    set_nextPathIndex_goroiwa(this);

    if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY || loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK) {
        if (this->currentWaypoint == 0 || this->currentWaypoint == this->endWaypoint) {
            set_specifyPos_goroiwa(this, play, this->currentWaypoint);
        }
    }

    set_direct_goroiwa(this, play);
}

void bomb_effect_goroiwa(EnGoroiwa* this, PlayState* play) {
    static f32 offset_y[] = { 0.0f, 59.5f };
    s16 angle1;
    s16 angle2;
    s32 pad;
    Vec3f* thisPos = &this->actor.world.pos;
    Vec3f effectPos;
    Vec3f fragmentVelocity;
    f32 cos1;
    f32 sin1;
    f32 sin2;
    s16 yOffsetIdx = PARAMS_GET_U(this->actor.params, 10, 1);
    s32 i;

    for (i = 0, angle1 = 0; i < 16; i++, angle1 += 0x4E20) {
        sin1 = sin_s(angle1);
        cos1 = cos_s(angle1);
        angle2 = fqrand() * 0xFFFF;
        effectPos.x = fqrand() * 50.0f * sin1 * sin_s(angle2);
        sin2 = sin_s(angle2);
        effectPos.y = (fqrand() - 0.5f) * 100.0f * sin2 + offset_y[yOffsetIdx];
        effectPos.z = fqrand() * 50.0f * cos1 * sin_s(angle2);
        fragmentVelocity.x = effectPos.x * 0.2f;
        fragmentVelocity.y = fqrand() * 15.0f + 2.0f;
        fragmentVelocity.z = effectPos.z * 0.2f;
        xyz_t_add(&effectPos, thisPos, &effectPos);
        Effect_Kakera_ct2(play, &effectPos, &fragmentVelocity, &effectPos, -340, 33, 28, 2, 0,
                             fqrand() * 7.0f + 1.0f, 1, 0, 70, KAKERA_COLOR_NONE, 1, gBoulderFragmentsDL);
    }

    effectPos.x = thisPos->x;
    effectPos.y = thisPos->y + offset_y[yOffsetIdx];
    effectPos.z = thisPos->z;
    dust_fly_set2(play, &effectPos, 80.0f, 5, 70, 110, 1);
    dust_fly_set2(play, &effectPos, 90.0f, 5, 110, 160, 1);
}

static InitChainEntry value_init[] = {
    ICHAIN_F32_DIV1000(gravity, -860, ICHAIN_CONTINUE),   ICHAIN_F32_DIV1000(minVelocityY, -15000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),    ICHAIN_F32(cullingVolumeDistance, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 150, ICHAIN_CONTINUE), ICHAIN_F32(cullingVolumeDownward, 1500, ICHAIN_STOP),
};

void En_Goroiwa_actor_ct(Actor* thisx, PlayState* play) {
    static f32 shape_offset[] = { 0.0f, 595.0f };
    EnGoroiwa* this = (EnGoroiwa*)thisx;
    s32 pathIdx;

    ValueSet_process(&this->actor, value_init);
    set_collision_goroiwa(this, play);
    pathIdx = PARAMS_GET_U(this->actor.params, 0, 8);
    if (pathIdx == 0xFF) {
        // "Error: Invalid arg_data"
        PRINTF("Ｅｒｒｏｒ : arg_data が不正(%s %d)(arg_data 0x%04x)\n", "../z_en_gr.c", 1033, this->actor.params);
        Actor_delete(&this->actor);
        return;
    }
    if (play->pathList[pathIdx].count < 2) {
        // "Error: Invalid Path Data"
        PRINTF("Ｅｒｒｏｒ : レールデータ が不正(%s %d)\n", "../z_en_gr.c", 1043);
        Actor_delete(&this->actor);
        return;
    }
    CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &StatusDt_goroiwa);
    Shape_Info_init(&this->actor.shape, shape_offset[PARAMS_GET_U(this->actor.params, 10, 1)], Actor_shadow_circle,
                    9.4f);
    this->actor.shape.shadowAlpha = 200;
    set_speed_type(this, play);
    init_PathIndex_goroiwa(this, play);
    set_specifyPos_goroiwa(this, play, 0);
    init_angle_goroiwa(this);
    set_direct_goroiwa(this, play);
    mv_walk_init(this);
    // "(Goroiwa)"
    PRINTF("(ごろ岩)(arg 0x%04x)(rail %d)(end %d)(bgc %d)(hit %d)\n", this->actor.params,
           PARAMS_GET_U(this->actor.params, 0, 8), PARAMS_GET_U(this->actor.params, 8, 2),
           PARAMS_GET_U(this->actor.params, 10, 1), this->actor.home.rot.z & 1);
}

void En_Goroiwa_actor_dt(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnGoroiwa* this = (EnGoroiwa*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

static void mv_walk_init(EnGoroiwa* this) {
    this->actionFunc = mv_walk;
    reset_collision_flag(this, ENGOROIWA_ENABLE_AT | ENGOROIWA_ENABLE_OC);
    this->rollRotSpeed = 1.0f;
}

static void mv_walk(EnGoroiwa* this, PlayState* play) {
    static EnGoroiwaUnkFunc1 pos_proc[] = { set_posPointToPoint, set_pos_goroiwa };
    static EnGoroiwaUnkFunc2 cc_next_proc[] = { mv_oneMoment_init, mv_bump_init };

    s32 ascendDirection;
    s16 yawDiff;
    s16 loopMode;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        this->stateFlags &= ~ENGOROIWA_PLAYER_IN_THE_WAY;
        yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
        if (yawDiff > -0x4000 && yawDiff < 0x4000) {
            this->stateFlags |= ENGOROIWA_PLAYER_IN_THE_WAY;
            if (PARAMS_GET_U(this->actor.params, 10, 1) || (this->actor.home.rot.z & 1) != 1) {
                set_revPathIndex_goroiwa(this);
                set_direct_goroiwa(this, play);
            }
        }
        Actor_player_power_damage_AT_set(play, &this->actor, 2.0f, this->actor.yawTowardsPlayer, 0.0f, 0);
        PRINTF_COLOR_CYAN();
        PRINTF("Player ぶっ飛ばし\n"); // "Player knocked down"
        PRINTF_RST();
        cc_next_proc[PARAMS_GET_U(this->actor.params, 10, 1)](this);
        player_SE_set(GET_PLAYER(play), NA_SE_PL_BODY_HIT);
        if ((this->actor.home.rot.z & 1) == 1) {
            this->collisionDisabledTimer = 50;
        }
    } else if (pos_proc[PARAMS_GET_U(this->actor.params, 10, 1)](this, play)) {
        loopMode = PARAMS_GET_U(this->actor.params, 8, 2);
        if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK &&
            (this->nextWaypoint == 0 || this->nextWaypoint == this->endWaypoint)) {
            bomb_effect_goroiwa(this, play);
        }
        set_nextWalk(this, play);
        if ((loopMode == ENGOROIWA_LOOPMODE_ROUNDTRIP) &&
            (this->currentWaypoint == 0 || this->currentWaypoint == this->endWaypoint)) {
            mv_oneMoment_init(this);
        } else if (!PARAMS_GET_U(this->actor.params, 10, 1) && this->currentWaypoint != 0 &&
                   this->currentWaypoint != this->endWaypoint) {
            ascendDirection = check_updownPathPos(this, play);
            if (ascendDirection > 0) {
                mv_up_init(this);
            } else if (ascendDirection < 0) {
                mv_down_init(this);
            } else {
                mv_walk_init(this);
            }
        } else {
            mv_walk_init(this);
        }
    }
    Actor_SE_set(&this->actor, NA_SE_EV_BIGBALL_ROLL - SFX_FLAG);
}

void mv_bump_init(EnGoroiwa* this) {
    this->actionFunc = mv_bump;
    reset_collision_flag(this, ENGOROIWA_ENABLE_OC);
    this->actor.gravity = -0.86f;
    this->actor.minVelocityY = -15.0f;
    this->actor.speed *= 0.15f;
    this->actor.velocity.y = 5.0f;
    this->rollRotSpeed = 1.0f;
}

void mv_bump(EnGoroiwa* this, PlayState* play) {
    set_pos_goroiwa(this, play);
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && this->actor.velocity.y < 0.0f) {
        if ((this->stateFlags & ENGOROIWA_PLAYER_IN_THE_WAY) && (this->actor.home.rot.z & 1) == 1) {
            set_revPathIndex_goroiwa(this);
            set_direct_goroiwa(this, play);
        }
        mv_oneMoment_init(this);
    }
}

void mv_oneMoment_init(EnGoroiwa* this) {
    static s16 stop_timer[] = { 20, 6 };

    this->actionFunc = mv_oneMoment;
    this->actor.speed = 0.0f;
    reset_collision_flag(this, ENGOROIWA_ENABLE_OC);
    this->waitTimer = stop_timer[this->actor.home.rot.z & 1];
    this->rollRotSpeed = 0.0f;
}

void mv_oneMoment(EnGoroiwa* this, PlayState* play) {
    if (this->waitTimer > 0) {
        this->waitTimer--;
    } else {
        this->collider.base.atFlags &= ~AT_HIT;
        mv_walk_init(this);
    }
}

static void mv_up_init(EnGoroiwa* this) {
    this->actionFunc = mv_up;
    reset_collision_flag(this, ENGOROIWA_ENABLE_AT | ENGOROIWA_ENABLE_OC);
    this->rollRotSpeed = 0.0f;
    this->actor.velocity.y = fabsf(this->actor.speed) * 0.1f;
}

static void mv_up(EnGoroiwa* this, PlayState* play) {
    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        Actor_player_power_damage_AT_set(play, &this->actor, 2.0f, this->actor.yawTowardsPlayer, 0.0f, 4);
        player_SE_set(GET_PLAYER(play), NA_SE_PL_BODY_HIT);
        if ((this->actor.home.rot.z & 1) == 1) {
            this->collisionDisabledTimer = 50;
        }
    } else if (set_posUp(this, play)) {
        set_nextWalk(this, play);
        mv_walk_init(this);
        this->actor.speed = 0.0f;
    }
}

static void mv_down_init(EnGoroiwa* this) {
    this->actionFunc = mv_down;
    reset_collision_flag(this, ENGOROIWA_ENABLE_AT | ENGOROIWA_ENABLE_OC);
    this->rollRotSpeed = 0.3f;
    this->bounceCount = 0;
    this->actor.velocity.y = fabsf(this->actor.speed) * -0.3f;
    this->stateFlags |= ENGOROIWA_RETAIN_ROT_SPEED;
    this->stateFlags &= ~ENGOROIWA_IN_WATER;
}

static void mv_down(EnGoroiwa* this, PlayState* play) {
    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        Actor_player_power_damage_AT_set(play, &this->actor, 2.0f, this->actor.yawTowardsPlayer, 0.0f, 4);
        player_SE_set(GET_PLAYER(play), NA_SE_PL_BODY_HIT);
        if ((this->actor.home.rot.z & 1) == 1) {
            this->collisionDisabledTimer = 50;
        }
    } else if (set_posDown(this, play)) {
        set_nextWalk(this, play);
        mv_walk_init(this);
        this->stateFlags &= ~ENGOROIWA_RETAIN_ROT_SPEED;
        this->actor.speed = 0.0f;
    }
}

void En_Goroiwa_actor_move(Actor* thisx, PlayState* play) {
    EnGoroiwa* this = (EnGoroiwa*)thisx;
    Player* player = GET_PLAYER(play);
    s32 pad;
    s32 bgId;

    if (!(player->stateFlags1 & (PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_28 | PLAYER_STATE1_29))) {
        if (this->collisionDisabledTimer > 0) {
            this->collisionDisabledTimer--;
        }
        this->actionFunc(this, play);
        switch (PARAMS_GET_U(this->actor.params, 10, 1)) {
            case 1:
                Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f,
                                        UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
                break;
            case 0:
                this->actor.floorHeight = T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &this->actor.floorPoly, &bgId,
                                                                     &this->actor, &this->actor.world.pos);
                break;
        }
        set_angle_goroiwa(this, play);
        if (this->actor.xzDistToPlayer < 300.0f) {
            set_collisionPos_goroiwa(this);
            if ((this->stateFlags & ENGOROIWA_ENABLE_AT) && this->collisionDisabledTimer <= 0) {
                CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
            }
            if ((this->stateFlags & ENGOROIWA_ENABLE_OC) && this->collisionDisabledTimer <= 0) {
                CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
            }
        }
    }
}

void En_Goroiwa_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gRollingRockDL);
}
