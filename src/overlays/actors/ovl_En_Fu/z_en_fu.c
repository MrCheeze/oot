/*
 * File: z_en_fu.c
 * Overlay: ovl_En_Fu
 * Description: Windmill Man
 */

#include "z_en_fu.h"
#include "assets/objects/object_fu/object_fu.h"
#include "assets/scenes/indoors/hakasitarelay/hakasitarelay_scene.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA)

#define FU_RESET_LOOK_ANGLE (1 << 0)
#define FU_WAIT (1 << 1)

void En_Fu_Actor_ct(Actor* thisx, PlayState* play);
void En_Fu_Actor_dt(Actor* thisx, PlayState* play);
void En_Fu_Actor_move(Actor* thisx, PlayState* play);
void En_Fu_Actor_draw(Actor* thisx, PlayState* play);

static void move_matsu(EnFu* this, PlayState* play);
static void talk_matsu(EnFu* this, PlayState* play);

void move_lecture(EnFu* this, PlayState* play);
void start_lecture(EnFu* this, PlayState* play);
void tehon_lecture(EnFu* this, PlayState* play);
void talk_lecture(EnFu* this, PlayState* play);
void do_lecture(EnFu* this, PlayState* play);
void end_lecture(EnFu* this, PlayState* play);

ActorProfile En_Fu_Profile = {
    /**/ ACTOR_EN_FU,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_FU,
    /**/ sizeof(EnFu),
    /**/ En_Fu_Actor_ct,
    /**/ En_Fu_Actor_dt,
    /**/ En_Fu_Actor_move,
    /**/ En_Fu_Actor_draw,
};

static ColliderCylinderInit EnFuOcInfoData = {
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

static Vec3f pos = {
    700.0f,
    700.0f,
    0.0f,
};

typedef enum EnFuFace {
    /* 0x00 */ FU_FACE_CALM,
    /* 0x01 */ FU_FACE_MAD
} EnFuFace;

void En_Fu_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFu* this = (EnFu*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 36.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelanime, &gWindmillManSkel, &gWindmillManPlayStillAnim, this->jointTable,
                       this->morphTable, FU_LIMB_MAX);
    Skeleton_Info2_init_standard_repeat(&this->skelanime, &gWindmillManPlayStillAnim);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnFuOcInfoData);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_set_scale(&this->actor, 0.01f);
    if (!LINK_IS_ADULT) {
        this->actionFunc = move_matsu;
        this->facialExpression = FU_FACE_CALM;
    } else {
        this->actionFunc = move_lecture;
        this->facialExpression = FU_FACE_MAD;
        this->skelanime.playSpeed = 2.0f;
    }
    this->behaviorFlags = 0;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
}

void En_Fu_Actor_dt(Actor* thisx, PlayState* play) {
    EnFu* this = (EnFu*)thisx;
    ClObjPipe_dt(play, &this->collider);
}

static s32 kihon_process(EnFu* this, PlayState* play, u16 textId, EnFuActionFunc actionFunc) {
    s16 s_dis;

    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = actionFunc;
        return true;
    }
    this->actor.textId = textId;
    s_dis = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if ((ABS(s_dis) < 0x2301) && (this->actor.xzDistToPlayer < 100.0f)) {
        Actor_talk_request2(&this->actor, play, 100.0f);
    } else {
        this->behaviorFlags |= FU_RESET_LOOK_ANGLE;
    }
    return false;
}

static void talk_matsu(EnFu* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        this->behaviorFlags &= ~FU_WAIT;
        this->actionFunc = move_matsu;

        if (this->skelanime.animation == &gWindmillManPlayAndMoveHeadAnim) {
            Skeleton_Info2_init(&this->skelanime, &gWindmillManPlayStillAnim, 1.0f, 0.0f,
                             Si2_anime_end_frame(&gWindmillManPlayStillAnim), ANIMMODE_ONCE, -4.0f);
        }
    }
}

static void move_matsu(EnFu* this, PlayState* play) {
    u16 textId = get_mask_message(play, MASK_REACTION_SET_WINDMILL_MAN);

    if (textId == 0) {
        textId = GET_EVENTCHKINF(EVENTCHKINF_DRAINED_WELL) ? 0x5033 : 0x5032;
    }

    // if ACTOR_FLAG_TALK is set and textId is 0x5033, change animation
    // if kihon_process returns 1, actionFunc is set to talk_matsu
    if (kihon_process(this, play, textId, talk_matsu)) {
        if (textId == 0x5033) {
            Skeleton_Info2_init(&this->skelanime, &gWindmillManPlayAndMoveHeadAnim, 1.0f, 0.0f,
                             Si2_anime_end_frame(&gWindmillManPlayAndMoveHeadAnim), ANIMMODE_ONCE, -4.0f);
        }
    }
}

void end_lecture(EnFu* this, PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        this->actionFunc = move_lecture;
        SET_EVENTCHKINF(EVENTCHKINF_5B);
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
    }
}

void talk_lecture(EnFu* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        this->actionFunc = move_lecture;
    }
}

void do_lecture(EnFu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->msgCtx.ocarinaMode >= OCARINA_MODE_04) {
        this->actionFunc = move_lecture;
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_03) {
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        this->actionFunc = end_lecture;
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gSongOfStormsCs);
        z_common_data.cutsceneTrigger = 1;
        item_get_setting(play, ITEM_SONG_STORMS);
        play->msgCtx.ocarinaMode = OCARINA_MODE_00;
        SET_EVENTCHKINF(EVENTCHKINF_65);
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_02) {
        player->stateFlags2 &= ~PLAYER_STATE2_24;
        this->actionFunc = move_lecture;
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_01) {
        player->stateFlags2 |= PLAYER_STATE2_23;
    }
}

void tehon_lecture(EnFu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->stateFlags2 |= PLAYER_STATE2_23;
    // if dialog state is 7, player has played back the song
    if (message_check(&play->msgCtx) == TEXT_STATE_SONG_DEMO_DONE) {
        ocarina_set(play, OCARINA_ACTION_PLAYBACK_STORMS);
        this->actionFunc = do_lecture;
    }
}

void start_lecture(EnFu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->stateFlags2 |= PLAYER_STATE2_23;
    // if dialog state is 2, start song demonstration
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->behaviorFlags &= ~FU_WAIT;
        // Ocarina is set to harp here but is immediately overwritten to the grind organ in the message system
        Na_SetOcarinaModeFlag(OCARINA_INSTRUMENT_HARP);
        ocarina_set(play, OCARINA_ACTION_TEACH_STORMS);
        this->actionFunc = tehon_lecture;
    }
}

void move_lecture(EnFu* this, PlayState* play) {
    static s16 s_dis;
    Player* player = GET_PLAYER(play);

    s_dis = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    if (GET_EVENTCHKINF(EVENTCHKINF_5B)) {
        kihon_process(this, play, 0x508E, talk_lecture);
    } else if (player->stateFlags2 & PLAYER_STATE2_24) {
        this->actor.textId = 0x5035;
        message_set(play, this->actor.textId, NULL);
        this->actionFunc = start_lecture;
        this->behaviorFlags |= FU_WAIT;
    } else if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = talk_lecture;
    } else if (ABS(s_dis) < 0x2301) {
        if (this->actor.xzDistToPlayer < 100.0f) {
            this->actor.textId = 0x5034;
            Actor_talk_request2(&this->actor, play, 100.0f);
            player->stateFlags2 |= PLAYER_STATE2_23;
        }
    }
}

void En_Fu_Actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFu* this = (EnFu*)thisx;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    if (!(this->behaviorFlags & FU_WAIT) && Skeleton_Info2_anime_play(&this->skelanime)) {
        Skeleton_Info2_init(&this->skelanime, this->skelanime.animation, 1.0f, 0.0f,
                         Si2_anime_end_frame(this->skelanime.animation), ANIMMODE_ONCE, 0.0f);
    }
    this->actionFunc(this, play);
    if (this->behaviorFlags & FU_RESET_LOOK_ANGLE) {
        add_calc_short_angle2(&this->lookAngleOffset.x, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->lookAngleOffset.y, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->unk_2A2.x, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->unk_2A2.y, 0, 6, 6200, 100);
        this->behaviorFlags &= ~FU_RESET_LOOK_ANGLE;
    } else {
        eye_move2(play, &this->actor, &this->lookAngleOffset, &this->unk_2A2, this->actor.focus.pos);
    }
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnFu* this = (EnFu*)thisx;
    s32 pad;

    if (limbIndex == FU_LIMB_UNK) {
        return false;
    }
    switch (limbIndex) {
        case FU_LIMB_HEAD:
            rot->x += this->lookAngleOffset.y;
            rot->z += this->lookAngleOffset.x;
            break;
        case FU_LIMB_CHEST_MUSIC_BOX:
            break;
    }

    if (!(this->behaviorFlags & FU_WAIT)) {
        return false;
    }

    if (limbIndex == FU_LIMB_CHEST_MUSIC_BOX) {
        rot->y += sin_s((play->state.frames * (limbIndex * FIDGET_FREQ_LIMB + FIDGET_FREQ_Y))) * FIDGET_AMPLITUDE;
        rot->z += cos_s((play->state.frames * (limbIndex * FIDGET_FREQ_LIMB + FIDGET_FREQ_Z))) * FIDGET_AMPLITUDE;
    }
    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnFu* this = (EnFu*)thisx;

    if (limbIndex == FU_LIMB_HEAD) {
        Matrix_Position(&pos, &this->actor.focus.pos);
    }
}

void En_Fu_Actor_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = { gWindmillManEyeClosedTex, gWindmillManEyeAngryTex };
    static void* mouth_txt[] = { gWindmillManMouthOpenTex, gWindmillManMouthAngryTex };
    s32 pad;
    EnFu* this = (EnFu*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_fu.c", 773);

    _polygon_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->facialExpression]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouth_txt[this->facialExpression]));
    Si2_draw_SV(play, this->skelanime.skeleton, this->skelanime.jointTable, this->skelanime.dListCount,
                          before_display, after_display, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_fu.c", 791);
}
