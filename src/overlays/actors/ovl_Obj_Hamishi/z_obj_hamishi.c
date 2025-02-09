/*
 * File: z_obj_hamishi.c
 * Overlay: ovl_Obj_Hamishi
 * Description: Bronze Boulder
 */

#include "z_obj_hamishi.h"
#include "assets/objects/gameplay_field_keep/gameplay_field_keep.h"

#define FLAGS 0

void Obj_Hamishi_actor_ct(Actor* thisx, PlayState* play);
void Obj_Hamishi_actor_dt(Actor* thisx, PlayState* play2);
void Obj_Hamishi_actor_move(Actor* thisx, PlayState* play);
void Obj_Hamishi_actor_draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Hamishi_Profile = {
    /**/ ACTOR_OBJ_HAMISHI,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_FIELD_KEEP,
    /**/ sizeof(ObjHamishi),
    /**/ Obj_Hamishi_actor_ct,
    /**/ Obj_Hamishi_actor_dt,
    /**/ Obj_Hamishi_actor_move,
    /**/ Obj_Hamishi_actor_draw,
};

static ColliderCylinderInit ClPipeDt_hamishi = {
    {
        COL_MATERIAL_HARD,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x4FC1FFF6, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 50, 70, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit StatusDt_hamishi = { 0, 12, 60, MASS_IMMOVABLE };

static s16 scl[] = {
    145, 135, 115, 85, 75, 53, 45, 40, 35,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 250, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 500, ICHAIN_STOP),
};

void set_collision_data_hamishi(Actor* thisx, PlayState* play) {
    ObjHamishi* this = (ObjHamishi*)thisx;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &ClPipeDt_hamishi);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
}

void vib_proc_hamishi(ObjHamishi* this) {
    if (this->shakeFrames > 0) {
        this->shakeFrames--;
        this->shakePosPhase += 5000;
        this->shakeRotPhase += 0xE10;

        chase_f(&this->shakePosSize, 0.0f, 0.15f);
        chase_f(&this->shakeRotSize, 0.0f, 40.0f);

        this->actor.world.pos.x = this->actor.home.pos.x + (sin_s(this->shakePosPhase * 4) * this->shakePosSize);
        this->actor.world.pos.z = this->actor.home.pos.z + (cos_s(this->shakePosPhase * 7) * this->shakePosSize);
        this->actor.shape.rot.x =
            this->actor.home.rot.x + (s16)(sin_s(this->shakeRotPhase * 4) * this->shakeRotSize);
        this->actor.shape.rot.z =
            this->actor.home.rot.z + (s16)(cos_s(this->shakeRotPhase * 7) * this->shakeRotSize);
    } else {
        chase_f(&this->actor.world.pos.x, this->actor.home.pos.x, 1.0f);
        chase_f(&this->actor.world.pos.z, this->actor.home.pos.z, 1.0f);
        chase_angle(&this->actor.shape.rot.x, this->actor.home.rot.x, 0xBB8);
        chase_angle(&this->actor.shape.rot.z, this->actor.home.rot.z, 0xBB8);
    }
}

void effect_hamishi(ObjHamishi* this, PlayState* play) {
    s32 pad;
    Vec3f velocity;
    Vec3f pos;
    s16 phi_s0 = 1000;
    s16 gravity;
    s16 phi_v0;
    f32 temp_f20;
    f32 temp_f22;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(scl); i++) {
        phi_s0 += 20000;

        temp_f20 = fqrand() * 10.0f;
        pos.x = (sin_s(phi_s0) * temp_f20) + this->actor.world.pos.x;
        pos.y = (fqrand() * 40.0f) + this->actor.world.pos.y + 5.0f;
        pos.z = (cos_s(phi_s0) * temp_f20) + this->actor.world.pos.z;

        temp_f20 = (fqrand() * 10.0f) + 2.0f;
        velocity.x = sin_s(phi_s0) * temp_f20;
        temp_f22 = fqrand();
        velocity.y = (fqrand() * i * 2.5f) + (temp_f22 * 15.0f);
        velocity.z = cos_s(phi_s0) * temp_f20;

        if (i == 0) {
            phi_v0 = 41;
            gravity = -450;
        } else if (i < 4) {
            phi_v0 = 37;
            gravity = -380;
        } else {
            phi_v0 = 69;
            gravity = -320;
        }

        Effect_Kakera_ct2(play, &pos, &velocity, &this->actor.world.pos, gravity, phi_v0, 30, 5, 0, scl[i],
                             3, 0, 70, 1, OBJECT_GAMEPLAY_FIELD_KEEP, gSilverRockFragmentsDL);
    }

    dust_fly_set2(play, &this->actor.world.pos, 140.0f, 6, 180, 90, 1);
    dust_fly_set2(play, &this->actor.world.pos, 140.0f, 12, 80, 90, 1);
}

void Obj_Hamishi_actor_ct(Actor* thisx, PlayState* play) {
    ObjHamishi* this = (ObjHamishi*)thisx;

    ValueSet_process(&this->actor, value_init);

    if (play->csCtx.state != CS_STATE_IDLE) {
        this->actor.cullingVolumeDistance += 1000.0f;
    }
    if (this->actor.shape.rot.y == 0) {
        this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.home.rot.y = rnd_f(65536.0f);
    }

    set_collision_data_hamishi(&this->actor, play);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &StatusDt_hamishi);

    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 0, 6))) {
        Actor_delete(&this->actor);
        return;
    }

    this->actor.shape.yOffset = 80.0f;
}

void Obj_Hamishi_actor_dt(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjHamishi* this = (ObjHamishi*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void Obj_Hamishi_actor_move(Actor* thisx, PlayState* play) {
    ObjHamishi* this = (ObjHamishi*)thisx;
    CollisionCheckContext* colChkCtx = &play->colChkCtx;

    vib_proc_hamishi(this);

    if ((this->collider.base.acFlags & AC_HIT) && (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & DMG_HAMMER)) {
        this->collider.base.acFlags &= ~AC_HIT;
        this->hitCount++;
        if (this->hitCount < 2) {
            this->shakeFrames = 15;
            this->shakePosSize = 2.0f;
            this->shakeRotSize = 400.0f;
        } else {
            effect_hamishi(this, play);
            Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EV_WALL_BROKEN);
            Actor_Environment_sw_On(play, PARAMS_GET_U(this->actor.params, 0, 6));
            Actor_delete(&this->actor);
        }
    } else {
        this->collider.base.acFlags &= ~AC_HIT;

        if (this->actor.xzDistToPlayer < 600.0f) {
            CollisionCheck_setAC(play, colChkCtx, &this->collider.base);
            CollisionCheck_setOC(play, colChkCtx, &this->collider.base);
        }
    }
}

void Obj_Hamishi_actor_draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_obj_hamishi.c", 399);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_obj_hamishi.c", 404);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 170, 130, 255);
    gSPDisplayList(POLY_OPA_DISP++, gSilverRockDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_obj_hamishi.c", 411);
}
