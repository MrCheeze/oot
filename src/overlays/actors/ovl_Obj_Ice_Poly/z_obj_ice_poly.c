/*
 * File: z_obj_ice_poly.c
 * Overlay: ovl_Obj_Ice_Poly
 * Description: Ice / Frozen Actors
 */

#include "z_obj_ice_poly.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Obj_Ice_Poly_actor_ct(Actor* thisx, PlayState* play);
void Obj_Ice_Poly_actor_dt(Actor* thisx, PlayState* play);
void Obj_Ice_Poly_actor_move(Actor* thisx, PlayState* play);
void Obj_Ice_Poly_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(ObjIcePoly* this, PlayState* play);
void mode_melt(ObjIcePoly* this, PlayState* play);

ActorProfile Obj_Ice_Poly_Profile = {
    /**/ ACTOR_OBJ_ICE_POLY,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(ObjIcePoly),
    /**/ Obj_Ice_Poly_actor_ct,
    /**/ Obj_Ice_Poly_actor_dt,
    /**/ Obj_Ice_Poly_actor_move,
    /**/ Obj_Ice_Poly_actor_draw,
};

static ColliderCylinderInit IcePolyAllPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x02, 0x00 },
        { 0x00020800, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 50, 120, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit IcePolyShieldPipeData = {
    {
        COL_MATERIAL_HARD,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x4E01F7F6, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 50, 120, 0, { 0, 0, 0 } },
};

static f32 scale_data[] = { 0.5f, 1.0f, 1.5f };
static s16 world_offset[] = { -25, 0, -20 };
static Color_RGBA8 prim = { 250, 250, 250, 255 };
static Color_RGBA8 env = { 180, 180, 180, 255 };

void Obj_Ice_Poly_actor_ct(Actor* thisx, PlayState* play) {
    ObjIcePoly* this = (ObjIcePoly*)thisx;

    this->unk_151 = PARAMS_GET_U(thisx->params, 8, 8);
    thisx->params &= 0xFF;
    if (thisx->params < 0 || thisx->params >= 3) {
        Actor_delete(thisx);
        return;
    }
    Actor_set_scale(thisx, scale_data[thisx->params]);
    thisx->world.pos.y = world_offset[thisx->params] + thisx->home.pos.y;
    ClObjPipe_ct(play, &this->colliderIce);
    ClObjPipe_set5(play, &this->colliderIce, thisx, &IcePolyAllPipeData);
    ClObjPipe_ct(play, &this->colliderHard);
    ClObjPipe_set5(play, &this->colliderHard, thisx, &IcePolyShieldPipeData);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->colliderIce);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->colliderHard);
    thisx->colChkInfo.mass = MASS_IMMOVABLE;
    this->alpha = 255;
    this->colliderIce.dim.radius *= thisx->scale.x;
    this->colliderIce.dim.height *= thisx->scale.y;
    this->colliderHard.dim.radius *= thisx->scale.x;
    this->colliderHard.dim.height *= thisx->scale.y;
    Actor_world_to_eye(thisx, thisx->scale.y * 30.0f);
    this->actionFunc = mode_wait;
}

void Obj_Ice_Poly_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjIcePoly* this = (ObjIcePoly*)thisx;

    if ((this->actor.params >= 0) && (this->actor.params < 3)) {
        ClObjPipe_dt(play, &this->colliderIce);
        ClObjPipe_dt(play, &this->colliderHard);
    }
}

static void mode_wait(ObjIcePoly* this, PlayState* play) {
    static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };
    s32 pad;
    Vec3f pos;

    if (this->colliderIce.base.acFlags & AC_HIT) {
        this->meltTimer = -this->colliderIce.elem.acHitElem->atDmgInfo.damage;
        this->actor.focus.rot.y = this->actor.yawTowardsPlayer;
        makeOnepointDemo(play, 5120, 40, &this->actor, CAM_ID_MAIN);
        this->actionFunc = mode_melt;
    } else if (this->actor.parent != NULL) {
        this->actor.parent->freezeTimer = 40;
        CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderIce.base);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderIce.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderIce.base);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderHard.base);
    } else {
        Actor_delete(&this->actor);
    }
    pos.x = this->actor.world.pos.x + this->actor.scale.x * (get_random_timer(15, 15) * (fqrand() < 0.5f ? -1 : 1));
    pos.y = this->actor.world.pos.y + this->actor.scale.y * get_random_timer(10, 90);
    pos.z = this->actor.world.pos.z + this->actor.scale.z * (get_random_timer(15, 15) * (fqrand() < 0.5f ? -1 : 1));
    if ((play->gameplayFrames % 7) == 0) {
        Effect_SS_KiraKira_sc_ct_ct(play, &pos, &zero_vec, &zero_vec, &prim, &env, 2000, 5);
    }
}

void mode_melt(ObjIcePoly* this, PlayState* play) {
    Vec3f accel;
    Vec3f vel;
    Vec3f pos;
    s32 i;

    accel.x = 0.0f;
    accel.y = this->actor.scale.y;
    accel.z = 0.0f;
    vel.x = 0.0f;
    vel.y = this->actor.scale.y;
    vel.z = 0.0f;

    for (i = 0; i < 2; i++) {
        pos.x =
            this->actor.world.pos.x + this->actor.scale.x * (get_random_timer(20, 20) * (fqrand() < 0.5f ? -1 : 1));
        pos.y = this->actor.world.pos.y + this->actor.scale.y * fqrand() * 50.0f;
        pos.z =
            this->actor.world.pos.z + this->actor.scale.x * (get_random_timer(20, 20) * (fqrand() < 0.5f ? -1 : 1));
        Effect_SS_Dust_sc_cl_ct(play, &pos, &vel, &accel, &prim, &env,
                      get_random_timer(0x15E, 0x64) * this->actor.scale.x, this->actor.scale.x * 20.0f);
    }
    if (this->meltTimer < 0) {
        if (this->actor.parent != NULL) {
            this->actor.parent->freezeTimer = 40;
        }
        this->meltTimer++;
        if (this->meltTimer == 0) {
            this->meltTimer = 40;
            Actor_SE_set(&this->actor, NA_SE_EV_ICE_MELT);
        }
    } else {
        if (this->meltTimer != 0) {
            this->meltTimer--;
        }
        this->actor.scale.y = scale_data[this->actor.params] * (0.5f + (this->meltTimer * 0.0125f));
        this->alpha -= 6;
        if (this->meltTimer == 0) {
            Actor_delete(&this->actor);
        }
    }
}

void Obj_Ice_Poly_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjIcePoly* this = (ObjIcePoly*)thisx;

    this->actionFunc(this, play);
}

void Obj_Ice_Poly_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjIcePoly* this = (ObjIcePoly*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_obj_ice_poly.c", 421);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    Actor_HiliteReflect_xlu_set_init(&this->actor, play, 0);
    Matrix_rotateXYZ(0x500, 0, -0x500, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_obj_ice_poly.c", 428);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, play->gameplayFrames % 0x100, 0x20, 0x10, 1, 0,
                                (play->gameplayFrames * 2) % 0x100, 0x40, 0x20));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, this->alpha);
    gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment3DL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_obj_ice_poly.c", 444);
}
