#include "gfx.h"
#include "macros.h"
#include "regs.h"
#include "sfx.h"
#include "sys_math3d.h"
#include "sys_matrix.h"
#include "terminal.h"
#include "versions.h"
#include "z64collision_check.h"
#include "z64effect.h"
#include "z64frame_advance.h"
#include "zelda_arena.h"
#include "z64play.h"

#include "overlays/effects/ovl_Effect_Ss_HitMark/z_eff_ss_hitmark.h"
#include "z_lib.h"

#pragma increment_block_number "gc-eu:128 gc-eu-mq:128 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128" \
                               "ique-cn:128 ntsc-1.0:96 ntsc-1.1:96 ntsc-1.2:96 pal-1.0:96 pal-1.1:96 hiratsu3:128"

typedef s32 (*ColChkResetFunc)(PlayState*, Collider*);
typedef void (*ColChkApplyFunc)(PlayState*, CollisionCheckContext*, Collider*);
typedef void (*ColChkVsFunc)(PlayState*, CollisionCheckContext*, Collider*, Collider*);
typedef s32 (*ColChkLineFunc)(PlayState*, CollisionCheckContext*, Collider*, Vec3f*, Vec3f*);

#define SAC_ENABLE (1 << 0)

#if DEBUG_FEATURES
/**
 * Draws a red triangle with vertices vA, vB, and vC.
 */
void Collider_DrawRedPoly(GraphicsContext* gfxCtx, Vec3f* vA, Vec3f* vB, Vec3f* vC) {
    Collider_DrawPoly(gfxCtx, vA, vB, vC, 255, 0, 0);
}

/**
 * Draws the triangle with vertices vA, vB, and vC and with the specified color.
 */
void Collider_DrawPoly(GraphicsContext* gfxCtx, Vec3f* vA, Vec3f* vB, Vec3f* vC, u8 r, u8 g, u8 b) {
    Vtx* vtxTbl;
    Vtx* vtx;
    f32 nx;
    f32 ny;
    f32 nz;
    f32 originDist;

    OPEN_DISPS(gfxCtx, "../z_collision_check.c", 713);

    gSPMatrix(POLY_OPA_DISP++, &Mtx_clear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0xFF, r, g, b, 50);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2);
    gSPTexture(POLY_OPA_DISP++, 0, 0, 0, G_TX_RENDERTILE, G_OFF);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, SHADE, 0, PRIMITIVE, 0, SHADE, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED, 0, 0, 0,
                      COMBINED);
    gSPClearGeometryMode(POLY_OPA_DISP++, G_CULL_BOTH);
    gSPSetGeometryMode(POLY_OPA_DISP++, G_LIGHTING);
    gDPPipeSync(POLY_OPA_DISP++);

    vtxTbl = GRAPH_ALLOC(gfxCtx, 3 * sizeof(Vtx));
    ASSERT(vtxTbl != NULL, "vtx_tbl != NULL", "../z_collision_check.c", 726);

    vtxTbl[0].n.ob[0] = vA->x;
    vtxTbl[0].n.ob[1] = vA->y;
    vtxTbl[0].n.ob[2] = vA->z;
    vtxTbl[1].n.ob[0] = vB->x;
    vtxTbl[1].n.ob[1] = vB->y;
    vtxTbl[1].n.ob[2] = vB->z;
    vtxTbl[2].n.ob[0] = vC->x;
    vtxTbl[2].n.ob[1] = vC->y;
    vtxTbl[2].n.ob[2] = vC->z;

    Math3DPlane(vA, vB, vC, &nx, &ny, &nz, &originDist);

    for (vtx = vtxTbl; vtx < vtxTbl + 3; vtx++) {
        vtx->n.flag = 0;
        vtx->n.tc[0] = 0;
        vtx->n.tc[1] = 0;
        vtx->n.n[0] = (u8)(s32)nx & 0xFF;
        vtx->n.n[1] = (u8)(s32)ny & 0xFF;
        vtx->n.n[2] = (u8)(s32)nz & 0xFF;
        vtx->n.a = 255;
    }

    gSPVertex(POLY_OPA_DISP++, vtxTbl, 3, 0);
    gSP1Triangle(POLY_OPA_DISP++, 0, 1, 2, 0);

    CLOSE_DISPS(gfxCtx, "../z_collision_check.c", 757);
}
#endif

s32 ClObj_ct(PlayState* play, Collider* col) {
    static Collider clobj_default = {
        NULL, NULL, NULL, NULL, AT_NONE, AC_NONE, OC1_NONE, OC2_NONE, COL_MATERIAL_HIT3, COLSHAPE_MAX,
    };

    *col = clobj_default;
    return true;
}

s32 ClObj_dt(PlayState* play, Collider* col) {
    return true;
}

/**
 * Uses default OC2_TYPE_1 and COL_MATERIAL_HIT0
 */
s32 ClObj_set(PlayState* play, Collider* col, ColliderInitToActor* src) {
    col->actor = src->actor;
    col->atFlags = src->atFlags;
    col->acFlags = src->acFlags;
    col->ocFlags1 = src->ocFlags1;
    col->ocFlags2 = OC2_TYPE_1;
    col->shape = src->shape;
    return true;
}

/**
 * Uses default OC2_TYPE_1
 */
s32 ClObj_set3(PlayState* play, Collider* col, Actor* actor, ColliderInitType1* src) {
    col->actor = actor;
    col->colMaterial = src->colMaterial;
    col->atFlags = src->atFlags;
    col->acFlags = src->acFlags;
    col->ocFlags1 = src->ocFlags1;
    col->ocFlags2 = OC2_TYPE_1;
    col->shape = src->shape;
    return true;
}

s32 ClObj_set4(PlayState* play, Collider* col, Actor* actor, ColliderInit* src) {
    col->actor = actor;
    col->colMaterial = src->colMaterial;
    col->atFlags = src->atFlags;
    col->acFlags = src->acFlags;
    col->ocFlags1 = src->ocFlags1;
    col->ocFlags2 = src->ocFlags2;
    col->shape = src->shape;
    return true;
}

void ClObj_ATClear(PlayState* play, Collider* col) {
    col->at = NULL;
    col->atFlags &= ~(AT_HIT | AT_BOUNCED);
}

void ClObj_ACClear(PlayState* play, Collider* col) {
    col->ac = NULL;
    col->acFlags &= ~(AC_HIT | AC_BOUNCED);
}

void ClObj_OCClear(PlayState* play, Collider* col) {
    col->oc = NULL;
    col->ocFlags1 &= ~OC1_HIT;
    col->ocFlags2 &= ~OC2_HIT_PLAYER;
}

s32 ClObjElem_AtBtlInfo_ct(PlayState* play, ColliderElementDamageInfoAT* atDmgInfo) {
    static ColliderElementDamageInfoAT cle_atbtlinfo_default = { 0x00000000, 0, 0 };

    *atDmgInfo = cle_atbtlinfo_default;
    return true;
}

s32 ClObjElem_AtBtlInfo_dt(PlayState* play, ColliderElementDamageInfoAT* atDmgInfo) {
    return true;
}

s32 ClObjElem_AtBtlInfo_set(PlayState* play, ColliderElementDamageInfoAT* dest,
                                    ColliderElementDamageInfoAT* src) {
    dest->dmgFlags = src->dmgFlags;
    dest->effect = src->effect;
    dest->damage = src->damage;
    return true;
}

void ClObjElem_AtBtlInfo_Clear(PlayState* play, ColliderElement* elem) {
}

s32 ClObjElem_AcBtlInfo_ct(PlayState* play, ColliderElementDamageInfoAC* acDmgInfo) {
    static ColliderElementDamageInfoAC cle_acbtlinfo_default = { 0xFFCFFFFF, 0, 0, { 0, 0, 0 } };

    *acDmgInfo = cle_acbtlinfo_default;
    return true;
}

s32 ClObjElem_AcBtlInfo_dt(PlayState* play, ColliderElementDamageInfoAC* acDmgInfo) {
    return true;
}

s32 ClObjElem_AcBtlInfo_set(PlayState* play, ColliderElementDamageInfoAC* acDmgInfo,
                                    ColliderElementDamageInfoACInit* clobj_default) {
    acDmgInfo->dmgFlags = clobj_default->dmgFlags;
    acDmgInfo->effect = clobj_default->effect;
    acDmgInfo->defense = clobj_default->defense;
    return true;
}

s32 ClObjElem_ct(PlayState* play, ColliderElement* elem) {
    static ColliderElement clobjelem_default = {
        { 0, 0, 0 },
        { 0xFFCFFFFF, 0, 0, { 0, 0, 0 } },
        ELEM_MATERIAL_UNK0,
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_NONE,
        NULL,
        NULL,
        NULL,
        NULL,
    };

    *elem = clobjelem_default;
    ClObjElem_AtBtlInfo_ct(play, &elem->atDmgInfo);
    ClObjElem_AcBtlInfo_ct(play, &elem->acDmgInfo);
    return true;
}

s32 ClObjElem_dt(PlayState* play, ColliderElement* elem) {
    ClObjElem_AtBtlInfo_dt(play, &elem->atDmgInfo);
    ClObjElem_AcBtlInfo_dt(play, &elem->acDmgInfo);
    return true;
}

s32 ClObjElem_set(PlayState* play, ColliderElement* elem, ColliderElementInit* elemInit) {
    elem->elemMaterial = elemInit->elemMaterial;
    ClObjElem_AtBtlInfo_set(play, &elem->atDmgInfo, &elemInit->atDmgInfo);
    ClObjElem_AcBtlInfo_set(play, &elem->acDmgInfo, &elemInit->acDmgInfo);
    elem->atElemFlags = elemInit->atElemFlags;
    elem->acElemFlags = elemInit->acElemFlags;
    elem->ocElemFlags = elemInit->ocElemFlags;
    return true;
}

void ClObjElem_ATClear(PlayState* play, ColliderElement* elem) {
    elem->atHit = NULL;
    elem->atHitElem = NULL;
    elem->atElemFlags &= ~ATELEM_HIT;
    elem->atElemFlags &= ~ATELEM_DREW_HITMARK;
    ClObjElem_AtBtlInfo_Clear(play, elem);
}

void ClObjElem_ACClear(PlayState* play, ColliderElement* elem) {
    elem->acDmgInfo.hitPos.x = elem->acDmgInfo.hitPos.y = elem->acDmgInfo.hitPos.z = 0;
    elem->acElemFlags &= ~ACELEM_HIT;
    elem->acElemFlags &= ~ACELEM_DRAW_HITMARK;
    elem->acHit = NULL;
    elem->acHitElem = NULL;
}

void ClObjElem_OCClear(PlayState* play, ColliderElement* elem) {
    elem->ocElemFlags &= ~OCELEM_HIT;
}

s32 ClObjJntSphElemAttr_ct(PlayState* play, ColliderJntSphElementDim* dim) {
    static ColliderJntSphElementDim default_jntsphelem_attr = {
        { { 0, 0, 0 }, 0 },
        { { 0, 0, 0 }, 0 },
        0.0f,
        0,
    };
    *dim = default_jntsphelem_attr;
    return true;
}

s32 ClObjJntSphElemAttr_dt(PlayState* play, ColliderJntSphElementDim* dim) {
    return true;
}

s32 ClObjJntSphElemAttr_set(PlayState* play, ColliderJntSphElementDim* dest, ColliderJntSphElementDimInit* src) {
    dest->limb = src->limb;
    dest->modelSphere = src->modelSphere;
    dest->scale = src->scale * 0.01f;
    return true;
}

s32 ClObjJntSphElem_ct(PlayState* play, ColliderJntSphElement* jntSphElem) {
    ClObjElem_ct(play, &jntSphElem->base);
    ClObjJntSphElemAttr_ct(play, &jntSphElem->dim);
    return true;
}

s32 ClObjJntSphElem_dt(PlayState* play, ColliderJntSphElement* jntSphElem) {
    ClObjElem_dt(play, &jntSphElem->base);
    ClObjJntSphElemAttr_dt(play, &jntSphElem->dim);
    return true;
}

s32 ClObjJntSphElem_set(PlayState* play, ColliderJntSphElement* dest, ColliderJntSphElementInit* src) {
    ClObjElem_set(play, &dest->base, &src->base);
    ClObjJntSphElemAttr_set(play, &dest->dim, &src->dim);
    return true;
}

s32 ClObjJntSphElem_ATClear(PlayState* play, ColliderJntSphElement* jntSphElem) {
    ClObjElem_ATClear(play, &jntSphElem->base);
    return true;
}

s32 ClObjJntSphElem_ACClear(PlayState* play, ColliderJntSphElement* jntSphElem) {
    ClObjElem_ACClear(play, &jntSphElem->base);
    return true;
}

s32 ClObjJntSphElem_OCClear(PlayState* play, ColliderJntSphElement* jntSphElem) {
    ClObjElem_OCClear(play, &jntSphElem->base);
    return true;
}

/**
 * Initializes a ColliderJntSph to default values
 */
s32 ClObjJntSph_ct(PlayState* play, ColliderJntSph* jntSph) {
    ClObj_ct(play, &jntSph->base);
    jntSph->count = 0;
    jntSph->elements = NULL;
    return true;
}

/**
 * Destroys a dynamically allocated ColliderJntSph
 */
s32 ClObjJntSph_dt(PlayState* play, ColliderJntSph* jntSph) {
    ColliderJntSphElement* jntSphElem;

    ClObj_dt(play, &jntSph->base);
    for (jntSphElem = jntSph->elements; jntSphElem < jntSph->elements + jntSph->count; jntSphElem++) {
        ClObjJntSphElem_dt(play, jntSphElem);
    }

    jntSph->count = 0;
    if (jntSph->elements != NULL) {
        ZELDA_ARENA_FREE(jntSph->elements, "../z_collision_check.c", 1393);
    }
    jntSph->elements = NULL;
    return true;
}

/**
 * Destroys a preallocated ColliderJntSph
 */
s32 ClObjJntSph_dt_nzf(PlayState* play, ColliderJntSph* jntSph) {
    ColliderJntSphElement* jntSphElem;

    ClObj_dt(play, &jntSph->base);
    for (jntSphElem = jntSph->elements; jntSphElem < jntSph->elements + jntSph->count; jntSphElem++) {
        ClObjJntSphElem_dt(play, jntSphElem);
    }
    jntSph->count = 0;
    jntSph->elements = NULL;
    return true;
}

/**
 * Sets up the ColliderJntSph using the values in src, sets it to the actor specified in src, and dynamically allocates
 * the element array. Uses default OC2_TYPE_1 and COL_MATERIAL_HIT0. Unused.
 */
s32 ClObjJntSph_set(PlayState* play, ColliderJntSph* dest, ColliderJntSphInitToActor* src) {
    ColliderJntSphElement* destElem;
    ColliderJntSphElementInit* srcElem;

    ClObj_set(play, &dest->base, &src->base);
    dest->count = src->count;
    dest->elements = ZELDA_ARENA_MALLOC(src->count * sizeof(ColliderJntSphElement), "../z_collision_check.c", 1443);

    if (dest->elements == NULL) {
        dest->count = 0;
        PRINTF_COLOR_RED();
        PRINTF(T("ClObjJntSph_set():zelda_malloc()出来ません。\n", "ClObjJntSph_set():zelda_malloc() Can not.\n"));
        PRINTF_RST();
        return false;
    }

    for (destElem = dest->elements, srcElem = src->elements; destElem < dest->elements + dest->count;
         destElem++, srcElem++) {
        ClObjJntSphElem_ct(play, destElem);
        ClObjJntSphElem_set(play, destElem, srcElem);
    }
    return true;
}

/**
 * Sets up the ColliderJntSph using the values in src and dynamically allocates the element array. Uses default
 * OC2_TYPE_1. Only used by En_Nwc, an unused and unfinished actor.
 */
s32 ClObjJntSph_set3(PlayState* play, ColliderJntSph* dest, Actor* actor, ColliderJntSphInitType1* src) {
    ColliderJntSphElement* destElem;
    ColliderJntSphElementInit* srcElem;

    ClObj_set3(play, &dest->base, actor, &src->base);
    dest->count = src->count;
    dest->elements = ZELDA_ARENA_MALLOC(src->count * sizeof(ColliderJntSphElement), "../z_collision_check.c", 1490);

    if (dest->elements == NULL) {
        dest->count = 0;
        PRINTF_COLOR_RED();
        PRINTF(T("ClObjJntSph_set3():zelda_malloc_出来ません。\n", "ClObjJntSph_set3():zelda_malloc_ Can not.\n"));
        PRINTF_RST();
        return false;
    }

    for (destElem = dest->elements, srcElem = src->elements; destElem < dest->elements + dest->count;
         destElem++, srcElem++) {
        ClObjJntSphElem_ct(play, destElem);
        ClObjJntSphElem_set(play, destElem, srcElem);
    }
    return true;
}

/**
 * Sets up the ColliderJntSph using the values in src and dynamically allocates the element array.
 * Unused.
 */
s32 ClObjJntSph_set5(PlayState* play, ColliderJntSph* dest, Actor* actor, ColliderJntSphInit* src) {
    ColliderJntSphElement* destElem;
    ColliderJntSphElementInit* srcElem;

    ClObj_set4(play, &dest->base, actor, &src->base);
    dest->count = src->count;
    dest->elements = ZELDA_ARENA_MALLOC(src->count * sizeof(ColliderJntSphElement), "../z_collision_check.c", 1551);

    if (dest->elements == NULL) {
        dest->count = 0;
        PRINTF_COLOR_RED();
        PRINTF(T("ClObjJntSph_set5():zelda_malloc出来ません\n", "ClObjJntSph_set5():zelda_malloc Can not\n"));
        PRINTF_RST();
        return false;
    }
    for (destElem = dest->elements, srcElem = src->elements; destElem < dest->elements + dest->count;
         destElem++, srcElem++) {
        ClObjJntSphElem_ct(play, destElem);
        ClObjJntSphElem_set(play, destElem, srcElem);
    }
    return true;
}

/**
 * Sets up the ColliderJntSph using the values in src, placing the element array in elements.
 */
s32 ClObjJntSph_set5_nzm(PlayState* play, ColliderJntSph* dest, Actor* actor, ColliderJntSphInit* src,
                       ColliderJntSphElement* jntSphElements) {
    ColliderJntSphElement* destElem;
    ColliderJntSphElementInit* srcElem;

    ClObj_set4(play, &dest->base, actor, &src->base);
    dest->count = src->count;
    dest->elements = jntSphElements;
    ASSERT(dest->elements != NULL, "pclobj_jntsph->elem_tbl != NULL", "../z_collision_check.c", 1603);

    for (destElem = dest->elements, srcElem = src->elements; destElem < dest->elements + dest->count;
         destElem++, srcElem++) {
        ClObjJntSphElem_ct(play, destElem);
        ClObjJntSphElem_set(play, destElem, srcElem);
    }
    return true;
}

/**
 * Resets the collider's AT collision flags.
 */
s32 ClObjJntSph_ATClear(PlayState* play, Collider* col) {
    ColliderJntSphElement* jntSphElem;
    ColliderJntSph* jntSph = (ColliderJntSph*)col;

    ClObj_ATClear(play, &jntSph->base);

    for (jntSphElem = jntSph->elements; jntSphElem < jntSph->elements + jntSph->count; jntSphElem++) {
        ClObjJntSphElem_ATClear(play, jntSphElem);
    }
    return true;
}

/**
 * Resets the collider's AC collision flags.
 */
s32 ClObjJntSph_ACClear(PlayState* play, Collider* col) {
    ColliderJntSphElement* jntSphElem;
    ColliderJntSph* jntSph = (ColliderJntSph*)col;

    ClObj_ACClear(play, &jntSph->base);

    for (jntSphElem = jntSph->elements; jntSphElem < jntSph->elements + jntSph->count; jntSphElem++) {
        ClObjJntSphElem_ACClear(play, jntSphElem);
    }
    return true;
}

/**
 * Resets the collider's OC collision flags.
 */
s32 ClObjJntSph_OCClear(PlayState* play, Collider* col) {
    ColliderJntSphElement* jntSphElem;
    ColliderJntSph* jntSph = (ColliderJntSph*)col;

    ClObj_OCClear(play, &jntSph->base);

    for (jntSphElem = jntSph->elements; jntSphElem < jntSph->elements + jntSph->count; jntSphElem++) {
        ClObjJntSphElem_OCClear(play, jntSphElem);
    }
    return true;
}

s32 ClObjPipeAttr_ct(PlayState* play, Cylinder16* dim) {
    Cylinder16 clobj_default = { 0, 0, 0, { 0, 0, 0 } };

    *dim = clobj_default;
    return true;
}

s32 ClObjPipeAttr_dt(PlayState* play, Cylinder16* dim) {
    return true;
}

s32 ClObjPipeAttr_set(PlayState* play, Cylinder16* dest, Cylinder16* src) {
    *dest = *src;
    return true;
}

/**
 * Initializes a ColliderCylinder to default values
 */
s32 ClObjPipe_ct(PlayState* play, ColliderCylinder* cyl) {
    ClObj_ct(play, &cyl->base);
    ClObjElem_ct(play, &cyl->elem);
    ClObjPipeAttr_ct(play, &cyl->dim);
    return true;
}

/**
 * Destroys a ColliderCylinder
 */
s32 ClObjPipe_dt(PlayState* play, ColliderCylinder* cyl) {
    ClObj_dt(play, &cyl->base);
    ClObjElem_dt(play, &cyl->elem);
    ClObjPipeAttr_dt(play, &cyl->dim);
    return true;
}

/**
 * Sets up the ColliderCylinder using the values in src and sets it to the actor specified in src. Uses default
 * OC2_TYPE_1 and COL_MATERIAL_0. Used only by DekuJr, who sets it to himself anyways.
 */
s32 ClObjPipe_set(PlayState* play, ColliderCylinder* dest, ColliderCylinderInitToActor* src) {
    ClObj_set(play, &dest->base, &src->base);
    ClObjElem_set(play, &dest->elem, &src->elem);
    ClObjPipeAttr_set(play, &dest->dim, &src->dim);
    return true;
}

/**
 * Sets up the ColliderCylinder using the values in src. Uses default OC2_TYPE_1
 */
s32 ClObjPipe_set3(PlayState* play, ColliderCylinder* dest, Actor* actor, ColliderCylinderInitType1* src) {
    ClObj_set3(play, &dest->base, actor, &src->base);
    ClObjElem_set(play, &dest->elem, &src->elem);
    ClObjPipeAttr_set(play, &dest->dim, &src->dim);
    return true;
}

/**
 * Sets up the ColliderCylinder using the values in src.
 */
s32 ClObjPipe_set5(PlayState* play, ColliderCylinder* dest, Actor* actor, ColliderCylinderInit* src) {
    ClObj_set4(play, &dest->base, actor, &src->base);
    ClObjElem_set(play, &dest->elem, &src->elem);
    ClObjPipeAttr_set(play, &dest->dim, &src->dim);
    return true;
}

/**
 * Resets the collider's AT collision flags.
 */
s32 ClObjPipe_ATClear(PlayState* play, Collider* col) {
    ColliderCylinder* cyl = (ColliderCylinder*)col;

    ClObj_ATClear(play, &cyl->base);
    ClObjElem_ATClear(play, &cyl->elem);
    return true;
}

/**
 * Resets the collider's AC collision flags.
 */
s32 ClObjPipe_ACClear(PlayState* play, Collider* col) {
    ColliderCylinder* cyl = (ColliderCylinder*)col;

    ClObj_ACClear(play, &cyl->base);
    ClObjElem_ACClear(play, &cyl->elem);
    return true;
}

/**
 * Resets the collider's OC collision flags.
 */
s32 ClObjPipe_OCClear(PlayState* play, Collider* col) {
    ColliderCylinder* cyl = (ColliderCylinder*)col;

    ClObj_OCClear(play, &cyl->base);
    ClObjElem_OCClear(play, &cyl->elem);
    return true;
}

s32 ClObjTrisElemAttr_ct(PlayState* play, TriNorm* dim) {
    static TriNorm default_clobjtriselem_attr = {
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
        { { 0.0f, 0.0f, 0.0f }, 0.0f },
    };

    *dim = default_clobjtriselem_attr;
    return true;
}

s32 ClObjTrisElemAttr_dt(PlayState* play, TriNorm* dim) {
    return true;
}

s32 ClObjTrisElemAttr_set(PlayState* play, TriNorm* dest, ColliderTrisElementDimInit* src) {
    Vec3f* destVtx;
    Vec3f* srcVtx;
    f32 nx;
    f32 ny;
    f32 nz;
    f32 originDist;

    for (destVtx = dest->vtx, srcVtx = src->vtx; destVtx < dest->vtx + 3; destVtx++, srcVtx++) {
        *destVtx = *srcVtx;
    }

    Math3DPlane(&src->vtx[0], &src->vtx[1], &src->vtx[2], &nx, &ny, &nz, &originDist);

    dest->plane.normal.x = nx;
    dest->plane.normal.y = ny;
    dest->plane.normal.z = nz;
    dest->plane.originDist = originDist;
    return true;
}

s32 ClObjTrisElem_ct(PlayState* play, ColliderTrisElement* trisElem) {
    ClObjElem_ct(play, &trisElem->base);
    ClObjTrisElemAttr_ct(play, &trisElem->dim);
    return true;
}

s32 ClObjTrisElem_dt(PlayState* play, ColliderTrisElement* trisElem) {
    ClObjElem_dt(play, &trisElem->base);
    ClObjTrisElemAttr_dt(play, &trisElem->dim);
    return true;
}

s32 ClObjTrisElem_set(PlayState* play, ColliderTrisElement* dest, ColliderTrisElementInit* src) {
    ClObjElem_set(play, &dest->base, &src->base);
    ClObjTrisElemAttr_set(play, &dest->dim, &src->dim);
    return true;
}

s32 ClObjTrisElem_ATClear(PlayState* play, ColliderTrisElement* trisElem) {
    ClObjElem_ATClear(play, &trisElem->base);
    return true;
}

s32 ClObjTrisElem_ACClear(PlayState* play, ColliderTrisElement* trisElem) {
    ClObjElem_ACClear(play, &trisElem->base);
    return true;
}

s32 ClObjTrisElem_OCClear(PlayState* play, ColliderTrisElement* trisElem) {
    ClObjElem_OCClear(play, &trisElem->base);
    return true;
}

/**
 * Initializes a ColliderTris to default values
 */
s32 ClObjTris_ct(PlayState* play, ColliderTris* tris) {
    ClObj_ct(play, &tris->base);
    tris->count = 0;
    tris->elements = NULL;
    return true;
}

/**
 * Destroys a dynamically allocated ColliderTris
 * Unused
 */
s32 ClObjTris_dt(PlayState* play, ColliderTris* tris) {
    ColliderTrisElement* trisElem;

    ClObj_dt(play, &tris->base);
    for (trisElem = tris->elements; trisElem < tris->elements + tris->count; trisElem++) {
        ClObjTrisElem_dt(play, trisElem);
    }

    tris->count = 0;
    if (tris->elements != NULL) {
        ZELDA_ARENA_FREE(tris->elements, "../z_collision_check.c", 2099);
    }
    tris->elements = NULL;
    return true;
}

/**
 * Destroys a preallocated ColliderTris
 */
s32 ClObjTris_dt_nzf(PlayState* play, ColliderTris* tris) {
    ColliderTrisElement* trisElem;

    ClObj_dt(play, &tris->base);
    for (trisElem = tris->elements; trisElem < tris->elements + tris->count; trisElem++) {
        ClObjTrisElem_dt(play, trisElem);
    }

    tris->count = 0;
    tris->elements = NULL;
    return true;
}

/**
 * Sets up the ColliderTris using the values in src and dynamically allocates the element array. Uses default OC2_TYPE_1
 * Unused.
 */
s32 ClObjTris_set3(PlayState* play, ColliderTris* dest, Actor* actor, ColliderTrisInitType1* src) {
    ColliderTrisElement* destElem;
    ColliderTrisElementInit* srcElem;

    ClObj_set3(play, &dest->base, actor, &src->base);
    dest->count = src->count;
    dest->elements = ZELDA_ARENA_MALLOC(dest->count * sizeof(ColliderTrisElement), "../z_collision_check.c", 2156);
    if (dest->elements == NULL) {
        dest->count = 0;
        PRINTF_COLOR_RED();
        PRINTF(T("ClObjTris_set3():zelda_malloc()出来ません\n", "ClObjTris_set3():zelda_malloc() Can not\n"));
        PRINTF_RST();
        return false;
    }
    for (destElem = dest->elements, srcElem = src->elements; destElem < dest->elements + dest->count;
         destElem++, srcElem++) {
        ClObjTrisElem_ct(play, destElem);
        ClObjTrisElem_set(play, destElem, srcElem);
    }
    return true;
}

/**
 * Sets up the ColliderTris using the values in src and dynamically allocates the element array.
 * Unused
 */
s32 ClObjTris_set5(PlayState* play, ColliderTris* dest, Actor* actor, ColliderTrisInit* src) {
    ColliderTrisElement* destElem;
    ColliderTrisElementInit* srcElem;

    ClObj_set4(play, &dest->base, actor, &src->base);
    dest->count = src->count;
    dest->elements = ZELDA_ARENA_MALLOC(dest->count * sizeof(ColliderTrisElement), "../z_collision_check.c", 2207);

    if (dest->elements == NULL) {
        PRINTF_COLOR_RED();
        PRINTF(T("ClObjTris_set5():zelda_malloc出来ません\n", "ClObjTris_set5():zelda_malloc Can not\n"));
        PRINTF_RST();
        dest->count = 0;
        return false;
    }

    for (destElem = dest->elements, srcElem = src->elements; destElem < dest->elements + dest->count;
         destElem++, srcElem++) {
        ClObjTrisElem_ct(play, destElem);
        ClObjTrisElem_set(play, destElem, srcElem);
    }
    return true;
}

/**
 * Sets up the ColliderTris using the values in src, placing the element array in elements.
 */
s32 ClObjTris_set5_nzm(PlayState* play, ColliderTris* dest, Actor* actor, ColliderTrisInit* src,
                     ColliderTrisElement* trisElements) {
    ColliderTrisElement* destElem;
    ColliderTrisElementInit* srcElem;

    ClObj_set4(play, &dest->base, actor, &src->base);
    dest->count = src->count;
    dest->elements = trisElements;
    ASSERT(dest->elements != NULL, "pclobj_tris->elem_tbl != NULL", "../z_collision_check.c", 2258);

    for (destElem = dest->elements, srcElem = src->elements; destElem < dest->elements + dest->count;
         destElem++, srcElem++) {
        ClObjTrisElem_ct(play, destElem);
        ClObjTrisElem_set(play, destElem, srcElem);
    }
    return true;
}

/**
 * Resets the collider's AT collision flags.
 */
s32 ClObjTris_ATClear(PlayState* play, Collider* col) {
    ColliderTrisElement* trisElem;
    ColliderTris* tris = (ColliderTris*)col;

    ClObj_ATClear(play, &tris->base);
    for (trisElem = tris->elements; trisElem < tris->elements + tris->count; trisElem++) {
        ClObjTrisElem_ATClear(play, trisElem);
    }
    return true;
}

/**
 * Resets the collider's AC collision flags.
 */
s32 ClObjTris_ACClear(PlayState* play, Collider* col) {
    ColliderTrisElement* trisElem;
    ColliderTris* tris = (ColliderTris*)col;

    ClObj_ACClear(play, &tris->base);
    for (trisElem = tris->elements; trisElem < tris->elements + tris->count; trisElem++) {
        ClObjTrisElem_ACClear(play, trisElem);
    }
    return true;
}

/**
 * Resets the collider's OC collision flags.
 */
s32 ClObjTris_OCClear(PlayState* play, Collider* col) {
    ColliderTrisElement* trisElem;
    ColliderTris* tris = (ColliderTris*)col;

    ClObj_OCClear(play, &tris->base);
    for (trisElem = tris->elements; trisElem < tris->elements + tris->count; trisElem++) {
        ClObjTrisElem_OCClear(play, trisElem);
    }
    return true;
}

s32 ClObjSwrdAttr_ct(PlayState* play, ColliderQuadDim* dim) {
    static ColliderQuadDim default_attr = {
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
        { 0, 0, 0 },
        { 0, 0, 0 },
        1.0e38f,
    };

    *dim = default_attr;
    return true;
}

s32 ClObjSwrdAttr_dt(PlayState* play, ColliderQuadDim* dim) {
    return true;
}

s32 ClObjSwrdAttr_ATClear(PlayState* play, ColliderQuadDim* dim) {
    dim->acDistSq = 1.0e38f;
    return true;
}

void ClObjSwrdAttr_setup(ColliderQuadDim* dim) {
    dim->dcMid.x = (dim->quad[3].x + dim->quad[2].x) * 0.5f;
    dim->dcMid.y = (dim->quad[3].y + dim->quad[2].y) * 0.5f;
    dim->dcMid.z = (dim->quad[3].z + dim->quad[2].z) * 0.5f;
    dim->baMid.x = (dim->quad[1].x + dim->quad[0].x) * 0.5f;
    dim->baMid.y = (dim->quad[1].y + dim->quad[0].y) * 0.5f;
    dim->baMid.z = (dim->quad[1].z + dim->quad[0].z) * 0.5f;
}

s32 ClObjSwrdAttr_set(PlayState* play, ColliderQuadDim* dest, ColliderQuadDimInit* src) {
    dest->quad[0] = src->quad[0];
    dest->quad[1] = src->quad[1];
    dest->quad[2] = src->quad[2];
    dest->quad[3] = src->quad[3];
    ClObjSwrdAttr_setup(dest);
    return true;
}

/**
 * Initializes a ColliderQuad to default values.
 */
s32 ClObjSwrd_ct(PlayState* play, ColliderQuad* quad) {
    ClObj_ct(play, &quad->base);
    ClObjElem_ct(play, &quad->elem);
    ClObjSwrdAttr_ct(play, &quad->dim);
    return true;
}

/**
 * Destroys a ColliderQuad.
 */
s32 ClObjSwrd_dt(PlayState* play, ColliderQuad* quad) {
    ClObj_dt(play, &quad->base);
    ClObjElem_dt(play, &quad->elem);
    ClObjSwrdAttr_dt(play, &quad->dim);
    return true;
}

/**
 * Sets up the ColliderQuad using the values in src. Uses the default OC2_TYPE_1
 */
s32 ClObjSwrd_set3(PlayState* play, ColliderQuad* dest, Actor* actor, ColliderQuadInitType1* src) {
    ClObj_set3(play, &dest->base, actor, &src->base);
    ClObjElem_set(play, &dest->elem, &src->elem);
    ClObjSwrdAttr_set(play, &dest->dim, &src->dim);
    return true;
}

/**
 * Sets up the ColliderQuad using the values in src.
 */
s32 ClObjSwrd_set5(PlayState* play, ColliderQuad* dest, Actor* actor, ColliderQuadInit* src) {
    ClObj_set4(play, &dest->base, actor, &src->base);
    ClObjElem_set(play, &dest->elem, &src->elem);
    ClObjSwrdAttr_set(play, &dest->dim, &src->dim);
    return true;
}

/**
 * Resets the collider's AT collision flags.
 */
s32 ClObjSwrd_ATClear(PlayState* play, Collider* col) {
    ColliderQuad* quad = (ColliderQuad*)col;

    ClObj_ATClear(play, &quad->base);
    ClObjElem_ATClear(play, &quad->elem);
    ClObjSwrdAttr_ATClear(play, &quad->dim);
    return true;
}

/**
 * Resets the collider's AC collision flags.
 */
s32 ClObjSwrd_ACClear(PlayState* play, Collider* col) {
    ColliderQuad* quad = (ColliderQuad*)col;

    ClObj_ACClear(play, &quad->base);
    ClObjElem_ACClear(play, &quad->elem);
    return true;
}

/**
 * Resets the collider's OC collision flags.
 */
s32 ClObjSwrd_OCClear(PlayState* play, Collider* col) {
    ColliderQuad* quad = (ColliderQuad*)col;

    ClObj_OCClear(play, &quad->base);
    ClObjElem_OCClear(play, &quad->elem);
    return true;
}

/**
 * For quad colliders with AT_NEAREST, resets the previous AC collider it hit if the current element is closer,
 * otherwise returns false. Used on player AT colliders to prevent multiple collisions from registering.
 */
s32 ClObjSwrd_lengthCheck(PlayState* play, ColliderQuad* quad, Vec3f* cross) {
    f32 acDistSq;
    Vec3f dcMid;

    if (!(quad->elem.atElemFlags & ATELEM_NEAREST)) {
        return true;
    }
    xyz_t_move_s_xyz(&dcMid, &quad->dim.dcMid);
    acDistSq = Math3DLengthSquare(&dcMid, cross);
    if (acDistSq < quad->dim.acDistSq) {
        quad->dim.acDistSq = acDistSq;
        if (quad->elem.atHit != NULL) {
            ClObj_ACClear(play, quad->elem.atHit);
        }
        if (quad->elem.atHitElem != NULL) {
            ClObjElem_ACClear(play, quad->elem.atHitElem);
        }
        return true;
    }
    return false;
}

/**
 * Initializes an OcLine to default values
 * OcLines are entirely unused.
 */
s32 ClOCLine_ct(PlayState* play, OcLine* line) {
    Vec3f clobj_default = { 0.0f, 0.0f, 0.0f };

    xyz_t_move(&line->line.a, &clobj_default);
    xyz_t_move(&line->line.b, &clobj_default);
    return true;
}

/**
 * Destroys an OcLine
 * OcLines are entirely unused.
 */
s32 ClOCLine_dt(PlayState* play, OcLine* line) {
    return true;
}

/**
 * Sets up an OcLine with endpoints a and b.
 * OcLines are entirely unused.
 */
s32 ClOCLine_positionSet(PlayState* play, OcLine* ocLine, Vec3f* a, Vec3f* b) {
    xyz_t_move(&ocLine->line.a, a);
    xyz_t_move(&ocLine->line.b, b);
    return true;
}

/**
 * Sets up an OcLine using the values in src.
 * OcLines are entirely unused.
 */
s32 ClOCLine_set(PlayState* play, OcLine* dest, OcLine* src) {
    dest->ocFlags = src->ocFlags;
    ClOCLine_positionSet(play, dest, &src->line.a, &src->line.b);
    return true;
}

/**
 * Resets the OcLine's collision flags.
 * OcLines are entirely unused.
 */
s32 ClOCLine_Clear(PlayState* play, OcLine* line) {
    line->ocFlags &= ~OCLINE_HIT;
    return true;
}

/**
 * Initializes CollisionCheckContext. Clears all collider arrays, disables SAC, and sets flags for drawing colliders.
 */
void CollisionCheck_ct(PlayState* play, CollisionCheckContext* colChkCtx) {
    colChkCtx->sacFlags = 0;
    CollisionCheck_clear(play, colChkCtx);

#if DEBUG_FEATURES
    AREG(21) = true;
    AREG(22) = true;
    AREG(23) = true;
#endif
}

void CollisionCheck_dt(PlayState* play, CollisionCheckContext* colChkCtx) {
}

/**
 * Clears all collider lists in CollisionCheckContext when not in SAC mode.
 */
void CollisionCheck_clear(PlayState* play, CollisionCheckContext* colChkCtx) {
    Collider** colP;
    OcLine** lineP;

    if (!(colChkCtx->sacFlags & SAC_ENABLE)) {
        colChkCtx->colATCount = 0;
        colChkCtx->colACCount = 0;
        colChkCtx->colOCCount = 0;
        colChkCtx->colLineCount = 0;
        for (colP = colChkCtx->colAT; colP < colChkCtx->colAT + COLLISION_CHECK_AT_MAX; colP++) {
            *colP = NULL;
        }

        for (colP = colChkCtx->colAC; colP < colChkCtx->colAC + COLLISION_CHECK_AC_MAX; colP++) {
            *colP = NULL;
        }

        for (colP = colChkCtx->colOC; colP < colChkCtx->colOC + COLLISION_CHECK_OC_MAX; colP++) {
            *colP = NULL;
        }

        for (lineP = colChkCtx->colLine; lineP < colChkCtx->colLine + COLLISION_CHECK_OC_LINE_MAX; lineP++) {
            *lineP = NULL;
        }
    }
}

/**
 * Enables SAC, an alternate collision check mode that allows direct management of collider lists. Unused.
 */
void CollisionCheck_setStatusStopAddClear(PlayState* play, CollisionCheckContext* colChkCtx) {
    colChkCtx->sacFlags |= SAC_ENABLE;
}

/**
 * Disables SAC, an alternate collision check mode that allows direct management of collider lists. Unused.
 */
void CollisionCheck_clearStatusStopAddClear(PlayState* play, CollisionCheckContext* colChkCtx) {
    colChkCtx->sacFlags &= ~SAC_ENABLE;
}

#if DEBUG_FEATURES
/**
 * Draws a collider of any shape.
 * Math3D_DrawSphere and Math3D_DrawCylinder are noops, so JntSph and Cylinder are not drawn.
 */
void Collider_Draw(PlayState* play, Collider* col) {
    ColliderJntSph* jntSph;
    ColliderCylinder* cyl;
    ColliderTris* tris;
    ColliderQuad* quad;
    s32 i;

    if (col == NULL) {
        return;
    }
    switch (col->shape) {
        case COLSHAPE_JNTSPH:
            jntSph = (ColliderJntSph*)col;
            for (i = 0; i < jntSph->count; i++) {
                Math3D_DrawSphere(play, &jntSph->elements[i].dim.worldSphere);
            }
            break;

        case COLSHAPE_CYLINDER:
            cyl = (ColliderCylinder*)col;
            Math3D_DrawCylinder(play, &cyl->dim);
            break;

        case COLSHAPE_TRIS:
            tris = (ColliderTris*)col;
            for (i = 0; i < tris->count; i++) {
                Collider_DrawRedPoly(play->state.gfxCtx, &tris->elements[i].dim.vtx[0], &tris->elements[i].dim.vtx[1],
                                     &tris->elements[i].dim.vtx[2]);
            }
            break;

        case COLSHAPE_QUAD:
            quad = (ColliderQuad*)col;
            Collider_DrawRedPoly(play->state.gfxCtx, &quad->dim.quad[2], &quad->dim.quad[3], &quad->dim.quad[1]);
            Collider_DrawRedPoly(play->state.gfxCtx, &quad->dim.quad[1], &quad->dim.quad[0], &quad->dim.quad[2]);
            break;
    }
}

/**
 * Draws collision if AREG(15) and other AREGs are set. AREG(21) draws AT colliders, AREG(22) draws AC colliders,
 * AREG(23) draws OC colliders, AREG(24) draws dynapolys, and AREG(25) draws bg polys
 */
void CollisionCheck_DrawCollision(PlayState* play, CollisionCheckContext* colChkCtx) {
    Collider* col;
    s32 i;

    if (AREG(15)) {
        if (AREG(21)) {
            for (i = 0; i < colChkCtx->colATCount; i++) {
                Collider_Draw(play, colChkCtx->colAT[i]);
            }
        }
        if (AREG(22)) {
            for (i = 0; i < colChkCtx->colACCount; i++) {
                Collider_Draw(play, colChkCtx->colAC[i]);
            }
        }
        if (AREG(23)) {
            for (i = 0; i < colChkCtx->colOCCount; i++) {
                col = colChkCtx->colOC[i];
                if (col->ocFlags1 & OC1_ON) {
                    Collider_Draw(play, col);
                }
            }
        }
        if (AREG(24)) {
            BgCheck_DrawDynaCollision(play, &play->colCtx);
        }
        if (AREG(25)) {
            BgCheck_DrawStaticCollision(play, &play->colCtx);
        }
    }
}
#endif

static ColChkResetFunc ATClearFunctionTable[] = {
    ClObjJntSph_ATClear,
    ClObjPipe_ATClear,
    ClObjTris_ATClear,
    ClObjSwrd_ATClear,
};

/**
 * Sets collider as an AT (attack) for the current frame, which will be checked against ACs (attack colliders)
 * The last argument takes a Collider, so pass collider.base rather than the raw collider.
 */
s32 CollisionCheck_setAT(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider) {
    s32 index;

    if (_Game_play_isPause(play) == true) {
        return -1;
    }
    ASSERT(collider->shape < COLSHAPE_MAX, "pcl_obj->data_type <= CL_DATA_LBL_SWRD", "../z_collision_check.c", 2997);
    ATClearFunctionTable[collider->shape](play, collider);
    if (collider->actor != NULL && collider->actor->update == NULL) {
        return -1;
    }
    if (colChkCtx->colATCount >= COLLISION_CHECK_AT_MAX) {
        PRINTF(T("CollisionCheck_setAT():インデックスがオーバーして追加不能\n",
                 "CollisionCheck_setAT(): Index exceeded and cannot add more\n"));
        return -1;
    }
    if (colChkCtx->sacFlags & SAC_ENABLE) {
        return -1;
    }
    index = colChkCtx->colATCount;
    colChkCtx->colAT[colChkCtx->colATCount++] = collider;
    return index;
}

/**
 * Unused. Sets collider as an AT (attack) for the current frame, which will be checked against ACs (attack colliders).
 * If CollisionCheck_SAC is enabled, the collider will be inserted into the list at the specified index, otherwise it
 * will be inserted into the next slot
 */
s32 CollisionCheck_setAT_SAC(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider, s32 index) {
    ASSERT(collider->shape < COLSHAPE_MAX, "pcl_obj->data_type <= CL_DATA_LBL_SWRD", "../z_collision_check.c", 3037);
    if (_Game_play_isPause(play) == true) {
        return -1;
    }
    ATClearFunctionTable[collider->shape](play, collider);
    if (collider->actor != NULL && collider->actor->update == NULL) {
        return -1;
    }
    if (colChkCtx->sacFlags & SAC_ENABLE) {
        if (!(index < colChkCtx->colATCount)) {
            PRINTF(T("CollisionCheck_setAT_SAC():全データ数より大きいところに登録しようとしている。\n",
                     "CollisionCheck_setAT_SAC(): You are trying to register a location that is larger than the total "
                     "number of data.\n"));
            return -1;
        }
        colChkCtx->colAT[index] = collider;
    } else {
        if (!(colChkCtx->colATCount < COLLISION_CHECK_AT_MAX)) {
            PRINTF(T("CollisionCheck_setAT():インデックスがオーバーして追加不能\n",
                     "CollisionCheck_setAT(): Index exceeded and cannot add more\n"));
            return -1;
        }
        index = colChkCtx->colATCount;
        colChkCtx->colAT[colChkCtx->colATCount++] = collider;
    }
    return index;
}

static ColChkResetFunc ACClearFunctionTable[] = {
    ClObjJntSph_ACClear,
    ClObjPipe_ACClear,
    ClObjTris_ACClear,
    ClObjSwrd_ACClear,
};

/**
 * Sets collider as an AC (attack collider) for the current frame, allowing it to detect ATs (attacks)
 * The last argument takes a Collider, so pass collider.base rather than the raw collider.
 */
s32 CollisionCheck_setAC(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider) {
    s32 index;

    if (_Game_play_isPause(play) == true) {
        return -1;
    }
    ASSERT(collider->shape < COLSHAPE_MAX, "pcl_obj->data_type <= CL_DATA_LBL_SWRD", "../z_collision_check.c", 3114);
    ACClearFunctionTable[collider->shape](play, collider);
    if (collider->actor != NULL && collider->actor->update == NULL) {
        return -1;
    }
    if (colChkCtx->colACCount >= COLLISION_CHECK_AC_MAX) {
        PRINTF(T("CollisionCheck_setAC():インデックスがオーバして追加不能\n",
                 "CollisionCheck_setAC(): Index exceeded and cannot add more\n"));
        return -1;
    }
    if (colChkCtx->sacFlags & SAC_ENABLE) {
        return -1;
    }
    index = colChkCtx->colACCount;
    colChkCtx->colAC[colChkCtx->colACCount++] = collider;
    return index;
}

/**
 * Unused. Sets collider as an AC (attack collider) for the current frame, allowing it to detect ATs (attacks).
 * If CollisionCheck_SAC is enabled, the collider will be inserted into the list at the specified index, otherwise it
 * will be inserted into the next slot
 */
s32 CollisionCheck_setAC_SAC(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider, s32 index) {
    ASSERT(collider->shape < COLSHAPE_MAX, "pcl_obj->data_type <= CL_DATA_LBL_SWRD", "../z_collision_check.c", 3153);
    if (_Game_play_isPause(play) == true) {
        return -1;
    }
    ACClearFunctionTable[collider->shape](play, collider);
    if (collider->actor != NULL && collider->actor->update == NULL) {
        return -1;
    }
    if (colChkCtx->sacFlags & SAC_ENABLE) {
        if (!(index < colChkCtx->colACCount)) {
            PRINTF(T("CollisionCheck_setAC_SAC():全データ数より大きいところに登録しようとしている。\n",
                     "CollisionCheck_setAC_SAC(): You are trying to register a location that is larger than the total "
                     "number of data.\n"));
            return -1;
        }
        colChkCtx->colAC[index] = collider;
    } else {
        if (!(colChkCtx->colACCount < COLLISION_CHECK_AC_MAX)) {
            PRINTF(T("CollisionCheck_setAC():インデックスがオーバして追加不能\n",
                     "CollisionCheck_setAC(): Index exceeded and cannot add more\n"));
            return -1;
        }
        index = colChkCtx->colACCount;
        colChkCtx->colAC[colChkCtx->colACCount++] = collider;
    }
    return index;
}

static ColChkResetFunc OCClearFunctionTable[] = {
    ClObjJntSph_OCClear,
    ClObjPipe_OCClear,
    ClObjTris_OCClear,
    ClObjSwrd_OCClear,
};

/**
 * Sets collider as an OC (object collider) for the current frame, allowing it to detect other OCs
 * The last argument takes a Collider, so pass collider.base rather than the raw collider.
 */
s32 CollisionCheck_setOC(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider) {
    s32 index;

    if (_Game_play_isPause(play) == true) {
        return -1;
    }

    ASSERT(collider->shape < COLSHAPE_MAX, "pcl_obj->data_type <= CL_DATA_LBL_SWRD", "../z_collision_check.c", 3229);

    OCClearFunctionTable[collider->shape](play, collider);
    if (collider->actor != NULL && collider->actor->update == NULL) {
        return -1;
    }
    if (colChkCtx->colOCCount >= COLLISION_CHECK_OC_MAX) {
        PRINTF(T("CollisionCheck_setOC():インデックスがオーバして追加不能\n",
                 "CollisionCheck_setOC(): Index exceeded and cannot add more\n"));
        return -1;
    }
    if (colChkCtx->sacFlags & SAC_ENABLE) {
        return -1;
    }
    index = colChkCtx->colOCCount;
    colChkCtx->colOC[colChkCtx->colOCCount++] = collider;
    return index;
}

/**
 * Unused. Sets collider as an OC (object collider) for the current frame, allowing it to detect other OCs
 * If CollisionCheck_SAC is enabled, the collider will be inserted into the list at the specified index, otherwise it
 * will be inserted into the next slot
 */
s32 CollisionCheck_setOC_SAC(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider, s32 index) {
    if (_Game_play_isPause(play) == true) {
        return -1;
    }
    ASSERT(collider->shape < COLSHAPE_MAX, "pcl_obj->data_type <= CL_DATA_LBL_SWRD", "../z_collision_check.c", 3274);
    OCClearFunctionTable[collider->shape](play, collider);
    if (collider->actor != NULL && collider->actor->update == NULL) {
        return -1;
    }
    if (colChkCtx->sacFlags & SAC_ENABLE) {
        if (!(index < colChkCtx->colOCCount)) {
            PRINTF(T("CollisionCheck_setOC_SAC():全データ数より大きいところに登録しようとしている。\n",
                     "CollisionCheck_setOC_SAC(): You are trying to register a location that is larger than the total "
                     "number of data.\n"));
            return -1;
        }
        //! @bug Should be colOC
        colChkCtx->colAT[index] = collider;
    } else {
        if (!(colChkCtx->colOCCount < COLLISION_CHECK_OC_MAX)) {
            PRINTF(T("CollisionCheck_setOC():インデックスがオーバして追加不能\n",
                     "CollisionCheck_setOC(): Index exceeded and cannot add more\n"));
            return -1;
        }
        index = colChkCtx->colOCCount;
        colChkCtx->colOC[colChkCtx->colOCCount++] = collider;
    }
    return index;
}

/**
 * Sets a line as an OC collider for this frame.
 * OC lines are entirely unused, and do not even have collision check functions.
 */
s32 CollisionCheck_setOCLine(PlayState* play, CollisionCheckContext* colChkCtx, OcLine* collider) {
    s32 index;

    if (_Game_play_isPause(play) == true) {
        return -1;
    }
    ClOCLine_Clear(play, collider);
    if (!(colChkCtx->colLineCount < COLLISION_CHECK_OC_LINE_MAX)) {
        PRINTF(T("CollisionCheck_setOCLine():インデックスがオーバして追加不能\n",
                 "CollisionCheck_setOCLine(): Index exceeded and cannot add more\n"));
        return -1;
    }
    index = colChkCtx->colLineCount;
    colChkCtx->colLine[colChkCtx->colLineCount++] = collider;
    return index;
}

s32 CollisionCheck_CheckNoAT(ColliderElement* elem) {
    if (!(elem->atElemFlags & ATELEM_ON)) {
        return true;
    }
    return false;
}

s32 CollisionCheck_CheckNoAC(ColliderElement* elem) {
    if (!(elem->acElemFlags & ACELEM_ON)) {
        return true;
    }
    return false;
}

/**
 * If the AT element has no dmgFlags in common with the AC element, no collision happens.
 */
s32 CollisionCheck_CheckNoATAC(ColliderElement* atElem, ColliderElement* acElem) {
    if (!(atElem->atDmgInfo.dmgFlags & acElem->acDmgInfo.dmgFlags)) {
        return true;
    }
    return false;
}

/**
 * Spawns no blood drops.
 * Used by collider types HIT1, HIT3, HIT5, METAL, NONE, WOOD, HARD, and TREE
 */
void CollisionCheck_setNoneBlood(PlayState* play, Collider* collider, Vec3f* v) {
}

/**
 * Spawns blue blood drops.
 * Used by collider types HIT0 and HIT8.
 */
void CollisionCheck_setBlueBlood(PlayState* play, Collider* collider, Vec3f* v) {
    static EffectSparkInit spark;
    s32 effectIndex;

    spark.position.x = v->x;
    spark.position.y = v->y;
    spark.position.z = v->z;
    spark.uDiv = 5;
    spark.vDiv = 5;
    spark.colorStart[0].r = 10;
    spark.colorStart[0].g = 10;
    spark.colorStart[0].b = 200;
    spark.colorStart[0].a = 255;
    spark.colorStart[1].r = 0;
    spark.colorStart[1].g = 0;
    spark.colorStart[1].b = 128;
    spark.colorStart[1].a = 255;
    spark.colorStart[2].r = 0;
    spark.colorStart[2].g = 0;
    spark.colorStart[2].b = 128;
    spark.colorStart[2].a = 255;
    spark.colorStart[3].r = 0;
    spark.colorStart[3].g = 0;
    spark.colorStart[3].b = 128;
    spark.colorStart[3].a = 255;
    spark.colorEnd[0].r = 0;
    spark.colorEnd[0].g = 0;
    spark.colorEnd[0].b = 32;
    spark.colorEnd[0].a = 0;
    spark.colorEnd[1].r = 0;
    spark.colorEnd[1].g = 0;
    spark.colorEnd[1].b = 32;
    spark.colorEnd[1].a = 0;
    spark.colorEnd[2].r = 0;
    spark.colorEnd[2].g = 0;
    spark.colorEnd[2].b = 64;
    spark.colorEnd[2].a = 0;
    spark.colorEnd[3].r = 0;
    spark.colorEnd[3].g = 0;
    spark.colorEnd[3].b = 64;
    spark.colorEnd[3].a = 0;
    spark.timer = 0;
    spark.duration = 16;
    spark.speed = 8.0f;
    spark.gravity = -1.0f;

    EffectAdd(play, &effectIndex, EFFECT_SPARK, 0, 1, &spark);
}

/**
 * Spawns green blood drops.
 * Used by collider types HIT2 and HIT6. No actor has type HIT2.
 */
void CollisionCheck_setGreenBlood(PlayState* play, Collider* collider, Vec3f* v) {
    static EffectSparkInit spark;
    s32 effectIndex;

    spark.position.x = v->x;
    spark.position.y = v->y;
    spark.position.z = v->z;
    spark.uDiv = 5;
    spark.vDiv = 5;
    spark.colorStart[0].r = 10;
    spark.colorStart[0].g = 200;
    spark.colorStart[0].b = 10;
    spark.colorStart[0].a = 255;
    spark.colorStart[1].r = 0;
    spark.colorStart[1].g = 128;
    spark.colorStart[1].b = 0;
    spark.colorStart[1].a = 255;
    spark.colorStart[2].r = 0;
    spark.colorStart[2].g = 128;
    spark.colorStart[2].b = 0;
    spark.colorStart[2].a = 255;
    spark.colorStart[3].r = 0;
    spark.colorStart[3].g = 128;
    spark.colorStart[3].b = 0;
    spark.colorStart[3].a = 255;
    spark.colorEnd[0].r = 0;
    spark.colorEnd[0].g = 32;
    spark.colorEnd[0].b = 0;
    spark.colorEnd[0].a = 0;
    spark.colorEnd[1].r = 0;
    spark.colorEnd[1].g = 32;
    spark.colorEnd[1].b = 0;
    spark.colorEnd[1].a = 0;
    spark.colorEnd[2].r = 0;
    spark.colorEnd[2].g = 64;
    spark.colorEnd[2].b = 0;
    spark.colorEnd[2].a = 0;
    spark.colorEnd[3].r = 0;
    spark.colorEnd[3].g = 64;
    spark.colorEnd[3].b = 0;
    spark.colorEnd[3].a = 0;
    spark.timer = 0;
    spark.duration = 16;
    spark.speed = 8.0f;
    spark.gravity = -1.0f;

    EffectAdd(play, &effectIndex, EFFECT_SPARK, 0, 1, &spark);
}

/**
 * Spawns a burst of water.
 * Used by collider type HIT4, which no actor has.
 */
void CollisionCheck_setWhiteBlood(PlayState* play, Collider* collider, Vec3f* pos) {
    DamageEffectSibukiSet(play, pos);
    CollisionCheckSetWhiteBlood(play, pos);
}

/**
 * Spawns red blood drops.
 * Used by collider type HIT7, which no actor has.
 */
void CollisionCheck_setBreakEffect(PlayState* play, Collider* collider, Vec3f* v) {
    CollisionCheckSetSpark(play, v);
}

/**
 * Spawns red blood drops.
 * Unused.
 */
void CollisionCheck_setRedBlood(PlayState* play, Collider* collider, Vec3f* v) {
    CollisionCheckSetSpark(play, v);
}

/**
 * Plays sound effects and displays hitmarks for solid-type AC colliders (METAL, WOOD, HARD, and TREE)
 */
void CollisionCheck_ArmorHitMarkProc(PlayState* play, ColliderElement* elem, Collider* collider, Vec3f* cross) {
    s32 flags = elem->atElemFlags & ATELEM_SFX_MASK;

    if (flags == ATELEM_SFX_NORMAL && collider->colMaterial != COL_MATERIAL_METAL) {
        Effect_SS_HitMark_ct(play, EFFECT_HITMARK_WHITE, cross);
        if (collider->actor == NULL) {
            Nai_FxFlagEntry(NA_SE_IT_SHIELD_BOUND, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        } else {
            Nai_FxFlagEntry(NA_SE_IT_SHIELD_BOUND, &collider->actor->projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    } else if (flags == ATELEM_SFX_NORMAL) { // collider->colMaterial == COL_MATERIAL_METAL
        Effect_SS_HitMark_ct(play, EFFECT_HITMARK_METAL, cross);
        if (collider->actor == NULL) {
            CollisionCheckSetSparkFlashBlue(play, cross);
        } else {
            CollisionCheckSetSparkFlashBlue_SeCamPos(play, cross, &collider->actor->projectedPos);
        }
    } else if (flags == ATELEM_SFX_HARD) {
        Effect_SS_HitMark_ct(play, EFFECT_HITMARK_WHITE, cross);
        if (collider->actor == NULL) {
            Nai_FxFlagEntry(NA_SE_IT_SHIELD_BOUND, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        } else {
            Nai_FxFlagEntry(NA_SE_IT_SHIELD_BOUND, &collider->actor->projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    } else if (flags == ATELEM_SFX_WOOD) {
        Effect_SS_HitMark_ct(play, EFFECT_HITMARK_DUST, cross);
        if (collider->actor == NULL) {
            Nai_FxFlagEntry(NA_SE_IT_REFLECTION_WOOD, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        } else {
            Nai_FxFlagEntry(NA_SE_IT_REFLECTION_WOOD, &collider->actor->projectedPos, 4,
                                 &_dummy_one, &_dummy_one, &_dummy_zero_s8);
        }
    }
}

/**
 * Plays a hit sound effect for AT colliders attached to Player based on the AC element's elemMaterial.
 */
s32 CollisionCheck_setLinkHitSound(Collider* atCol, ColliderElement* acElem) {
    if (atCol->actor != NULL && atCol->actor->category == ACTORCAT_PLAYER) {
        if (acElem->elemMaterial == ELEM_MATERIAL_UNK0) {
            Nai_FxFlagEntry(NA_SE_IT_SWORD_STRIKE, &atCol->actor->projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        } else if (acElem->elemMaterial == ELEM_MATERIAL_UNK1) {
            Nai_FxFlagEntry(NA_SE_IT_SWORD_STRIKE_HARD, &atCol->actor->projectedPos, 4,
                                 &_dummy_one, &_dummy_one, &_dummy_zero_s8);
        } else if (acElem->elemMaterial == ELEM_MATERIAL_UNK2) {
            Nai_FxFlagEntry(NA_SE_NONE, &atCol->actor->projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        } else if (acElem->elemMaterial == ELEM_MATERIAL_UNK3) {
            Nai_FxFlagEntry(NA_SE_NONE, &atCol->actor->projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    }
    return true;
}

typedef struct HitInfo {
    /* 0x0 */ u8 blood;
    /* 0x1 */ u8 effect;
} HitInfo; // size = 0x2

typedef enum ColChkBloodType {
    /* 0 */ BLOOD_NONE,
    /* 1 */ BLOOD_BLUE,
    /* 2 */ BLOOD_GREEN,
    /* 3 */ BLOOD_WATER,
    /* 4 */ BLOOD_RED,
    /* 5 */ BLOOD_RED2
} ColChkBloodType;

typedef enum ColChkHitType {
    /* 0 */ HIT_WHITE,
    /* 1 */ HIT_DUST,
    /* 2 */ HIT_RED,
    /* 3 */ HIT_SOLID,
    /* 4 */ HIT_WOOD,
    /* 5 */ HIT_NONE
} ColChkHitType;

typedef void (*ColChkBloodFunc)(PlayState*, Collider*, Vec3f*);

static ColChkBloodFunc blood_disp_function[] = {
    CollisionCheck_setNoneBlood,        // BLOOD_NONE
    CollisionCheck_setBlueBlood,      // BLOOD_BLUE
    CollisionCheck_setGreenBlood,     // BLOOD_GREEN
    CollisionCheck_setWhiteBlood,     // BLOOD_WATER
    CollisionCheck_setBreakEffect,       // BLOOD_RED
    CollisionCheck_setRedBlood, // BLOOD_RED2
};

static HitInfo hit_mark_pattern[] = {
    { BLOOD_BLUE, HIT_WHITE },  // COL_MATERIAL_HIT0
    { BLOOD_NONE, HIT_DUST },   // COL_MATERIAL_HIT1
    { BLOOD_GREEN, HIT_DUST },  // COL_MATERIAL_HIT2
    { BLOOD_NONE, HIT_WHITE },  // COL_MATERIAL_HIT3
    { BLOOD_WATER, HIT_NONE },  // COL_MATERIAL_HIT4
    { BLOOD_NONE, HIT_RED },    // COL_MATERIAL_HIT5
    { BLOOD_GREEN, HIT_WHITE }, // COL_MATERIAL_HIT6
    { BLOOD_RED, HIT_WHITE },   // COL_MATERIAL_HIT7
    { BLOOD_BLUE, HIT_RED },    // COL_MATERIAL_HIT8
    { BLOOD_NONE, HIT_SOLID },  // COL_MATERIAL_METAL
    { BLOOD_NONE, HIT_NONE },   // COL_MATERIAL_NONE
    { BLOOD_NONE, HIT_SOLID },  // COL_MATERIAL_WOOD
    { BLOOD_NONE, HIT_SOLID },  // COL_MATERIAL_HARD
    { BLOOD_NONE, HIT_WOOD },   // COL_MATERIAL_TREE
};

/**
 * Handles hitmarks, blood, and sound effects for each AC collision, determined by the AC collider's colMaterial
 */
void CollisionCheck_setHitMark(PlayState* play, Collider* atCol, ColliderElement* atElem, Collider* acCol,
                               ColliderElement* acElem, Vec3f* cross) {
    if (acElem->acElemFlags & ACELEM_NO_HITMARK) {
        return;
    }
    if (!(atElem->atElemFlags & ATELEM_AT_HITMARK) && atElem->atElemFlags & ATELEM_DREW_HITMARK) {
        return;
    }
    if (acCol->actor != NULL) {
        blood_disp_function[hit_mark_pattern[acCol->colMaterial].blood](play, acCol, cross);
    }
    if (acCol->actor != NULL) {
        if (hit_mark_pattern[acCol->colMaterial].effect == HIT_SOLID) {
            CollisionCheck_ArmorHitMarkProc(play, atElem, acCol, cross);
        } else if (hit_mark_pattern[acCol->colMaterial].effect == HIT_WOOD) {
            if (atCol->actor == NULL) {
                CollisionCheckSetSparkFlashBlue_NoSE(play, cross);
                Nai_FxFlagEntry(NA_SE_IT_REFLECTION_WOOD, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            } else {
                CollisionCheckSetWoodParticle(play, cross, &atCol->actor->projectedPos);
            }
        } else if (hit_mark_pattern[acCol->colMaterial].effect != HIT_NONE) {
            Effect_SS_HitMark_ct(play, hit_mark_pattern[acCol->colMaterial].effect, cross);
            if (!(acElem->acElemFlags & ACELEM_NO_SWORD_SFX)) {
                CollisionCheck_setLinkHitSound(atCol, acElem);
            }
        }
    } else {
        Effect_SS_HitMark_ct(play, EFFECT_HITMARK_WHITE, cross);
        if (acCol->actor == NULL) {
            Nai_FxFlagEntry(NA_SE_IT_SHIELD_BOUND, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        } else {
            Nai_FxFlagEntry(NA_SE_IT_SHIELD_BOUND, &acCol->actor->projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    }
}

/**
 * Sets the flags to indicate an attack bounced off an AC_HARD collider.
 */
void CollisionCheck_checkShield(Collider* atCol, Collider* acCol) {
    atCol->atFlags |= AT_BOUNCED;
    acCol->acFlags |= AC_BOUNCED;
}

/**
 * Performs the AC collision between the AT element and AC element that collided.
 */
s32 CollisionCheck_setATAC_HitInfo(PlayState* play, Collider* atCol, ColliderElement* atElem, Vec3f* atPos, Collider* acCol,
                             ColliderElement* acElem, Vec3f* acPos, Vec3f* cross) {
    if (acCol->acFlags & AC_HARD && atCol->actor != NULL && acCol->actor != NULL) {
        CollisionCheck_checkShield(atCol, acCol);
    }
    if (!(acElem->acElemFlags & ACELEM_NO_AT_INFO)) {
        atCol->atFlags |= AT_HIT;
        atCol->at = acCol->actor;
        atElem->atHit = acCol;
        atElem->atHitElem = acElem;
        atElem->atElemFlags |= ATELEM_HIT;
        if (atCol->actor != NULL) {
            atCol->actor->colChkInfo.atHitEffect = acElem->acDmgInfo.effect;
        }
    }
    acCol->acFlags |= AC_HIT;
    acCol->ac = atCol->actor;
    acElem->acHit = atCol;
    acElem->acHitElem = atElem;
    acElem->acElemFlags |= ACELEM_HIT;
    if (acCol->actor != NULL) {
        acCol->actor->colChkInfo.acHitEffect = atElem->atDmgInfo.effect;
    }
    acElem->acDmgInfo.hitPos.x = cross->x;
    acElem->acDmgInfo.hitPos.y = cross->y;
    acElem->acDmgInfo.hitPos.z = cross->z;
    if (!(atElem->atElemFlags & ATELEM_AT_HITMARK) && acCol->colMaterial != COL_MATERIAL_METAL &&
        acCol->colMaterial != COL_MATERIAL_WOOD && acCol->colMaterial != COL_MATERIAL_HARD) {
        acElem->acElemFlags |= ACELEM_DRAW_HITMARK;
    } else {
        CollisionCheck_setHitMark(play, atCol, atElem, acCol, acElem, cross);
        atElem->atElemFlags |= ATELEM_DREW_HITMARK;
    }
    return true;
}

void CollisionCheck_ATAC_JntSph_Vs_JntSph(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                       Collider* acCol) {
    ColliderJntSph* atJntSph = (ColliderJntSph*)atCol;
    ColliderJntSphElement* atJntSphElem;
    ColliderJntSph* acJntSph = (ColliderJntSph*)acCol;
    ColliderJntSphElement* acJntSphElem;
    f32 overlapSize;
    f32 centerDist;

    if (atJntSph->count > 0 && atJntSph->elements != NULL && acJntSph->count > 0 && acJntSph->elements != NULL) {
        for (atJntSphElem = atJntSph->elements; atJntSphElem < atJntSph->elements + atJntSph->count; atJntSphElem++) {
            if (CollisionCheck_CheckNoAT(&atJntSphElem->base) == true) {
                continue;
            }
            for (acJntSphElem = acJntSph->elements; acJntSphElem < acJntSph->elements + acJntSph->count;
                 acJntSphElem++) {
                if (CollisionCheck_CheckNoAC(&acJntSphElem->base) == true) {
                    continue;
                }
                if (CollisionCheck_CheckNoATAC(&atJntSphElem->base, &acJntSphElem->base) == true) {
                    continue;
                }
                if (Math3D_sphereCrossSphere_cl_cc(&atJntSphElem->dim.worldSphere, &acJntSphElem->dim.worldSphere,
                                                     &overlapSize, &centerDist) == true) {
                    f32 acToHit;
                    Vec3f cross;
                    Vec3f atPos;
                    Vec3f acPos;

                    atPos.x = atJntSphElem->dim.worldSphere.center.x;
                    atPos.y = atJntSphElem->dim.worldSphere.center.y;
                    atPos.z = atJntSphElem->dim.worldSphere.center.z;
                    acPos.x = acJntSphElem->dim.worldSphere.center.x;
                    acPos.y = acJntSphElem->dim.worldSphere.center.y;
                    acPos.z = acJntSphElem->dim.worldSphere.center.z;
                    if (!IS_ZERO(centerDist)) {
                        acToHit = acJntSphElem->dim.worldSphere.radius / centerDist;
                        cross.x = (((atPos.x - acPos.x) * acToHit) + acPos.x);
                        cross.y = (((atPos.y - acPos.y) * acToHit) + acPos.y);
                        cross.z = (((atPos.z - acPos.z) * acToHit) + acPos.z);
                    } else {
                        xyz_t_move(&cross, &atPos);
                    }
                    CollisionCheck_setATAC_HitInfo(play, &atJntSph->base, &atJntSphElem->base, &atPos, &acJntSph->base,
                                             &acJntSphElem->base, &acPos, &cross);
                    if (!(acJntSph->base.ocFlags2 & OC2_FIRST_ONLY)) {
                        return;
                    }
                }
            }
        }
    }
}

void CollisionCheck_ATAC_JntSph_Vs_Pipe(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                    Collider* acCol) {
    ColliderJntSph* atJntSph = (ColliderJntSph*)atCol;
    ColliderJntSphElement* atJntSphElem;
    ColliderCylinder* acCyl = (ColliderCylinder*)acCol;
    f32 overlapSize;
    f32 centerDist;

    if (atJntSph->count > 0 && atJntSph->elements != NULL && acCyl->dim.radius > 0 && acCyl->dim.height > 0) {
        if (CollisionCheck_CheckNoAC(&acCyl->elem) == true) {
            return;
        }
        for (atJntSphElem = atJntSph->elements; atJntSphElem < atJntSph->elements + atJntSph->count; atJntSphElem++) {
            if (CollisionCheck_CheckNoAT(&atJntSphElem->base) == true) {
                continue;
            }
            if (CollisionCheck_CheckNoATAC(&atJntSphElem->base, &acCyl->elem) == true) {
                continue;
            }
            if (Math3D_sphereVsPipe_cl_cc(&atJntSphElem->dim.worldSphere, &acCyl->dim, &overlapSize,
                                                 &centerDist)) {
                Vec3f cross;
                Vec3f atPos;
                Vec3f acPos;

                atPos.x = atJntSphElem->dim.worldSphere.center.x;
                atPos.y = atJntSphElem->dim.worldSphere.center.y;
                atPos.z = atJntSphElem->dim.worldSphere.center.z;
                acPos.x = acCyl->dim.pos.x;
                acPos.y = acCyl->dim.pos.y;
                acPos.z = acCyl->dim.pos.z;
                if (!IS_ZERO(centerDist)) {
                    f32 acToHit = acCyl->dim.radius / centerDist;

#if OOT_VERSION < PAL_1_0
                    cross.x = ((atPos.x - acPos.x) * acToHit) + acPos.x;
                    cross.y = ((atPos.y - acPos.y) * acToHit) + acPos.y;
                    cross.z = ((atPos.z - acPos.z) * acToHit) + acPos.z;
#else
                    if (acToHit <= 1.0f) {
                        cross.x = ((atPos.x - acPos.x) * acToHit) + acPos.x;
                        cross.y = ((atPos.y - acPos.y) * acToHit) + acPos.y;
                        cross.z = ((atPos.z - acPos.z) * acToHit) + acPos.z;
                    } else {
                        xyz_t_move(&cross, &atPos);
                    }
#endif
                } else {
                    xyz_t_move(&cross, &atPos);
                }
                CollisionCheck_setATAC_HitInfo(play, &atJntSph->base, &atJntSphElem->base, &atPos, &acCyl->base, &acCyl->elem,
                                         &acPos, &cross);
                return;
            }
        }
    }
}

void CollisionCheck_ATAC_Pipe_Vs_JntSph(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                    Collider* acCol) {
    ColliderCylinder* atCyl = (ColliderCylinder*)atCol;
    ColliderJntSph* acJntSph = (ColliderJntSph*)acCol;
    f32 overlapSize;
    f32 centerDist;
    ColliderJntSphElement* acJntSphElem;

    if (acJntSph->count > 0 && acJntSph->elements != NULL && atCyl->dim.radius > 0 && atCyl->dim.height > 0) {
        if (CollisionCheck_CheckNoAT(&atCyl->elem) == true) {
            return;
        }
        for (acJntSphElem = acJntSph->elements; acJntSphElem < acJntSph->elements + acJntSph->count; acJntSphElem++) {
            if (CollisionCheck_CheckNoAC(&acJntSphElem->base) == true) {
                continue;
            }
            if (CollisionCheck_CheckNoATAC(&atCyl->elem, &acJntSphElem->base) == true) {
                continue;
            }
            if (Math3D_sphereVsPipe_cl_cc(&acJntSphElem->dim.worldSphere, &atCyl->dim, &overlapSize,
                                                 &centerDist)) {
                Vec3f cross;
                Vec3f atPos;
                Vec3f acPos;
                f32 acToHit;

                atPos.x = atCyl->dim.pos.x;
                atPos.y = atCyl->dim.pos.y;
                atPos.z = atCyl->dim.pos.z;
                acPos.x = acJntSphElem->dim.worldSphere.center.x;
                acPos.y = acJntSphElem->dim.worldSphere.center.y;
                acPos.z = acJntSphElem->dim.worldSphere.center.z;
                if (!IS_ZERO(centerDist)) {
                    f32 acToHit = acJntSphElem->dim.worldSphere.radius / centerDist;

#if OOT_VERSION < PAL_1_0
                    cross.x = ((atPos.x - acPos.x) * acToHit) + acPos.x;
                    cross.y = ((atPos.y - acPos.y) * acToHit) + acPos.y;
                    cross.z = ((atPos.z - acPos.z) * acToHit) + acPos.z;
#else
                    if (acToHit <= 1.0f) {
                        cross.x = ((atPos.x - acPos.x) * acToHit) + acPos.x;
                        cross.y = ((atPos.y - acPos.y) * acToHit) + acPos.y;
                        cross.z = ((atPos.z - acPos.z) * acToHit) + acPos.z;
                    } else {
                        xyz_t_move(&cross, &atPos);
                    }
#endif
                } else {
                    xyz_t_move(&cross, &atPos);
                }
                CollisionCheck_setATAC_HitInfo(play, &atCyl->base, &atCyl->elem, &atPos, &acJntSph->base, &acJntSphElem->base,
                                         &acPos, &cross);
                if (!(acJntSph->base.ocFlags2 & OC2_FIRST_ONLY)) {
                    break;
                }
            }
        }
    }
}

void CollisionCheck_ATAC_Jntsph_Vs_Tris(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                     Collider* acCol) {
    ColliderJntSph* atJntSph = (ColliderJntSph*)atCol;
    ColliderJntSphElement* atJntSphElem;
    ColliderTris* ac_tris = (ColliderTris*)acCol;
    ColliderTrisElement* acTrisElem;
    Vec3f cross;

    if (atJntSph->count > 0 && atJntSph->elements != NULL && ac_tris->count > 0 && ac_tris->elements != NULL) {
        for (atJntSphElem = atJntSph->elements; atJntSphElem < atJntSph->elements + atJntSph->count; atJntSphElem++) {
            if (CollisionCheck_CheckNoAT(&atJntSphElem->base) == true) {
                continue;
            }
            for (acTrisElem = ac_tris->elements; acTrisElem < ac_tris->elements + ac_tris->count; acTrisElem++) {
                if (CollisionCheck_CheckNoAC(&acTrisElem->base) == true) {
                    continue;
                }
                if (CollisionCheck_CheckNoATAC(&atJntSphElem->base, &acTrisElem->base) == true) {
                    continue;
                }
                if (Math3D_sphereCrossTriangle3_cp(&atJntSphElem->dim.worldSphere, &acTrisElem->dim, &cross) == true) {
                    Vec3f atPos;
                    Vec3f acPos;

                    atPos.x = atJntSphElem->dim.worldSphere.center.x;
                    atPos.y = atJntSphElem->dim.worldSphere.center.y;
                    atPos.z = atJntSphElem->dim.worldSphere.center.z;
                    acPos.x =
                        (acTrisElem->dim.vtx[0].x + acTrisElem->dim.vtx[1].x + acTrisElem->dim.vtx[2].x) * (1.0f / 3);
                    acPos.y =
                        (acTrisElem->dim.vtx[0].y + acTrisElem->dim.vtx[1].y + acTrisElem->dim.vtx[2].y) * (1.0f / 3);
                    acPos.z =
                        (acTrisElem->dim.vtx[0].z + acTrisElem->dim.vtx[1].z + acTrisElem->dim.vtx[2].z) * (1.0f / 3);
                    CollisionCheck_setATAC_HitInfo(play, &atJntSph->base, &atJntSphElem->base, &atPos, &ac_tris->base,
                                             &acTrisElem->base, &acPos, &cross);
                    return;
                }
            }
        }
    }
}

void CollisionCheck_ATAC_Tris_Vs_Jntsph(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                     Collider* acCol) {
    ColliderTris* at_tris = (ColliderTris*)atCol;
    ColliderTrisElement* atTrisElem;
    ColliderJntSph* acJntSph = (ColliderJntSph*)acCol;
    ColliderJntSphElement* acJntSphElem;
    Vec3f cross;

    if (acJntSph->count > 0 && acJntSph->elements != NULL && at_tris->count > 0 && at_tris->elements != NULL) {
        for (acJntSphElem = acJntSph->elements; acJntSphElem < acJntSph->elements + acJntSph->count; acJntSphElem++) {
            if (CollisionCheck_CheckNoAC(&acJntSphElem->base) == true) {
                continue;
            }
            for (atTrisElem = at_tris->elements; atTrisElem < at_tris->elements + at_tris->count; atTrisElem++) {
                if (CollisionCheck_CheckNoAT(&atTrisElem->base) == true) {
                    continue;
                }
                if (CollisionCheck_CheckNoATAC(&atTrisElem->base, &acJntSphElem->base) == true) {
                    continue;
                }
                if (Math3D_sphereCrossTriangle3_cp(&acJntSphElem->dim.worldSphere, &atTrisElem->dim, &cross) == true) {
                    Vec3f atPos;
                    Vec3f acPos;

                    xyz_t_move_s_xyz(&acPos, &acJntSphElem->dim.worldSphere.center);
                    atPos.x =
                        (atTrisElem->dim.vtx[0].x + atTrisElem->dim.vtx[1].x + atTrisElem->dim.vtx[2].x) * (1.0f / 3);
                    atPos.y =
                        (atTrisElem->dim.vtx[0].y + atTrisElem->dim.vtx[1].y + atTrisElem->dim.vtx[2].y) * (1.0f / 3);
                    atPos.z =
                        (atTrisElem->dim.vtx[0].z + atTrisElem->dim.vtx[1].z + atTrisElem->dim.vtx[2].z) * (1.0f / 3);
                    CollisionCheck_setATAC_HitInfo(play, &at_tris->base, &atTrisElem->base, &atPos, &acJntSph->base,
                                             &acJntSphElem->base, &acPos, &cross);
                    if (!(acJntSph->base.ocFlags2 & OC2_FIRST_ONLY)) {
                        return;
                    }
                }
            }
        }
    }
}

void CollisionCheck_ATAC_Jntsph_Vs_Swrd(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                     Collider* acCol) {
    static TriNorm tris1;
    static TriNorm tris2;
    ColliderJntSph* atJntSph = (ColliderJntSph*)atCol;
    ColliderQuad* acQuad = (ColliderQuad*)acCol;
    Vec3f cross;
    ColliderJntSphElement* atJntSphElem;

    if (atJntSph->count > 0 && atJntSph->elements != NULL) {
        if (CollisionCheck_CheckNoAC(&acQuad->elem) == true) {
            return;
        }
        Math3D_triangle_ct(&tris1, &acQuad->dim.quad[2], &acQuad->dim.quad[3], &acQuad->dim.quad[1]);
        Math3D_triangle_ct(&tris2, &acQuad->dim.quad[1], &acQuad->dim.quad[0], &acQuad->dim.quad[2]);
        for (atJntSphElem = atJntSph->elements; atJntSphElem < atJntSph->elements + atJntSph->count; atJntSphElem++) {
            if (CollisionCheck_CheckNoAT(&atJntSphElem->base) == true) {
                continue;
            }
            if (CollisionCheck_CheckNoATAC(&atJntSphElem->base, &acQuad->elem) == true) {
                continue;
            }
            if (Math3D_sphereCrossTriangle3_cp(&atJntSphElem->dim.worldSphere, &tris1, &cross) == true ||
                Math3D_sphereCrossTriangle3_cp(&atJntSphElem->dim.worldSphere, &tris2, &cross) == true) {
                Vec3f atPos;
                Vec3f acPos;

                xyz_t_move_s_xyz(&atPos, &atJntSphElem->dim.worldSphere.center);

                acPos.x = (acQuad->dim.quad[0].x +
                           (acQuad->dim.quad[1].x + (acQuad->dim.quad[3].x + acQuad->dim.quad[2].x))) /
                          4.0f;
                acPos.y = (acQuad->dim.quad[0].y +
                           (acQuad->dim.quad[1].y + (acQuad->dim.quad[3].y + acQuad->dim.quad[2].y))) /
                          4.0f;
                acPos.z = (acQuad->dim.quad[0].z +
                           (acQuad->dim.quad[1].z + (acQuad->dim.quad[3].z + acQuad->dim.quad[2].z))) /
                          4.0f;

                CollisionCheck_setATAC_HitInfo(play, &atJntSph->base, &atJntSphElem->base, &atPos, &acQuad->base,
                                         &acQuad->elem, &acPos, &cross);
                return;
            }
        }
    }
}

void CollisionCheck_ATAC_Swrd_Vs_Jntsph(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                     Collider* acCol) {
    static TriNorm tris1;
    static TriNorm tris2;
    ColliderJntSph* acJntSph = (ColliderJntSph*)acCol;
    Vec3f cross;
    ColliderQuad* atQuad = (ColliderQuad*)atCol;
    ColliderJntSphElement* acJntSphElem;

    if (acJntSph->count > 0 && acJntSph->elements != NULL) {
        if (CollisionCheck_CheckNoAT(&atQuad->elem) == true) {
            return;
        }
        Math3D_triangle_ct(&tris1, &atQuad->dim.quad[2], &atQuad->dim.quad[3], &atQuad->dim.quad[1]);
        Math3D_triangle_ct(&tris2, &atQuad->dim.quad[2], &atQuad->dim.quad[1], &atQuad->dim.quad[0]);
        for (acJntSphElem = acJntSph->elements; acJntSphElem < acJntSph->elements + acJntSph->count; acJntSphElem++) {
            if (CollisionCheck_CheckNoAC(&acJntSphElem->base) == true) {
                continue;
            }
            if (CollisionCheck_CheckNoATAC(&atQuad->elem, &acJntSphElem->base) == true) {
                continue;
            }
            if (Math3D_sphereCrossTriangle3_cp(&acJntSphElem->dim.worldSphere, &tris1, &cross) == true ||
                Math3D_sphereCrossTriangle3_cp(&acJntSphElem->dim.worldSphere, &tris2, &cross) == true) {
                if (ClObjSwrd_lengthCheck(play, atQuad, &cross)) {
                    Vec3f atPos;
                    Vec3f acPos;

                    acPos.x = acJntSphElem->dim.worldSphere.center.x;
                    acPos.y = acJntSphElem->dim.worldSphere.center.y;
                    acPos.z = acJntSphElem->dim.worldSphere.center.z;

                    atPos.x = (atQuad->dim.quad[0].x +
                               (atQuad->dim.quad[1].x + (atQuad->dim.quad[3].x + atQuad->dim.quad[2].x))) /
                              4.0f;
                    atPos.y = (atQuad->dim.quad[0].y +
                               (atQuad->dim.quad[1].y + (atQuad->dim.quad[3].y + atQuad->dim.quad[2].y))) /
                              4.0f;
                    atPos.z = (atQuad->dim.quad[0].z +
                               (atQuad->dim.quad[1].z + (atQuad->dim.quad[3].z + atQuad->dim.quad[2].z))) /
                              4.0f;
                    CollisionCheck_setATAC_HitInfo(play, &atQuad->base, &atQuad->elem, &atPos, &acJntSph->base,
                                             &acJntSphElem->base, &acPos, &cross);
                    if (!(acJntSph->base.ocFlags2 & OC2_FIRST_ONLY)) {
                        return;
                    }
                }
            }
        }
    }
}

void CollisionCheck_ATAC_Pipe_Vs_Pipe(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol, Collider* acCol) {
    ColliderCylinder* atCyl = (ColliderCylinder*)atCol;
    ColliderCylinder* acCyl = (ColliderCylinder*)acCol;
    f32 overlapSize;
    f32 centerDist;
    Vec3f cross;

    if (atCyl->dim.radius > 0 && atCyl->dim.height > 0 && acCyl->dim.radius > 0 && acCyl->dim.height > 0) {
        if (CollisionCheck_CheckNoAC(&acCyl->elem) == true) {
            return;
        }
        if (CollisionCheck_CheckNoAT(&atCyl->elem) == true) {
            return;
        }
        if (CollisionCheck_CheckNoATAC(&atCyl->elem, &acCyl->elem) == true) {
            return;
        }
        if (Math3D_pipeVsPipe_cl_cc(&atCyl->dim, &acCyl->dim, &overlapSize, &centerDist) == true) {
            Vec3f atPos;
            Vec3f acPos;
            f32 acToHit;

            xyz_t_move_s_xyz(&atPos, &atCyl->dim.pos);
            xyz_t_move_s_xyz(&acPos, &acCyl->dim.pos);
            if (!IS_ZERO(centerDist)) {
                acToHit = acCyl->dim.radius / centerDist;
                cross.y = (f32)acCyl->dim.pos.y + acCyl->dim.yShift + acCyl->dim.height * 0.5f;
                cross.x = ((f32)atCyl->dim.pos.x - acCyl->dim.pos.x) * acToHit + acCyl->dim.pos.x;
                cross.z = ((f32)atCyl->dim.pos.z - acCyl->dim.pos.z) * acToHit + acCyl->dim.pos.z;
            } else {
                xyz_t_move_s_xyz(&cross, &acCyl->dim.pos);
            }
            CollisionCheck_setATAC_HitInfo(play, &atCyl->base, &atCyl->elem, &atPos, &acCyl->base, &acCyl->elem, &acPos,
                                     &cross);
        }
    }
}

void CollisionCheck_ATAC_Pipe_Vs_Tris(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol, Collider* acCol) {
    ColliderCylinder* atCyl = (ColliderCylinder*)atCol;
    ColliderTris* ac_tris = (ColliderTris*)acCol;
    ColliderTrisElement* acTrisElem;
    Vec3f cross;

    if (atCyl->dim.radius > 0 && atCyl->dim.height > 0 && ac_tris->count > 0 && ac_tris->elements != NULL) {
        if (CollisionCheck_CheckNoAT(&atCyl->elem) == true) {
            return;
        }
        for (acTrisElem = ac_tris->elements; acTrisElem < ac_tris->elements + ac_tris->count; acTrisElem++) {
            if (CollisionCheck_CheckNoAC(&acTrisElem->base) == true) {
                continue;
            }
            if (CollisionCheck_CheckNoATAC(&atCyl->elem, &acTrisElem->base) == true) {
                continue;
            }
            if (Math3D_pipeCrossTriangle_cp(&atCyl->dim, &acTrisElem->dim, &cross) == true) {
                Vec3f atPos;
                Vec3f acPos;

                xyz_t_move_s_xyz(&atPos, &atCyl->dim.pos);

                acPos.x = (acTrisElem->dim.vtx[0].x + acTrisElem->dim.vtx[1].x + acTrisElem->dim.vtx[2].x) * (1.0f / 3);
                acPos.y = (acTrisElem->dim.vtx[0].y + acTrisElem->dim.vtx[1].y + acTrisElem->dim.vtx[2].y) * (1.0f / 3);
                acPos.z = (acTrisElem->dim.vtx[0].z + acTrisElem->dim.vtx[1].z + acTrisElem->dim.vtx[2].z) * (1.0f / 3);
                CollisionCheck_setATAC_HitInfo(play, &atCyl->base, &atCyl->elem, &atPos, &ac_tris->base, &acTrisElem->base,
                                         &acPos, &cross);
                return;
            }
        }
    }
}

void CollisionCheck_ATAC_Tris_Vs_Pipe(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol, Collider* acCol) {
    static Vec3f cross;
    ColliderTris* at_tris = (ColliderTris*)atCol;
    ColliderTrisElement* atTrisElem;
    ColliderCylinder* acCyl = (ColliderCylinder*)acCol;

    if (acCyl->dim.radius > 0 && acCyl->dim.height > 0 && at_tris->count > 0 && at_tris->elements != NULL) {
        if (CollisionCheck_CheckNoAC(&acCyl->elem) == true) {
            return;
        }
        for (atTrisElem = at_tris->elements; atTrisElem < at_tris->elements + at_tris->count; atTrisElem++) {
            if (CollisionCheck_CheckNoAT(&atTrisElem->base) == true) {
                continue;
            }
            if (CollisionCheck_CheckNoATAC(&atTrisElem->base, &acCyl->elem) == true) {
                continue;
            }

            if (Math3D_pipeCrossTriangle_cp(&acCyl->dim, &atTrisElem->dim, &cross) == true) {
                Vec3f atPos;
                Vec3f acPos;

                atPos.x = (atTrisElem->dim.vtx[0].x + atTrisElem->dim.vtx[1].x + atTrisElem->dim.vtx[2].x) * (1.0f / 3);
                atPos.y = (atTrisElem->dim.vtx[0].y + atTrisElem->dim.vtx[1].y + atTrisElem->dim.vtx[2].y) * (1.0f / 3);
                atPos.z = (atTrisElem->dim.vtx[0].z + atTrisElem->dim.vtx[1].z + atTrisElem->dim.vtx[2].z) * (1.0f / 3);
                xyz_t_move_s_xyz(&acPos, &acCyl->dim.pos);
                CollisionCheck_setATAC_HitInfo(play, &at_tris->base, &atTrisElem->base, &atPos, &acCyl->base, &acCyl->elem,
                                         &acPos, &cross);
                return;
            }
        }
    }
}

#pragma increment_block_number "gc-eu:252 gc-eu-mq:252 gc-jp:252 gc-jp-ce:252 gc-jp-mq:252 gc-us:252 gc-us-mq:252" \
                               "ique-cn:252 ntsc-1.0:252 ntsc-1.1:252 ntsc-1.2:252 pal-1.0:252 pal-1.1:252 hiratsu3:252"

void CollisionCheck_ATAC_Pipe_Vs_Swrd(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol, Collider* acCol) {
    static TriNorm tris1;
    static TriNorm tris2;
    static Vec3f cross;
    ColliderCylinder* atCyl = (ColliderCylinder*)atCol;
    ColliderQuad* acQuad = (ColliderQuad*)acCol;

    if (atCyl->dim.height > 0 && atCyl->dim.radius > 0) {
        if (CollisionCheck_CheckNoAT(&atCyl->elem) == true ||
            CollisionCheck_CheckNoAC(&acQuad->elem) == true) {
            return;
        }
        if (CollisionCheck_CheckNoATAC(&atCyl->elem, &acQuad->elem) == true) {
            return;
        }
        Math3D_triangle_ct(&tris1, &acQuad->dim.quad[2], &acQuad->dim.quad[3], &acQuad->dim.quad[1]);
        Math3D_triangle_ct(&tris2, &acQuad->dim.quad[1], &acQuad->dim.quad[0], &acQuad->dim.quad[2]);
        if (Math3D_pipeCrossTriangle_cp(&atCyl->dim, &tris1, &cross) == true) {
            Vec3f atPos1;
            Vec3f acPos1;

            xyz_t_move_s_xyz(&atPos1, &atCyl->dim.pos);
            acPos1.x =
                (acQuad->dim.quad[0].x + (acQuad->dim.quad[1].x + (acQuad->dim.quad[3].x + acQuad->dim.quad[2].x))) /
                4.0f;
            acPos1.y =
                (acQuad->dim.quad[0].y + (acQuad->dim.quad[1].y + (acQuad->dim.quad[3].y + acQuad->dim.quad[2].y))) /
                4.0f;
            acPos1.z =
                (acQuad->dim.quad[0].z + (acQuad->dim.quad[1].z + (acQuad->dim.quad[3].z + acQuad->dim.quad[2].z))) /
                4.0f;
            CollisionCheck_setATAC_HitInfo(play, &atCyl->base, &atCyl->elem, &atPos1, &acQuad->base, &acQuad->elem, &acPos1,
                                     &cross);
        } else if (Math3D_pipeCrossTriangle_cp(&atCyl->dim, &tris2, &cross) == true) {
            Vec3f atPos2;
            Vec3f acPos2;

            xyz_t_move_s_xyz(&atPos2, &atCyl->dim.pos);
            acPos2.x =
                (acQuad->dim.quad[0].x + (acQuad->dim.quad[1].x + (acQuad->dim.quad[3].x + acQuad->dim.quad[2].x))) /
                4.0f;
            acPos2.y =
                (acQuad->dim.quad[0].y + (acQuad->dim.quad[1].y + (acQuad->dim.quad[3].y + acQuad->dim.quad[2].y))) /
                4.0f;
            acPos2.z =
                (acQuad->dim.quad[0].z + (acQuad->dim.quad[1].z + (acQuad->dim.quad[3].z + acQuad->dim.quad[2].z))) /
                4.0f;
            CollisionCheck_setATAC_HitInfo(play, &atCyl->base, &atCyl->elem, &atPos2, &acQuad->base, &acQuad->elem, &acPos2,
                                     &cross);
        }
    }
}

void CollisionCheck_ATAC_Swrd_Vs_Pipe(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol, Collider* acCol) {
    static TriNorm tris1;
    static TriNorm tris2;
    static Vec3f cross;
    ColliderQuad* atQuad = (ColliderQuad*)atCol;
    ColliderCylinder* acCyl = (ColliderCylinder*)acCol;

    if (acCyl->dim.height > 0 && acCyl->dim.radius > 0) {
        if (CollisionCheck_CheckNoAC(&acCyl->elem) == true ||
            CollisionCheck_CheckNoAT(&atQuad->elem) == true) {
            return;
        }
        if (CollisionCheck_CheckNoATAC(&atQuad->elem, &acCyl->elem) == true) {
            return;
        }
        Math3D_triangle_ct(&tris1, &atQuad->dim.quad[2], &atQuad->dim.quad[3], &atQuad->dim.quad[1]);
        Math3D_triangle_ct(&tris2, &atQuad->dim.quad[2], &atQuad->dim.quad[1], &atQuad->dim.quad[0]);
        if (Math3D_pipeCrossTriangle_cp(&acCyl->dim, &tris1, &cross) == true) {
            if (ClObjSwrd_lengthCheck(play, atQuad, &cross)) {
                Vec3f atPos1;
                Vec3f acPos1;

                atPos1.x = (atQuad->dim.quad[0].x +
                            (atQuad->dim.quad[1].x + (atQuad->dim.quad[3].x + atQuad->dim.quad[2].x))) /
                           4.0f;
                atPos1.y = (atQuad->dim.quad[0].y +
                            (atQuad->dim.quad[1].y + (atQuad->dim.quad[3].y + atQuad->dim.quad[2].y))) /
                           4.0f;
                atPos1.z = (atQuad->dim.quad[0].z +
                            (atQuad->dim.quad[1].z + (atQuad->dim.quad[3].z + atQuad->dim.quad[2].z))) /
                           4.0f;
                xyz_t_move_s_xyz(&acPos1, &acCyl->dim.pos);
                CollisionCheck_setATAC_HitInfo(play, &atQuad->base, &atQuad->elem, &atPos1, &acCyl->base, &acCyl->elem,
                                         &acPos1, &cross);
                return;
            }
        }
        if (Math3D_pipeCrossTriangle_cp(&acCyl->dim, &tris2, &cross) == true) {
            if (ClObjSwrd_lengthCheck(play, atQuad, &cross)) {
                Vec3f atPos2;
                Vec3f acPos2;

                atPos2.x = (atQuad->dim.quad[0].x +
                            (atQuad->dim.quad[1].x + (atQuad->dim.quad[3].x + atQuad->dim.quad[2].x))) /
                           4.0f;
                atPos2.y = (atQuad->dim.quad[0].y +
                            (atQuad->dim.quad[1].y + (atQuad->dim.quad[3].y + atQuad->dim.quad[2].y))) /
                           4.0f;
                atPos2.z = (atQuad->dim.quad[0].z +
                            (atQuad->dim.quad[1].z + (atQuad->dim.quad[3].z + atQuad->dim.quad[2].z))) /
                           4.0f;
                xyz_t_move_s_xyz(&acPos2, &acCyl->dim.pos);
                CollisionCheck_setATAC_HitInfo(play, &atQuad->base, &atQuad->elem, &atPos2, &acCyl->base, &acCyl->elem,
                                         &acPos2, &cross);
            }
        }
    }
}

#if DEBUG_FEATURES
static s8 sBssDummy3;
static s8 sBssDummy4;
#endif

void CollisionCheck_ATAC_Tris_Vs_Tris(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                   Collider* acCol) {
    static Vec3f cross;
    ColliderTris* at_tris = (ColliderTris*)atCol;
    ColliderTrisElement* atTrisElem;
    ColliderTris* ac_tris = (ColliderTris*)acCol;
    ColliderTrisElement* acTrisElem;

    if (ac_tris->count > 0 && ac_tris->elements != NULL && at_tris->count > 0 && at_tris->elements != NULL) {
        for (acTrisElem = ac_tris->elements; acTrisElem < ac_tris->elements + ac_tris->count; acTrisElem++) {
            if (CollisionCheck_CheckNoAC(&acTrisElem->base) == true) {
                continue;
            }
            for (atTrisElem = at_tris->elements; atTrisElem < at_tris->elements + at_tris->count; atTrisElem++) {
                if (CollisionCheck_CheckNoAT(&atTrisElem->base) == true) {
                    continue;
                }
                if (CollisionCheck_CheckNoATAC(&atTrisElem->base, &acTrisElem->base) == true) {
                    continue;
                }
                if (Math3D_triangleCrossTriangle_cp(&atTrisElem->dim, &acTrisElem->dim, &cross) == true) {
                    Vec3f atPos;
                    Vec3f acPos;

                    atPos.x =
                        (atTrisElem->dim.vtx[0].x + atTrisElem->dim.vtx[1].x + atTrisElem->dim.vtx[2].x) * (1.0f / 3);
                    atPos.y =
                        (atTrisElem->dim.vtx[0].y + atTrisElem->dim.vtx[1].y + atTrisElem->dim.vtx[2].y) * (1.0f / 3);
                    atPos.z =
                        (atTrisElem->dim.vtx[0].z + atTrisElem->dim.vtx[1].z + atTrisElem->dim.vtx[2].z) * (1.0f / 3);
                    acPos.x =
                        (acTrisElem->dim.vtx[0].x + acTrisElem->dim.vtx[1].x + acTrisElem->dim.vtx[2].x) * (1.0f / 3);
                    acPos.y =
                        (acTrisElem->dim.vtx[0].y + acTrisElem->dim.vtx[1].y + acTrisElem->dim.vtx[2].y) * (1.0f / 3);
                    acPos.z =
                        (acTrisElem->dim.vtx[0].z + acTrisElem->dim.vtx[1].z + acTrisElem->dim.vtx[2].z) * (1.0f / 3);
                    CollisionCheck_setATAC_HitInfo(play, &at_tris->base, &atTrisElem->base, &atPos, &ac_tris->base,
                                             &acTrisElem->base, &acPos, &cross);
                    return;
                }
            }
        }
    }
}

void CollisionCheck_ATAC_Tris_Vs_Swrd(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                   Collider* acCol) {
    static Vec3f cross;
    static TriNorm tris1;
    static TriNorm tris2;
    ColliderTris* at_tris = (ColliderTris*)atCol;
    ColliderTrisElement* atTrisElem;
    ColliderQuad* acQuad = (ColliderQuad*)acCol;

    if (at_tris->count > 0 && at_tris->elements != NULL) {
        if (CollisionCheck_CheckNoAC(&acQuad->elem) == true) {
            return;
        }
        Math3D_triangle_ct(&tris1, &acQuad->dim.quad[2], &acQuad->dim.quad[3], &acQuad->dim.quad[1]);
        Math3D_triangle_ct(&tris2, &acQuad->dim.quad[1], &acQuad->dim.quad[0], &acQuad->dim.quad[2]);
        for (atTrisElem = at_tris->elements; atTrisElem < at_tris->elements + at_tris->count; atTrisElem++) {
            if (CollisionCheck_CheckNoAT(&atTrisElem->base) == true) {
                continue;
            }
            if (CollisionCheck_CheckNoATAC(&atTrisElem->base, &acQuad->elem) == true) {
                continue;
            }
            if (Math3D_triangleCrossTriangle_cp(&tris1, &atTrisElem->dim, &cross) == true ||
                Math3D_triangleCrossTriangle_cp(&tris2, &atTrisElem->dim, &cross) == true) {
                Vec3f atPos;
                Vec3f acPos;

                atPos.x = (atTrisElem->dim.vtx[0].x + atTrisElem->dim.vtx[1].x + atTrisElem->dim.vtx[2].x) * (1.0f / 3);
                atPos.y = (atTrisElem->dim.vtx[0].y + atTrisElem->dim.vtx[1].y + atTrisElem->dim.vtx[2].y) * (1.0f / 3);
                atPos.z = (atTrisElem->dim.vtx[0].z + atTrisElem->dim.vtx[1].z + atTrisElem->dim.vtx[2].z) * (1.0f / 3);
                acPos.x = (acQuad->dim.quad[0].x +
                           (acQuad->dim.quad[1].x + (acQuad->dim.quad[3].x + acQuad->dim.quad[2].x))) /
                          4.0f;
                acPos.y = (acQuad->dim.quad[0].y +
                           (acQuad->dim.quad[1].y + (acQuad->dim.quad[3].y + acQuad->dim.quad[2].y))) /
                          4.0f;
                acPos.z = (acQuad->dim.quad[0].z +
                           (acQuad->dim.quad[1].z + (acQuad->dim.quad[3].z + acQuad->dim.quad[2].z))) /
                          4.0f;
                CollisionCheck_setATAC_HitInfo(play, &at_tris->base, &atTrisElem->base, &atPos, &acQuad->base, &acQuad->elem,
                                         &acPos, &cross);
                return;
            }
        }
    }
}

void CollisionCheck_ATAC_Swrd_Vs_Tris(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                   Collider* acCol) {
    static Vec3f cross;
    static TriNorm tris1;
    static TriNorm tris2;
    ColliderQuad* atQuad = (ColliderQuad*)atCol;
    ColliderTris* ac_tris = (ColliderTris*)acCol;
    ColliderTrisElement* acTrisElem;

    if (ac_tris->count > 0 && ac_tris->elements != NULL) {
        if (CollisionCheck_CheckNoAT(&atQuad->elem) == true) {
            return;
        }
        Math3D_triangle_ct(&tris1, &atQuad->dim.quad[2], &atQuad->dim.quad[3], &atQuad->dim.quad[1]);
        Math3D_triangle_ct(&tris2, &atQuad->dim.quad[1], &atQuad->dim.quad[0], &atQuad->dim.quad[2]);
        for (acTrisElem = ac_tris->elements; acTrisElem < ac_tris->elements + ac_tris->count; acTrisElem++) {
            if (CollisionCheck_CheckNoAC(&acTrisElem->base) == true) {
                continue;
            }
            if (CollisionCheck_CheckNoATAC(&atQuad->elem, &acTrisElem->base) == true) {
                continue;
            }
            if (Math3D_triangleCrossTriangle_cp(&tris1, &acTrisElem->dim, &cross) == true ||
                Math3D_triangleCrossTriangle_cp(&tris2, &acTrisElem->dim, &cross) == true) {
                if (ClObjSwrd_lengthCheck(play, atQuad, &cross)) {
                    Vec3f atPos;
                    Vec3f acPos;

                    acPos.x =
                        (acTrisElem->dim.vtx[0].x + acTrisElem->dim.vtx[1].x + acTrisElem->dim.vtx[2].x) * (1.0f / 3);
                    acPos.y =
                        (acTrisElem->dim.vtx[0].y + acTrisElem->dim.vtx[1].y + acTrisElem->dim.vtx[2].y) * (1.0f / 3);
                    acPos.z =
                        (acTrisElem->dim.vtx[0].z + acTrisElem->dim.vtx[1].z + acTrisElem->dim.vtx[2].z) * (1.0f / 3);
                    atPos.x = (atQuad->dim.quad[0].x +
                               (atQuad->dim.quad[1].x + (atQuad->dim.quad[3].x + atQuad->dim.quad[2].x))) /
                              4.0f;
                    atPos.y = (atQuad->dim.quad[0].y +
                               (atQuad->dim.quad[1].y + (atQuad->dim.quad[3].y + atQuad->dim.quad[2].y))) /
                              4.0f;
                    atPos.z = (atQuad->dim.quad[0].z +
                               (atQuad->dim.quad[1].z + (atQuad->dim.quad[3].z + atQuad->dim.quad[2].z))) /
                              4.0f;
                    CollisionCheck_setATAC_HitInfo(play, &atQuad->base, &atQuad->elem, &atPos, &ac_tris->base,
                                             &acTrisElem->base, &acPos, &cross);
                    return;
                }
            }
        }
    }
}

void CollisionCheck_ATAC_Swrd_Vs_Swrd(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol,
                                   Collider* acCol) {
    static TriNorm ac_tris[2];
    static Vec3f cross;
    static TriNorm at_tris[2];
    ColliderQuad* atQuad = (ColliderQuad*)atCol;
    ColliderQuad* acQuad = (ColliderQuad*)acCol;
    s32 i;
    s32 j;

    if (CollisionCheck_CheckNoAT(&atQuad->elem) == true) {
        return;
    }
    if (CollisionCheck_CheckNoAC(&acQuad->elem) == true) {
        return;
    }
    if (CollisionCheck_CheckNoATAC(&atQuad->elem, &acQuad->elem) == true) {
        return;
    }

    Math3D_triangle_ct(&at_tris[0], &atQuad->dim.quad[2], &atQuad->dim.quad[3], &atQuad->dim.quad[1]);
    Math3D_triangle_ct(&at_tris[1], &atQuad->dim.quad[2], &atQuad->dim.quad[1], &atQuad->dim.quad[0]);
    Math3D_triangle_ct(&ac_tris[0], &acQuad->dim.quad[2], &acQuad->dim.quad[3], &acQuad->dim.quad[1]);
    Math3D_triangle_ct(&ac_tris[1], &acQuad->dim.quad[2], &acQuad->dim.quad[1], &acQuad->dim.quad[0]);

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (Math3D_triangleCrossTriangle_cp(&at_tris[j], &ac_tris[i], &cross) == true) {
                if (ClObjSwrd_lengthCheck(play, atQuad, &cross)) {
                    Vec3f atPos;
                    Vec3f acPos;

                    atPos.x = (atQuad->dim.quad[0].x +
                               (atQuad->dim.quad[1].x + (atQuad->dim.quad[3].x + atQuad->dim.quad[2].x))) /
                              4.0f;
                    atPos.y = (atQuad->dim.quad[0].y +
                               (atQuad->dim.quad[1].y + (atQuad->dim.quad[3].y + atQuad->dim.quad[2].y))) /
                              4.0f;
                    atPos.z = (atQuad->dim.quad[0].z +
                               (atQuad->dim.quad[1].z + (atQuad->dim.quad[3].z + atQuad->dim.quad[2].z))) /
                              4.0f;
                    acPos.x = (acQuad->dim.quad[0].x +
                               (acQuad->dim.quad[1].x + (acQuad->dim.quad[3].x + acQuad->dim.quad[2].x))) /
                              4.0f;
                    acPos.y = (acQuad->dim.quad[0].y +
                               (acQuad->dim.quad[1].y + (acQuad->dim.quad[3].y + acQuad->dim.quad[2].y))) /
                              4.0f;
                    acPos.z = (acQuad->dim.quad[0].z +
                               (acQuad->dim.quad[1].z + (acQuad->dim.quad[3].z + acQuad->dim.quad[2].z))) /
                              4.0f;
                    CollisionCheck_setATAC_HitInfo(play, &atQuad->base, &atQuad->elem, &atPos, &acQuad->base, &acQuad->elem,
                                             &acPos, &cross);
                    return;
                }
            }
        }
    }
}

void CollisionCheck_ACWaitHitmark_JntSph(PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderJntSph* jntSph = (ColliderJntSph*)col;
    ColliderJntSphElement* jntSphElem;

    for (jntSphElem = jntSph->elements; jntSphElem < jntSph->elements + jntSph->count; jntSphElem++) {
        if ((jntSphElem->base.acElemFlags & ACELEM_DRAW_HITMARK) && (jntSphElem->base.acHitElem != NULL) &&
            !(jntSphElem->base.acHitElem->atElemFlags & ATELEM_DREW_HITMARK)) {
            Vec3f cross;

            xyz_t_move_s_xyz(&cross, &jntSphElem->base.acDmgInfo.hitPos);
            CollisionCheck_setHitMark(play, jntSphElem->base.acHit, jntSphElem->base.acHitElem, &jntSph->base,
                                      &jntSphElem->base, &cross);
            jntSphElem->base.acHitElem->atElemFlags |= ATELEM_DREW_HITMARK;
            return;
        }
    }
}

void CollisionCheck_ACWaitHitmark_Pipe(PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderCylinder* cyl = (ColliderCylinder*)col;

    if ((cyl->elem.acElemFlags & ACELEM_DRAW_HITMARK) && (cyl->elem.acHitElem != NULL) &&
        !(cyl->elem.acHitElem->atElemFlags & ATELEM_DREW_HITMARK)) {
        Vec3f cross;

        xyz_t_move_s_xyz(&cross, &cyl->elem.acDmgInfo.hitPos);
        CollisionCheck_setHitMark(play, cyl->elem.acHit, cyl->elem.acHitElem, &cyl->base, &cyl->elem, &cross);
        cyl->elem.acHitElem->atElemFlags |= ATELEM_DREW_HITMARK;
    }
}

void CollisionCheck_ACWaitHitmark_Tris(PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderTris* tris = (ColliderTris*)col;
    ColliderTrisElement* trisElem;

    for (trisElem = tris->elements; trisElem < tris->elements + tris->count; trisElem++) {
        if ((trisElem->base.acElemFlags & ACELEM_DRAW_HITMARK) && (trisElem->base.acHitElem != NULL) &&
            !(trisElem->base.acHitElem->atElemFlags & ATELEM_DREW_HITMARK)) {
            Vec3f cross;

            xyz_t_move_s_xyz(&cross, &trisElem->base.acDmgInfo.hitPos);
            CollisionCheck_setHitMark(play, trisElem->base.acHit, trisElem->base.acHitElem, &tris->base,
                                      &trisElem->base, &cross);
            trisElem->base.acHitElem->atElemFlags |= ATELEM_DREW_HITMARK;
            return;
        }
    }
}

void CollisionCheck_ACWaitHitmark_Swrd(PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderQuad* quad = (ColliderQuad*)col;
    Vec3f cross;

    if ((quad->elem.acElemFlags & ACELEM_DRAW_HITMARK) && (quad->elem.acHitElem != NULL) &&
        !(quad->elem.acHitElem->atElemFlags & ATELEM_DREW_HITMARK)) {
        xyz_t_move_s_xyz(&cross, &quad->elem.acDmgInfo.hitPos);
        CollisionCheck_setHitMark(play, quad->elem.acHit, quad->elem.acHitElem, &quad->base, &quad->elem, &cross);
        quad->elem.acHitElem->atElemFlags |= ATELEM_DREW_HITMARK;
    }
}

static ColChkApplyFunc whm_function[] = {
    CollisionCheck_ACWaitHitmark_JntSph,
    CollisionCheck_ACWaitHitmark_Pipe,
    CollisionCheck_ACWaitHitmark_Tris,
    CollisionCheck_ACWaitHitmark_Swrd,
};

/**
 * Handles hit effects for each AC collider that had an AC collision. Spawns hitmarks and plays sound effects.
 */
void CollisionCheck_ACWaitHitMark(PlayState* play, CollisionCheckContext* colChkCtx) {
    Collider** acColP;
    Collider* acCol;

    for (acColP = colChkCtx->colAC; acColP < colChkCtx->colAC + colChkCtx->colACCount; acColP++) {
        acCol = *acColP;

        if (acCol != NULL && acCol->acFlags & AC_ON) {
            if (acCol->actor != NULL && acCol->actor->update == NULL) {
                continue;
            }
            whm_function[acCol->shape](play, colChkCtx, acCol);
        }
    }
}

static ColChkVsFunc collision_function[COLSHAPE_MAX][COLSHAPE_MAX] = {
    // COLSHAPE_JNTSPH
    {
        CollisionCheck_ATAC_JntSph_Vs_JntSph, // COLSHAPE_JNTSPH
        CollisionCheck_ATAC_JntSph_Vs_Pipe,    // COLSHAPE_CYLINDER
        CollisionCheck_ATAC_Jntsph_Vs_Tris,   // COLSHAPE_TRIS
        CollisionCheck_ATAC_Jntsph_Vs_Swrd    // COLSHAPE_QUAD
    },
    // COLSHAPE_CYLINDER
    {
        CollisionCheck_ATAC_Pipe_Vs_JntSph, // COLSHAPE_JNTSPH
        CollisionCheck_ATAC_Pipe_Vs_Pipe,    // COLSHAPE_CYLINDER
        CollisionCheck_ATAC_Pipe_Vs_Tris,   // COLSHAPE_TRIS
        CollisionCheck_ATAC_Pipe_Vs_Swrd    // COLSHAPE_QUAD
    },
    // COLSHAPE_TRIS
    {
        CollisionCheck_ATAC_Tris_Vs_Jntsph, // COLSHAPE_JNTSPH
        CollisionCheck_ATAC_Tris_Vs_Pipe,    // COLSHAPE_CYLINDER
        CollisionCheck_ATAC_Tris_Vs_Tris,   // COLSHAPE_TRIS
        CollisionCheck_ATAC_Tris_Vs_Swrd    // COLSHAPE_QUAD
    },
    // COLSHAPE_QUAD
    {
        CollisionCheck_ATAC_Swrd_Vs_Jntsph, // COLSHAPE_JNTSPH
        CollisionCheck_ATAC_Swrd_Vs_Pipe,    // COLSHAPE_CYLINDER
        CollisionCheck_ATAC_Swrd_Vs_Tris,   // COLSHAPE_TRIS
        CollisionCheck_ATAC_Swrd_Vs_Swrd    // COLSHAPE_QUAD
    },
};

/**
 * Iterates through all AC colliders, performing AC collisions with the AT collider.
 */
void CollsionCheck_vsACObj(PlayState* play, CollisionCheckContext* colChkCtx, Collider* atCol) {
    Collider** acColP;
    Collider* acCol;

    for (acColP = colChkCtx->colAC; acColP < colChkCtx->colAC + colChkCtx->colACCount; acColP++) {
        acCol = *acColP;

        if (acCol != NULL && acCol->acFlags & AC_ON) {
            if (acCol->actor != NULL && acCol->actor->update == NULL) {
                continue;
            }
            if ((acCol->acFlags & atCol->atFlags & AC_TYPE_ALL) && (atCol != acCol)) {
                if (!(atCol->atFlags & AT_SELF) && atCol->actor != NULL && acCol->actor == atCol->actor) {
                    continue;
                }
                collision_function[atCol->shape][acCol->shape](play, colChkCtx, atCol, acCol);
            }
        }
    }
}

/**
 * Iterates through all AT colliders, testing them for AC collisions with each AC collider, setting the info regarding
 * the collision for each AC and AT collider that collided. Then spawns hitmarks and plays sound effects for each
 * successful collision. To collide, an AT collider must share a type (AC_TYPE_PLAYER, AC_TYPE_ENEMY, or AC_TYPE_OTHER)
 * with the AC collider and the AT and AC elements that overlapped must share a dmgFlag.
 */
void CollisionCheck_ATAC(PlayState* play, CollisionCheckContext* colChkCtx) {
    Collider** atColP;
    Collider* atCol;

    if (colChkCtx->colATCount == 0 || colChkCtx->colACCount == 0) {
        return;
    }
    for (atColP = colChkCtx->colAT; atColP < colChkCtx->colAT + colChkCtx->colATCount; atColP++) {
        atCol = *atColP;

        if (atCol != NULL && atCol->atFlags & AT_ON) {
            if (atCol->actor != NULL && atCol->actor->update == NULL) {
                continue;
            }
            CollsionCheck_vsACObj(play, colChkCtx, atCol);
        }
    }
    CollisionCheck_ACWaitHitMark(play, colChkCtx);
}

typedef enum ColChkMassType {
    /* 0 */ MASSTYPE_IMMOVABLE,
    /* 1 */ MASSTYPE_HEAVY,
    /* 2 */ MASSTYPE_NORMAL
} ColChkMassType;

/**
 * Get mass type. Immovable colliders cannot be pushed, while heavy colliders can only be pushed by heavy and immovable
 * colliders.
 */
s32 get_type(u8 mass) {
    if (mass == MASS_IMMOVABLE) {
        return MASSTYPE_IMMOVABLE;
    }
    if (mass == MASS_HEAVY) {
        return MASSTYPE_HEAVY;
    }
    return MASSTYPE_NORMAL;
}

/**
 * Sets OC collision flags for OC collider overlaps. If both colliders are attached to actors and can push,
 * also performs an elastic collision where both colliders are moved apart in proportion to their masses.
 */
void CollisionCheck_setOC_HitInfo(Collider* leftCol, ColliderElement* leftElem, Vec3f* leftPos, Collider* rightCol,
                              ColliderElement* rightElem, Vec3f* rightPos, f32 overlap) {
    f32 pad;
    f32 leftDispRatio;
    f32 rightDispRatio;
    f32 xzDist;
    f32 leftMass;
    f32 rightMass;
    f32 totalMass;
    f32 inverseTotalMass;
    f32 xDelta;
    f32 zDelta;
    Actor* leftActor = leftCol->actor;
    Actor* rightActor = rightCol->actor;
    s32 rightMassType;
    s32 leftMassType;

    leftCol->ocFlags1 |= OC1_HIT;
    leftCol->oc = rightActor;
    leftElem->ocElemFlags |= OCELEM_HIT;
    if (rightCol->ocFlags2 & OC2_TYPE_PLAYER) {
        leftCol->ocFlags2 |= OC2_HIT_PLAYER;
    }
    rightCol->oc = leftActor;
    rightCol->ocFlags1 |= OC1_HIT;
    rightElem->ocElemFlags |= OCELEM_HIT;
    if (leftCol->ocFlags2 & OC2_TYPE_PLAYER) {
        rightCol->ocFlags2 |= OC2_HIT_PLAYER;
    }
    if (leftActor == NULL || rightActor == NULL || leftCol->ocFlags1 & OC1_NO_PUSH ||
        rightCol->ocFlags1 & OC1_NO_PUSH) {
        return;
    }
    leftMassType = get_type(leftActor->colChkInfo.mass);
    rightMassType = get_type(rightActor->colChkInfo.mass);
    leftMass = leftActor->colChkInfo.mass;
    rightMass = rightActor->colChkInfo.mass;
    totalMass = leftMass + rightMass;
    if (IS_ZERO(totalMass)) {
        leftMass = rightMass = 1.0f;
        totalMass = 2.0f;
    }
    xDelta = rightPos->x - leftPos->x;
    zDelta = rightPos->z - leftPos->z;
    xzDist = sqrtf(SQ(xDelta) + SQ(zDelta));

    if (leftMassType == MASSTYPE_IMMOVABLE) {
        if (rightMassType == MASSTYPE_IMMOVABLE) {
            return;
        } else { // rightMassType == MASSTYPE_HEAVY or MASSTYPE_NORMAL
            leftDispRatio = 0;
            rightDispRatio = 1;
        }
    } else if (leftMassType == MASSTYPE_HEAVY) {
        if (rightMassType == MASSTYPE_IMMOVABLE) {
            leftDispRatio = 1;
            rightDispRatio = 0;
        } else if (rightMassType == MASSTYPE_HEAVY) {
            leftDispRatio = 0.5f;
            rightDispRatio = 0.5f;
        } else { // rightMassType == MASSTYPE_NORMAL
            leftDispRatio = 0;
            rightDispRatio = 1;
        }
    } else { // leftMassType == MASSTYPE_NORMAL
        if (rightMassType == MASSTYPE_NORMAL) {
            inverseTotalMass = 1 / totalMass;
            leftDispRatio = rightMass * inverseTotalMass;
            rightDispRatio = leftMass * inverseTotalMass;
        } else { // rightMassType == MASSTYPE_HEAVY or MASSTYPE_IMMOVABLE
            leftDispRatio = 1;
            rightDispRatio = 0;
        }
    }

    if (!IS_ZERO(xzDist)) {
        xDelta *= overlap / xzDist;
        zDelta *= overlap / xzDist;
        leftActor->colChkInfo.displacement.x += -xDelta * leftDispRatio;
        leftActor->colChkInfo.displacement.z += -zDelta * leftDispRatio;
        rightActor->colChkInfo.displacement.x += xDelta * rightDispRatio;
        rightActor->colChkInfo.displacement.z += zDelta * rightDispRatio;
    } else if (!(overlap == 0.0f)) {
        leftActor->colChkInfo.displacement.x += -overlap * leftDispRatio;
        rightActor->colChkInfo.displacement.x += overlap * rightDispRatio;
    } else {
        leftActor->colChkInfo.displacement.x -= leftDispRatio;
        rightActor->colChkInfo.displacement.x += rightDispRatio;
    }
}

void CollisionCheck_OC_JntSph_Vs_JntSph(PlayState* play, CollisionCheckContext* colChkCtx, Collider* leftCol,
                                      Collider* rightCol) {
    ColliderJntSph* leftJntSph = (ColliderJntSph*)leftCol;
    ColliderJntSphElement* leftJntSphElem;
    ColliderJntSph* rightJntSph = (ColliderJntSph*)rightCol;
    ColliderJntSphElement* rightJntSphElem;
    f32 overlapSize;

    if (leftJntSph->count > 0 && leftJntSph->elements != NULL && rightJntSph->count > 0 &&
        rightJntSph->elements != NULL) {
        for (leftJntSphElem = leftJntSph->elements; leftJntSphElem < leftJntSph->elements + leftJntSph->count;
             leftJntSphElem++) {
            if (!(leftJntSphElem->base.ocElemFlags & OCELEM_ON)) {
                continue;
            }
            for (rightJntSphElem = rightJntSph->elements; rightJntSphElem < rightJntSph->elements + rightJntSph->count;
                 rightJntSphElem++) {
                if (!(rightJntSphElem->base.ocElemFlags & OCELEM_ON)) {
                    continue;
                }
                if (Math3D_sphereCrossSphere_cl(&leftJntSphElem->dim.worldSphere, &rightJntSphElem->dim.worldSphere,
                                           &overlapSize) == true) {
                    Vec3f leftPos;
                    Vec3f rightPos;

                    xyz_t_move_s_xyz(&leftPos, &leftJntSphElem->dim.worldSphere.center);
                    xyz_t_move_s_xyz(&rightPos, &rightJntSphElem->dim.worldSphere.center);
                    CollisionCheck_setOC_HitInfo(&leftJntSph->base, &leftJntSphElem->base, &leftPos, &rightJntSph->base,
                                             &rightJntSphElem->base, &rightPos, overlapSize);
                }
            }
        }
    }
}

void CollisionCheck_OC_JntSph_Vs_Pipe(PlayState* play, CollisionCheckContext* colChkCtx, Collider* leftCol,
                                   Collider* rightCol) {
    ColliderJntSph* leftJntSph = (ColliderJntSph*)leftCol;
    ColliderJntSphElement* leftJntSphElem;
    ColliderCylinder* rightCyl = (ColliderCylinder*)rightCol;
    f32 overlapSize;

    if (leftJntSph->count > 0 && leftJntSph->elements != NULL) {
        if ((rightCyl->base.ocFlags1 & OC1_ON) && (rightCyl->elem.ocElemFlags & OCELEM_ON)) {
            for (leftJntSphElem = leftJntSph->elements; leftJntSphElem < leftJntSph->elements + leftJntSph->count;
                 leftJntSphElem++) {
                if (!(leftJntSphElem->base.ocElemFlags & OCELEM_ON)) {
                    continue;
                }
                if (Math3D_sphereVsPipe_cl(&leftJntSphElem->dim.worldSphere, &rightCyl->dim, &overlapSize) == true) {
                    Vec3f leftPos;
                    Vec3f rightPos;

                    xyz_t_move_s_xyz(&leftPos, &leftJntSphElem->dim.worldSphere.center);
                    xyz_t_move_s_xyz(&rightPos, &rightCyl->dim.pos);
                    CollisionCheck_setOC_HitInfo(&leftJntSph->base, &leftJntSphElem->base, &leftPos, &rightCyl->base,
                                             &rightCyl->elem, &rightPos, overlapSize);
                }
            }
        }
    }
}

void CollisionCheck_OC_Pipe_Vs_JntSph(PlayState* play, CollisionCheckContext* colChkCtx, Collider* leftCol,
                                   Collider* rightCol) {
    CollisionCheck_OC_JntSph_Vs_Pipe(play, colChkCtx, rightCol, leftCol);
}

void CollisionCheck_OC_Pipe_Vs_Pipe(PlayState* play, CollisionCheckContext* colChkCtx, Collider* leftCol,
                                Collider* rightCol) {
    ColliderCylinder* leftCyl = (ColliderCylinder*)leftCol;
    ColliderCylinder* rightCyl = (ColliderCylinder*)rightCol;
    f32 overlapSize;

    if ((leftCyl->base.ocFlags1 & OC1_ON) && (rightCyl->base.ocFlags1 & OC1_ON)) {
        if ((leftCyl->elem.ocElemFlags & OCELEM_ON) && (rightCyl->elem.ocElemFlags & OCELEM_ON)) {
            if (Math3D_pipeVsPipe_cl(&leftCyl->dim, &rightCyl->dim, &overlapSize) == true) {
                Vec3f leftPos;
                Vec3f rightPos;

                xyz_t_move_s_xyz(&leftPos, &leftCyl->dim.pos);
                xyz_t_move_s_xyz(&rightPos, &rightCyl->dim.pos);
                CollisionCheck_setOC_HitInfo(&leftCyl->base, &leftCyl->elem, &leftPos, &rightCyl->base, &rightCyl->elem,
                                         &rightPos, overlapSize);
            }
        }
    }
}

/**
 *  Skip any OC colliders that are off
 */
s32 CollisionCheck_Check1ClObjNoOC(Collider* collider) {
    if (!(collider->ocFlags1 & OC1_ON)) {
        return true;
    }
    return false;
}

/**
 * Checks for OC compatibility. There are three conditions:
 * First, each collider must have an OC flag corresponding to the other's OC type.
 * Second, OC2_UNK1 and OC2_UNK2 can't collide with each other (has something to do with horses?)
 * Third, the colliders can't collide if they belong to the same actor
 */
s32 CollisionCheck_Check2ClObjNoOC(Collider* left, Collider* right) {
    if (!((left->ocFlags1 & right->ocFlags2 & OC1_TYPE_ALL) && (left->ocFlags2 & right->ocFlags1 & OC1_TYPE_ALL)) ||
        ((left->ocFlags2 & OC2_UNK1) && (right->ocFlags2 & OC2_UNK2)) ||
        ((right->ocFlags2 & OC2_UNK1) && (left->ocFlags2 & OC2_UNK2))) {
        return true;
    }
    if (left->actor == right->actor) {
        return true;
    }
    return false;
}

static ColChkVsFunc oc_collision_function[COLSHAPE_MAX][COLSHAPE_MAX] = {
    // COLSHAPE_JNTSPH
    {
        CollisionCheck_OC_JntSph_Vs_JntSph, // COLSHAPE_JNTSPH
        CollisionCheck_OC_JntSph_Vs_Pipe,    // COLSHAPE_CYLINDER
        NULL,                             // COLSHAPE_TRIS
        NULL                              // COLSHAPE_QUAD
    },
    // COLSHAPE_CYLINDER
    {
        CollisionCheck_OC_Pipe_Vs_JntSph, // COLSHAPE_JNTSPH
        CollisionCheck_OC_Pipe_Vs_Pipe,    // COLSHAPE_CYLINDER
        NULL,                          // COLSHAPE_TRIS
        NULL                           // COLSHAPE_QUAD
    },
    // COLSHAPE_TRIS
    {
        NULL, // COLSHAPE_JNTSPH
        NULL, // COLSHAPE_CYLINDER
        NULL, // COLSHAPE_TRIS
        NULL  // COLSHAPE_QUAD
    },
    // COLSHAPE_QUAD
    {
        NULL, // COLSHAPE_JNTSPH
        NULL, // COLSHAPE_CYLINDER
        NULL, // COLSHAPE_TRIS
        NULL  // COLSHAPE_QUAD
    },
};

/**
 * Iterates through all OC colliders and collides them with all subsequent OC colliders on the list. During an OC
 * collision, colliders with overlapping elements move away from each other so that their elements no longer overlap.
 * The relative amount each collider is pushed is determined by the collider's mass. Only JntSph and Cylinder colliders
 * can collide, and each collider must have the OC flag corresponding to the other's OC type. Additionally, OC2_UNK1
 * cannot collide with OC2_UNK2, nor can two colliders that share an actor.
 */
void CollisionCheck_OC(PlayState* play, CollisionCheckContext* colChkCtx) {
    Collider** leftColP;
    Collider** rightColP;
    ColChkVsFunc vsFunc;

    for (leftColP = colChkCtx->colOC; leftColP < colChkCtx->colOC + colChkCtx->colOCCount; leftColP++) {
        if (*leftColP == NULL || CollisionCheck_Check1ClObjNoOC(*leftColP) == true) {
            continue;
        }
        for (rightColP = leftColP + 1; rightColP < colChkCtx->colOC + colChkCtx->colOCCount; rightColP++) {
            if (*rightColP == NULL || CollisionCheck_Check1ClObjNoOC(*rightColP) == true ||
                CollisionCheck_Check2ClObjNoOC(*leftColP, *rightColP) == true) {
                continue;
            }
            vsFunc = oc_collision_function[(*leftColP)->shape][(*rightColP)->shape];
            if (vsFunc == NULL) {
                PRINTF(T("CollisionCheck_OC():未対応 %d, %d\n", "CollisionCheck_OC(): Not compatible %d, %d\n"),
                       (*leftColP)->shape, (*rightColP)->shape);
                continue;
            }
            vsFunc(play, colChkCtx, *leftColP, *rightColP);
        }
    }
}

/**
 * Initializes CollisionCheckInfo to default values
 */
void CollisionCheck_Status_ct(CollisionCheckInfo* info) {
    static CollisionCheckInfo status_org = {
        NULL, { 0.0f, 0.0f, 0.0f }, 10, 10, 0, 50, 8, 0, 0, 0, 0,
    };

    *info = status_org;
}

/**
 * Resets ColisionCheckInfo fields other than DamageTable, mass, and dim.
 */
void CollisionCheck_Status_Clear(CollisionCheckInfo* info) {
    info->damage = 0;
    info->damageEffect = 0;
    info->atHitEffect = 0;
    info->acHitEffect = 0;
    info->displacement.x = info->displacement.y = info->displacement.z = 0.0f;
}

/**
 * Sets up CollisionCheckInfo using the values in clobj_default. Does not set a damage table or the unused unk_14.
 * Unused, as all actors that don't set a damage table set their CollisionCheckInfo manually
 */
void CollisionCheck_Status_set(CollisionCheckInfo* info, CollisionCheckInfoInit* clobj_default) {
    info->health = clobj_default->health;
    info->cylRadius = clobj_default->cylRadius;
    info->cylHeight = clobj_default->cylHeight;
    info->mass = clobj_default->mass;
}

/**
 * Sets up CollisionCheckInfo using the values in clobj_default. Does not set the unused unk_14
 */
void CollisionCheck_Status_set2(CollisionCheckInfo* info, DamageTable* damageTable, CollisionCheckInfoInit* clobj_default) {
    info->health = clobj_default->health;
    info->damageTable = damageTable;
    info->cylRadius = clobj_default->cylRadius;
    info->cylHeight = clobj_default->cylHeight;
    info->mass = clobj_default->mass;
}

/**
 * Sets up CollisionCheckInfo using the values in clobj_default. Sets the unused unk_14
 */
void CollisionCheck_Status_set3(CollisionCheckInfo* info, DamageTable* damageTable, CollisionCheckInfoInit2* clobj_default) {
    info->health = clobj_default->health;
    info->damageTable = damageTable;
    info->cylRadius = clobj_default->cylRadius;
    info->cylHeight = clobj_default->cylHeight;
    info->cylYShift = clobj_default->cylYShift;
    info->mass = clobj_default->mass;
}

/**
 * Sets up CollisionCheckInfo using the values in Init and a preset damage table. Sets the unused unk_14.
 * Unused, as all actors that use a preset damage table set their CollisionCheckInfo manually.
 */
void CollisionCheck_Status_set3_l(CollisionCheckInfo* info, s32 index, CollisionCheckInfoInit2* clobj_default) {
    CollisionCheck_Status_set3(info, CollisionBtlTbl_get(index), clobj_default);
}

/**
 * Apply AC damage effect
 */
void CollisionCheck_Damage_CalcGeneral(PlayState* play, CollisionCheckContext* colChkCtx, Collider* col,
                                ColliderElement* elem) {
    DamageTable* tbl;
    f32 damage;

    if (col->actor == NULL || !(col->acFlags & AC_HIT)) {
        return;
    }
    if (!(elem->acElemFlags & ACELEM_HIT) || elem->acElemFlags & ACELEM_NO_DAMAGE) {
        return;
    }

    ASSERT(elem->acHitElem != NULL, "pclobj_elem->ac_hit_elem != NULL", "../z_collision_check.c", 6493);
    tbl = col->actor->colChkInfo.damageTable;
    if (tbl == NULL) {
        damage = (f32)elem->acHitElem->atDmgInfo.damage - elem->acDmgInfo.defense;
        if (damage < 0) {
            damage = 0;
        }
    } else {
        s32 i;
        u32 flags = elem->acHitElem->atDmgInfo.dmgFlags;

        for (i = 0; i < 32; i++, flags >>= 1) {
            if (flags == 1) {
                break;
            }
        }

        damage = tbl->table[i] & 0xF;
        col->actor->colChkInfo.damageEffect = tbl->table[i] >> 4 & 0xF;
    }
    if (!(col->acFlags & AC_HARD)) {
        col->actor->colChkInfo.damage += damage;
    }
}

/**
 * Apply ColliderJntSph AC damage effect
 */
void CollisionCheck_Damage_CalcJntSph(PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderJntSph* jntSph = (ColliderJntSph*)col;
    s32 i;

    if (jntSph->count > 0 && jntSph->elements != NULL) {
        for (i = 0; i < jntSph->count; i++) {
            CollisionCheck_Damage_CalcGeneral(play, colChkCtx, &jntSph->base, &jntSph->elements[i].base);
        }
    }
}

/**
 * Apply ColliderCylinder AC damage effect
 */
void CollisionCheck_Damage_CalcPipe(PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderCylinder* cyl = (ColliderCylinder*)col;

    CollisionCheck_Damage_CalcGeneral(play, colChkCtx, &cyl->base, &cyl->elem);
}

/**
 * Apply ColliderTris AC damage effect
 */
void CollisionCheck_Damage_CalcTris(PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderTris* tris = (ColliderTris*)col;
    s32 i;

    for (i = 0; i < tris->count; i++) {
        CollisionCheck_Damage_CalcGeneral(play, colChkCtx, col, &tris->elements[i].base);
    }
}

/**
 *  Apply ColliderQuad AC damage effect
 */
void CollisionCheck_Damage_CalcSwrd(PlayState* play, CollisionCheckContext* colChkCtx, Collider* col) {
    ColliderQuad* quad = (ColliderQuad*)col;

    CollisionCheck_Damage_CalcGeneral(play, colChkCtx, &quad->base, &quad->elem);
}

static ColChkApplyFunc DataType_DamageCalc_FuncTbl[COLSHAPE_MAX] = {
    CollisionCheck_Damage_CalcJntSph,
    CollisionCheck_Damage_CalcPipe,
    CollisionCheck_Damage_CalcTris,
    CollisionCheck_Damage_CalcSwrd,
};

/**
 * For all AC colliders, sets any damage effects from collisions with AT colliders to their corresponding actor's
 * CollisionCheckInfo.
 */
void CollisionCheck_Damage_Calc(PlayState* play, CollisionCheckContext* colChkCtx) {
    s32 i;
    Collider* col;

    for (i = 0; i < colChkCtx->colACCount; i++) {
        col = colChkCtx->colAC[i];

        if (col == NULL) {
            continue;
        }
        if (col->acFlags & AC_NO_DAMAGE) {
            continue;
        }
        DataType_DamageCalc_FuncTbl[col->shape](play, colChkCtx, col);
    }
}

/**
 * Checks if the line ab intersects any of the ColliderJntSph's elements
 */
s32 CollisionCheck_OCLine_Vs_JntSph2(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider, Vec3f* a,
                                 Vec3f* b) {
    static Linef ls;
    ColliderJntSph* jntSph = (ColliderJntSph*)collider;
    s32 i;

    for (i = 0; i < jntSph->count; i++) {
        ColliderJntSphElement* element = &jntSph->elements[i];

        if (!(element->base.ocElemFlags & OCELEM_ON)) {
            continue;
        }
        ls.a = *a;
        ls.b = *b;
        if (Math3D_sphereCrossLineSegment(&element->dim.worldSphere, &ls) == true) {
            return true;
        }
    }
    return false;
}

/**
 * Checks if the line segment ab intersects the ColliderCylinder
 */
s32 CollisionCheck_OCLine_Vs_Pipe2(PlayState* play, CollisionCheckContext* colChkCtx, Collider* collider, Vec3f* a,
                              Vec3f* b) {
    static Vec3f cp1;
    static Vec3f cp2;
    ColliderCylinder* cylinder = (ColliderCylinder*)collider;

    if (!(cylinder->elem.ocElemFlags & OCELEM_ON)) {
        return false;
    }
    if (Math3D_pipeCrossLine(&cylinder->dim, a, b, &cp1, &cp2) != 0) {
        return true;
    }
    return false;
}

static ColChkLineFunc oc_line_collision_function2[COLSHAPE_MAX] = {
    CollisionCheck_OCLine_Vs_JntSph2,
    CollisionCheck_OCLine_Vs_Pipe2,
    NULL,
    NULL,
};

/**
 * Checks if the line segment ab intersects any OC colliders, excluding those attached to actors
 * on the exclusion list. Returns true if there are any intersections and false otherwise.
 */
s32 CollisionCheck_generalLineOcCheck(PlayState* play, CollisionCheckContext* colChkCtx, Vec3f* a, Vec3f* b, Actor** exclusions,
                          s32 numExclusions) {
    ColChkLineFunc lineCheck;
    Collider** col;
    s32 i;
    s32 exclude;
    s32 result = 0;

    for (col = colChkCtx->colOC; col < colChkCtx->colOC + colChkCtx->colOCCount; col++) {
        if (CollisionCheck_Check1ClObjNoOC(*col) == true) {
            continue;
        }
        exclude = false;
        for (i = 0; i < numExclusions; i++) {
            if ((*col)->actor == exclusions[i]) {
                exclude = true;
                break;
            }
        }
        if (exclude == true) {
            continue;
        }
        lineCheck = oc_line_collision_function2[(*col)->shape];
        if (lineCheck == NULL) {
            PRINTF(T("CollisionCheck_generalLineOcCheck():未対応 %dタイプ\n",
                     "CollisionCheck_generalLineOcCheck(): type %d not supported\n"),
                   (*col)->shape);
        } else {
            result = lineCheck(play, colChkCtx, (*col), a, b);
            if (result) {
                break;
            }
        }
    }
    return result;
}

/**
 * Checks if the line segment ab intersects any OC colliders. Returns true if there are any intersections and false
 * otherwise. Unused.
 */
s32 CollisionCheck_lineOcCheck(PlayState* play, CollisionCheckContext* colChkCtx, Vec3f* a, Vec3f* b) {
    return CollisionCheck_generalLineOcCheck(play, colChkCtx, a, b, NULL, 0);
}

/**
 * Checks if the line segment ab intersects any OC colliders, excluding those attached to actors on the exclusion list.
 * Returns true if there are any intersections and false otherwise.
 */
s32 CollisionCheck_lineOcCheck2(PlayState* play, CollisionCheckContext* colChkCtx, Vec3f* a, Vec3f* b,
                               Actor** exclusions, s32 numExclusions) {
    return CollisionCheck_generalLineOcCheck(play, colChkCtx, a, b, exclusions, numExclusions);
}

/**
 * Moves the ColliderCylinder's position to the actor's position
 */
void CollisionCheck_Uty_ActorWorldPosSetPipeC(Actor* actor, ColliderCylinder* cyl) {
    cyl->dim.pos.x = actor->world.pos.x;
    cyl->dim.pos.y = actor->world.pos.y;
    cyl->dim.pos.z = actor->world.pos.z;
}

/**
 * Sets the ColliderCylinder's position
 */
void CollisionCheck_Uty_SetPipeC_s(ColliderCylinder* cyl, Vec3s* pos) {
    cyl->dim.pos.x = pos->x;
    cyl->dim.pos.y = pos->y;
    cyl->dim.pos.z = pos->z;
}

/**
 * Sets the ColliderQuad's vertices
 */
void CollisionCheck_Uty_setSword4Pos(ColliderQuad* quad, Vec3f* a, Vec3f* b, Vec3f* c, Vec3f* d) {
    xyz_t_move(&quad->dim.quad[2], c);
    xyz_t_move(&quad->dim.quad[3], d);
    xyz_t_move(&quad->dim.quad[0], a);
    xyz_t_move(&quad->dim.quad[1], b);
    ClObjSwrdAttr_setup(&quad->dim);
}

/**
 * Sets the specified ColliderTrisElement's vertices
 */
void CollisionCheck_Uty_setTrisPos(ColliderTris* tris, s32 elemIndex, Vec3f* a, Vec3f* b, Vec3f* c) {
    ColliderTrisElement* trisElem = &tris->elements[elemIndex];
    f32 nx;
    f32 ny;
    f32 nz;
    f32 originDist;

    xyz_t_move(&trisElem->dim.vtx[0], a);
    xyz_t_move(&trisElem->dim.vtx[1], b);
    xyz_t_move(&trisElem->dim.vtx[2], c);
    Math3DPlane(a, b, c, &nx, &ny, &nz, &originDist);
    trisElem->dim.plane.normal.x = nx;
    trisElem->dim.plane.normal.y = ny;
    trisElem->dim.plane.normal.z = nz;
    trisElem->dim.plane.originDist = originDist;
}

/**
 * Sets the specified ColliderTrisElement's dim using the values in src
 */
void CollisionCheck_Uty_setTrisPos_ad(PlayState* play, ColliderTris* tris, s32 elemIndex, ColliderTrisElementDimInit* src) {
    ColliderTrisElement* trisElem = &tris->elements[elemIndex];

    ClObjTrisElemAttr_set(play, &trisElem->dim, src);
}

#if DEBUG_FEATURES
// The two static Vec3f variables in the function below cross a block index rollover, causing a bss order swap.
//! In order to replicate this behavior, we declare a certain amount of sBssDummy variables throughout the file, which
//! we fit inside padding added by the compiler between structs like TriNorm and/or Vec3f, so they don't take space in
//! bss.
#endif

/**
 * Updates the world spheres for all of the collider's JntSph elements attached to the specified limb
 */
void CollisionCheck_Uty_convJntSphL2G(s32 limb, ColliderJntSph* jntSph) {
    static Vec3f local;
    static Vec3f global; // bss ordering changes here
    s32 i;

    for (i = 0; i < jntSph->count; i++) {
        if (limb == jntSph->elements[i].dim.limb) {
            local.x = jntSph->elements[i].dim.modelSphere.center.x;
            local.y = jntSph->elements[i].dim.modelSphere.center.y;
            local.z = jntSph->elements[i].dim.modelSphere.center.z;
            Matrix_Position(&local, &global);
            jntSph->elements[i].dim.worldSphere.center.x = global.x;
            jntSph->elements[i].dim.worldSphere.center.y = global.y;
            jntSph->elements[i].dim.worldSphere.center.z = global.z;
            jntSph->elements[i].dim.worldSphere.radius =
                jntSph->elements[i].dim.modelSphere.radius * jntSph->elements[i].dim.scale;
        }
    }
}

/**
 * Spawns red blood droplets.
 * No actor has a collision type that spawns red blood.
 */
void CollisionCheckSetSpark(PlayState* play, Vec3f* v) {
    static EffectSparkInit spark;
    s32 effectIndex;

    spark.position.x = v->x;
    spark.position.y = v->y;
    spark.position.z = v->z;
    spark.uDiv = 5;
    spark.vDiv = 5;
    spark.colorStart[0].r = 128;
    spark.colorStart[0].g = 0;
    spark.colorStart[0].b = 64;
    spark.colorStart[0].a = 255;
    spark.colorStart[1].r = 128;
    spark.colorStart[1].g = 0;
    spark.colorStart[1].b = 64;
    spark.colorStart[1].a = 255;
    spark.colorStart[2].r = 255;
    spark.colorStart[2].g = 128;
    spark.colorStart[2].b = 0;
    spark.colorStart[2].a = 255;
    spark.colorStart[3].r = 255;
    spark.colorStart[3].g = 128;
    spark.colorStart[3].b = 0;
    spark.colorStart[3].a = 255;
    spark.colorEnd[0].r = 64;
    spark.colorEnd[0].g = 0;
    spark.colorEnd[0].b = 32;
    spark.colorEnd[0].a = 0;
    spark.colorEnd[1].r = 64;
    spark.colorEnd[1].g = 0;
    spark.colorEnd[1].b = 32;
    spark.colorEnd[1].a = 0;
    spark.colorEnd[2].r = 128;
    spark.colorEnd[2].g = 0;
    spark.colorEnd[2].b = 64;
    spark.colorEnd[2].a = 0;
    spark.colorEnd[3].r = 128;
    spark.colorEnd[3].g = 0;
    spark.colorEnd[3].b = 64;
    spark.colorEnd[3].a = 0;
    spark.timer = 0;
    spark.duration = 16;
    spark.speed = 8.0f;
    spark.gravity = -1.0f;

    EffectAdd(play, &effectIndex, EFFECT_SPARK, 0, 1, &spark);
}

/**
 * Spawns water droplets.
 * No actor has a collision type that spawns water droplets.
 */
void CollisionCheckSetWhiteBlood(PlayState* play, Vec3f* v) {
    static EffectSparkInit spark;
    s32 effectIndex;

    spark.position.x = v->x;
    spark.position.y = v->y;
    spark.position.z = v->z;
    spark.uDiv = 5;
    spark.vDiv = 5;
    spark.colorStart[0].r = 255;
    spark.colorStart[0].g = 255;
    spark.colorStart[0].b = 255;
    spark.colorStart[0].a = 255;
    spark.colorStart[1].r = 100;
    spark.colorStart[1].g = 100;
    spark.colorStart[1].b = 100;
    spark.colorStart[1].a = 100;
    spark.colorStart[2].r = 100;
    spark.colorStart[2].g = 100;
    spark.colorStart[2].b = 100;
    spark.colorStart[2].a = 100;
    spark.colorStart[3].r = 100;
    spark.colorStart[3].g = 100;
    spark.colorStart[3].b = 100;
    spark.colorStart[3].a = 100;
    spark.colorEnd[0].r = 50;
    spark.colorEnd[0].g = 50;
    spark.colorEnd[0].b = 50;
    spark.colorEnd[0].a = 50;
    spark.colorEnd[1].r = 50;
    spark.colorEnd[1].g = 50;
    spark.colorEnd[1].b = 50;
    spark.colorEnd[1].a = 50;
    spark.colorEnd[2].r = 50;
    spark.colorEnd[2].g = 50;
    spark.colorEnd[2].b = 50;
    spark.colorEnd[2].a = 50;
    spark.colorEnd[3].r = 0;
    spark.colorEnd[3].g = 0;
    spark.colorEnd[3].b = 0;
    spark.colorEnd[3].a = 0;
    spark.timer = 0;
    spark.duration = 16;
    spark.speed = 8.0f;
    spark.gravity = -1.0f;

    EffectAdd(play, &effectIndex, EFFECT_SPARK, 0, 1, &spark);
}

/**
 * Spawns streaks of light from hits against solid objects
 */
void CollisionCheckSetSparkFlashBlue_NoSE(PlayState* play, Vec3f* v) {
    static EffectShieldParticleInit spd = {
        16,
        { 0, 0, 0 },
        { 0, 200, 255, 255 },
        { 255, 255, 255, 255 },
        { 255, 255, 128, 255 },
        { 255, 255, 0, 255 },
        { 255, 64, 0, 200 },
        { 255, 0, 0, 255 },
        2.1f,
        35.0f,
        30.0f,
        8,
        { 0, 0, 0, { 0, 128, 255 }, false, 300 },
        true,
    };
    s32 effectIndex;

    spd.position.x = v->x;
    spd.position.y = v->y;
    spd.position.z = v->z;
    spd.lightPoint.x = spd.position.x;
    spd.lightPoint.y = spd.position.y;
    spd.lightPoint.z = spd.position.z;

    EffectAdd(play, &effectIndex, EFFECT_SHIELD_PARTICLE, 0, 1, &spd);
}

/**
 * Spawns streaks of light and plays a metallic sound effect
 */
void CollisionCheckSetSparkFlashBlue(PlayState* play, Vec3f* v) {
    CollisionCheckSetSparkFlashBlue_NoSE(play, v);
    Nai_FxFlagEntry(NA_SE_IT_SHIELD_REFLECT_SW, &_dummy_zero_f, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
}

/**
 * Spawns streaks of light and plays a metallic sound effect at the specified position
 */
void CollisionCheckSetSparkFlashBlue_SeCamPos(PlayState* play, Vec3f* v, Vec3f* pos) {
    CollisionCheckSetSparkFlashBlue_NoSE(play, v);
    Nai_FxFlagEntry(NA_SE_IT_SHIELD_REFLECT_SW, pos, 4, &_dummy_one, &_dummy_one,
                         &_dummy_zero_s8);
}

/**
 * Spawns streaks of light and plays a metallic sound effect
 */
void CollisionCheckSetSparkFlash(PlayState* play, Vec3f* v) {
    CollisionCheckSetSparkFlashBlue(play, v);
}

/**
 * Spawns streaks of light and plays a wooden sound effect
 */
void CollisionCheckSetWoodParticle(PlayState* play, Vec3f* v, Vec3f* actorPos) {
    static EffectShieldParticleInit spd = {
        16,
        { 0, 0, 0 },
        { 0, 200, 255, 255 },
        { 255, 255, 255, 255 },
        { 255, 255, 128, 255 },
        { 255, 255, 0, 255 },
        { 255, 64, 0, 200 },
        { 255, 0, 0, 255 },
        2.1f,
        35.0f,
        30.0f,
        8,
        { 0, 0, 0, { 0, 128, 255 }, false, 300 },
        false,
    };
    s32 effectIndex;

    spd.position.x = v->x;
    spd.position.y = v->y;
    spd.position.z = v->z;
    spd.lightPoint.x = spd.position.x;
    spd.lightPoint.y = spd.position.y;
    spd.lightPoint.z = spd.position.z;

    EffectAdd(play, &effectIndex, EFFECT_SHIELD_PARTICLE, 0, 1, &spd);
    Nai_FxFlagEntry(NA_SE_IT_REFLECTION_WOOD, actorPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
}

/**
 * Determines if the line segment connecting itemPos and itemProjPos intersects the side of a cylinder with the given
 * radius, height, and offset at actorPos. Returns 3 if either endpoint is inside the cylinder, otherwise returns the
 * number of points of intersection with the side of the cylinder. The locations of those points are put in out1 and
 * out2, with out1 being closer to itemPos. Line segments that pass through both bases of the cylinder are not detected.
 */
s32 CollisionCheckPipeVsLine2(f32 radius, f32 height, f32 offset, Vec3f* actorPos, Vec3f* itemPos,
                                    Vec3f* itemProjPos, Vec3f* out1, Vec3f* out2) {
    Vec3f actorToItem;
    Vec3f actorToItemProj;
    Vec3f itemStep;
    f32 frac1;
    f32 frac2;
    u32 intersect2;
    u32 intersect1;
    u32 test1;
    u32 test2;
    f32 radSqDiff;
    f32 actorDotItemXZ;
    f32 zero = 0.0f;
    f32 closeDist;
    s32 pad1;
    s32 pad2;

    actorToItem.x = itemPos->x - actorPos->x;
    actorToItem.y = itemPos->y - actorPos->y - offset;
    actorToItem.z = itemPos->z - actorPos->z;

    actorToItemProj.x = itemProjPos->x - actorPos->x;
    actorToItemProj.y = itemProjPos->y - actorPos->y - offset;
    actorToItemProj.z = itemProjPos->z - actorPos->z;

    itemStep.x = actorToItemProj.x - actorToItem.x;
    itemStep.y = actorToItemProj.y - actorToItem.y;
    itemStep.z = actorToItemProj.z - actorToItem.z;

    if ((actorToItem.y > 0.0f) && (actorToItem.y < height) && (sqrtf(SQXZ(actorToItem)) < radius)) {
        return 3;
    }

    if ((actorToItemProj.y > 0.0f) && (actorToItemProj.y < height) && (sqrtf(SQXZ(actorToItemProj)) < radius)) {
        return 3;
    }
    radSqDiff = SQXZ(actorToItem) - SQ(radius);
    if (!IS_ZERO(SQXZ(itemStep))) {
        actorDotItemXZ = (2.0f * itemStep.x * actorToItem.x) + (2.0f * itemStep.z * actorToItem.z);
        if (SQ(actorDotItemXZ) < (4.0f * SQXZ(itemStep) * radSqDiff)) {
            return 0;
        }
        if (SQ(actorDotItemXZ) - (4.0f * SQXZ(itemStep) * radSqDiff) > zero) {
            intersect1 = intersect2 = true;
        } else {
            intersect1 = true;
            intersect2 = false;
        }
        closeDist = sqrtf(SQ(actorDotItemXZ) - (4.0f * SQXZ(itemStep) * radSqDiff));
        if (intersect1 == true) {
            frac1 = (closeDist - actorDotItemXZ) / (2.0f * SQXZ(itemStep));
        }
        if (intersect2 == true) {
            frac2 = (-actorDotItemXZ - closeDist) / (2.0f * SQXZ(itemStep));
        }
    } else if (!IS_ZERO((2.0f * itemStep.x * actorToItem.x) + (2.0f * itemStep.z * actorToItem.z))) {
        intersect1 = true;
        intersect2 = false;
        frac1 = -radSqDiff / ((2.0f * itemStep.x * actorToItem.x) + (2.0f * itemStep.z * actorToItem.z));
    } else {
        if (radSqDiff <= 0.0f) {
            test1 = (0.0f < actorToItem.y) && (actorToItem.y < height);
            test2 = (0.0f < actorToItemProj.y) && (actorToItemProj.y < height);

            if (test1 && test2) {
                *out1 = actorToItem;
                *out2 = actorToItemProj;
                return 2;
            }
            if (test1) {
                *out1 = actorToItem;
                return 1;
            }
            if (test2) {
                *out1 = actorToItemProj;
                return 1;
            }
        }
        return 0;
    }

    if (!intersect2) {
        if (frac1 < 0.0f || 1.0f < frac1) {
            return 0;
        }
    } else {
        test1 = (frac1 < 0.0f || 1.0f < frac1);
        test2 = (frac2 < 0.0f || 1.0f < frac2);

        if (test1 && test2) {
            return 0;
        }
        if (test1) {
            intersect1 = false;
        }
        if (test2) {
            intersect2 = false;
        }
    }

    if ((intersect1 == true) &&
        ((frac1 * itemStep.y + actorToItem.y < 0.0f) || (height < frac1 * itemStep.y + actorToItem.y))) {
        intersect1 = false;
    }
    if ((intersect2 == true) &&
        ((frac2 * itemStep.y + actorToItem.y < 0.0f) || (height < frac2 * itemStep.y + actorToItem.y))) {
        intersect2 = false;
    }
    if (!intersect1 && !intersect2) {
        return 0;
    } else if ((intersect1 == true) && (intersect2 == true)) {
        out1->x = frac1 * itemStep.x + actorToItem.x + actorPos->x;
        out1->y = frac1 * itemStep.y + actorToItem.y + actorPos->y;
        out1->z = frac1 * itemStep.z + actorToItem.z + actorPos->z;
        out2->x = frac2 * itemStep.x + actorToItem.x + actorPos->x;
        out2->y = frac2 * itemStep.y + actorToItem.y + actorPos->y;
        out2->z = frac2 * itemStep.z + actorToItem.z + actorPos->z;
        return 2;
    } else if (intersect1 == true) {
        out1->x = frac1 * itemStep.x + actorToItem.x + actorPos->x;
        out1->y = frac1 * itemStep.y + actorToItem.y + actorPos->y;
        out1->z = frac1 * itemStep.z + actorToItem.z + actorPos->z;
        return 1;
    } else if (intersect2 == true) {
        out1->x = frac2 * itemStep.x + actorToItem.x + actorPos->x;
        out1->y = frac2 * itemStep.y + actorToItem.y + actorPos->y;
        out1->z = frac2 * itemStep.z + actorToItem.z + actorPos->z;
        return 1;
    }
    return 1;
}

/**
 * Gets damage from a sword strike using generic values, and returns 0 if the attack is
 * not sword-type. Used by bosses to require that a sword attack deal the killing blow.
 */
u8 GetSwordAP(s32 dmgFlags) {
    u8 damage = 0;

    if (dmgFlags & (DMG_SPIN_KOKIRI | DMG_SLASH_KOKIRI)) {
        damage = 1;
    } else if (dmgFlags & (DMG_JUMP_KOKIRI | DMG_SPIN_MASTER | DMG_SLASH_MASTER | DMG_HAMMER_SWING | DMG_DEKU_STICK)) {
        damage = 2;
    } else if (dmgFlags & (DMG_HAMMER_JUMP | DMG_JUMP_MASTER | DMG_SPIN_GIANT | DMG_SLASH_GIANT)) {
        damage = 4;
    } else if (dmgFlags & DMG_JUMP_GIANT) {
        damage = 8;
    }

#if DEBUG_FEATURES
    KREG(7) = damage;
#endif

    return damage;
}
