#include "z_en_ma2.h"
#include "assets/objects/object_ma2/object_ma2.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void En_Ma2_Actor_ct(Actor* thisx, PlayState* play);
void En_Ma2_Actor_dt(Actor* thisx, PlayState* play);
void En_Ma2_Actor_move(Actor* thisx, PlayState* play);
void En_Ma2_Actor_draw(Actor* thisx, PlayState* play);

void ma2_eye_move(EnMa2* this, PlayState* play);
s32 ma2_eye_control_sing(EnMa2* this);
void ma2_eye_control(EnMa2* this);
void ma2_bgm_control(EnMa2* this, PlayState* play);
void ma2_wait(EnMa2* this, PlayState* play);
void ma2_ocarina_wait(EnMa2* this, PlayState* play);
void ma2_ocarina_play(EnMa2* this, PlayState* play);
void ma2_good_play(EnMa2* this, PlayState* play);

ActorProfile En_Ma2_Profile = {
    /**/ ACTOR_EN_MA2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_MA2,
    /**/ sizeof(EnMa2),
    /**/ En_Ma2_Actor_ct,
    /**/ En_Ma2_Actor_dt,
    /**/ En_Ma2_Actor_move,
    /**/ En_Ma2_Actor_draw,
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

typedef enum EnMa2Animation {
    /* 0 */ ENMA2_ANIM_0,
    /* 1 */ ENMA2_ANIM_1,
    /* 2 */ ENMA2_ANIM_2,
    /* 3 */ ENMA2_ANIM_3,
    /* 4 */ ENMA2_ANIM_4
} EnMa2Animation;

static AnimationFrameCountInfo anime_ct_data[] = {
    { &gMalonAdultIdleAnim, 1.0f, ANIMMODE_LOOP, 0.0f },       { &gMalonAdultIdleAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gMalonAdultStandStillAnim, 1.0f, ANIMMODE_LOOP, 0.0f }, { &gMalonAdultSingAnim, 1.0f, ANIMMODE_LOOP, 0.0f },
    { &gMalonAdultSingAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
};

u16 ma2_set_message(PlayState* play, Actor* thisx) {
    u16 textId = get_mask_message(play, MASK_REACTION_SET_MALON);

    if (textId != 0) {
        return textId;
    }
    if (GET_EVENTCHKINF(EVENTCHKINF_EPONA_OBTAINED)) {
        return 0x2056;
    }
    if (IS_NIGHT) {
        if (GET_INFTABLE(INFTABLE_8C)) {
            return 0x2052;
        } else if (GET_INFTABLE(INFTABLE_8E)) {
            return 0x2051;
        } else {
            return 0x2050;
        }
    }
    return 0x204C;
}

s16 ma2_end_message(PlayState* play, Actor* thisx) {
    s16 talkState = NPC_TALK_STATE_TALKING;

    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            switch (thisx->textId) {
                case 0x2051:
                    SET_INFTABLE(INFTABLE_8C);
                    talkState = NPC_TALK_STATE_ACTION;
                    break;
                case 0x2053:
                    SET_INFTABLE(INFTABLE_8D);
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
                default:
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
            }
            break;
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
        case TEXT_STATE_DONE:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_9:
            break;
    }
    return talkState;
}

void ma2_eye_move(EnMa2* this, PlayState* play) {
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

u16 ma2_appearance_check(EnMa2* this, PlayState* play) {
    if (LINK_IS_CHILD) {
        return 0;
    }
    if (!GET_EVENTCHKINF(EVENTCHKINF_EPONA_OBTAINED) && (play->sceneId == SCENE_STABLE) && IS_DAY &&
        (this->actor.shape.rot.z == 5)) {
        return 1;
    }
    if (!GET_EVENTCHKINF(EVENTCHKINF_EPONA_OBTAINED) && (play->sceneId == SCENE_LON_LON_RANCH) && IS_NIGHT &&
        (this->actor.shape.rot.z == 6)) {
        return 2;
    }
    if (!GET_EVENTCHKINF(EVENTCHKINF_EPONA_OBTAINED) || (play->sceneId != SCENE_LON_LON_RANCH)) {
        return 0;
    }
    if ((this->actor.shape.rot.z == 7) && IS_DAY) {
        return 3;
    }
    if ((this->actor.shape.rot.z == 8) && IS_NIGHT) {
        return 3;
    }
    return 0;
}

s32 ma2_eye_control_sing(EnMa2* this) {
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

void ma2_eye_control(EnMa2* this) {
    if ((!ma2_eye_control_sing(this)) && (DECR(this->blinkTimer) == 0)) {
        this->eyeIndex++;
        if (this->eyeIndex >= 3) {
            this->blinkTimer = get_random_timer(30, 30);
            this->eyeIndex = 0;
        }
    }
}

void ma2_anime_ct(EnMa2* this, s32 index) {
    f32 frameCount = Si2_anime_end_frame(anime_ct_data[index].animation);

    Skeleton_Info2_init(&this->skelAnime, anime_ct_data[index].animation, 1.0f, 0.0f, frameCount,
                     anime_ct_data[index].mode, anime_ct_data[index].morphFrames);
}

void ma2_bgm_control(EnMa2* this, PlayState* play) {
    if (this->skelAnime.animation == &gMalonAdultSingAnim) {
        if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
            if (this->isNotSinging) {
                // Turn on singing
                Na_SetMuteBgm(false);
                this->isNotSinging = false;
            }
        } else {
            if (!this->isNotSinging) {
                // Turn off singing
                Na_SetMuteBgm(true);
                this->isNotSinging = true;
            }
        }
    }
}

void En_Ma2_Actor_ct(Actor* thisx, PlayState* play) {
    EnMa2* this = (EnMa2*)thisx;
    s32 pad;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 18.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gMalonAdultSkel, NULL, NULL, NULL, 0);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnMaAtInfoData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, CollisionBtlTbl_get(22), &MaStatusData);

    switch (ma2_appearance_check(this, play)) {
        case 1:
            ma2_anime_ct(this, ENMA2_ANIM_2);
            this->actionFunc = ma2_wait;
            break;
        case 2:
            ma2_anime_ct(this, ENMA2_ANIM_3);
            this->actionFunc = ma2_ocarina_wait;
            break;
        case 3:
            if (GET_INFTABLE(INFTABLE_8D)) {
                ma2_anime_ct(this, ENMA2_ANIM_0);
            } else {
                ma2_anime_ct(this, ENMA2_ANIM_3);
            }
            this->actionFunc = ma2_wait;
            break;
        case 0:
            Actor_delete(&this->actor);
            return;
    }

    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    Actor_set_scale(&this->actor, 0.01f);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
}

void En_Ma2_Actor_dt(Actor* thisx, PlayState* play) {
    EnMa2* this = (EnMa2*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
    ClObjPipe_dt(play, &this->collider);
}

void ma2_wait(EnMa2* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    }
}

void ma2_ocarina_wait(EnMa2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags2 & PLAYER_STATE2_24) {
        player->stateFlags2 |= PLAYER_STATE2_25;
        player->unk_6A8 = &this->actor;
        ocarina_set(play, OCARINA_ACTION_CHECK_EPONA);
        this->actionFunc = ma2_ocarina_play;
    } else if (this->actor.xzDistToPlayer < 30.0f + this->collider.dim.radius) {
        player->stateFlags2 |= PLAYER_STATE2_23;
    }
}

void ma2_ocarina_play(EnMa2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->msgCtx.ocarinaMode >= OCARINA_MODE_04) {
        this->actionFunc = ma2_ocarina_wait;
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_03) {
        Nai_FxFlagEntry(NA_SE_SY_CORRECT_CHIME, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        this->unk_208 = 0x1E;
        SET_INFTABLE(INFTABLE_8E);
        this->actionFunc = ma2_good_play;
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
    } else {
        player->stateFlags2 |= PLAYER_STATE2_23;
    }
}

void ma2_good_play(EnMa2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (DECR(this->unk_208)) {
        player->stateFlags2 |= PLAYER_STATE2_23;
    } else {
        if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
            this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            message_close(play);
        } else {
            this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            this->actionFunc = ma2_wait;
        }
    }
}

void En_Ma2_Actor_move(Actor* thisx, PlayState* play) {
    EnMa2* this = (EnMa2*)thisx;
    s32 pad;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Skeleton_Info2_anime_play(&this->skelAnime);
    ma2_eye_control(this);
    this->actionFunc(this, play);
    ma2_bgm_control(this, play);
    ma2_eye_move(this, play);
    if (this->actionFunc != ma2_ocarina_play) {
        npc_talk(play, &this->actor, &this->interactInfo.talkState, this->collider.dim.radius + 30.0f,
                          ma2_set_message, ma2_end_message);
    }
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnMa2* this = (EnMa2*)thisx;
    Vec3s limbRot;

    if ((limbIndex == MALON_ADULT_LEFT_THIGH_LIMB) || (limbIndex == MALON_ADULT_RIGHT_THIGH_LIMB)) {
        *dList = NULL;
    }
    if (limbIndex == MALON_ADULT_HEAD_LIMB) {
        Matrix_translate(1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        limbRot = this->interactInfo.headRot;
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
        Matrix_translate(-1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limbIndex == MALON_ADULT_CHEST_AND_NECK_LIMB) {
        limbRot = this->interactInfo.torsoRot;
        Matrix_rotateY(BINANG_TO_RAD_ALT(-limbRot.y), MTXMODE_APPLY);
        Matrix_rotateX(BINANG_TO_RAD_ALT(-limbRot.x), MTXMODE_APPLY);
    }
    if ((limbIndex == MALON_ADULT_CHEST_AND_NECK_LIMB) || (limbIndex == MALON_ADULT_LEFT_SHOULDER_LIMB) ||
        (limbIndex == MALON_ADULT_RIGHT_SHOULDER_LIMB)) {
        rot->y += sin_s(this->unk_212[limbIndex].y) * 200.0f;
        rot->z += cos_s(this->unk_212[limbIndex].z) * 200.0f;
    }
    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnMa2* this = (EnMa2*)thisx;
    Vec3f vec = { 900.0f, 0.0f, 0.0f };

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ma2.c", 904);

    if (limbIndex == MALON_ADULT_HEAD_LIMB) {
        Matrix_Position(&vec, &this->actor.focus.pos);
    }
    if ((limbIndex == MALON_ADULT_LEFT_HAND_LIMB) && (this->skelAnime.animation == &gMalonAdultStandStillAnim)) {
        gSPDisplayList(POLY_OPA_DISP++, gMalonAdultBasketDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ma2.c", 927);
}

void En_Ma2_Actor_draw(Actor* thisx, PlayState* play) {
    static void* mouth_txt[] = { gMalonAdultMouthNeutralTex, gMalonAdultMouthSadTex, gMalonAdultMouthHappyTex };
    static void* eye_txt[] = { gMalonAdultEyeOpenTex, gMalonAdultEyeHalfTex, gMalonAdultEyeClosedTex };

    EnMa2* this = (EnMa2*)thisx;
    Camera* activeCam;
    f32 distFromCamEye;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ma2.c", 955);

    activeCam = GET_ACTIVE_CAM(play);
    distFromCamEye = search_position_distanceXZ(&this->actor.world.pos, &activeCam->eye);
    Na_SetObjectBgmDistance(distFromCamEye, NA_BGM_LONLON);
    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouth_txt[this->mouthIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeIndex]));

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ma2.c", 990);
}
