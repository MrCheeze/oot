/*
 * File: z_bg_spot08_iceblock.c
 * Overlay: ovl_Bg_Spot08_Iceblock
 * Description: Floating ice platforms
 */

#include "z_bg_spot08_iceblock.h"

#include "libc64/math64.h"
#include "libc64/qrand.h"
#include "ichain.h"
#include "sys_math3d.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/object_spot08_obj/object_spot08_obj.h"

#define FLAGS 0

void Bg_Spot08_Iceblock_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot08_Iceblock_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot08_Iceblock_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot08_Iceblock_actor_draw(Actor* thisx, PlayState* play);

void mv_norm_init(BgSpot08Iceblock* this);
void mv_norm(BgSpot08Iceblock* this, PlayState* play);
static void mv_rot_init(BgSpot08Iceblock* this);
static void mv_rot(BgSpot08Iceblock* this, PlayState* play);
void mv_twin_init(BgSpot08Iceblock* this);
void mv_twin(BgSpot08Iceblock* this, PlayState* play);
static void mv_stop_init(BgSpot08Iceblock* this);

ActorProfile Bg_Spot08_Iceblock_Profile = {
    /**/ ACTOR_BG_SPOT08_ICEBLOCK,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT08_OBJ,
    /**/ sizeof(BgSpot08Iceblock),
    /**/ Bg_Spot08_Iceblock_actor_ct,
    /**/ Bg_Spot08_Iceblock_actor_dt,
    /**/ Bg_Spot08_Iceblock_actor_move,
    /**/ Bg_Spot08_Iceblock_actor_draw,
};

static void set_moveProc(BgSpot08Iceblock* this, BgSpot08IceblockActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

static void set_dynaPoly(BgSpot08Iceblock* this, PlayState* play, CollisionHeader* collision, s32 flags) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, flags);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        // "Warning: move BG registration failed"
        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_spot08_iceblock.c", 0xD9,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

// Sets params to 0x10 (medium, nonrotating) if not in the cases listed.
void check_arg_data(BgSpot08Iceblock* this) {
    switch (PARAMS_GET_U(this->dyna.actor.params, 0, 8)) {
        case 0xFF:
            this->dyna.actor.params = 0x10;
            break;
        default:
            // "Error: arg_data setting error"
            PRINTF("Error : arg_data 設定ミスです。(%s %d)(arg_data 0x%04x)\n", "../z_bg_spot08_iceblock.c", 0xF6,
                   this->dyna.actor.params);
            this->dyna.actor.params = 0x10;
            break;
        case 1:
        case 4:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x14:
        case 0x20:
        case 0x23:
        case 0x24:
            break;
    }
}

void set_swing_vertical(BgSpot08Iceblock* this) {
    this->bobOffset = (sin_s(this->bobPhaseSlow) * 4.0f) + (sin_s(this->bobPhaseFast) * 3.0f);
}

void set_sink(BgSpot08Iceblock* this) {
    f32 target;
    f32 step;

    switch (PARAMS_GET_NOSHIFT(this->dyna.actor.params, 4, 4)) {
        case 0:
            step = 0.15f;
            break;
        case 0x10:
            step = 0.2f;
            break;
        case 0x20:
            step = 0.4f;
            break;
    }

    // Sink under an actor's weight if standing on it
    target = (MoveBG_checkRideStatus(&this->dyna) ? -4.0f : 0.0f);

    chase_f(&this->sinkOffset, target, step);
}

void set_positionY(BgSpot08Iceblock* this) {
    this->dyna.actor.world.pos.y = this->sinkOffset + this->bobOffset + this->dyna.actor.home.pos.y;
}

void vector_mul_scalar(Vec3f* dest, Vec3f* v, f32 scale) {
    dest->x = v->x * scale;
    dest->y = v->y * scale;
    dest->z = v->z * scale;
}

static void gaiseki(Vec3f* dest, Vec3f* v1, Vec3f* v2) {
    dest->x = (v1->y * v2->z) - (v1->z * v2->y);
    dest->y = (v1->z * v2->x) - (v1->x * v2->z);
    dest->z = (v1->x * v2->y) - (v1->y * v2->x);
}

static s32 get_unitVec(Vec3f* dest, Vec3f* v) {
    f32 magnitude;

    magnitude = Math3DVecLength(v);
    if (magnitude < 0.001f) {
        dest->x = dest->y = 0.0f;
        dest->z = 1.0f;
        return false;
    } else {
        dest->x = v->x * (1.0f / magnitude);
        dest->y = v->y * (1.0f / magnitude);
        dest->z = v->z * (1.0f / magnitude);
        return true;
    }
}

static Vec3f BASE_vecY = { 0.0f, 1.0f, 0.0f };
static Vec3f ZERO_vec = { 0.0f, 0.0f, 0.0f };
static f32 scalar_l[] = { 1.0f / 70000000, 1.0f / 175000000, 1.0f / 700000000 };
static f32 zx_capa[] = { 0.96f, 0.96f, 0.98f };

static f32 limit_ss[] = {
    0.22495104f, // sin(13 degrees)
    0.22495104f, // sin(13 degrees)
    0.03489947f, // sin(2 degrees)
};

static f32 limit_cc[] = {
    0.97437006f, // cos(13 degrees)
    0.97437006f, // cos(13 degrees)
    0.99939084f, // cos(2 degrees)
};

/**
 *  Handles all the factors that influence rolling: inertia, random oscillations, and most significantly, player weight,
 * and combines them to produce a matrix that rotates the actor to match the surface normal
 */
void set_slope(BgSpot08Iceblock* this, PlayState* play) {
    f32 deviationFromVertSq;
    f32 stabilityCorrection;
    Vec3f surfaceNormalHorizontal;
    Vec3f playerCentroidDiff;
    Vec3f playerMoment;
    Vec3f surfaceNormalHorizontalScaled;
    Vec3f randomNutation;
    Vec3f tempVec; // reused with different meanings
    Vec3f torqueDirection;
    f32 playerCentroidDist;
    s32 rollDataIndex;
    MtxF mtx;
    s32 pad;
    Player* player = GET_PLAYER(play);

    switch (PARAMS_GET_U(this->dyna.actor.params, 0, 8)) {
        case 0x11: // Medium nonrotating
            rollDataIndex = 0;
            break;
        case 1:
            rollDataIndex = 1; // Large nonrotating
            break;
        default:
            rollDataIndex = 2;
            break;
    }

    xyz_t_sub(&player->actor.world.pos, &this->dyna.actor.world.pos, &playerCentroidDiff);
    playerCentroidDiff.y -= (150.0f * this->dyna.actor.scale.y);
    playerCentroidDist = Math3DVecLength(&playerCentroidDiff);

    randomNutation.x = (fqrand() - 0.5f) * (1.0f / 625);
    randomNutation.y = 0.0f;
    randomNutation.z = (fqrand() - 0.5f) * (1.0f / 625);

    surfaceNormalHorizontal.x = this->surfaceNormal.x;
    surfaceNormalHorizontal.y = 0.0f;
    surfaceNormalHorizontal.z = this->surfaceNormal.z;

    // If player is standing on it or holding the edge
    if (MoveBG_checkRidePlayerStatus(&this->dyna) && (playerCentroidDist > 3.0f)) {
        xyz_t_sub(&playerCentroidDiff, &surfaceNormalHorizontal, &playerMoment);
        vector_mul_scalar(&playerMoment, &playerMoment,
                                          (scalar_l[rollDataIndex] * playerCentroidDist) / this->dyna.actor.scale.x);
    } else {
        playerMoment = ZERO_vec;
    }

    vector_mul_scalar(&surfaceNormalHorizontalScaled, &surfaceNormalHorizontal, -0.01f);

    // Add all three deviations
    xyz_t_add(&this->normalDelta, &playerMoment, &this->normalDelta);
    xyz_t_add(&this->normalDelta, &surfaceNormalHorizontalScaled, &this->normalDelta);
    xyz_t_add(&this->normalDelta, &randomNutation, &this->normalDelta);

    this->normalDelta.y = 0.0f;

    xyz_t_add(&this->surfaceNormal, &this->normalDelta, &tempVec);

    tempVec.x *= zx_capa[rollDataIndex];
    tempVec.z *= zx_capa[rollDataIndex];

    // Set up roll axis and final new angle
    if (get_unitVec(&this->surfaceNormal, &tempVec)) {
        deviationFromVertSq = Math3DVecLengthSquare2D(this->surfaceNormal.z, this->surfaceNormal.x);

        // Prevent overrolling
        if (limit_ss[rollDataIndex] < deviationFromVertSq) {
            stabilityCorrection = limit_ss[rollDataIndex] / deviationFromVertSq;

            this->surfaceNormal.x *= stabilityCorrection;
            this->surfaceNormal.y = limit_cc[rollDataIndex];
            this->surfaceNormal.z *= stabilityCorrection;
        }

        gaiseki(&tempVec, &BASE_vecY, &this->surfaceNormal);

        if (get_unitVec(&torqueDirection, &tempVec)) {
            this->rotationAxis = torqueDirection;
        }
    } else {
        this->surfaceNormal = BASE_vecY;
    }

    // Rotation by the angle between surfaceNormal and the vertical about rotationAxis
    Matrix_rotateVector(facos(M3D_getCos(&BASE_vecY, &this->surfaceNormal)), &this->rotationAxis,
                      MTXMODE_NEW);
    Matrix_rotateY(BINANG_TO_RAD(this->dyna.actor.shape.rot.y), MTXMODE_APPLY);
    Matrix_get(&mtx);
    Matrix_to_rotate_new(&mtx, &this->dyna.actor.shape.rot, 0);
}

void moveType_twin_ct(BgSpot08Iceblock* this, PlayState* play) {
    s32 pad[2];
    f32 sin;
    f32 cos;

    sin = sin_s(this->dyna.actor.home.rot.y) * 100.0f;
    cos = cos_s(this->dyna.actor.home.rot.y) * 100.0f;

    if (!PARAMS_GET_NOSHIFT(this->dyna.actor.params, 8, 1)) {
        Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_BG_SPOT08_ICEBLOCK,
                           this->dyna.actor.home.pos.x, this->dyna.actor.home.pos.y, this->dyna.actor.home.pos.z,
                           this->dyna.actor.home.rot.x, this->dyna.actor.home.rot.y, this->dyna.actor.home.rot.z,
                           0x123);

        this->dyna.actor.world.pos.x += sin;
        this->dyna.actor.world.pos.z += cos;
    } else {
        this->dyna.actor.world.pos.x -= sin;
        this->dyna.actor.world.pos.z -= cos;
    }
    mv_twin_init(this);
}

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 3000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 2200, ICHAIN_STOP),
};

void Bg_Spot08_Iceblock_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot08Iceblock* this = (BgSpot08Iceblock*)thisx;
    CollisionHeader* colHeader;

    // "spot08 ice floe"
    PRINTF("(spot08 流氷)(arg_data 0x%04x)\n", this->dyna.actor.params);
    check_arg_data(this);

    switch (PARAMS_GET_NOSHIFT(this->dyna.actor.params, 9, 1)) {
        case 0:
            colHeader = &gZorasFountainIcebergCol;
            break;
        case 0x200:
            colHeader = &gZorasFountainIceRampCol;
            break;
    }

    switch (PARAMS_GET_U(this->dyna.actor.params, 0, 4)) {
        case 2:
        case 3:
            set_dynaPoly(this, play, colHeader, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
            break;
        default:
            set_dynaPoly(this, play, colHeader, 0);
            break;
    }

    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        Actor_delete(&this->dyna.actor);
        return;
    }

    ValueSet_process(&this->dyna.actor, value_init);

    switch (PARAMS_GET_NOSHIFT(this->dyna.actor.params, 4, 4)) {
        case 0:
            Actor_set_scale(&this->dyna.actor, 0.2f);
            break;
        case 0x10:
            Actor_set_scale(&this->dyna.actor, 0.1f);
            break;
        case 0x20:
            Actor_set_scale(&this->dyna.actor, 0.05f);
            break;
    }

    this->bobPhaseSlow = (s32)(fqrand() * (0xFFFF + 0.5f));
    this->bobPhaseFast = (s32)(fqrand() * (0xFFFF + 0.5f));
    this->surfaceNormal.y = 1.0f;
    this->rotationAxis.x = 1.0f;

    switch (PARAMS_GET_U(this->dyna.actor.params, 0, 4)) {
        case 0:
        case 1:
            mv_norm_init(this);
            break;
        case 2:
            mv_rot_init(this);
            break;
        case 3:
            moveType_twin_ct(this, play);
            break;
        case 4:
            mv_stop_init(this);
            break;
    }
}

void Bg_Spot08_Iceblock_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot08Iceblock* this = (BgSpot08Iceblock*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void mv_norm_init(BgSpot08Iceblock* this) {
    set_moveProc(this, mv_norm);
}

void mv_norm(BgSpot08Iceblock* this, PlayState* play) {
    set_swing_vertical(this);
    set_sink(this);
    set_positionY(this);
    this->dyna.actor.shape.rot.y = this->dyna.actor.home.rot.y;
    set_slope(this, play);
}

static void mv_rot_init(BgSpot08Iceblock* this) {
    set_moveProc(this, mv_rot);
}

static void mv_rot(BgSpot08Iceblock* this, PlayState* play) {
    set_swing_vertical(this);
    set_sink(this);
    set_positionY(this);
    this->dyna.actor.world.rot.y += 0x190;
    this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y;
    set_slope(this, play);
}

void mv_twin_init(BgSpot08Iceblock* this) {
    set_moveProc(this, mv_twin);
}

void mv_twin(BgSpot08Iceblock* this, PlayState* play) {
    f32 cos;
    f32 sin;

    set_swing_vertical(this);
    set_sink(this);
    set_positionY(this);

    // parent handles rotations of both
    if (!PARAMS_GET_NOSHIFT(this->dyna.actor.params, 8, 1)) {
        this->dyna.actor.world.rot.y += 0x190;
        sin = sin_s(this->dyna.actor.world.rot.y) * 100.0f;
        cos = cos_s(this->dyna.actor.world.rot.y) * 100.0f;

        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x + sin;
        this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z + cos;

        if (this->dyna.actor.child != NULL) {
            this->dyna.actor.child->world.pos.x = this->dyna.actor.home.pos.x - sin;
            this->dyna.actor.child->world.pos.z = this->dyna.actor.home.pos.z - cos;
        }
    }

    this->dyna.actor.shape.rot.y = this->dyna.actor.home.rot.y;
    set_slope(this, play);
}

static void mv_stop_init(BgSpot08Iceblock* this) {
    set_moveProc(this, NULL);
}

void Bg_Spot08_Iceblock_actor_move(Actor* thisx, PlayState* play) {
    BgSpot08Iceblock* this = (BgSpot08Iceblock*)thisx;

    if (fqrand() < 0.05f) {
        this->bobIncrSlow = get_random_timer(300, 100);
        this->bobIncrFast = get_random_timer(800, 400);
    }

    this->bobPhaseSlow += this->bobIncrSlow;
    this->bobPhaseFast += this->bobIncrFast;
    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void Bg_Spot08_Iceblock_actor_draw(Actor* thisx, PlayState* play) {
    Gfx* dList;
    BgSpot08Iceblock* this = (BgSpot08Iceblock*)thisx;

    switch (PARAMS_GET_NOSHIFT(this->dyna.actor.params, 9, 1)) {
        case 0:
            dList = gZorasFountainIcebergDL;
            break;
        case 0x200:
            dList = gZorasFountainIceRampDL;
            break;
    }

    Cheap_gfx_display(play, dList);
}
