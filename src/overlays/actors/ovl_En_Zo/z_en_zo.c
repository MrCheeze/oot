/*
 * File: z_en_zo.c
 * Overlay: ovl_En_Zo
 * Description: Zora
 */

#include "z_en_zo.h"
#include "assets/objects/object_zo/object_zo.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

typedef enum EnZoEffectType {
    /* 0 */ ENZO_EFFECT_NONE,
    /* 1 */ ENZO_EFFECT_RIPPLE,
    /* 2 */ ENZO_EFFECT_SPLASH,
    /* 3 */ ENZO_EFFECT_BUBBLE
} EnZoEffectType;

void En_Zo_actor_ct(Actor* thisx, PlayState* play);
void En_Zo_actor_dt(Actor* thisx, PlayState* play);
void En_Zo_actor_move(Actor* thisx, PlayState* play);
void En_Zo_actor_draw(Actor* thisx, PlayState* play);

// Actions
void zo_wait(EnZo* this, PlayState* play);
void zo_wait_deep(EnZo* this, PlayState* play);
void zo_float(EnZo* this, PlayState* play);
void zo_wait_surface(EnZo* this, PlayState* play);
void zo_dive(EnZo* this, PlayState* play);

#include "z_en_zo_eff.inc.c"

static ColliderCylinderInit ZoClPipeData = {
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
    { 26, 64, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 ZoStatusData = { 0, 0, 0, 0, MASS_IMMOVABLE };

ActorProfile En_Zo_Profile = {
    /**/ ACTOR_EN_ZO,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ZO,
    /**/ sizeof(EnZo),
    /**/ En_Zo_actor_ct,
    /**/ En_Zo_actor_dt,
    /**/ En_Zo_actor_move,
    /**/ En_Zo_actor_draw,
};

typedef enum EnZoAnimation {
    /* 0 */ ENZO_ANIM_0,
    /* 1 */ ENZO_ANIM_1,
    /* 2 */ ENZO_ANIM_2,
    /* 3 */ ENZO_ANIM_3,
    /* 4 */ ENZO_ANIM_4,
    /* 5 */ ENZO_ANIM_5,
    /* 6 */ ENZO_ANIM_6,
    /* 7 */ ENZO_ANIM_7
} EnZoAnimation;

static AnimationInfo animetbl[] = {
    { &gZoraIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gZoraIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gZoraSurfaceAnim, 0.0f, 1.0f, 1.0f, ANIMMODE_ONCE, 0.0f },
    { &gZoraSurfaceAnim, 1.0f, 1.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gZoraSurfaceAnim, 1.0f, 8.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gZoraThrowRupeesAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gZoraHandsOnHipsTappingFootAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gZoraOpenArmsAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
};

u16 zo_set_message(PlayState* play, Actor* thisx) {
    u16 textId = get_mask_message(play, MASK_REACTION_SET_ZORA);

    if (textId != 0) {
        return textId;
    }

    switch (PARAMS_GET_U(thisx->params, 0, 6)) {
        case 8:
            if (GET_EVENTCHKINF(EVENTCHKINF_30)) {
                return 0x402A;
            }
            break;

        case 6:
            return 0x4020;

        case 7:
            return 0x4021;

        case 0:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402D;
            }
            if (GET_EVENTCHKINF(EVENTCHKINF_30)) {
                return 0x4007;
            }
            break;

        case 1:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402E;
            }

            if (GET_EVENTCHKINF(EVENTCHKINF_30)) {
                return GET_INFTABLE(INFTABLE_124) ? 0x4009 : 0x4008;
            }
            break;

        case 2:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402D;
            }
            if (GET_EVENTCHKINF(EVENTCHKINF_31)) {
                return GET_INFTABLE(INFTABLE_129) ? 0x400B : 0x402F;
            }
            if (GET_EVENTCHKINF(EVENTCHKINF_30)) {
                return 0x400A;
            }
            break;

        case 3:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402E;
            }
            if (GET_EVENTCHKINF(EVENTCHKINF_30)) {
                return 0x400C;
            }
            break;

        case 4:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402D;
            }

            if (GET_EVENTCHKINF(EVENTCHKINF_GAVE_LETTER_TO_KING_ZORA)) {
                return 0x4010;
            }
            if (GET_EVENTCHKINF(EVENTCHKINF_30)) {
                return 0x400F;
            }
            break;

        case 5:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402E;
            }
            if (GET_EVENTCHKINF(EVENTCHKINF_30)) {
                return 0x4011;
            }
            break;
    }
    return 0x4006;
}

s16 zo_end_message(PlayState* play, Actor* thisx) {
    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_DONE:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            return NPC_TALK_STATE_TALKING;

        case TEXT_STATE_CLOSING:
            switch (thisx->textId) {
                case 0x4020:
                case 0x4021:
                    return NPC_TALK_STATE_IDLE;
                case 0x4008:
                    SET_INFTABLE(INFTABLE_124);
                    break;
                case 0x402F:
                    SET_INFTABLE(INFTABLE_129);
                    break;
            }
            SET_EVENTCHKINF(EVENTCHKINF_30);
            return NPC_TALK_STATE_IDLE;

        case TEXT_STATE_CHOICE:
            switch (pad_on_check(play)) {
                case 0:
                    return NPC_TALK_STATE_TALKING;
                default:
                    if (thisx->textId == 0x400C) {
                        thisx->textId = (play->msgCtx.choiceIndex == 0) ? 0x400D : 0x400E;
                        message_set2(play, thisx->textId);
                    }
                    break;
            }
            return NPC_TALK_STATE_TALKING;

        case TEXT_STATE_EVENT:
            switch (pad_on_check(play)) {
                case 0:
                    return NPC_TALK_STATE_TALKING;
                default:
                    return NPC_TALK_STATE_ACTION;
            }
            return NPC_TALK_STATE_TALKING;
    }

    return NPC_TALK_STATE_TALKING;
}

static void eye_paci2(EnZo* this) {
    if (DECR(this->blinkTimer) == 0) {
        this->eyeTexture++;
        if (this->eyeTexture >= 3) {
            this->blinkTimer = get_random_timer(30, 30);
            this->eyeTexture = 0;
        }
    }
}

void zo_sub_action(EnZo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->interactInfo.trackPos = player->actor.world.pos;
    if (this->actionFunc == zo_wait) {
        // Look down at link if child, look up if adult
        this->interactInfo.yOffset = !LINK_IS_ADULT ? 10.0f : -10.0f;
    } else {
        this->interactInfo.trackPos.y = this->actor.world.pos.y;
    }
    eye_moveM(&this->actor, &this->interactInfo, 11, this->trackingMode);
    if (this->canSpeak == true) {
        npc_talk(play, &this->actor, &this->interactInfo.talkState, this->dialogRadius, zo_set_message,
                          zo_end_message);
    }
}

static s32 pl_approach_check(EnZo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f surfacePos;
    f32 yDist;
    f32 hDist;

    surfacePos.x = this->actor.world.pos.x;
    surfacePos.y = this->actor.world.pos.y + this->actor.depthInWater;
    surfacePos.z = this->actor.world.pos.z;

    hDist = search_position_distanceXZ(&surfacePos, &player->actor.world.pos);
    yDist = fabsf(player->actor.world.pos.y - surfacePos.y);

    if (hDist < 240.0f && yDist < 80.0f) {
        return 1;
    }
    return 0;
}

void zo_chg_anime(EnZo* this) {
    s32 animId = ARRAY_COUNT(animetbl);

    if (this->skelAnime.animation == &gZoraHandsOnHipsTappingFootAnim ||
        this->skelAnime.animation == &gZoraOpenArmsAnim) {
        if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
            if (this->actionFunc == zo_wait) {
                animId = ENZO_ANIM_0;
            } else {
                animId = ENZO_ANIM_3;
            }
        }
    }

    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE && this->actor.textId == 0x4006 &&
        this->skelAnime.animation != &gZoraHandsOnHipsTappingFootAnim) {
        animId = ENZO_ANIM_6;
    }

    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE && this->actor.textId == 0x4007 &&
        this->skelAnime.animation != &gZoraOpenArmsAnim) {
        animId = ENZO_ANIM_7;
    }

    if (animId != ARRAY_COUNT(animetbl)) {
        npc_anime_ct(&this->skelAnime, animetbl, animId);
        if (animId == ENZO_ANIM_3) {
            this->skelAnime.curFrame = this->skelAnime.endFrame;
            this->skelAnime.playSpeed = 0.0f;
        }
    }
}

void En_Zo_actor_ct(Actor* thisx, PlayState* play) {
    EnZo* this = (EnZo*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, NULL, 0.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gZoraSkel, NULL, this->jointTable, this->morphTable, 20);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &ZoClPipeData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, NULL, &ZoStatusData);

    if (LINK_IS_ADULT && (PARAMS_GET_U(this->actor.params, 0, 6) == 8)) {
        Actor_delete(&this->actor);
        return;
    }

    npc_anime_ct(&this->skelAnime, animetbl, ENZO_ANIM_2);
    Actor_set_scale(&this->actor, 0.01f);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->dialogRadius = this->collider.dim.radius + 30.0f;
    this->trackingMode = NPC_TRACKING_NONE;
    this->canSpeak = false;
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    Actor_BGcheck2(play, &this->actor, this->collider.dim.height * 0.5f, this->collider.dim.radius, 0.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);

    if (this->actor.depthInWater < 54.0f || PARAMS_GET_U(this->actor.params, 0, 6) == 8) {
        this->actor.shape.shadowDraw = Actor_shadow_circle;
        this->actor.shape.shadowScale = 24.0f;
        npc_anime_ct(&this->skelAnime, animetbl, ENZO_ANIM_1);
        this->canSpeak = true;
        this->alpha = 255.0f;
        this->actionFunc = zo_wait;
    } else {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->actionFunc = zo_wait_deep;
    }
}

void En_Zo_actor_dt(Actor* thisx, PlayState* play) {
}

void zo_wait(EnZo* this, PlayState* play) {
    s16 angle;

    program_wait(play, this->fidgetTableY, this->fidgetTableZ, 20);
    zo_chg_anime(this);
    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        this->trackingMode = NPC_TRACKING_FULL_BODY;
        return;
    }

    angle = ABS((s16)((f32)this->actor.yawTowardsPlayer - (f32)this->actor.shape.rot.y));
    if (angle < 0x4718) {
        if (pl_approach_check(this, play)) {
            this->trackingMode = NPC_TRACKING_HEAD_AND_TORSO;
        } else {
            this->trackingMode = NPC_TRACKING_NONE;
        }
    } else {
        this->trackingMode = NPC_TRACKING_NONE;
    }
}

void zo_wait_deep(EnZo* this, PlayState* play) {
    if (pl_approach_check(this, play)) {
        this->actionFunc = zo_float;
        this->actor.velocity.y = 4.0f;
    }
}

void zo_float(EnZo* this, PlayState* play) {
    if (this->actor.depthInWater < 54.0f) {
        Actor_SE_set(&this->actor, NA_SE_EV_OUT_OF_WATER);
        set_mizu_effect(this);
        npc_anime_ct(&this->skelAnime, animetbl, ENZO_ANIM_3);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actionFunc = zo_wait_surface;
        this->actor.velocity.y = 0.0f;
        this->alpha = 255.0f;
    } else if (this->actor.depthInWater < 80.0f) {
        add_calc2(&this->actor.velocity.y, 2.0f, 0.4f, 0.6f);
        add_calc2(&this->alpha, 255.0f, 0.3f, 10.0f);
    }
}

void zo_wait_surface(EnZo* this, PlayState* play) {
    program_wait(play, this->fidgetTableY, this->fidgetTableZ, 20);
    if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
        this->canSpeak = true;
        this->trackingMode = NPC_TRACKING_FULL_BODY;
        this->skelAnime.playSpeed = 0.0f;
    }
    zo_chg_anime(this);

    add_calc2(&this->actor.velocity.y, this->actor.depthInWater < 54.0f ? -0.6f : 0.6f, 0.3f, 0.2f);
    if (this->rippleTimer != 0) {
        this->rippleTimer--;
        if ((this->rippleTimer == 3) || (this->rippleTimer == 6)) {
            set_hamon_effect(this, 0.2f, 1.0f, 200);
        }
    } else {
        set_hamon_effect(this, 0.2f, 1.0f, 200);
        this->rippleTimer = 12;
    }

    if (pl_approach_check(this, play) != 0) {
        this->timeToDive = get_random_timer(40, 40);
    } else if (DECR(this->timeToDive) == 0) {
        f32 startFrame;
        npc_anime_ct(&this->skelAnime, animetbl, ENZO_ANIM_4);
        this->canSpeak = false;
        this->trackingMode = NPC_TRACKING_NONE;
        this->actionFunc = zo_dive;
        startFrame = this->skelAnime.startFrame;
        this->skelAnime.startFrame = this->skelAnime.endFrame;
        this->skelAnime.curFrame = this->skelAnime.endFrame;
        this->skelAnime.endFrame = startFrame;
        this->skelAnime.playSpeed = -1.0f;
    }
}

void zo_dive(EnZo* this, PlayState* play) {
    if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
        Actor_SE_set(&this->actor, NA_SE_EV_DIVE_WATER);
        set_mizu_effect(this);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.velocity.y = -4.0f;
        this->skelAnime.playSpeed = 0.0f;
    }

    if (this->skelAnime.playSpeed > 0.0f) {
        return;
    }

    if (this->actor.depthInWater > 80.0f || this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        add_calc2(&this->actor.velocity.y, -1.0f, 0.4f, 0.6f);
        add_calc2(&this->alpha, 0.0f, 0.3f, 10.0f);
    }

    if ((s16)this->alpha == 0) {
        npc_anime_ct(&this->skelAnime, animetbl, ENZO_ANIM_2);
        this->actor.world.pos = this->actor.home.pos;
        this->alpha = 0.0f;
        this->actionFunc = zo_wait_deep;
    }
}

void En_Zo_actor_move(Actor* thisx, PlayState* play) {
    EnZo* this = (EnZo*)thisx;
    u32 pad;
    Vec3f pos;

    if ((s32)this->alpha != 0) {
        Skeleton_Info2_anime_play(&this->skelAnime);
        eye_paci2(this);
    }

    Actor_position_moveF(thisx);
    Actor_BGcheck2(play, thisx, this->collider.dim.radius, this->collider.dim.height * 0.25f, 0.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    this->actionFunc(this, play);
    zo_sub_action(this, play);

    // Spawn air bubbles
    if (play->state.frames & 8) {
        pos = this->actor.world.pos;

        pos.y += (fqrand() - 0.5f) * 10.0f + 18.0f;
        pos.x += (fqrand() - 0.5f) * 28.0f;
        pos.z += (fqrand() - 0.5f) * 28.0f;
        zo_eff_bubble_ct(this, &pos);
    }

    if ((s32)this->alpha != 0) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->collider);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }

    zo_eff_hamon_mv(this);
    zo_eff_bubble_mv(this);
    zo_eff_mizu_mv(this);
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnZo* this = (EnZo*)thisx;
    Vec3s limbRot;

    if (limbIndex == 15) {
        Matrix_translate(1800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        limbRot = this->interactInfo.headRot;
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
        Matrix_translate(-1800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == 8) {
        limbRot = this->interactInfo.torsoRot;
        Matrix_rotateX(BINANG_TO_RAD_ALT(-limbRot.y), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
    }

    if ((limbIndex == 8) || (limbIndex == 9) || (limbIndex == 12)) {
        rot->y += sin_s(this->fidgetTableY[limbIndex]) * FIDGET_AMPLITUDE;
        rot->z += cos_s(this->fidgetTableZ[limbIndex]) * FIDGET_AMPLITUDE;
    }

    return 0;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnZo* this = (EnZo*)thisx;
    Vec3f vec = { 0.0f, 600.0f, 0.0f };

    if (limbIndex == 15) {
        Matrix_Position(&vec, &this->actor.focus.pos);
    }
}

void En_Zo_actor_draw(Actor* thisx, PlayState* play) {
    EnZo* this = (EnZo*)thisx;
    void* eyeTextures[] = { gZoraEyeOpenTex, gZoraEyeHalfTex, gZoraEyeClosedTex };

    Matrix_push();
    zo_eff_hamon_dr(this, play);
    zo_eff_bubble_dr(this, play);
    zo_eff_mizu_dr(this, play);
    Matrix_pull();

    if ((s32)this->alpha != 0) {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_zo.c", 1008);

        if (this->alpha == 255.0f) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeTexture]));
            no_clarity(play, &this->skelAnime, before_display, after_display, thisx, this->alpha);
        } else {
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeTexture]));
            clarity(play, &this->skelAnime, before_display, after_display, thisx, this->alpha);
        }

        CLOSE_DISPS(play->state.gfxCtx, "../z_en_zo.c", 1025);
    }
}
