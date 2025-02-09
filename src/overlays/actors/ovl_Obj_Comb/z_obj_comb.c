/*
 * File: z_obj_comb.c
 * Overlay: ovl_Obj_Comb
 * Description: Beehive
 */

#include "z_obj_comb.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "assets/objects/gameplay_field_keep/gameplay_field_keep.h"

#define FLAGS 0

void Obj_Comb_actor_ct(Actor* thisx, PlayState* play);
void Obj_Comb_actor_dt(Actor* thisx, PlayState* play2);
void Obj_Comb_actor_move(Actor* thisx, PlayState* play);
void Obj_Comb_actor_draw(Actor* thisx, PlayState* play);

void hahen_comb(ObjComb* this, PlayState* play);
void item_set_comb(ObjComb* this, PlayState* play);
static void mv_stop_init(ObjComb* this);
static void mv_stop(ObjComb* this, PlayState* play);

ActorProfile Obj_Comb_Profile = {
    /**/ ACTOR_OBJ_COMB,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_FIELD_KEEP,
    /**/ sizeof(ObjComb),
    /**/ Obj_Comb_actor_ct,
    /**/ Obj_Comb_actor_dt,
    /**/ Obj_Comb_actor_move,
    /**/ Obj_Comb_actor_draw,
};

static ColliderJntSphElementInit ClSphElemDt_comb[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x4001FFFE, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 15 }, 100 },
    },
};

static ColliderJntSphInit ClSphDt_comb = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    ClSphElemDt_comb,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 900, ICHAIN_STOP),
};

void hahen_comb(ObjComb* this, PlayState* play) {
    Vec3f pos1;
    Vec3f pos;
    Vec3f velocity;
    Gfx* dlist = gFieldBeehiveFragmentDL;
    s16 scale;
    s16 angle = 0;
    s16 gravity;
    u8 arg5;
    u8 arg6;
    f32 rand1;
    f32 rand2;
    s32 i;

    for (i = 0; i < 31; i++) {
        angle += 0x4E20;
        rand1 = fqrand() * 10.0f;

        pos1.x = sin_s(angle) * rand1;
        pos1.y = (i - 15) * 0.7f;
        pos1.z = cos_s(angle) * rand1;

        xyz_t_add(&pos1, &this->actor.world.pos, &pos);

        velocity.x = (fqrand() - 0.5f) + pos1.x * 0.5f;
        velocity.y = (fqrand() - 0.5f) + pos1.y * 0.6f;
        velocity.z = (fqrand() - 0.5f) + pos1.z * 0.5f;

        scale = fqrand() * 72.0f + 25.0f;

        if (scale < 40) {
            gravity = -200;
            arg6 = 40;
        } else if (scale < 70) {
            gravity = -280;
            arg6 = 30;
        } else {
            gravity = -340;
            arg6 = 20;
        }

        rand2 = fqrand();

        if (rand2 < 0.1f) {
            arg5 = 96;
        } else if (rand2 < 0.8f) {
            arg5 = 64;
        } else {
            arg5 = 32;
        }

        Effect_Kakera_ct2(play, &pos, &velocity, &pos, gravity, arg5, arg6, 4, 0, scale, 0, 0, 80, KAKERA_COLOR_NONE,
                             OBJECT_GAMEPLAY_FIELD_KEEP, dlist);
    }

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y - 10.0f;
    pos.z = this->actor.world.pos.z;
    dust_fly_set2(play, &pos, 40.0f, 6, 70, 60, 1);
}

void item_set_comb(ObjComb* this, PlayState* play) {
    s16 params = PARAMS_GET_U(this->actor.params, 0, 5);

    if ((params > 0) || (params < ITEM00_MAX)) { // conditional always true. May have been intended to be &&
        if (params == ITEM00_HEART_PIECE) {
            if (Actor_Environment_item_Check(play, PARAMS_GET_U(this->actor.params, 8, 6))) {
                params = -1;
            } else {
                params = (params | (PARAMS_GET_U(this->actor.params, 8, 6) << 8));
            }
        } else if (fqrand() < 0.5f) {
            params = -1;
        }
        if (params >= 0) {
            Item_set0(play, &this->actor.world.pos, params);
        }
    }
}

void Obj_Comb_actor_ct(Actor* thisx, PlayState* play) {
    ObjComb* this = (ObjComb*)thisx;

    ValueSet_process(&this->actor, value_init);
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &ClSphDt_comb, this->colliderItems);
    mv_stop_init(this);
}

void Obj_Comb_actor_dt(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjComb* this = (ObjComb*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

static void mv_stop_init(ObjComb* this) {
    this->actionFunc = mv_stop;
}

static void mv_stop(ObjComb* this, PlayState* play) {
    s32 dmgFlags;

    this->unk_1B0 -= 50;
    if (this->unk_1B0 < 0) {
        this->unk_1B0 = 0;
    }

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        dmgFlags = this->collider.elements[0].base.acHitElem->atDmgInfo.dmgFlags;
        if (dmgFlags & (DMG_HAMMER | DMG_ARROW | DMG_SLINGSHOT | DMG_DEKU_STICK)) {
            this->unk_1B0 = 1500;
        } else {
            hahen_comb(this, play);
            item_set_comb(this, play);
            Actor_delete(&this->actor);
        }
    } else {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->actor.update != NULL) {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void Obj_Comb_actor_move(Actor* thisx, PlayState* play) {
    ObjComb* this = (ObjComb*)thisx;

    this->unk_1B2 += 0x2EE0;
    this->actionFunc(this, play);
    this->actor.shape.rot.x = sin_s(this->unk_1B2) * this->unk_1B0 + this->actor.home.rot.x;
}

void Obj_Comb_actor_draw(Actor* thisx, PlayState* play) {
    ObjComb* this = (ObjComb*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_obj_comb.c", 369);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y + (118.0f * this->actor.scale.y),
                     this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_rotateY(BINANG_TO_RAD(this->actor.shape.rot.y), MTXMODE_APPLY);
    Matrix_rotateX(BINANG_TO_RAD(this->actor.shape.rot.x), MTXMODE_APPLY);
    Matrix_rotateZ(BINANG_TO_RAD(this->actor.shape.rot.z), MTXMODE_APPLY);
    Matrix_translate(0, -(this->actor.scale.y * 118.0f), 0, MTXMODE_APPLY);
    Matrix_scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_obj_comb.c", 394);

    gSPDisplayList(POLY_OPA_DISP++, gFieldBeehiveDL);

    CollisionCheck_Uty_convJntSphL2G(0, &this->collider);

    CLOSE_DISPS(play->state.gfxCtx, "../z_obj_comb.c", 402);
}
