/*
 * File: z_bg_menkuri_eye.c
 * Overlay: ovl_Bg_Menkuri_Eye
 * Description: Eye platform eye switches
 */

#include "z_bg_menkuri_eye.h"
#include "assets/objects/object_menkuri_objects/object_menkuri_objects.h"

#define FLAGS ACTOR_FLAG_DRAW_CULLING_DISABLED

void Bg_Menkuri_Eye_actor_ct(Actor* thisx, PlayState* play);
void Bg_Menkuri_Eye_actor_dt(Actor* thisx, PlayState* play);
void Bg_Menkuri_Eye_actor_move(Actor* thisx, PlayState* play);
void Bg_Menkuri_Eye_actor_draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Menkuri_Eye_Profile = {
    /**/ ACTOR_BG_MENKURI_EYE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_MENKURI_OBJECTS,
    /**/ sizeof(BgMenkuriEye),
    /**/ Bg_Menkuri_Eye_actor_ct,
    /**/ Bg_Menkuri_Eye_actor_dt,
    /**/ Bg_Menkuri_Eye_actor_move,
    /**/ Bg_Menkuri_Eye_actor_draw,
};

static s32 arrow_hit_count;

static ColliderJntSphElementInit MenkuriEyeAcSphElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F820, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 1, { { 0, 0, 0 }, 14 }, 100 },
    },
};

static ColliderJntSphInit MenkuriEyeAcSphData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    MenkuriEyeAcSphElemData,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Menkuri_Eye_actor_ct(Actor* thisx, PlayState* play) {
    BgMenkuriEye* this = (BgMenkuriEye*)thisx;
    ColliderJntSphElement* colliderList;

    ValueSet_process(&this->actor, value_init);
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &MenkuriEyeAcSphData, this->colliderItems);
    this->collider.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider.elements[0].dim.worldSphere.center.y = this->actor.world.pos.y;
    this->collider.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;
    colliderList = this->collider.elements;
    colliderList->dim.worldSphere.radius = colliderList->dim.modelSphere.radius;
    if (!Actor_Environment_sw_Check(play, this->actor.params)) {
        arrow_hit_count = 0;
    }
    this->framesUntilDisable = -1;
}

void Bg_Menkuri_Eye_actor_dt(Actor* thisx, PlayState* play) {
    BgMenkuriEye* this = (BgMenkuriEye*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

void Bg_Menkuri_Eye_actor_move(Actor* thisx, PlayState* play) {
    BgMenkuriEye* this = (BgMenkuriEye*)thisx;

    if (!Actor_Environment_sw_Check(play, this->actor.params)) {
        if (this->framesUntilDisable != -1) {
            if (this->framesUntilDisable != 0) {
                this->framesUntilDisable--;
            }
            if (this->framesUntilDisable == 0) {
                this->framesUntilDisable = -1;
                arrow_hit_count--;
            }
        }
    }
    if ((this->collider.base.acFlags & AC_HIT) &&
        (ABS((s16)(this->collider.base.ac->world.rot.y - this->actor.shape.rot.y)) > 0x5000)) {
        this->collider.base.acFlags &= ~AC_HIT;
        if (this->framesUntilDisable == -1) {
            Actor_SE_set(&this->actor, NA_SE_EN_AMOS_DAMAGE);
            arrow_hit_count++;
            arrow_hit_count = CLAMP_MAX(arrow_hit_count, 4);
        }
        this->framesUntilDisable = 416;
        if (arrow_hit_count == 4) {
            Actor_Environment_sw_On(play, this->actor.params);
            Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        }
    }
    if (this->framesUntilDisable == -1) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
    Actor_world_to_eye(&this->actor, 0.0f);
}

void Bg_Menkuri_Eye_actor_draw(Actor* thisx, PlayState* play) {
    BgMenkuriEye* this = (BgMenkuriEye*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_menkuri_eye.c", 292);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    if (Actor_Environment_sw_Check(play, this->actor.params)) {
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 0, 0, 255);
    } else if (this->framesUntilDisable == -1) {
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 0, 0, 0);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 0, 0, 255);
    }
    Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_rotateXYZ(this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, MTXMODE_APPLY);
    Matrix_scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_menkuri_eye.c", 331);

    gSPDisplayList(POLY_XLU_DISP++, gGTGEyeStatueEyeDL);
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_menkuri_eye.c", 335);
}
