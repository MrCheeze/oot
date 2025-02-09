void Actor_Noop(Actor* actor, PlayState* play) {
}

s32 func_80035124(Actor* actor, PlayState* play) {
    s32 ret = 0;

    switch (actor->params) {
        case 0:
            if (Actor_HasParent(actor, play)) {
                actor->params = 1;
            } else if (!(actor->bgCheckFlags & BGCHECKFLAG_GROUND)) {
                Actor_MoveXZGravity(actor);
                Math_SmoothStepToF(&actor->speed, 0.0f, 1.0f, 0.1f, 0.0f);
            } else if ((actor->bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) && (actor->velocity.y < -4.0f)) {
                ret = 1;
            } else {
                actor->shape.rot.x = actor->shape.rot.z = 0;
                Actor_OfferCarry(actor, play);
            }
            break;
        case 1:
            if (Actor_HasNoParent(actor, play)) {
                actor->params = 0;
            }
            break;
    }

    Actor_UpdateBgCheckInfo(
        play, actor, actor->colChkInfo.cylHeight, actor->colChkInfo.cylRadius, actor->colChkInfo.cylRadius,
        UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);

    return ret;
}

#include "global.h"

void Gfx_DrawDListOpa(PlayState* play, Gfx* dlist) {
    OPEN_DISPS(play->state.gfxCtx, "../z_cheap_proc.c", 214);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_cheap_proc.c", 216);
    gSPDisplayList(POLY_OPA_DISP++, dlist);

    CLOSE_DISPS(play->state.gfxCtx, "../z_cheap_proc.c", 219);
}

void Gfx_DrawDListXlu(PlayState* play, Gfx* dlist) {
    OPEN_DISPS(play->state.gfxCtx, "../z_cheap_proc.c", 228);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_cheap_proc.c", 230);
    gSPDisplayList(POLY_XLU_DISP++, dlist);

    CLOSE_DISPS(play->state.gfxCtx, "../z_cheap_proc.c", 233);
}

u8 func_800353E8(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return player->unk_845;
}

/**
 * Finds the first actor instance of a specified ID and category within a given range from
 * an actor if there is one. If the ID provided is -1, this will look for any actor of the
 * specified category rather than a specific ID.
 */
Actor* Actor_FindNearby(PlayState* play, Actor* refActor, s16 actorId, u8 actorCategory, f32 range) {
    Actor* actor = play->actorCtx.actorLists[actorCategory].head;

    while (actor != NULL) {
        if (actor == refActor || ((actorId != -1) && (actor->id != actorId))) {
            actor = actor->next;
        } else {
            if (Actor_WorldDistXYZToActor(refActor, actor) <= range) {
                return actor;
            } else {
                actor = actor->next;
            }
        }
    }

    return NULL;
}

s32 func_800354B4(PlayState* play, Actor* actor, f32 range, s16 arg3, s16 arg4, s16 arg5) {
    Player* player = GET_PLAYER(play);
    s16 var1;
    s16 var2;

    var1 = (s16)(actor->yawTowardsPlayer + 0x8000) - player->actor.shape.rot.y;
    var2 = actor->yawTowardsPlayer - arg5;

    if ((actor->xzDistToPlayer <= range) && (player->meleeWeaponState != 0) && (arg4 >= ABS(var1)) &&
        (arg3 >= ABS(var2))) {
        return true;
    } else {
        return false;
    }
}

void func_8003555C(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel) {
    Color_RGBA8 color1;
    Color_RGBA8 color2;

    color1.r = 200;
    color1.g = 160;
    color1.b = 120;

    color2.r = 130;
    color2.g = 90;
    color2.b = 50;

    //! @bug color1 and color2 alpha components not set before being passed on
    EffectSsKiraKira_SpawnSmall(play, pos, velocity, accel, &color1, &color2);
}

Vec3f D_80116268 = { 0.0f, -1.5f, 0.0f };
Vec3f D_80116274 = { 0.0f, -0.2f, 0.0f };

Gfx D_80116280[] = {
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU |
                                            FORCE_BL | GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)),
    gsDPSetAlphaCompare(G_AC_THRESHOLD),
    gsSPEndDisplayList(),
};

void func_800355B8(PlayState* play, Vec3f* pos) {
    func_8003555C(play, pos, &D_80116268, &D_80116274);
}

u8 func_800355E4(PlayState* play, Collider* collider) {
    Player* player = GET_PLAYER(play);

    if ((collider->acFlags & AC_TYPE_PLAYER) && (player->meleeWeaponState != 0) &&
        (player->meleeWeaponAnimation == PLAYER_MWA_HAMMER_FORWARD)) {
        return true;
    } else {
        return false;
    }
}

u8 Actor_ApplyDamage(Actor* actor) {
    if (actor->colChkInfo.health <= actor->colChkInfo.damage) {
        actor->colChkInfo.health = 0;
    } else {
        actor->colChkInfo.health -= actor->colChkInfo.damage;
    }

    return actor->colChkInfo.health;
}

void Actor_SetDropFlag(Actor* actor, ColliderElement* elem, s32 freezeFlag) {
    ColliderElement* acHitElem = elem->acHitElem;

    if (acHitElem == NULL) {
        actor->dropFlag = 0x00;
    } else if (freezeFlag && (acHitElem->atDmgInfo.dmgFlags & (DMG_UNKNOWN_1 | DMG_MAGIC_ICE | DMG_MAGIC_FIRE))) {
        actor->freezeTimer = acHitElem->atDmgInfo.damage;
        actor->dropFlag = 0x00;
    } else if (acHitElem->atDmgInfo.dmgFlags & DMG_ARROW_FIRE) {
        actor->dropFlag = 0x01;
    } else if (acHitElem->atDmgInfo.dmgFlags & DMG_ARROW_ICE) {
        actor->dropFlag = 0x02;
    } else if (acHitElem->atDmgInfo.dmgFlags & DMG_ARROW_UNK1) {
        actor->dropFlag = 0x04;
    } else if (acHitElem->atDmgInfo.dmgFlags & DMG_ARROW_UNK2) {
        actor->dropFlag = 0x08;
    } else if (acHitElem->atDmgInfo.dmgFlags & DMG_ARROW_UNK3) {
        actor->dropFlag = 0x10;
    } else if (acHitElem->atDmgInfo.dmgFlags & DMG_ARROW_LIGHT) {
        actor->dropFlag = 0x20;
    } else if (acHitElem->atDmgInfo.dmgFlags & DMG_MAGIC_LIGHT) {
        if (freezeFlag) {
            actor->freezeTimer = acHitElem->atDmgInfo.damage;
        }
        actor->dropFlag = 0x40;
    } else {
        actor->dropFlag = 0x00;
    }
}

void Actor_SetDropFlagJntSph(Actor* actor, ColliderJntSph* jntSph, s32 freezeFlag) {
    ColliderElement* elem;
    ColliderElement* acHitElem;
    s32 flag;
    s32 i;

    actor->dropFlag = 0x00;

    for (i = jntSph->count - 1; i >= 0; i--) {
        elem = &jntSph->elements[i].base;
        acHitElem = elem->acHitElem;
        if (acHitElem == NULL) {
            flag = 0x00;
        } else if (freezeFlag && (acHitElem->atDmgInfo.dmgFlags & (DMG_UNKNOWN_1 | DMG_MAGIC_ICE | DMG_MAGIC_FIRE))) {
            actor->freezeTimer = acHitElem->atDmgInfo.damage;
            flag = 0x00;
        } else if (acHitElem->atDmgInfo.dmgFlags & DMG_ARROW_FIRE) {
            flag = 0x01;
        } else if (acHitElem->atDmgInfo.dmgFlags & DMG_ARROW_ICE) {
            flag = 0x02;
        } else if (acHitElem->atDmgInfo.dmgFlags & DMG_ARROW_UNK1) {
            flag = 0x04;
        } else if (acHitElem->atDmgInfo.dmgFlags & DMG_ARROW_UNK2) {
            flag = 0x08;
        } else if (acHitElem->atDmgInfo.dmgFlags & DMG_ARROW_UNK3) {
            flag = 0x10;
        } else if (acHitElem->atDmgInfo.dmgFlags & DMG_ARROW_LIGHT) {
            flag = 0x20;
        } else if (acHitElem->atDmgInfo.dmgFlags & DMG_MAGIC_LIGHT) {
            if (freezeFlag) {
                actor->freezeTimer = acHitElem->atDmgInfo.damage;
            }
            flag = 0x40;
        } else {
            flag = 0x00;
        }
        actor->dropFlag |= flag;
    }
}

void func_80035844(Vec3f* arg0, Vec3f* arg1, Vec3s* arg2, s32 arg3) {
    f32 dx = arg1->x - arg0->x;
    f32 dz = arg1->z - arg0->z;
    f32 dy = arg3 ? (arg1->y - arg0->y) : (arg0->y - arg1->y);

    arg2->y = Math_Atan2S(dz, dx);
    arg2->x = Math_Atan2S(sqrtf(SQ(dx) + SQ(dz)), dy);
}

/**
 * Spawns En_Part (Dissipating Flames) actor as a child of the given actor.
 */
Actor* func_800358DC(Actor* actor, Vec3f* spawnPos, Vec3s* spawnRot, f32* arg3, s32 timer, s16* unused, PlayState* play,
                     s16 params, Gfx* dList) {
    EnPart* spawnedEnPart;

    spawnedEnPart = (EnPart*)Actor_SpawnAsChild(&play->actorCtx, actor, play, ACTOR_EN_PART, spawnPos->x, spawnPos->y,
                                                spawnPos->z, spawnRot->x, spawnRot->y, actor->objectSlot, params);
    if (spawnedEnPart != NULL) {
        spawnedEnPart->actor.scale = actor->scale;
        spawnedEnPart->actor.speed = arg3[0];
        spawnedEnPart->displayList = dList;
        spawnedEnPart->action = 2;
        spawnedEnPart->timer = timer;
        spawnedEnPart->rotZ = arg3[1];
        spawnedEnPart->rotZSpeed = arg3[2];
        return &spawnedEnPart->actor;
    }

    return NULL;
}

void func_800359B8(Actor* actor, s16 arg1, Vec3s* arg2) {
    if (actor->floorPoly != NULL) {
        f32 floorPolyNormalX;
        f32 floorPolyNormalY;
        f32 floorPolyNormalZ;
        f32 sp38;
        f32 sp34;
        f32 sp30;
        f32 sp2C;
        f32 sp28;
        f32 sp24;
        CollisionPoly* floorPoly;
        s32 pad;

        floorPoly = actor->floorPoly;
        floorPolyNormalX = COLPOLY_GET_NORMAL(floorPoly->normal.x);
        floorPolyNormalY = COLPOLY_GET_NORMAL(floorPoly->normal.y);
        floorPolyNormalZ = COLPOLY_GET_NORMAL(floorPoly->normal.z);

        sp38 = Math_SinS(arg1);
        sp34 = Math_CosS(arg1);
        sp28 = (-(floorPolyNormalX * sp38) - (floorPolyNormalZ * sp34));
        arg2->x = -RAD_TO_BINANG(Math_FAtan2F(sp28 * floorPolyNormalY, 1.0f));

        sp2C = Math_SinS(arg1 - 16375);
        sp30 = Math_CosS(arg1 - 16375);
        sp24 = (-(floorPolyNormalX * sp2C) - (floorPolyNormalZ * sp30));
        arg2->z = -RAD_TO_BINANG(Math_FAtan2F(sp24 * floorPolyNormalY, 1.0f));
    }
}
