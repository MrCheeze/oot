/*
 * File: z_obj_hana.c
 * Overlay: Obj_Hana
 * Description: Grave Flower
 */

#include "z_obj_hana.h"
#include "assets/objects/gameplay_field_keep/gameplay_field_keep.h"

#define FLAGS 0

void Obj_Hana_actor_ct(Actor* thisx, PlayState* play);
void Obj_Hana_actor_dt(Actor* thisx, PlayState* play);
void Obj_Hana_actor_move(Actor* thisx, PlayState* play);
void Obj_Hana_actor_draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Hana_Profile = {
    /**/ ACTOR_OBJ_HANA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_FIELD_KEEP,
    /**/ sizeof(ObjHana),
    /**/ Obj_Hana_actor_ct,
    /**/ Obj_Hana_actor_dt,
    /**/ Obj_Hana_actor_move,
    /**/ Obj_Hana_actor_draw,
};

static ColliderCylinderInit ClPipeDt_hana = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 8, 10, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit StatusDt_hana = { 0, 12, 60, MASS_IMMOVABLE };

typedef struct HanaParams {
    /* 0x00 */ Gfx* dList;
    /* 0x04 */ f32 scale;
    /* 0x08 */ f32 yOffset;
    /* 0x0C */ s16 radius;
    /* 0x0E */ s16 height;
} HanaParams; // size = 0x10

static HanaParams OH_Data[] = {
    { gHanaDL, 0.01f, 0.0f, -1, 0 },
    { gFieldKakeraDL, 0.1f, 58.0f, 10, 18 },
    { gFieldBushDL, 0.4f, 0.0f, 12, 44 },
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 900, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 60, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 800, ICHAIN_STOP),
};

void Obj_Hana_actor_ct(Actor* thisx, PlayState* play) {
    ObjHana* this = (ObjHana*)thisx;
    s16 type = PARAMS_GET_U(this->actor.params, 0, 2);
    HanaParams* params = &OH_Data[type];

    ValueSet_process(&this->actor, value_init);
    Actor_set_scale(&this->actor, params->scale);
    this->actor.shape.yOffset = params->yOffset;
    if (params->radius >= 0) {
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, &this->actor, &ClPipeDt_hana);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        this->collider.dim.radius = params->radius;
        this->collider.dim.height = params->height;
        CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &StatusDt_hana);
    }

    if (type == 2 && GET_EVENTCHKINF(EVENTCHKINF_40)) {
        Actor_delete(&this->actor);
    }
}

void Obj_Hana_actor_dt(Actor* thisx, PlayState* play) {
    ObjHana* this = (ObjHana*)thisx;

    if (OH_Data[PARAMS_GET_U(this->actor.params, 0, 2)].radius >= 0) {
        ClObjPipe_dt(play, &this->collider);
    }
}

void Obj_Hana_actor_move(Actor* thisx, PlayState* play) {
    ObjHana* this = (ObjHana*)thisx;

    if (OH_Data[PARAMS_GET_U(this->actor.params, 0, 2)].radius >= 0 && this->actor.xzDistToPlayer < 400.0f) {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void Obj_Hana_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, OH_Data[PARAMS_GET_U(thisx->params, 0, 2)].dList);
}
