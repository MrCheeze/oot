#include "global.h"
#include "terminal.h"

/**
 * Update the `carriedActor`'s position based on the dynapoly actor identified by `bgId`.
 */
void BGCheckCollection_typicalActorPos(CollisionContext* colCtx, s32 bgId, Actor* carriedActor) {
    MtxF prevTransform;
    MtxF prevTransformInv;
    MtxF curTransform;
    Vec3f pos;
    Vec3f tempPos;

    if (DynaPolyInfo_checkMoveBG(bgId)) {

        Skin_Matrix_SetSRzxyTMatrix(
            &prevTransform, colCtx->dyna.bgActors[bgId].prevTransform.scale.x,
            colCtx->dyna.bgActors[bgId].prevTransform.scale.y, colCtx->dyna.bgActors[bgId].prevTransform.scale.z,
            colCtx->dyna.bgActors[bgId].prevTransform.rot.x, colCtx->dyna.bgActors[bgId].prevTransform.rot.y,
            colCtx->dyna.bgActors[bgId].prevTransform.rot.z, colCtx->dyna.bgActors[bgId].prevTransform.pos.x,
            colCtx->dyna.bgActors[bgId].prevTransform.pos.y, colCtx->dyna.bgActors[bgId].prevTransform.pos.z);

        if (Skin_Matrix_InverseMatrix(&prevTransform, &prevTransformInv) != 2) {

            Skin_Matrix_SetSRzxyTMatrix(
                &curTransform, colCtx->dyna.bgActors[bgId].curTransform.scale.x,
                colCtx->dyna.bgActors[bgId].curTransform.scale.y, colCtx->dyna.bgActors[bgId].curTransform.scale.z,
                colCtx->dyna.bgActors[bgId].curTransform.rot.x, colCtx->dyna.bgActors[bgId].curTransform.rot.y,
                colCtx->dyna.bgActors[bgId].curTransform.rot.z, colCtx->dyna.bgActors[bgId].curTransform.pos.x,
                colCtx->dyna.bgActors[bgId].curTransform.pos.y, colCtx->dyna.bgActors[bgId].curTransform.pos.z);

            // Apply the movement of the dynapoly actor `bgId` over the last frame to the `carriedActor` position
            // pos = curTransform * prevTransformInv * pos
            // Note (curTransform * prevTransformInv) represents the transform relative to the previous frame
            Skin_Matrix_MulVector(&prevTransformInv, &carriedActor->world.pos, &tempPos);
            Skin_Matrix_MulVector(&curTransform, &tempPos, &pos);
            carriedActor->world.pos = pos;

#if DEBUG_FEATURES
            if (BGCHECK_XYZ_ABSMAX <= pos.x || pos.x <= -BGCHECK_XYZ_ABSMAX || BGCHECK_XYZ_ABSMAX <= pos.y ||
                pos.y <= -BGCHECK_XYZ_ABSMAX || BGCHECK_XYZ_ABSMAX <= pos.z || pos.z <= -BGCHECK_XYZ_ABSMAX) {

                PRINTF_COLOR_RED();
                //! @bug file and line are not passed to PRINTF
                PRINTF(T("BGCheckCollection_typicalActorPos():位置が妥当ではありません。\n"
                         "pos (%f,%f,%f) file:%s line:%d\n",
                         "BGCheckCollection_typicalActorPos(): Position is not valid. \n"
                         "pos (%f,%f,%f) file:%s line:%d\n"),
                       pos.x, pos.y, pos.z);
                PRINTF_RST();
            }
#endif
        }
    }
}

/**
 * Update the `carriedActor`'s Y rotation based on the dynapoly actor identified by `bgId`.
 */
void BGCheckCollection_typicalActorRotY(CollisionContext* colCtx, s32 bgId, Actor* carriedActor) {
    if (DynaPolyInfo_checkMoveBG(bgId)) {
        s16 rotY = colCtx->dyna.bgActors[bgId].curTransform.rot.y - colCtx->dyna.bgActors[bgId].prevTransform.rot.y;

        if (carriedActor->id == ACTOR_PLAYER) {
            ((Player*)carriedActor)->yaw += rotY;
        }

        carriedActor->shape.rot.y += rotY;
        carriedActor->world.rot.y += rotY;
    }
}

void BGCheckCollection_setRideStatus(CollisionContext* colCtx, Actor* actor, s32 bgId) {
    if (DynaPolyInfo_checkMoveBG(bgId)) {
        DynaPolyActor* dynaActor = DynaPolyInfo_actor_index2pointer(colCtx, bgId);
        if (dynaActor != NULL) {
            MoveBG_setRideStatus(dynaActor);

            if (CHECK_FLAG_ALL(actor->flags, ACTOR_FLAG_CAN_PRESS_SWITCHES)) {
                MoveBG_setSwOnStatus(dynaActor);
            }
        }
    }
}

/**
 * Update the `carriedActor`'s position and Y rotation based on the dynapoly actor identified by `bgId`, according to
 * the dynapoly actor's move flags (see `DYNA_TRANSFORM_POS` and `DYNA_TRANSFORM_ROT_Y`).
 */
s32 BGCheckCollection_proc(CollisionContext* colCtx, s32 bgId, Actor* carriedActor) {
    s32 result = false;
    DynaPolyActor* dynaActor;

    if (!DynaPolyInfo_checkMoveBG(bgId)) {
        return false;
    }

    if ((colCtx->dyna.bgActorFlags[bgId] & BGACTOR_1) || !(colCtx->dyna.bgActorFlags[bgId] & BGACTOR_IN_USE)) {
        return false;
    }

    dynaActor = DynaPolyInfo_actor_index2pointer(colCtx, bgId);

    if (dynaActor == NULL) {
        return false;
    }

    if (dynaActor->transformFlags & DYNA_TRANSFORM_POS) {
        BGCheckCollection_typicalActorPos(colCtx, bgId, carriedActor);
        result = true;
    }

    if (dynaActor->transformFlags & DYNA_TRANSFORM_ROT_Y) {
        BGCheckCollection_typicalActorRotY(colCtx, bgId, carriedActor);
        result = true;
    }

    return result;
}
