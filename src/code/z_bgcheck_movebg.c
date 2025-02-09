#include "global.h"

/**
 * @param transformFlags How other actors standing on the dynapoly actor's collision move when the dynapoly actor moves.
 *   See `DYNA_TRANSFORM_POS`, `DYNA_TRANSFORM_ROT_Y`.
 */
void MoveBG_ct(DynaPolyActor* dynaActor, s32 transformFlags) {
    dynaActor->bgId = -1;
    dynaActor->unk_150 = 0.0f;
    dynaActor->unk_154 = 0.0f;
    dynaActor->transformFlags = transformFlags;
    dynaActor->interactFlags = 0;
}

void MoveBG_clearStatus(DynaPolyActor* dynaActor) {
    dynaActor->interactFlags = 0;
}

void MoveBG_setRideStatus(DynaPolyActor* dynaActor) {
    dynaActor->interactFlags |= DYNA_INTERACT_ACTOR_ON_TOP;
}

void MoveBG_setRidePlayerStatus(DynaPolyActor* dynaActor) {
    dynaActor->interactFlags |= DYNA_INTERACT_PLAYER_ON_TOP;
}

void MoveBG_setRidePlayerStatus_ai(CollisionContext* colCtx, s32 floorBgId) {
    DynaPolyActor* dynaActor = DynaPolyInfo_actor_index2pointer(colCtx, floorBgId);

    if (dynaActor != NULL) {
        MoveBG_setRidePlayerStatus(dynaActor);
    }
}

void MoveBG_setOverPlayerStatus(DynaPolyActor* dynaActor) {
    dynaActor->interactFlags |= DYNA_INTERACT_PLAYER_ABOVE;
}

void MoveBG_setOverPlayerStatus_ai(CollisionContext* colCtx, s32 floorBgId) {
    DynaPolyActor* dynaActor = DynaPolyInfo_actor_index2pointer(colCtx, floorBgId);

    if (dynaActor != NULL) {
        MoveBG_setOverPlayerStatus(dynaActor);
    }
}

void MoveBG_setSwOnStatus(DynaPolyActor* dynaActor) {
    dynaActor->interactFlags |= DYNA_INTERACT_ACTOR_SWITCH_PRESSED;
}

s32 MoveBG_checkRideStatus(DynaPolyActor* dynaActor) {
    if (dynaActor->interactFlags & DYNA_INTERACT_ACTOR_ON_TOP) {
        return true;
    } else {
        return false;
    }
}

s32 MoveBG_checkRidePlayerStatus(DynaPolyActor* dynaActor) {
    if (dynaActor->interactFlags & DYNA_INTERACT_PLAYER_ON_TOP) {
        return true;
    } else {
        return false;
    }
}

s32 MoveBG_checkOverPlayerStatus(DynaPolyActor* dynaActor) {
    if (dynaActor->interactFlags & DYNA_INTERACT_PLAYER_ABOVE) {
        return true;
    } else {
        return false;
    }
}

s32 MoveBG_checkSwOnStatus(DynaPolyActor* dynaActor) {
    if (dynaActor->interactFlags & DYNA_INTERACT_ACTOR_SWITCH_PRESSED) {
        return true;
    } else {
        return false;
    }
}

s32 MoveBG_checkMoveArea(PlayState* play, DynaPolyActor* dynaActor, s16 arg2, s16 arg3, s16 arg4) {
    Vec3f posA;
    Vec3f posB;
    Vec3f posResult;
    f32 sin = sin_s(dynaActor->unk_158);
    f32 cos = cos_s(dynaActor->unk_158);
    s32 bgId;
    CollisionPoly* poly;
    f32 a2;
    f32 a3;
    f32 sign = (0.0f <= dynaActor->unk_150) ? 1.0f : -1.0f;

    a2 = (f32)arg2 - 0.1f;
    posA.x = dynaActor->actor.world.pos.x + (a2 * cos);
    posA.y = dynaActor->actor.world.pos.y + arg4;
    posA.z = dynaActor->actor.world.pos.z - (a2 * sin);

    a3 = (f32)arg3 - 0.1f;
    posB.x = sign * a3 * sin + posA.x;
    posB.y = posA.y;
    posB.z = sign * a3 * cos + posA.z;

    if (T_BGCheck_ObjLineCheck_poly_chgrp_aiac_skp(&play->colCtx, &posA, &posB, &posResult, &poly, true, false, false, true, &bgId,
                                &dynaActor->actor, 0.0f)) {
        return false;
    }
    posA.x = (dynaActor->actor.world.pos.x * 2) - posA.x;
    posA.z = (dynaActor->actor.world.pos.z * 2) - posA.z;
    posB.x = sign * a3 * sin + posA.x;
    posB.z = sign * a3 * cos + posA.z;
    if (T_BGCheck_ObjLineCheck_poly_chgrp_aiac_skp(&play->colCtx, &posA, &posB, &posResult, &poly, true, false, false, true, &bgId,
                                &dynaActor->actor, 0.0f)) {
        return false;
    }
    return true;
}
