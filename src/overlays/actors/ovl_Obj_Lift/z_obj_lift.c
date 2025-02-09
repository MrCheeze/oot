/*
 * File: z_obj_lift.c
 * Overlay: ovl_Obj_Lift
 * Description: Square, collapsing platform
 */

#include "z_obj_lift.h"
#include "assets/objects/object_d_lift/object_d_lift.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "quake.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Obj_Lift_actor_ct(Actor* thisx, PlayState* play);
void Obj_Lift_actor_dt(Actor* thisx, PlayState* play);
void Obj_Lift_actor_move(Actor* thisx, PlayState* play);
void Obj_Lift_actor_draw(Actor* thisx, PlayState* play);

static void mv_wait_init(ObjLift* this);
static void mv_vibrato_init(ObjLift* this);
static void mv_drop_init(ObjLift* this);

static void mv_wait(ObjLift* this, PlayState* play);
static void mv_vibrato(ObjLift* this, PlayState* play);
static void mv_drop(ObjLift* this, PlayState* play);

ActorProfile Obj_Lift_Profile = {
    /**/ ACTOR_OBJ_LIFT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_D_LIFT,
    /**/ sizeof(ObjLift),
    /**/ Obj_Lift_actor_ct,
    /**/ Obj_Lift_actor_dt,
    /**/ Obj_Lift_actor_move,
    /**/ Obj_Lift_actor_draw,
};

static s16 WaitTimerData[] = { 0, 10, 20, 30, 40, 50, 60 };

typedef struct ObjLiftFramgentScale {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 z;
} ObjLiftFramgentScale; // size = 0x4

static ObjLiftFramgentScale pos_xz[] = {
    { 120, -120 }, { 120, 0 },     { 120, 120 }, { 0, -120 },   { 0, 0 },
    { 0, 120 },    { -120, -120 }, { -120, 0 },  { -120, 120 },
};

static InitChainEntry value_init[] = {
    ICHAIN_F32_DIV1000(gravity, -600, ICHAIN_CONTINUE),       ICHAIN_F32_DIV1000(minVelocityY, -15000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE), ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 2000, ICHAIN_STOP),
};

static f32 scale_init[] = { 0.1f, 0.05f };
static f32 pos_vecY[] = { -18.0f, -9.0f };

static void set_moveProc(ObjLift* this, ObjLiftActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

static void set_dynaPoly(ObjLift* this, PlayState* play, CollisionHeader* collision, s32 flags) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, flags);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_obj_lift.c", 188,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

void eff_lift_break(ObjLift* this, PlayState* play) {
    Vec3f pos;
    Vec3f velocity;
    Vec3f* temp_s3;
    s32 pad0;
    s32 i;

    temp_s3 = &this->dyna.actor.world.pos;

    for (i = 0; i < ARRAY_COUNT(pos_xz); i++) {
        pos.x = pos_xz[i].x * this->dyna.actor.scale.x + temp_s3->x;
        pos.y = temp_s3->y;
        pos.z = pos_xz[i].z * this->dyna.actor.scale.z + temp_s3->z;
        velocity.x = pos_xz[i].x * this->dyna.actor.scale.x * 0.8f;
        velocity.y = fqrand() * 10.0f + 6.0f;
        velocity.z = pos_xz[i].z * this->dyna.actor.scale.z * 0.8f;
        Effect_Kakera_ct2(play, &pos, &velocity, temp_s3, -256, (fqrand() < 0.5f) ? 64 : 32, 15, 15, 0,
                             (fqrand() * 50.0f + 50.0f) * this->dyna.actor.scale.x, 0, 32, 50, KAKERA_COLOR_NONE,
                             OBJECT_D_LIFT, gCollapsingPlatformDL);
    }

    if (PARAMS_GET_U(this->dyna.actor.params, 1, 1) == 0) {
        dust_fly_set2(play, &this->dyna.actor.world.pos, 120.0f, 12, 120, 100, 1);
    } else if (PARAMS_GET_U(this->dyna.actor.params, 1, 1) == 1) {
        dust_fly_set2(play, &this->dyna.actor.world.pos, 60.0f, 8, 60, 100, 1);
    }
}

void Obj_Lift_actor_ct(Actor* thisx, PlayState* play) {
    ObjLift* this = (ObjLift*)thisx;

    set_dynaPoly(this, play, &gCollapsingPlatformCol, DYNA_TRANSFORM_POS);

    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 2, 6))) {
        Actor_delete(&this->dyna.actor);
        return;
    }

    Actor_set_scale(&this->dyna.actor, scale_init[PARAMS_GET_U(this->dyna.actor.params, 1, 1)]);
    ValueSet_process(&this->dyna.actor, value_init);
    this->shakeOrientation.x = fqrand() * 65535.5f;
    this->shakeOrientation.y = fqrand() * 65535.5f;
    this->shakeOrientation.z = fqrand() * 65535.5f;
    mv_wait_init(this);
    PRINTF("(Dungeon Lift)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void Obj_Lift_actor_dt(Actor* thisx, PlayState* play) {
    ObjLift* this = (ObjLift*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mv_wait_init(ObjLift* this) {
    this->timer = WaitTimerData[PARAMS_GET_U(this->dyna.actor.params, 8, 3)];
    set_moveProc(this, mv_wait);
}

static void mv_wait(ObjLift* this, PlayState* play) {
    s32 pad;
    s32 quakeIndex;

    if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
        if (this->timer <= 0) {
            if (PARAMS_GET_U(this->dyna.actor.params, 8, 3) == 7) {
                mv_drop_init(this);
            } else {
                quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_1);
                setSpeedQuake(quakeIndex, 10000);
                setScaleQuake(quakeIndex, 2, 0, 0, 0);
                setTimerQuake(quakeIndex, 20);
                mv_vibrato_init(this);
            }
        }
    } else {
        this->timer = WaitTimerData[PARAMS_GET_U(this->dyna.actor.params, 8, 3)];
    }
}

static void mv_vibrato_init(ObjLift* this) {
    this->timer = 20;
    set_moveProc(this, mv_vibrato);
}

static void mv_vibrato(ObjLift* this, PlayState* play) {
    if (this->timer <= 0) {
        mv_drop_init(this);
    } else {
        this->shakeOrientation.x += 10000;
        this->dyna.actor.world.rot.x =
            (s16)(sin_s(this->shakeOrientation.x) * 300.0f) + this->dyna.actor.home.rot.x;
        this->dyna.actor.world.rot.z =
            (s16)(cos_s(this->shakeOrientation.x) * 300.0f) + this->dyna.actor.home.rot.z;
        this->dyna.actor.shape.rot.x = this->dyna.actor.world.rot.x;
        this->dyna.actor.shape.rot.z = this->dyna.actor.world.rot.z;
        this->shakeOrientation.y += 18000;
        this->dyna.actor.world.pos.y = sin_s(this->shakeOrientation.y) + this->dyna.actor.home.pos.y;
        this->shakeOrientation.z += 18000;
        this->dyna.actor.world.pos.x = sin_s(this->shakeOrientation.z) * 3.0f + this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = cos_s(this->shakeOrientation.z) * 3.0f + this->dyna.actor.home.pos.z;
    }

    if ((this->timer & 3) == 3) {
        Effect_SE_Info_new(play, &this->dyna.actor.world.pos, 16, NA_SE_EV_BLOCK_SHAKE);
    }
}

static void mv_drop_init(ObjLift* this) {
    set_moveProc(this, mv_drop);
    xyz_t_move(&this->dyna.actor.world.pos, &this->dyna.actor.home.pos);
    this->dyna.actor.shape.rot = this->dyna.actor.world.rot = this->dyna.actor.home.rot;
}

static void mv_drop(ObjLift* this, PlayState* play) {
    s32 pad;
    s32 bgId;
    Vec3f pos;

    Actor_position_moveF(&this->dyna.actor);
    xyz_t_move(&pos, &this->dyna.actor.prevPos);
    pos.y += pos_vecY[PARAMS_GET_U(this->dyna.actor.params, 1, 1)];
    this->dyna.actor.floorHeight =
        T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &this->dyna.actor.floorPoly, &bgId, &this->dyna.actor, &pos);

    if ((this->dyna.actor.floorHeight - this->dyna.actor.world.pos.y) >=
        (pos_vecY[PARAMS_GET_U(this->dyna.actor.params, 1, 1)] - 0.001f)) {
        eff_lift_break(this, play);
        Effect_SE_Info_new(play, &this->dyna.actor.world.pos, 20, NA_SE_EV_BOX_BREAK);
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 2, 6));
        Actor_delete(&this->dyna.actor);
    }
}

void Obj_Lift_actor_move(Actor* thisx, PlayState* play) {
    ObjLift* this = (ObjLift*)thisx;

    if (this->timer > 0) {
        this->timer--;
    }

    this->actionFunc(this, play);
}

void Obj_Lift_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gCollapsingPlatformDL);
}
