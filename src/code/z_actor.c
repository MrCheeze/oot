#include "global.h"
#include "fault.h"
#include "quake.h"
#include "rand.h"
#include "terminal.h"
#include "versions.h"
#include "z64horse.h"

#include "overlays/actors/ovl_Arms_Hook/z_arms_hook.h"
#include "overlays/actors/ovl_En_Part/z_en_part.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "assets/objects/object_bdoor/object_bdoor.h"

#pragma increment_block_number "gc-eu:128 gc-eu-mq:128 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128" \
                               "ntsc-1.0:0 ntsc-1.1:0 ntsc-1.2:0 pal-1.0:0 pal-1.1:0 hiratsu3:128"

CollisionPoly* roof_polygon_info;
s32 roof_pbg_actor_index;

#if DEBUG_FEATURES
#define ACTOR_DEBUG_PRINTF           \
    if (R_ENABLE_ACTOR_DEBUG_PRINTF) \
    PRINTF
#else
#define ACTOR_DEBUG_PRINTF \
    if (0)                 \
    PRINTF
#endif

#if DEBUG_FEATURES
#define ACTOR_COLOR_WARNING VT_COL(YELLOW, BLACK)
#define ACTOR_COLOR_ERROR VT_COL(RED, WHITE)
#define ACTOR_RST VT_RST
#else
#define ACTOR_COLOR_WARNING ""
#define ACTOR_COLOR_ERROR ""
#define ACTOR_RST ""
#endif

void Shape_Info_init(ActorShape* shape, f32 yOffset, ActorShadowFunc shadowDraw, f32 shadowScale) {
    shape->yOffset = yOffset;
    shape->shadowDraw = shadowDraw;
    shape->shadowScale = shadowScale;
    shape->shadowAlpha = 255;
}

void Actor_shadow_draw(Actor* actor, Lights* lights, PlayState* play, Gfx* dlist, Color_RGBA8* color) {
    f32 temp1;
    f32 temp2;
    MtxF sp60;

    if (actor->floorPoly == NULL) {
        return;
    }

    temp1 = actor->world.pos.y - actor->floorHeight;

    if (temp1 >= -50.0f && temp1 < 500.0f) {
        OPEN_DISPS(play->state.gfxCtx, "../z_actor.c", 1553);

        POLY_OPA_DISP = rcp_mode_set(POLY_OPA_DISP, SETUPDL_44);

        gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED, 0, 0, 0,
                          COMBINED);

        temp1 = (temp1 < 0.0f) ? 0.0f : ((temp1 > 150.0f) ? 150.0f : temp1);
        temp2 = 1.0f - (temp1 * (1.0f / 350));

        if (color != NULL) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, color->r, color->g, color->b,
                            (u32)(actor->shape.shadowAlpha * temp2) & 0xFF);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, (u32)(actor->shape.shadowAlpha * temp2) & 0xFF);
        }

        T_Polygon_Ground_Matrix(actor->floorPoly, actor->world.pos.x, actor->floorHeight, actor->world.pos.z, &sp60);
        Matrix_put(&sp60);

        if (dlist != gCircleShadowDL) {
            Matrix_rotateY(BINANG_TO_RAD(actor->shape.rot.y), MTXMODE_APPLY);
        }

        temp2 = (1.0f - (temp1 * (1.0f / 350)));
        temp2 *= actor->shape.shadowScale;
        Matrix_scale(temp2 * actor->scale.x, 1.0f, temp2 * actor->scale.z, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_actor.c", 1588);
        gSPDisplayList(POLY_OPA_DISP++, dlist);

        CLOSE_DISPS(play->state.gfxCtx, "../z_actor.c", 1594);
    }
}

void Actor_shadow_circle(Actor* actor, Lights* lights, PlayState* play) {
    Actor_shadow_draw(actor, lights, play, gCircleShadowDL, NULL);
}

void Actor_shadow_white_circle(Actor* actor, Lights* lights, PlayState* play) {
    static Color_RGBA8 color = { 255, 255, 255, 255 };

    Actor_shadow_draw(actor, lights, play, gCircleShadowDL, &color);
}

void Actor_shadow_horse(Actor* actor, Lights* lights, PlayState* play) {
    Actor_shadow_draw(actor, lights, play, gHorseShadowDL, NULL);
}

void shadow_foot_draw(PlayState* play, Light* light, MtxF* arg2, s32 arg3, f32 arg4, f32 arg5, f32 arg6) {
    s32 pad1;
    f32 sp58;
    f32 temp;
    s32 pad2;

    OPEN_DISPS(play->state.gfxCtx, "../z_actor.c", 1661);

    temp = arg3 * 0.00005f;
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, (u32)(arg4 * (temp > 1.0f ? 1.0f : temp)) & 0xFF);

    sp58 = fatan2(light->l.dir[0], light->l.dir[2]);
    arg6 *= (4.5f - (light->l.dir[1] * 0.035f));
    if (arg6 < 1.0f) {
        arg6 = 1.0f;
    }
    Matrix_put(arg2);
    Matrix_rotateY(sp58, MTXMODE_APPLY);
    Matrix_scale(arg5, 1.0f, arg5 * arg6, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_actor.c", 1687);
    gSPDisplayList(POLY_OPA_DISP++, gFootShadowDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_actor.c", 1693);
}

void Actor_shadow_foot(Actor* actor, Lights* lights, PlayState* play) {
    f32 distToFloor = actor->world.pos.y - actor->floorHeight;

    if (distToFloor > 20.0f) {
        f32 shadowScale = actor->shape.shadowScale;
        u8 shadowAlpha = actor->shape.shadowAlpha;
        f32 alphaRatio;

        actor->shape.shadowScale *= 0.3f;
        alphaRatio = (distToFloor - 20.0f) * 0.02f;
        actor->shape.shadowAlpha = (f32)actor->shape.shadowAlpha * CLAMP_MAX(alphaRatio, 1.0f);
        Actor_shadow_circle(actor, lights, play);
        actor->shape.shadowScale = shadowScale;
        actor->shape.shadowAlpha = shadowAlpha;
    }

    if (distToFloor < 200.0f) {
        MtxF floorMtx;
        f32 floorHeight[2]; // One for each foot
        f32 distToFloor;
        f32 shadowAlpha;
        f32 shadowScaleX;
        f32 shadowScaleZ;
        Light* lightPtr;
        s32 lightNum;
        s32 lightNumMax;
        s32 i;
        s32 j;
        s32 numLights = lights->numLights - 2;
        Light* firstLightPtr = &lights->l.l[0];
        Vec3f* feetPosPtr = actor->shape.feetPos;
        f32* floorHeightPtr = floorHeight;

        OPEN_DISPS(play->state.gfxCtx, "../z_actor.c", 1741);

        POLY_OPA_DISP = rcp_mode_set(POLY_OPA_DISP, SETUPDL_44);

        // feetFloorFlag is temporarily a bitfield where the bits are set if the foot is on ground
        // feetFloorFlag & 2 is left foot, feetFloorFlag & 1 is right foot
        actor->shape.feetFloorFlag = 0;

        for (i = 0; i < 2; i++) {
            feetPosPtr->y += 50.0f;
            *floorHeightPtr = Game_play_ground_matrix(play, &floorMtx, feetPosPtr);
            feetPosPtr->y -= 50.0f;
            actor->shape.feetFloorFlag <<= 1;
            distToFloor = feetPosPtr->y - *floorHeightPtr;

            if ((-1.0f <= distToFloor) && (distToFloor < 500.0f)) {
                if (distToFloor <= 0.0f) {
                    actor->shape.feetFloorFlag++;
                }
                if (distToFloor > 30.0f) {
                    distToFloor = 30.0f;
                }
                shadowAlpha = (f32)actor->shape.shadowAlpha * (1.0f - (distToFloor * (1.0f / 30.0f)));
                if (distToFloor > 30.0f) {
                    distToFloor = 30.0f;
                }
                shadowScaleZ = 1.0f - (distToFloor * (1.0f / (30.0f + 40.0f)));
                shadowScaleX = shadowScaleZ * actor->shape.shadowScale * actor->scale.x;
                lightNumMax = 0;
                lightPtr = firstLightPtr;

                for (j = 0; j < numLights; j++) {
                    if (lightPtr->l.dir[1] > 0) {
                        lightNum =
                            (lightPtr->l.col[0] + lightPtr->l.col[1] + lightPtr->l.col[2]) * ABS(lightPtr->l.dir[1]);
                        if (lightNum > 0) {
                            lightNumMax += lightNum;
                            shadow_foot_draw(play, lightPtr, &floorMtx, lightNum, shadowAlpha, shadowScaleX,
                                                 shadowScaleZ);
                        }
                    }
                    lightPtr++;
                }

                for (j = 0; j < 2; j++) {
                    if (lightPtr->l.dir[1] > 0) {
                        lightNum =
                            ((lightPtr->l.col[0] + lightPtr->l.col[1] + lightPtr->l.col[2]) * ABS(lightPtr->l.dir[1])) -
                            (lightNumMax * 8);
                        if (lightNum > 0) {
                            shadow_foot_draw(play, lightPtr, &floorMtx, lightNum, shadowAlpha, shadowScaleX,
                                                 shadowScaleZ);
                        }
                    }
                    lightPtr++;
                }
            }
            feetPosPtr++;
            floorHeightPtr++;
        }

        if (!(actor->bgCheckFlags & BGCHECKFLAG_GROUND)) {
            actor->shape.feetFloorFlag = 0;
        } else if (actor->shape.feetFloorFlag == 3) {
            f32 footDistY = actor->shape.feetPos[FOOT_LEFT].y - actor->shape.feetPos[FOOT_RIGHT].y;

            if ((floorHeight[FOOT_LEFT] + footDistY) < (floorHeight[FOOT_RIGHT] - footDistY)) {
                actor->shape.feetFloorFlag = 2;
            } else {
                actor->shape.feetFloorFlag = 1;
            }
        }

        CLOSE_DISPS(play->state.gfxCtx, "../z_actor.c", 1831);
    }
}

void Actor_foot_shadow_pos_set(Actor* actor, s32 limbIndex, s32 leftFootIndex, Vec3f* leftFootPos, s32 rightFootIndex,
                      Vec3f* rightFootPos) {
    if (limbIndex == leftFootIndex) {
        Matrix_Position(leftFootPos, &actor->shape.feetPos[FOOT_LEFT]);
    } else if (limbIndex == rightFootIndex) {
        Matrix_Position(rightFootPos, &actor->shape.feetPos[FOOT_RIGHT]);
    }
}

void projection_pos_set(PlayState* play, Vec3f* src, Vec3f* xyzDest, f32* cappedInvWDest) {
    Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, src, xyzDest, cappedInvWDest);
    if (*cappedInvWDest < 1.0f) {
        *cappedInvWDest = 1.0f;
    } else {
        *cappedInvWDest = 1.0f / *cappedInvWDest;
    }
}

typedef struct AttentionColor {
    /* 0x00 */ Color_RGBA8 primary;   // Used for Navi's inner color, lock-on arrow, and lock-on reticle
    /* 0x04 */ Color_RGBA8 secondary; // Used for Navi's outer color
} AttentionColor;                     // size = 0x8

// Needs to be static to work around an EGCS codegen bug in Anchor_Marker_data_set on iQue builds.
// If this isn't static, accessing the element at offset 4 into the struct (secondary.r) will not
// be offset correctly in the compiled code.
static AttentionColor status_color[ACTORCAT_MAX + 1] = {
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_SWITCH
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_BG
    { { 255, 255, 255, 255 }, { 0, 0, 255, 0 } },     // ACTORCAT_PLAYER
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_EXPLOSIVE
    { { 150, 150, 255, 255 }, { 150, 150, 255, 0 } }, // ACTORCAT_NPC
    { { 255, 255, 0, 255 }, { 200, 155, 0, 0 } },     // ACTORCAT_ENEMY
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_PROP
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_ITEMACTION
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_MISC
    { { 255, 255, 0, 255 }, { 200, 155, 0, 0 } },     // ACTORCAT_BOSS
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_DOOR
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_CHEST
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // unused extra entry
};

// unused
Gfx dummy_hilite_data[] = {
    gsSPEndDisplayList(),
};

void Anchor_Marker_target_data_set(Attention* attention, s32 reticleNum, f32 x, f32 y, f32 z) {
    LockOnReticle* reticle = &attention->lockOnReticles[reticleNum];

    reticle->pos.x = x;
    reticle->pos.y = y;
    reticle->pos.z = z;

    reticle->radius = attention->reticleRadius;
}

void Anchor_Marker_target_init(Attention* attention, s32 actorCategory, PlayState* play) {
    LockOnReticle* reticle;
    AttentionColor* attentionColor = &status_color[actorCategory];
    s32 i;

    xyz_t_move(&attention->reticlePos, &play->view.eye);

    attention->reticleRadius = 500.0f; // radius starts wide to zoom in on the actor
    attention->reticleFadeAlphaControl = 256;

    reticle = &attention->lockOnReticles[0];

    for (i = 0; i < ARRAY_COUNT(attention->lockOnReticles); i++, reticle++) {
        Anchor_Marker_target_data_set(attention, i, 0.0f, 0.0f, 0.0f);

        reticle->color.r = attentionColor->primary.r;
        reticle->color.g = attentionColor->primary.g;
        reticle->color.b = attentionColor->primary.b;
    }
}

void Anchor_Marker_data_set(Attention* attention, Actor* actor, s32 actorCategory, PlayState* play) {
    attention->naviHoverPos.x = actor->focus.pos.x;
    attention->naviHoverPos.y = actor->focus.pos.y + (actor->lockOnArrowOffset * actor->scale.y);
    attention->naviHoverPos.z = actor->focus.pos.z;

    attention->naviInnerColor.r = status_color[actorCategory].primary.r;
    attention->naviInnerColor.g = status_color[actorCategory].primary.g;
    attention->naviInnerColor.b = status_color[actorCategory].primary.b;
    attention->naviInnerColor.a = status_color[actorCategory].primary.a;

    attention->naviOuterColor.r = status_color[actorCategory].secondary.r;
    attention->naviOuterColor.g = status_color[actorCategory].secondary.g;
    attention->naviOuterColor.b = status_color[actorCategory].secondary.b;
    attention->naviOuterColor.a = status_color[actorCategory].secondary.a;
}

void Anchor_Marker_init(Attention* attention, Actor* actor, PlayState* play) {
    attention->naviHoverActor = attention->reticleActor = attention->forcedLockOnActor = attention->bgmEnemy = NULL;

    attention->reticleSpinCounter = 0;
    attention->naviMoveProgressFactor = 0.0f;
    attention->curReticle = 0;

    Anchor_Marker_data_set(attention, actor, actor->category, play);
    Anchor_Marker_target_init(attention, actor->category, play);
}

void Anchor_Marker_target_draw(Attention* attention, PlayState* play) {
    Actor* actor; // used for both the reticle actor and arrow hover actor

    actor = attention->reticleActor;

    OPEN_DISPS(play->state.gfxCtx, "../z_actor.c", 2029);

    if (attention->reticleFadeAlphaControl != 0) {
        LockOnReticle* reticle;
        Player* player = GET_PLAYER(play);
        s16 alpha;
        f32 projectdPosScale;
        Vec3f projectedPos;
        s32 numReticles;
        f32 invW;
        s32 i;
        s32 curReticle;
        f32 lockOnScaleX;
        s32 triangleIndex;

        alpha = 255;
        projectdPosScale = 1.0f;

        if (attention->reticleSpinCounter != 0) {
            // Reticle is spinning so it is active, only need to draw one
            numReticles = 1;
        } else {
            // Use multiple reticles for the motion blur effect from the reticle
            // quickly zooming in on an actor from off screen
            numReticles = ARRAY_COUNT(attention->lockOnReticles);
        }

        if (actor != NULL) {
            xyz_t_move(&attention->reticlePos, &actor->focus.pos);
            projectdPosScale = (500.0f - attention->reticleRadius) / 420.0f;
        } else {
            // Not locked on, start fading out
            attention->reticleFadeAlphaControl -= 120;

            if (attention->reticleFadeAlphaControl < 0) {
                attention->reticleFadeAlphaControl = 0;
            }

            // `reticleFadeAlphaControl` is only used as an alpha when fading out.
            // Otherwise it defaults to 255, set above.
            alpha = attention->reticleFadeAlphaControl;
        }

        projection_pos_set(play, &attention->reticlePos, &projectedPos, &invW);

        projectedPos.x = ((SCREEN_WIDTH / 2) * (projectedPos.x * invW)) * projectdPosScale;
        projectedPos.x = CLAMP(projectedPos.x, -SCREEN_WIDTH, SCREEN_WIDTH);

        projectedPos.y = ((SCREEN_HEIGHT / 2) * (projectedPos.y * invW)) * projectdPosScale;
        projectedPos.y = CLAMP(projectedPos.y, -SCREEN_HEIGHT, SCREEN_HEIGHT);

        projectedPos.z *= projectdPosScale;

        attention->curReticle--;

        if (attention->curReticle < 0) {
            attention->curReticle = ARRAY_COUNT(attention->lockOnReticles) - 1;
        }

        Anchor_Marker_target_data_set(attention, attention->curReticle, projectedPos.x, projectedPos.y, projectedPos.z);

        if (!(player->stateFlags1 & PLAYER_STATE1_TALKING) || (player->focusActor != actor)) {
            OVERLAY_DISP = rcp_mode_set(OVERLAY_DISP, SETUPDL_57);

            for (i = 0, curReticle = attention->curReticle; i < numReticles;
                 i++, curReticle = (curReticle + 1) % ARRAY_COUNT(attention->lockOnReticles)) {
                reticle = &attention->lockOnReticles[curReticle];

                if (reticle->radius < 500.0f) {
                    if (reticle->radius <= 120.0f) {
                        lockOnScaleX = 0.15f;
                    } else {
                        lockOnScaleX = ((reticle->radius - 120.0f) * 0.001f) + 0.15f;
                    }

                    Matrix_translate(reticle->pos.x, reticle->pos.y, 0.0f, MTXMODE_NEW);
                    Matrix_scale(lockOnScaleX, 0.15f, 1.0f, MTXMODE_APPLY);

                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, reticle->color.r, reticle->color.g, reticle->color.b,
                                    (u8)alpha);

                    Matrix_rotateZ((attention->reticleSpinCounter & 0x7F) * (M_PI / 64), MTXMODE_APPLY);

                    // Draw the 4 triangles that make up the reticle
                    for (triangleIndex = 0; triangleIndex < 4; triangleIndex++) {
                        Matrix_rotateZ(M_PI / 2, MTXMODE_APPLY);
                        Matrix_push();
                        Matrix_translate(reticle->radius, reticle->radius, 0.0f, MTXMODE_APPLY);
                        MATRIX_FINALIZE_AND_LOAD(OVERLAY_DISP++, play->state.gfxCtx, "../z_actor.c", 2116);
                        gSPDisplayList(OVERLAY_DISP++, gLockOnReticleTriangleDL);
                        Matrix_pull();
                    }
                }

                alpha -= 255 / ARRAY_COUNT(attention->lockOnReticles);

                if (alpha < 0) {
                    alpha = 0;
                }
            }
        }
    }

    actor = attention->arrowHoverActor;

    if ((actor != NULL) && !(actor->flags & ACTOR_FLAG_LOCK_ON_DISABLED)) {
        AttentionColor* attentionColor = &status_color[actor->category];

        POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_7);

        Matrix_translate(actor->focus.pos.x, actor->focus.pos.y + (actor->lockOnArrowOffset * actor->scale.y) + 17.0f,
                         actor->focus.pos.z, MTXMODE_NEW);
        Matrix_rotateY(BINANG_TO_RAD((play->gameplayFrames * 3000) & 0xFFFF), MTXMODE_APPLY);
        Matrix_scale((iREG(27) + 35) / 1000.0f, (iREG(28) + 60) / 1000.0f, (iREG(29) + 50) / 1000.0f, MTXMODE_APPLY);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, attentionColor->primary.r, attentionColor->primary.g,
                        attentionColor->primary.b, 255);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_actor.c", 2153);
        gSPDisplayList(POLY_XLU_DISP++, gLockOnArrowDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_actor.c", 2158);
}

void Anchor_Marker_process(Attention* attention, Player* player, Actor* playerFocusActor, PlayState* play) {
    s32 pad;
    Actor* actor; // used for both the Navi hover actor and reticle actor
    s32 category;
    Vec3f projectedFocusPos;
    f32 cappedInvWDest;

    actor = NULL;

    if ((player->focusActor != NULL) &&
        (player->controlStickDirections[player->controlStickDataIndex] == PLAYER_STICK_DIR_BACKWARD)) {
        // Holding backward on the control stick prevents an arrow appearing over the next lock-on actor.
        // This helps escape a lock-on loop when using Switch Targeting, but note that this still works for
        // Hold Targeting as well.
        attention->arrowHoverActor = NULL;
    } else {
        // Find the next attention actor so Navi and an arrow can hover over it (if applicable)
        Actor_info_anchor_search(play, &play->actorCtx, &actor, player);
        attention->arrowHoverActor = actor;
    }

    if (attention->forcedLockOnActor != NULL) {
        // This lock-on actor takes precedence over anything else
        // (this feature is never used in practice)
        actor = attention->forcedLockOnActor;
        attention->forcedLockOnActor = NULL;
    } else if (playerFocusActor != NULL) {
        // Stay locked-on to the same actor, if there is one.
        // This also makes Navi fly over to the current focus actor, if there is one.
        actor = playerFocusActor;
    }

    if (actor != NULL) {
        category = actor->category;
    } else {
        category = player->actor.category;
    }

    if ((actor != attention->naviHoverActor) || (category != attention->naviHoverActorCategory)) {
        // Set Navi to hover over a new actor
        attention->naviHoverActor = actor;
        attention->naviHoverActorCategory = category;
        attention->naviMoveProgressFactor = 1.0f;
    }

    if (actor == NULL) {
        // Setting the actor to Player will make Navi return to him
        actor = &player->actor;
    }

    if (!chase_f(&attention->naviMoveProgressFactor, 0.0f, 0.25f)) {
        f32 moveScale = 0.25f / attention->naviMoveProgressFactor;
        f32 x = actor->world.pos.x - attention->naviHoverPos.x;
        f32 y = (actor->world.pos.y + (actor->lockOnArrowOffset * actor->scale.y)) - attention->naviHoverPos.y;
        f32 z = actor->world.pos.z - attention->naviHoverPos.z;

        attention->naviHoverPos.x += x * moveScale;
        attention->naviHoverPos.y += y * moveScale;
        attention->naviHoverPos.z += z * moveScale;
    } else {
        // Set Navi pos and color after reaching destination
        Anchor_Marker_data_set(attention, actor, category, play);
    }

    if ((playerFocusActor != NULL) && (attention->reticleSpinCounter == 0)) {
        projection_pos_set(play, &playerFocusActor->focus.pos, &projectedFocusPos, &cappedInvWDest);

        if (((projectedFocusPos.z <= 0.0f) || (1.0f <= fabsf(projectedFocusPos.x * cappedInvWDest))) ||
            (1.0f <= fabsf(projectedFocusPos.y * cappedInvWDest))) {
            // Release the reticle if the actor is off screen.
            // It is possible to move far enough away from an actor that it goes off screen, despite being
            // locked onto it. In this case the reticle will release, but the lock-on will remain
            // because Player is still updating focusActor.
            // It is unclear if this is intentional, or if it is a bug and the lock-on as a whole is supposed
            // to release.
            playerFocusActor = NULL;
        }
    }

    if (playerFocusActor != NULL) {
        if (playerFocusActor != attention->reticleActor) {
            s32 lockOnSfxId;

            // Set up a new reticle
            Anchor_Marker_target_init(attention, playerFocusActor->category, play);
            attention->reticleActor = playerFocusActor;

            if (playerFocusActor->id == ACTOR_EN_BOOM) {
                // Don't draw the reticle when locked onto the boomerang.
                // Note that it isn't possible to lock onto the boomerang, so this code doesn't do anything.
                // This implies that the boomerang camera lock may have been implemented with Z-Targeting at one point,
                // but was eventually implemented as its own camera mode instead.
                attention->reticleFadeAlphaControl = 0;
            }

            lockOnSfxId = CHECK_FLAG_ALL(playerFocusActor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)
                              ? NA_SE_SY_LOCK_ON
                              : NA_SE_SY_LOCK_ON_HUMAN;
            Na_StartSystemSe_F(lockOnSfxId);
        }

        // Update reticle

        attention->reticlePos.x = playerFocusActor->world.pos.x;
        attention->reticlePos.y =
            playerFocusActor->world.pos.y - (playerFocusActor->shape.yOffset * playerFocusActor->scale.y);
        attention->reticlePos.z = playerFocusActor->world.pos.z;

        if (attention->reticleSpinCounter == 0) {
            f32 step = (500.0f - attention->reticleRadius) * 3.0f;
            f32 reticleZoomStep = CLAMP(step, 30.0f, 100.0f);

            if (chase_f(&attention->reticleRadius, 80.0f, reticleZoomStep)) {
                // Non-zero counter indicates the reticle is done zooming in
                attention->reticleSpinCounter++;
            }
        } else {
            // Finished zooming in, spin the reticle around the lock-on actor

            // 0x80 is or'd to avoid a value of zero.
            // This rotation value gets multiplied by 0x200, which multiplied by 0x80 gives a full turn (0x10000)
            attention->reticleSpinCounter = (attention->reticleSpinCounter + 3) | 0x80;
            attention->reticleRadius = 120.0f;
        }
    } else {
        // Expand the radius quickly as the reticle is released
        attention->reticleActor = NULL;
        chase_f(&attention->reticleRadius, 500.0f, 80.0f);
    }
}

/**
 * Tests if current scene switch flag is set.
 */
s32 Actor_Environment_sw_Check(PlayState* play, s32 flag) {
    if (flag < 0x20) {
        return play->actorCtx.flags.swch & (1 << flag);
    } else {
        return play->actorCtx.flags.tempSwch & (1 << (flag - 0x20));
    }
}

/**
 * Sets current scene switch flag.
 */
void Actor_Environment_sw_On(PlayState* play, s32 flag) {
    if (flag < 0x20) {
        play->actorCtx.flags.swch |= (1 << flag);
    } else {
        play->actorCtx.flags.tempSwch |= (1 << (flag - 0x20));
    }
}

/**
 * Unsets current scene switch flag.
 */
void Actor_Environment_sw_Off(PlayState* play, s32 flag) {
    if (flag < 0x20) {
        play->actorCtx.flags.swch &= ~(1 << flag);
    } else {
        play->actorCtx.flags.tempSwch &= ~(1 << (flag - 0x20));
    }
}

/**
 * Tests if unknown flag is set.
 */
s32 Actor_Environment_sw_c_Check(PlayState* play, s32 flag) {
    if (flag < 0x20) {
        return play->actorCtx.flags.unk0 & (1 << flag);
    } else {
        return play->actorCtx.flags.unk1 & (1 << (flag - 0x20));
    }
}

/**
 * Sets unknown flag.
 */
void Actor_Environment_sw_c_On(PlayState* play, s32 flag) {
    if (flag < 0x20) {
        play->actorCtx.flags.unk0 |= (1 << flag);
    } else {
        play->actorCtx.flags.unk1 |= (1 << (flag - 0x20));
    }
}

/**
 * Unsets unknown flag.
 */
void Actor_Environment_sw_c_Off(PlayState* play, s32 flag) {
    if (flag < 0x20) {
        play->actorCtx.flags.unk0 &= ~(1 << flag);
    } else {
        play->actorCtx.flags.unk1 &= ~(1 << (flag - 0x20));
    }
}

/**
 * Tests if current scene chest flag is set.
 */
s32 Actor_Environment_Tbox_Check(PlayState* play, s32 flag) {
    return play->actorCtx.flags.chest & (1 << flag);
}

/**
 * Sets current scene chest flag.
 */
void Actor_Environment_Tbox_On(PlayState* play, s32 flag) {
    play->actorCtx.flags.chest |= (1 << flag);
}

/**
 * Tests if current scene clear flag is set.
 */
s32 Actor_Environment_room_clear_Check(PlayState* play, s32 flag) {
    return play->actorCtx.flags.clear & (1 << flag);
}

/**
 * Sets current scene clear flag.
 */
void Actor_Environment_room_clear_On(PlayState* play, s32 flag) {
    play->actorCtx.flags.clear |= (1 << flag);
}

/**
 * Unsets current scene clear flag.
 */
void Actor_Environment_room_clear_Off(PlayState* play, s32 flag) {
    play->actorCtx.flags.clear &= ~(1 << flag);
}

/**
 * Tests if current scene temp clear flag is set.
 */
s32 Actor_Environment_no_enemy_Check(PlayState* play, s32 flag) {
    return play->actorCtx.flags.tempClear & (1 << flag);
}

/**
 * Sets current scene temp clear flag.
 */
void Actor_Environment_no_enemy_On(PlayState* play, s32 flag) {
    play->actorCtx.flags.tempClear |= (1 << flag);
}

/**
 * Unsets current scene temp clear flag.
 */
void Actor_Environment_no_enemy_Off(PlayState* play, s32 flag) {
    play->actorCtx.flags.tempClear &= ~(1 << flag);
}

/**
 * Tests if current scene collectible flag is set.
 */
s32 Actor_Environment_item_Check(PlayState* play, s32 flag) {
    if (flag < 0x20) {
        return play->actorCtx.flags.collect & (1 << flag);
    } else {
        return play->actorCtx.flags.tempCollect & (1 << (flag - 0x20));
    }
}

/**
 * Sets current scene collectible flag.
 */
void Actor_Environment_item_On(PlayState* play, s32 flag) {
    if (flag != 0) {
        if (flag < 0x20) {
            play->actorCtx.flags.collect |= (1 << flag);
        } else {
            play->actorCtx.flags.tempCollect |= (1 << (flag - 0x20));
        }
    }
}

void Actor_Name_Disp_ct(PlayState* play, TitleCardContext* titleCtx) {
    titleCtx->durationTimer = titleCtx->delayTimer = titleCtx->intensity = titleCtx->alpha = 0;
}

void Actor_Name_Disp_Set(PlayState* play, TitleCardContext* titleCtx, void* texture, s16 x, s16 y, u8 width,
                            u8 height) {
    titleCtx->texture = texture;
    titleCtx->x = x;
    titleCtx->y = y;
    titleCtx->width = width;
    titleCtx->height = height;
    titleCtx->durationTimer = 80;
    titleCtx->delayTimer = 0;
}

void Actor_Name_Disp_Set2(PlayState* play, TitleCardContext* titleCtx, void* texture, s32 x, s32 y, s32 width,
                             s32 height, s32 delay) {
    SceneTableEntry* loadedScene = play->loadedScene;
    u32 size = loadedScene->titleFile.vromEnd - loadedScene->titleFile.vromStart;

    if ((size != 0) && (size <= 0x1000 * LANGUAGE_MAX)) {
        DMA_REQUEST_SYNC(texture, loadedScene->titleFile.vromStart, size, "../z_actor.c", 2765);
    }

    titleCtx->texture = texture;
    titleCtx->x = x;
    titleCtx->y = y;
    titleCtx->width = width;
    titleCtx->height = height;
    titleCtx->durationTimer = 80;
    titleCtx->delayTimer = delay;
}

void Actor_Name_Disp_Move(PlayState* play, TitleCardContext* titleCtx) {
    if (DECR(titleCtx->delayTimer) == 0) {
        if (DECR(titleCtx->durationTimer) == 0) {
            chase_s(&titleCtx->alpha, 0, 30);
            chase_s(&titleCtx->intensity, 0, 70);
        } else {
            chase_s(&titleCtx->alpha, 255, 10);
            chase_s(&titleCtx->intensity, 255, 20);
        }
    }
}

void Actor_Name_Disp_Draw(PlayState* play, TitleCardContext* titleCtx) {
    s32 width;
    s32 height;
    s32 doubleWidth;
    s32 titleX1;
    s32 titleX2;
    s32 titleY1;
    s32 titleY2;
    s32 textureLanguageOffset;

    if (titleCtx->alpha != 0) {
        width = titleCtx->width;
        height = titleCtx->height;
        doubleWidth = width * 2;
        titleX1 = (titleCtx->x * 4) - (width * 2);
        titleX2 = titleX1 + (doubleWidth * 2) - 4;
        titleY1 = (titleCtx->y * 4) - (height * 2);

        OPEN_DISPS(play->state.gfxCtx, "../z_actor.c", 2824);

#if OOT_NTSC
        if (z_common_data.language == LANGUAGE_JPN) {
            textureLanguageOffset = 0;
        } else {
            textureLanguageOffset = width * height;
        }
#else
        textureLanguageOffset = width * height * z_common_data.language;
#endif

        if (width * height > 0x1000) {
            height = 0x1000 / width;
        }

        titleY2 = titleY1 + (height * 4);

        OVERLAY_DISP = gfx_texture_cullback_xlu(OVERLAY_DISP);

        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, (u8)titleCtx->intensity, (u8)titleCtx->intensity, (u8)titleCtx->intensity,
                        (u8)titleCtx->alpha);

        gDPLoadTextureBlock(OVERLAY_DISP++, (u8*)titleCtx->texture + textureLanguageOffset, G_IM_FMT_IA, G_IM_SIZ_8b,
                            width, height, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gSPTextureRectangle(OVERLAY_DISP++, titleX1, titleY1, titleX2, titleY2 - 1, G_TX_RENDERTILE, 0, 0, 1 << 10,
                            1 << 10);

        height = titleCtx->height - height;

        // If texture is bigger than 0x1000, display the rest
        if (height > 0) {
            gDPLoadTextureBlock(OVERLAY_DISP++, (u8*)titleCtx->texture + 0x1000 + textureLanguageOffset, G_IM_FMT_IA,
                                G_IM_SIZ_8b, width, height, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gSPTextureRectangle(OVERLAY_DISP++, titleX1, titleY2, titleX2, titleY2 + (height * 4) - 1, G_TX_RENDERTILE,
                                0, 0, 1 << 10, 1 << 10);
        }

        CLOSE_DISPS(play->state.gfxCtx, "../z_actor.c", 2880);
    }
}

s32 Actor_Name_Disp_Cancel(PlayState* play, TitleCardContext* titleCtx) {
    if ((play->actorCtx.titleCtx.delayTimer != 0) || (play->actorCtx.titleCtx.alpha != 0)) {
        titleCtx->durationTimer = 0;
        titleCtx->delayTimer = 0;
        return false;
    }

    return true;
}

void Actor_delete(Actor* actor) {
    actor->draw = NULL;
    actor->update = NULL;
    actor->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void Actor_home_to_world(Actor* actor) {
    actor->world = actor->home;
}

void Actor_world_to_eye(Actor* actor, f32 yOffset) {
    actor->focus.pos.x = actor->world.pos.x;
    actor->focus.pos.y = actor->world.pos.y + yOffset;
    actor->focus.pos.z = actor->world.pos.z;

    actor->focus.rot.x = actor->world.rot.x;
    actor->focus.rot.y = actor->world.rot.y;
    actor->focus.rot.z = actor->world.rot.z;
}

void Actor_shape_to_world(Actor* actor) {
    actor->world.rot = actor->shape.rot;
}

void Actor_world_to_shape(Actor* actor) {
    actor->shape.rot = actor->world.rot;
}

void Actor_set_scale(Actor* actor, f32 scale) {
    actor->scale.z = scale;
    actor->scale.y = scale;
    actor->scale.x = scale;
}

void Actor_set_segment(PlayState* play, Actor* actor) {
    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[actor->objectSlot].segment);
}

void Actor_ct(Actor* actor, PlayState* play) {
    Actor_home_to_world(actor);
    Actor_world_to_shape(actor);
    Actor_world_to_eye(actor, 0.0f);
    xyz_t_move(&actor->prevPos, &actor->world.pos);
    Actor_set_scale(actor, 0.01f);
    actor->attentionRangeType = ATTENTION_RANGE_3;
    actor->minVelocityY = -20.0f;
    actor->xyzDistToPlayerSq = MAXFLOAT;
    actor->naviEnemyId = NAVI_ENEMY_NONE;
    actor->cullingVolumeDistance = 1000.0f;
    actor->cullingVolumeScale = 350.0f;
    actor->cullingVolumeDownward = 700.0f;
    CollisionCheck_Status_ct(&actor->colChkInfo);
    actor->floorBgId = BGCHECK_SCENE;
    Shape_Info_init(&actor->shape, 0.0f, NULL, 0.0f);
    if (Object_Exchange_bank_dma_check(&play->objectCtx, actor->objectSlot)) {
        Actor_set_segment(play, actor);
        actor->init(actor, play);
        actor->init = NULL;
    }
}

void Actor_dt(Actor* actor, PlayState* play) {
    ActorOverlay* overlayEntry;
    char* name;

    if (actor->destroy != NULL) {
        actor->destroy(actor, play);
        actor->destroy = NULL;
    } else {
#if DEBUG_FEATURES
        overlayEntry = actor->overlayEntry;
        name = overlayEntry->name != NULL ? overlayEntry->name : "";

        PRINTF(T("Ａｃｔｏｒクラス デストラクトがありません [%s]\n", "No Actor class destruct [%s]\n") ACTOR_RST, name);
#endif
    }
}

/**
 * Update actor's position factoring in velocity and collider displacement
 */
void Actor_position_move(Actor* actor) {
    f32 speedRate = R_UPDATE_RATE * 0.5f;

    actor->world.pos.x += (actor->velocity.x * speedRate) + actor->colChkInfo.displacement.x;
    actor->world.pos.y += (actor->velocity.y * speedRate) + actor->colChkInfo.displacement.y;
    actor->world.pos.z += (actor->velocity.z * speedRate) + actor->colChkInfo.displacement.z;
}

/**
 * Update actor's velocity accounting for gravity (without dropping below minimum y velocity)
 */
void Actor_position_speed_set(Actor* actor) {
    actor->velocity.x = actor->speed * sin_s(actor->world.rot.y);
    actor->velocity.z = actor->speed * cos_s(actor->world.rot.y);

    actor->velocity.y += actor->gravity;

    if (actor->velocity.y < actor->minVelocityY) {
        actor->velocity.y = actor->minVelocityY;
    }
}

/**
 * Move actor while accounting for its current velocity and gravity.
 * `actor.speed` is used as the XZ velocity.
 * The actor will move in the direction of its world yaw.
 */
void Actor_position_moveF(Actor* actor) {
    Actor_position_speed_set(actor);
    Actor_position_move(actor);
}

/**
 * Update actor's velocity without gravity.
 */
void Actor_position_speed_set_XY(Actor* actor) {
    f32 speedXZ = actor->speed * cos_s(actor->world.rot.x);

    actor->velocity.x = speedXZ * sin_s(actor->world.rot.y);
    actor->velocity.y = actor->speed * sin_s(actor->world.rot.x);
    actor->velocity.z = speedXZ * cos_s(actor->world.rot.y);
}

/**
 * Move actor while accounting for its current velocity.
 * `actor.speed` is used as the XYZ velocity.
 * The actor will move in the direction of its world yaw and pitch, with positive pitch moving upwards.
 */
void Actor_position_moveF_XY(Actor* actor) {
    Actor_position_speed_set_XY(actor);
    Actor_position_move(actor);
}

/**
 * From a given XYZ speed value, set the corresponding XZ speed as `actor.speed`, and Y speed as Y velocity.
 * Only the actor's world pitch is factored in, with positive pitch moving downwards.
 */
void Actor_vector_to_position_speed(Actor* actor, f32 speedXYZ) {
    actor->speed = speedXYZ * cos_s(actor->world.rot.x);
    actor->velocity.y = speedXYZ * -sin_s(actor->world.rot.x);
}

void Actor_position_move_anime(Actor* actor, SkelAnime* skelAnime) {
    Vec3f posDiff;

    Skeleton_Info_translate_set(skelAnime, &posDiff, actor->shape.rot.y);

    actor->world.pos.x += posDiff.x * actor->scale.x;
    actor->world.pos.y += posDiff.y * actor->scale.y;
    actor->world.pos.z += posDiff.z * actor->scale.z;
}

/**
 * @return Yaw towards `target` for `origin`, using world positions.
 */
s16 Actor_search_actor_angleY(Actor* origin, Actor* target) {
    return search_position_angleY(&origin->world.pos, &target->world.pos);
}

/**
 * @return Yaw towards `target` for `origin`, using focus positions.
 */
s16 Actor_eye_search_actor_angleY(Actor* origin, Actor* target) {
    return search_position_angleY(&origin->focus.pos, &target->focus.pos);
}

/**
 * @return Yaw towards `point` for `origin`.
 */
s16 Actor_search_position_angleY(Actor* origin, Vec3f* point) {
    return search_position_angleY(&origin->world.pos, point);
}

s16 Actor_search_actor_angleX(Actor* actorA, Actor* actorB) {
    return search_position_angleX(&actorA->world.pos, &actorB->world.pos);
}

s16 Actor_eye_search_actor_angleX(Actor* actorA, Actor* actorB) {
    return search_position_angleX(&actorA->focus.pos, &actorB->focus.pos);
}

s16 Actor_search_position_angleX(Actor* actor, Vec3f* refPoint) {
    return search_position_angleX(&actor->world.pos, refPoint);
}

f32 Actor_search_actor_distance(Actor* actorA, Actor* actorB) {
    return search_position_distance(&actorA->world.pos, &actorB->world.pos);
}

f32 Actor_search_position_distance(Actor* actor, Vec3f* refPoint) {
    return search_position_distance(&actor->world.pos, refPoint);
}

f32 Actor_search_actor_distanceXZ(Actor* actorA, Actor* actorB) {
    return search_position_distanceXZ(&actorA->world.pos, &actorB->world.pos);
}

f32 Actor_search_position_distanceXZ(Actor* actor, Vec3f* refPoint) {
    return search_position_distanceXZ(&actor->world.pos, refPoint);
}

/**
 * Convert `pos` to be relative to the actor's position and yaw, store into `dest`.
 */
void Actor_search_position_project_distanceXZ(Actor* actor, Vec3f* dest, Vec3f* pos) {
    f32 cosY;
    f32 sinY;
    f32 deltaX;
    f32 deltaZ;

    cosY = cos_s(actor->shape.rot.y);
    sinY = sin_s(actor->shape.rot.y);
    deltaX = pos->x - actor->world.pos.x;
    deltaZ = pos->z - actor->world.pos.z;

    dest->x = -(deltaZ * sinY) + (deltaX * cosY);
    dest->z = (deltaX * sinY) + (deltaZ * cosY);
    dest->y = pos->y - actor->world.pos.y;
}

f32 Actor_search_actor_high(Actor* actorA, Actor* actorB) {
    return actorB->world.pos.y - actorA->world.pos.y;
}

f32 player_get_tall(Player* player) {
    f32 offset = (player->stateFlags1 & PLAYER_STATE1_23) ? 32.0f : 0.0f;

    if (LINK_IS_ADULT) {
        return offset + 68.0f;
    } else {
        return offset + 44.0f;
    }
}

f32 player_get_max_speed(Player* player) {
    s32 pad;

    if (player->stateFlags1 & PLAYER_STATE1_23) {
        return 8.0f;
    } else if (player->stateFlags1 & PLAYER_STATE1_27) {
        return (R_RUN_SPEED_LIMIT / 100.0f) * 0.6f;
    } else {
        return R_RUN_SPEED_LIMIT / 100.0f;
    }
}

int player_bow_check(Player* player) {
    return player->stateFlags1 & PLAYER_STATE1_3;
}

int player_bow_ready_check(Player* player) {
    return player_bow_check(player) && (player->unk_834 != 0);
}

int player_fight_check(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return (player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) || player_bow_ready_check(player);
}

s32 player_big_SE_action_check(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return player->stateFlags2 & PLAYER_STATE2_3;
}

s32 player_climb_stop_check(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return player->stateFlags2 & PLAYER_STATE2_12;
}

/**
 * Swap hookshot attachment state from one actor to another.
 *
 * Note: There is no safety check for a NULL hookshot pointer.
 * The responsibility is on the caller to make sure the hookshot exists.
 */
void player_hook_carry_change(PlayState* play, Actor* srcActor, Actor* destActor) {
    ArmsHook* hookshot = (ArmsHook*)Actor_info_name_search(&play->actorCtx, ACTOR_ARMS_HOOK, ACTORCAT_ITEMACTION);

    hookshot->attachedActor = destActor;

    // The hookshot will attach at exactly the actors world position with 0 offset
    hookshot->attachPointOffset.x = hookshot->attachPointOffset.y = hookshot->attachPointOffset.z = 0.0f;

    destActor->flags |= ACTOR_FLAG_HOOKSHOT_ATTACHED;
    srcActor->flags &= ~ACTOR_FLAG_HOOKSHOT_ATTACHED;
}

void player_make_horse_camera(PlayState* play, Player* player) {
    if ((play->roomCtx.curRoom.type != ROOM_TYPE_4) && Game_play_change_camera_check(play)) {
        changeCameraSet(Gama_play_get_camera(play, CAM_ID_MAIN), CAM_SET_HORSE);
    }
}

void player_ride_horse_init(PlayState* play, Player* player, Actor* horse) {
    player->rideActor = horse;
    player->stateFlags1 |= PLAYER_STATE1_23;
    horse->child = &player->actor;
}

int Actor_player_demo_check(Player* player) {
    return (player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_29)) ||
           (player->csAction != PLAYER_CSACTION_NONE);
}

void Actor_Horse_Set_Check(PlayState* play, Player* player) {
    Horse_Set_Check(play, player);
}

/**
 * Sets a Player Cutscene Action specified by `csAction`.
 * There are no safety checks to see if Player is already in some form of a cutscene state.
 * This will instantly take effect.
 *
 * `haltActorsDuringCsAction` being set to false in this function means that all actors will
 * be able to update while Player is performing the cutscene action.
 *
 * Note: due to how player implements initializing the cutscene action state, `haltActorsDuringCsAction`
 * will only be considered the first time player starts a `csAction`.
 * Player must leave the cutscene action state and enter it again before halting actors can be toggled.
 */
s32 player_demo_mode_set2(PlayState* play, Actor* csActor, u8 csAction) {
    Player* player = GET_PLAYER(play);

    player->csAction = csAction;
    player->csActor = csActor;
    player->cv.haltActorsDuringCsAction = false;

    return true;
}

/**
 * Sets a Player Cutscene Action specified by `csAction`.
 * There are no safety checks to see if Player is already in some form of a cutscene state.
 * This will instantly take effect.
 *
 * `haltActorsDuringCsAction` being set to true in this function means that eventually `PLAYER_STATE1_29` will be set.
 * This makes it so actors belonging to categories `ACTORCAT_ENEMY` and `ACTORCAT_MISC` will not update
 * while Player is performing the cutscene action.
 *
 * Note: due to how player implements initializing the cutscene action state, `haltActorsDuringCsAction`
 * will only be considered the first time player starts a `csAction`.
 * Player must leave the cutscene action state and enter it again before halting actors can be toggled.
 */
s32 player_demo_mode_set(PlayState* play, Actor* csActor, u8 csAction) {
    Player* player = GET_PLAYER(play);

    player_demo_mode_set2(play, csActor, csAction);
    player->cv.haltActorsDuringCsAction = true;

    return true;
}

void MoveBG_Actor_power_clear(DynaPolyActor* dynaActor) {
    dynaActor->unk_150 = dynaActor->unk_154 = 0.0f;
}

void MoveBG_Actor_powerF_set(DynaPolyActor* dynaActor, f32 arg1, s16 arg2) {
    dynaActor->unk_150 += arg1;
    dynaActor->unk_158 = arg2;
}

/**
 * Chcek if the player is facing the specified actor.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngle`.
 */
s32 Actor_player_look_direction_check(Actor* actor, s16 maxAngle, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff = (s16)(actor->yawTowardsPlayer + 0x8000) - player->actor.shape.rot.y;

    if (ABS(yawDiff) < maxAngle) {
        return true;
    }

    return false;
}

/**
 * Chcek if `actorB` is facing `actorA`.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngle`.
 *
 * This function is unused in the original game.
 */
s32 Actor_look_direction_check(Actor* actorA, Actor* actorB, s16 maxAngle) {
    s16 yawDiff = (s16)(Actor_search_actor_angleY(actorA, actorB) + 0x8000) - actorB->shape.rot.y;

    if (ABS(yawDiff) < maxAngle) {
        return true;
    }

    return false;
}

/**
 * Chcek if the specified actor is facing the player.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngle`.
 */
s32 Actor_player_direction_check(Actor* actor, s16 maxAngle) {
    s16 yawDiff = actor->yawTowardsPlayer - actor->shape.rot.y;

    if (ABS(yawDiff) < maxAngle) {
        return true;
    }

    return false;
}

/**
 * Chcek if `actorA` is facing `actorB`.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngle`.
 *
 * This function is unused in the original game.
 */
s32 Actor_actor_direction_check(Actor* actorA, Actor* actorB, s16 maxAngle) {
    s16 yawDiff = Actor_search_actor_angleY(actorA, actorB) - actorA->shape.rot.y;

    if (ABS(yawDiff) < maxAngle) {
        return true;
    }

    return false;
}

/**
 * Chcek if the specified actor is facing the player and is nearby.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngle`.
 * The minimum distance that qualifies as "nearby" is specified by `range`.
 */
s32 Actor_player_distance_direction_check(Actor* actor, f32 range, s16 maxAngle) {
    s16 yawDiff = actor->yawTowardsPlayer - actor->shape.rot.y;

    if (ABS(yawDiff) < maxAngle) {
        f32 xyzDistanceFromLink = sqrtf(SQ(actor->xzDistToPlayer) + SQ(actor->yDistToPlayer));

        if (xyzDistanceFromLink < range) {
            return true;
        }
    }

    return false;
}

/**
 * Chcek if `actorA` is facing `actorB` and is nearby.
 * The maximum angle difference that qualifies as "facing" is specified by `maxAngle`.
 * The minimum distance that qualifies as "nearby" is specified by `range`.
 */
s32 Actor_actor_distance_direction_check(Actor* actorA, Actor* actorB, f32 range, s16 maxAngle) {
    if (Actor_search_actor_distance(actorA, actorB) < range) {
        s16 yawDiff = Actor_search_actor_angleY(actorA, actorB) - actorA->shape.rot.y;

        if (ABS(yawDiff) < maxAngle) {
            return true;
        }
    }

    return false;
}

s32 ground_no_hit_set(Actor* actor, f32 arg1, s32 arg2) {
    if ((actor->bgCheckFlags & BGCHECKFLAG_GROUND) && (arg1 < -11.0f)) {
        actor->bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        actor->bgCheckFlags |= BGCHECKFLAG_GROUND_LEAVE;

        if ((actor->velocity.y < 0.0f) && (arg2 & UPDBGCHECKINFO_FLAG_4)) {
            actor->velocity.y = 0.0f;
        }

        return false;
    }

    return true;
}

s32 Actor_foot_bgcheck(PlayState* play, Actor* actor, Vec3f* pos, s32 arg3) {
    f32 floorHeightDiff;
    s32 floorBgId;

    pos->y += 50.0f;

    actor->floorHeight = T_BGCheck_ObjGroundCheck_aiac2(play, &play->colCtx, &actor->floorPoly, &floorBgId, actor, pos);
    actor->bgCheckFlags &= ~(BGCHECKFLAG_GROUND_TOUCH | BGCHECKFLAG_GROUND_LEAVE | BGCHECKFLAG_GROUND_STRICT);

    if (actor->floorHeight <= BGCHECK_Y_MIN) {
        return ground_no_hit_set(actor, BGCHECK_Y_MIN, arg3);
    }

    floorHeightDiff = actor->floorHeight - actor->world.pos.y;
    actor->floorBgId = floorBgId;

    if (floorHeightDiff >= 0.0f) { // actor is on or below the ground
        actor->bgCheckFlags |= BGCHECKFLAG_GROUND_STRICT;

        if (actor->bgCheckFlags & BGCHECKFLAG_CEILING) {
            if (roof_pbg_actor_index != floorBgId) {
                if (floorHeightDiff > 15.0f) {
                    actor->bgCheckFlags |= BGCHECKFLAG_CRUSHED;
                }
            } else {
                actor->world.pos.x = actor->prevPos.x;
                actor->world.pos.z = actor->prevPos.z;
            }
        }

        actor->world.pos.y = actor->floorHeight;

        if (actor->velocity.y <= 0.0f) {
            if (!(actor->bgCheckFlags & BGCHECKFLAG_GROUND)) {
                actor->bgCheckFlags |= BGCHECKFLAG_GROUND_TOUCH;
            } else if ((arg3 & UPDBGCHECKINFO_FLAG_3) && (actor->gravity < 0.0f)) {
                actor->velocity.y = -4.0f;
            } else {
                actor->velocity.y = 0.0f;
            }

            actor->bgCheckFlags |= BGCHECKFLAG_GROUND;
            BGCheckCollection_setRideStatus(&play->colCtx, actor, actor->floorBgId);
        }
    } else { // actor is above ground
        if ((actor->bgCheckFlags & BGCHECKFLAG_GROUND) && (floorHeightDiff >= -11.0f)) {
            BGCheckCollection_setRideStatus(&play->colCtx, actor, actor->floorBgId);
        }

        return ground_no_hit_set(actor, floorHeightDiff, arg3);
    }

    return true;
}

void Actor_BGcheck2(PlayState* play, Actor* actor, f32 wallCheckHeight, f32 wallCheckRadius,
                             f32 ceilingCheckHeight, s32 flags) {
    f32 sp74;
    s32 floorBgId;
    Vec3f sp64;

    sp74 = actor->world.pos.y - actor->prevPos.y;
    floorBgId = actor->floorBgId;

    if ((floorBgId != BGCHECK_SCENE) && (actor->bgCheckFlags & BGCHECKFLAG_GROUND)) {
        BGCheckCollection_proc(&play->colCtx, floorBgId, actor);
    }

    if (flags & UPDBGCHECKINFO_FLAG_0) {
        s32 bgId;

        if ((!(flags & UPDBGCHECKINFO_FLAG_7) &&
             T_BGCheck_ObjWallCheck2_aiac(&play->colCtx, &sp64, &actor->world.pos, &actor->prevPos, wallCheckRadius,
                                      &actor->wallPoly, &bgId, actor, wallCheckHeight)) ||
            ((flags & UPDBGCHECKINFO_FLAG_7) &&
             T_BGCheck_ObjWallCheck2_aiac_nl(&play->colCtx, &sp64, &actor->world.pos, &actor->prevPos, wallCheckRadius,
                                      &actor->wallPoly, &bgId, actor, wallCheckHeight))) {
            CollisionPoly* wallPoly;

            wallPoly = actor->wallPoly;
            xyz_t_move(&actor->world.pos, &sp64);
            actor->wallYaw = atans_table(wallPoly->normal.z, wallPoly->normal.x);
            actor->bgCheckFlags |= BGCHECKFLAG_WALL;
            actor->wallBgId = bgId;
        } else {
            actor->bgCheckFlags &= ~BGCHECKFLAG_WALL;
        }
    }

    sp64.x = actor->world.pos.x;
    sp64.z = actor->world.pos.z;

    if (flags & UPDBGCHECKINFO_FLAG_1) {
        f32 sp58;

        sp64.y = actor->prevPos.y + 10.0f;
        if (T_BGCheck_ObjRoofCheck_aiac(&play->colCtx, &sp58, &sp64, (ceilingCheckHeight + sp74) - 10.0f,
                                       &roof_polygon_info, &roof_pbg_actor_index, actor)) {
            actor->bgCheckFlags |= BGCHECKFLAG_CEILING;
            actor->world.pos.y = (sp58 + sp74) - 10.0f;
        } else {
            actor->bgCheckFlags &= ~BGCHECKFLAG_CEILING;
        }
    }

    if (flags & UPDBGCHECKINFO_FLAG_2) {
        WaterBox* waterBox;
        f32 waterBoxYSurface;

        sp64.y = actor->prevPos.y;
        Actor_foot_bgcheck(play, actor, &sp64, flags);
        waterBoxYSurface = actor->world.pos.y;
        if (T_BGCheck_WaterSurfaceCheck3(play, &play->colCtx, actor->world.pos.x, actor->world.pos.z, &waterBoxYSurface,
                                 &waterBox)) {
            actor->depthInWater = waterBoxYSurface - actor->world.pos.y;
            if (actor->depthInWater < 0.0f) {
                actor->bgCheckFlags &= ~(BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH);
            } else {
                if (!(actor->bgCheckFlags & BGCHECKFLAG_WATER)) {
                    Vec3f ripplePos;

                    actor->bgCheckFlags |= BGCHECKFLAG_WATER_TOUCH;
                    if (!(flags & UPDBGCHECKINFO_FLAG_6)) {
                        ripplePos.x = actor->world.pos.x;
                        ripplePos.y = waterBoxYSurface;
                        ripplePos.z = actor->world.pos.z;
                        Effect_SS_G_Ripple_ct2(play, &ripplePos, 100, 500, 0);
                        Effect_SS_G_Ripple_ct2(play, &ripplePos, 100, 500, 4);
                        Effect_SS_G_Ripple_ct2(play, &ripplePos, 100, 500, 8);
                    }
                }
                actor->bgCheckFlags |= BGCHECKFLAG_WATER;
            }
        } else {
            actor->depthInWater = BGCHECK_Y_MIN;
            actor->bgCheckFlags &= ~(BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH);
        }
    }
}

Mtx work_view_matrix;

Gfx* HiliteReflect_new(Vec3f* object, Vec3f* eye, Vec3f* lightDir, GraphicsContext* gfxCtx, Gfx* gfx, Hilite** hilite) {
    LookAt* lookAt;
    f32 correctedEyeX;

    lookAt = GRAPH_ALLOC(gfxCtx, sizeof(LookAt));

    correctedEyeX = (eye->x == object->x) && (eye->z == object->z) ? eye->x + 0.001f : eye->x;

    *hilite = GRAPH_ALLOC(gfxCtx, sizeof(Hilite));

#if DEBUG_FEATURES
    if (R_HREG_MODE == HREG_MODE_PRINT_HILITE_INFO) {
        PRINTF("z_actor.c 3529 eye=[%f(%f) %f %f] object=[%f %f %f] light_direction=[%f %f %f]\n", correctedEyeX,
               eye->x, eye->y, eye->z, object->x, object->y, object->z, lightDir->x, lightDir->y, lightDir->z);
    }
#endif

    VIEW_ERROR_CHECK_EYE_POS(correctedEyeX, eye->y, eye->z);

    guLookAtHilite(&work_view_matrix, lookAt, *hilite, correctedEyeX, eye->y, eye->z, object->x, object->y, object->z, 0.0f,
                   1.0f, 0.0f, lightDir->x, lightDir->y, lightDir->z, lightDir->x, lightDir->y, lightDir->z, 16, 16);

    gSPLookAt(gfx++, lookAt);
    gDPSetHilite1Tile(gfx++, 1, *hilite, 16, 16);

    return gfx;
}

Hilite* HiliteReflect_init(Vec3f* object, Vec3f* eye, Vec3f* lightDir, GraphicsContext* gfxCtx) {
    Hilite* hilite;

    OPEN_DISPS(gfxCtx, "../z_actor.c", 4306);

    POLY_OPA_DISP = HiliteReflect_new(object, eye, lightDir, gfxCtx, POLY_OPA_DISP, &hilite);

    CLOSE_DISPS(gfxCtx, "../z_actor.c", 4313);

    return hilite;
}

Hilite* HiliteReflect_xlu_init(Vec3f* object, Vec3f* eye, Vec3f* lightDir, GraphicsContext* gfxCtx) {
    Hilite* hilite;

    OPEN_DISPS(gfxCtx, "../z_actor.c", 4332);

    POLY_XLU_DISP = HiliteReflect_new(object, eye, lightDir, gfxCtx, POLY_XLU_DISP, &hilite);

    CLOSE_DISPS(gfxCtx, "../z_actor.c", 4339);

    return hilite;
}

void Actor_HiliteReflect_set_init(Actor* actor, PlayState* play, s32 flag) {
    Hilite* hilite;
    Vec3f lightDir;
    Gfx* displayListHead;
    Gfx* displayList;

    lightDir.x = play->envCtx.dirLight1.params.dir.x;
    lightDir.y = play->envCtx.dirLight1.params.dir.y;
    lightDir.z = play->envCtx.dirLight1.params.dir.z;

#if DEBUG_FEATURES
    if (R_HREG_MODE == HREG_MODE_PRINT_HILITE_INFO) {
        PRINTF("z_actor.c 3637 game_play->view.eye=[%f(%f) %f %f]\n", play->view.eye.x, play->view.eye.y,
               play->view.eye.z);
    }
#endif

    hilite = HiliteReflect_init(&actor->world.pos, &play->view.eye, &lightDir, play->state.gfxCtx);

    if (flag != 0) {
        displayList = GRAPH_ALLOC(play->state.gfxCtx, 2 * sizeof(Gfx));
        displayListHead = displayList;

        OPEN_DISPS(play->state.gfxCtx, "../z_actor.c", 4384);

        gDPSetHilite1Tile(displayListHead++, 1, hilite, 16, 16);
        gSPEndDisplayList(displayListHead);
        gSPSegment(POLY_OPA_DISP++, 0x07, displayList);

        CLOSE_DISPS(play->state.gfxCtx, "../z_actor.c", 4394);
    }
}

void Actor_HiliteReflect_xlu_set_init(Actor* actor, PlayState* play, s32 flag) {
    Hilite* hilite;
    Vec3f lightDir;
    Gfx* displayListHead;
    Gfx* displayList;

    lightDir.x = play->envCtx.dirLight1.params.dir.x;
    lightDir.y = play->envCtx.dirLight1.params.dir.y;
    lightDir.z = play->envCtx.dirLight1.params.dir.z;

    hilite = HiliteReflect_xlu_init(&actor->world.pos, &play->view.eye, &lightDir, play->state.gfxCtx);

    if (flag != 0) {
        displayList = GRAPH_ALLOC(play->state.gfxCtx, 2 * sizeof(Gfx));
        displayListHead = displayList;

        OPEN_DISPS(play->state.gfxCtx, "../z_actor.c", 4429);

        gDPSetHilite1Tile(displayListHead++, 1, hilite, 16, 16);
        gSPEndDisplayList(displayListHead);
        gSPSegment(POLY_XLU_DISP++, 0x07, displayList);

        CLOSE_DISPS(play->state.gfxCtx, "../z_actor.c", 4439);
    }
}

PosRot Actor_get_eye(Actor* actor) {
    return actor->focus;
}

PosRot Actor_get_world(Actor* actor) {
    return actor->world;
}

PosRot Actor_get_shape(Actor* actor) {
    PosRot worldPosRot;

    xyz_t_move(&worldPosRot.pos, &actor->world.pos);
    worldPosRot.rot = actor->shape.rot;

    return worldPosRot;
}

/**
 * Returns the squared xyz distance from the actor to Player.
 * This distance will be weighted if Player is already locked onto another actor.
 */
f32 Actor_anchor_ratio(Actor* actor, Player* player, s16 playerShapeYaw) {
    s16 yawTempAbs = (s16)ABS((s16)((s16)(actor->yawTowardsPlayer - 0x8000) - playerShapeYaw));

    if (player->focusActor != NULL) {
        if ((yawTempAbs > 0x4000) || (actor->flags & ACTOR_FLAG_LOCK_ON_DISABLED)) {
            return MAXFLOAT;
        } else {
            f32 adjDistSq;

            // The distance returned is scaled down as the player faces more toward the actor.
            // At 90 degrees, 100% of the original distance will be returned.
            // This scales down linearly to 60% when facing 0 degrees away.
            adjDistSq =
                actor->xyzDistToPlayerSq - actor->xyzDistToPlayerSq * 0.8f * ((0x4000 - yawTempAbs) * (1.0f / 0x8000));

            return adjDistSq;
        }
    }

    // Player has to be facing less than ~60 degrees away from the actor
    if (yawTempAbs > 0x2AAA) {
        return MAXFLOAT;
    }

    // Unweighted distSq
    return actor->xyzDistToPlayerSq;
}

typedef struct AttentionRangeParams {
    /* 0x0 */ f32 attentionRangeSq;
    /* 0x4 */ f32 lockOnLeashScale;
} AttentionRangeParams; // size = 0x8

#define ATTENTION_RANGES(range, lockOnLeashRange) \
    { SQ(range), (f32)range / lockOnLeashRange }

AttentionRangeParams anchor_status_data[ATTENTION_RANGE_MAX] = {
    ATTENTION_RANGES(70, 140),        // ATTENTION_RANGE_0
    ATTENTION_RANGES(170, 255),       // ATTENTION_RANGE_1
    ATTENTION_RANGES(280, 5600),      // ATTENTION_RANGE_2
    ATTENTION_RANGES(350, 525),       // ATTENTION_RANGE_3
    ATTENTION_RANGES(700, 1050),      // ATTENTION_RANGE_4
    ATTENTION_RANGES(1000, 1500),     // ATTENTION_RANGE_5
    ATTENTION_RANGES(100, 105.36842), // ATTENTION_RANGE_6
    ATTENTION_RANGES(140, 163.33333), // ATTENTION_RANGE_7
    ATTENTION_RANGES(240, 576),       // ATTENTION_RANGE_8
    ATTENTION_RANGES(280, 280000),    // ATTENTION_RANGE_9
};

/**
 * Checks if an actor at `distSq` is inside the range specified by its `attentionRangeType`.
 *
 * Note that this gets used for both the attention range check and for the lock-on leash range check.
 * Despite how the data is presented in `anchor_status_data`, the leash range is stored as a scale factor value.
 * When checking the leash range, this scale factor is applied to the input distance and checked against
 * the base `attentionRangeSq` value, which was used to initiate the lock-on in the first place.
 */
u32 Actor_anchor_ratio_check(Actor* actor, f32 distSq) {
    return distSq < anchor_status_data[actor->attentionRangeType].attentionRangeSq;
}

/**
 * Returns true if an actor lock-on should be released.
 * This function does not actually release the lock-on, as that is Player's responsibility.
 *
 * If an actor's update function is NULL or `ACTOR_FLAG_ATTENTION_ENABLED` is unset, the lock-on should be released.
 *
 * There is also a check for Player exceeding the lock-on leash distance.
 * Note that this check will be ignored if `ignoreLeash` is true.
 *
 */
s32 Actor_anchor_check(Actor* actor, Player* player, s32 ignoreLeash) {
    if ((actor->update == NULL) || !(actor->flags & ACTOR_FLAG_ATTENTION_ENABLED)) {
        return true;
    }

    if (!ignoreLeash) {
        s16 yawDiffAbs = (s16)ABS((s16)((s16)(actor->yawTowardsPlayer - 0x8000) - player->actor.shape.rot.y));
        // This function is only called (and is only relevant) when `player->focusActor != NULL`,
        // so the MAXFLOAT case is unreachable.
        f32 distSq = ((player->focusActor == NULL) && (yawDiffAbs > 0x2AAA)) ? MAXFLOAT : actor->xyzDistToPlayerSq;

        return !Actor_anchor_ratio_check(actor, distSq * anchor_status_data[actor->attentionRangeType].lockOnLeashScale);
    }

    return false;
}

/**
 * When a given talk offer is accepted, Player will set `ACTOR_FLAG_TALK` for that actor.
 * This function serves to acknowledge that the offer was accepted by Player, and notifies the actor
 * that it should proceed with its own internal processes for handling dialogue.
 *
 * @return  true if the talk offer was accepted, false otherwise
 */
s32 Actor_talk_check(Actor* actor, PlayState* play) {
    if (actor->flags & ACTOR_FLAG_TALK) {
        actor->flags &= ~ACTOR_FLAG_TALK;
        return true;
    }

    return false;
}

/**
 * This function covers offering the ability to talk with the player.
 * Passing an exchangeItemId (see `ExchangeItemID`) allows the player to also use the item to initiate the
 * conversation.
 *
 * This function carries a talk exchange offer to the player actor if context allows it (e.g. the player is in range
 * and not busy with certain things).
 *
 * @return true If the player actor is capable of accepting the offer.
 */
s32 Actor_talk_request_get_item_set(Actor* actor, PlayState* play, f32 xzRange, f32 yRange, u32 exchangeItemId) {
    Player* player = GET_PLAYER(play);

    if ((player->actor.flags & ACTOR_FLAG_TALK) || ((exchangeItemId != EXCH_ITEM_NONE) && player_demo_check(play)) ||
        (!actor->isLockedOn &&
         ((fabsf(actor->yDistToPlayer) > yRange) || (actor->xzDistToPlayer > player->talkActorDistance) ||
          (actor->xzDistToPlayer > xzRange)))) {
        return false;
    }

    player->talkActor = actor;
    player->talkActorDistance = actor->xzDistToPlayer;
    player->exchangeItemId = exchangeItemId;

    return true;
}

/**
 * Offers a talk exchange request within an equilateral cylinder with the radius specified.
 */
s32 Actor_talk_request_get_item(Actor* actor, PlayState* play, f32 radius, u32 exchangeItemId) {
    return Actor_talk_request_get_item_set(actor, play, radius, radius, exchangeItemId);
}

/**
 * Offers a talk request within an equilateral cylinder with the radius specified.
 */
s32 Actor_talk_request2(Actor* actor, PlayState* play, f32 radius) {
    return Actor_talk_request_get_item(actor, play, radius, EXCH_ITEM_NONE);
}

/**
 * Offers a talk request within an equilateral cylinder whose radius is determined by the actor's collision check
 * cylinder's radius.
 */
s32 Actor_talk_request(Actor* actor, PlayState* play) {
    f32 cylRadius = 50.0f + actor->colChkInfo.cylRadius;

    return Actor_talk_request2(actor, play, cylRadius);
}

u32 Actor_talk_end_check(Actor* actor, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        return true;
    } else {
        return false;
    }
}

s8 Actor_get_item_check(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return player->exchangeItemId;
}

void Actor_display_position_set(PlayState* play, Actor* actor, s16* x, s16* y) {
    Vec3f projectedPos;
    f32 cappedInvW;

    projection_pos_set(play, &actor->focus.pos, &projectedPos, &cappedInvW);
    *x = projectedPos.x * cappedInvW * (SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 2);
    *y = projectedPos.y * cappedInvW * -(SCREEN_HEIGHT / 2) + (SCREEN_HEIGHT / 2);
}

u32 Actor_carry_check(Actor* actor, PlayState* play) {
    if (actor->parent != NULL) {
        return true;
    } else {
        return false;
    }
}

/**
 * This function covers various interactions with the player actor, using Get Item IDs (see `GetItemID` enum).
 * It is typically used to give items to the player, but also has other purposes.
 *
 * This function carries a get item request to the player actor if context allows it (e.g. the player is in range and
 * not busy with certain things). The player actor performs the requested action itself.
 *
 * The following description of what the `getItemId` values can do is provided here for completeness, but these
 * behaviors are entirely out of the scope of this function. All behavior is defined by the player actor.
 *
 * - Positive values (`GI_NONE < getItemId < GI_MAX`):
 *    Give an item to the player. The player may not get it immediately (for example if diving), but is expected to
 *    in the near future.
 * - Negative values (`-GI_MAX < getItemId < GI_NONE`):
 *    Used by treasure chests to indicate the chest can be opened (by pressing A).
 *    The item gotten corresponds to the positive Get Item ID `abs(getItemId)`.
 * - `GI_NONE`:
 *    Allows the player to pick up the actor (by pressing A), to carry it around.
 * - `GI_MAX`:
 *    Allows the player to catch specific actors in a bottle.
 *
 * @return true If the player actor is capable of accepting the offer.
 */
s32 Actor_carry_request_set2(Actor* actor, PlayState* play, s32 getItemId, f32 xzRange, f32 yRange) {
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 &
          (PLAYER_STATE1_DEAD | PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_13 | PLAYER_STATE1_14 |
           PLAYER_STATE1_18 | PLAYER_STATE1_19 | PLAYER_STATE1_20 | PLAYER_STATE1_21)) &&
        bom_check(player) < 0) {
        if ((((player->heldActor != NULL) || (player->talkActor == actor)) && (getItemId > GI_NONE) &&
             (getItemId < GI_MAX)) ||
            (!(player->stateFlags1 & (PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_29)))) {
            if ((actor->xzDistToPlayer < xzRange) && (fabsf(actor->yDistToPlayer) < yRange)) {
                s16 yawDiff = actor->yawTowardsPlayer - player->actor.shape.rot.y;
                s32 absYawDiff = ABS(yawDiff);

                if ((getItemId != GI_NONE) || (player->getItemDirection < absYawDiff)) {
                    player->getItemId = getItemId;
                    player->interactRangeActor = actor;
                    player->getItemDirection = absYawDiff;
                    return true;
                }
            }
        }
    }

    return false;
}

s32 Actor_carry_request_set(Actor* actor, PlayState* play, s32 getItemId) {
    return Actor_carry_request_set2(actor, play, getItemId, 50.0f, 10.0f);
}

s32 Actor_carry_request(Actor* actor, PlayState* play) {
    return Actor_carry_request_set(actor, play, GI_NONE);
}

u32 Actor_carry_end_check(Actor* actor, PlayState* play) {
    if (actor->parent == NULL) {
        return true;
    } else {
        return false;
    }
}

void Actor_carry_change(Actor* actorA, Actor* actorB, PlayState* play) {
    Actor* parent = actorA->parent;

    if (parent->id == ACTOR_PLAYER) {
        Player* player = (Player*)parent;

        player->heldActor = actorB;
        player->interactRangeActor = actorB;
    }

    parent->child = actorB;
    actorB->parent = parent;
    actorA->parent = NULL;
}

void Actor_dowsing_request(Actor* actor, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (actor->xyzDistToPlayerSq < player->closestSecretDistSq) {
        player->closestSecretDistSq = actor->xyzDistToPlayerSq;
    }
}

s32 Actor_ride_check(PlayState* play, Actor* horse) {
    if (horse->child != NULL) {
        return true;
    } else {
        return false;
    }
}

u32 Actor_ride_request(PlayState* play, Actor* horse, s32 mountSide) {
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 &
          (PLAYER_STATE1_DEAD | PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_13 |
           PLAYER_STATE1_14 | PLAYER_STATE1_18 | PLAYER_STATE1_19 | PLAYER_STATE1_20 | PLAYER_STATE1_21))) {
        player->rideActor = horse;
        player->mountSide = mountSide;
        return true;
    }

    return false;
}

s32 Actor_ride_end_check(PlayState* play, Actor* horse) {
    if (horse->child == NULL) {
        return true;
    } else {
        return false;
    }
}

/**
 * Sets the player's knockback properties
 *
 * @param play
 * @param actor source actor applying knockback damage
 * @param speed
 * @param rot the direction the player will be pushed
 * @param yVelocity
 * @param type PlayerKnockbackType
 * @param damage additional amount of damage to deal to the player
 */
void Actor_player_power_damage_set_init(PlayState* play, Actor* actor, f32 speed, s16 rot, f32 yVelocity, u32 type, u32 damage) {
    Player* player = GET_PLAYER(play);

    player->knockbackDamage = damage;
    player->knockbackType = type;
    player->knockbackSpeed = speed;
    player->knockbackRot = rot;
    player->knockbackYVelocity = yVelocity;
}

/**
 * Knocks the player to the ground
 *
 * @param play
 * @param actor source actor applying knockback damage
 * @param speed
 * @param rot the direction the player will be pushed
 * @param yVelocity
 * @param damage additional amount of damage to deal to the player
 */
void Actor_player_power_damage_AT_set(PlayState* play, Actor* actor, f32 speed, s16 rot, f32 yVelocity, u32 damage) {
    Actor_player_power_damage_set_init(play, actor, speed, rot, yVelocity, PLAYER_KNOCKBACK_LARGE, damage);
}

/**
 * Knocks the player to the ground, without applying additional damage
 *
 * @param play
 * @param actor source actor applying knockback damage
 * @param speed
 * @param rot the direction the player will be pushed
 * @param yVelocity
 */
void Actor_player_power_damage_set(PlayState* play, Actor* actor, f32 speed, s16 rot, f32 yVelocity) {
    Actor_player_power_damage_AT_set(play, actor, speed, rot, yVelocity, 0);
}

/**
 * Knocks the player back while keeping them on their feet
 *
 * @param play
 * @param actor
 * @param speed overridden
 * @param rot the direction the player will be pushed
 * @param yVelocity overridden
 * @param damage additional amount of damage to deal to the player
 */
void Actor_player_power_damage_AT_set2(PlayState* play, Actor* actor, f32 speed, s16 rot, f32 yVelocity, u32 damage) {
    Actor_player_power_damage_set_init(play, actor, speed, rot, yVelocity, PLAYER_KNOCKBACK_SMALL, damage);
}

/**
 * Knocks the player back while keeping them on their feet, without applying additional damage
 *
 * @param play
 * @param actor
 * @param speed overridden
 * @param rot the direction the player will be pushed
 * @param yVelocity overridden
 */
void Actor_player_power_damage_set2(PlayState* play, Actor* actor, f32 speed, s16 rot, f32 yVelocity) {
    Actor_player_power_damage_AT_set2(play, actor, speed, rot, yVelocity, 0);
}

/**
 * Play a sound effect at the player's position
 */
void player_SE_set(Player* player, u16 sfxId) {
    Nai_FxFlagEntry(sfxId, &player->actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
}

/**
 * Play a sound effect at the actor's position
 */
void Actor_SE_set(Actor* actor, u16 sfxId) {
    Na_StartObjectSe_F(&actor->projectedPos, sfxId);
}

void Actor_bound_SE_set(PlayState* play, Actor* actor) {
    s32 surfaceSfxOffset;

    if (actor->bgCheckFlags & BGCHECKFLAG_WATER) {
        if (actor->depthInWater < 20.0f) {
            surfaceSfxOffset = SURFACE_SFX_OFFSET_WATER_SHALLOW;
        } else {
            surfaceSfxOffset = SURFACE_SFX_OFFSET_WATER_DEEP;
        }
    } else {
        surfaceSfxOffset = T_BGCheck_getSoundGroundLabel(&play->colCtx, actor->floorPoly, actor->floorBgId);
    }

    Na_StartObjectSe_F(&actor->projectedPos, NA_SE_EV_BOMB_BOUND);
    Na_StartObjectSe_F(&actor->projectedPos, NA_SE_PL_WALK_GROUND + surfaceSfxOffset);
}

/**
 * Play a sfx at the actor's position using the shared flagged audio system
 */
void Actor_player_level_SE_set(Actor* actor, u16 sfxId) {
    actor->sfx = sfxId;
    actor->flags |= ACTOR_FLAG_SFX_ACTOR_POS_2;
    actor->flags &= ~(ACTOR_AUDIO_FLAG_SFX_CENTERED_1 | ACTOR_AUDIO_FLAG_SFX_CENTERED_2 | ACTOR_FLAG_SFX_TIMER);
}

/**
 * Play a sfx at the center of the screen using the shared flagged audio system
 */
void Actor_system_level_SE_set(Actor* actor, u16 sfxId) {
    actor->sfx = sfxId;
    actor->flags |= ACTOR_AUDIO_FLAG_SFX_CENTERED_1;
    actor->flags &= ~(ACTOR_FLAG_SFX_ACTOR_POS_2 | ACTOR_AUDIO_FLAG_SFX_CENTERED_2 | ACTOR_FLAG_SFX_TIMER);
}

/**
 * Play a sfx at the center of the screen using the shared flagged audio system
 */
void Actor_fix_level_SE_set(Actor* actor, u16 sfxId) {
    actor->sfx = sfxId;
    actor->flags |= ACTOR_AUDIO_FLAG_SFX_CENTERED_2;
    actor->flags &= ~(ACTOR_FLAG_SFX_ACTOR_POS_2 | ACTOR_AUDIO_FLAG_SFX_CENTERED_1 | ACTOR_FLAG_SFX_TIMER);
}

/**
 * Play a sfx at the actor's position using the shared flagged audio system
 */
void Actor_level_SE_set(Actor* actor, u16 sfxId) {
    actor->flags &= ~(ACTOR_FLAG_SFX_ACTOR_POS_2 | ACTOR_AUDIO_FLAG_SFX_CENTERED_1 | ACTOR_AUDIO_FLAG_SFX_CENTERED_2 |
                      ACTOR_FLAG_SFX_TIMER);
    actor->sfx = sfxId;
}

void Actor_timer_level_SE_set(Actor* actor, s32 timer) {
    actor->flags |= ACTOR_FLAG_SFX_TIMER;
    actor->flags &= ~(ACTOR_FLAG_SFX_ACTOR_POS_2 | ACTOR_AUDIO_FLAG_SFX_CENTERED_1 | ACTOR_AUDIO_FLAG_SFX_CENTERED_2);

    // The sfx field is not used for an actual sound effect here.
    // Instead, it controls the tick speed of the timer sound effect.
    if (timer < 40) {
        actor->sfx = 3;
    } else if (timer < 100) {
        actor->sfx = 2;
    } else {
        actor->sfx = 1;
    }
}

// Tests if something hit Jabu Jabu surface, displaying hit splash and playing sfx if true
s32 jyabujyabu_kiru_check(PlayState* play, Actor* actor, CollisionPoly* poly, s32 bgId, Vec3f* pos) {
    if (T_BGCheck_getAttributeCode_ai(&play->colCtx, poly, bgId) == FLOOR_TYPE_8) {
        play->roomCtx.drawParams[0] = 1;
        CollisionCheck_setBlueBlood(play, NULL, pos);
        Actor_SE_set(actor, NA_SE_IT_WALL_HIT_BUYO);
        return true;
    }

    return false;
}

#pragma increment_block_number "gc-eu:22 gc-eu-mq:22 gc-jp:22 gc-jp-ce:22 gc-jp-mq:22 gc-us:22 gc-us-mq:22" \
                               "ntsc-1.0:22 ntsc-1.1:22 ntsc-1.2:22 pal-1.0:22 pal-1.1:22 hiratsu3:22"

// Local data used for Farore's Wind light (stored in BSS)
LightInfo magic_window_light_data;
LightNode* magic_window_light_list;
s32 pointer_move_wait;
f32 pointer_move_counter;

void magic_window_pointer_ct(PlayState* play) {
    f32 lightPosX;
    f32 lightPosY;
    f32 lightPosZ;

    if (z_common_data.save.info.fw.set) {
        z_common_data.respawn[RESPAWN_MODE_TOP].data = 0x28;
        z_common_data.respawn[RESPAWN_MODE_TOP].pos.x = z_common_data.save.info.fw.pos.x;
        z_common_data.respawn[RESPAWN_MODE_TOP].pos.y = z_common_data.save.info.fw.pos.y;
        z_common_data.respawn[RESPAWN_MODE_TOP].pos.z = z_common_data.save.info.fw.pos.z;
        z_common_data.respawn[RESPAWN_MODE_TOP].yaw = z_common_data.save.info.fw.yaw;
        z_common_data.respawn[RESPAWN_MODE_TOP].playerParams = z_common_data.save.info.fw.playerParams;
        z_common_data.respawn[RESPAWN_MODE_TOP].entranceIndex = z_common_data.save.info.fw.entranceIndex;
        z_common_data.respawn[RESPAWN_MODE_TOP].roomIndex = z_common_data.save.info.fw.roomIndex;
        z_common_data.respawn[RESPAWN_MODE_TOP].tempSwchFlags = z_common_data.save.info.fw.tempSwchFlags;
        z_common_data.respawn[RESPAWN_MODE_TOP].tempCollectFlags = z_common_data.save.info.fw.tempCollectFlags;
    } else {
        z_common_data.respawn[RESPAWN_MODE_TOP].data = 0;
        z_common_data.respawn[RESPAWN_MODE_TOP].pos.x = 0.0f;
        z_common_data.respawn[RESPAWN_MODE_TOP].pos.y = 0.0f;
        z_common_data.respawn[RESPAWN_MODE_TOP].pos.z = 0.0f;
    }

    // clang-format off
    lightPosX = z_common_data.respawn[RESPAWN_MODE_TOP].pos.x; \
    lightPosY = z_common_data.respawn[RESPAWN_MODE_TOP].pos.y + 80.0f; \
    lightPosZ = z_common_data.respawn[RESPAWN_MODE_TOP].pos.z; \
    Light_point_ct(&magic_window_light_data, lightPosX, lightPosY, lightPosZ, 0xFF, 0xFF, 0xFF, -1);
    // clang-format on

    magic_window_light_list = Global_light_list_new(play, &play->lightCtx, &magic_window_light_data);
    pointer_move_wait = 0;
    pointer_move_counter = 0.0f;
}

void magic_window_pointer_draw(PlayState* play) {
    s32 lightRadius = -1;
    s32 params;

    OPEN_DISPS(play->state.gfxCtx, "../z_actor.c", 5308);

    params = z_common_data.respawn[RESPAWN_MODE_TOP].data;

    if (params) {
        f32 yOffset = LINK_IS_ADULT ? 80.0f : 60.0f;
        f32 ratio = 1.0f;
        s32 alpha = 255;
        s32 temp = params - 40;

        if (temp < 0) {
            z_common_data.respawn[RESPAWN_MODE_TOP].data = ++params;
            ratio = ABS(params) * 0.025f;
            pointer_move_wait = 60;
            pointer_move_counter = 1.0f;
        } else if (pointer_move_wait) {
            pointer_move_wait--;
        } else if (pointer_move_counter > 0.0f) {
            static Vec3f kirakira_vec = { 0.0f, -0.05f, 0.0f };
            static Vec3f kirakira_acc = { 0.0f, -0.025f, 0.0f };
            static Color_RGBA8 kirakira_prim = { 255, 255, 255, 0 };
            static Color_RGBA8 kirakira_env = { 100, 200, 0, 0 };
            Vec3f* curPos = &z_common_data.respawn[RESPAWN_MODE_TOP].pos;
            Vec3f* nextPos = &z_common_data.respawn[RESPAWN_MODE_DOWN].pos;
            f32 prevNum = pointer_move_counter;
            Vec3f dist;
            f32 diff = search_position_distance2(nextPos, curPos, &dist);
            Vec3f effectPos;
            f32 factor;
            f32 length;
            f32 dx;
            f32 speed;

            if (diff < 20.0f) {
                pointer_move_counter = 0.0f;
                xyz_t_move(curPos, nextPos);
            } else {
                length = diff * (1.0f / pointer_move_counter);
                speed = 20.0f / length;
                if (speed < 0.05f) {
                    speed = 0.05f;
                }
                chase_f(&pointer_move_counter, 0.0f, speed);
                factor = (diff * (pointer_move_counter / prevNum)) / diff;
                curPos->x = nextPos->x + (dist.x * factor);
                curPos->y = nextPos->y + (dist.y * factor);
                curPos->z = nextPos->z + (dist.z * factor);
                length *= 0.5f;
                dx = diff - length;
                yOffset += sqrtf(SQ(length) - SQ(dx)) * 0.2f;
                PRINTF("-------- DISPLAY Y=%f\n", yOffset);
            }

            effectPos.x = curPos->x + rnd_fx(6.0f);
            effectPos.y = curPos->y + 80.0f + (6.0f * fqrand());
            effectPos.z = curPos->z + rnd_fx(6.0f);

            Effect_SS_KiraKira_sc_ct_ct(play, &effectPos, &kirakira_vec, &kirakira_acc, &kirakira_prim, &kirakira_env,
                                            1000, 16);

            if (pointer_move_counter == 0.0f) {
                z_common_data.respawn[RESPAWN_MODE_TOP] = z_common_data.respawn[RESPAWN_MODE_DOWN];
                z_common_data.respawn[RESPAWN_MODE_TOP].playerParams =
                    PLAYER_PARAMS(PLAYER_START_MODE_FARORES_WIND, PLAYER_START_BG_CAM_DEFAULT);
                z_common_data.respawn[RESPAWN_MODE_TOP].data = 40;
            }

            z_common_data.respawn[RESPAWN_MODE_TOP].pos = *curPos;
        } else if (temp > 0) {
            Vec3f* curPos = &z_common_data.respawn[RESPAWN_MODE_TOP].pos;
            f32 nextRatio = 1.0f - temp * 0.1f;
            f32 curRatio = 1.0f - (f32)(temp - 1) * 0.1f;
            Vec3f eye;
            Vec3f dist;
            f32 diff;

            if (nextRatio > 0.0f) {
                eye.x = play->view.eye.x;
                eye.y = play->view.eye.y - yOffset;
                eye.z = play->view.eye.z;
                diff = search_position_distance2(&eye, curPos, &dist);
                diff = (diff * (nextRatio / curRatio)) / diff;
                curPos->x = eye.x + (dist.x * diff);
                curPos->y = eye.y + (dist.y * diff);
                curPos->z = eye.z + (dist.z * diff);
                z_common_data.respawn[RESPAWN_MODE_TOP].pos = *curPos;
            }

            alpha = 255 - (temp * 30);

            if (alpha < 0) {
                z_common_data.save.info.fw.set = 0;
                z_common_data.respawn[RESPAWN_MODE_TOP].data = 0;
                alpha = 0;
            } else {
                z_common_data.respawn[RESPAWN_MODE_TOP].data = ++params;
            }

            ratio = 1.0f + ((f32)temp * 0.2); // required to match
        }

        lightRadius = 500.0f * ratio;

        //! @bug One of the conditions for this block checks an entrance index to see if the light ball should draw.
        //! This does not account for the fact that some dungeons have multiple entrances.
        //! If a dungeon is entered through a different entrance than the one that was saved, the light ball will not
        //! draw.
        if ((play->csCtx.state == CS_STATE_IDLE) &&
            (((void)0, z_common_data.respawn[RESPAWN_MODE_TOP].entranceIndex) ==
             ((void)0, z_common_data.save.entranceIndex)) &&
            (((void)0, z_common_data.respawn[RESPAWN_MODE_TOP].roomIndex) == play->roomCtx.curRoom.num)) {
            f32 scale = 0.025f * ratio;

            POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_25);

            Matrix_translate(((void)0, z_common_data.respawn[RESPAWN_MODE_TOP].pos.x),
                             ((void)0, z_common_data.respawn[RESPAWN_MODE_TOP].pos.y) + yOffset,
                             ((void)0, z_common_data.respawn[RESPAWN_MODE_TOP].pos.z), MTXMODE_NEW);
            Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
            Matrix_mult(&play->billboardMtxF, MTXMODE_APPLY);
            Matrix_push();

            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 128, 128, 255, 255, 200, alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 100, 200, 0, 255);

            Matrix_rotateZ(BINANG_TO_RAD_ALT2((play->gameplayFrames * 1500) & 0xFFFF), MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_actor.c", 5458);
            gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);

            Matrix_pull();
            Matrix_rotateZ(BINANG_TO_RAD_ALT2(~((play->gameplayFrames * 1200) & 0xFFFF)), MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_actor.c", 5463);
            gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);
        }

        //! @bug This function call is not contained in the above block, meaning the light for Farore's Wind will draw
        //! in every scene at the same position that it was originally set.
        Light_point_ct(&magic_window_light_data, ((void)0, z_common_data.respawn[RESPAWN_MODE_TOP].pos.x),
                                  ((void)0, z_common_data.respawn[RESPAWN_MODE_TOP].pos.y) + yOffset,
                                  ((void)0, z_common_data.respawn[RESPAWN_MODE_TOP].pos.z), 255, 255, 255, lightRadius);

        CLOSE_DISPS(play->state.gfxCtx, "../z_actor.c", 5474);
    }
}

void magic_window_pointer_dt(PlayState* play) {
    Global_light_list_delete(play, &play->lightCtx, magic_window_light_list);
}

void magic_grass_cancel(PlayState* play) {
    if (play->actorCtx.lensActive) {
        play->actorCtx.lensActive = false;
        magic_cancel_check(play);
    }
}

void Actor_info_ct(PlayState* play, ActorContext* actorCtx, ActorEntry* playerEntry) {
    ActorOverlay* overlayEntry;
    SavedSceneFlags* savedSceneFlags;
    s32 i;

    savedSceneFlags = &z_common_data.save.info.sceneFlags[play->sceneId];

    bzero(actorCtx, sizeof(ActorContext));

    actor_dlftbls_init();
    Matrix_copy_MtxF(&play->billboardMtxF, &MtxF_clear);
    Matrix_copy_MtxF(&play->viewProjectionMtxF, &MtxF_clear);

    overlayEntry = &actor_dlftbls[0];
    for (i = 0; i < ARRAY_COUNT(actor_dlftbls); i++) {
        overlayEntry->loadedRamAddr = NULL;
        overlayEntry->numLoaded = 0;
        overlayEntry++;
    }

    actorCtx->flags.chest = savedSceneFlags->chest;
    actorCtx->flags.swch = savedSceneFlags->swch;
    actorCtx->flags.clear = savedSceneFlags->clear;
    actorCtx->flags.collect = savedSceneFlags->collect;

    Actor_Name_Disp_ct(play, &actorCtx->titleCtx);

    actorCtx->absoluteSpace = NULL;

    Actor_info_new(actorCtx, playerEntry, play);
    Anchor_Marker_init(&actorCtx->attention, actorCtx->actorLists[ACTORCAT_PLAYER].head, play);
    magic_window_pointer_ct(play);
}

u32 move_stop_flag[ACTORCAT_MAX] = {
    // ACTORCAT_SWITCH
    PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_28,
    // ACTORCAT_BG
    PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_28,
    // ACTORCAT_PLAYER
    0,
    // ACTORCAT_EXPLOSIVE
    PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_10 | PLAYER_STATE1_28,
    // ACTORCAT_NPC
    PLAYER_STATE1_DEAD,
    // ACTORCAT_ENEMY
    PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_28 | PLAYER_STATE1_29,
    // ACTORCAT_PROP
    PLAYER_STATE1_DEAD | PLAYER_STATE1_28,
    // ACTORCAT_ITEMACTION
    0,
    // ACTORCAT_MISC
    PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_28 | PLAYER_STATE1_29,
    // ACTORCAT_BOSS
    PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_10 | PLAYER_STATE1_28,
    // ACTORCAT_DOOR
    0,
    // ACTORCAT_CHEST
    PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_28,
};

void Actor_info_call_actor(PlayState* play, ActorContext* actorCtx) {
    s32 i;
    Actor* actor;
    Player* player;
    u32* categoryFreezeMaskP;
    u32 freezeExceptionFlag;
    u32 canFreezeCategory;
    Actor* sp74;
    ActorEntry* actorEntry;

    player = GET_PLAYER(play);

#if DEBUG_FEATURES
    if (0) {
        // This ASSERT is optimized out, but it can be assumed to exist because its string is present in rodata
        ASSERT(actor_dlftbls_num == ACTOR_ID_MAX, "MaxProfile == ACTOR_DLF_MAX", "../z_actor.c", UNK_LINE);
    }
#endif

    sp74 = NULL;
    freezeExceptionFlag = 0;

    if (play->numActorEntries != 0) {
        actorEntry = &play->actorEntryList[0];
        for (i = 0; i < play->numActorEntries; i++) {
            Actor_info_new(&play->actorCtx, actorEntry++, play);
        }
        play->numActorEntries = 0;
    }

    if (actorCtx->unk_02 != 0) {
        actorCtx->unk_02--;
    }

#if DEBUG_FEATURES
    if (KREG(0) == -100) {
        Actor* player = &GET_PLAYER(play)->actor;

        KREG(0) = 0;
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, player->world.pos.x, player->world.pos.y + 100.0f,
                    player->world.pos.z, 0, 0, 0, 1);
    }
#endif

    categoryFreezeMaskP = &move_stop_flag[0];

    if (player->stateFlags2 & PLAYER_STATE2_USING_OCARINA) {
        freezeExceptionFlag = ACTOR_FLAG_UPDATE_DURING_OCARINA;
    }

    if ((player->stateFlags1 & PLAYER_STATE1_TALKING) && ((player->actor.textId & 0xFF00) != 0x600)) {
        sp74 = player->talkActor;
    }

    for (i = 0; i < ARRAY_COUNT(actorCtx->actorLists); i++, categoryFreezeMaskP++) {
        canFreezeCategory = (player->stateFlags1 & *categoryFreezeMaskP);

        actor = actorCtx->actorLists[i].head;
        while (actor != NULL) {
            if (actor->world.pos.y < -25000.0f) {
                actor->world.pos.y = -25000.0f;
            }

            actor->sfx = 0;

            if (actor->init != NULL) {
                if (Object_Exchange_bank_dma_check(&play->objectCtx, actor->objectSlot)) {
                    Actor_set_segment(play, actor);
                    actor->init(actor, play);
                    actor->init = NULL;
                }
                actor = actor->next;
            } else if (!Object_Exchange_bank_dma_check(&play->objectCtx, actor->objectSlot)) {
                Actor_delete(actor);
                actor = actor->next;
            } else if ((freezeExceptionFlag != 0 && !(actor->flags & freezeExceptionFlag)) ||
                       (freezeExceptionFlag == 0 && canFreezeCategory &&
                        !((sp74 == actor) || (player->naviActor == actor) || (player->heldActor == actor) ||
                          (actor->parent == &player->actor)))) {
                CollisionCheck_Status_Clear(&actor->colChkInfo);
                actor = actor->next;
            } else if (actor->update == NULL) {
                if (!actor->isDrawn) {
                    actor = Actor_info_delete(&play->actorCtx, actor, play);
                } else {
                    Actor_dt(actor, play);
                    actor = actor->next;
                }
            } else {
                xyz_t_move(&actor->prevPos, &actor->world.pos);
                actor->xzDistToPlayer = Actor_search_actor_distanceXZ(actor, &player->actor);
                actor->yDistToPlayer = Actor_search_actor_high(actor, &player->actor);
                actor->xyzDistToPlayerSq = SQ(actor->xzDistToPlayer) + SQ(actor->yDistToPlayer);

                actor->yawTowardsPlayer = Actor_search_actor_angleY(actor, &player->actor);
                actor->flags &= ~ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;

                if ((DECR(actor->freezeTimer) == 0) &&
                    (actor->flags & (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_INSIDE_CULLING_VOLUME))) {
                    if (actor == player->focusActor) {
                        actor->isLockedOn = true;
                    } else {
                        actor->isLockedOn = false;
                    }

                    if ((actor->attentionPriority != 0) && (player->focusActor == NULL)) {
                        actor->attentionPriority = 0;
                    }

                    Actor_set_segment(play, actor);
                    if (actor->colorFilterTimer != 0) {
                        actor->colorFilterTimer--;
                    }
                    actor->update(actor, play);
                    DynaPolyInfo_statusClear(play, &play->colCtx.dyna, actor);
                }

                CollisionCheck_Status_Clear(&actor->colChkInfo);

                actor = actor->next;
            }
        }

        if (i == ACTORCAT_BG) {
            DynaPolyInfo_setup(play, &play->colCtx.dyna);
        }
    }

    actor = player->focusActor;

    if ((actor != NULL) && (actor->update == NULL)) {
        actor = NULL;
        anchor_cancel(player);
    }

    if ((actor == NULL) || (player->zTargetActiveTimer < 5)) {
        if (actorCtx->attention.reticleSpinCounter != 0) {
            actorCtx->attention.reticleSpinCounter = 0;
            Na_StartSystemSe_F(NA_SE_SY_LOCK_OFF);
        }
        actor = NULL;
    }

    Anchor_Marker_process(&actorCtx->attention, player, actor, play);
    Actor_Name_Disp_Move(play, &actorCtx->titleCtx);
    DynaPolyInfo_cleanUp(play, &play->colCtx.dyna);
}

void display_actor(Actor* actor, char* command) {
    ActorOverlay* overlayEntry;
    char* name;

    if ((actor == NULL) || (actor->overlayEntry == NULL)) {
        faultprint_Locate(48, 24);
        faultprint_Printf("ACTOR NAME is NULL");
    }

#if DEBUG_FEATURES
    overlayEntry = actor->overlayEntry;
    name = overlayEntry->name != NULL ? overlayEntry->name : "";
#else
    name = "";
#endif

    PRINTF(T("アクターの名前(%08x:%s)\n", "Actor name (%08x:%s)\n"), actor, name);

    if (command != NULL) {
        PRINTF(T("コメント:%s\n", "Command: %s\n"), command);
    }

    faultprint_Locate(48, 24);
    faultprint_Printf("ACTOR NAME %08x:%s", actor, name);
}

void Actor_draw(PlayState* play, Actor* actor) {
    FaultClient faultClient;
    Lights* lights;
#if PLATFORM_IQUE
    ObjectEntry* slots;
#endif

    fault_AddClient(&faultClient, display_actor, actor, "Actor_draw");

    OPEN_DISPS(play->state.gfxCtx, "../z_actor.c", 6035);

    lights = Global_light_read(&play->lightCtx, play->state.gfxCtx);

    LightsN_list_check(lights, play->lightCtx.listHead,
                   (actor->flags & ACTOR_FLAG_IGNORE_POINT_LIGHTS) ? NULL : &actor->world.pos);
    LightsN_disp(lights, play->state.gfxCtx);

    if (actor->flags & ACTOR_FLAG_IGNORE_QUAKE) {
        Matrix_softcv3_load(actor->world.pos.x + play->mainCamera.quakeOffset.x,
                                     actor->world.pos.y +
                                         ((actor->shape.yOffset * actor->scale.y) + play->mainCamera.quakeOffset.y),
                                     actor->world.pos.z + play->mainCamera.quakeOffset.z, &actor->shape.rot);
    } else {
        Matrix_softcv3_load(actor->world.pos.x, actor->world.pos.y + (actor->shape.yOffset * actor->scale.y),
                                     actor->world.pos.z, &actor->shape.rot);
    }

    Matrix_scale(actor->scale.x, actor->scale.y, actor->scale.z, MTXMODE_APPLY);
    Actor_set_segment(play, actor);

#if !PLATFORM_IQUE
    gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[actor->objectSlot].segment);
    gSPSegment(POLY_XLU_DISP++, 0x06, play->objectCtx.slots[actor->objectSlot].segment);
#else
    // Workaround for EGCS internal compiler error (see docs/compilers.md)
    slots = play->objectCtx.slots;
    gSPSegment(POLY_OPA_DISP++, 0x06, slots[actor->objectSlot].segment);
    gSPSegment(POLY_XLU_DISP++, 0x06, slots[actor->objectSlot].segment);
#endif

    if (actor->colorFilterTimer != 0) {
        Color_RGBA8 color = { 0, 0, 0, 255 };

        if (actor->colorFilterParams & COLORFILTER_COLORFLAG_GRAY) {
            color.r = color.g = color.b = COLORFILTER_GET_COLORINTENSITY(actor->colorFilterParams) | 7;
        } else if (actor->colorFilterParams & COLORFILTER_COLORFLAG_RED) {
            color.r = COLORFILTER_GET_COLORINTENSITY(actor->colorFilterParams) | 7;
        } else {
            color.b = COLORFILTER_GET_COLORINTENSITY(actor->colorFilterParams) | 7;
        }

        if (actor->colorFilterParams & COLORFILTER_BUFFLAG_XLU) {
            Eff_Set_Fog3_xlu(play, &color, actor->colorFilterTimer, COLORFILTER_GET_DURATION(actor->colorFilterParams));
        } else {
            Eff_Set_Fog3(play, &color, actor->colorFilterTimer, COLORFILTER_GET_DURATION(actor->colorFilterParams));
        }
    }

    actor->draw(actor, play);

    if (actor->colorFilterTimer != 0) {
        if (actor->colorFilterParams & COLORFILTER_BUFFLAG_XLU) {
            Eff_Off_Fog_xlu(play);
        } else {
            Eff_Off_Fog(play);
        }
    }

    if (actor->shape.shadowDraw != NULL) {
        actor->shape.shadowDraw(actor, lights, play);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_actor.c", 6119);

    fault_RemoveClient(&faultClient);
}

void level_SE_set(Actor* actor) {
    if (actor->flags & ACTOR_FLAG_SFX_ACTOR_POS_2) {
        Nai_FxFlagEntry(actor->sfx, &actor->projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (actor->flags & ACTOR_AUDIO_FLAG_SFX_CENTERED_1) {
        Na_StartSystemSe_F(actor->sfx);
    } else if (actor->flags & ACTOR_AUDIO_FLAG_SFX_CENTERED_2) {
        Na_StartFixSe_F(actor->sfx);
    } else if (actor->flags & ACTOR_FLAG_SFX_TIMER) {
        Na_StartInputPortSe(&_dummy_zero_f, NA_SE_SY_TIMER - SFX_FLAG, (s8)(actor->sfx - 1));
    } else {
        Na_StartObjectSe_F(&actor->projectedPos, actor->sfx);
    }
}

#define LENS_MASK_WIDTH 64
#define LENS_MASK_HEIGHT 64
// 26 and 6 are for padding between the mask texture and the screen borders
#define LENS_MASK_OFFSET_S ((SCREEN_WIDTH / 2 - LENS_MASK_WIDTH) - 26)
#define LENS_MASK_OFFSET_T ((SCREEN_HEIGHT / 2 - LENS_MASK_HEIGHT) - 6)

void Actor_info_draw_actor_glass_txt_draw(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_actor.c", 6161);

    gDPLoadTextureBlock(POLY_XLU_DISP++, gLensOfTruthMaskTex, G_IM_FMT_I, G_IM_SIZ_8b, LENS_MASK_WIDTH,
                        LENS_MASK_HEIGHT, 0, G_TX_MIRROR | G_TX_CLAMP, G_TX_MIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD,
                        G_TX_NOLOD);

    gDPSetTileSize(POLY_XLU_DISP++, G_TX_RENDERTILE, (SCREEN_WIDTH / 2 - LENS_MASK_WIDTH) << 2,
                   (SCREEN_HEIGHT / 2 - LENS_MASK_HEIGHT) << 2, (SCREEN_WIDTH / 2 + LENS_MASK_WIDTH - 1) << 2,
                   (SCREEN_HEIGHT / 2 + LENS_MASK_HEIGHT - 1) << 2);
    gSPTextureRectangle(POLY_XLU_DISP++, 0, 0, SCREEN_WIDTH << 2, SCREEN_HEIGHT << 2, G_TX_RENDERTILE,
                        LENS_MASK_OFFSET_S << 5, LENS_MASK_OFFSET_T << 5,
                        (1 << 10) * (SCREEN_WIDTH - 2 * LENS_MASK_OFFSET_S) / SCREEN_WIDTH,
                        (1 << 10) * (SCREEN_HEIGHT - 2 * LENS_MASK_OFFSET_T) / SCREEN_HEIGHT);
    gDPPipeSync(POLY_XLU_DISP++);

    CLOSE_DISPS(gfxCtx, "../z_actor.c", 6183);
}

void Actor_info_draw_actor_glass_draw(PlayState* play, s32 numInvisibleActors, Actor** invisibleActors) {
    Actor** invisibleActor;
    GraphicsContext* gfxCtx;
    s32 i;

    gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_actor.c", 6197);

    gDPNoOpString(POLY_OPA_DISP++, "魔法のメガネ START", 0); // "Magic lens START"

    gDPPipeSync(POLY_XLU_DISP++);

    if (play->roomCtx.curRoom.lensMode == LENS_MODE_SHOW_ACTORS) {
        // Update both the color frame buffer and the z-buffer
        gDPSetOtherMode(POLY_XLU_DISP++,
                        G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                            G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                        G_AC_THRESHOLD | G_ZS_PRIM | Z_UPD | G_RM_CLD_SURF | G_RM_CLD_SURF2);

        gDPSetCombineMode(POLY_XLU_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 0, 0, 255);

        // the z-buffer will later only allow drawing inside the lens circle
    } else {
        // Update the z-buffer but not the color frame buffer
        gDPSetOtherMode(POLY_XLU_DISP++,
                        G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                            G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                        G_AC_THRESHOLD | G_ZS_PRIM | Z_UPD | IM_RD | CVG_DST_SAVE | ZMODE_OPA | FORCE_BL |
                            GBL_c1(G_BL_CLR_BL, G_BL_0, G_BL_CLR_MEM, G_BL_1MA) |
                            GBL_c2(G_BL_CLR_BL, G_BL_0, G_BL_CLR_MEM, G_BL_1MA));

        // inverts the mask image, which initially is 0 inner and 74 outer,
        // by setting the combiner to draw 74 - image instead of the image
        gDPSetCombineLERP(POLY_XLU_DISP++, PRIMITIVE, TEXEL0, PRIM_LOD_FRAC, 0, PRIMITIVE, TEXEL0, PRIM_LOD_FRAC, 0,
                          PRIMITIVE, TEXEL0, PRIM_LOD_FRAC, 0, PRIMITIVE, TEXEL0, PRIM_LOD_FRAC, 0);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0xFF, 74, 74, 74, 74);

        // the z-buffer will later only allow drawing outside the lens circle
    }

    // Together with the depth source set above, this sets the depth to the closest.
    // For a pixel with such a depth value, the z-buffer will reject drawing to that pixel.
    gDPSetPrimDepth(POLY_XLU_DISP++, 0, 0);

    // The z-buffer will be updated where the mask is not fully transparent.
    Actor_info_draw_actor_glass_txt_draw(gfxCtx);

    // "Magic lens invisible Actor display START"
    gDPNoOpString(POLY_OPA_DISP++, "魔法のメガネ 見えないＡcｔｏｒ表示 START", numInvisibleActors);

    invisibleActor = &invisibleActors[0];
    for (i = 0; i < numInvisibleActors; i++) {
        // "Magic lens invisible Actor display"
        gDPNoOpString(POLY_OPA_DISP++, "魔法のメガネ 見えないＡcｔｏｒ表示", i);
        Actor_draw(play, *(invisibleActor++));
    }

    // "Magic lens invisible Actor display END"
    gDPNoOpString(POLY_OPA_DISP++, "魔法のメガネ 見えないＡcｔｏｒ表示 END", numInvisibleActors);

    if (play->roomCtx.curRoom.lensMode != LENS_MODE_SHOW_ACTORS) {
        // Draw the lens overlay to the color frame buffer

        gDPNoOpString(POLY_OPA_DISP++, "青い眼鏡(外側)", 0); // "Blue spectacles (exterior)"

        gDPPipeSync(POLY_XLU_DISP++);

        gDPSetOtherMode(POLY_XLU_DISP++,
                        G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                            G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                        G_AC_THRESHOLD | G_ZS_PRIM | G_RM_CLD_SURF | G_RM_CLD_SURF2);
        gDPSetCombineMode(POLY_XLU_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 0, 0, 255);

        Actor_info_draw_actor_glass_txt_draw(gfxCtx);

        gDPNoOpString(POLY_OPA_DISP++, "青い眼鏡(外側)", 1); // "Blue spectacles (exterior)"
    }

    gDPNoOpString(POLY_OPA_DISP++, "魔法のメガネ END", 0); // "Magic lens END"

    CLOSE_DISPS(gfxCtx, "../z_actor.c", 6284);
}

/**
 * Checks if an actor should be culled or not, by seeing if it is contained within its own culling volume.
 * For more details on the culling test, see `Actor_draw_actor_no_culling_check2`.
 *
 * Returns true if the actor is inside its culling volume. In other words, it should not cull.
 *
 * "Culling" in this context refers to the removal of something for the sake of improving performance.
 * For actors, being culled means that their Update and Draw processes are halted.
 * While halted, an Actor's update state is frozen and it will not draw, making it invisible.
 *
 * Actors that are within the bounds of their culling volume may update and draw, while actors that are
 * out of bounds of its culling volume may be excluded from updating and drawing until they are within bounds.
 *
 * It is possible for actors to opt out of update culling or draw culling.
 * This is set per-actor with `ACTOR_FLAG_UPDATE_CULLING_DISABLED` and `ACTOR_FLAG_DRAW_CULLING_DISABLED`.
 *
 * Note: Even if either `ACTOR_FLAG_UPDATE_CULLING_DISABLED` or `ACTOR_FLAG_DRAW_CULLING_DISABLED` are set, the actor
 * will still undergo the culling test and set `ACTOR_FLAG_INSIDE_CULLING_VOLUME` accordingly.
 * So, `ACTOR_FLAG_INSIDE_CULLING_VOLUME` cannot be used on it own to determine if an actor is actually culled.
 * It simply says whether or not they are physically located within the bounds of the culling volume.
 */
s32 Actor_draw_actor_no_culling_check(PlayState* play, Actor* actor) {
    return Actor_draw_actor_no_culling_check2(play, actor, &actor->projectedPos, actor->projectedW);
}

/**
 * Tests if an actor is currently within the bounds of its own culling volume.
 *
 * The culling volume is a 3D shape composed of a frustum with a box attached to the end of it. The frustum sits at the
 * camera's position and projects forward, encompassing the player's current view; the box extrudes behind the camera,
 * allowing actors in the immediate vicinity behind and to the sides of the camera to be detected.
 *
 * This function returns true if the actor is within bounds, false if not.
 * The comparison is done in projected space against the actor's projected position as the viewing frustum
 * in world space transforms to a box in projected space, making the calculation easy.
 *
 * Every actor can set properties for their own culling volume, changing its dimensions to suit the needs of
 * it and its environment. These properties are in units of projected space (i.e. compared to the actor's position
 * after perspective projection is applied) are therefore not directly comparable to world units.
 * These depend on the current view parameters (fov, aspect, scale, znear, zfar).
 * The default parameters considered are (60 degrees, 4/3, 1.0, 10, 12800).
 *
 *    cullingVolumeDistance: Configures how far forward the far plane of the frustum should extend.
 *                           This along with cullingVolumeScale determines the maximum distance from
 *                           the camera eye that the actor can be detected at. This quantity is related
 *                           to world units by a factor of
 *                                   (znear - zfar) / ((znear + zfar) * scale).
 *                           For default view parameters, increasing this property by 1 increases the
 *                           distance by ~0.995 world units.
 *
 *    cullingVolumeScale: Scales the entire culling volume in all directions except the downward
 *                        direction. Both the frustum and the box will scale in size. This quantity is
 *                        related to world units by different factors based on direction:
 *                         - For the forward and backward directions, they are related in the same way
 *                           as above. For default view parameters, increasing this property by 1 increases
 *                           the forward and backward scales by ~0.995 world units.
 *                         - For the sideways directions, the relation to world units is
 *                                   (aspect / scale) * tan(0.5 * fov)
 *                           For default view parameters, increasing this property by 1 increases the
 *                           sideways scales by ~0.77 world units.
 *                         - For the upward direction, the relation to world units is
 *                                   (1 / scale) * tan(0.5 * fov)
 *                           For default view parameters, increasing this property by 1 increases the
 *                           scale by ~0.58 world units.
 *
 *    cullingVolumeDownward: Sets the height of the culling volume in the downward direction. Increasing
 *                           this value will make actors below the camera more easily detected. This
 *                           quantity is related to world units by the same factor as the upward scale.
 *                           For default view parameters, increasing this property by 1 increases the
 *                           downward height by ~0.58 world units.
 *
 * This interactive 3D graph visualizes the shape of the culling volume and has sliders for the 3 properties mentioned
 * above: https://www.desmos.com/3d/4ztkxqky2a.
 */
s32 Actor_draw_actor_no_culling_check2(PlayState* play, Actor* actor, Vec3f* projPos, f32 projW) {
    f32 invW;

    if ((projPos->z > -actor->cullingVolumeScale) &&
        (projPos->z < (actor->cullingVolumeDistance + actor->cullingVolumeScale))) {
        // Clamping `projW` affects points behind the camera, so that the culling volume has
        // a frustum shape in front of the camera and a box shape behind the camera.
        invW = (projW < 1.0f) ? 1.0f : 1.0f / (f32)projW;

        if ((((fabsf(projPos->x) - actor->cullingVolumeScale) * invW) < 1.0f) &&
            (((projPos->y + actor->cullingVolumeDownward) * invW) > -1.0f) &&
            (((projPos->y - actor->cullingVolumeScale) * invW) < 1.0f)) {
            return true;
        }
    }

    return false;
}

void Actor_info_draw_actor(PlayState* play, ActorContext* actorCtx) {
    s32 invisibleActorCounter;
    Actor* invisibleActors[INVISIBLE_ACTOR_MAX];
    ActorListEntry* actorListEntry;
    Actor* actor;
    s32 i;

    invisibleActorCounter = 0;

    OPEN_DISPS(play->state.gfxCtx, "../z_actor.c", 6336);

    actorListEntry = &actorCtx->actorLists[0];

    for (i = 0; i < ARRAY_COUNT(actorCtx->actorLists); i++, actorListEntry++) {
        actor = actorListEntry->head;

        while (actor != NULL) {
            ActorOverlay* overlayEntry = actor->overlayEntry;
            char* actorName;

#if DEBUG_FEATURES
            actorName = overlayEntry->name != NULL ? overlayEntry->name : "";
#else
            actorName = "";
#endif

            gDPNoOpString(POLY_OPA_DISP++, actorName, i);
            gDPNoOpString(POLY_XLU_DISP++, actorName, i);

            if (DEBUG_FEATURES) {
                HREG(66) = i;
            }

            if (!DEBUG_FEATURES || (HREG(64) != 1) || ((HREG(65) != -1) && (HREG(65) != HREG(66))) || (HREG(68) == 0)) {
                Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, &actor->world.pos, &actor->projectedPos,
                                             &actor->projectedW);
            }

            if (!DEBUG_FEATURES || (HREG(64) != 1) || ((HREG(65) != -1) && (HREG(65) != HREG(66))) || (HREG(69) == 0)) {
                if (actor->sfx != 0) {
                    level_SE_set(actor);
                }
            }

            if (!DEBUG_FEATURES || (HREG(64) != 1) || ((HREG(65) != -1) && (HREG(65) != HREG(66))) || (HREG(70) == 0)) {
                if (Actor_draw_actor_no_culling_check(play, actor)) {
                    actor->flags |= ACTOR_FLAG_INSIDE_CULLING_VOLUME;
                } else {
                    actor->flags &= ~ACTOR_FLAG_INSIDE_CULLING_VOLUME;
                }
            }

            actor->isDrawn = false;

            if (!DEBUG_FEATURES || (HREG(64) != 1) || ((HREG(65) != -1) && (HREG(65) != HREG(66))) || (HREG(71) == 0)) {
                if ((actor->init == NULL) && (actor->draw != NULL) &&
                    (actor->flags & (ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_INSIDE_CULLING_VOLUME))) {
                    if ((actor->flags & ACTOR_FLAG_REACT_TO_LENS) &&
                        ((play->roomCtx.curRoom.lensMode == LENS_MODE_SHOW_ACTORS) || play->actorCtx.lensActive ||
                         (actor->room != play->roomCtx.curRoom.num))) {
                        ASSERT(invisibleActorCounter < INVISIBLE_ACTOR_MAX,
                               "invisible_actor_counter < INVISIBLE_ACTOR_MAX", "../z_actor.c", 6464);
                        invisibleActors[invisibleActorCounter] = actor;
                        invisibleActorCounter++;
                    } else {
                        if (!DEBUG_FEATURES || (HREG(64) != 1) || ((HREG(65) != -1) && (HREG(65) != HREG(66))) ||
                            (HREG(72) == 0)) {
                            Actor_draw(play, actor);
                            actor->isDrawn = true;
                        }
                    }
                }
            }

            actor = actor->next;
        }
    }

    if (!DEBUG_FEATURES || (HREG(64) != 1) || (HREG(73) != 0)) {
        EffectDisp(play->state.gfxCtx);
    }

    if (!DEBUG_FEATURES || (HREG(64) != 1) || (HREG(74) != 0)) {
        EffectSoftSprite_disp(play);
    }

    if (!DEBUG_FEATURES || (HREG(64) != 1) || (HREG(72) != 0)) {
        if (play->actorCtx.lensActive) {
            Actor_info_draw_actor_glass_draw(play, invisibleActorCounter, invisibleActors);
            if ((play->csCtx.state != CS_STATE_IDLE) || player_demo_check(play)) {
                magic_grass_cancel(play);
            }
        }
    }

    magic_window_pointer_draw(play);

    if (IREG(32) == 0) {
        Light_list_point_draw(play);
    }

    if (!DEBUG_FEATURES || (HREG(64) != 1) || (HREG(75) != 0)) {
        Actor_Name_Disp_Draw(play, &actorCtx->titleCtx);
    }

#if DEBUG_FEATURES
    if ((HREG(64) != 1) || (HREG(76) != 0)) {
        CollisionCheck_DrawCollision(play, &play->colChkCtx);
    }
#endif

    CLOSE_DISPS(play->state.gfxCtx, "../z_actor.c", 6563);
}

/**
 * Kill every actor which depends on an object that is not loaded.
 */
void Actor_info_bank_actor_check(PlayState* play, ActorContext* actorCtx) {
    Actor* actor;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(actorCtx->actorLists); i++) {
        actor = actorCtx->actorLists[i].head;
        while (actor != NULL) {
            if (!Object_Exchange_bank_dma_check(&play->objectCtx, actor->objectSlot)) {
                Actor_delete(actor);
            }
            actor = actor->next;
        }
    }
}

u8 stop_timer_set_part[] = { ACTORCAT_ENEMY, ACTORCAT_BOSS };

void Actor_info_stop_timer_set(PlayState* play, ActorContext* actorCtx, s32 duration) {
    Actor* actor;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(stop_timer_set_part); i++) {
        actor = actorCtx->actorLists[stop_timer_set_part[i]].head;
        while (actor != NULL) {
            actor->freezeTimer = duration;
            actor = actor->next;
        }
    }
}

/**
 * Kill actors on room change and update flags accordingly
 */
void Actor_info_room_actor_check(PlayState* play, ActorContext* actorCtx) {
    Actor* actor;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(actorCtx->actorLists); i++) {
        actor = actorCtx->actorLists[i].head;
        while (actor != NULL) {
            if ((actor->room >= 0) && (actor->room != play->roomCtx.curRoom.num) &&
                (actor->room != play->roomCtx.prevRoom.num)) {
                if (!actor->isDrawn) {
                    actor = Actor_info_delete(actorCtx, actor, play);
                } else {
                    Actor_delete(actor);
                    Actor_dt(actor, play);
                    actor = actor->next;
                }
            } else {
                actor = actor->next;
            }
        }
    }

    CollisionCheck_clear(play, &play->colChkCtx);
    actorCtx->flags.tempClear = 0;
    actorCtx->flags.tempSwch &= 0xFFFFFF;
    play->msgCtx.unk_E3F4 = 0;
}

// Actor_CleanupContext
void Actor_info_dt(ActorContext* actorCtx, PlayState* play) {
    Actor* actor;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(actorCtx->actorLists); i++) {
        actor = actorCtx->actorLists[i].head;
        while (actor != NULL) {
            Actor_info_delete(actorCtx, actor, play);
            actor = actorCtx->actorLists[i].head;
        }
    }

    ACTOR_DEBUG_PRINTF(T("絶対魔法領域解放\n", "Absolute magic field deallocation\n"));

    if (actorCtx->absoluteSpace != NULL) {
        ZELDA_ARENA_FREE(actorCtx->absoluteSpace, "../z_actor.c", 6731);
        actorCtx->absoluteSpace = NULL;
    }

    Game_play_room_inf_set(play);
    magic_window_pointer_dt(play);
    actor_dlftbls_cleanup();
}

/**
 * Adds a given actor instance at the front of the actor list of the specified category.
 * Also sets the actor instance as being of that category.
 */
void Actor_info_part_new(ActorContext* actorCtx, Actor* actorToAdd, u8 actorCategory) {
    Actor* prevHead;

    actorToAdd->category = actorCategory;

    actorCtx->total++;
    actorCtx->actorLists[actorCategory].length++;
    prevHead = actorCtx->actorLists[actorCategory].head;

    if (prevHead != NULL) {
        prevHead->prev = actorToAdd;
    }

    actorCtx->actorLists[actorCategory].head = actorToAdd;
    actorToAdd->next = prevHead;
}

/**
 * Removes a given actor instance from its actor list.
 * Also sets the temp clear flag of the current room if the actor removed was the last enemy loaded.
 */
Actor* Actor_info_part_delete(PlayState* play, ActorContext* actorCtx, Actor* actorToRemove) {
    Actor* newHead;

    actorCtx->total--;
    actorCtx->actorLists[actorToRemove->category].length--;

    if (actorToRemove->prev != NULL) {
        actorToRemove->prev->next = actorToRemove->next;
    } else {
        actorCtx->actorLists[actorToRemove->category].head = actorToRemove->next;
    }

    newHead = actorToRemove->next;

    if (newHead != NULL) {
        newHead->prev = actorToRemove->prev;
    }

    actorToRemove->next = NULL;
    actorToRemove->prev = NULL;

    if ((actorToRemove->room == play->roomCtx.curRoom.num) && (actorToRemove->category == ACTORCAT_ENEMY) &&
        (actorCtx->actorLists[ACTORCAT_ENEMY].length == 0)) {
        Actor_Environment_no_enemy_On(play, play->roomCtx.curRoom.num);
    }

    return newHead;
}

void actor_free_check(ActorOverlay* actorOverlay) {
    PRINTF_COLOR_CYAN();

    if (actorOverlay->numLoaded == 0) {
        ACTOR_DEBUG_PRINTF(T("アクタークライアントが０になりました\n", "Actor clients are now 0\n"));

        if (actorOverlay->loadedRamAddr != NULL) {
            if (actorOverlay->allocType & ACTOROVL_ALLOC_PERSISTENT) {
                ACTOR_DEBUG_PRINTF(T("オーバーレイ解放しません\n", "Overlay will not be deallocated\n"));
            } else if (actorOverlay->allocType & ACTOROVL_ALLOC_ABSOLUTE) {
                ACTOR_DEBUG_PRINTF(T("絶対魔法領域確保なので解放しません\n",
                                     "Absolute magic field reserved, so deallocation will not occur\n"));
                actorOverlay->loadedRamAddr = NULL;
            } else {
                ACTOR_DEBUG_PRINTF(T("オーバーレイ解放します\n", "Overlay deallocated\n"));
                ZELDA_ARENA_FREE(actorOverlay->loadedRamAddr, "../z_actor.c", 6834);
                actorOverlay->loadedRamAddr = NULL;
            }
        }
    } else {
        ACTOR_DEBUG_PRINTF(T("アクタークライアントはあと %d 残っています\n", "%d of actor client remaining\n"),
                           actorOverlay->numLoaded);
    }

    PRINTF_RST();
}

Actor* Actor_info_make_actor(ActorContext* actorCtx, PlayState* play, s16 actorId, f32 posX, f32 posY, f32 posZ, s16 rotX,
                   s16 rotY, s16 rotZ, s16 params) {
    s32 pad;
    Actor* actor;
    ActorProfile* profile;
    s32 objectSlot;
    ActorOverlay* overlayEntry;
    uintptr_t temp;
    char* name;
    u32 overlaySize;

    overlayEntry = &actor_dlftbls[actorId];
    ASSERT(actorId < ACTOR_ID_MAX, "profile < ACTOR_DLF_MAX", "../z_actor.c", 6883);

#if DEBUG_FEATURES
    name = overlayEntry->name != NULL ? overlayEntry->name : "";
#endif

    overlaySize = (uintptr_t)overlayEntry->vramEnd - (uintptr_t)overlayEntry->vramStart;

    ACTOR_DEBUG_PRINTF(T("アクタークラス追加 [%d:%s]\n", "Actor class addition [%d:%s]\n"), actorId, name);

    if (actorCtx->total > ACTOR_NUMBER_MAX) {
        PRINTF(ACTOR_COLOR_WARNING T("Ａｃｔｏｒセット数オーバー\n", "Actor set number exceeded\n") ACTOR_RST);
        return NULL;
    }

    if (overlayEntry->vramStart == NULL) {
        ACTOR_DEBUG_PRINTF(T("オーバーレイではありません\n", "Not an overlay\n"));

        profile = overlayEntry->profile;
    } else {
        if (overlayEntry->loadedRamAddr != NULL) {
            ACTOR_DEBUG_PRINTF(T("既にロードされています\n", "Already loaded\n"));
        } else {
            if (overlayEntry->allocType & ACTOROVL_ALLOC_ABSOLUTE) {
                ASSERT(overlaySize <= ACTOROVL_ABSOLUTE_SPACE_SIZE, "actor_segsize <= AM_FIELD_SIZE", "../z_actor.c",
                       6934);

                if (actorCtx->absoluteSpace == NULL) {
                    actorCtx->absoluteSpace = ZELDA_ARENA_MALLOC_R(
                        ACTOROVL_ABSOLUTE_SPACE_SIZE, T("AMF:絶対魔法領域", "AMF: absolute magic field"), 0);
                    ACTOR_DEBUG_PRINTF(
                        T("絶対魔法領域確保 %d バイト確保\n", "Absolute magic field allocation %d bytes allocated\n"),
                        ACTOROVL_ABSOLUTE_SPACE_SIZE);
                }

                overlayEntry->loadedRamAddr = actorCtx->absoluteSpace;
            } else if (overlayEntry->allocType & ACTOROVL_ALLOC_PERSISTENT) {
                overlayEntry->loadedRamAddr = ZELDA_ARENA_MALLOC_R(overlaySize, name, 0);
            } else {
                overlayEntry->loadedRamAddr = ZELDA_ARENA_MALLOC(overlaySize, name, 0);
            }

            if (overlayEntry->loadedRamAddr == NULL) {
                PRINTF(ACTOR_COLOR_ERROR T("Ａｃｔｏｒプログラムメモリが確保できません\n",
                                           "Cannot reserve actor program memory\n") ACTOR_RST);
                return NULL;
            }

            LoadFragmentFix2(overlayEntry->file.vromStart, overlayEntry->file.vromEnd, overlayEntry->vramStart,
                         overlayEntry->vramEnd, overlayEntry->loadedRamAddr);

            PRINTF_COLOR_GREEN();
            PRINTF("OVL(a):Seg:%08x-%08x Ram:%08x-%08x Off:%08x %s\n", overlayEntry->vramStart, overlayEntry->vramEnd,
                   overlayEntry->loadedRamAddr,
                   (uintptr_t)overlayEntry->loadedRamAddr + (uintptr_t)overlayEntry->vramEnd -
                       (uintptr_t)overlayEntry->vramStart,
                   (uintptr_t)overlayEntry->vramStart - (uintptr_t)overlayEntry->loadedRamAddr, name);
            PRINTF_RST();

            overlayEntry->numLoaded = 0;
        }

        profile = (void*)(uintptr_t)((overlayEntry->profile != NULL)
                                         ? (void*)((uintptr_t)overlayEntry->profile -
                                                   (intptr_t)((uintptr_t)overlayEntry->vramStart -
                                                              (uintptr_t)overlayEntry->loadedRamAddr))
                                         : NULL);
    }

    objectSlot = Object_Exchange_bank_check(&play->objectCtx, profile->objectId);

    if ((objectSlot < 0) ||
        ((profile->category == ACTORCAT_ENEMY) && Actor_Environment_room_clear_Check(play, play->roomCtx.curRoom.num))) {
        PRINTF(ACTOR_COLOR_ERROR T("データバンク無し！！<データバンク＝%d>(profilep->bank=%d)\n",
                                   "No data bank!! <data bank=%d> (profilep->bank=%d)\n") ACTOR_RST,
               objectSlot, profile->objectId);
        actor_free_check(overlayEntry);
        return NULL;
    }

    actor = ZELDA_ARENA_MALLOC(profile->instanceSize, name, 1);

    if (actor == NULL) {
        PRINTF(ACTOR_COLOR_ERROR T("Ａｃｔｏｒクラス確保できません！ %s <サイズ＝%dバイト>\n",
                                   "Actor class cannot be reserved! %s <size=%d bytes>\n"),
               ACTOR_RST, name, profile->instanceSize);
        actor_free_check(overlayEntry);
        return NULL;
    }

    ASSERT(overlayEntry->numLoaded < 255, "actor_dlftbl->clients < 255", "../z_actor.c", 7031);

    overlayEntry->numLoaded++;

    if (1) {}

    ACTOR_DEBUG_PRINTF(T("アクタークライアントは %d 個目です\n", "Actor client No. %d\n"), overlayEntry->numLoaded);

    mem_clear((u8*)actor, profile->instanceSize, 0);
    actor->overlayEntry = overlayEntry;
    actor->id = profile->id;
    actor->flags = profile->flags;

    if (profile->id == ACTOR_EN_PART) {
        actor->objectSlot = rotZ;
        rotZ = 0;
    } else {
        actor->objectSlot = objectSlot;
    }

    actor->init = profile->init;
    actor->destroy = profile->destroy;
    actor->update = profile->update;
    actor->draw = profile->draw;

    actor->room = play->roomCtx.curRoom.num;

    actor->home.pos.x = posX;
    actor->home.pos.y = posY;
    actor->home.pos.z = posZ;

    actor->home.rot.x = rotX;
    actor->home.rot.y = rotY;
    actor->home.rot.z = rotZ;

    actor->params = params;

    Actor_info_part_new(actorCtx, actor, profile->category);

    temp = SegmentBaseAddress[6];
    Actor_ct(actor, play);
    SegmentBaseAddress[6] = temp;

    return actor;
}

Actor* Actor_info_make_child_actor(ActorContext* actorCtx, Actor* parent, PlayState* play, s16 actorId, f32 posX, f32 posY,
                          f32 posZ, s16 rotX, s16 rotY, s16 rotZ, s16 params) {
    Actor* spawnedActor = Actor_info_make_actor(actorCtx, play, actorId, posX, posY, posZ, rotX, rotY, rotZ, params);

    if (spawnedActor == NULL) {
        return NULL;
    }

    parent->child = spawnedActor;
    spawnedActor->parent = parent;

    if (spawnedActor->room >= 0) {
        spawnedActor->room = parent->room;
    }

    return spawnedActor;
}

void Actor_info_make_door_actor(PlayState* play, ActorContext* actorCtx) {
    TransitionActorEntry* transitionActor;
    u8 numActors;
    s32 i;

    transitionActor = play->transitionActors.list;
    numActors = play->transitionActors.count;

    for (i = 0; i < numActors; i++) {
        if (transitionActor->id >= 0) {
            if (((transitionActor->sides[0].room >= 0) &&
                 ((transitionActor->sides[0].room == play->roomCtx.curRoom.num) ||
                  (transitionActor->sides[0].room == play->roomCtx.prevRoom.num))) ||
                ((transitionActor->sides[1].room >= 0) &&
                 ((transitionActor->sides[1].room == play->roomCtx.curRoom.num) ||
                  (transitionActor->sides[1].room == play->roomCtx.prevRoom.num)))) {
                Actor_info_make_actor(actorCtx, play, (s16)(transitionActor->id & 0x1FFF), transitionActor->pos.x,
                            transitionActor->pos.y, transitionActor->pos.z, 0, transitionActor->rotY, 0,
                            (i << TRANSITION_ACTOR_PARAMS_INDEX_SHIFT) + transitionActor->params);

                transitionActor->id = -transitionActor->id;
                numActors = play->transitionActors.count;
            }
        }
        transitionActor++;
    }
}

Actor* Actor_info_new(ActorContext* actorCtx, ActorEntry* actorEntry, PlayState* play) {
    return Actor_info_make_actor(actorCtx, play, actorEntry->id, actorEntry->pos.x, actorEntry->pos.y, actorEntry->pos.z,
                       actorEntry->rot.x, actorEntry->rot.y, actorEntry->rot.z, actorEntry->params);
}

Actor* Actor_info_delete(ActorContext* actorCtx, Actor* actor, PlayState* play) {
    PlayState* play2 = (PlayState*)play;
    Player* player;
    Actor* newHead;
    ActorOverlay* overlayEntry;
    UNUSED_NDEBUG char* name;

    player = GET_PLAYER(play);

    overlayEntry = actor->overlayEntry;

#if DEBUG_FEATURES
    name = overlayEntry->name != NULL ? overlayEntry->name : "";
#else
    name = "";
#endif

    ACTOR_DEBUG_PRINTF(T("アクタークラス削除 [%s]\n", "Actor class deleted [%s]\n"), name);

    if ((player != NULL) && (player->focusActor == actor)) {
        anchor_cancel(player);
        changeCameraMode(Gama_play_get_camera(play2, Gama_play_active_camera(play2)), CAM_MODE_NORMAL);
    }

    if (actorCtx->attention.naviHoverActor == actor) {
        actorCtx->attention.naviHoverActor = NULL;
    }

    if (actorCtx->attention.forcedLockOnActor == actor) {
        actorCtx->attention.forcedLockOnActor = NULL;
    }

    if (actorCtx->attention.bgmEnemy == actor) {
        actorCtx->attention.bgmEnemy = NULL;
    }

    Nai_StopAllObjFx(&actor->projectedPos);
    Actor_dt(actor, play2);

    newHead = Actor_info_part_delete(play2, actorCtx, actor);

    ZELDA_ARENA_FREE(actor, "../z_actor.c", 7242);

    if (overlayEntry->vramStart == NULL) {
        ACTOR_DEBUG_PRINTF(T("オーバーレイではありません\n", "Not an overlay\n"));
    } else {
        ASSERT(overlayEntry->loadedRamAddr != NULL, "actor_dlftbl->allocp != NULL", "../z_actor.c", 7251);
        ASSERT(overlayEntry->numLoaded > 0, "actor_dlftbl->clients > 0", "../z_actor.c", 7252);
        overlayEntry->numLoaded--;
        actor_free_check(overlayEntry);
    }

    return newHead;
}

/**
 * Checks that an actor is on-screen enough to be considered an attention actor.
 *
 * Note that the screen bounds checks are larger than the actual screen region
 * to give room for error.
 */
int anchor_search_display_check(PlayState* play, Actor* actor) {
    s16 x;
    s16 y;

    Actor_display_position_set(play, actor, &x, &y);

#define X_LEEWAY 20
#define Y_LEEWAY 160

    return (x > 0 - X_LEEWAY) && (x < SCREEN_WIDTH + X_LEEWAY) && (y > 0 - Y_LEEWAY) && (y < SCREEN_HEIGHT + Y_LEEWAY);
}

Actor* primary_actor;
Actor* wait_primary_actor;
f32 primary_ratio;
f32 enemyBGM_ratio;
s32 primary_wait;
s16 target_angle_y;

/**
 * Search for attention actors within the specified category.
 *
 * To be considered an attention actor the actor needs to:
 * - Have a non-NULL update function (still active)
 * - Not be player (this is technically a redundant check because the PLAYER category is never searched)
 * - Have `ACTOR_FLAG_ATTENTION_ENABLED` set
 * - Not be the current focus actor
 * - Be the closest attention actor found so far
 * - Be within range, specified by attentionRangeType
 * - Be roughly on-screen
 * - Not be blocked by a surface
 *
 * If an actor has a priority value set and the value is the lowest found so far, it will be set as the prioritized
 * attention actor. Otherwise, it is set as the nearest attention actor.
 *
 * This function is expected to be called with almost every actor category in each cycle. On a new cycle its global
 * variables must be reset by the caller, otherwise the information of the previous cycle will be retained.
 */
void anchor_search_check(PlayState* play, ActorContext* actorCtx, Player* player, u32 actorCategory) {
    f32 distSq;
    Actor* actor;
    Actor* playerFocusActor;
    CollisionPoly* poly;
    s32 bgId;
    Vec3f lineTestResultPos;

    actor = actorCtx->actorLists[actorCategory].head;
    playerFocusActor = player->focusActor;

    while (actor != NULL) {
        if ((actor->update != NULL) && ((Player*)actor != player) &&
            CHECK_FLAG_ALL(actor->flags, ACTOR_FLAG_ATTENTION_ENABLED)) {
            if ((actorCategory == ACTORCAT_ENEMY) &&
                CHECK_FLAG_ALL(actor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE) &&
                (actor->xyzDistToPlayerSq < SQ(500.0f)) && (actor->xyzDistToPlayerSq < enemyBGM_ratio)) {
                actorCtx->attention.bgmEnemy = actor;
                enemyBGM_ratio = actor->xyzDistToPlayerSq;
            }

            if (actor != playerFocusActor) {
                distSq = Actor_anchor_ratio(actor, player, target_angle_y);

                if ((distSq < primary_ratio) && Actor_anchor_ratio_check(actor, distSq) &&
                    anchor_search_display_check(play, actor) &&
                    (!T_BGCheck_CameraLineCheck_poly_chgrp_ai(&play->colCtx, &player->actor.focus.pos, &actor->focus.pos,
                                              &lineTestResultPos, &poly, true, true, true, true, &bgId) ||
                     T_BGCheck_CheckArrowNoHit(&play->colCtx, poly, bgId))) {
                    if (actor->attentionPriority != 0) {
                        // Lower values are considered higher priority
                        if (actor->attentionPriority < primary_wait) {
                            wait_primary_actor = actor;
                            primary_wait = actor->attentionPriority;
                        }
                    } else {
                        primary_actor = actor;
                        primary_ratio = distSq;
                    }
                }
            }
        }

        actor = actor->next;
    }
}

u8 anchor_part[] = {
    ACTORCAT_BOSS,  ACTORCAT_ENEMY,  ACTORCAT_BG,   ACTORCAT_EXPLOSIVE, ACTORCAT_NPC,  ACTORCAT_ITEMACTION,
    ACTORCAT_CHEST, ACTORCAT_SWITCH, ACTORCAT_PROP, ACTORCAT_MISC,      ACTORCAT_DOOR, ACTORCAT_SWITCH,
};

/**
 * Search for the nearest attention actor by iterating through most actor categories.
 * See `anchor_search_check` for more details on search criteria.
 *
 * The actor found is stored in the `attentionActorP` parameter, which is also returned.
 * It may be NULL if no actor that fulfills the criteria is found.
 */
Actor* Actor_info_anchor_search(PlayState* play, ActorContext* actorCtx, Actor** attentionActorP, Player* player) {
    s32 i;
    u8* category;

    primary_actor = wait_primary_actor = NULL;
    primary_ratio = enemyBGM_ratio = MAXFLOAT;
    primary_wait = INT32_MAX;

    if (!player_demo_check(play)) {
        category = &anchor_part[0];
        actorCtx->attention.bgmEnemy = NULL;
        target_angle_y = player->actor.shape.rot.y;

        // Search the first 3 actor categories first for an attention actor
        // These are Boss, Enemy, and Bg, in order.
        for (i = 0; i < 3; i++, category++) {
            anchor_search_check(play, actorCtx, player, *category);
        }

        // If no actor in the above categories was found, then try searching in the remaining categories
        if (primary_actor == NULL) {
            for (; i < ARRAY_COUNT(anchor_part); i++, category++) {
                anchor_search_check(play, actorCtx, player, *category);
            }
        }
    }

    if (primary_actor == NULL) {
        *attentionActorP = wait_primary_actor;
    } else {
        *attentionActorP = primary_actor;
    }

    return *attentionActorP;
}

/**
 * Finds the first actor instance of a specified ID and category if there is one.
 */
Actor* Actor_info_name_search(ActorContext* actorCtx, s32 actorId, s32 actorCategory) {
    Actor* actor = actorCtx->actorLists[actorCategory].head;

    while (actor != NULL) {
        if (actor->id == actorId) {
            return actor;
        }
        actor = actor->next;
    }

    return NULL;
}

/**
 * Play the death sound effect and flash the screen color for 4 frames.
 * While the screen flashes, the game freezes.
 */
void Actor_info_finish(PlayState* play, Actor* actor) {
    play->actorCtx.freezeFlashTimer = 5;
    Effect_SE_Info_new(play, &actor->world.pos, 20, NA_SE_EN_LAST_DAMAGE);
}

/**
 * Updates `FaceChange` data for a blinking pattern.
 * This system expects that the actor using the system has defined 3 faces in this exact order:
 * "eyes open", "eyes half open", "eyes closed".
 *
 * @param faceChange  pointer to an actor's faceChange data
 * @param blinkIntervalBase  The base number of frames between blinks
 * @param blinkIntervalRandRange  The range for a random number of frames that can be added to `blinkIntervalBase`
 * @param blinkDuration  The number of frames it takes for a single blink to occur
 */
s16 Eye_Anime_pattern_set(FaceChange* faceChange, s16 blinkIntervalBase, s16 blinkIntervalRandRange,
                              s16 blinkDuration) {
    if (DECR(faceChange->timer) == 0) {
        faceChange->timer = get_random_timer(blinkIntervalBase, blinkIntervalRandRange);
    }

    if ((faceChange->timer - blinkDuration) > 0) {
        // `timer - duration` is positive so this is the default state: "eyes open" face
        faceChange->face = 0;
    } else if (((faceChange->timer - blinkDuration) > -2) || (faceChange->timer < 2)) {
        // This condition aims to catch both cases where the "eyes half open" face is needed.
        // Note that the comparison assumes the duration of the "eyes half open" phase is 2 frames, irrespective of the
        // value of `blinkDuration`. The duration for the "eyes closed" phase is `blinkDuration - 4`.
        // For Player's use case `blinkDuration` is 6, so the "eyes closed" phase happens to have
        // the same duration as each "eyes half open" phase.
        faceChange->face = 1;
    } else {
        // If both conditions above fail, the only possibility left is the "eyes closed" face
        faceChange->face = 2;
    }

    return faceChange->face;
}

/**
 * Updates `FaceChange` data for randomly selected face sets.
 * Each set contains 3 faces. After the timer runs out, the next face in the set is used.
 * After the third face in a set is used, a new face set is randomly chosen.
 *
 * @param faceChange  pointer to an actor's faceChange data
 * @param changeTimerBase  The base number of frames between each face change
 * @param changeTimerRandRange  The range for a random number of frames that can be added to `changeTimerBase`
 * @param faceSetRange  The max number of face sets that will be chosen from
 */
s16 Mouth_Anime_pattern_set(FaceChange* faceChange, s16 changeTimerBase, s16 changeTimerRandRange,
                               s16 faceSetRange) {
    if (DECR(faceChange->timer) == 0) {
        faceChange->timer = get_random_timer(changeTimerBase, changeTimerRandRange);
        faceChange->face++;

        if ((faceChange->face % 3) == 0) {
            // Randomly chose a "set number", then multiply by 3 because each set has 3 faces.
            // This will use the first face in the newly chosen set.
            faceChange->face = (s32)(fqrand() * faceSetRange) * 3;
        }
    }

    return faceChange->face;
}

void Part_Break_init(BodyBreak* bodyBreak, s32 count, PlayState* play) {
    if ((bodyBreak->matrices = ZELDA_ARENA_MALLOC((count + 1) * sizeof(*bodyBreak->matrices), "../z_actor.c", 7540)) !=
            NULL &&
        (bodyBreak->dLists = ZELDA_ARENA_MALLOC((count + 1) * sizeof(*bodyBreak->dLists), "../z_actor.c", 7543)) !=
            NULL &&
        (bodyBreak->objectSlots =
             ZELDA_ARENA_MALLOC((count + 1) * sizeof(*bodyBreak->objectSlots), "../z_actor.c", 7546)) != NULL) {

        mem_clear((u8*)bodyBreak->matrices, (count + 1) * sizeof(*bodyBreak->matrices), 0);
        mem_clear((u8*)bodyBreak->dLists, (count + 1) * sizeof(*bodyBreak->dLists), 0);
        mem_clear((u8*)bodyBreak->objectSlots, (count + 1) * sizeof(*bodyBreak->objectSlots), 0);
        bodyBreak->val = 1;
    } else {
        if (bodyBreak->matrices != NULL) {
            ZELDA_ARENA_FREE(bodyBreak->matrices, "../z_actor.c", 7558);
        }

        if (bodyBreak->dLists != NULL) {
            ZELDA_ARENA_FREE(bodyBreak->dLists, "../z_actor.c", 7561);
        }

        if (bodyBreak->objectSlots != NULL) {
            ZELDA_ARENA_FREE(bodyBreak->objectSlots, "../z_actor.c", 7564);
        }
    }
}

void Part_Break_Get(BodyBreak* bodyBreak, s32 limbIndex, s32 minLimbIndex, s32 maxLimbIndex, u32 count, Gfx** dList,
                       s16 objectSlot) {
    PlayState* play = Effect_GetGamePointer();

    if ((play->actorCtx.freezeFlashTimer == 0) && (bodyBreak->val > 0)) {
        if ((limbIndex >= minLimbIndex) && (limbIndex <= maxLimbIndex) && (*dList != NULL)) {
            bodyBreak->dLists[bodyBreak->val] = *dList;
            Matrix_get(&bodyBreak->matrices[bodyBreak->val]);
            bodyBreak->objectSlots[bodyBreak->val] = objectSlot;
            bodyBreak->val++;
        }

        if (bodyBreak->prevLimbIndex != limbIndex) {
            bodyBreak->count++;
        }

        if ((u32)bodyBreak->count >= count) {
            bodyBreak->count = bodyBreak->val - 1;
            bodyBreak->val = BODYBREAK_STATUS_READY;
        }
    }

    bodyBreak->prevLimbIndex = limbIndex;
}

s32 Part_break(Actor* actor, BodyBreak* bodyBreak, PlayState* play, s16 type) {
    EnPart* spawnedEnPart;
    s16 objectSlot;

    if (bodyBreak->val != BODYBREAK_STATUS_READY) {
        return false;
    }

    while (bodyBreak->count > 0) {
        Matrix_put(&bodyBreak->matrices[bodyBreak->count]);
        Matrix_scale(1.0f / actor->scale.x, 1.0f / actor->scale.y, 1.0f / actor->scale.z, MTXMODE_APPLY);
        Matrix_get(&bodyBreak->matrices[bodyBreak->count]);

        if (1) {
            if (bodyBreak->objectSlots[bodyBreak->count] > BODYBREAK_OBJECT_SLOT_DEFAULT) {
                objectSlot = bodyBreak->objectSlots[bodyBreak->count];
            } else {
                objectSlot = actor->objectSlot;
            }
        }

        spawnedEnPart = (EnPart*)Actor_info_make_child_actor(
            &play->actorCtx, actor, play, ACTOR_EN_PART, bodyBreak->matrices[bodyBreak->count].xw,
            bodyBreak->matrices[bodyBreak->count].yw, bodyBreak->matrices[bodyBreak->count].zw, 0, 0, objectSlot, type);

        if (spawnedEnPart != NULL) {
#if OOT_VERSION < PAL_1_0
            //! @bug Wrong rotation order compared to Actor_draw
            Matrix_to_rotate2_new(&bodyBreak->matrices[bodyBreak->count], &spawnedEnPart->actor.shape.rot, 0);
#else
            Matrix_to_rotate_new(&bodyBreak->matrices[bodyBreak->count], &spawnedEnPart->actor.shape.rot, 0);
#endif
            spawnedEnPart->displayList = bodyBreak->dLists[bodyBreak->count];
            spawnedEnPart->actor.scale = actor->scale;
        }

        bodyBreak->count--;
    }

    bodyBreak->val = BODYBREAK_STATUS_FINISHED;

    ZELDA_ARENA_FREE(bodyBreak->matrices, "../z_actor.c", 7678);
    ZELDA_ARENA_FREE(bodyBreak->dLists, "../z_actor.c", 7679);
    ZELDA_ARENA_FREE(bodyBreak->objectSlots, "../z_actor.c", 7680);

    return true;
}

void _dust_ground_set(PlayState* play, Actor* actor, Vec3f* posXZ, f32 radius, s32 amountMinusOne,
                              f32 randAccelWeight, s16 scale, s16 scaleStep, u8 useLighting) {
    Vec3f pos;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    f32 angle;
    s32 i;

    angle = (fqrand() - 0.5f) * (2.0f * 3.14f);
    pos.y = actor->floorHeight;
    accel.y += (fqrand() - 0.5f) * 0.2f;

    for (i = amountMinusOne; i >= 0; i--) {
        pos.x = posXZ->x + sinf_table(angle) * radius;
        pos.z = posXZ->z + cosf_table(angle) * radius;
        accel.x = (fqrand() - 0.5f) * randAccelWeight;
        accel.z = (fqrand() - 0.5f) * randAccelWeight;

        if (scale == 0) {
            Effect_SS_Dust_ct_direct(play, &pos, &velocity, &accel);
        } else {
            if (useLighting) {
                Effect_SS_Dust_sc_li_ct(play, &pos, &velocity, &accel, scale, scaleStep);
            } else {
                Effect_SS_Dust_sc_ct(play, &pos, &velocity, &accel, scale, scaleStep);
            }
        }

        angle += (2.0f * 3.14f) / (amountMinusOne + 1.0f);
    }
}

void dust_fly_set2(PlayState* play, Vec3f* posBase, f32 randRangeDiameter, s32 amountMinusOne, s16 scaleBase,
                   s16 scaleStep, u8 arg6) {
    Vec3f pos;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    s16 scale;
    u32 var2;
    s32 i;

    for (i = amountMinusOne; i >= 0; i--) {
        pos.x = posBase->x + ((fqrand() - 0.5f) * randRangeDiameter);
        pos.y = posBase->y + ((fqrand() - 0.5f) * randRangeDiameter);
        pos.z = posBase->z + ((fqrand() - 0.5f) * randRangeDiameter);

        scale = (s16)((scaleBase * fqrand()) * 0.2f) + scaleBase;
        var2 = arg6;

        if (var2 != 0) {
            Effect_SS_Dust_sc_li_ct(play, &pos, &velocity, &accel, scale, scaleStep);
        } else {
            Effect_SS_Dust_sc_ct(play, &pos, &velocity, &accel, scale, scaleStep);
        }
    }
}

Actor* BlastVsMyCheck(PlayState* play, Collider* collider) {
    if ((collider->acFlags & AC_HIT) && (collider->ac->category == ACTORCAT_EXPLOSIVE)) {
        collider->acFlags &= ~AC_HIT;
        return collider->ac;
    }

    return NULL;
}

Actor* BlastVsMyCheck_c(PlayState* play, Actor* explosiveActor) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;

    while (actor != NULL) {
        if ((actor == explosiveActor) || (actor->params != 1)) {
            actor = actor->next;
        } else {
            if (Actor_search_actor_distance(explosiveActor, actor) <= (actor->shape.rot.z * 10) + 80.0f) {
                return actor;
            } else {
                actor = actor->next;
            }
        }
    }

    return NULL;
}

/**
 * Dynamically changes the category of a given actor instance.
 * This is done by moving it to the corresponding category list and setting its category variable accordingly.
 */
void Actor_info_part_chg(PlayState* play, ActorContext* actorCtx, Actor* actor, u8 actorCategory) {
    //! @bug Calling this function immediately moves an actor from one category list to the other.
    //! So, if Actor_info_part_chg is called during an actor update, the inner loop in
    //! Actor_info_call_actor will continue from the next actor in the new category, rather than the next
    //! actor in the old category. This will cause any actors after this one in the old category to
    //! be skipped over and not updated, and any actors in the new category to be updated more than
    //! once.
    Actor_info_part_delete(play, actorCtx, actor);
    Actor_info_part_new(actorCtx, actor, actorCategory);
}

/**
 * Checks if a hookshot or arrow actor is going to collide with the cylinder denoted by the
 * actor's `cylRadius` and `cylHeight`.
 * The check is only peformed if the projectile actor is within the provided sphere radius.
 *
 * Returns the actor if there will be collision, NULL otherwise.
 */
Actor* ShotVsMyCheck(PlayState* play, Actor* refActor, f32 radius) {
    Actor* actor;
    Vec3f spA8;
    Vec3f delta;
    Vec3f sp90;
    Vec3f sp84;

    actor = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;
    while (actor != NULL) {
        if (((actor->id != ACTOR_ARMS_HOOK) && (actor->id != ACTOR_EN_ARROW)) || (actor == refActor)) {
            actor = actor->next;
        } else {
            //! @bug The projectile actor gets unsafely casted to a hookshot to check its timer, even though
            //  it can also be an arrow.
            //  Luckily, the field at the same offset in the arrow actor is the x component of a vector
            //  which will rarely ever be 0. So it's very unlikely for this bug to cause an issue.
            if ((search_position_distance(&refActor->world.pos, &actor->world.pos) > radius) ||
                (((ArmsHook*)actor)->timer == 0)) {
                actor = actor->next;
            } else {
                delta.x = (actor->speed * 10.0f) * sin_s(actor->world.rot.y);
                delta.y = actor->velocity.y + (actor->gravity * 10.0f);
                delta.z = (actor->speed * 10.0f) * cos_s(actor->world.rot.y);

                spA8.x = actor->world.pos.x + delta.x;
                spA8.y = actor->world.pos.y + delta.y;
                spA8.z = actor->world.pos.z + delta.z;

                if (CollisionCheckPipeVsLine2(refActor->colChkInfo.cylRadius, refActor->colChkInfo.cylHeight,
                                                    0.0f, &refActor->world.pos, &actor->world.pos, &spA8, &sp90,
                                                    &sp84)) {
                    return actor;
                } else {
                    actor = actor->next;
                }
            }
        }
    }

    return NULL;
}

/**
 * Sets the actor's text id with a dynamic prefix based on the current scene.
 */
void mes_set(PlayState* play, Actor* actor, s16 baseTextId) {
    s16 prefix;

    switch (play->sceneId) {
        case SCENE_DEKU_TREE:
        case SCENE_DEKU_TREE_BOSS:
        case SCENE_FOREST_TEMPLE_BOSS:
        case SCENE_KNOW_IT_ALL_BROS_HOUSE:
        case SCENE_TWINS_HOUSE:
        case SCENE_MIDOS_HOUSE:
        case SCENE_SARIAS_HOUSE:
        case SCENE_KOKIRI_SHOP:
        case SCENE_LINKS_HOUSE:
        case SCENE_KOKIRI_FOREST:
        case SCENE_SACRED_FOREST_MEADOW:
        case SCENE_LOST_WOODS:
        case 112:
            prefix = 0x1000;
            break;
        case SCENE_STABLE:
        case SCENE_HYRULE_FIELD:
        case SCENE_LON_LON_RANCH:
            prefix = 0x2000;
            break;
        case SCENE_FIRE_TEMPLE:
        case SCENE_DODONGOS_CAVERN_BOSS:
        case SCENE_FIRE_TEMPLE_BOSS:
        case SCENE_DEATH_MOUNTAIN_TRAIL:
        case SCENE_DEATH_MOUNTAIN_CRATER:
        case SCENE_GORON_CITY:
            prefix = 0x3000;
            break;
        case SCENE_JABU_JABU:
        case SCENE_JABU_JABU_BOSS:
        case SCENE_ZORAS_RIVER:
        case SCENE_ZORAS_DOMAIN:
        case SCENE_ZORAS_FOUNTAIN:
            prefix = 0x4000;
            break;
        case SCENE_SHADOW_TEMPLE:
        case SCENE_SHADOW_TEMPLE_BOSS:
        case SCENE_KAKARIKO_CENTER_GUEST_HOUSE:
        case SCENE_BACK_ALLEY_HOUSE:
        case SCENE_DOG_LADY_HOUSE:
        case SCENE_GRAVEKEEPERS_HUT:
        case SCENE_REDEAD_GRAVE:
        case SCENE_WINDMILL_AND_DAMPES_GRAVE:
        case SCENE_KAKARIKO_VILLAGE:
        case SCENE_GRAVEYARD:
            prefix = 0x5000;
            break;
        case SCENE_SPIRIT_TEMPLE:
        case SCENE_SPIRIT_TEMPLE_BOSS:
        case SCENE_IMPAS_HOUSE:
        case SCENE_CARPENTERS_TENT:
        case SCENE_LAKE_HYLIA:
        case SCENE_GERUDO_VALLEY:
        case SCENE_DESERT_COLOSSUS:
            prefix = 0x6000;
            break;
        case SCENE_MARKET_ENTRANCE_DAY:
        case SCENE_BACK_ALLEY_DAY:
        case SCENE_BACK_ALLEY_NIGHT:
        case SCENE_MARKET_DAY:
        case SCENE_MARKET_NIGHT:
        case SCENE_MARKET_RUINS:
        case SCENE_HYRULE_CASTLE:
            prefix = 0x7000;
            break;
        default:
            prefix = 0x0000;
            break;
    }

    actor->textId = prefix | baseTextId;
}

/**
 * Checks if a given actor will be standing on the ground after being translated
 * by the provided distance and angle.
 *
 * Returns true if the actor will be standing on ground.
 */
s16 BG_point_check(Actor* actor, PlayState* play, f32 distance, s16 angle) {
    s16 ret;
    s16 prevBgCheckFlags;
    f32 dx;
    f32 dz;
    Vec3f prevActorPos;

    xyz_t_move(&prevActorPos, &actor->world.pos);
    prevBgCheckFlags = actor->bgCheckFlags;

    dx = distance * sin_s(angle);
    dz = distance * cos_s(angle);
    actor->world.pos.x += dx;
    actor->world.pos.z += dz;

    Actor_BGcheck2(play, actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);

    xyz_t_move(&actor->world.pos, &prevActorPos);

    ret = actor->bgCheckFlags & BGCHECKFLAG_GROUND;
    actor->bgCheckFlags = prevBgCheckFlags;

    return ret;
}

/**
 * Returns true if the player is locked onto the specified actor
 */
s32 Anc_Fight_My_Check(PlayState* play, Actor* actor) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags1 & PLAYER_STATE1_HOSTILE_LOCK_ON) && actor->isLockedOn) {
        return true;
    } else {
        return false;
    }
}

/**
 * Returns true if the player is locked onto an actor other than the specified actor
 */
s32 Anc_Fight_ham_Check(PlayState* play, Actor* actor) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags1 & PLAYER_STATE1_HOSTILE_LOCK_ON) && !actor->isLockedOn) {
        return true;
    } else {
        return false;
    }
}

f32 chase_pos(Vec3f* arg0, Vec3f* arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5) {
    f32 ret = 0.0f;

    if (arg4 <= search_position_distance(arg0, arg1)) {
        ret = add_calc(&arg1->x, arg0->x, arg2, arg3, 0.0f);
        ret += add_calc(&arg1->y, arg0->y, arg2, arg3, 0.0f);
        ret += add_calc(&arg1->z, arg0->z, arg2, arg3, 0.0f);
    } else if (arg5 < search_position_distance(arg0, arg1)) {
        ret = add_calc(&arg1->x, arg0->x, arg2, arg3, 0.0f);
        ret += add_calc(&arg1->y, arg0->y, arg2, arg3, 0.0f);
        ret += add_calc(&arg1->z, arg0->z, arg2, arg3, 0.0f);
    }

    return ret;
}

void Shadow_draw(Vec3f* arg0, Vec3f* arg1, u8 alpha, PlayState* play) {
    MtxF sp60;
    f32 yIntersect;
    Vec3f checkPos;
    CollisionPoly* groundPoly;

    OPEN_DISPS(play->state.gfxCtx, "../z_actor.c", 8120);

    POLY_OPA_DISP = rcp_mode_set(POLY_OPA_DISP, SETUPDL_44);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, alpha);

    checkPos.x = arg0->x;
    checkPos.y = arg0->y + 1.0f;
    checkPos.z = arg0->z;

    yIntersect = T_BGCheck_ObjGroundCheck2(play, &play->colCtx, &groundPoly, &checkPos);

    if (groundPoly != NULL) {
        T_Polygon_Ground_Matrix(groundPoly, arg0->x, yIntersect, arg0->z, &sp60);
        Matrix_put(&sp60);
    } else {
        Matrix_translate(arg0->x, arg0->y, arg0->z, MTXMODE_NEW);
    }

    Matrix_scale(arg1->x, 1.0f, arg1->z, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_actor.c", 8149);
    gSPDisplayList(POLY_OPA_DISP++, gCircleShadowDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_actor.c", 8155);
}

void set_jisin(PlayState* play, s16 y, s16 duration) {
    s16 quakeIndex = startQuake(&play->mainCamera, QUAKE_TYPE_3);

    setSpeedQuake(quakeIndex, 20000);
    setScaleQuake(quakeIndex, y, 0, 0, 0);
    setTimerQuake(quakeIndex, duration);
}

void set_jisin_2(PlayState* play, s16 y, s16 duration, s16 speed) {
    s16 quakeIndex = startQuake(&play->mainCamera, QUAKE_TYPE_3);

    setSpeedQuake(quakeIndex, speed);
    setScaleQuake(quakeIndex, y, 0, 0, 0);
    setTimerQuake(quakeIndex, duration);
}

void ActorQuakeRumbleSet(Actor* actor, PlayState* play, s16 quakeY, s16 quakeDuration) {
    if (quakeY >= 5) {
        z_vibctl2_vib_setQ(actor->xyzDistToPlayerSq, 255, 20, 150);
    } else {
        z_vibctl2_vib_setQ(actor->xyzDistToPlayerSq, 180, 20, 100);
    }
    set_jisin(play, quakeY, quakeDuration);
}

f32 rnd_f(f32 f) {
    return fqrand() * f;
}

f32 rnd_fx(f32 f) {
    return (fqrand() - 0.5f) * f;
}

typedef struct DoorLockInfo {
    /* 0x00 */ f32 chainAngle;
    /* 0x04 */ f32 chainLength;
    /* 0x08 */ f32 yShift;
    /* 0x0C */ f32 chainsScale;
    /* 0x10 */ f32 chainsRotZInit;
    /* 0x14 */ Gfx* chainDL;
    /* 0x18 */ Gfx* lockDL;
} DoorLockInfo; // size = 0x1C

static DoorLockInfo door_key_status_data[] = {
    /* DOORLOCK_NORMAL */ { 0.54f, 6000.0f, 5000.0f, 1.0f, 0.0f, gDoorChainDL, gDoorLockDL },
    /* DOORLOCK_BOSS */ { 0.644f, 12000.0f, 8000.0f, 1.0f, 0.0f, gBossDoorChainDL, gBossDoorLockDL },
    /* DOORLOCK_NORMAL_SPIRIT */ { 0.64000005f, 8500.0f, 8000.0f, 1.75f, 0.1f, gDoorChainDL, gDoorLockDL },
};

/**
 * Draws chains and lock of a locked door, of the specified `type` (see `DoorLockType`).
 * `frame` can be 0 to 10, where 0 is "open" and 10 is "closed", the chains slide accordingly.
 */
void key_draw(PlayState* play, s32 frame, s32 type) {
    DoorLockInfo* entry;
    s32 i;
    MtxF baseMtxF;
    f32 chainRotZ;
    f32 chainsTranslateX;
    f32 chainsTranslateY;
    f32 scale;

    entry = &door_key_status_data[type];
    chainRotZ = entry->chainsRotZInit;

    OPEN_DISPS(play->state.gfxCtx, "../z_actor.c", 8265);

    Matrix_translate(0.0f, entry->yShift, 500.0f, MTXMODE_APPLY);
    Matrix_get(&baseMtxF);

    {
        f32 rotZStep;

        chainsTranslateX = -(10 - frame) * sinf(entry->chainAngle - chainRotZ) * 0.1f * entry->chainLength;
        chainsTranslateY = (10 - frame) * cosf(entry->chainAngle - chainRotZ) * 0.1f * entry->chainLength;

        for (i = 0; i < 4; i++) {

            Matrix_put(&baseMtxF);
            Matrix_rotateZ(chainRotZ, MTXMODE_APPLY);
            Matrix_translate(chainsTranslateX, chainsTranslateY, 0.0f, MTXMODE_APPLY);

            if (entry->chainsScale != 1.0f) {
                Matrix_scale(entry->chainsScale, entry->chainsScale, entry->chainsScale, MTXMODE_APPLY);
            }

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_actor.c", 8299);
            gSPDisplayList(POLY_OPA_DISP++, entry->chainDL);

            if (i % 2) {
                rotZStep = 2.0f * entry->chainAngle;
            } else {
                rotZStep = M_PI - (2.0f * entry->chainAngle);
            }

            chainRotZ += rotZStep;
        }
    }

    scale = frame * 0.1f;
    Matrix_put(&baseMtxF);
    Matrix_scale(scale, scale, scale, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_actor.c", 8314);
    gSPDisplayList(POLY_OPA_DISP++, entry->lockDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_actor.c", 8319);
}

void SetSparkFlash(PlayState* play, Vec3f* arg1) {
    CollisionCheckSetSparkFlashBlue(play, arg1);
}

void Set_Fog(Actor* actor, s16 colorFlag, s16 colorIntensityMax, s16 bufFlag, s16 duration) {
    //! @bug This first comparison is always false as COLORFILTER_COLORFLAG_GRAY is out of range of an s16.
    if ((colorFlag == COLORFILTER_COLORFLAG_GRAY) && !(colorIntensityMax & COLORFILTER_INTENSITY_FLAG)) {
        Actor_SE_set(actor, NA_SE_EN_LIGHT_ARROW_HIT);
    }

    actor->colorFilterParams = colorFlag | bufFlag | ((colorIntensityMax & 0xF8) << 5) | duration;
    actor->colorFilterTimer = duration;
}

void Setpos_HiliteReflect_init(Vec3f* object, PlayState* play) {
    Vec3f lightDir;

    lightDir.x = play->envCtx.dirLight1.params.dir.x;
    lightDir.y = play->envCtx.dirLight1.params.dir.y;
    lightDir.z = play->envCtx.dirLight1.params.dir.z;

    HiliteReflect_init(object, &play->view.eye, &lightDir, play->state.gfxCtx);
}

void Setpos_HiliteReflect_xlu_init(Vec3f* object, PlayState* play) {
    Vec3f lightDir;

    lightDir.x = play->envCtx.dirLight1.params.dir.x;
    lightDir.y = play->envCtx.dirLight1.params.dir.y;
    lightDir.z = play->envCtx.dirLight1.params.dir.z;

    HiliteReflect_xlu_init(object, &play->view.eye, &lightDir, play->state.gfxCtx);
}

/**
 * Updates NPC talking state. Checks for a talk request and updates
 * the talkState parameter when a dialog is ongoing. Otherwise checks if
 * the actor is onscreen, advertises the interaction in a range and sets
 * the current text id if necessary.
 *
 * The talk state values are defined in the NpcTalkState enum.
 *
 * @see NpcTalkState
 *
 * @param[in,out] talkState Talk state
 * @param interactRange The interact (talking) range for the actor
 * @param getTextId Callback for getting the next text id
 * @param updateTalkState Callback for getting the next talkState value
 * @return True if a new dialog was started (player talked to the actor). False otherwise.
 */
s32 npc_talk(PlayState* play, Actor* actor, s16* talkState, f32 interactRange, NpcGetTextIdFunc getTextId,
                      NpcUpdateTalkStateFunc updateTalkState) {
    s16 x;
    s16 y;

    if (Actor_talk_check(actor, play)) {
        *talkState = NPC_TALK_STATE_TALKING;
        return true;
    }

    if (*talkState != NPC_TALK_STATE_IDLE) {
        *talkState = updateTalkState(play, actor);
        return false;
    }

    Actor_display_position_set(play, actor, &x, &y);
    if ((x < 0) || (x > SCREEN_WIDTH) || (y < 0) || (y > SCREEN_HEIGHT)) {
        // Actor is offscreen
        return false;
    }

    if (!Actor_talk_request2(actor, play, interactRange)) {
        return false;
    }

    actor->textId = getTextId(play, actor);

    return false;
}

typedef struct NpcTrackingRotLimits {
    /* 0x00 */ s16 maxHeadYaw;
    /* 0x02 */ s16 minHeadPitch;
    /* 0x04 */ s16 maxHeadPitch;
    /* 0x06 */ s16 maxTorsoYaw;
    /* 0x08 */ s16 minTorsoPitch;
    /* 0x0A */ s16 maxTorsoPitch;
    /* 0x0C */ u8 rotateYaw;
} NpcTrackingRotLimits; // size = 0x10

typedef struct NpcTrackingParams {
    /* 0x00 */ NpcTrackingRotLimits rotLimits;
    // Fields specific to NPC_TRACKING_PLAYER_AUTO_TURN mode
    /* 0x10 */ f32 autoTurnDistanceRange;   // Max distance to player to enable tracking and auto-turn
    /* 0x14 */ s16 maxYawForPlayerTracking; // Player is tracked if within this yaw
} NpcTrackingParams;                        // size = 0x18

/**
 * Npc tracking angle limit presets to use with eye_moveM.
 *
 * @see eye_moveM
 */
static NpcTrackingParams eye_move_info[] = {
    { { 0x2AA8, -0x0E38, 0x18E2, 0x1554, 0x0000, 0x0000, true }, 170.0f, 0x3FFC },
    { { 0x2AA8, -0x1554, 0x1554, 0x1554, -0x071C, 0x0E38, true }, 170.0f, 0x3FFC },
    { { 0x31C4, -0x1C70, 0x0E38, 0x0E38, -0x0E38, 0x071C, true }, 170.0f, 0x3FFC },
    { { 0x1554, -0x0E38, 0x0000, 0x071C, -0x071C, 0x0000, true }, 170.0f, 0x3FFC },
    { { 0x2AA8, -0x071C, 0x071C, 0x0E38, -0x2AA8, 0x2AA8, true }, 170.0f, 0x3FFC },
    { { 0x0000, -0x1C70, 0x2AA8, 0x3FFC, -0x0E38, 0x0E38, true }, 170.0f, 0x3FFC },
    { { 0x2AA8, -0x0E38, 0x0E38, 0x0E38, 0x0000, 0x0000, true }, 0.0f, 0x0000 },
    { { 0x2AA8, -0x0E38, 0x0000, 0x0E38, 0x0000, 0x1C70, true }, 0.0f, 0x0000 },
    { { 0x2AA8, -0x0E38, -0x0E38, 0x0000, 0x0000, 0x0000, true }, 0.0f, 0x0000 },
    { { 0x071C, -0x0E38, 0x0E38, 0x1C70, 0x0000, 0x0000, true }, 0.0f, 0x0000 },
    { { 0x0E38, -0x0E38, 0x0000, 0x1C70, 0x0000, 0x0E38, true }, 0.0f, 0x0000 },
    { { 0x2AA8, -0x1C70, 0x1C70, 0x0E38, -0x0E38, 0x0E38, true }, 0.0f, 0x0000 },
    { { 0x18E2, -0x0E38, 0x0E38, 0x0E38, 0x0000, 0x0000, true }, 0.0f, 0x0000 },
};

/**
 * Smoothly turns the actor's whole body and updates torso and head rotations in
 * NpcInteractInfo so that the actor tracks the point specified in NpcInteractInfo.trackPos.
 * Rotations are limited to specified angles.
 *
 * Head and torso rotation angles are determined by calculating the pitch and yaw
 * from the actor position to the given target position.
 *
 * The y position of the actor is offset by NpcInteractInfo.yOffset
 * before calculating the angles. It can be used to configure the height difference
 * between the actor and the target.
 *
 * @param maxHeadYaw maximum head yaw difference from neutral position
 * @param maxHeadPitch maximum head pitch angle
 * @param minHeadPitch minimum head pitch angle
 * @param maxTorsoYaw maximum torso yaw difference from neutral position
 * @param maxTorsoPitch maximum torso pitch angle
 * @param minTorsoPitch minimum torso pitch angle
 * @param rotateYaw if true, the actor's yaw (shape.rot.y) is updated to turn the actor's whole body
 */
void eye_moveS(Actor* actor, NpcInteractInfo* interactInfo, s16 maxHeadYaw, s16 maxHeadPitch,
                              s16 minHeadPitch, s16 maxTorsoYaw, s16 maxTorsoPitch, s16 minTorsoPitch, u8 rotateYaw) {
    s16 pitchTowardsTarget;
    s16 yawTowardsTarget;
    s16 pitch;
    s16 bodyYawDiff;
    s16 temp;
    Vec3f offsetActorPos;

    offsetActorPos.x = actor->world.pos.x;
    offsetActorPos.y = actor->world.pos.y + interactInfo->yOffset;
    offsetActorPos.z = actor->world.pos.z;

    pitchTowardsTarget = search_position_angleX(&offsetActorPos, &interactInfo->trackPos);
    yawTowardsTarget = search_position_angleY(&offsetActorPos, &interactInfo->trackPos);
    bodyYawDiff = search_position_angleY(&actor->world.pos, &interactInfo->trackPos);
    bodyYawDiff -= actor->shape.rot.y;

    temp = CLAMP(bodyYawDiff, -maxHeadYaw, maxHeadYaw);
    add_calc_short_angle2(&interactInfo->headRot.y, temp, 6, 2000, 1);

    temp = (ABS(bodyYawDiff) >= 0x8000) ? 0 : ABS(bodyYawDiff);
    interactInfo->headRot.y = CLAMP(interactInfo->headRot.y, -temp, temp);

    bodyYawDiff -= interactInfo->headRot.y;

    temp = CLAMP(bodyYawDiff, -maxTorsoYaw, maxTorsoYaw);
    add_calc_short_angle2(&interactInfo->torsoRot.y, temp, 6, 2000, 1);

    temp = (ABS(bodyYawDiff) >= 0x8000) ? 0 : ABS(bodyYawDiff);
    interactInfo->torsoRot.y = CLAMP(interactInfo->torsoRot.y, -temp, temp);

    if (rotateYaw) {
        add_calc_short_angle2(&actor->shape.rot.y, yawTowardsTarget, 6, 2000, 1);
    }

    pitch = pitchTowardsTarget;
    temp = CLAMP(pitch, minHeadPitch, maxHeadPitch);
    add_calc_short_angle2(&interactInfo->headRot.x, temp, 6, 2000, 1);

    pitch -= interactInfo->headRot.x;
    temp = CLAMP(pitch, minTorsoPitch, maxTorsoPitch);
    add_calc_short_angle2(&interactInfo->torsoRot.x, temp, 6, 2000, 1);
}

s16 get_search_angle(s16 presetIndex) {
    return eye_move_info[presetIndex].maxYawForPlayerTracking;
}

/**
 * Handles NPC tracking modes and auto-turning towards the player when
 * NPC_TRACKING_PLAYER_AUTO_TURN tracking mode is used.
 *
 * Returns a tracking mode that will determine which actor limbs
 * will be rotated towards the target.
 *
 * When the player is behind the actor (i.e. not in the yaw range in front of the actor
 * defined by maxYawForPlayerTracking), the actor will start an auto-turn sequence:
 *   - look forward for 30-60 frames
 *   - turn head to look at the player for 10-20 frames
 *   - look forward for 30-60 frames
 *   - turn the entire body to face the player
 *
 * @param distanceRange Max distance to player that tracking and auto-turning will be active for
 * @param maxYawForPlayerTracking Maximum angle for tracking the player.
 * @param trackingMode The tracking mode selected by the actor. If this is not
 *        NPC_TRACKING_PLAYER_AUTO_TURN this function does nothing
 *
 * @return The tracking mode (NpcTrackingMode) to use for the current frame.
 */
s16 check_behind(Actor* actor, NpcInteractInfo* interactInfo, f32 distanceRange, s16 maxYawForPlayerTracking,
                       s16 trackingMode) {

    s32 pad;
    s16 yaw;
    s16 yawDiff;

    if (trackingMode != NPC_TRACKING_PLAYER_AUTO_TURN) {
        return trackingMode;
    }

    if (interactInfo->talkState != NPC_TALK_STATE_IDLE) {
        // When talking, always fully turn to face the player
        return NPC_TRACKING_FULL_BODY;
    }

    if (distanceRange < search_position_distance(&actor->world.pos, &interactInfo->trackPos)) {
        // Player is too far away, do not track
        interactInfo->autoTurnTimer = 0;
        interactInfo->autoTurnState = 0;
        return NPC_TRACKING_NONE;
    }

    yaw = search_position_angleY(&actor->world.pos, &interactInfo->trackPos);
    yawDiff = ABS((s16)((f32)yaw - actor->shape.rot.y));
    if (maxYawForPlayerTracking >= yawDiff) {
        // Player is in front of the actor, track with the head and the torso
        interactInfo->autoTurnTimer = 0;
        interactInfo->autoTurnState = 0;
        return NPC_TRACKING_HEAD_AND_TORSO;
    }

    // Player is behind the actor, run the auto-turn sequence.

    if (DECR(interactInfo->autoTurnTimer) != 0) {
        // While the timer is still running, return the previous tracking mode
        return interactInfo->trackingMode;
    }

    switch (interactInfo->autoTurnState) {
        case 0:
        case 2:
            // Just stand still, not tracking the player
            interactInfo->autoTurnTimer = get_random_timer(30, 30);
            interactInfo->autoTurnState++;
            return NPC_TRACKING_NONE;
        case 1:
            // Glance at the player by only turning the head
            interactInfo->autoTurnTimer = get_random_timer(10, 10);
            interactInfo->autoTurnState++;
            return NPC_TRACKING_HEAD;
    }

    // Auto-turn sequence complete, turn towards the player
    return NPC_TRACKING_FULL_BODY;
}

/**
 * Rotates the actor's whole body, torso and head tracking the point specified in NpcInteractInfo.trackPos.
 * Uses angle limits from a preset selected from from eye_move_info.
 *
 * The trackingMode parameter controls whether the head and torso are turned towards the target.
 * If not, they are smoothly turned towards zero. Setting the parameter to NPC_TRACKING_FULL_BODY
 * causes the actor's whole body to be rotated to face the target.
 *
 * If NPC_TRACKING_PLAYER_AUTO_TURN is used, the actor will track the player with its head and torso as long
 * as the player is in front of the actor (within a yaw angle specified in the option preset).
 * If the player is outside of this angle, the actor will turn to face the player after a while.
 *
 * @see check_behind
 * @see eye_move_info
 * @see NpcTrackingMode
 *
 * @param presetIndex The index to a preset in eye_move_info
 * @param trackingMode A value from NpcTrackingMode enum
 */
void eye_moveM(Actor* actor, NpcInteractInfo* interactInfo, s16 presetIndex, s16 trackingMode) {
    NpcTrackingRotLimits rotLimits;

    interactInfo->trackingMode =
        check_behind(actor, interactInfo, eye_move_info[presetIndex].autoTurnDistanceRange,
                           eye_move_info[presetIndex].maxYawForPlayerTracking, trackingMode);

    rotLimits = eye_move_info[presetIndex].rotLimits;

    switch (interactInfo->trackingMode) {
        case NPC_TRACKING_NONE:
            rotLimits.maxHeadYaw = 0;
            rotLimits.maxHeadPitch = 0;
            rotLimits.minHeadPitch = 0;
            FALLTHROUGH;
        case NPC_TRACKING_HEAD:
            rotLimits.maxTorsoYaw = 0;
            rotLimits.maxTorsoPitch = 0;
            rotLimits.minTorsoPitch = 0;
            FALLTHROUGH;
        case NPC_TRACKING_HEAD_AND_TORSO:
            rotLimits.rotateYaw = false;
            break;
    }

    eye_moveS(actor, interactInfo, rotLimits.maxHeadYaw, rotLimits.maxHeadPitch, rotLimits.minHeadPitch,
                             rotLimits.maxTorsoYaw, rotLimits.maxTorsoPitch, rotLimits.minTorsoPitch,
                             rotLimits.rotateYaw);
}

static Gfx* npc_nothing(GraphicsContext* gfxCtx) {
    Gfx* displayList;

    displayList = GRAPH_ALLOC(gfxCtx, sizeof(Gfx));
    gSPEndDisplayList(displayList);

    return displayList;
}

Gfx* change_render_mode(GraphicsContext* gfxCtx) {
    Gfx* displayListHead;
    Gfx* displayList;

    displayList = displayListHead = GRAPH_ALLOC(gfxCtx, 2 * sizeof(Gfx));

    gDPSetRenderMode(displayListHead++, G_RM_FOG_SHADE_A,
                     AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
                         GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA));

    gSPEndDisplayList(displayListHead++);

    return displayList;
}

void no_clarity(PlayState* play, SkelAnime* skelAnime, OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw,
                   Actor* actor, s16 alpha) {
    OPEN_DISPS(play->state.gfxCtx, "../z_actor.c", 8831);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, alpha);
    gDPPipeSync(POLY_OPA_DISP++);
    gSPSegment(POLY_OPA_DISP++, 0x0C, npc_nothing(play->state.gfxCtx));

    POLY_OPA_DISP = Si2_draw2_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       overrideLimbDraw, postLimbDraw, actor, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_actor.c", 8860);
}

void clarity(PlayState* play, SkelAnime* skelAnime, OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw,
                   Actor* actor, s16 alpha) {
    OPEN_DISPS(play->state.gfxCtx, "../z_actor.c", 8876);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, change_render_mode(play->state.gfxCtx));

    POLY_XLU_DISP = Si2_draw2_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       overrideLimbDraw, postLimbDraw, actor, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_actor.c", 8904);
}

s16 kokiri_alpha_set(Actor* actor, PlayState* play, s16 alpha, f32 radius) {
    Player* player = GET_PLAYER(play);
    f32 distance;

    if ((play->csCtx.state != CS_STATE_IDLE) || debug_camera_sw) {
        distance = search_position_distance(&actor->world.pos, &play->view.eye) * 0.25f;
    } else {
        distance = search_position_distance(&actor->world.pos, &player->actor.world.pos);
    }

    if (radius < distance) {
        actor->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        add_calc_short_angle2(&alpha, 0, 6, 0x14, 1);
    } else {
        actor->flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        add_calc_short_angle2(&alpha, 0xFF, 6, 0x14, 1);
    }

    return alpha;
}

void npc_anime_ct(SkelAnime* skelAnime, AnimationInfo* animationInfo, s32 index) {
    f32 frameCount;

    animationInfo += index;

    if (animationInfo->frameCount > 0.0f) {
        frameCount = animationInfo->frameCount;
    } else {
        frameCount = Si2_anime_end_frame(animationInfo->animation);
    }

    Skeleton_Info2_init(skelAnime, animationInfo->animation, animationInfo->playSpeed, animationInfo->startFrame,
                     frameCount, animationInfo->mode, animationInfo->morphFrames);
}

/**
 * Fills two tables with rotation angles that can be used to simulate idle animations.
 *
 * The rotation angles are dependent on the current frame, so should be updated regularly, generally every frame.
 *
 * This is done for the desired limb by taking either the `sin` of the yTable value or the `cos` of the zTable value,
 * multiplying by some scale factor (generally 200), and adding that to the already existing rotation.
 *
 * Note: With the common scale factor of 200, this effect is practically unnoticeable if the current animation already
 * has motion involved.
 */
void program_wait(PlayState* play, s16* fidgetTableY, s16* fidgetTableZ, s32 tableLen) {
    u32 frames = play->gameplayFrames;
    s32 i;

    for (i = 0; i < tableLen; i++) {
        fidgetTableY[i] = frames * (FIDGET_FREQ_Y + FIDGET_FREQ_LIMB * i);
        fidgetTableZ[i] = frames * (FIDGET_FREQ_Z + FIDGET_FREQ_LIMB * i);
    }
}

#include "z_cheap_proc.inc.c"

#include "z_common.inc.c"
