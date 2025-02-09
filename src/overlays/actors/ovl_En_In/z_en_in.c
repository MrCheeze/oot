#include "z_en_in.h"
#include "versions.h"
#include "overlays/actors/ovl_En_Horse/z_en_horse.h"
#include "assets/objects/object_in/object_in.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_In_Actor_ct(Actor* thisx, PlayState* play);
void En_In_Actor_dt(Actor* thisx, PlayState* play);
void En_In_Actor_move(Actor* thisx, PlayState* play);
void En_In_Actor_draw(Actor* thisx, PlayState* play);

void in_init(EnIn* this, PlayState* play);
void in_race(EnIn* this, PlayState* play);
void in_event_wait(EnIn* this, PlayState* play);
void in_event(EnIn* this, PlayState* play);
void in_return_match(EnIn* this, PlayState* play);
void close_gate_demo(EnIn* this, PlayState* play);
void in_stopper(EnIn* this, PlayState* play);
void scene_change(EnIn* this, PlayState* play);
void in_time_up(EnIn* this, PlayState* play);
void close_gate_demo_ed(EnIn* this, PlayState* play);
void in_event_win(EnIn* this, PlayState* play);
void in_event_lose(EnIn* this, PlayState* play);
void close_gate_demo_ct(EnIn* this, PlayState* play);
void in_wait(EnIn* this, PlayState* play);

ActorProfile En_In_Profile = {
    /**/ ACTOR_EN_IN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_IN,
    /**/ sizeof(EnIn),
    /**/ En_In_Actor_ct,
    /**/ En_In_Actor_dt,
    /**/ En_In_Actor_move,
    /**/ En_In_Actor_draw,
};

static ColliderCylinderInit EnInAtInfoData = {
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

static CollisionCheckInfoInit2 InStatusData = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum EnInAnimation {
    /* 0 */ ENIN_ANIM_0,
    /* 1 */ ENIN_ANIM_1,
    /* 2 */ ENIN_ANIM_2,
    /* 3 */ ENIN_ANIM_3,
    /* 4 */ ENIN_ANIM_4,
    /* 5 */ ENIN_ANIM_5,
    /* 6 */ ENIN_ANIM_6,
    /* 7 */ ENIN_ANIM_7,
    /* 8 */ ENIN_ANIM_8,
    /* 9 */ ENIN_ANIM_9
} EnInAnimation;

typedef enum EnInStartMode {
    /* 0 */ ENIN_START_MODE_0,
    /* 1 */ ENIN_START_MODE_1,
    /* 2 */ ENIN_START_MODE_2,
    /* 3 */ ENIN_START_MODE_3,
    /* 4 */ ENIN_START_MODE_4
} EnInStartMode;

static AnimationFrameCountInfo anime_ct_data[] = {
    { &object_in_Anim_001CC0, 1.0f, ANIMMODE_LOOP, 0.0f }, { &object_in_Anim_001CC0, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &object_in_Anim_013C6C, 1.0f, ANIMMODE_LOOP, 0.0f }, { &object_in_Anim_013C6C, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &object_in_Anim_000CB0, 1.0f, ANIMMODE_LOOP, 0.0f }, { &object_in_Anim_0003B4, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &object_in_Anim_001BE0, 1.0f, ANIMMODE_LOOP, 0.0f }, { &object_in_Anim_013D60, 1.0f, ANIMMODE_LOOP, 0.0f },
    { &object_in_Anim_01431C, 1.0f, ANIMMODE_LOOP, 0.0f }, { &object_in_Anim_014CA8, 1.0f, ANIMMODE_LOOP, 0.0f },
};

static AnimationHeader* raceanim[] = {
    &object_in_Anim_0151C8, &object_in_Anim_015DF0, &object_in_Anim_016B3C, &object_in_Anim_015814,
    &object_in_Anim_01646C, &object_in_Anim_0175D0, &object_in_Anim_017B58, &object_in_Anim_018C38,
};

static Gfx* parts[] = {
    NULL,
    NULL,
    gIngoAdultEraLeftThighDL,
    gIngoAdultEraLeftLegDL,
    gIngoAdultEraLeftFootDL,
    gIngoAdultEraRightThighDL,
    gIngoAdultEraRightLegDL,
    gIngoAdultEraRightFootDL,
    gIngoAdultEraTorsoDL,
    gIngoAdultEraChestDL,
    gIngoAdultEraLeftShoulderDL,
    gIngoAdultEraLeftArmDL,
    gIngoAdultEraLeftHandDL,
    gIngoAdultEraRightShoulderDL,
    gIngoAdultEraRightArmDL,
    gIngoAdultEraRightHandDL,
    gIngoAdultEraHeadDL,
    gIngoAdultEraLeftEyebrowDL,
    gIngoAdultEraRightEyebrowDL,
    gIngoAdultEraMustacheDL,
};

u16 in_set_message_ch(PlayState* play) {
    if (GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
        if (GET_INFTABLE(INFTABLE_97)) {
            return 0x2046;
        } else {
            return 0x2045;
        }
    }
    if (GET_INFTABLE(INFTABLE_94)) {
        return 0x2040;
    } else {
        return 0x203F;
    }
}

u16 in_set_message_ad(PlayState* play) {
    Player* player = GET_PLAYER(play);
    u16 textId = get_mask_message(play, MASK_REACTION_SET_INGO);

    if (textId != 0) {
        return textId;
    }
    if (GET_EVENTCHKINF(EVENTCHKINF_EPONA_OBTAINED)) {
        if (IS_DAY) {
            return 0x205F;
        } else {
            return 0x2057;
        }
    }
    if (IS_NIGHT) {
        return 0x204E;
    }
    switch (GET_EVENTINF_INGO_RACE_STATE()) {
        case INGO_RACE_STATE_HORSE_RENTAL_PERIOD:
            if (!(player->stateFlags1 & PLAYER_STATE1_23)) {
                return 0x2036;
            } else if (GET_EVENTCHKINF(EVENTCHKINF_1B)) {
                if (GET_INFTABLE(INFTABLE_A2)) {
                    return 0x2036;
                } else {
                    return 0x2038;
                }
            } else {
                return 0x2037;
            }
        case INGO_RACE_STATE_PLAYER_LOSE:
            if (GET_EVENTINF(EVENTINF_INGO_RACE_SECOND_RACE) || GET_EVENTINF(EVENTINF_INGO_RACE_LOST_ONCE)) {
                return 0x203E;
            } else {
                return 0x203D;
            }
        case INGO_RACE_STATE_FIRST_WIN:
            return 0x203A;
        case INGO_RACE_STATE_TRAPPED_WIN_UNUSED:
        case INGO_RACE_STATE_TRAPPED_WIN_EPONA:
            return 0x203C;
        case INGO_RACE_STATE_REMATCH:
            // Ask to race again after losing race
            return 0x205B;
        case INGO_RACE_STATE_RACING:
        default:
            if (GET_INFTABLE(INFTABLE_9A)) {
                // Pay to Ride
                return 0x2031;
            } else {
                // Adult Ingo first dialog, branches to pay to ride
                return 0x2030;
            }
    }
}

u16 in_set_message(PlayState* play, Actor* thisx) {
    u16 textId = get_mask_message(play, MASK_REACTION_SET_INGO);

    if (textId != 0) {
        return textId;
    }
    if (!LINK_IS_ADULT) {
        return in_set_message_ch(play);
    } else {
        return in_set_message_ad(play);
    }
}

s16 in_end_message_002(PlayState* play, Actor* thisx) {
    s32 talkState = NPC_TALK_STATE_IDLE;

    switch (thisx->textId) {
        case 0x2045:
            SET_INFTABLE(INFTABLE_97);
            break;
        case 0x203E:
            talkState = NPC_TALK_STATE_ACTION;
            break;
        case 0x203F:
            SET_EVENTCHKINF(EVENTCHKINF_11);
            SET_INFTABLE(INFTABLE_94);
            break;
    }
    return talkState;
}

s16 in_end_message_004(PlayState* play, Actor* thisx) {
    EnIn* this = (EnIn*)thisx;
    s32 talkState = NPC_TALK_STATE_TALKING;

    switch (this->actor.textId) {
        case 0x2030:
        case 0x2031:
            if (play->msgCtx.choiceIndex == 1) {
                this->actor.textId = 0x2032;
            } else if (z_common_data.save.info.playerData.rupees < 10) {
                this->actor.textId = 0x2033;
            } else {
                this->actor.textId = 0x2034;
            }
            message_set2(play, this->actor.textId);
            SET_INFTABLE(INFTABLE_9A);
            break;
        case 0x2034:
            if (play->msgCtx.choiceIndex == 1) {
                lupy_increase(-10);
                this->actor.textId = 0x205C;
            } else {
                this->actor.textId = 0x2035;
            }
            message_set2(play, this->actor.textId);
            break;
        case 0x2036:
        case 0x2037:
            if (play->msgCtx.choiceIndex == 1) {
                talkState = NPC_TALK_STATE_ACTION;
            } else {
                this->actor.textId = 0x201F;
                message_set2(play, this->actor.textId);
            }
            break;
        case 0x2038:
            if (play->msgCtx.choiceIndex == 0 && z_common_data.save.info.playerData.rupees >= 50) {
                talkState = NPC_TALK_STATE_ACTION;
            } else {
                this->actor.textId = 0x2039;
                message_set2(play, this->actor.textId);
                SET_INFTABLE(INFTABLE_A2);
            }
            break;
        case 0x205B:
            if (play->msgCtx.choiceIndex == 0 && z_common_data.save.info.playerData.rupees >= 50) {
                talkState = NPC_TALK_STATE_ACTION;
            } else {
                message_set2(play, this->actor.textId = 0x2039);
                SET_EVENTINF_INGO_RACE_STATE(INGO_RACE_STATE_OFFER_RENTAL);
                CLEAR_EVENTINF(EVENTINF_INGO_RACE_LOST_ONCE);
                CLEAR_EVENTINF(EVENTINF_INGO_RACE_SECOND_RACE);
                this->actionFunc = in_event_wait;
            }
            break;
    }
    if (!z_common_data.save.info.playerData.rupees) {}

    return talkState;
}

s16 in_end_message_005(PlayState* play, Actor* thisx) {
    s32 talkState = NPC_TALK_STATE_TALKING;

    if (thisx->textId == 0x2035) {
        lupy_increase(-10);
        thisx->textId = 0x205C;
        message_set2(play, thisx->textId);
    } else {
        talkState = NPC_TALK_STATE_ACTION;
    }
    return talkState;
}

s16 in_end_message(PlayState* play, Actor* thisx) {
    s16 talkState = NPC_TALK_STATE_TALKING;

    PRINTF("message_check->(%d[%x])\n", message_check(&play->msgCtx), thisx->textId);
    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
            break;
        case TEXT_STATE_CLOSING:
            talkState = in_end_message_002(play, thisx);
            break;
        case TEXT_STATE_DONE_FADING:
            break;
        case TEXT_STATE_CHOICE:
            if (pad_on_check(play)) {
                talkState = in_end_message_004(play, thisx);
            }
            break;
        case TEXT_STATE_EVENT:
            if (pad_on_check(play)) {
                talkState = in_end_message_005(play, thisx);
            }
            break;
        case TEXT_STATE_DONE:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            break;
    }
    return talkState;
}

void in_eye_move(EnIn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 trackingMode;

    if (this->skelAnime.animation == &object_in_Anim_0003B4 || this->skelAnime.animation == &object_in_Anim_001BE0 ||
        this->skelAnime.animation == &object_in_Anim_013D60) {
        trackingMode = NPC_TRACKING_NONE;
    } else {
        trackingMode = NPC_TRACKING_PLAYER_AUTO_TURN;
    }
    if (this->actionFunc == in_event) {
        trackingMode = NPC_TRACKING_FULL_BODY;
    }
    if (this->actionFunc == in_time_up) {
        this->interactInfo.trackPos = play->view.eye;
        this->interactInfo.yOffset = 60.0f;
    } else {
        this->interactInfo.trackPos = player->actor.world.pos;
        this->interactInfo.yOffset = 16.0f;
    }
    eye_moveM(&this->actor, &this->interactInfo, 1, trackingMode);
}

void movement_by_animation(SkelAnime* skelAnime, EnIn* this, PlayState* play) {
    if (skelAnime->baseTransl.y < skelAnime->jointTable[0].y) {
        skelAnime->movementFlags |= ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y;
        Skeleton_Proc_Anime_Move_init(play, &this->actor, skelAnime, 1.0f);
    }
}

void in_anime_ct(EnIn* this, s32 index) {
    Skeleton_Info2_init(&this->skelAnime, anime_ct_data[index].animation, 1.0f, 0.0f,
                     Si2_anime_end_frame(anime_ct_data[index].animation), anime_ct_data[index].mode,
                     anime_ct_data[index].morphFrames);
}

s32 in_race_check(EnIn* this, PlayState* play) {
    if (this->actor.params != 1 || this->actor.shape.rot.z != 1 || !LINK_IS_ADULT) {
        return 0;
    }
    this->animationIdx = 1;
    this->collider.base.ocFlags1 &= ~OC1_ON;
    Skeleton_Info2_init(&this->skelAnime, raceanim[this->animationIdx], 1.0f, 0.0f,
                     Si2_anime_end_frame(raceanim[this->animationIdx]), 2, 0.0f);
    this->actionFunc = in_race;
    return 1;
}

s32 in_appearance_check(EnIn* this, PlayState* play) {
    if (play->sceneId == SCENE_LON_LON_RANCH && LINK_IS_CHILD && IS_DAY && this->actor.shape.rot.z == 1 &&
        !GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
        return ENIN_START_MODE_1;
    }
    if (play->sceneId == SCENE_STABLE && LINK_IS_CHILD && IS_DAY && this->actor.shape.rot.z == 3 &&
        GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
        return ENIN_START_MODE_1;
    }
    if (play->sceneId == SCENE_STABLE && LINK_IS_CHILD && IS_NIGHT) {
        if ((this->actor.shape.rot.z == 2) && !GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
            return ENIN_START_MODE_1;
        }
        if ((this->actor.shape.rot.z == 4) && GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
            return ENIN_START_MODE_1;
        }
    }
    if (play->sceneId == SCENE_LON_LON_RANCH && LINK_IS_ADULT && IS_DAY) {
        if ((this->actor.shape.rot.z == 5) && !GET_EVENTCHKINF(EVENTCHKINF_EPONA_OBTAINED)) {
            return ENIN_START_MODE_2;
        }
        if ((this->actor.shape.rot.z == 7) && GET_EVENTCHKINF(EVENTCHKINF_EPONA_OBTAINED)) {
            return ENIN_START_MODE_4;
        }
    }
    if (play->sceneId == SCENE_LON_LON_BUILDINGS && LINK_IS_ADULT && IS_NIGHT) {
        if (this->actor.shape.rot.z == 6 && !GET_EVENTCHKINF(EVENTCHKINF_EPONA_OBTAINED)) {
            return ENIN_START_MODE_3;
        }
        if (this->actor.shape.rot.z == 8 && GET_EVENTCHKINF(EVENTCHKINF_EPONA_OBTAINED)) {
            return ENIN_START_MODE_3;
        }
    }
    return ENIN_START_MODE_0;
}

void in_eye_control(EnIn* this) {
    if (this->eyeIndex != 3) {
        if (DECR(this->blinkTimer) == 0) {
            this->eyeIndex++;
            if (this->eyeIndex >= 3) {
                this->blinkTimer = get_random_timer(30, 30);
                this->eyeIndex = 0;
            }
        }
    }
}

void in_program_wait(EnIn* this, PlayState* play) {
    s32 i;
    u32 f = 0;

    if (this->skelAnime.animation != &object_in_Anim_014CA8) {
        f = play->gameplayFrames;
    }
    for (i = 0; i < ARRAY_COUNT(this->unk_330); i++) {
        this->unk_330[i].y = (2068 + 50 * i) * f;
        this->unk_330[i].z = (2368 + 50 * i) * f;
    }
}

void event_scene_change(EnIn* this, PlayState* play, s32 index, u32 transitionType) {
    s16 entrances[] = { ENTR_LON_LON_RANCH_8, ENTR_LON_LON_RANCH_6, ENTR_LON_LON_RANCH_0 };

    play->nextEntranceIndex = entrances[index];
    if (index == 2) {
        z_common_data.nextCutsceneIndex = 0xFFF0;
    }
    play->transitionType = transitionType;
    play->transitionTrigger = TRANS_TRIGGER_START;
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
    alpha_change(HUD_VISIBILITY_NOTHING);
    if (index == 0) {
        R_EXITED_SCENE_RIDING_HORSE = false;
    }
    z_common_data.timerState = TIMER_STATE_OFF;
}

void in_demo_ct(EnIn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f subCamAt;
    Vec3f subCamEye;
    Vec3s zeroVec = { 0, 0, 0 };

    this->subCamId = Gama_play_make_camera(play);
    Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
    Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
    subCamAt.x = this->actor.world.pos.x;
    subCamAt.y = this->actor.world.pos.y + 60.0f;
    subCamAt.z = this->actor.world.pos.z;
    subCamEye.x = subCamAt.x;
    subCamEye.y = subCamAt.y - 22.0f;
    subCamEye.z = subCamAt.z + 40.0f;
    Gama_play_camera_setting(play, this->subCamId, &subCamAt, &subCamEye);
    this->actor.shape.rot.y = search_position_angleY(&this->actor.world.pos, &subCamEye);
    this->interactInfo.headRot = zeroVec;
    this->interactInfo.torsoRot = zeroVec;
    message_set(play, 0x2025, NULL);
    this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
    player->actor.world.pos = this->actor.world.pos;
    player->actor.world.pos.x += 100.0f * sin_s(this->actor.shape.rot.y);
    player->actor.world.pos.z += 100.0f * cos_s(this->actor.shape.rot.y);
    if (player->rideActor != NULL) {
        player->rideActor->world.pos = player->actor.world.pos;
        player->rideActor->freezeTimer = 10;
    }
    player->actor.freezeTimer = 10;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    shrink_window_setval(32);
    alpha_change(HUD_VISIBILITY_NOTHING_ALT);
}

static s32 first_flag = 0;

void En_In_Actor_ct(Actor* thisx, PlayState* play) {
    EnIn* this = (EnIn*)thisx;
    RespawnData* respawn = &z_common_data.respawn[RESPAWN_MODE_DOWN];
    Vec3f respawnPos;

    this->requiredObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_IN);
    if (this->requiredObjectSlot < 0 && this->actor.params > 0) {
        this->actionFunc = NULL;
        Actor_delete(&this->actor);
        return;
    }
    respawnPos = respawn->pos;
    // hardcoded coords for lon lon entrance
    if (first_flag == 0 && respawnPos.x == 1107.0f && respawnPos.y == 0.0f && respawnPos.z == -3740.0f) {
        z_common_data.eventInf[EVENTINF_INDEX_HORSES] = 0;
        first_flag = 1;
    }
    this->actionFunc = in_init;
}

void En_In_Actor_dt(Actor* thisx, PlayState* play) {
    EnIn* this = (EnIn*)thisx;

    if (this->actionFunc != NULL && this->actionFunc != in_init) {
        ClObjPipe_dt(play, &this->collider);
    }
}

// This function does not actually wait since it waits for OBJECT_IN,
// but the object is already loaded at this point from being set in the ActorProfile data
void in_init(EnIn* this, PlayState* play) {
    s32 staySpawned = false;

    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot) || this->actor.params <= 0) {
        Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 36.0f);
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gIngoSkel, NULL, this->jointTable, this->morphTable, 20);
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, &this->actor, &EnInAtInfoData);
        CollisionCheck_Status_set3(&this->actor.colChkInfo, NULL, &InStatusData);
        if (in_race_check(this, play)) {
            WRITE_EVENTINF_INGO_RACE_0F(0);
            return;
        }
        Actor_set_scale(&this->actor, 0.01f);
        this->actor.attentionRangeType = ATTENTION_RANGE_6;
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        this->actionFunc = in_wait;

        switch (in_appearance_check(this, play)) {
            case ENIN_START_MODE_1:
                in_anime_ct(this, ENIN_ANIM_9);
                this->actionFunc = in_wait;
                break;
            case ENIN_START_MODE_3:
                in_anime_ct(this, ENIN_ANIM_7);
                this->actionFunc = in_wait;
                if (!GET_EVENTCHKINF(EVENTCHKINF_EPONA_OBTAINED)) {
                    this->actor.params = 5;
                }
                break;
            case ENIN_START_MODE_4:
                in_anime_ct(this, ENIN_ANIM_8);
                this->eyeIndex = 3;
                this->actionFunc = in_wait;
                break;
            case ENIN_START_MODE_0:
                Actor_delete(&this->actor);
                break;
            default: // ENIN_START_MODE_2
                switch (GET_EVENTINF_INGO_RACE_STATE()) {
                    case INGO_RACE_STATE_OFFER_RENTAL:
                    case INGO_RACE_STATE_RACING:
                    case INGO_RACE_STATE_PLAYER_LOSE:
                    case INGO_RACE_STATE_FIRST_WIN:
                    case INGO_RACE_STATE_REMATCH:
                        if (this->actor.params == 2) {
                            staySpawned = true;
                        }
                        break;
                    case INGO_RACE_STATE_HORSE_RENTAL_PERIOD:
                        if (this->actor.params == 3) {
                            staySpawned = true;
                        }
                        break;
                    case INGO_RACE_STATE_TRAPPED_WIN_UNUSED:
                    case INGO_RACE_STATE_TRAPPED_WIN_EPONA:
                        if (this->actor.params == 4) {
                            staySpawned = true;
                        }
                        break;
                }
                if (staySpawned != true) {
                    Actor_delete(&this->actor);
                    return;
                }
                switch (GET_EVENTINF_INGO_RACE_STATE()) {
                    case INGO_RACE_STATE_OFFER_RENTAL:
                    case INGO_RACE_STATE_RACING:
                        in_anime_ct(this, ENIN_ANIM_2);
                        this->actionFunc = in_event_wait;
                        z_common_data.eventInf[EVENTINF_INDEX_HORSES] = 0;
                        break;
                    case INGO_RACE_STATE_HORSE_RENTAL_PERIOD:
                        this->actor.attentionRangeType = ATTENTION_RANGE_3;
                        in_anime_ct(this, ENIN_ANIM_2);
                        this->actionFunc = in_event;
                        event_timer_set(60);
                        break;
                    case INGO_RACE_STATE_PLAYER_LOSE:
                        in_anime_ct(this, ENIN_ANIM_4);
                        this->actionFunc = in_event_win;
                        break;
                    case INGO_RACE_STATE_FIRST_WIN:
                        in_anime_ct(this, ENIN_ANIM_6);
                        this->unk_1EC = 8;
                        this->actionFunc = in_event_lose;
                        break;
                    case INGO_RACE_STATE_TRAPPED_WIN_UNUSED:
                    case INGO_RACE_STATE_TRAPPED_WIN_EPONA:
                        this->actor.attentionRangeType = ATTENTION_RANGE_3;
                        in_anime_ct(this, ENIN_ANIM_6);
                        this->unk_1EC = 8;
                        this->actionFunc = close_gate_demo_ct;
                        break;
                    case INGO_RACE_STATE_REMATCH:
                        in_anime_ct(this, ENIN_ANIM_2);
                        this->actionFunc = in_return_match;
                        break;
                }
        }
    }
}

void in_race(EnIn* this, PlayState* play) {
    if (this->skelAnime.animation == &object_in_Anim_015814 || this->skelAnime.animation == &object_in_Anim_01646C) {
        if (this->skelAnime.curFrame == 8.0f) {
            Na_SetRandomSeFlag(&this->actor.projectedPos, NA_SE_VO_IN_LASH_0,
                                NA_SE_VO_IN_LASH_1 - NA_SE_VO_IN_LASH_0 + 1);
        }
    }
    if (this->skelAnime.animation == &object_in_Anim_018C38 && this->skelAnime.curFrame == 20.0f) {
        Actor_SE_set(&this->actor, NA_SE_VO_IN_CRY_0);
    }
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->animationIdx %= 8;
        this->unk_1E8 = this->animationIdx;
        if (this->animationIdx == 3 || this->animationIdx == 4) {
            Nai_FxFlagEntry(NA_SE_IT_LASH, &this->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            if (fqrand() < 0.3f) {
                Nai_FxFlagEntry(NA_SE_IT_INGO_HORSE_NEIGH, &this->actor.projectedPos, 4,
                                     &_dummy_one, &_dummy_one, &_dummy_zero_s8);
            }
        }
        Skeleton_Info2_init(&this->skelAnime, raceanim[this->animationIdx], 1.0f, 0.0f,
                         Si2_anime_end_frame(raceanim[this->animationIdx]), 2, -10.0f);
    }
}

void in_wait(EnIn* this, PlayState* play) {
}

void in_event_wait(EnIn* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        event_scene_change(this, play, 1, TRANS_TYPE_CIRCLE(TCA_NORMAL, TCC_BLACK, TCS_FAST));
        SET_EVENTINF_INGO_RACE_STATE(INGO_RACE_STATE_HORSE_RENTAL_PERIOD);
        WRITE_EVENTINF_INGO_RACE_0F(1);
        CLEAR_INFTABLE(INFTABLE_A2);
        Nottori_Bgm(NA_BGM_HORSE);
        play->msgCtx.stateTimer = 0;
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    }
}

void in_event(EnIn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 phi_a2;
    s32 transitionType;

    if (!GET_EVENTCHKINF(EVENTCHKINF_1B) && (player->stateFlags1 & PLAYER_STATE1_23)) {
        SET_INFTABLE(INFTABLE_AB);
    }
    if (z_common_data.timerState == TIMER_STATE_STOP) {
        Nai_FxFlagEntry(NA_SE_SY_FOUND, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        in_demo_ct(this, play);
        this->actionFunc = in_time_up;
        z_common_data.timerState = TIMER_STATE_OFF;
    } else if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        if (play->msgCtx.choiceIndex == 0) {
            if (z_common_data.save.info.playerData.rupees < 50) {
                play->msgCtx.stateTimer = 4;
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
                return;
            }
            WRITE_EVENTINF_INGO_RACE_HORSETYPE(((EnHorse*)GET_PLAYER(play)->rideActor)->type);
            SET_EVENTINF_INGO_RACE_STATE(INGO_RACE_STATE_RACING);
            phi_a2 = 2;
            transitionType = TRANS_TYPE_FADE_BLACK;
        } else {
            Nai_FxFlagEntry(NA_SE_SY_FOUND, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            if (!GET_EVENTCHKINF(EVENTCHKINF_1B)) {
                if (GET_INFTABLE(INFTABLE_AB)) {
                    SET_EVENTCHKINF(EVENTCHKINF_1B);
                    SET_INFTABLE(INFTABLE_AB);
                }
            }
            SET_EVENTINF_INGO_RACE_STATE(INGO_RACE_STATE_OFFER_RENTAL);
            phi_a2 = 0;
            transitionType = TRANS_TYPE_CIRCLE(TCA_NORMAL, TCC_BLACK, TCS_FAST);
        }
        event_scene_change(this, play, phi_a2, transitionType);
        play->msgCtx.stateTimer = 0;
        WRITE_EVENTINF_INGO_RACE_0F(1);
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    }
}

void in_event_win(EnIn* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        lupy_increase(-50);
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        in_anime_ct(this, ENIN_ANIM_3);
        this->actionFunc = in_return_match;
        SET_EVENTINF_INGO_RACE_STATE(INGO_RACE_STATE_REMATCH);
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        SET_EVENTINF_INGO_RACE_FLAG(EVENTINF_INGO_RACE_LOST_ONCE);
        if (!GET_EVENTINF(EVENTINF_INGO_RACE_SECOND_RACE)) {
            play->msgCtx.stateTimer = 4;
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        }
    }
}

void in_return_match(EnIn* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        if ((play->msgCtx.choiceIndex == 0 && z_common_data.save.info.playerData.rupees < 50) ||
            play->msgCtx.choiceIndex == 1) {
            SET_EVENTINF_INGO_RACE_STATE(INGO_RACE_STATE_OFFER_RENTAL);
            this->actionFunc = in_event_wait;
        } else {
            event_scene_change(this, play, 2, TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_BLACK, TCS_FAST));
            SET_EVENTINF_INGO_RACE_STATE(INGO_RACE_STATE_RACING);
            WRITE_EVENTINF_INGO_RACE_0F(1);
            play->msgCtx.stateTimer = 0;
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        }
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        CLEAR_EVENTINF(EVENTINF_INGO_RACE_LOST_ONCE);
        CLEAR_EVENTINF(EVENTINF_INGO_RACE_SECOND_RACE);
    }
}

void in_event_lose(EnIn* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        return;
    }
    if (this->unk_1EC != 0) {
        this->unk_1EC--;
        if (this->unk_1EC == 0) {
            Actor_SE_set(&this->actor, NA_SE_VO_IN_LOST);
        }
    }
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        event_scene_change(this, play, 2, TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_BLACK, TCS_FAST));
        SET_EVENTINF_INGO_RACE_STATE(INGO_RACE_STATE_RACING);
        WRITE_EVENTINF_INGO_RACE_0F(1);
        play->msgCtx.stateTimer = 0;
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        SET_EVENTINF_INGO_RACE_FLAG(EVENTINF_INGO_RACE_SECOND_RACE);
    }
}

void close_gate_demo_ct(EnIn* this, PlayState* play) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f subCamAt;
    Vec3f subCamEye;

    this->subCamId = Gama_play_make_camera(play);
    this->returnToCamId = play->activeCamId;
    Gama_play_set_camera_status(play, this->returnToCamId, CAM_STAT_WAIT);
    Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);

    this->subCamAtOffset.x = 0.0f;
    this->subCamAtOffset.y = 50.0f;
    this->subCamAtOffset.z = 0.0f;
    this->subCamEyeOffset.x = 0.0f;
    this->subCamEyeOffset.y = 50.0f;
    this->subCamEyeOffset.z = 50.0f;

    subCamAt = this->actor.world.pos;
    subCamEye = this->actor.world.pos;

    subCamAt.x += this->subCamAtOffset.x;
    subCamAt.y += this->subCamAtOffset.y;
    subCamAt.z += this->subCamAtOffset.z;

    subCamEye.x += this->subCamEyeOffset.x;
    subCamEye.y += this->subCamEyeOffset.y;
    subCamEye.z += this->subCamEyeOffset.z;

    Gama_play_camera_setting(play, this->subCamId, &subCamAt, &subCamEye);
    this->actor.textId = 0x203B;
    message_set(play, this->actor.textId, NULL);
    this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
    this->unk_1FC = 0;
    play->csCtx.curFrame = 0;
    shrink_window_setval(32);
    alpha_change(HUD_VISIBILITY_NOTHING_ALT);
    this->actionFunc = close_gate_demo;
}

void close_gate_demo(EnIn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f subCamAt;
    Vec3f subCamEye;

    if (player->rideActor != NULL) {
        player->rideActor->freezeTimer = 10;
    }
    player->actor.freezeTimer = 10;
    if (this->actor.textId == 0x203B) {
        if (this->unk_1EC != 0) {
            this->unk_1EC--;
            if (this->unk_1EC == 0) {
                Actor_SE_set(&this->actor, NA_SE_VO_IN_LOST);
            }
        }
    }
    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
            if (this->actor.textId == 0x203B) {
                this->actor.textId = 0x203C;
                message_set(play, this->actor.textId, NULL);
                this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
                in_anime_ct(this, ENIN_ANIM_3);
            } else {
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
            }
        }
    } else {
        if (play->csCtx.curFrame++ >= 50) {
            this->actionFunc = close_gate_demo_ed;
            return;
        }
        if (play->csCtx.curFrame == 44) {
            Actor_SE_set(&this->actor, NA_SE_EV_RONRON_DOOR_CLOSE);
        }
        add_calc(&this->subCamAtOffset.x, 0.0f, 0.06f, 10000.0f, 0.0f);
        add_calc(&this->subCamAtOffset.y, 50.0f, 0.06f, 10000.0f, 0.0f);
        add_calc(&this->subCamAtOffset.z, 0.0f, 0.06f, 10000.0f, 0.0f);
        add_calc(&this->subCamEyeOffset.x, 0.0f, 0.06f, 10000.0f, 0.0f);
        add_calc(&this->subCamEyeOffset.y, 150.0f, 0.06f, 10000.0f, 0.0f);
        add_calc(&this->subCamEyeOffset.z, 300.0f, 0.06f, 10000.0f, 0.0f);

        subCamAt = this->actor.world.pos;
        subCamEye = this->actor.world.pos;

        subCamAt.x += this->subCamAtOffset.x;
        subCamAt.y += this->subCamAtOffset.y;
        subCamAt.z += this->subCamAtOffset.z;
        subCamEye.x += this->subCamEyeOffset.x;
        subCamEye.y += this->subCamEyeOffset.y;
        subCamEye.z += this->subCamEyeOffset.z;
        Gama_play_camera_setting(play, this->subCamId, &subCamAt, &subCamEye);
    }
}

void close_gate_demo_ed(EnIn* this, PlayState* play) {
    Gama_play_set_camera_status(play, this->returnToCamId, CAM_STAT_ACTIVE);
    Gama_play_clear_camera(play, this->subCamId);
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
    alpha_change(HUD_VISIBILITY_ALL);
    this->actionFunc = in_stopper;
}

void in_stopper(EnIn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 yaw;
    Vec3f pos = this->actor.world.pos;

    pos.x += 90.0f * sin_s(this->actor.shape.rot.y);
    pos.z += 90.0f * cos_s(this->actor.shape.rot.y);
    yaw = search_position_angleY(&pos, &player->actor.world.pos);
    if (ABS(yaw) > 0x4000) {
        play->nextEntranceIndex = ENTR_HYRULE_FIELD_15;
        play->transitionTrigger = TRANS_TRIGGER_START;
        play->transitionType = TRANS_TYPE_FADE_WHITE_FAST;
        this->actionFunc = scene_change;
    } else if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        play->msgCtx.stateTimer = 4;
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    }
}

void scene_change(EnIn* this, PlayState* play) {
}

void in_time_up(EnIn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->rideActor != NULL) {
        player->rideActor->freezeTimer = 10;
    }
    player->actor.freezeTimer = 10;

    if (this->interactInfo.talkState != NPC_TALK_STATE_ACTION) {
        return;
    }

    if (!GET_EVENTCHKINF(EVENTCHKINF_1B) && GET_INFTABLE(INFTABLE_AB)) {
        SET_EVENTCHKINF(EVENTCHKINF_1B);
        SET_INFTABLE(INFTABLE_AB);
    }

    event_scene_change(this, play, 0, TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_BLACK, TCS_FAST));
    SET_EVENTINF_INGO_RACE_STATE(INGO_RACE_STATE_OFFER_RENTAL);
    WRITE_EVENTINF_INGO_RACE_0F(1);
    play->msgCtx.stateTimer = 4;
    play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
}

void En_In_Actor_move(Actor* thisx, PlayState* play) {
    ColliderCylinder* collider;
    EnIn* this = (EnIn*)thisx;

    if (this->actionFunc == in_init) {
        this->actionFunc(this, play);
        return;
    }
    collider = &this->collider;
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &collider->base);
    if (this->actionFunc != in_race) {
        Skeleton_Info2_anime_play(&this->skelAnime);
        if (this->skelAnime.animation == &object_in_Anim_001BE0 &&
            GET_EVENTINF_INGO_RACE_STATE() != INGO_RACE_STATE_TRAPPED_WIN_EPONA) {
            movement_by_animation(&this->skelAnime, this, play);
        }
        Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    }
    in_eye_control(this);
    this->actionFunc(this, play);
    if (this->actionFunc != in_race) {
        in_program_wait(this, play);
#if OOT_VERSION < PAL_1_0
        npc_talk(play, &this->actor, &this->interactInfo.talkState,
                          ((this->actor.attentionRangeType == 6) ? 80.0f : 320.0f) + this->collider.dim.radius,
                          in_set_message, in_end_message);
        if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
            this->unk_1FA = this->unk_1F8;
            this->unk_1F8 = message_check(&play->msgCtx);
        }
#else
        if ((z_common_data.subTimerSeconds < 6) && (z_common_data.subTimerState != SUBTIMER_STATE_OFF) &&
            this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
            if (Actor_talk_check(&this->actor, play)) {}
        } else {
            npc_talk(play, &this->actor, &this->interactInfo.talkState,
                              ((this->actor.attentionRangeType == 6) ? 80.0f : 320.0f) + this->collider.dim.radius,
                              in_set_message, in_end_message);
            if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
                this->unk_1FA = this->unk_1F8;
                this->unk_1F8 = message_check(&play->msgCtx);
            }
        }
#endif
        in_eye_move(this, play);
    }
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnIn* this = (EnIn*)thisx;
    Vec3s limbRot;

    if (this->actor.params > 0 && limbIndex != INGO_HEAD_LIMB) {
        if (parts[limbIndex] != NULL) {
            *dList = parts[limbIndex];
        }
    }
    if (limbIndex == INGO_HEAD_LIMB) {
        Matrix_translate(1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        limbRot = this->interactInfo.headRot;
        Matrix_rotateZ(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_translate(-1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limbIndex == INGO_CHEST_LIMB) {
        limbRot = this->interactInfo.torsoRot;
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
        Matrix_rotateY(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
    }
    if (limbIndex == INGO_CHEST_LIMB || limbIndex == INGO_LEFT_SHOULDER_LIMB || limbIndex == INGO_RIGHT_SHOULDER_LIMB) {
        rot->y += sin_s(this->unk_330[limbIndex].y) * 200.0f;
        rot->z += cos_s(this->unk_330[limbIndex].z) * 200.0f;
    }
    return 0;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnIn* this = (EnIn*)thisx;
    Vec3f D_80A7B9A8 = { 1600.0, 0.0f, 0.0f };

    OPEN_DISPS(play->state.gfxCtx, "../z_en_in.c", 2335);

    if (limbIndex == INGO_HEAD_LIMB) {
        Matrix_Position(&D_80A7B9A8, &this->actor.focus.pos);
        this->actor.focus.rot = this->actor.world.rot;
    }
    if (limbIndex == INGO_LEFT_HAND_LIMB && this->skelAnime.animation == &object_in_Anim_014CA8) {
        gSPDisplayList(POLY_OPA_DISP++, gIngoChildEraBasketDL);
    }
    if (limbIndex == INGO_RIGHT_HAND_LIMB && this->skelAnime.animation == &object_in_Anim_014CA8) {
        gSPDisplayList(POLY_OPA_DISP++, gIngoChildEraPitchForkDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_in.c", 2365);
}

void En_In_Actor_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = { gIngoEyeOpenTex, gIngoEyeHalfTex, gIngoEyeClosedTex, gIngoEyeClosed2Tex };

    EnIn* this = (EnIn*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_in.c", 2384);
    if (this->actionFunc != in_init) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeIndex]));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gIngoHeadGradient2Tex));
        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              before_display, after_display, &this->actor);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_in.c", 2416);
}
