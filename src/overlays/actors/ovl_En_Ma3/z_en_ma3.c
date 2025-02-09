/*
 * File: z_en_ma3.c
 * Overlay: En_Ma3
 * Description: Adult Malon (Ranch)
 */

#include "z_en_ma3.h"
#include "assets/objects/object_ma2/object_ma2.h"
#include "versions.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Ma3_Actor_ct(Actor* thisx, PlayState* play);
void En_Ma3_Actor_dt(Actor* thisx, PlayState* play);
void En_Ma3_Actor_move(Actor* thisx, PlayState* play);
void En_Ma3_Actor_draw(Actor* thisx, PlayState* play);

void ma3_eye_move(EnMa3* this, PlayState* play);
s32 ma3_appearance_check(EnMa3* this, PlayState* play);
s32 ma3_eye_control_sing(EnMa3* this);
void ma3_eye_control(EnMa3* this);
void ma3_wait(EnMa3* this, PlayState* play);

ActorProfile En_Ma3_Profile = {
    /**/ ACTOR_EN_MA3,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_MA2,
    /**/ sizeof(EnMa3),
    /**/ En_Ma3_Actor_ct,
    /**/ En_Ma3_Actor_dt,
    /**/ En_Ma3_Actor_move,
    /**/ En_Ma3_Actor_draw,
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

typedef enum EnMa3Animation {
    /* 0 */ ENMA3_ANIM_0,
    /* 1 */ ENMA3_ANIM_1,
    /* 2 */ ENMA3_ANIM_2,
    /* 3 */ ENMA3_ANIM_3,
    /* 4 */ ENMA3_ANIM_4
} EnMa3Animation;

static AnimationFrameCountInfo anime_ct_data[] = {
    { &gMalonAdultIdleAnim, 1.0f, ANIMMODE_LOOP, 0.0f },       { &gMalonAdultIdleAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gMalonAdultStandStillAnim, 1.0f, ANIMMODE_LOOP, 0.0f }, { &gMalonAdultSingAnim, 1.0f, ANIMMODE_LOOP, 0.0f },
    { &gMalonAdultSingAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
};

u16 ma3_set_message(PlayState* play, Actor* thisx) {
    Player* player = GET_PLAYER(play);

    if (!GET_INFTABLE(INFTABLE_B8)) {
        return 0x2000;
    }

    if (GET_EVENTINF(EVENTINF_HORSES_0A)) {
        z_common_data.timerSeconds = z_common_data.timerSeconds;
        thisx->flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;

        if (((void)0, z_common_data.timerSeconds) > 210) {
            return 0x208E;
        }

        if ((HIGH_SCORE(HS_HORSE_RACE) == 0) || (HIGH_SCORE(HS_HORSE_RACE) >= 180)) {
            HIGH_SCORE(HS_HORSE_RACE) = 180;
        }

        if (!GET_EVENTCHKINF(EVENTCHKINF_HORSE_RACE_COW_UNLOCK) && (((void)0, z_common_data.timerSeconds) < 50)) {
            return 0x208F;
        }

        if (HIGH_SCORE(HS_HORSE_RACE) > ((void)0, z_common_data.timerSeconds)) {
            return 0x2012;
        }

        return 0x2004;
    }

    if (!(player->stateFlags1 & PLAYER_STATE1_23) &&
        (ActorSearch(play, thisx, ACTOR_EN_HORSE, 1, 1200.0f) == NULL)) {
        return 0x2001;
    }

    if (!GET_INFTABLE(INFTABLE_B9)) {
        return 0x2002;
    }

    return 0x2003;
}

s16 ma3_end_message(PlayState* play, Actor* thisx) {
    s16 talkState = NPC_TALK_STATE_TALKING;

    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_EVENT:
            if (pad_on_check(play)) {
                play->nextEntranceIndex = ENTR_LON_LON_RANCH_0;
                z_common_data.nextCutsceneIndex = 0xFFF0;
                play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_BLACK, TCS_FAST);
                play->transitionTrigger = TRANS_TRIGGER_START;
                SET_EVENTINF(EVENTINF_HORSES_0A);
                z_common_data.timerState = TIMER_STATE_UP_FREEZE;
            }
            break;
        case TEXT_STATE_CHOICE:
            if (pad_on_check(play)) {
                SET_INFTABLE(INFTABLE_B9);
                if (play->msgCtx.choiceIndex == 0) {
                    if (GET_EVENTCHKINF(EVENTCHKINF_HORSE_RACE_COW_UNLOCK)) {
                        message_set2(play, 0x2091);
                    } else if (HIGH_SCORE(HS_HORSE_RACE) == 0) {
                        message_set2(play, 0x2092);
                    } else {
                        message_set2(play, 0x2090);
                    }
                }
            }
            break;
        case TEXT_STATE_CLOSING:
            switch (thisx->textId) {
                case 0x2000:
                    SET_INFTABLE(INFTABLE_B8);
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x208F:
                    SET_EVENTCHKINF(EVENTCHKINF_HORSE_RACE_COW_UNLOCK);
                    FALLTHROUGH;
                case 0x2004:
                case 0x2012:
                    if (HIGH_SCORE(HS_HORSE_RACE) > z_common_data.timerSeconds) {
                        HIGH_SCORE(HS_HORSE_RACE) = z_common_data.timerSeconds;
                    }
                    FALLTHROUGH;
                case 0x208E:
                    CLEAR_EVENTINF(EVENTINF_HORSES_0A);
                    thisx->flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                    talkState = NPC_TALK_STATE_IDLE;
                    z_common_data.timerState = TIMER_STATE_STOP;
                    break;
                case 0x2002:
                    SET_INFTABLE(INFTABLE_B9);
                    FALLTHROUGH;
                case 0x2003:
                    if (!GET_EVENTINF(EVENTINF_HORSES_0A)) {
                        talkState = NPC_TALK_STATE_IDLE;
                    }
                    break;
                default:
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
            }
            break;
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_DONE:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            break;
    }
    return talkState;
}

void ma3_eye_move(EnMa3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 trackingMode;

    if ((this->interactInfo.talkState == NPC_TALK_STATE_IDLE) && (this->skelAnime.animation == &gMalonAdultSingAnim)) {
        trackingMode = NPC_TRACKING_NONE;
    } else {
        trackingMode = NPC_TRACKING_PLAYER_AUTO_TURN;
    }

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = 0.0f;
    eye_moveM(&this->actor, &this->interactInfo, 0, trackingMode);
}

s32 ma3_appearance_check(EnMa3* this, PlayState* play) {
    if (LINK_IS_CHILD) {
        return 2;
    }
    if (!GET_EVENTCHKINF(EVENTCHKINF_EPONA_OBTAINED)) {
        return 2;
    }
    if (GET_EVENTINF(EVENTINF_HORSES_0A)) {
        return 1;
    }
    return 0;
}

s32 ma3_eye_control_sing(EnMa3* this) {
    if (this->skelAnime.animation != &gMalonAdultSingAnim) {
        return 0;
    }
    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        return 0;
    }
    this->blinkTimer = 0;
    if (this->eyeIndex != 2) {
        return 0;
    }
    this->mouthIndex = 2;
    return 1;
}

void ma3_eye_control(EnMa3* this) {
    if ((!ma3_eye_control_sing(this)) && (DECR(this->blinkTimer) == 0)) {
        this->eyeIndex++;
        if (this->eyeIndex >= 3) {
            this->blinkTimer = get_random_timer(30, 30);
            this->eyeIndex = 0;
        }
    }
}

void ma3_anime_ct(EnMa3* this, s32 index) {
    f32 frameCount = Si2_anime_end_frame(anime_ct_data[index].animation);

    Skeleton_Info2_init(&this->skelAnime, anime_ct_data[index].animation, 1.0f, 0.0f, frameCount,
                     anime_ct_data[index].mode, anime_ct_data[index].morphFrames);
}

void En_Ma3_Actor_ct(Actor* thisx, PlayState* play) {
    EnMa3* this = (EnMa3*)thisx;
    s32 pad;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 18.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gMalonAdultSkel, NULL, NULL, NULL, 0);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnMaAtInfoData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, CollisionBtlTbl_get(22), &MaStatusData);

    switch (ma3_appearance_check(this, play)) {
        case 0:
            ma3_anime_ct(this, ENMA3_ANIM_0);
            this->actionFunc = ma3_wait;
            break;
        case 1:
            ma3_anime_ct(this, ENMA3_ANIM_0);
            this->actionFunc = ma3_wait;
            break;
        case 2:
            Actor_delete(&this->actor);
            return;
    }

    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    Actor_set_scale(&this->actor, 0.01f);
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
}

void En_Ma3_Actor_dt(Actor* thisx, PlayState* play) {
    EnMa3* this = (EnMa3*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
    ClObjPipe_dt(play, &this->collider);
}

void ma3_wait(EnMa3* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    }
}

#if OOT_PAL_N64
// Same as npc_talk, but with an additional check for the subtimer state
s32 En_Ma3_Actor_moveTalking(PlayState* play, Actor* actor, s16* talkState, f32 interactRange, NpcGetTextIdFunc getTextId,
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

    if ((z_common_data.subTimerState != 0) && (z_common_data.subTimerSeconds < 6)) {
        return false;
    }

    if (!Actor_talk_request2(actor, play, interactRange)) {
        return false;
    }

    actor->textId = getTextId(play, actor);

    return false;
}
#endif

void En_Ma3_Actor_move(Actor* thisx, PlayState* play) {
    EnMa3* this = (EnMa3*)thisx;
    s32 pad;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Skeleton_Info2_anime_play(&this->skelAnime);
    ma3_eye_control(this);
    this->actionFunc(this, play);
    ma3_eye_move(this, play);

#if !OOT_PAL_N64
    npc_talk(play, &this->actor, &this->interactInfo.talkState, this->collider.dim.radius + 150.0f,
                      ma3_set_message, ma3_end_message);
#else
    En_Ma3_Actor_moveTalking(play, &this->actor, &this->interactInfo.talkState, this->collider.dim.radius + 150.0f,
                        ma3_set_message, ma3_end_message);
#endif

#if OOT_VERSION >= PAL_1_0
    if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
        if (this->isNotSinging) {
            // Turn on singing
            Na_SetMuteBgm(false);
            this->isNotSinging = false;
        }
    } else if (!this->isNotSinging) {
        // Turn off singing
        Na_SetMuteBgm(true);
        this->isNotSinging = true;
    }
#endif
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnMa3* this = (EnMa3*)thisx;
    Vec3s limbRot;

    if ((limbIndex == MALON_ADULT_LIMB_LEFT_THIGH) || (limbIndex == MALON_ADULT_LIMB_RIGHT_THIGH)) {
        *dList = NULL;
    }
    if (limbIndex == MALON_ADULT_LIMB_HEAD) {
        Matrix_translate(1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        limbRot = this->interactInfo.headRot;
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
        Matrix_translate(-1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limbIndex == MALON_ADULT_LIMB_CHEST_AND_NECK) {
        limbRot = this->interactInfo.torsoRot;
        Matrix_rotateY(BINANG_TO_RAD_ALT(-limbRot.y), MTXMODE_APPLY);
        Matrix_rotateX(BINANG_TO_RAD_ALT(-limbRot.x), MTXMODE_APPLY);
    }
    if ((limbIndex == MALON_ADULT_LIMB_CHEST_AND_NECK) || (limbIndex == MALON_ADULT_LIMB_LEFT_SHOULDER) ||
        (limbIndex == MALON_ADULT_LIMB_RIGHT_SHOULDER)) {
        rot->y += sin_s(this->unk_212[limbIndex].y) * 200.0f;
        rot->z += cos_s(this->unk_212[limbIndex].z) * 200.0f;
    }
    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnMa3* this = (EnMa3*)thisx;
    Vec3f vec = { 900.0f, 0.0f, 0.0f };

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ma3.c", 927);

    if (limbIndex == MALON_ADULT_LIMB_HEAD) {
        Matrix_Position(&vec, &this->actor.focus.pos);
    }

    if ((limbIndex == MALON_ADULT_LIMB_LEFT_HAND) && (this->skelAnime.animation == &gMalonAdultStandStillAnim)) {
        gSPDisplayList(POLY_OPA_DISP++, gMalonAdultBasketDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ma3.c", 950);
}

void En_Ma3_Actor_draw(Actor* thisx, PlayState* play) {
    static void* mouth_txt[] = { gMalonAdultMouthNeutralTex, gMalonAdultMouthSadTex, gMalonAdultMouthHappyTex };
    static void* eye_txt[] = { gMalonAdultEyeOpenTex, gMalonAdultEyeHalfTex, gMalonAdultEyeClosedTex };
    EnMa3* this = (EnMa3*)thisx;
    Camera* activeCam;
    f32 distFromCamEye;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ma3.c", 978);

    activeCam = GET_ACTIVE_CAM(play);
    distFromCamEye = search_position_distanceXZ(&this->actor.world.pos, &activeCam->eye);
    Na_SetObjectBgmDistance(distFromCamEye, NA_BGM_LONLON);
    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouth_txt[this->mouthIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeIndex]));

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ma3.c", 1013);
}
