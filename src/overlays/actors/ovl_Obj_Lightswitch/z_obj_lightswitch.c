/*
 * File: z_obj_lightswitch.c
 * Overlay: ovl_Obj_Lightswitch
 * Description: Sun Emblem Trigger (Spirit Temple)
 */

#include "z_obj_lightswitch.h"
#include "terminal.h"
#include "overlays/actors/ovl_Obj_Oshihiki/z_obj_oshihiki.h"
#include "assets/objects/object_lightswitch/object_lightswitch.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef enum FaceTextureIndex {
    /* 0x00 */ FACE_EYES_CLOSED,
    /* 0x01 */ FACE_EYES_OPEN,
    /* 0x02 */ FACE_EYES_OPEN_SMILING
} FaceTextureIndex;

void Obj_Lightswitch_actor_ct(Actor* thisx, PlayState* play);
void Obj_Lightswitch_actor_dt(Actor* thisx, PlayState* play2);
void Obj_Lightswitch_actor_move(Actor* thisx, PlayState* play2);
void Obj_Lightswitch_actor_draw(Actor* thisx, PlayState* play);

void mv_formOFF_init(ObjLightswitch* this);
void mv_formOFF(ObjLightswitch* this, PlayState* play);
void mv_OFFtoON_init(ObjLightswitch* this);
void mv_OFFtoON(ObjLightswitch* this, PlayState* play);
void mv_formON_init(ObjLightswitch* this);
void mv_formON(ObjLightswitch* this, PlayState* play);
void mv_ONtoOFF_init(ObjLightswitch* this);
void mv_ONtoOFF(ObjLightswitch* this, PlayState* play);
void mv_burndown1_init(ObjLightswitch* this);
void mv_burndown1(ObjLightswitch* this, PlayState* play);
void mv_burndown2_init(ObjLightswitch* this);
void mv_burndown2(ObjLightswitch* this, PlayState* play);

ActorProfile Obj_Lightswitch_Profile = {
    /**/ ACTOR_OBJ_LIGHTSWITCH,
    /**/ ACTORCAT_SWITCH,
    /**/ FLAGS,
    /**/ OBJECT_LIGHTSWITCH,
    /**/ sizeof(ObjLightswitch),
    /**/ Obj_Lightswitch_actor_ct,
    /**/ Obj_Lightswitch_actor_dt,
    /**/ Obj_Lightswitch_actor_move,
    /**/ Obj_Lightswitch_actor_draw,
};

static ColliderJntSphElementInit ClSphElemDt_lightswitch[] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00200000, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 19 }, 100 },
    },
};
static ColliderJntSphInit ClSphDt_lightswitch = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    ClSphElemDt_lightswitch,
};

static CollisionCheckInfoInit StatusDt_lightswitch = { 0, 12, 60, MASS_IMMOVABLE };

static void* LSW_FaceTxtTbl[] = { object_lightswitch_Tex_000C20, object_lightswitch_Tex_000420,
                                 object_lightswitch_Tex_001420 };

static Vec3f LSW_SwOnPos = { -1707.0f, 843.0f, -180.0f };
static Vec3f vec = { 0.0f, 0.0f, 0.0f };

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

void set_collision_data_lightswitch(ObjLightswitch* this, PlayState* play) {
    s32 pad;

    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &ClSphDt_lightswitch, this->colliderItems);
    Matrix_softcv3_load(this->actor.world.pos.x,
                                 this->actor.world.pos.y + (this->actor.shape.yOffset * this->actor.scale.y),
                                 this->actor.world.pos.z, &this->actor.shape.rot);
    Matrix_scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    CollisionCheck_Uty_convJntSphL2G(0, &this->collider);
}

static void set_switchON(ObjLightswitch* this, PlayState* play) {
    Actor* thisx = &this->actor; // required
    s32 type;

    if (!Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 8, 6))) {
        type = PARAMS_GET_U(this->actor.params, 4, 2);

        Actor_Environment_sw_On(play, PARAMS_GET_U(this->actor.params, 8, 6));

        if (type == OBJLIGHTSWITCH_TYPE_1) {
            makeActorAttentionDemoSE(play, thisx, NA_SE_SY_TRE_BOX_APPEAR);
        } else if (type == OBJLIGHTSWITCH_TYPE_BURN) {
            makeActorAttentionDemoSE(play, thisx, NA_SE_SY_ERROR);
        } else {
            makeActorAttentionDemoSE(play, thisx, NA_SE_SY_CORRECT_CHIME);
        }
    }
}

static void set_switchOFF(ObjLightswitch* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 8, 6))) {
        Actor_Environment_sw_Off(play, PARAMS_GET_U(this->actor.params, 8, 6));

        if (PARAMS_GET_U(this->actor.params, 4, 2) == OBJLIGHTSWITCH_TYPE_1) {
            makeActorAttentionDemoSE(play, &this->actor, NA_SE_SY_TRE_BOX_APPEAR);
        }
    }
}

void set_effect_burn(ObjLightswitch* this, PlayState* play) {
    Vec3f pos;
    f32 s = sin_s(this->actor.shape.rot.y);
    f32 c = cos_s(this->actor.shape.rot.y);
    f32 x;
    f32 y;
    f32 z;
    s32 pad;

    if (this->alpha >= (100 << 6)) {
        x = (CLAMP_MAX((1.0f - 1.0f / (255 << 6) * this->alpha) * 400.0f, 60.0f) - 30.0f + 30.0f) * fqrand();
        y = x - 30.0f;
        if (x > 30.0f) {
            x = 30.0f;
        } else {
            x = 900.0f - (y * y);
            if (x < 100.0f) {
                x = 100.0f;
            }
            x = sqrtf(x);
        }
        x = 2.0f * (x * (fqrand() - 0.5f));
        z = (30.0f - fabsf(x)) * 0.5f + 10.0f * fqrand();
        pos.x = this->actor.world.pos.x + ((z * s) + (x * c));
        pos.y = this->actor.world.pos.y + y + 10.0f;
        pos.z = this->actor.world.pos.z + ((z * c) - (x * s));
        _Effect_SS_Db_ct(play, &pos, &vec, &vec, 100, 0, 255, 255, 160, 160, 255, 0, 0, 1, 9, true);
    }
}

void Obj_Lightswitch_actor_ct(Actor* thisx, PlayState* play) {
    ObjLightswitch* this = (ObjLightswitch*)thisx;
    s32 switchFlagSet = Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 8, 6));
    s32 removeSelf = false;

    ValueSet_process(&this->actor, value_init);
    Actor_world_to_eye(&this->actor, 0.0f);
    if (switchFlagSet) {
        if (PARAMS_GET_U(this->actor.params, 4, 2) == OBJLIGHTSWITCH_TYPE_BURN) {
            removeSelf = true;
        } else {
            mv_formON_init(this);
        }
    } else {
        mv_formOFF_init(this);
    }
    if (PARAMS_GET_U(this->actor.params, 0, 1) == 1) {
        if (switchFlagSet) {
            xyz_t_move(&this->actor.world.pos, &LSW_SwOnPos);
            xyz_t_move(&this->actor.home.pos, &LSW_SwOnPos);
        }
        this->actor.shape.rot.x = -0x4000;
        this->actor.shape.rot.z = 0;
        this->actor.world.rot.x = this->actor.home.rot.x = this->actor.shape.rot.x;
        this->actor.world.rot.z = this->actor.home.rot.z = this->actor.shape.rot.z;
        this->actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
        if (Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_OBJ_OSHIHIKI, this->actor.home.pos.x,
                               this->actor.home.pos.y, this->actor.home.pos.z, 0, this->actor.home.rot.y, 0,
                               (0xFF << 8) | PUSHBLOCK_SMALL_START_ON) == NULL) {
            PRINTF_COLOR_ERROR();
            // "Push-pull block occurrence failure"
            PRINTF("押引ブロック発生失敗(%s %d)(arg_data 0x%04x)\n", "../z_obj_lightswitch.c", 452, this->actor.params);
            PRINTF_RST();
            removeSelf = true;
        }
    }
    set_collision_data_lightswitch(this, play);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &StatusDt_lightswitch);
    if (removeSelf) {
        Actor_delete(&this->actor);
    }
    // "Light switch"
    PRINTF("(光スイッチ)(arg_data 0x%04x)\n", this->actor.params);
}

void Obj_Lightswitch_actor_dt(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjLightswitch* this = (ObjLightswitch*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

void mv_formOFF_init(ObjLightswitch* this) {
    this->actionFunc = mv_formOFF;
    this->faceTextureIndex = FACE_EYES_CLOSED;
    this->color[0] = 155 << 6;
    this->color[1] = 125 << 6;
    this->color[2] = 255 << 6;
    this->alpha = 255 << 6;
}

void mv_formOFF(ObjLightswitch* this, PlayState* play) {
    switch (PARAMS_GET_U(this->actor.params, 4, 2)) {
        case OBJLIGHTSWITCH_TYPE_STAY_ON:
        case OBJLIGHTSWITCH_TYPE_2:
            if (this->collider.base.acFlags & AC_HIT) {
                mv_OFFtoON_init(this);
                set_switchON(this, play);
            }
            break;
        case OBJLIGHTSWITCH_TYPE_1:
            if ((this->collider.base.acFlags & AC_HIT) && !(this->prevFrameACflags & AC_HIT)) {
                mv_OFFtoON_init(this);
                set_switchON(this, play);
            }
            break;
        case OBJLIGHTSWITCH_TYPE_BURN:
            if (this->collider.base.acFlags & AC_HIT) {
                mv_burndown1_init(this);
                set_switchON(this, play);
            }
            break;
    }
}

void mv_OFFtoON_init(ObjLightswitch* this) {
    this->actionFunc = mv_OFFtoON;
    this->toggleDelay = 100;
    this->timer = 0;
    this->faceTextureIndex = FACE_EYES_CLOSED;
}

void mv_OFFtoON(ObjLightswitch* this, PlayState* play) {
    if (getAttentionDemoPart() == this->actor.category || this->toggleDelay <= 0) {
        if (this->timer == 0) {
            Actor_SE_set(&this->actor, NA_SE_EV_TRIFORCE_FLASH);
        }

        this->timer++;

        chase_s(&this->flameRingRotSpeed, -0xAA, 0xA);
        this->flameRingRot += this->flameRingRotSpeed;

        this->color[0] = this->timer * (((255 - 155) << 6) / 20) + (155 << 6);
        this->color[1] = this->timer * (((255 - 125) << 6) / 20) + (125 << 6);

        if (this->timer >= 20) {
            mv_formON_init(this);
        } else if (this->timer == 15) {
            this->faceTextureIndex = FACE_EYES_OPEN;
            Actor_SE_set(&this->actor, NA_SE_EV_FOOT_SWITCH);
        }
    }
}

void mv_formON_init(ObjLightswitch* this) {
    this->actionFunc = mv_formON;
    this->faceTextureIndex = FACE_EYES_OPEN_SMILING;

    this->color[0] = 255 << 6;
    this->color[1] = 255 << 6;
    this->color[2] = 255 << 6;
    this->alpha = 255 << 6;

    this->flameRingRotSpeed = -0xAA;
    this->timer = 0;
}

void mv_formON(ObjLightswitch* this, PlayState* play) {
    switch (PARAMS_GET_U(this->actor.params, 4, 2)) {
        case OBJLIGHTSWITCH_TYPE_STAY_ON:
            if (!Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 8, 6))) {
                mv_ONtoOFF_init(this);
            }
            break;
        case OBJLIGHTSWITCH_TYPE_1:
            if (this->collider.base.acFlags & AC_HIT && !(this->prevFrameACflags & AC_HIT)) {
                mv_ONtoOFF_init(this);
                set_switchOFF(this, play);
            }
            break;
        case OBJLIGHTSWITCH_TYPE_2:
            if (!(this->collider.base.acFlags & AC_HIT)) {
                if (this->timer >= 7) {
                    mv_ONtoOFF_init(this);
                    set_switchOFF(this, play);
                } else {
                    this->timer++;
                }
            } else {
                this->timer = 0;
            }
            break;
    }
    this->flameRingRot += this->flameRingRotSpeed;
}

void mv_ONtoOFF_init(ObjLightswitch* this) {
    this->actionFunc = mv_ONtoOFF;
    this->toggleDelay = 100;
    this->timer = 20;
    this->faceTextureIndex = FACE_EYES_OPEN;
}

void mv_ONtoOFF(ObjLightswitch* this, PlayState* play) {
    if (PARAMS_GET_U(this->actor.params, 4, 2) != OBJLIGHTSWITCH_TYPE_1 || getAttentionDemoPart() == this->actor.category ||
        this->toggleDelay <= 0) {
        this->timer--;

        chase_s(&this->flameRingRotSpeed, 0, 0xA);
        this->flameRingRot += this->flameRingRotSpeed;

        this->color[0] = this->timer * (((255 - 155) << 6) / 20) + (155 << 6);
        this->color[1] = this->timer * (((255 - 125) << 6) / 20) + (125 << 6);

        if (this->timer <= 0) {
            mv_formOFF_init(this);
        } else if (this->timer == 15) {
            this->faceTextureIndex = FACE_EYES_CLOSED;
            Actor_SE_set(&this->actor, NA_SE_EV_FOOT_SWITCH);
        }
    }
}

void mv_burndown1_init(ObjLightswitch* this) {
    this->actionFunc = mv_burndown1;
    this->toggleDelay = 100;
}

void mv_burndown1(ObjLightswitch* this, PlayState* play) {
    if (getAttentionDemoPart() == this->actor.category || this->toggleDelay <= 0) {
        mv_burndown2_init(this);
    }
}

void mv_burndown2_init(ObjLightswitch* this) {
    this->actionFunc = mv_burndown2;
    this->alpha = 255 << 6;
}

void mv_burndown2(ObjLightswitch* this, PlayState* play) {
    this->alpha -= 200;
    set_effect_burn(this, play);
    if (this->alpha < 0) {
        Actor_delete(&this->actor);
    }
}

void Obj_Lightswitch_actor_move(Actor* thisx, PlayState* play2) {
    ObjLightswitch* this = (ObjLightswitch*)thisx;
    PlayState* play = play2;

    if (this->toggleDelay > 0) {
        this->toggleDelay--;
    }

    this->actionFunc(this, play);

    if (this->actor.update != NULL) {
        if (PARAMS_GET_U(this->actor.params, 0, 1) == 1) {
            this->actor.world.pos.x = this->actor.child->world.pos.x;
            this->actor.world.pos.y = this->actor.child->world.pos.y + 60.0f;
            this->actor.world.pos.z = this->actor.child->world.pos.z;
            Actor_world_to_eye(&this->actor, 0.0f);
        }

        this->prevFrameACflags = this->collider.base.acFlags;
        this->collider.base.acFlags &= ~AC_HIT;
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void draw_lightswitch_opa(Actor* thisx, PlayState* play) {
    ObjLightswitch* this = (ObjLightswitch*)thisx;
    Vec3f pos;
    Vec3s rot;

    OPEN_DISPS(play->state.gfxCtx, "../z_obj_lightswitch.c", 809);
    _texture_z_light_fog_prim(play->state.gfxCtx);

    gDPSetEnvColor(POLY_OPA_DISP++, (u8)(this->color[0] >> 6), (u8)(this->color[1] >> 6), (u8)(this->color[2] >> 6),
                   (u8)(this->alpha >> 6));
    gSPSegment(POLY_OPA_DISP++, 0x09, &Actor_change_render_mode[2]);

    if (PARAMS_GET_U(thisx->params, 0, 1) == 1) {
        thisx->world.pos.x = thisx->child->world.pos.x;
        thisx->world.pos.y = thisx->child->world.pos.y + 60.0f;
        thisx->world.pos.z = thisx->child->world.pos.z;
        xyz_t_move(&pos, &thisx->world.pos);
        Matrix_softcv3_load(pos.x, pos.y, pos.z, &thisx->shape.rot);
        Matrix_scale(thisx->scale.x, thisx->scale.y, thisx->scale.z, MTXMODE_APPLY);
    } else {
        pos.x = thisx->world.pos.x;
        pos.y = thisx->world.pos.y + thisx->shape.yOffset * thisx->scale.y;
        pos.z = thisx->world.pos.z;
    }

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_obj_lightswitch.c", 841);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(LSW_FaceTxtTbl[this->faceTextureIndex]));
    gSPDisplayList(POLY_OPA_DISP++, object_lightswitch_DL_000260);

    rot.x = thisx->shape.rot.x;
    rot.y = thisx->shape.rot.y;
    rot.z = thisx->shape.rot.z + this->flameRingRot;
    Matrix_softcv3_load(pos.x, pos.y, pos.z, &rot);
    Matrix_scale(thisx->scale.x, thisx->scale.y, thisx->scale.z, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_obj_lightswitch.c", 859);
    gSPDisplayList(POLY_OPA_DISP++, object_lightswitch_DL_000398);

    rot.z = thisx->shape.rot.z - this->flameRingRot;
    Matrix_softcv3_load(pos.x, pos.y, pos.z, &rot);
    Matrix_scale(thisx->scale.x, thisx->scale.y, thisx->scale.z, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_obj_lightswitch.c", 873);
    gSPDisplayList(POLY_OPA_DISP++, object_lightswitch_DL_000408);

    CLOSE_DISPS(play->state.gfxCtx, "../z_obj_lightswitch.c", 878);
}

void draw_lightswitch_xlu(Actor* thisx, PlayState* play) {
    ObjLightswitch* this = (ObjLightswitch*)thisx;
    Vec3f sp68;
    Vec3s sp60;

    OPEN_DISPS(play->state.gfxCtx, "../z_obj_lightswitch.c", 890);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gDPSetEnvColor(POLY_XLU_DISP++, (u8)(this->color[0] >> 6), (u8)(this->color[1] >> 6), (u8)(this->color[2] >> 6),
                   (u8)(this->alpha >> 6));
    gSPSegment(POLY_XLU_DISP++, 0x09, Actor_change_render_mode);

    sp68.x = thisx->world.pos.x;
    sp68.y = thisx->world.pos.y + (thisx->shape.yOffset * thisx->scale.y);
    sp68.z = thisx->world.pos.z;

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_obj_lightswitch.c", 912);
    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(LSW_FaceTxtTbl[this->faceTextureIndex]));
    gSPDisplayList(POLY_XLU_DISP++, object_lightswitch_DL_000260);

    sp60.x = thisx->shape.rot.x;
    sp60.y = thisx->shape.rot.y;
    sp60.z = thisx->shape.rot.z + this->flameRingRot;

    Matrix_softcv3_load(sp68.x, sp68.y, sp68.z, &sp60);
    Matrix_scale(thisx->scale.x, thisx->scale.y, thisx->scale.z, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_obj_lightswitch.c", 930);
    gSPDisplayList(POLY_XLU_DISP++, object_lightswitch_DL_000398);

    sp60.z = thisx->shape.rot.z - this->flameRingRot;
    Matrix_softcv3_load(sp68.x, sp68.y, sp68.z, &sp60);
    Matrix_scale(thisx->scale.x, thisx->scale.y, thisx->scale.z, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_obj_lightswitch.c", 944);
    gSPDisplayList(POLY_XLU_DISP++, object_lightswitch_DL_000408);

    CLOSE_DISPS(play->state.gfxCtx, "../z_obj_lightswitch.c", 949);
}

void Obj_Lightswitch_actor_draw(Actor* thisx, PlayState* play) {
    ObjLightswitch* this = (ObjLightswitch*)thisx;
    s32 alpha = this->alpha >> 6 & 0xFF;

    if (PARAMS_GET_U(this->actor.params, 0, 1) == 1) {
        CollisionCheck_Uty_convJntSphL2G(0, &this->collider);
    }

    if (PARAMS_GET_U(this->actor.params, 4, 2) == OBJLIGHTSWITCH_TYPE_BURN && (alpha > 0 || alpha < 255)) {
        draw_lightswitch_xlu(thisx, play);
    } else {
        draw_lightswitch_opa(thisx, play);
    }
}
