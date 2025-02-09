/*
 * File: z_en_mm.c
 * Overlay: ovl_En_Mm
 * Description: Running Man (adult)
 */

#include "z_en_mm2.h"
#include "terminal.h"
#include "assets/objects/object_mm/object_mm.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

typedef enum RunningManAnimIndex {
    /* 0 */ RM2_ANIM_RUN,
    /* 1 */ RM2_ANIM_SIT,
    /* 2 */ RM2_ANIM_SIT_WAIT,
    /* 3 */ RM2_ANIM_STAND,
    /* 4 */ RM2_ANIM_SPRINT,
    /* 5 */ RM2_ANIM_EXCITED, // plays when talking to him with bunny hood on
    /* 6 */ RM2_ANIM_HAPPY    // plays when you sell him the bunny hood
} RunningManAnimIndex;

typedef enum RunningManMouthTex {
    /* 0 */ RM2_MOUTH_CLOSED,
    /* 1 */ RM2_MOUTH_OPEN
} RunningManMouthTex;

void En_Mm2_Actor_ct(Actor* thisx, PlayState* play2);
void En_Mm2_Actor_dt(Actor* thisx, PlayState* play);
void En_Mm2_Actor_move(Actor* thisx, PlayState* play);
void En_Mm2_Actor_draw(Actor* thisx, PlayState* play);
void talk_sit(EnMm2* this, PlayState* play);
static void mode_move_sit(EnMm2* this, PlayState* play);
void mode_move_sit2(EnMm2* this, PlayState* play);
static s32 func_before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx);
static void func_after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx);

ActorProfile En_Mm2_Profile = {
    /**/ ACTOR_EN_MM2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_MM,
    /**/ sizeof(EnMm2),
    /**/ En_Mm2_Actor_ct,
    /**/ En_Mm2_Actor_dt,
    /**/ En_Mm2_Actor_move,
    /**/ En_Mm2_Actor_draw,
};

static ColliderCylinderInit EnMm2OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000004, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 18, 63, 0, { 0, 0, 0 } },
};

static AnimationSpeedInfo anime_ct_data[] = {
    { &gRunningManRunAnim, 1.0f, ANIMMODE_LOOP, -7.0f },     { &gRunningManSitStandAnim, -1.0f, ANIMMODE_ONCE, -7.0f },
    { &gRunningManSitWaitAnim, 1.0f, ANIMMODE_LOOP, -7.0f }, { &gRunningManSitStandAnim, 1.0f, ANIMMODE_ONCE, -7.0f },
    { &gRunningManSprintAnim, 1.0f, ANIMMODE_LOOP, -7.0f },  { &gRunningManExcitedAnim, 1.0f, ANIMMODE_LOOP, -12.0f },
    { &gRunningManHappyAnim, 1.0f, ANIMMODE_LOOP, -12.0f },
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_STOP),
};

void mm2_anime_ct(EnMm2* this, s32 index, s32* currentIndex) {
    f32 phi_f0;

    if ((*currentIndex < 0) || (index == *currentIndex)) {
        phi_f0 = 0.0f;
    } else {
        phi_f0 = anime_ct_data[index].morphFrames;
    }

    if (anime_ct_data[index].playSpeed >= 0.0f) {
        Skeleton_Info2_init(&this->skelAnime, anime_ct_data[index].animation, anime_ct_data[index].playSpeed, 0.0f,
                         (f32)Si2_anime_end_frame(anime_ct_data[index].animation), anime_ct_data[index].mode,
                         phi_f0);
    } else {
        Skeleton_Info2_init(&this->skelAnime, anime_ct_data[index].animation, anime_ct_data[index].playSpeed,
                         (f32)Si2_anime_end_frame(anime_ct_data[index].animation), 0.0f, anime_ct_data[index].mode,
                         phi_f0);
    }
    *currentIndex = index;
}

void mm2_set_message(EnMm2* this, PlayState* play) {
    if (!GET_EVENTCHKINF_CARPENTERS_ALL_RESCUED()) {
        this->actor.textId = 0x6086;
    } else if (GET_INFTABLE(INFTABLE_17F)) {
        if (GET_EVENTINF(EVENTINF_MARATHON_ACTIVE)) {
            this->actor.textId = 0x6082;
        } else if (z_common_data.subTimerState != SUBTIMER_STATE_OFF) {
            this->actor.textId = 0x6076;
        } else if (HIGH_SCORE(HS_MARATHON) == 158) {
            this->actor.textId = 0x607E;
        } else {
            this->actor.textId = 0x6081;
        }
    } else if (z_common_data.subTimerState != SUBTIMER_STATE_OFF) {
        this->actor.textId = 0x6076;
    } else {
        this->actor.textId = 0x607D;
        CLEAR_EVENTINF(EVENTINF_MARATHON_ACTIVE);
        HIGH_SCORE(HS_MARATHON) = 158;
    }
}

void En_Mm2_Actor_ct(Actor* thisx, PlayState* play2) {
    EnMm2* this = (EnMm2*)thisx;
    PlayState* play = play2;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 21.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gRunningManSkel, NULL, this->jointTable, this->morphTable, 16);
    Skeleton_Info2_init(&this->skelAnime, anime_ct_data[RM2_ANIM_SIT_WAIT].animation, 1.0f, 0.0f,
                     Si2_anime_end_frame(anime_ct_data[RM2_ANIM_SIT_WAIT].animation),
                     anime_ct_data[RM2_ANIM_SIT_WAIT].mode, anime_ct_data[RM2_ANIM_SIT_WAIT].morphFrames);
    this->previousAnimation = RM2_ANIM_SIT_WAIT;
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnMm2OcInfoData);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->mouthTexIndex = RM2_MOUTH_CLOSED;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->unk_1F4 |= 1;
    this->actor.gravity = -1.0f;
    if (this->actor.params == 1) {
        this->actionFunc = mode_move_sit2;
    } else {
        mm2_set_message(this, play);
        this->actionFunc = mode_move_sit;
    }
    if (!LINK_IS_ADULT) {
        Actor_delete(&this->actor);
    }
    if (this->actor.params == 1) {
        if (!GET_INFTABLE(INFTABLE_17F) || !GET_EVENTINF(EVENTINF_MARATHON_ACTIVE)) {
            PRINTF(VT_FGCOL(CYAN) " マラソン 開始されていない \n" VT_RST "\n");
            Actor_delete(&this->actor);
        }
    }
}

void En_Mm2_Actor_dt(Actor* thisx, PlayState* play) {
    EnMm2* this = (EnMm2*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static s32 kihon_process(EnMm2* this, PlayState* play, EnMm2ActionFunc actionFunc) {
    s16 yawDiff;

    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = actionFunc;
        return 1;
    }
    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    if ((ABS(yawDiff) <= 0x4300) && (this->actor.xzDistToPlayer < 100.0f)) {
        Actor_talk_request2(&this->actor, play, 100.0f);
    }
    return 0;
}

static void mode_move_run(EnMm2* this, PlayState* play) {
    if (this->unk_1F6 > 60) {
        Actor_delete(&this->actor);
    }
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->unk_1F6++;
    add_calc(&this->actor.speed, 10.0f, 0.6f, 2.0f, 0.0f);
}

static void mode_move_sit2run(EnMm2* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->actionFunc = mode_move_run;
        mm2_anime_ct(this, RM2_ANIM_RUN, &this->previousAnimation);
        this->mouthTexIndex = RM2_MOUTH_OPEN;
        if (!(this->unk_1F4 & 2)) {
            message_close(play);
        }
        z_common_data.subTimerState = SUBTIMER_STATE_OFF;
        CLEAR_EVENTINF(EVENTINF_MARATHON_ACTIVE);
    }
}

void talk_sit(EnMm2* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    switch (this->actor.textId) {
        case 0x607D:
        case 0x607E:
            if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        message_set2(play, 0x607F);
                        this->actor.textId = 0x607F;
                        SET_EVENTINF(EVENTINF_MARATHON_ACTIVE);
                        break;
                    case 1:
                        message_set2(play, 0x6080);
                        this->actor.textId = 0x6080;
                        break;
                }

                if (this->unk_1F4 & 4) {
                    this->unk_1F4 &= ~4;
                    HIGH_SCORE(HS_MARATHON)++;
                }
            }
            break;

        case 0x6081:
            if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
                this->unk_1F4 |= 4;
                HIGH_SCORE(HS_MARATHON)--;
                message_set2(play, 0x607E);
                this->actor.textId = 0x607E;
            }
            break;

        default:
            if (Actor_talk_end_check(&this->actor, play)) {
                if (this->actor.textId == 0x607F) {
                    total_event_timer_set(0);
                    this->actionFunc = mode_move_sit;
                } else {
                    this->actionFunc = mode_move_sit;
                }
                this->actionFunc = mode_move_sit;
                mm2_set_message(this, play);
            }
            break;
    }
}

static void mode_move_sit(EnMm2* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    mm2_set_message(this, play);
    if ((kihon_process(this, play, talk_sit)) && (this->actor.textId == 0x607D)) {
        SET_INFTABLE(INFTABLE_17F);
    }
}

void talk_sit2(EnMm2* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        this->unk_1F4 &= ~1;
        mm2_anime_ct(this, RM2_ANIM_STAND, &this->previousAnimation);
        this->actionFunc = mode_move_sit2run;
    }
}

void mode_move_sit2(EnMm2* this, PlayState* play) {
    this->actor.world.rot.y = -0x3E80;
    this->actor.shape.rot.y = this->actor.world.rot.y;
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (((void)0, z_common_data.subTimerSeconds) < HIGH_SCORE(HS_MARATHON)) {
        this->actor.textId = 0x6085;
    } else {
        this->actor.textId = 0x6084;
    }
    if (kihon_process(this, play, talk_sit2)) {
        this->unk_1F6 = 0;
        if (((void)0, z_common_data.subTimerSeconds) < HIGH_SCORE(HS_MARATHON)) {
            HIGH_SCORE(HS_MARATHON) = z_common_data.subTimerSeconds;
        }
    } else {
        LOG_HEX("((z_common_data.event_inf[1]) & (0x0001))", GET_EVENTINF(EVENTINF_MARATHON_ACTIVE), "../z_en_mm2.c",
                541);
        if (!GET_EVENTINF(EVENTINF_MARATHON_ACTIVE)) {
            this->unk_1F4 |= 2;
            this->unk_1F4 &= ~1;
            mm2_anime_ct(this, RM2_ANIM_STAND, &this->previousAnimation);
            this->actionFunc = mode_move_sit2run;
        }
    }
}

void En_Mm2_Actor_move(Actor* thisx, PlayState* play) {
    EnMm2* this = (EnMm2*)thisx;
    s32 pad;

    if (this->unk_1F4 & 1) {
        eye_move2(play, &this->actor, &this->unk_1E8, &this->unk_1EE, this->actor.focus.pos);
    } else {
        add_calc_short_angle2(&this->unk_1E8.x, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->unk_1E8.y, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->unk_1EE.x, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->unk_1EE.y, 0, 6, 6200, 100);
    }
    this->actionFunc(this, play);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
}

void En_Mm2_Actor_draw(Actor* thisx, PlayState* play) {
    static void* mouth_txt[] = { gRunningManMouthOpenTex, gRunningManMouthClosedTex };
    EnMm2* this = (EnMm2*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_mm2.c", 634);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(mouth_txt[this->mouthTexIndex]));
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          func_before_display, func_after_display, this);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_mm2.c", 654);
}

static s32 func_before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnMm2* this = (EnMm2*)thisx;

    switch (limbIndex) {
        case 8:
            rot->x += this->unk_1EE.y;
            rot->y -= this->unk_1EE.x;
            break;
        case 15:
            rot->x += this->unk_1E8.y;
            rot->z += this->unk_1E8.x + 0xFA0;
            break;
    }

    return 0;
}

static void func_after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f pos = { 200.0f, 800.0f, 0.0f };
    EnMm2* this = (EnMm2*)thisx;

    if (limbIndex == 15) {
        Matrix_Position(&pos, &this->actor.focus.pos);
    }
}
