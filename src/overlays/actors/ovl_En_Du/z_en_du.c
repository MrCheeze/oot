#include "z_en_du.h"
#include "assets/objects/object_du/object_du.h"
#include "assets/scenes/overworld/spot18/spot18_scene.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void En_Du_Actor_ct(Actor* thisx, PlayState* play);
void En_Du_Actor_dt(Actor* thisx, PlayState* play);
void En_Du_Actor_move(Actor* thisx, PlayState* play);
void En_Du_Actor_draw(Actor* thisx, PlayState* play);

void du_wait(EnDu* this, PlayState* play);
void du_ocarina_wait(EnDu* this, PlayState* play);
void du_demo_start(EnDu* this, PlayState* play);
void du_dance_demo(EnDu* this, PlayState* play);
void du_ocarina_play(EnDu* this, PlayState* play);
void du_demo_move(EnDu* this, PlayState* play);
void du_demo_talk(EnDu* this, PlayState* play);
void du_demo_end(EnDu* this, PlayState* play);
void du_carry_request_wait(EnDu* this, PlayState* play);
void du_carry_request(EnDu* this, PlayState* play);
void du_carry_end(EnDu* this, PlayState* play);
void du_dance_demo_end(EnDu* this, PlayState* play);

ActorProfile En_Du_Profile = {
    /**/ ACTOR_EN_DU,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DU,
    /**/ sizeof(EnDu),
    /**/ En_Du_Actor_ct,
    /**/ En_Du_Actor_dt,
    /**/ En_Du_Actor_move,
    /**/ En_Du_Actor_draw,
};

static ColliderCylinderInit EnDuAtInfoData = {
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
    { 20, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 DuStatusData = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum EnDuAnimation {
    /*  0 */ ENDU_ANIM_0,
    /*  1 */ ENDU_ANIM_1,
    /*  2 */ ENDU_ANIM_2,
    /*  3 */ ENDU_ANIM_3,
    /*  4 */ ENDU_ANIM_4,
    /*  5 */ ENDU_ANIM_5,
    /*  6 */ ENDU_ANIM_6,
    /*  7 */ ENDU_ANIM_7,
    /*  8 */ ENDU_ANIM_8,
    /*  9 */ ENDU_ANIM_9,
    /* 10 */ ENDU_ANIM_10,
    /* 11 */ ENDU_ANIM_11,
    /* 12 */ ENDU_ANIM_12,
    /* 13 */ ENDU_ANIM_13,
    /* 14 */ ENDU_ANIM_14
} EnDuAnimation;

static AnimationInfo anime_ct_data[] = {
    { &gDaruniaIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gDaruniaIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaItemGiveAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaItemGiveIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaHitLinkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaHitBreastAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaStandUpAfterFallingAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaDancingLoop1Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -10.0f },
    { &gDaruniaDancingLoop1Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gDaruniaDancingLoop2Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gDaruniaDancingLoop3Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gDaruniaWrongSongAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gDaruniaWrongSongEndAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gDaruniaDancingLoop4Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gDaruniaDancingEndAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -6.0f },
};

void En_Du_actor_set_process(EnDu* this, EnDuActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

u16 du_set_message(PlayState* play, Actor* actor) {
    u16 textId = get_mask_message(play, MASK_REACTION_SET_DARUNIA);

    if (textId != 0) {
        return textId;
    }
    if (CUR_UPG_VALUE(UPG_STRENGTH) != 0) {
        if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
            return 0x301E;
        } else {
            return 0x301D;
        }
    }
    if (GET_INFTABLE(INFTABLE_113)) {
        return 0x301B;
    } else {
        return 0x301A;
    }
}

s16 du_end_message(PlayState* play, Actor* actor) {
    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
            break;
        case TEXT_STATE_CLOSING:
            switch (actor->textId) {
                case 0x301A:
                    SET_INFTABLE(INFTABLE_113);
                    break;
                case 0x301C:
                case 0x301F:
                    return NPC_TALK_STATE_ACTION;
                case 0x3020:
                    SET_EVENTCHKINF(EVENTCHKINF_22);
                    break;
            }
            return NPC_TALK_STATE_IDLE;
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            break;
        case TEXT_STATE_DONE:
            if (pad_on_check(play)) {
                return NPC_TALK_STATE_ITEM_GIVEN;
            }
            break;
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            break;
    }
    return NPC_TALK_STATE_TALKING;
}

s32 du_appearance_check(EnDu* this, PlayState* play) {
    if (play->sceneId == SCENE_GORON_CITY && LINK_IS_CHILD) {
        return 1;
    } else if (play->sceneId == SCENE_FIRE_TEMPLE && !GET_INFTABLE(INFTABLE_11A) && LINK_IS_ADULT) {
        return 1;
    }
    return 0;
}

void du_eye_move(EnDu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 trackingMode = NPC_TRACKING_PLAYER_AUTO_TURN;

    if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
        trackingMode = NPC_TRACKING_NONE;
    }
    if (this->actionFunc == du_dance_demo) {
        trackingMode = NPC_TRACKING_NONE;
    }
    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = 10.0f;
    eye_moveM(&this->actor, &this->interactInfo, 3, trackingMode);
}

void du_eye_control(EnDu* this) {
    if (this->blinkTimer > 0) {
        this->blinkTimer--;
    } else {
        this->blinkTimer = 0;
    }
    if (this->blinkTimer < 3) {
        this->eyeTexIndex = this->blinkTimer;
    }

    switch (this->unk_1EC) {
        case 0:
            if (this->blinkTimer == 0) {
                this->blinkTimer = get_random_timer(30, 30);
            }
            break;
        case 1:
            if (this->blinkTimer == 0) {
                this->eyeTexIndex = 2;
            }
            break;
        case 2:
            if (this->blinkTimer == 0) {
                this->eyeTexIndex = 2;
            }
            break;
        case 3:
            if (this->blinkTimer == 0) {
                this->eyeTexIndex = 0;
            }
            break;
    }

    switch (this->unk_1ED) {
        case 1:
            this->mouthTexIndex = 1;
            break;
        case 2:
            this->mouthTexIndex = 2;
            break;
        case 3:
            this->mouthTexIndex = 3;
            break;
        default:
            this->mouthTexIndex = 0;
            break;
    }

    if (this->unk_1EE == 1) {
        this->noseTexIndex = 1;
    } else {
        this->noseTexIndex = 0;
    }
}

static void demo_start_pos_set(CsCmdActorCue* cue, Vec3f* dst) {
    dst->x = cue->startPos.x;
    dst->y = cue->startPos.y;
    dst->z = cue->startPos.z;
}

static void demo_end_pos_set(CsCmdActorCue* cue, Vec3f* dst) {
    dst->x = cue->endPos.x;
    dst->y = cue->endPos.y;
    dst->z = cue->endPos.z;
}

void du_dai_manzoku_dance(EnDu* this) {
    s32 animationIndices[] = {
        ENDU_ANIM_8, ENDU_ANIM_8, ENDU_ANIM_8, ENDU_ANIM_8, ENDU_ANIM_9, ENDU_ANIM_10, ENDU_ANIM_10, ENDU_ANIM_13,
    };

    if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
        this->unk_1E6++;
        if (this->unk_1E6 >= 8) {
            this->unk_1E6 = 0;
        }
        npc_anime_ct(&this->skelAnime, anime_ct_data, animationIndices[this->unk_1E6]);
    }
}

void du_syo_manzoku_dance(EnDu* this) {
    s32 animationIndices[] = {
        ENDU_ANIM_8,
        ENDU_ANIM_8,
        ENDU_ANIM_11,
        ENDU_ANIM_12,
    };

    if (this->unk_1E6 < 4) {
        if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
            this->unk_1E6++;
            if (this->unk_1E6 < 4) {
                npc_anime_ct(&this->skelAnime, anime_ct_data, animationIndices[this->unk_1E6]);
            }
        }
    }
}

void En_Du_Actor_ct(Actor* thisx, PlayState* play) {
    EnDu* this = (EnDu*)thisx;
    s32 pad;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gDaruniaSkel, NULL, NULL, NULL, 0);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnDuAtInfoData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, CollisionBtlTbl_get(0x16), &DuStatusData);
    if (du_appearance_check(this, play) == 0) {
        Actor_delete(&this->actor);
        return;
    }
    npc_anime_ct(&this->skelAnime, anime_ct_data, ENDU_ANIM_0);
    Actor_set_scale(&this->actor, 0.01f);
    this->actor.attentionRangeType = ATTENTION_RANGE_1;
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;

    if (z_common_data.save.cutsceneIndex >= 0xFFF0) {
        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gGoronCityDaruniaDancingCs);
        z_common_data.cutsceneTrigger = 1;
        En_Du_actor_set_process(this, du_dance_demo);
    } else if (play->sceneId == SCENE_FIRE_TEMPLE) {
        En_Du_actor_set_process(this, du_demo_start);
    } else if (!LINK_IS_ADULT) {
        En_Du_actor_set_process(this, du_ocarina_wait);
    } else {
        En_Du_actor_set_process(this, du_wait);
    }
}

void En_Du_Actor_dt(Actor* thisx, PlayState* play) {
    EnDu* this = (EnDu*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
    ClObjPipe_dt(play, &this->collider);
}

void du_wait(EnDu* this, PlayState* play) {
}

void du_ocarina_wait(EnDu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags2 & PLAYER_STATE2_24) {
        ocarina_set_111(play, OCARINA_ACTION_CHECK_SARIA);
        player->stateFlags2 |= PLAYER_STATE2_25;
        player->unk_6A8 = &this->actor;
        En_Du_actor_set_process(this, du_ocarina_play);
        return;
    }
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    }
    if (this->actor.xzDistToPlayer < 116.0f + this->collider.dim.radius) {
        player->stateFlags2 |= PLAYER_STATE2_23;
    }
}

void du_ocarina_play(EnDu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_00;
        En_Du_actor_set_process(this, du_ocarina_wait);
    } else if (play->msgCtx.ocarinaMode >= OCARINA_MODE_06) {
        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gGoronCityDaruniaWrongSongCs);
        z_common_data.cutsceneTrigger = 1;
        this->unk_1E8 = 1;
        En_Du_actor_set_process(this, du_dance_demo);
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_03) {
        Nai_FxFlagEntry(NA_SE_SY_CORRECT_CHIME, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gGoronCityDaruniaCorrectSongCs);
        z_common_data.cutsceneTrigger = 1;
        this->unk_1E8 = 0;
        En_Du_actor_set_process(this, du_dance_demo);
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
    } else {
        player->stateFlags2 |= PLAYER_STATE2_23;
    }
}

void du_demo_start(EnDu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags1 & PLAYER_STATE1_29) {
        return;
    }

    makeOnepointDemo(play, 3330, -99, &this->actor, CAM_ID_MAIN);
    player->actor.shape.rot.y = player->actor.world.rot.y = this->actor.world.rot.y + 0x7FFF;
    Na_StartFanfare(NA_BGM_APPEAR);
    En_Du_actor_set_process(this, du_demo_move);
    this->unk_1E2 = 0x32;
}

void du_demo_move(EnDu* this, PlayState* play) {
    if (DECR(this->unk_1E2) == 0) {
        this->actor.textId = 0x3039;
        message_set(play, this->actor.textId, NULL);
        this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
        En_Du_actor_set_process(this, du_demo_talk);
    }
}

void du_demo_talk(EnDu* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
        restartCameraStoped(GET_ACTIVE_CAM(play));
        this->unk_1E2 = 0x5A;
        En_Du_actor_set_process(this, du_demo_end);
    }
}

void du_demo_end(EnDu* this, PlayState* play) {
    if (DECR(this->unk_1E2) != 0) {
        switch (this->unk_1E2) {
            case 0x50:
                Actor_SE_set(&this->actor, NA_SE_EV_CHAIN_KEY_UNLOCK_B);
                break;
            case 0x3C:
                Actor_SE_set(&this->actor, NA_SE_EV_SLIDE_DOOR_OPEN);
                break;
            case 0xF:
                Actor_SE_set(&this->actor, NA_SE_EV_SLIDE_DOOR_CLOSE);
                break;
            case 5:
                Actor_SE_set(&this->actor, NA_SE_EV_STONE_BOUND);
                break;
        }
        if (this->unk_1E2 >= 0x3D) {
            this->actor.world.pos.x -= 10.0f;
        }
    } else {
        Actor_delete(&this->actor);
        SET_INFTABLE(INFTABLE_11A);
    }
}

void du_dance_demo(EnDu* this, PlayState* play) {
    f32 frame;
    Vec3f startPos;
    Vec3f endPos;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    CsCmdActorCue* cue;

    if (play->csCtx.state == CS_STATE_IDLE) {
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
        En_Du_actor_set_process(this, du_dance_demo_end);
        return;
    }

    cue = play->csCtx.actorCues[2];

    if (cue != NULL) {
        demo_start_pos_set(cue, &startPos);
        demo_end_pos_set(cue, &endPos);

        if (this->unk_1EA == 0) {
            demo_start_pos_set(cue, &startPos);
            this->actor.world.pos = startPos;
        }

        if (this->unk_1EA != cue->id) {
            if (cue->id == 1) {
                npc_anime_ct(&this->skelAnime, anime_ct_data, ENDU_ANIM_1);
            }

            if (cue->id == 7 || cue->id == 8) {
                this->unk_1E6 = 0;
                npc_anime_ct(&this->skelAnime, anime_ct_data, ENDU_ANIM_7);
            }

            this->unk_1EA = cue->id;

            if (this->unk_1EA == 7) {
                this->blinkTimer = 11;
                this->unk_1EC = 2;
                this->unk_1ED = 2;
                this->unk_1EE = 1;
            }

            if (this->unk_1EA == 8) {
                this->blinkTimer = 11;
                this->unk_1EC = 3;
                this->unk_1ED = 3;
                this->unk_1EE = 0;
            }
        }

        if (this->unk_1EA == 7) {
            du_dai_manzoku_dance(this);
        }

        if (this->unk_1EA == 8) {
            du_syo_manzoku_dance(this);
        }

        this->actor.shape.rot.x = cue->rot.x;
        this->actor.shape.rot.y = cue->rot.y;
        this->actor.shape.rot.z = cue->rot.z;

        this->actor.velocity = velocity;

        if (play->csCtx.curFrame < cue->endFrame) {
            frame = cue->endFrame - cue->startFrame;

            this->actor.velocity.x = (endPos.x - startPos.x) / frame;
            this->actor.velocity.y = (endPos.y - startPos.y) / frame;
            this->actor.velocity.y += this->actor.gravity;

            if (this->actor.velocity.y < this->actor.minVelocityY) {
                this->actor.velocity.y = this->actor.minVelocityY;
            }

            this->actor.velocity.z = (endPos.z - startPos.z) / frame;
        }
    }
}

void du_dance_demo_end(EnDu* this, PlayState* play) {
    this->blinkTimer = 11;
    this->unk_1EC = 0;
    this->unk_1ED = 0;
    this->unk_1EE = 0;

    if (this->unk_1E8 == 1) {
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
        npc_anime_ct(&this->skelAnime, anime_ct_data, ENDU_ANIM_1);
        En_Du_actor_set_process(this, du_ocarina_wait);
        return;
    }
    if (CUR_UPG_VALUE(UPG_STRENGTH) <= 0) {
        this->actor.textId = 0x301C;
        En_Du_actor_set_process(this, du_carry_request_wait);
    } else {
        this->actor.textId = 0x301F;
        En_Du_actor_set_process(this, du_ocarina_wait);
    }
    message_set(play, this->actor.textId, NULL);
    npc_anime_ct(&this->skelAnime, anime_ct_data, ENDU_ANIM_14);
    this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
}

void du_carry_request_wait(EnDu* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
        En_Du_actor_set_process(this, du_carry_request);
        du_carry_request(this, play);
    }
}

void du_carry_request(EnDu* this, PlayState* play) {
    f32 xzRange;

    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        En_Du_actor_set_process(this, du_carry_end);
    } else {
        xzRange = this->actor.xzDistToPlayer + 1.0f;

        Actor_carry_request_set2(&this->actor, play, GI_GORONS_BRACELET, xzRange, fabsf(this->actor.yDistToPlayer) + 1.0f);
    }
}

void du_carry_end(EnDu* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ITEM_GIVEN) {
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        En_Du_actor_set_process(this, du_ocarina_wait);
    }
}

void En_Du_Actor_move(Actor* thisx, PlayState* play) {
    EnDu* this = (EnDu*)thisx;
    s32 pad;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);

    if (this->skelAnime.animation == &gDaruniaDancingEndAnim &&
        Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
        npc_anime_ct(&this->skelAnime, anime_ct_data, ENDU_ANIM_1);
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
    du_eye_control(this);
    du_eye_move(this, play);

    if (this->actionFunc == du_dance_demo) {
        this->actor.world.pos.x += this->actor.velocity.x;
        this->actor.world.pos.y += this->actor.velocity.y;
        this->actor.world.pos.z += this->actor.velocity.z;
    } else {
        Actor_position_move(&this->actor);
    }

    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);

    if (this->actionFunc != du_ocarina_play) {
        npc_talk(play, &this->actor, &this->interactInfo.talkState, this->collider.dim.radius + 116.0f,
                          du_set_message, du_end_message);
    }
    this->actionFunc(this, play);
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnDu* this = (EnDu*)thisx;
    Vec3s limbRot;

    if (limbIndex == 16) {
        Matrix_translate(2400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        limbRot = this->interactInfo.headRot;
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
        Matrix_translate(-2400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limbIndex == 8) {
        limbRot = this->interactInfo.torsoRot;
        Matrix_rotateY(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
    }
    return 0;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnDu* this = (EnDu*)thisx;
    Vec3f D_809FF40C = { 0.0f, -1000.0f, 0.0f };

    if (limbIndex == 16) {
        Matrix_Position(&D_809FF40C, &this->actor.focus.pos);
    }
}

void En_Du_Actor_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = {
        gDaruniaEyeOpenTex,
        gDaruniaEyeOpeningTex,
        gDaruniaEyeShutTex,
        gDaruniaEyeClosingTex,
    };
    static void* mouth_txt[] = {
        gDaruniaMouthSeriousTex,
        gDaruniaMouthGrinningTex,
        gDaruniaMouthOpenTex,
        gDaruniaMouthHappyTex,
    };
    static void* hana_txt[] = {
        gDaruniaNoseSeriousTex,
        gDaruniaNoseHappyTex,
    };
    EnDu* this = (EnDu*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_du.c", 1470);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouth_txt[this->mouthTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(hana_txt[this->noseTexIndex]));

    no_clarity(play, &this->skelAnime, before_display, after_display, &this->actor, 255);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_du.c", 1487);
}
