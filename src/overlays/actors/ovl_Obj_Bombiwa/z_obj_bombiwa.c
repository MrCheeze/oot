/*
 * File: z_obj_bombiwa.c
 * Overlay: ovl_Obj_Bombiwa
 * Description: Round, brown, breakable boulder
 */

#include "z_obj_bombiwa.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "assets/objects/object_bombiwa/object_bombiwa.h"

#define FLAGS 0

void Obj_Bombiwa_actor_ct(Actor* thisx, PlayState* play);
void set_collision_bombiwa(Actor* thisx, PlayState* play);
void Obj_Bombiwa_actor_dt(Actor* thisx, PlayState* play2);
void Obj_Bombiwa_actor_move(Actor* thisx, PlayState* play);
void Obj_Bombiwa_actor_draw(Actor* thisx, PlayState* play);

void set_eff_bombstone(ObjBombiwa* this, PlayState* play);

ActorProfile Obj_Bombiwa_Profile = {
    /**/ ACTOR_OBJ_BOMBIWA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_BOMBIWA,
    /**/ sizeof(ObjBombiwa),
    /**/ Obj_Bombiwa_actor_ct,
    /**/ Obj_Bombiwa_actor_dt,
    /**/ Obj_Bombiwa_actor_move,
    /**/ Obj_Bombiwa_actor_draw,
};

static ColliderCylinderInit ClPipeDt_bombiwa = {
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
        { 0x4FC1FFFE, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 55, 70, 0, { 0 } },
};

static CollisionCheckInfoInit StatusDt_bombiwa = { 0, 12, 60, MASS_IMMOVABLE };

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 350, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

static s16 scl[] = {
    17, 14, 10, 8, 7, 5, 3, 2,
};

void set_collision_bombiwa(Actor* thisx, PlayState* play) {
    ObjBombiwa* this = (ObjBombiwa*)thisx;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &ClPipeDt_bombiwa);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
}

void Obj_Bombiwa_actor_ct(Actor* thisx, PlayState* play) {
    ValueSet_process(thisx, value_init);
    set_collision_bombiwa(thisx, play);
    if ((Actor_Environment_sw_Check(play, PARAMS_GET_U(thisx->params, 0, 6)) != 0)) {
        Actor_delete(thisx);
    } else {
        CollisionCheck_Status_set2(&thisx->colChkInfo, NULL, &StatusDt_bombiwa);
        if (thisx->shape.rot.y == 0) {
            s16 rand = (s16)rnd_f(65536.0f);

            thisx->world.rot.y = rand;
            thisx->shape.rot.y = rand;
        }
        thisx->shape.yOffset = -200.0f;
        thisx->world.pos.y = thisx->home.pos.y + 20.0f;
    }
}

void Obj_Bombiwa_actor_dt(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjBombiwa* this = (ObjBombiwa*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void set_eff_bombstone(ObjBombiwa* this, PlayState* play) {
    Vec3f pos;
    Vec3f velocity;
    Gfx* dlist;
    s16 arg5;
    s16 scale;
    s32 i;

    dlist = object_bombiwa_DL_0009E0;
    for (i = 0; i < ARRAY_COUNT(scl); i++) {
        pos.x = ((fqrand() - 0.5f) * 10.0f) + this->actor.home.pos.x;
        pos.y = ((fqrand() * 5.0f) + this->actor.home.pos.y) + 8.0f;
        pos.z = ((fqrand() - 0.5f) * 10.0f) + this->actor.home.pos.z;
        velocity.x = (fqrand() - 0.5f) * 15.0f;
        velocity.y = (fqrand() * 16.0f) + 5.0f;
        velocity.z = (fqrand() - 0.5f) * 15.0f;
        scale = scl[i];
        arg5 = (scale >= 11) ? 37 : 33;
        Effect_Kakera_ct2(play, &pos, &velocity, &pos, -400, arg5, 10, 2, 0, scale, 1, 0, 80, KAKERA_COLOR_NONE,
                             OBJECT_BOMBIWA, dlist);
    }
    dust_fly_set2(play, &this->actor.world.pos, 60.0f, 8, 100, 160, 1);
}

void Obj_Bombiwa_actor_move(Actor* thisx, PlayState* play) {
    ObjBombiwa* this = (ObjBombiwa*)thisx;
    s32 pad;

    if ((BlastVsMyCheck_c(play, &this->actor) != NULL) ||
        ((this->collider.base.acFlags & AC_HIT) && (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & DMG_HAMMER))) {
        set_eff_bombstone(this, play);
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->actor.params, 0, 6));
        Effect_SE_Info_new(play, &this->actor.world.pos, 80, NA_SE_EV_WALL_BROKEN);
        if (PARAMS_GET_U(this->actor.params, 15, 1) != 0) {
            Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        }
        Actor_delete(&this->actor);
    } else {
        this->collider.base.acFlags &= ~AC_HIT;
        if (this->actor.xzDistToPlayer < 800.0f) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void Obj_Bombiwa_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, object_bombiwa_DL_0009E0);
}
