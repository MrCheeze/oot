/*
 * File: z_en_boom.c
 * Overlay: ovl_En_Boom
 * Description: Thrown Boomerang. Actor spawns when thrown and is killed when caught.
 */

#include "z_en_boom.h"
#include "global.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Boom_actor_ct(Actor* thisx, PlayState* play);
void En_Boom_actor_dt(Actor* thisx, PlayState* play);
void En_Boom_actor_move(Actor* thisx, PlayState* play);
void En_Boom_actor_draw(Actor* thisx, PlayState* play);

void move_move(EnBoom* this, PlayState* play);

ActorProfile En_Boom_Profile = {
    /**/ ACTOR_EN_BOOM,
    /**/ ACTORCAT_MISC,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnBoom),
    /**/ En_Boom_actor_ct,
    /**/ En_Boom_actor_dt,
    /**/ En_Boom_actor_move,
    /**/ En_Boom_actor_draw,
};

static ColliderQuadInit ATBoomInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000010, 0x00, 0x01 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_NEAREST | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static InitChainEntry value_init[] = {
    ICHAIN_S8(attentionRangeType, ATTENTION_RANGE_5, ICHAIN_CONTINUE),
    ICHAIN_VEC3S(shape.rot, 0, ICHAIN_STOP),
};

void En_Boom_actor_set_process(EnBoom* this, EnBoomActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Boom_actor_ct(Actor* thisx, PlayState* play) {
    EnBoom* this = (EnBoom*)thisx;
    EffectBlureInit1 blure;

    this->actor.room = -1;

    ValueSet_process(&this->actor, value_init);

    blure.p1StartColor[0] = 255;
    blure.p1StartColor[1] = 255;
    blure.p1StartColor[2] = 100;
    blure.p1StartColor[3] = 255;

    blure.p2StartColor[0] = 255;
    blure.p2StartColor[1] = 255;
    blure.p2StartColor[2] = 100;
    blure.p2StartColor[3] = 64;

    blure.p1EndColor[0] = 255;
    blure.p1EndColor[1] = 255;
    blure.p1EndColor[2] = 100;
    blure.p1EndColor[3] = 0;

    blure.p2EndColor[0] = 255;
    blure.p2EndColor[1] = 255;
    blure.p2EndColor[2] = 100;
    blure.p2EndColor[3] = 0;

    blure.elemDuration = 8;
    blure.unkFlag = 0;
    blure.calcMode = 0;

    EffectAdd(play, &this->effectIndex, EFFECT_BLURE1, 0, 0, &blure);

    ClObjSwrd_ct(play, &this->collider);
    ClObjSwrd_set5(play, &this->collider, &this->actor, &ATBoomInfoData);

    En_Boom_actor_set_process(this, move_move);
}

void En_Boom_actor_dt(Actor* thisx, PlayState* play) {
    EnBoom* this = (EnBoom*)thisx;

    EffectFreeIndex(play, this->effectIndex);
    ClObjSwrd_dt(play, &this->collider);
}

void move_move(EnBoom* this, PlayState* play) {
    Actor* target;
    Player* player;
    s32 collided;
    s16 yawTarget;
    s16 yawDiff;
    s16 pitchTarget;
    s16 pitchDiff;
    s32 pad1;
    f32 distXYZScale;
    f32 distFromLink;
    DynaPolyActor* hitActor;
    s32 hitDynaID;
    Vec3f hitPoint;
    s32 pad2;

    player = GET_PLAYER(play);
    target = this->moveTo;

    // If the boomerang is moving toward a targeted actor, handle setting the proper x and y angle to fly toward it.
    if (target != NULL) {
        yawTarget = Actor_search_position_angleY(&this->actor, &target->focus.pos);
        yawDiff = this->actor.world.rot.y - yawTarget;

        pitchTarget = Actor_search_position_angleX(&this->actor, &target->focus.pos);
        pitchDiff = this->actor.world.rot.x - pitchTarget;

        distXYZScale = (200.0f - search_position_distance(&this->actor.world.pos, &target->focus.pos)) * 0.005f;
        if (distXYZScale < 0.12f) {
            distXYZScale = 0.12f;
        }

        if ((target != &player->actor) && ((target->update == NULL) || (ABS(yawDiff) > 0x4000))) {
            //! @bug  This condition is why the boomerang will randomly fly off in a the down left direction sometimes.
            //      If the actor targeted is not Link and the difference between the 2 y angles is greater than 0x4000,
            //      the moveTo pointer is nulled and it flies off in a seemingly random direction.
            this->moveTo = NULL;
        } else {
            chase_angle(&this->actor.world.rot.y, yawTarget, (s16)(ABS(yawDiff) * distXYZScale));
            chase_angle(&this->actor.world.rot.x, pitchTarget, (s16)(ABS(pitchDiff) * distXYZScale));
        }
    }

    // Set xyz speed, move forward, and play the boomerang sound effect
    Actor_vector_to_position_speed(&this->actor, 12.0f);
    Actor_position_moveF(&this->actor);
    Actor_level_SE_set(&this->actor, NA_SE_IT_BOOMERANG_FLY - SFX_FLAG);

    // If the boomerang collides with EnItem00 or a Skulltula token, set grabbed pointer to pick it up
    collided = this->collider.base.atFlags & AT_HIT;
    collided = !!(collided);
    if (collided) {
        if (((this->collider.base.at->id == ACTOR_EN_ITEM00) || (this->collider.base.at->id == ACTOR_EN_SI))) {
            this->grabbed = this->collider.base.at;
            if (this->collider.base.at->id == ACTOR_EN_SI) {
                this->collider.base.at->flags |= ACTOR_FLAG_HOOKSHOT_ATTACHED;
            }
        }
    }

    // Decrement the return timer and check if it's 0. If it is, check if Link can catch it and handle accordingly.
    // Otherwise handle grabbing and colliding.
    if (DECR(this->returnTimer) == 0) {
        distFromLink = search_position_distance(&this->actor.world.pos, &player->actor.focus.pos);
        this->moveTo = &player->actor;

        // If the boomerang is less than 40 units away from Link, he can catch it.
        if (distFromLink < 40.0f) {
            target = this->grabbed;
            if (target != NULL) {
                xyz_t_move(&target->world.pos, &player->actor.world.pos);

                // If the grabbed actor is EnItem00 (HP/Key etc) set gravity and flags so it falls in front of Link.
                // Otherwise if it's a Skulltula Token, just set flags so he collides with it to collect it.
                if (target->id == ACTOR_EN_ITEM00) {
                    target->gravity = -0.9f;
                    target->bgCheckFlags &= ~(BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH);
                } else {
                    target->flags &= ~ACTOR_FLAG_HOOKSHOT_ATTACHED;
                }
            }
            // Set player flags and kill the boomerang beacause Link caught it.
            player->stateFlags1 &= ~PLAYER_STATE1_BOOMERANG_THROWN;
            Actor_delete(&this->actor);
        }
    } else {
        collided = (this->collider.base.atFlags & AT_HIT);
        collided = (!!(collided));
        if (collided) {
            // Copy the position from the prevous frame to the boomerang to start the bounce back.
            xyz_t_move(&this->actor.world.pos, &this->actor.prevPos);
        } else {
            collided = T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &this->actor.prevPos, &this->actor.world.pos, &hitPoint,
                                               &this->actor.wallPoly, true, true, true, true, &hitDynaID);

            if (collided) {
                // If the boomerang collides with something and it's is a Jabu Object actor with params equal to 0, then
                // set collided to 0 so that the boomerang will go through the wall.
                // Otherwise play a clank sound effect and keep collided set to bounce back.
                if (jyabujyabu_kiru_check(play, &this->actor, this->actor.wallPoly, hitDynaID, &hitPoint) != 0 ||
                    (hitDynaID != BGCHECK_SCENE && ((hitActor = DynaPolyInfo_actor_index2pointer(&play->colCtx, hitDynaID)) != NULL) &&
                     hitActor->actor.id == ACTOR_BG_BDAN_OBJECTS && hitActor->actor.params == 0)) {
                    collided = false;
                } else {
                    CollisionCheckSetSparkFlashBlue(play, &hitPoint);
                }
            }
        }

        // If the boomerang needs to bounce back, set x and y angle accordingly.
        // Set timer to 0 and set return actor to player so it goes back to Link.
        if (collided) {
            this->actor.world.rot.x = -this->actor.world.rot.x;
            this->actor.world.rot.y += 0x8000;
            this->moveTo = &player->actor;
            this->returnTimer = 0;
        }
    }

    // If the actor the boomerang is holding has a null update function, set grabbed to null.
    // Otherwise, copy the position from the boomerang to the actor to move it.
    target = this->grabbed;
    if (target != NULL) {
        if (target->update == NULL) {
            this->grabbed = NULL;
        } else {
            xyz_t_move(&target->world.pos, &this->actor.world.pos);
        }
    }
}

void En_Boom_actor_move(Actor* thisx, PlayState* play) {
    EnBoom* this = (EnBoom*)thisx;
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 & PLAYER_STATE1_29)) {
        this->actionFunc(this, play);
        Actor_world_to_eye(&this->actor, 0.0f);
        this->activeTimer++;
    }
}

void En_Boom_actor_draw(Actor* thisx, PlayState* play) {
    static Vec3f local_sword_top = { -960.0f, 0.0f, 0.0f };
    static Vec3f local_sword_root = { 960.0f, 0.0f, 0.0f };
    EnBoom* this = (EnBoom*)thisx;
    Vec3f vec1;
    Vec3f vec2;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_boom.c", 567);

    Matrix_rotateY(BINANG_TO_RAD(this->actor.world.rot.y), MTXMODE_APPLY);
    Matrix_rotateZ(BINANG_TO_RAD(0x1F40), MTXMODE_APPLY);
    Matrix_rotateX(BINANG_TO_RAD(this->actor.world.rot.x), MTXMODE_APPLY);
    Matrix_Position(&local_sword_top, &vec1);
    Matrix_Position(&local_sword_root, &vec2);

    if (sword_attack_collision_set(play, &this->collider, &this->boomerangInfo, &vec1, &vec2)) {
        EffectBlure_edge_add(Effect_GetEffectMemoryPointer(this->effectIndex), &vec1, &vec2);
    }

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Matrix_rotateY(BINANG_TO_RAD(this->activeTimer * 12000), MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_boom.c", 601);
    gSPDisplayList(POLY_OPA_DISP++, gBoomerangRefDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_boom.c", 604);
}
