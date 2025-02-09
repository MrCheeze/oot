/*
 * File: z_en_nwc.c
 * Overlay: ovl_En_Nwc
 * Description: Cluster of cucco chicks. Unfinished.
 */

#include "z_en_nwc.h"
#include "assets/objects/object_nwc/object_nwc.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Nwc_Actor_ct(Actor* thisx, PlayState* play);
void En_Nwc_Actor_dt(Actor* thisx, PlayState* play);
void En_Nwc_Actor_move(Actor* thisx, PlayState* play);
void En_Nwc_Actor_draw(Actor* thisx, PlayState* play);

void En_Nwc_Actor_set_process(EnNwc* this, EnNwcUpdateFunc updateFunc);
void Piyo_Object_non(EnNwcChick* chick, EnNwc* this, PlayState* play);
void Piyo_Object_BGcheck(EnNwcChick* chick, PlayState* play);
void Piyo_Object_wait(EnNwcChick* chick, EnNwc* this, PlayState* play);
void Piyo_Object_move(EnNwc* this, PlayState* play);
void Piyo_Object_draw(EnNwc* this, PlayState* play);
static void move_wait(EnNwc* this, PlayState* play);

#define CHICK_BG_FLOOR (1 << 0)
#define CHICK_BG_WALL (1 << 1)

typedef enum ChickTypes {
    /* 0 */ CHICK_NONE,
    /* 1 */ CHICK_NORMAL
} ChickTypes;

ActorProfile En_Nwc_Profile = {
    /**/ ACTOR_EN_NWC,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_NWC,
    /**/ sizeof(EnNwc),
    /**/ En_Nwc_Actor_ct,
    /**/ En_Nwc_Actor_dt,
    /**/ En_Nwc_Actor_move,
    /**/ En_Nwc_Actor_draw,
};

static ColliderJntSphElementInit PiyoElemData = {
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 0, { { 0, 0, 0 }, 10 }, 100 },
};

static ColliderJntSphInitType1 PiyoAcInfoData = {
    {
        COL_MATERIAL_HIT3,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_JNTSPH,
    },
    16,
    NULL,
};

void En_Nwc_Actor_set_process(EnNwc* this, EnNwcUpdateFunc updateFunc) {
    this->updateFunc = updateFunc;
}

void Piyo_Object_non(EnNwcChick* chick, EnNwc* this, PlayState* play) {
}

void Piyo_Object_BGcheck(EnNwcChick* chick, PlayState* play) {
    CollisionPoly* groundPoly;
    s32 bgId;
    Vec3f outPos;
    f32 dy;

    chick->bgFlags &= ~CHICK_BG_WALL & ~CHICK_BG_FLOOR;
    outPos.x = chick->pos.x;
    outPos.y = chick->pos.y;
    outPos.z = chick->pos.z;
    if (T_BGCheck_ObjWallCheck2(&play->colCtx, &outPos, &chick->pos, &chick->lastPos, 10.0f, &chick->floorPoly,
                                 20.0f)) {
        chick->bgFlags |= CHICK_BG_WALL;
    }
    //! @bug The use of outPos here is totally wrong. Even if it didn't get overwritten
    //       by the wall check, it should add an offset to the y-value so the raycast
    //       doesn't go through the floor and cause the chicks to ignore all floors.
    chick->floorY = T_BGCheck_ObjGroundCheck_ai(&play->colCtx, &groundPoly, &bgId, &outPos);
    dy = chick->floorY - chick->pos.y;
    if ((0.0f <= dy) && (dy < 40.0f)) {
        chick->pos.y = chick->floorY;
        chick->bgFlags |= CHICK_BG_FLOOR;
    }
}

void Piyo_Object_wait(EnNwcChick* chick, EnNwc* this, PlayState* play) {
    chick->velY -= 0.1f;
    if (chick->velY < -10.0f) {
        chick->velY = -10.0f;
    }
    chick->pos.y += chick->velY;
    Piyo_Object_BGcheck(chick, play);
    if (chick) {} // Needed for matching. Possibly from remnant of unfinished code?
}

void Piyo_Object_move(EnNwc* this, PlayState* play) {
    static EnNwcChickFunc piyo_object_proc[] = { Piyo_Object_non, Piyo_Object_wait };
    EnNwcChick* chick = this->chicks;
    ColliderJntSphElement* element = &this->collider.elements[0];
    Vec3f prevChickPos;
    s32 i;
    f32 test;

    prevChickPos.y = 99999.9f;
    for (i = 0; i < this->count; i++, prevChickPos = chick->pos, chick++, element++) {
        xyz_t_move(&chick->lastPos, &chick->pos);

        piyo_object_proc[chick->type](chick, this, play);

        element->dim.worldSphere.center.x = chick->pos.x;
        element->dim.worldSphere.center.y = chick->pos.y;
        element->dim.worldSphere.center.z = chick->pos.z;

        test = chick->pos.y - prevChickPos.y;
        if (fabsf(test) < 10.0f) {
            f32 dx = chick->pos.x - prevChickPos.x;
            f32 dz = chick->pos.z - prevChickPos.z;

            test = SQ(dx) + SQ(dz);
            if (test < SQ(10.0f)) {
                if (test != 0.0f) {
                    chick->pos.x += dx / sqrtf(test);
                    chick->pos.z += dz / sqrtf(test);
                } else {
                    chick->pos.x += 1.0f;
                    chick->pos.z += 1.0f;
                }
            }
        }
    }
}

void Piyo_Object_draw(EnNwc* this, PlayState* play) {
    s32 i;
    Gfx* dList1;
    Gfx* dList2;
    Gfx* dList3;
    MtxF floorMat;
    EnNwcChick* chick;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_nwc.c", 316);
    _texture_z_light_fog_prim2(play);

    dList1 = POLY_XLU_DISP;
    dList2 = dList1 + 3 * this->count + 1;
    dList3 = dList2 + 2 * this->count + 1;

    gSPDisplayList(dList1++, gCuccoChickSetupBodyDL);
    gSPDisplayList(dList2++, gCuccoChickSetupEyeDL);
    gSPDisplayList(dList3++, gCuccoChickSetupBeakDL);

    chick = this->chicks;
    for (i = 0; i < this->count; i++, chick++) {
        if (chick->type != CHICK_NONE) {
            Mtx* mtx;

            Matrix_softcv3_load(chick->pos.x, chick->pos.y + chick->height, chick->pos.z, &chick->rot);
            Matrix_scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
            mtx = MATRIX_FINALIZE(play->state.gfxCtx, "../z_en_nwc.c", 346);
            gDPSetEnvColor(dList1++, 0, 100, 255, 255);
            gSPMatrix(dList1++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(dList1++, gCuccoChickBodyDL);
            gSPMatrix(dList2++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(dList2++, gCuccoChickEyesDL);
            gSPMatrix(dList3++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(dList3++, gCuccoChickBeakDL);
        }
    }

    chick = this->chicks;
    POLY_XLU_DISP = dList3;
    _texture_decal_shadow(play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gCuccoChickSetupShadowDL);

    for (i = 0; i < this->count; i++, chick++) {
        if ((chick->type != CHICK_NONE) && (chick->floorPoly != NULL)) {
            T_Polygon_Ground_Matrix(chick->floorPoly, chick->pos.x, chick->floorY, chick->pos.z, &floorMat);
            Matrix_put(&floorMat);
            Matrix_rotateY(BINANG_TO_RAD(chick->rot.y), MTXMODE_APPLY);
            Matrix_scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_nwc.c", 388);
            gSPDisplayList(POLY_XLU_DISP++, gCuccoChickShadowDL);
        }
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_nwc.c", 395);
}

void En_Nwc_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnNwc* this = (EnNwc*)thisx;
    ColliderJntSphElementInit elementInits[16];
    ColliderJntSphElementInit* element;
    EnNwcChick* chick;
    s32 i;

    element = PiyoAcInfoData.elements = elementInits;
    for (i = 0; i < 16; i++, element++) {
        *element = PiyoElemData;
    }

    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set3(play, &this->collider, &this->actor, &PiyoAcInfoData);
    this->count = 16;
    chick = this->chicks;
    for (i = 0; i < this->count; i++, chick++) {
        chick->type = CHICK_NORMAL;
        chick->pos.x = thisx->world.pos.x + ((fqrand() * 100.0f) - 50.0f);
        chick->pos.y = thisx->world.pos.y + 20.0f;
        chick->pos.z = thisx->world.pos.z + ((fqrand() * 100.0f) - 50.0f);
        chick->height = 5;
    }
    En_Nwc_Actor_set_process(this, move_wait);
}

void En_Nwc_Actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnNwc* this = (EnNwc*)thisx;

    ClObjJntSph_dt(play, &this->collider);
}

static void move_wait(EnNwc* this, PlayState* play) {
    Piyo_Object_move(this, play);
}

void En_Nwc_Actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnNwc* this = (EnNwc*)thisx;

    this->updateFunc(this, play);
    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
}

void En_Nwc_Actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnNwc* this = (EnNwc*)thisx;

    Piyo_Object_draw(this, play);
}
