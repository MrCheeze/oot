#include "z_arms_hook.h"

#include "libc64/math64.h"
#include "controller.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "sfx.h"
#include "sys_math.h"
#include "sys_matrix.h"
#include "z64play.h"
#include "z64player.h"
#include "z_lib.h"

#include "assets/objects/object_link_boy/object_link_boy.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Arms_Hook_actor_ct(Actor* thisx, PlayState* play);
void Arms_Hook_actor_dt(Actor* thisx, PlayState* play);
void Arms_Hook_actor_move(Actor* thisx, PlayState* play);
void Arms_Hook_actor_draw(Actor* thisx, PlayState* play);

static void move_catch(ArmsHook* this, PlayState* play);
static void move_shot(ArmsHook* this, PlayState* play);

ActorProfile Arms_Hook_Profile = {
    /**/ ACTOR_ARMS_HOOK,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_LINK_BOY,
    /**/ sizeof(ArmsHook),
    /**/ Arms_Hook_actor_ct,
    /**/ Arms_Hook_actor_dt,
    /**/ Arms_Hook_actor_move,
    /**/ Arms_Hook_actor_draw,
};

static ColliderQuadInit ATHookInfoData = {
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
        { 0x00000080, 0x00, 0x01 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_NEAREST | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static Vec3f fire_vec = { 0.0f, 0.5f, 0.0f };
static Vec3f fire_acc = { 0.0f, 0.5f, 0.0f };

static Color_RGB8 fire_prim[] = {
    { 255, 255, 100 },
    { 255, 255, 50 },
};

static Vec3f local_BGcheck_point_root = { 0.0f, 0.0f, 0.0f };
static Vec3f local_shot_BGcheck_point_root = { 0.0f, 0.0f, 900.0f };
static Vec3f local_sword_top = { 0.0f, 500.0f, -3000.0f };
static Vec3f local_sword_root = { 0.0f, -500.0f, -3000.0f };
static Vec3f local_shot_sword_top = { 0.0f, 500.0f, 1200.0f };
static Vec3f local_shot_sword_root = { 0.0f, -500.0f, 1200.0f };

void Arms_Hook_actor_set_process(ArmsHook* this, ArmsHookActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Arms_Hook_actor_ct(Actor* thisx, PlayState* play) {
    ArmsHook* this = (ArmsHook*)thisx;

    ClObjSwrd_ct(play, &this->collider);
    ClObjSwrd_set5(play, &this->collider, &this->actor, &ATHookInfoData);
    Arms_Hook_actor_set_process(this, move_catch);
    this->unk_1E8 = this->actor.world.pos;
}

void Arms_Hook_actor_dt(Actor* thisx, PlayState* play) {
    ArmsHook* this = (ArmsHook*)thisx;

    if (this->attachedActor != NULL) {
        this->attachedActor->flags &= ~ACTOR_FLAG_HOOKSHOT_ATTACHED;
    }

    ClObjSwrd_dt(play, &this->collider);
}

static void move_catch(ArmsHook* this, PlayState* play) {
    if (this->actor.parent == NULL) {
        Player* player = GET_PLAYER(play);
        // get correct timer length for hookshot or longshot
        s32 length = (player->heldItemAction == PLAYER_IA_HOOKSHOT) ? 13 : 26;

        Arms_Hook_actor_set_process(this, move_shot);
        Actor_vector_to_position_speed(&this->actor, 20.0f);
        this->actor.parent = &GET_PLAYER(play)->actor;
        this->timer = length;
    }
}

/**
 * Start pulling Player so he flies toward the hookshot's current location.
 * Setting Player's parent pointer indicates that he should begin flying.
 * See `uperbody_action_check` and `move_hook_fly` for Player's side of the interation.
 */
void player_carry_set(ArmsHook* this) {
    this->actor.child = this->actor.parent;
    this->actor.parent->parent = &this->actor;
}

s32 player_carry_cancel_check(ArmsHook* this, Player* player) {
    player->actor.child = &this->actor;
    player->heldActor = &this->actor;
    if (this->actor.child != NULL) {
        player->actor.parent = NULL;
        this->actor.child = NULL;
        return true;
    }
    return false;
}

void carry_actor_cancel_check(ArmsHook* this) {
    if (this->attachedActor != NULL) {
        this->attachedActor->flags &= ~ACTOR_FLAG_HOOKSHOT_ATTACHED;
        this->attachedActor = NULL;
    }
}

s32 player_down_check(ArmsHook* this) {
    Player* player = (Player*)this->actor.parent;

    if (hook_check(player)) {
        if ((player->itemAction != player->heldItemAction) || (player->actor.flags & ACTOR_FLAG_TALK) ||
            ((player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_26)))) {
            this->timer = 0;
            carry_actor_cancel_check(this);
            xyz_t_move(&this->actor.world.pos, &player->unk_3C8);
            return 1;
        }
    }
    return 0;
}

void hook_carry_check(ArmsHook* this, Actor* actor) {
    actor->flags |= ACTOR_FLAG_HOOKSHOT_ATTACHED;
    this->attachedActor = actor;
    xyz_t_sub(&actor->world.pos, &this->actor.world.pos, &this->attachPointOffset);
}

static void move_shot(ArmsHook* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((this->actor.parent == NULL) || (!hook_check(player))) {
        carry_actor_cancel_check(this);
        Actor_delete(&this->actor);
        return;
    }

    Actor_player_level_SE_set(&player->actor, NA_SE_IT_HOOKSHOT_CHAIN - SFX_FLAG);
    player_down_check(this);

    if ((this->timer != 0) && (this->collider.base.atFlags & AT_HIT) &&
        (this->collider.elem.atHitElem->elemMaterial != ELEM_MATERIAL_UNK4)) {
        Actor* touchedActor = this->collider.base.at;

        if ((touchedActor->update != NULL) &&
            (touchedActor->flags & (ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR | ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER))) {
            if (this->collider.elem.atHitElem->acElemFlags & ACELEM_HOOKABLE) {
                hook_carry_check(this, touchedActor);

                if (CHECK_FLAG_ALL(touchedActor->flags, ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)) {
                    player_carry_set(this);
                }
            }
        }
        this->timer = 0;
        Nai_FxFlagEntry(NA_SE_IT_ARROW_STICK_CRE, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        return;
    }

    if (DECR(this->timer) == 0) {
        Actor* attachedActor;
        Vec3f bodyDistDiffVec;
        Vec3f newPos;
        f32 bodyDistDiff;
        f32 phi_f16;
        s32 pad1;
        f32 curActorOffsetXYZ;
        f32 attachPointOffsetXYZ;
        f32 velocity;

        attachedActor = this->attachedActor;

        if (attachedActor != NULL) {
            if ((attachedActor->update == NULL) ||
                !CHECK_FLAG_ALL(attachedActor->flags, ACTOR_FLAG_HOOKSHOT_ATTACHED)) {
                attachedActor = NULL;
                this->attachedActor = NULL;
            } else if (this->actor.child != NULL) {
                curActorOffsetXYZ = Actor_search_actor_distance(&this->actor, attachedActor);
                attachPointOffsetXYZ = sqrtf(SQ(this->attachPointOffset.x) + SQ(this->attachPointOffset.y) +
                                             SQ(this->attachPointOffset.z));

                // Keep the hookshot actor at the same relative offset as the initial attachment even if the actor moves
                xyz_t_sub(&attachedActor->world.pos, &this->attachPointOffset, &this->actor.world.pos);

                // If the actor the hookshot is attached to is moving, the hookshot's current relative
                // position will be different than the initial attachment position.
                // If the distance between those two points is larger than 50 units, detach the hookshot.
                if ((curActorOffsetXYZ - attachPointOffsetXYZ) > 50.0f) {
                    carry_actor_cancel_check(this);
                    attachedActor = NULL;
                }
            }
        }

        bodyDistDiff = search_position_distance2(&player->unk_3C8, &this->actor.world.pos, &bodyDistDiffVec);

        if (bodyDistDiff < 30.0f) {
            velocity = 0.0f;
            phi_f16 = 0.0f;
        } else {
            if (this->actor.child != NULL) {
                velocity = 30.0f;
            } else if (attachedActor != NULL) {
                velocity = 50.0f;
            } else {
                velocity = 200.0f;
            }
            phi_f16 = bodyDistDiff - velocity;
            if (bodyDistDiff <= velocity) {
                phi_f16 = 0.0f;
            }
            velocity = phi_f16 / bodyDistDiff;
        }

        newPos.x = bodyDistDiffVec.x * velocity;
        newPos.y = bodyDistDiffVec.y * velocity;
        newPos.z = bodyDistDiffVec.z * velocity;

        if (this->actor.child == NULL) {
            // Not pulling Player
            if ((attachedActor != NULL) && (attachedActor->id == ACTOR_BG_SPOT06_OBJECTS)) {
                xyz_t_sub(&attachedActor->world.pos, &this->attachPointOffset, &this->actor.world.pos);
                phi_f16 = 1.0f;
            } else {
                xyz_t_add(&player->unk_3C8, &newPos, &this->actor.world.pos);
                if (attachedActor != NULL) {
                    xyz_t_add(&this->actor.world.pos, &this->attachPointOffset, &attachedActor->world.pos);
                }
            }
        } else {
            // Pulling Player
            xyz_t_sub(&bodyDistDiffVec, &newPos, &player->actor.velocity);
            player->actor.world.rot.x =
                atans_table(sqrtf(SQ(bodyDistDiffVec.x) + SQ(bodyDistDiffVec.z)), -bodyDistDiffVec.y);
        }

        if (phi_f16 < 50.0f) {
            carry_actor_cancel_check(this);
            if (phi_f16 == 0.0f) {
                Arms_Hook_actor_set_process(this, move_catch);
                if (player_carry_cancel_check(this, player)) {
                    xyz_t_sub(&this->actor.world.pos, &player->actor.world.pos, &player->actor.velocity);
                    player->actor.velocity.y -= 20.0f;
                }
            }
        }
    } else {
        CollisionPoly* poly;
        s32 bgId;
        Vec3f intersectPos;
        Vec3f prevFrameDiff;
        Vec3f sp60;

        Actor_position_moveF(&this->actor);
        xyz_t_sub(&this->actor.world.pos, &this->actor.prevPos, &prevFrameDiff);
        xyz_t_add(&this->unk_1E8, &prevFrameDiff, &this->unk_1E8);
        this->actor.shape.rot.x = atans_table(this->actor.speed, -this->actor.velocity.y);
        sp60.x = this->unk_1F4.x - (this->unk_1E8.x - this->unk_1F4.x);
        sp60.y = this->unk_1F4.y - (this->unk_1E8.y - this->unk_1F4.y);
        sp60.z = this->unk_1F4.z - (this->unk_1E8.z - this->unk_1F4.z);
        if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &sp60, &this->unk_1E8, &intersectPos, &poly, true, true, true, true,
                                    &bgId) &&
            !jyabujyabu_kiru_check(play, &this->actor, poly, bgId, &intersectPos)) {
            f32 polyNormalX = COLPOLY_GET_NORMAL(poly->normal.x);
            f32 polyNormalZ = COLPOLY_GET_NORMAL(poly->normal.z);
            s32 pad;

            xyz_t_move(&this->actor.world.pos, &intersectPos);
            this->actor.world.pos.x += 10.0f * polyNormalX;
            this->actor.world.pos.z += 10.0f * polyNormalZ;
            this->timer = 0;
            if (T_BGCheck_checkHookShotStick(&play->colCtx, poly, bgId)) {
                DynaPolyActor* dynaPolyActor;

                if (bgId != BGCHECK_SCENE) {
                    dynaPolyActor = DynaPolyInfo_actor_index2pointer(&play->colCtx, bgId);

                    if (dynaPolyActor != NULL) {
                        hook_carry_check(this, &dynaPolyActor->actor);
                    }
                }
                player_carry_set(this);
                Nai_FxFlagEntry(NA_SE_IT_HOOKSHOT_STICK_OBJ, &this->actor.projectedPos, 4,
                                     &_dummy_one, &_dummy_one, &_dummy_zero_s8);
            } else {
                CollisionCheckSetSparkFlashBlue(play, &this->actor.world.pos);
                Nai_FxFlagEntry(NA_SE_IT_HOOKSHOT_REFLECT, &this->actor.projectedPos, 4,
                                     &_dummy_one, &_dummy_one, &_dummy_zero_s8);
            }
        } else if (CHECK_BTN_ANY(play->state.input[0].press.button,
                                 (BTN_A | BTN_B | BTN_R | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT))) {
            this->timer = 0;
        }
    }
}

void Arms_Hook_actor_move(Actor* thisx, PlayState* play) {
    ArmsHook* this = (ArmsHook*)thisx;

    this->actionFunc(this, play);
    this->unk_1F4 = this->unk_1E8;
}

void Arms_Hook_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    ArmsHook* this = (ArmsHook*)thisx;
    Player* player = GET_PLAYER(play);
    Vec3f sp78;
    Vec3f hookNewTip;
    Vec3f hookNewBase;
    f32 sp5C;
    f32 sp58;

    if ((player->actor.draw != NULL) && (player->rightHandType == PLAYER_MODELTYPE_RH_HOOKSHOT)) {
        OPEN_DISPS(play->state.gfxCtx, "../z_arms_hook.c", 850);

        if (1) {}

        if ((move_shot != this->actionFunc) || (this->timer <= 0)) {
            Matrix_Position(&local_BGcheck_point_root, &this->unk_1E8);
            Matrix_Position(&local_sword_top, &hookNewTip);
            Matrix_Position(&local_sword_root, &hookNewBase);
            this->hookInfo.active = 0;
        } else {
            Matrix_Position(&local_shot_BGcheck_point_root, &this->unk_1E8);
            Matrix_Position(&local_shot_sword_top, &hookNewTip);
            Matrix_Position(&local_shot_sword_root, &hookNewBase);
        }

        sword_attack_collision_set(play, &this->collider, &this->hookInfo, &hookNewTip, &hookNewBase);
        _texture_z_light_fog_prim(play->state.gfxCtx);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_arms_hook.c", 895);
        gSPDisplayList(POLY_OPA_DISP++, gLinkAdultHookshotTipDL);
        Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        xyz_t_sub(&player->unk_3C8, &this->actor.world.pos, &sp78);
        sp58 = SQ(sp78.x) + SQ(sp78.z);
        sp5C = sqrtf(sp58);
        Matrix_rotateY(fatan2(sp78.x, sp78.z), MTXMODE_APPLY);
        Matrix_rotateX(fatan2(-sp78.y, sp5C), MTXMODE_APPLY);
        Matrix_scale(0.015f, 0.015f, sqrtf(SQ(sp78.y) + sp58) * 0.01f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_arms_hook.c", 910);
        gSPDisplayList(POLY_OPA_DISP++, gLinkAdultHookshotChainDL);

        CLOSE_DISPS(play->state.gfxCtx, "../z_arms_hook.c", 913);
    }
}
