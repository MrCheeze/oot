/*
 * File: z_en_cow.c
 * Overlay: ovl_En_Cow
 * Description: Cow
 */

#include "z_en_cow.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Cow_Actor_ct(Actor* thisx, PlayState* play);
void En_Cow_Actor_dt(Actor* thisx, PlayState* play);
void En_Cow_Actor_move(Actor* thisx, PlayState* play2);
void En_Cow_Actor_draw(Actor* thisx, PlayState* play);

void move_talk_end(EnCow* this, PlayState* play);
void move_talk_end2(EnCow* this, PlayState* play);
static void move_give(EnCow* this, PlayState* play);
void move_talk2(EnCow* this, PlayState* play);
static void move_talk(EnCow* this, PlayState* play);
void owl_common_after(EnCow* this, PlayState* play);
void move_coll(EnCow* this, PlayState* play);
static void move_wait(EnCow* this, PlayState* play);

void En_CowTail_Actor_draw(Actor* thisx, PlayState* play);
void En_CowTail_Actor_move(Actor* thisx, PlayState* play);
void move_tail(EnCow* this, PlayState* play);

ActorProfile En_Cow_Profile = {
    /**/ ACTOR_EN_COW,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_COW,
    /**/ sizeof(EnCow),
    /**/ En_Cow_Actor_ct,
    /**/ En_Cow_Actor_dt,
    /**/ En_Cow_Actor_move,
    /**/ En_Cow_Actor_draw,
};

static ColliderCylinderInit EnCowOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0, 0, 0 } },
};

static Vec3f pos = { 0.0f, -1300.0f, 1100.0f };

void rotate_y_pos(Vec3f* vec, s16 rotY) {
    f32 xCalc;
    f32 rotCalcTemp;

    rotCalcTemp = cos_s(rotY);
    xCalc = (sin_s(rotY) * vec->z) + (rotCalcTemp * vec->x);
    rotCalcTemp = sin_s(rotY);

    vec->z = (cos_s(rotY) * vec->z) + (-rotCalcTemp * vec->x);
    vec->x = xCalc;
}

void set_collision_pos(EnCow* this) {
    Vec3f vec;

    vec.y = 0.0f;
    vec.x = 0.0f;
    vec.z = 30.0f;

    rotate_y_pos(&vec, this->actor.shape.rot.y);

    this->colliders[COW_COLLIDER_FRONT].dim.pos.x = this->actor.world.pos.x + vec.x;
    this->colliders[COW_COLLIDER_FRONT].dim.pos.y = this->actor.world.pos.y;
    this->colliders[COW_COLLIDER_FRONT].dim.pos.z = this->actor.world.pos.z + vec.z;

    vec.x = 0.0f;
    vec.y = 0.0f;
    vec.z = -20.0f;

    rotate_y_pos(&vec, this->actor.shape.rot.y);

    this->colliders[COW_COLLIDER_REAR].dim.pos.x = this->actor.world.pos.x + vec.x;
    this->colliders[COW_COLLIDER_REAR].dim.pos.y = this->actor.world.pos.y;
    this->colliders[COW_COLLIDER_REAR].dim.pos.z = this->actor.world.pos.z + vec.z;
}

void set_tail_pos(EnCow* this) {
    Vec3f vec;

    VEC_SET(vec, 0.0f, 57.0f, -36.0f);
    rotate_y_pos(&vec, this->actor.shape.rot.y);

    this->actor.world.pos.x += vec.x;
    this->actor.world.pos.y += vec.y;
    this->actor.world.pos.z += vec.z;
}

void En_Cow_Actor_ct(Actor* thisx, PlayState* play) {
    EnCow* this = (EnCow*)thisx;
    s32 pad;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 72.0f);

    switch (COW_GET_TYPE(this)) {
        case COW_TYPE_BODY:
            Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gCowBodySkel, NULL, this->jointTable, this->morphTable,
                               COW_LIMB_MAX);
            Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gCowBodyChewAnim);

            ClObjPipe_ct(play, &this->colliders[COW_COLLIDER_FRONT]);
            ClObjPipe_set5(play, &this->colliders[COW_COLLIDER_FRONT], &this->actor, &EnCowOcInfoData);

            ClObjPipe_ct(play, &this->colliders[COW_COLLIDER_REAR]);
            ClObjPipe_set5(play, &this->colliders[COW_COLLIDER_REAR], &this->actor, &EnCowOcInfoData);

            set_collision_pos(this);

            this->actionFunc = move_wait;

            if (play->sceneId == SCENE_LINKS_HOUSE) {
                if (!LINK_IS_ADULT) {
                    Actor_delete(&this->actor);
                    return;
                }

                if (!GET_EVENTCHKINF(EVENTCHKINF_HORSE_RACE_COW_UNLOCK)) {
                    Actor_delete(&this->actor);
                    return;
                }
            }

            Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_COW, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0,
                               COW_TYPE_TAIL);
            this->animationTimer = rnd_f(1000.0f) + 40.0f;
            this->breathTimer = 0;
            this->actor.attentionRangeType = ATTENTION_RANGE_6;
            R_EPONAS_SONG_PLAYED = false;
            break;

        case COW_TYPE_TAIL:
            Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gCowTailSkel, NULL, this->jointTable, this->morphTable,
                               COW_TAIL_LIMB_MAX);
            Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gCowTailIdleAnim);
            this->actor.update = En_CowTail_Actor_move;
            this->actor.draw = En_CowTail_Actor_draw;
            this->actionFunc = move_tail;
            set_tail_pos(this);
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->animationTimer = (u16)rnd_f(1000.0f) + 40.0f;
            break;
    }

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_set_scale(&this->actor, 0.01f);
    this->cowFlags = 0;
}

void En_Cow_Actor_dt(Actor* thisx, PlayState* play) {
    EnCow* this = (EnCow*)thisx;

    if (COW_GET_TYPE(this) == COW_TYPE_BODY) {
        ClObjPipe_dt(play, &this->colliders[COW_COLLIDER_FRONT]);
        ClObjPipe_dt(play, &this->colliders[COW_COLLIDER_REAR]);
    }
}

void owl_common_after(EnCow* this, PlayState* play) {
    if (this->animationTimer > 0) {
        this->animationTimer--;
    } else {
        this->animationTimer = rnd_f(500.0f) + 40.0f;
        Skeleton_Info2_init(&this->skelAnime, &gCowBodyChewAnim, 1.0f, this->skelAnime.curFrame,
                         Si2_anime_end_frame(&gCowBodyChewAnim), ANIMMODE_ONCE, 1.0f);
    }

    if (this->actor.xzDistToPlayer < 150.0f) {
        if (!(this->cowFlags & COW_FLAG_PLAYER_NEARBY)) {
            this->cowFlags |= COW_FLAG_PLAYER_NEARBY;

            if (this->skelAnime.animation == &gCowBodyChewAnim) {
                this->animationTimer = 0;
            }
        }
    }

    this->breathTimer++;

    if (this->breathTimer > 48) {
        this->breathTimer = 0;
    }

    if (this->breathTimer < 32) {
        this->actor.scale.x = ((sin_s(this->breathTimer * 0x0400) * (1.0f / 100.0f)) + 1.0f) * 0.01f;
    } else {
        this->actor.scale.x = 0.01f;
    }

    if (this->breathTimer > 16) {
        this->actor.scale.y = ((sin_s((this->breathTimer * 0x0400) - 0x4000) * (1.0f / 100.0f)) + 1.0f) * 0.01f;
    } else {
        this->actor.scale.y = 0.01f;
    }
}

void move_talk_end(EnCow* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        message_close(play);
        this->actionFunc = move_wait;
    }
}

void move_talk_end2(EnCow* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = move_wait;
    }
}

static void move_give(EnCow* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = move_talk_end2;
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_MILK, 10000.0f, 100.0f);
    }
}

void move_talk2(EnCow* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        message_close(play);
        this->actionFunc = move_give;
        Actor_carry_request_set2(&this->actor, play, GI_MILK, 10000.0f, 100.0f);
    }
}

static void move_talk(EnCow* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        if (findEmptyBottle()) {
            message_set2(play, 0x2007);
            this->actionFunc = move_talk2;
        } else {
            message_set2(play, 0x2013);
            this->actionFunc = move_talk_end;
        }
    }
}

void move_coll(EnCow* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = move_talk;
    } else {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_talk_request2(&this->actor, play, 170.0f);
        this->actor.textId = 0x2006;
    }

    owl_common_after(this, play);
}

static void move_wait(EnCow* this, PlayState* play) {
    if ((play->msgCtx.ocarinaMode == OCARINA_MODE_00) || (play->msgCtx.ocarinaMode == OCARINA_MODE_04)) {
        // There is a complex interaction between `R_EPONAS_SONG_PLAYED` and `COW_FLAG_FAILED_TO_GIVE_MILK` to allow
        // multiple cows to try and give milk on the same frame.
        // `COW_FLAG_FAILED_TO_GIVE_MILK` gets set if this cow is not in range with the player to interact.
        // In the case of a failure, `R_EPONAS_SONG_PLAYED` is not set to false in case another cow can succeed.
        // On the following frame, if both `R_EPONAS_SONG_PLAYED` and `COW_FLAG_FAILED_TO_GIVE_MILK` are set, the
        // first cow that updates can assume all other cows also failed and can safely unset `R_EPONAS_SONG_PLAYED`.
        // All cows also unset their own `COW_FLAG_FAILED_TO_GIVE_MILK` flag.
        if (R_EPONAS_SONG_PLAYED) {
            if (this->cowFlags & COW_FLAG_FAILED_TO_GIVE_MILK) {
                this->cowFlags &= ~COW_FLAG_FAILED_TO_GIVE_MILK;
                R_EPONAS_SONG_PLAYED = false;
            } else {
                if ((this->actor.xzDistToPlayer < 150.0f) &&
                    (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) < 25000)) {
                    R_EPONAS_SONG_PLAYED = false;
                    this->actionFunc = move_coll;
                    this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                    Actor_talk_request2(&this->actor, play, 170.0f);
                    this->actor.textId = 0x2006;
                } else {
                    this->cowFlags |= COW_FLAG_FAILED_TO_GIVE_MILK;
                }
            }
        } else {
            this->cowFlags &= ~COW_FLAG_FAILED_TO_GIVE_MILK;
        }
    }

    owl_common_after(this, play);
}

void move_tail(EnCow* this, PlayState* play) {
    if (this->animationTimer > 0) {
        this->animationTimer--;
    } else {
        this->animationTimer = rnd_f(200.0f) + 40.0f;
        Skeleton_Info2_init(&this->skelAnime, &gCowTailIdleAnim, 1.0f, this->skelAnime.curFrame,
                         Si2_anime_end_frame(&gCowTailIdleAnim), ANIMMODE_ONCE, 1.0f);
    }

    if ((this->actor.xzDistToPlayer < 150.0f) &&
        (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) > 25000)) {
        if (!(this->cowFlags & COW_FLAG_PLAYER_NEARBY)) {
            this->cowFlags |= COW_FLAG_PLAYER_NEARBY;

            if (this->skelAnime.animation == &gCowTailIdleAnim) {
                this->animationTimer = 0;
            }
        }
    }
}

void En_Cow_Actor_move(Actor* thisx, PlayState* play2) {
    EnCow* this = (EnCow*)thisx;
    PlayState* play = play2;
    s16 targetX;
    s16 targetY;
    Player* player = GET_PLAYER(play);

    CollisionCheck_setOC(play, &play->colChkCtx, &this->colliders[COW_COLLIDER_FRONT].base);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->colliders[COW_COLLIDER_REAR].base);

    Actor_position_moveF(thisx);
    Actor_BGcheck2(play, thisx, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->skelAnime.animation == &gCowBodyChewAnim) {
            Actor_SE_set(thisx, NA_SE_EV_COW_CRY);
            Skeleton_Info2_init(&this->skelAnime, &gCowBodyMoveHeadAnim, 1.0f, 0.0f,
                             Si2_anime_end_frame(&gCowBodyMoveHeadAnim), ANIMMODE_ONCE, 1.0f);
        } else {
            Skeleton_Info2_init(&this->skelAnime, &gCowBodyChewAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gCowBodyChewAnim),
                             ANIMMODE_LOOP, 1.0f);
        }
    }

    this->actionFunc(this, play);

    if ((thisx->xzDistToPlayer < 150.0f) &&
        (ABS(search_position_angleY(&thisx->world.pos, &player->actor.world.pos)) < 0xC000)) {
        targetX = search_position_angleX(&thisx->focus.pos, &player->actor.focus.pos);
        targetY = search_position_angleY(&thisx->focus.pos, &player->actor.focus.pos) - thisx->shape.rot.y;

        if (targetX > 0x1000) {
            targetX = 0x1000;
        } else if (targetX < -0x1000) {
            targetX = -0x1000;
        }

        if (targetY > 0x2500) {
            targetY = 0x2500;
        } else if (targetY < -0x2500) {
            targetY = -0x2500;
        }
    } else {
        targetY = 0;
        targetX = 0;
    }

    add_calc_short_angle2(&this->headRot.x, targetX, 10, 200, 10);
    add_calc_short_angle2(&this->headRot.y, targetY, 10, 200, 10);
}

void En_CowTail_Actor_move(Actor* thisx, PlayState* play) {
    EnCow* this = (EnCow*)thisx;
    s32 pad;

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->skelAnime.animation == &gCowTailIdleAnim) {
            Skeleton_Info2_init(&this->skelAnime, &gCowTailSwishAnim, 1.0f, 0.0f,
                             Si2_anime_end_frame(&gCowTailSwishAnim), ANIMMODE_ONCE, 1.0f);
        } else {
            Skeleton_Info2_init(&this->skelAnime, &gCowTailIdleAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gCowTailIdleAnim),
                             ANIMMODE_LOOP, 1.0f);
        }
    }

    this->actionFunc(this, play);
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnCow* this = (EnCow*)thisx;

    if (limbIndex == COW_LIMB_HEAD) {
        rot->y += this->headRot.y;
        rot->x += this->headRot.x;
    }

    if (limbIndex == COW_LIMB_NOSE_RING) {
        *dList = NULL;
    }

    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnCow* this = (EnCow*)thisx;

    if (limbIndex == COW_LIMB_HEAD) {
        Matrix_Position(&pos, &this->actor.focus.pos);
    }
}

void En_Cow_Actor_draw(Actor* thisx, PlayState* play) {
    EnCow* this = (EnCow*)thisx;

    _polygon_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, this);
}

void En_CowTail_Actor_draw(Actor* thisx, PlayState* play) {
    EnCow* this = (EnCow*)thisx;

    _polygon_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, this);
}
