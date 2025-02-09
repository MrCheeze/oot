/*
 * File: z_en_zl4.c
 * Overlay: ovl_En_Zl4
 * Description: Child Princess Zelda
 */

#include "z_en_zl4.h"
#include "assets/objects/object_zl4/object_zl4.h"
#include "assets/scenes/indoors/nakaniwa/nakaniwa_scene.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

typedef enum EnZl4CutsceneState {
    /* 0 */ ZL4_CS_WAIT,
    /* 1 */ ZL4_CS_START,
    /* 2 */ ZL4_CS_MEET,
    /* 3 */ ZL4_CS_STONE,
    /* 4 */ ZL4_CS_NAMES,
    /* 5 */ ZL4_CS_LEGEND,
    /* 6 */ ZL4_CS_WINDOW,
    /* 7 */ ZL4_CS_GANON,
    /* 8 */ ZL4_CS_PLAN
} EnZl4CutsceneState;

typedef enum EnZl4EyeExpression {
    /* 0 */ ZL4_EYES_NEUTRAL,
    /* 1 */ ZL4_EYES_SHUT,
    /* 2 */ ZL4_EYES_LOOK_LEFT,
    /* 3 */ ZL4_EYES_LOOK_RIGHT,
    /* 4 */ ZL4_EYES_WIDE,
    /* 5 */ ZL4_EYES_SQUINT,
    /* 6 */ ZL4_EYES_OPEN
} EnZl4EyeExpression;

typedef enum EnZl4MouthExpression {
    /* 0 */ ZL4_MOUTH_NEUTRAL,
    /* 1 */ ZL4_MOUTH_HAPPY,
    /* 2 */ ZL4_MOUTH_WORRIED,
    /* 3 */ ZL4_MOUTH_SURPRISED
} EnZl4MouthExpression;

typedef enum EnZl4EyeState {
    /* 0 */ ZL4_EYE_OPEN,
    /* 1 */ ZL4_EYE_BLINK,
    /* 2 */ ZL4_EYE_SHUT,
    /* 3 */ ZL4_EYE_WIDE,
    /* 4 */ ZL4_EYE_SQUINT,
    /* 5 */ ZL4_EYE_LOOK_OUT,
    /* 6 */ ZL4_EYE_LOOK_IN
} EnZl4EyeState;

void En_Zl4_Actor_ct(Actor* thisx, PlayState* play);
void En_Zl4_Actor_dt(Actor* thisx, PlayState* play);
void En_Zl4_Actor_move(Actor* thisx, PlayState* play);
void En_Zl4_Actor_draw(Actor* thisx, PlayState* play);

void zl4_normal_wait(EnZl4* this, PlayState* play);
void zl4_wait(EnZl4* this, PlayState* play);
void zl4_demo(EnZl4* this, PlayState* play);

ActorProfile En_Zl4_Profile = {
    /**/ ACTOR_EN_ZL4,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ZL4,
    /**/ sizeof(EnZl4),
    /**/ En_Zl4_Actor_ct,
    /**/ En_Zl4_Actor_dt,
    /**/ En_Zl4_Actor_move,
    /**/ En_Zl4_Actor_draw,
};

static ColliderCylinderInit Zl4ClInfoData = {
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
    { 10, 44, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 Zl4StatusData = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum EnZl4Animation {
    /*  0 */ ZL4_ANIM_0,
    /*  1 */ ZL4_ANIM_1,
    /*  2 */ ZL4_ANIM_2,
    /*  3 */ ZL4_ANIM_3,
    /*  4 */ ZL4_ANIM_4,
    /*  5 */ ZL4_ANIM_5,
    /*  6 */ ZL4_ANIM_6,
    /*  7 */ ZL4_ANIM_7,
    /*  8 */ ZL4_ANIM_8,
    /*  9 */ ZL4_ANIM_9,
    /* 10 */ ZL4_ANIM_10,
    /* 11 */ ZL4_ANIM_11,
    /* 12 */ ZL4_ANIM_12,
    /* 13 */ ZL4_ANIM_13,
    /* 14 */ ZL4_ANIM_14,
    /* 15 */ ZL4_ANIM_15,
    /* 16 */ ZL4_ANIM_16,
    /* 17 */ ZL4_ANIM_17,
    /* 18 */ ZL4_ANIM_18,
    /* 19 */ ZL4_ANIM_19,
    /* 20 */ ZL4_ANIM_20,
    /* 21 */ ZL4_ANIM_21,
    /* 22 */ ZL4_ANIM_22,
    /* 23 */ ZL4_ANIM_23,
    /* 24 */ ZL4_ANIM_24,
    /* 25 */ ZL4_ANIM_25,
    /* 26 */ ZL4_ANIM_26,
    /* 27 */ ZL4_ANIM_27,
    /* 28 */ ZL4_ANIM_28,
    /* 29 */ ZL4_ANIM_29,
    /* 30 */ ZL4_ANIM_30,
    /* 31 */ ZL4_ANIM_31,
    /* 32 */ ZL4_ANIM_32,
    /* 33 */ ZL4_ANIM_33
} EnZl4Animation;

#include "z_en_zl4_anm.inc.c"

#include "z_en_zl4_cam.inc.c"

u16 zl4_set_message(PlayState* play, Actor* thisx) {
    u16 maskReactionTextId = get_mask_message(play, MASK_REACTION_SET_ZELDA);
    u16 stoneCount;
    s16 textId;

    if (maskReactionTextId != 0) {
        return maskReactionTextId;
    }

    stoneCount = 0;
    if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
        stoneCount = 1;
    }
    if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        stoneCount++;
    }
    if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
        stoneCount++;
    }

    if (stoneCount > 1) {
        textId = 0x703D;
    } else {
        textId = 0x703C;
    }
    return textId;
}

s16 zl4_end_message(PlayState* play, Actor* thisx) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        return NPC_TALK_STATE_IDLE;
    }
    return NPC_TALK_STATE_TALKING;
}

void zl4_eye_paci2(EnZl4* this) {
    if (this->blinkTimer > 0) {
        this->blinkTimer--;
    } else {
        this->blinkTimer = 0;
    }
    if (this->blinkTimer <= 2) {
        this->leftEyeState = this->rightEyeState = this->blinkTimer;
    }
    switch (this->eyeExpression) {
        case ZL4_EYES_NEUTRAL:
            if (this->blinkTimer == 0) {
                this->blinkTimer = get_random_timer(30, 30);
            }
            break;
        case ZL4_EYES_SHUT:
            if (this->blinkTimer == 0) {
                this->leftEyeState = this->rightEyeState = ZL4_EYE_SHUT;
            }
            break;
        case ZL4_EYES_LOOK_LEFT:
            if (this->blinkTimer == 0) {
                this->leftEyeState = ZL4_EYE_LOOK_OUT;
                this->rightEyeState = ZL4_EYE_LOOK_IN;
            }
            break;
        case ZL4_EYES_LOOK_RIGHT:
            if (this->blinkTimer == 0) {
                this->leftEyeState = ZL4_EYE_LOOK_IN;
                this->rightEyeState = ZL4_EYE_LOOK_OUT;
            }
            break;
        case ZL4_EYES_WIDE:
            if (this->blinkTimer == 0) {
                this->leftEyeState = this->rightEyeState = ZL4_EYE_WIDE;
            }
            break;
        case ZL4_EYES_SQUINT:
            if (this->blinkTimer == 0) {
                this->leftEyeState = this->rightEyeState = ZL4_EYE_SQUINT;
            }
            break;
        case ZL4_EYES_OPEN:
            if (this->blinkTimer >= 3) {
                this->blinkTimer = ZL4_EYE_OPEN;
            }
            break;
    }
    switch (this->mouthExpression) {
        case ZL4_MOUTH_HAPPY:
            this->mouthState = 1;
            break;
        case ZL4_MOUTH_WORRIED:
            this->mouthState = 2;
            break;
        case ZL4_MOUTH_SURPRISED:
            this->mouthState = 3;
            break;
        default:
            this->mouthState = 0;
            break;
    }
}

static void movement_by_anime(EnZl4* this, PlayState* play) {
    this->skelAnime.movementFlags |= ANIM_FLAG_UPDATE_XZ;
    Skeleton_Proc_Anime_Move_init(play, &this->actor, &this->skelAnime, 1.0f);
}

void zl4_eye_move(EnZl4* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->interactInfo.trackPos = player->actor.world.pos;
    eye_moveM(&this->actor, &this->interactInfo, 2, NPC_TRACKING_HEAD_AND_TORSO);
}

static void demo_start_pos_set(CsCmdActorCue* cue, Vec3f* dest) {
    dest->x = cue->startPos.x;
    dest->y = cue->startPos.y;
    dest->z = cue->startPos.z;
}

s32 fst_demo_continue(EnZl4* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* playerx = &GET_PLAYER(play)->actor;
    s16 rotY;

    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
    playerx->world.pos = this->actor.world.pos;
    rotY = this->actor.shape.rot.y;
    playerx->world.pos.x += 56.0f * sin_s(rotY);
    playerx->world.pos.z += 56.0f * cos_s(rotY);

    player->speedXZ = playerx->speed = 0.0f;

    start_spline_demo_camera(play, 5);
    shrink_window_setval(32);
    alpha_change(HUD_VISIBILITY_NOTHING_ALT);
    this->talkTimer2 = 0;
    return true;
}

s32 movement_anm_check(EnZl4* this) {
    if ((this->skelAnime.animation == &gChildZeldaAnim_01B874) ||
        (this->skelAnime.animation == &gChildZeldaAnim_01BCF0) ||
        (this->skelAnime.animation == &gChildZeldaAnim_0125E4) ||
        (this->skelAnime.animation == &gChildZeldaAnim_012E58) ||
        (this->skelAnime.animation == &gChildZeldaAnim_015F14) ||
        (this->skelAnime.animation == &gChildZeldaAnim_0169B4) ||
        (this->skelAnime.animation == &gChildZeldaAnim_016D08) ||
        (this->skelAnime.animation == &gChildZeldaAnim_01805C) ||
        (this->skelAnime.animation == &gChildZeldaAnim_01A2FC) ||
        (this->skelAnime.animation == &gChildZeldaAnim_01AAE0) ||
        (this->skelAnime.animation == &gChildZeldaAnim_01CE08) ||
        (this->skelAnime.animation == &gChildZeldaAnim_018898) ||
        (this->skelAnime.animation == &gChildZeldaAnim_01910C) ||
        (this->skelAnime.animation == &gChildZeldaAnim_00F0A4) ||
        (this->skelAnime.animation == &gChildZeldaAnim_00F894)) {
        return true;
    }
    return false;
}

void En_Zl4_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZl4* this = (EnZl4*)thisx;

    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gChildZeldaSkel, NULL, this->jointTable, this->morphTable, 18);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 18.0f);
    npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_21);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &Zl4ClInfoData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, NULL, &Zl4StatusData);
    Actor_set_scale(&this->actor, 0.01f);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.textId = -1;
    this->eyeExpression = this->mouthExpression = ZL4_MOUTH_NEUTRAL;

    if (IS_CUTSCENE_LAYER) {
        npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_0);
        this->actionFunc = zl4_demo;
    } else if (GET_EVENTCHKINF(EVENTCHKINF_40)) {
        npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_0);
        this->actionFunc = zl4_wait;
    } else {
        if (z_common_data.save.entranceIndex != ENTR_CASTLE_COURTYARD_ZELDA_1) {
            npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_21);
            this->csState = ZL4_CS_WAIT;
            this->talkState = 0;
        } else {
            fst_demo_continue(this, play);
            npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_0);
            this->csState = ZL4_CS_LEGEND;
            this->talkState = 0;
        }
        this->actionFunc = zl4_normal_wait;
    }
}

void En_Zl4_Actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZl4* this = (EnZl4*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

s32 end_to_next_anime(EnZl4* this, s32 nextAnim) {
    if (!Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
        return false;
    }
    npc_anime_ct(&this->skelAnime, animetbl, nextAnim);
    return true;
}

static void anime_revers(EnZl4* this) {
    f32 tempFrame = this->skelAnime.startFrame;

    this->skelAnime.startFrame = this->skelAnime.endFrame;
    this->skelAnime.curFrame = this->skelAnime.endFrame;
    this->skelAnime.endFrame = tempFrame;
    this->skelAnime.playSpeed = -1.0f;
}

s32 fst_demo_start_check(EnZl4* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* playerx = &GET_PLAYER(play)->actor;
    s16 rotY;
    s16 yawDiff;
    s16 absYawDiff;

    if (!Actor_talk_check(&this->actor, play)) {
        yawDiff = (f32)this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        absYawDiff = ABS(yawDiff);
        if ((playerx->world.pos.y != this->actor.world.pos.y) || (absYawDiff >= 0x3FFC)) {
            return false;
        } else {
            Actor_talk_request2(&this->actor, play, this->collider.dim.radius + 60.0f);
            return false;
        }
    }
    playerx->world.pos = this->actor.world.pos;
    rotY = this->actor.shape.rot.y;
    playerx->world.pos.x += 56.0f * sin_s(rotY);
    playerx->world.pos.z += 56.0f * cos_s(rotY);
    playerx->speed = 0.0f;
    player->speedXZ = 0.0f;
    return true;
}

s32 fst_demo_furimuku(EnZl4* this, PlayState* play) {
    switch (this->talkState) {
        case 0:
            if (this->skelAnime.curFrame == 50.0f) {
                Actor_SE_set(&this->actor, NA_SE_VO_Z0_MEET);
            }
            if (!end_to_next_anime(this, ZL4_ANIM_4)) {
                break;
            } else {
                message_set(play, 0x702E, NULL);
                this->talkState++;
            }
            break;
        case 1:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                start_fixed_demo_camera(play, 1);
                message_set(play, 0x702F, NULL);
                this->talkTimer2 = 0;
                this->talkState++;
            }
            break;
        case 2:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                play->csCtx.script = SEGMENTED_TO_VIRTUAL(gZeldasCourtyardMeetCs);
                z_common_data.cutsceneTrigger = 1;
                start_spline_demo_camera(play, 0);
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->talkTimer2 = 0;
                this->talkState++;
            }
            break;
        case 3:
            this->talkTimer2++;
            if (this->talkTimer2 >= 45) {
                message_set(play, 0x70F9, NULL);
                this->talkState++;
            }
            break;
        case 4:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                start_spline_demo_camera(play, 1);
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->talkTimer2 = 0;
                this->talkState++;
            }
            break;
        case 5:
            this->talkTimer2++;
            if (this->talkTimer2 >= 10) {
                message_set(play, 0x70FA, NULL);
                this->talkState++;
            }
            break;
        case 6:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                start_fixed_demo_camera(play, 2);
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_22);
                this->mouthExpression = ZL4_MOUTH_NEUTRAL;
                this->talkTimer2 = 0;
                this->talkState++;
                message_set(play, 0x70FB, NULL);
            }
            break;
    }
    return (this->talkState == 7) ? 1 : 0;
}

s32 fst_demo_motteiru(EnZl4* this, PlayState* play) {
    switch (this->talkState) {
        case 0:
            if (end_to_next_anime(this, ZL4_ANIM_25)) {
                this->talkState++;
            }
            FALLTHROUGH;
        case 1:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                start_fixed_demo_camera(play, 3);
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->talkTimer1 = 40;
                this->talkState = 2;
            }
            break;
        case 2:
            if (DECR(this->talkTimer1) == 0) {
                message_set(play, 0x7030, NULL);
                this->talkState++;
            }
            break;
        case 3:
            if (!((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play))) {
                break;
            } else if (play->msgCtx.choiceIndex == 0) {
                start_fixed_demo_camera(play, 4);
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_28);
                this->blinkTimer = 0;
                this->eyeExpression = ZL4_EYES_SQUINT;
                this->mouthExpression = ZL4_MOUTH_HAPPY;
                message_set(play, 0x7032, NULL);
                this->talkState = 7;
            } else {
                start_fixed_demo_camera(play, 2);
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_9);
                this->mouthExpression = ZL4_MOUTH_WORRIED;
                message_set(play, 0x7031, NULL);
                this->talkState++;
            }
            break;
        case 4:
            if (this->skelAnime.curFrame == 16.0f) {
                Actor_SE_set(&this->actor, NA_SE_VO_Z0_QUESTION);
            }
            if (end_to_next_anime(this, ZL4_ANIM_10)) {
                this->talkState++;
            }
            FALLTHROUGH;
        case 5:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_9);
                this->mouthExpression = ZL4_MOUTH_WORRIED;
                anime_revers(this);
                this->talkState = 6;
            }
            break;
        case 6:
            this->mouthExpression = ZL4_MOUTH_NEUTRAL;
            start_fixed_demo_camera(play, 3);
            message_set(play, 0x7030, NULL);
            this->talkState = 12;
            break;
        case 12:
            if (end_to_next_anime(this, ZL4_ANIM_25)) {
                this->talkState = 13;
            }
            FALLTHROUGH;
        case 13:
            if (!((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play))) {
                break;
            } else if (play->msgCtx.choiceIndex == 0) {
                start_fixed_demo_camera(play, 4);
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_28);
                this->blinkTimer = 0;
                this->eyeExpression = ZL4_EYES_SQUINT;
                this->mouthExpression = ZL4_MOUTH_HAPPY;
                message_set(play, 0x7032, NULL);
                this->talkState = 7;
            } else {
                start_fixed_demo_camera(play, 2);
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_9);
                this->mouthExpression = ZL4_MOUTH_WORRIED;
                message_set(play, 0x7031, NULL);
                this->talkState = 4;
            }
            break;
        case 7:
            if (this->skelAnime.curFrame == 17.0f) {
                Actor_SE_set(&this->actor, NA_SE_VO_Z0_SMILE_0);
            }
            if (end_to_next_anime(this, ZL4_ANIM_29)) {
                this->talkState++;
            }
            FALLTHROUGH;
        case 8:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                start_spline_demo_camera(play, 2);
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_0);
                this->blinkTimer = 0;
                this->eyeExpression = ZL4_EYES_NEUTRAL;
                this->mouthExpression = ZL4_MOUTH_NEUTRAL;
                message_set(play, 0x70FC, NULL);
                this->talkState = 9;
            }
            break;
        case 9:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                start_fixed_demo_camera(play, 5);
                message_set(play, 0x70FD, NULL);
                this->talkState++;
            }
            break;
        case 10:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_5);
                this->eyeExpression = ZL4_EYES_OPEN;
                this->mouthExpression = ZL4_MOUTH_SURPRISED;
                message_set(play, 0x70FE, NULL);
                this->talkState++;
            }
            break;
    }
    return (this->talkState == 11) ? 1 : 0;
}

s32 fst_demo_iwanaide(EnZl4* this, PlayState* play) {
    switch (this->talkState) {
        case 0:
            if (end_to_next_anime(this, ZL4_ANIM_4)) {
                this->talkState++;
            }
            break;
        case 1:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                start_fixed_demo_camera(play, 6);
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_1);
                this->blinkTimer = 11;
                this->eyeExpression = ZL4_EYES_SQUINT;
                this->mouthExpression = ZL4_MOUTH_NEUTRAL;
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                message_set(play, 0x70FF, NULL);
                this->talkState++;
            }
            break;
        case 2:
            if (end_to_next_anime(this, ZL4_ANIM_2)) {
                this->talkState++;
            }
            FALLTHROUGH;
        case 3:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_16);
                this->blinkTimer = 0;
                this->eyeExpression = ZL4_EYES_NEUTRAL;
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->talkState = 4;
            }
            break;
        case 4:
            if (end_to_next_anime(this, ZL4_ANIM_17)) {
                message_set(play, 0x2073, NULL);
                this->talkState++;
            }
            break;
        case 5:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                start_spline_demo_camera(play, 3);
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_0);
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->talkTimer2 = 0;
                this->talkState = 6;
            }
            break;
        case 6:
            this->talkTimer2++;
            if (this->talkTimer2 >= 15) {
                message_set(play, 0x2074, NULL);
                this->talkState++;
            }
            break;
        case 7:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_6);
                this->mouthExpression = ZL4_MOUTH_HAPPY;
                message_set(play, 0x2075, NULL);
                this->talkState++;
            }
            break;
        case 8:
            if (end_to_next_anime(this, ZL4_ANIM_25)) {
                this->talkState++;
            }
            FALLTHROUGH;
        case 9:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                message_set(play, 0x7033, NULL);
                this->talkState = 10;
            }
            break;
        case 10:
            if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
                if (play->msgCtx.choiceIndex == 0) {
                    start_spline_demo_camera(play, 4);
                    npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_33);
                    this->mouthExpression = ZL4_MOUTH_NEUTRAL;
                    play->msgCtx.msgMode = MSGMODE_PAUSED;
                    this->talkTimer2 = 0;
                    this->talkState = 15;
                } else {
                    start_fixed_demo_camera(play, 6);
                    play->msgCtx.msgMode = MSGMODE_PAUSED;
                    this->talkTimer1 = 20;
                    this->talkState++;
                    this->skelAnime.playSpeed = 0.0f;
                }
            }
            break;
        case 11:
            if (DECR(this->talkTimer1) == 0) {
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_11);
                this->blinkTimer = 11;
                this->eyeExpression = ZL4_EYES_LOOK_RIGHT;
                this->mouthExpression = ZL4_MOUTH_WORRIED;
                message_set(play, 0x7034, NULL);
                this->talkState++;
            }
            break;
        case 12:
            if (this->skelAnime.curFrame == 5.0f) {
                Actor_SE_set(&this->actor, NA_SE_VO_Z0_SIGH_0);
            }
            if (end_to_next_anime(this, ZL4_ANIM_12)) {
                this->talkState++;
            }
            FALLTHROUGH;
        case 13:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_6);
                this->blinkTimer = 3;
                this->eyeExpression = ZL4_EYES_NEUTRAL;
                this->mouthExpression = ZL4_MOUTH_HAPPY;
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->talkState = 14;
            }
            break;
        case 14:
            if (end_to_next_anime(this, ZL4_ANIM_25)) {
                message_set(play, 0x7033, NULL);
                this->talkState = 10;
            }
            break;
        case 15:
            this->talkTimer2++;
            if (this->talkTimer2 >= 30) {
                message_set(play, 0x7035, NULL);
                this->talkState++;
            }
            break;
        case 16:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->talkState++;
            }
            FALLTHROUGH;
        case 17:
            this->talkTimer2++;
            if (this->talkTimer2 == 130) {
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                play->nextEntranceIndex = ENTR_CUTSCENE_MAP_0;
                z_common_data.nextCutsceneIndex = 0xFFF7;
                play->transitionTrigger = TRANS_TRIGGER_START;
                play->transitionType = TRANS_TYPE_FADE_WHITE;
            }
            break;
    }
    if ((this->talkTimer2 == 17) && (this->talkTimer2 > 130)) {
        return true;
    }
    return false;
}

s32 fst_demo_tutawari(EnZl4* this, PlayState* play) {
    Camera* activeCam = GET_ACTIVE_CAM(play);

    switch (this->talkState) {
        case 0:
            this->talkTimer2++;
            if (this->talkTimer2 >= 60) {
                message_set(play, 0x7037, NULL);
                this->talkState++;
            }
            break;
        case 1:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                start_fixed_demo_camera(play, 7);
                message_set(play, 0x2076, NULL);
                this->talkState++;
            }
            break;
        case 2:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                start_spline_demo_camera(play, 6);
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->talkState++;
            }
            break;
        case 3:
            if (activeCam->animState == 2) {
                message_set(play, 0x2077, NULL);
                this->talkState++;
            }
            break;
        case 4:
            if (!((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play))) {
                break;
            } else if (play->msgCtx.choiceIndex == 0) {
                start_fixed_demo_camera(play, 8);
                message_set(play, 0x7005, NULL);
                this->talkState = 9;
            } else {
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_5);
                this->mouthExpression = ZL4_MOUTH_SURPRISED;
                message_set(play, 0x7038, NULL);
                this->talkState++;
                Actor_SE_set(&this->actor, NA_SE_VO_Z0_HURRY);
            }
            break;
        case 5:
            if (end_to_next_anime(this, ZL4_ANIM_4)) {
                this->talkState++;
            }
            FALLTHROUGH;
        case 6:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_33);
                this->mouthExpression = ZL4_MOUTH_NEUTRAL;
                message_set(play, 0x7037, NULL);
                this->talkState++;
            }
            break;
        case 7:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                message_set(play, 0x2076, NULL);
                this->talkState++;
            }
            break;
        case 8:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                message_set(play, 0x2077, NULL);
                this->talkState = 4;
            }
            break;
        case 9:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_26);
                message_set(play, 0x2078, NULL);
                this->talkState++;
            }
            break;
        case 10:
            if (end_to_next_anime(this, ZL4_ANIM_27)) {
                this->talkState++;
            }
            FALLTHROUGH;
        case 11:
            if (!((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play))) {
                break;
            } else if (play->msgCtx.choiceIndex == 0) {
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->talkState = 13;
            } else {
                message_set(play, 0x700B, NULL);
                this->talkState = 12;
            }
            break;
        case 12:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->talkState = 13;
            }
            break;
    }
    return (this->talkState == 13) ? 1 : 0;
}

s32 fst_demo_ganonmiru(EnZl4* this, PlayState* play) {
    switch (this->talkState) {
        case 0:
            start_spline_demo_camera(play, 7);
            play->csCtx.script = SEGMENTED_TO_VIRTUAL(gZeldasCourtyardWindowCs);
            z_common_data.cutsceneTrigger = 1;
            this->talkState++;
            break;
        case 1:
            if (play->csCtx.state != CS_STATE_IDLE) {
                if (play->csCtx.curFrame == 90) {
                    play->csCtx.state = CS_STATE_STOP;
                }
            } else {
                play->csCtx.script = SEGMENTED_TO_VIRTUAL(gZeldasCourtyardGanonCs);
                z_common_data.cutsceneTrigger = 1;
                this->talkState++;
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
            }
            break;
        case 2:
            if (play->csCtx.state != CS_STATE_IDLE) {
                if (play->csCtx.curFrame == 209) {
                    play->csCtx.state = CS_STATE_STOP;
                }
            } else {
                z_vibctl2_vib_setQ(0.0f, 160, 10, 40);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_30);
                start_fixed_demo_camera(play, 11);
                message_set(play, 0x7039, NULL);
                this->talkState++;
            }
            break;
        case 3:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->talkState++;
            }
            break;
    }
    return (this->talkState == 4) ? 1 : 0;
}

s32 fst_demo_onegaides(EnZl4* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 rotY;

    switch (this->talkState) {
        case 0:
            player->actor.world.pos = this->actor.world.pos;
            rotY = this->actor.shape.rot.y - 0x3FFC;
            player->actor.world.pos.x += 34.0f * sin_s(rotY);
            player->actor.world.pos.z += 34.0f * cos_s(rotY);
            start_spline_demo_camera(play, 8);
            this->blinkTimer = 0;
            this->eyeExpression = ZL4_EYES_WIDE;
            this->mouthExpression = ZL4_MOUTH_WORRIED;
            this->talkTimer2 = 0;
            this->talkState++;
            message_set(play, 0x2079, NULL);
            FALLTHROUGH;
        case 1:
            this->talkTimer2++;
            if (this->talkTimer2 >= 20) {
                this->talkState++;
            }
            break;
        case 2:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                start_spline_demo_camera(play, 9);
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->talkTimer2 = 0;
                this->talkState++;
            }
            break;
        case 3:
            this->talkTimer2++;
            if (this->talkTimer2 >= 20) {
                message_set(play, 0x207A, NULL);
                this->talkState++;
            }
            break;
        case 4:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                start_fixed_demo_camera(play, 12);
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_23);
                this->blinkTimer = 0;
                this->eyeExpression = ZL4_EYES_NEUTRAL;
                this->mouthExpression = ZL4_MOUTH_SURPRISED;
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->talkState++;
            }
            break;
        case 5:
            if (end_to_next_anime(this, ZL4_ANIM_24)) {
                message_set(play, 0x207B, NULL);
                this->talkState++;
            }
            break;
        case 6:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                message_set(play, 0x703A, NULL);
                this->talkState++;
            }
            break;
        case 7:
            if (!((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play))) {
                break;
            } else if (play->msgCtx.choiceIndex == 0) {
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_31);
                this->blinkTimer = 11;
                this->eyeExpression = ZL4_EYES_SQUINT;
                this->mouthExpression = ZL4_MOUTH_HAPPY;
                message_set(play, 0x703B, NULL);
                this->talkState = 11;
            } else {
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_13);
                this->blinkTimer = 11;
                this->eyeExpression = ZL4_EYES_LOOK_LEFT;
                this->mouthExpression = ZL4_MOUTH_WORRIED;
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->talkState++;
            }
            break;
        case 8:
            if (end_to_next_anime(this, ZL4_ANIM_15)) {
                this->blinkTimer = 3;
                this->eyeExpression = ZL4_EYES_NEUTRAL;
                this->mouthExpression = ZL4_MOUTH_SURPRISED;
                message_set(play, 0x7073, NULL);
                this->talkState++;
            }
            break;
        case 9:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_14);
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->talkState++;
            }
            break;
        case 10:
            if (end_to_next_anime(this, ZL4_ANIM_24)) {
                message_set(play, 0x703A, NULL);
                this->talkState = 7;
            }
            break;
        case 11:
            if (end_to_next_anime(this, ZL4_ANIM_32)) {
                this->talkState++;
            }
            FALLTHROUGH;
        case 12:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->talkState = 13;
            }
            break;
    }
    return (this->talkState == 13) ? 1 : 0;
}

s32 fst_demo_tegamiget(EnZl4* this, PlayState* play) {
    switch (this->talkState) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_18);
            this->blinkTimer = 0;
            this->eyeExpression = ZL4_EYES_NEUTRAL;
            this->mouthExpression = ZL4_MOUTH_WORRIED;
            start_spline_demo_camera(play, 10);
            this->talkTimer2 = 0;
            this->talkState++;
            FALLTHROUGH;
        case 1:
            this->talkTimer2++;
            if (this->talkTimer2 >= 10) {
                message_set(play, 0x7123, NULL);
                this->talkState++;
            }
            break;
        case 2:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                start_fixed_demo_camera(play, 13);
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_19);
                this->blinkTimer = 0;
                this->eyeExpression = ZL4_EYES_NEUTRAL;
                this->mouthExpression = ZL4_MOUTH_SURPRISED;
                message_set(play, 0x207C, NULL);
                this->talkState++;
            }
            break;
        case 3:
            if (end_to_next_anime(this, ZL4_ANIM_20)) {
                this->talkState++;
            }
            FALLTHROUGH;
        case 4:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                message_set(play, 0x207D, NULL);
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_7);
                this->blinkTimer = 0;
                this->eyeExpression = ZL4_EYES_NEUTRAL;
                this->mouthExpression = ZL4_MOUTH_NEUTRAL;
                this->talkState = 5;
                this->unk_20F = this->cueId = 0;
            }
            break;
        case 5:
            if (end_to_next_anime(this, ZL4_ANIM_8)) {
                this->talkState++;
            }
            FALLTHROUGH;
        case 6:
            if (!((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play))) {
                break;
            } else {
                changeCameraSet(GET_ACTIVE_CAM(play), CAM_SET_NORMAL0);
                this->talkState = 7;
                play->talkWithPlayer(play, &this->actor);
                Actor_carry_request_set2(&this->actor, play, GI_ZELDAS_LETTER, fabsf(this->actor.xzDistToPlayer) + 1.0f,
                                   fabsf(this->actor.yDistToPlayer) + 1.0f);
                play->msgCtx.stateTimer = 4;
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            }
            break;
        case 7:
            if (Actor_carry_check(&this->actor, play)) {
                npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_0);
                this->talkState++;
            } else {
                Actor_carry_request_set2(&this->actor, play, GI_ZELDAS_LETTER, fabsf(this->actor.xzDistToPlayer) + 1.0f,
                                   fabsf(this->actor.yDistToPlayer) + 1.0f);
            }
            // no break here is required for matching
    }
    return (this->talkState == 8) ? 1 : 0;
}

void zl4_normal_wait(EnZl4* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->csState) {
        case ZL4_CS_WAIT:
            if (fst_demo_start_check(this, play)) {
                this->talkState = 0;
                this->csState++;
            }
            break;
        case ZL4_CS_START:
            npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_3);
            this->blinkTimer = 0;
            this->eyeExpression = ZL4_EYES_NEUTRAL;
            this->mouthExpression = ZL4_MOUTH_SURPRISED;
            Na_StartFanfare(NA_BGM_APPEAR);
            start_fixed_demo_camera(play, 0);
            alpha_change(HUD_VISIBILITY_NOTHING_ALT);
            shrink_window_setval(32);
            this->talkState = 0;
            this->csState++;
            break;
        case ZL4_CS_MEET:
            if (fst_demo_furimuku(this, play)) {
                this->talkState = 0;
                this->csState++;
            }
            break;
        case ZL4_CS_STONE:
            if (fst_demo_motteiru(this, play)) {
                this->talkState = 0;
                this->csState++;
            }
            break;
        case ZL4_CS_NAMES:
            if (fst_demo_iwanaide(this, play)) {
                this->talkState = 0;
                this->csState++;
            }
            break;
        case ZL4_CS_LEGEND:
            if (fst_demo_tutawari(this, play)) {
                this->talkState = 0;
                this->csState++;
            }
            break;
        case ZL4_CS_WINDOW:
            if (fst_demo_ganonmiru(this, play)) {
                this->talkState = 0;
                this->csState++;
            }
            break;
        case ZL4_CS_GANON:
            if (fst_demo_onegaides(this, play)) {
                this->talkState = 0;
                this->csState++;
            }
            break;
        case ZL4_CS_PLAN:
            if (fst_demo_tegamiget(this, play)) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                z_common_data.prevHudVisibilityMode = HUD_VISIBILITY_ALL;
                SET_EVENTCHKINF(EVENTCHKINF_40);
                this->actionFunc = zl4_wait;
            }
            break;
    }
    this->interactInfo.trackPos = player->actor.world.pos;
    eye_moveM(&this->actor, &this->interactInfo, 2,
                   (this->csState == ZL4_CS_WINDOW) ? NPC_TRACKING_HEAD_AND_TORSO : NPC_TRACKING_NONE);
    if (movement_anm_check(this)) {
        movement_by_anime(this, play);
    }
}

void zl4_wait(EnZl4* this, PlayState* play) {
    npc_talk(play, &this->actor, &this->interactInfo.talkState, this->collider.dim.radius + 60.0f,
                      zl4_set_message, zl4_end_message);
    zl4_eye_move(this, play);
}

void zl4_demo(EnZl4* this, PlayState* play) {
    s32 animIndex[] = { ZL4_ANIM_0, ZL4_ANIM_0, ZL4_ANIM_0,  ZL4_ANIM_0,  ZL4_ANIM_0,
                        ZL4_ANIM_0, ZL4_ANIM_0, ZL4_ANIM_26, ZL4_ANIM_21, ZL4_ANIM_3 };
    CsCmdActorCue* cue;
    Vec3f pos;

    if (Skeleton_Info2_anime_play(&this->skelAnime) && (this->skelAnime.animation == &gChildZeldaAnim_010DF8)) {
        npc_anime_ct(&this->skelAnime, animetbl, ZL4_ANIM_4);
    }

    if (movement_anm_check(this)) {
        movement_by_anime(this, play);
    }

    if (play->csCtx.curFrame == 100) {
        this->eyeExpression = ZL4_EYES_LOOK_LEFT;
    }

    if (play->csCtx.curFrame == 450) {
        this->blinkTimer = 3;
        this->eyeExpression = ZL4_EYES_NEUTRAL;
        this->mouthExpression = ZL4_MOUTH_SURPRISED;
    }

    cue = play->csCtx.actorCues[0];

    if (cue != NULL) {
        demo_start_pos_set(cue, &pos);

        if (this->cueId == 0) {
            this->actor.world.pos = this->actor.home.pos = pos;
        }

        if (this->cueId != cue->id) {
            npc_anime_ct(&this->skelAnime, animetbl, animIndex[cue->id]);
            this->cueId = cue->id;
        }

        this->actor.velocity.x = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.velocity.z = 0.0f;
    }
}

void En_Zl4_Actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZl4* this = (EnZl4*)thisx;

    if (this->actionFunc != zl4_demo) {
        Skeleton_Info2_anime_play(&this->skelAnime);
    }
    zl4_eye_paci2(this);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    this->actionFunc(this, play);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnZl4* this = (EnZl4*)thisx;
    Vec3s limbRot;

    if (limbIndex == 17) {
        limbRot = this->interactInfo.headRot;
        Matrix_translate(900.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
        Matrix_translate(-900.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limbIndex == 10) {
        limbRot = this->interactInfo.torsoRot;
        Matrix_rotateY(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
    }
    if ((limbIndex >= 3) && (limbIndex < 7)) {
        *dList = NULL;
    }
    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    EnZl4* this = (EnZl4*)thisx;

    if (limbIndex == 17) {
        Matrix_Position(&zeroVec, &this->actor.focus.pos);
    }
}

void En_Zl4_Actor_draw(Actor* thisx, PlayState* play) {
    EnZl4* this = (EnZl4*)thisx;
    void* mouthTex[] = { gChildZeldaMouthNeutralTex, gChildZeldaMouthHappyTex, gChildZeldaMouthWorriedTex,
                         gChildZeldaMouthSurprisedTex };
    void* eyeTex[] = {
        gChildZeldaEyeOpenTex,   gChildZeldaEyeBlinkTex, gChildZeldaEyeShutTex, gChildZeldaEyeWideTex,
        gChildZeldaEyeSquintTex, gChildZeldaEyeOutTex,   gChildZeldaEyeInTex,
    };

    OPEN_DISPS(play->state.gfxCtx, "../z_en_zl4.c", 2012);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex[this->rightEyeState]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex[this->leftEyeState]));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouthTex[this->mouthState]));
    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, this);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_zl4.c", 2043);
}
