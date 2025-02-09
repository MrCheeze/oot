/*
 * File: z_en_ma1.c
 * Overlay: En_Ma1
 * Description: Child Malon
 */

#include "z_en_ma1.h"
#include "assets/objects/object_ma1/object_ma1.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void En_Ma1_Actor_ct(Actor* thisx, PlayState* play);
void En_Ma1_Actor_dt(Actor* thisx, PlayState* play);
void En_Ma1_Actor_move(Actor* thisx, PlayState* play);
void En_Ma1_Actor_draw(Actor* thisx, PlayState* play);

void ma1_matsu(EnMa1* this, PlayState* play);
void ma1_carry_request(EnMa1* this, PlayState* play);
void ma1_carry_end(EnMa1* this, PlayState* play);
void ma1_wait(EnMa1* this, PlayState* play);
void ma1_sing_start(EnMa1* this, PlayState* play);
void ma1_sing_end(EnMa1* this, PlayState* play);
void ma1_ocarina_play(EnMa1* this, PlayState* play);
void ma1_learn(EnMa1* this, PlayState* play);

ActorProfile En_Ma1_Profile = {
    /**/ ACTOR_EN_MA1,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_MA1,
    /**/ sizeof(EnMa1),
    /**/ En_Ma1_Actor_ct,
    /**/ En_Ma1_Actor_dt,
    /**/ En_Ma1_Actor_move,
    /**/ En_Ma1_Actor_draw,
};

static ColliderCylinderInit EnMaAtInfoData = {
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
    { 18, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 MaStatusData = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum EnMa1Animation {
    /* 0 */ MALON_ANIM_IDLE_NOMORPH,
    /* 1 */ MALON_ANIM_IDLE,
    /* 2 */ MALON_ANIM_SING_NOMORPH,
    /* 3 */ MALON_ANIM_SING
} EnMa1Animation;

static AnimationFrameCountInfo anime_ct_data[] = {
    { &gMalonChildIdleAnim, 1.0f, ANIMMODE_LOOP, 0.0f },
    { &gMalonChildIdleAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gMalonChildSingAnim, 1.0f, ANIMMODE_LOOP, 0.0f },
    { &gMalonChildSingAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
};

u16 ma1_set_message(PlayState* play, Actor* thisx) {
    u16 textId = get_mask_message(play, MASK_REACTION_SET_MALON);

    if (textId != 0) {
        return textId;
    }
    if (CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
        return 0x204A;
    }
    if (GET_EVENTCHKINF(EVENTCHKINF_CAN_LEARN_EPONAS_SONG)) {
        return 0x2049;
    }
    if (GET_EVENTCHKINF(EVENTCHKINF_TALKED_TO_CHILD_MALON_AT_RANCH)) {
        if (GET_INFTABLE(INFTABLE_TOLD_EPONA_IS_SCARED)) {
            return 0x2049;
        } else {
            return 0x2048;
        }
    }
    if (GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
        return 0x2047;
    }
    if (GET_EVENTCHKINF(EVENTCHKINF_RECEIVED_WEIRD_EGG)) {
        return 0x2044;
    }
    if (GET_INFTABLE(INFTABLE_TALKED_TO_MALON_FIRST_TIME)) {
        if (GET_INFTABLE(INFTABLE_MALON_SPAWNED_AT_HYRULE_CASTLE)) {
            return 0x2043;
        } else {
            return 0x2042;
        }
    }
    return 0x2041;
}

s16 ma1_end_message(PlayState* play, Actor* thisx) {
    s16 talkState = NPC_TALK_STATE_TALKING;

    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            switch (thisx->textId) {
                case 0x2041:
                    SET_INFTABLE(INFTABLE_TALKED_TO_MALON_FIRST_TIME);
                    SET_EVENTCHKINF(EVENTCHKINF_TALKED_TO_MALON_FIRST_TIME);
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x2043:
                    talkState = NPC_TALK_STATE_TALKING;
                    break;
                case 0x2047:
                    SET_EVENTCHKINF(EVENTCHKINF_TALKED_TO_CHILD_MALON_AT_RANCH);
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x2048:
                    SET_INFTABLE(INFTABLE_TOLD_EPONA_IS_SCARED);
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x2049:
                    SET_EVENTCHKINF(EVENTCHKINF_CAN_LEARN_EPONAS_SONG);
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x2061:
                    talkState = NPC_TALK_STATE_ACTION;
                    break;
                default:
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
            }
            break;
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            if (pad_on_check(play)) {
                talkState = NPC_TALK_STATE_ACTION;
            }
            break;
        case TEXT_STATE_DONE:
            if (pad_on_check(play)) {
                talkState = NPC_TALK_STATE_ITEM_GIVEN;
            }
            break;
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            talkState = NPC_TALK_STATE_TALKING;
            break;
    }
    return talkState;
}

s32 ma1_appearance_check(EnMa1* this, PlayState* play) {
    if ((this->actor.shape.rot.z == 3) && (z_common_data.sceneLayer == 5)) {
        return true;
    }

    if (!LINK_IS_CHILD) {
        return false;
    }

    if (((play->sceneId == SCENE_MARKET_NIGHT) || (play->sceneId == SCENE_MARKET_DAY)) &&
        !GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE) &&
        !GET_INFTABLE(INFTABLE_MALON_SPAWNED_AT_HYRULE_CASTLE)) {
        return true;
    }

    if ((play->sceneId == SCENE_HYRULE_CASTLE) && !GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
        if (GET_INFTABLE(INFTABLE_MALON_SPAWNED_AT_HYRULE_CASTLE)) {
            return true;
        } else {
            SET_INFTABLE(INFTABLE_MALON_SPAWNED_AT_HYRULE_CASTLE);
            return false;
        }
    }

    if ((play->sceneId == SCENE_LON_LON_BUILDINGS) && IS_NIGHT &&
        GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
        return true;
    }

    if (play->sceneId != SCENE_LON_LON_RANCH) {
        return false;
    }

    if ((this->actor.shape.rot.z == 3) && IS_DAY && GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
        return true;
    }

    return false;
}

void ma1_eye_control(EnMa1* this) {
    if (DECR(this->blinkTimer) == 0) {
        this->eyeIndex++;
        if (this->eyeIndex >= 3) {
            this->blinkTimer = get_random_timer(30, 30);
            this->eyeIndex = 0;
        }
    }
}

void ma1_anime_ct(EnMa1* this, s32 index) {
    f32 frameCount = Si2_anime_end_frame(anime_ct_data[index].animation);

    Skeleton_Info2_init(&this->skelAnime, anime_ct_data[index].animation, 1.0f, 0.0f, frameCount,
                     anime_ct_data[index].mode, anime_ct_data[index].morphFrames);
}

void ma1_eye_move(EnMa1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 trackingMode;

    if ((this->interactInfo.talkState == NPC_TALK_STATE_IDLE) && (this->skelAnime.animation == &gMalonChildSingAnim)) {
        trackingMode = NPC_TRACKING_NONE;
    } else {
        trackingMode = NPC_TRACKING_PLAYER_AUTO_TURN;
    }

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.trackPos.y -= -10.0f;

    eye_moveM(&this->actor, &this->interactInfo, 0, trackingMode);
}

void ma1_bgm_control(EnMa1* this) {
    if (this->skelAnime.animation == &gMalonChildSingAnim) {
        if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
            if (this->singingDisabled) {
                this->singingDisabled = false;
                Na_SetMuteBgm(false);
            }
        } else {
            if (!this->singingDisabled) {
                this->singingDisabled = true;
                Na_SetMuteBgm(true);
            }
        }
    }
}

void En_Ma1_Actor_ct(Actor* thisx, PlayState* play) {
    EnMa1* this = (EnMa1*)thisx;
    s32 pad;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 18.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gMalonChildSkel, NULL, NULL, NULL, 0);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnMaAtInfoData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, CollisionBtlTbl_get(22), &MaStatusData);

    if (!ma1_appearance_check(this, play)) {
        Actor_delete(&this->actor);
        return;
    }

    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    Actor_set_scale(&this->actor, 0.01f);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;

    if (!GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE) || CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
        this->actionFunc = ma1_matsu;
        ma1_anime_ct(this, MALON_ANIM_SING_NOMORPH);
    } else {
        this->actionFunc = ma1_wait;
        ma1_anime_ct(this, MALON_ANIM_SING_NOMORPH);
    }
}

void En_Ma1_Actor_dt(Actor* thisx, PlayState* play) {
    EnMa1* this = (EnMa1*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
    ClObjPipe_dt(play, &this->collider);
}

void ma1_matsu(EnMa1* this, PlayState* play) {
    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        if (this->skelAnime.animation != &gMalonChildIdleAnim) {
            ma1_anime_ct(this, MALON_ANIM_IDLE);
        }
    } else {
        if (this->skelAnime.animation != &gMalonChildSingAnim) {
            ma1_anime_ct(this, MALON_ANIM_SING);
        }
    }

    if ((play->sceneId == SCENE_HYRULE_CASTLE) && GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
        Actor_delete(&this->actor);
    } else if (!GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE) || CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
        if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
            this->actionFunc = ma1_carry_request;
            play->msgCtx.stateTimer = 4;
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        }
    }
}

void ma1_carry_request(EnMa1* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = ma1_carry_end;
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_WEIRD_EGG, 120.0f, 10.0f);
    }
}

void ma1_carry_end(EnMa1* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ITEM_GIVEN) {
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        this->actionFunc = ma1_matsu;
        SET_EVENTCHKINF(EVENTCHKINF_RECEIVED_WEIRD_EGG);
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    }
}

void ma1_wait(EnMa1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        if (this->skelAnime.animation != &gMalonChildIdleAnim) {
            ma1_anime_ct(this, MALON_ANIM_IDLE);
        }
    } else {
        if (this->skelAnime.animation != &gMalonChildSingAnim) {
            ma1_anime_ct(this, MALON_ANIM_SING);
        }
    }

    if (GET_EVENTCHKINF(EVENTCHKINF_CAN_LEARN_EPONAS_SONG)) {
        if (player->stateFlags2 & PLAYER_STATE2_24) {
            player->stateFlags2 |= PLAYER_STATE2_25;
            player->unk_6A8 = &this->actor;
            this->actor.textId = 0x2061;
            message_set(play, this->actor.textId, NULL);
            this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
            this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            this->actionFunc = ma1_sing_start;
        } else if (this->actor.xzDistToPlayer < 30.0f + this->collider.dim.radius) {
            player->stateFlags2 |= PLAYER_STATE2_23;
        }
    }
}

void ma1_sing_start(EnMa1* this, PlayState* play) {
    GET_PLAYER(play)->stateFlags2 |= PLAYER_STATE2_23;
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        Na_SetOcarinaModeFlag(OCARINA_INSTRUMENT_MALON);
        ocarina_set(play, OCARINA_ACTION_TEACH_EPONA);
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = ma1_sing_end;
    }
}

void ma1_sing_end(EnMa1* this, PlayState* play) {
    GET_PLAYER(play)->stateFlags2 |= PLAYER_STATE2_23;
    if (message_check(&play->msgCtx) == TEXT_STATE_SONG_DEMO_DONE) {
        ocarina_set(play, OCARINA_ACTION_PLAYBACK_EPONA);
        this->actionFunc = ma1_ocarina_play;
    }
}

void ma1_ocarina_play(EnMa1* this, PlayState* play) {
    GET_PLAYER(play)->stateFlags2 |= PLAYER_STATE2_23;
    if (play->msgCtx.ocarinaMode == OCARINA_MODE_03) {
        play->nextEntranceIndex = ENTR_LON_LON_RANCH_0;
        z_common_data.nextCutsceneIndex = 0xFFF1;
        play->transitionType = TRANS_TYPE_CIRCLE(TCA_WAVE, TCC_WHITE, TCS_FAST);
        play->transitionTrigger = TRANS_TRIGGER_START;
        this->actionFunc = ma1_learn;
    }
}

void ma1_learn(EnMa1* this, PlayState* play) {
}

void En_Ma1_Actor_move(Actor* thisx, PlayState* play) {
    EnMa1* this = (EnMa1*)thisx;
    s32 pad;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);

    Skeleton_Info2_anime_play(&this->skelAnime);
    ma1_eye_control(this);

    this->actionFunc(this, play);

    if (this->actionFunc != ma1_learn) {
        npc_talk(play, &this->actor, &this->interactInfo.talkState, this->collider.dim.radius + 30.0f,
                          ma1_set_message, ma1_end_message);
    }

    ma1_bgm_control(this);
    ma1_eye_move(this, play);
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnMa1* this = (EnMa1*)thisx;
    Vec3s limbRot;

    if ((limbIndex == CHILD_MALON_LIMB_LEFT_THIGH) || (limbIndex == CHILD_MALON_LIMB_RIGHT_THIGH)) {
        *dList = NULL;
    }
    if (limbIndex == CHILD_MALON_LIMB_HEAD) {
        Matrix_translate(1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        limbRot = this->interactInfo.headRot;
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
        Matrix_translate(-1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limbIndex == CHILD_MALON_LIMB_CHEST) {
        limbRot = this->interactInfo.torsoRot;
        Matrix_rotateX(BINANG_TO_RAD_ALT(-limbRot.y), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD_ALT(-limbRot.x), MTXMODE_APPLY);
    }
    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnMa1* this = (EnMa1*)thisx;
    Vec3f offset = { 800.0f, 0.0f, 0.0f };

    if (limbIndex == CHILD_MALON_LIMB_HEAD) {
        Matrix_Position(&offset, &this->actor.focus.pos);
    }
}

void En_Ma1_Actor_draw(Actor* thisx, PlayState* play) {
    static void* mouth_txt[] = {
        gMalonChildNeutralMouthTex,
        gMalonChildSmilingMouthTex,
        gMalonChildTalkingMouthTex,
    };
    static void* eye_txt[] = {
        gMalonChildEyeOpenTex,
        gMalonChildEyeHalfTex,
        gMalonChildEyeClosedTex,
    };
    EnMa1* this = (EnMa1*)thisx;
    Camera* activeCam;
    f32 distFromCamEye;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ma1.c", 1226);

    activeCam = GET_ACTIVE_CAM(play);
    distFromCamEye = search_position_distanceXZ(&this->actor.world.pos, &activeCam->eye);
    Na_SetObjectBgmDistance(distFromCamEye, NA_BGM_LONLON);
    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouth_txt[this->mouthIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeIndex]));

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ma1.c", 1261);
}
