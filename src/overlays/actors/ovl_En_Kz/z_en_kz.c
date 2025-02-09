/*
 * File: z_en_kz.c
 * Overlay: ovl_En_Kz
 * Description: King Zora
 */

#include "z_en_kz.h"
#include "versions.h"
#include "assets/objects/object_kz/object_kz.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Kz_Actor_ct(Actor* thisx, PlayState* play);
void En_Kz_Actor_dt(Actor* thisx, PlayState* play);
void En_Kz_Actor_move(Actor* thisx, PlayState* play);
void En_Kz_Actor_draw(Actor* thisx, PlayState* play);

void kz_taiki(EnKz* this, PlayState* play);
void kz_demo_start(EnKz* this, PlayState* play);
void kz_demo(EnKz* this, PlayState* play);
void kz_demo_end(EnKz* this, PlayState* play);
void kz_matsu(EnKz* this, PlayState* play);
void kz_carry_request(EnKz* this, PlayState* play);
void kz_carry_end(EnKz* this, PlayState* play);

ActorProfile En_Kz_Profile = {
    /**/ ACTOR_EN_KZ,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_KZ,
    /**/ sizeof(EnKz),
    /**/ En_Kz_Actor_ct,
    /**/ En_Kz_Actor_dt,
    /**/ En_Kz_Actor_move,
    /**/ En_Kz_Actor_draw,
};

static ColliderCylinderInit EnKzAtInfoData = {
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
    { 80, 120, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 KzStatusData = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum EnKzAnimation {
    /* 0 */ ENKZ_ANIM_0,
    /* 1 */ ENKZ_ANIM_1,
    /* 2 */ ENKZ_ANIM_2
} EnKzAnimation;

static AnimationInfo anime_ct_data[] = {
    { &gKzIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gKzIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gKzMweepAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
};

u16 kz_set_message_ch(PlayState* play, EnKz* this) {
    Player* player = GET_PLAYER(play);

    if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
        return 0x402B;
    } else if (GET_EVENTCHKINF(EVENTCHKINF_GAVE_LETTER_TO_KING_ZORA)) {
        return 0x401C;
    } else {
        player->exchangeItemId = EXCH_ITEM_BOTTLE_RUTOS_LETTER;
        return 0x401A;
    }
}

u16 kz_set_message_ad(PlayState* play, EnKz* this) {
    Player* player = GET_PLAYER(play);

    if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_EYEBALL_FROG) {
        if (!GET_INFTABLE(INFTABLE_139)) {
            if (CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_ZORA)) {
                return 0x401F;
            } else {
                return 0x4012;
            }
        } else {
            return CHECK_QUEST_ITEM(QUEST_SONG_SERENADE) ? 0x4045 : 0x401A;
        }
    } else {
        player->exchangeItemId = EXCH_ITEM_PRESCRIPTION;
        return 0x4012;
    }
}

u16 kz_set_message(PlayState* play, Actor* thisx) {
    EnKz* this = (EnKz*)thisx;
    u16 textId = get_mask_message(play, MASK_REACTION_SET_KING_ZORA);

    if (textId != 0) {
        return textId;
    }

    if (LINK_IS_ADULT) {
        return kz_set_message_ad(play, this);
    } else {
        return kz_set_message_ch(play, this);
    }
}

s16 kz_end_message(PlayState* play, Actor* thisx) {
    EnKz* this = (EnKz*)thisx;
    s16 talkState = NPC_TALK_STATE_TALKING;

    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
            break;
#if OOT_VERSION < PAL_1_0
        case TEXT_STATE_CLOSING:
            talkState = NPC_TALK_STATE_IDLE;
            switch (this->actor.textId) {
                case 0x4012:
                    SET_INFTABLE(INFTABLE_139);
                    FALLTHROUGH;
                case 0x401B:
                    talkState = NPC_TALK_STATE_ACTION;
                    break;
                case 0x401F:
                    SET_INFTABLE(INFTABLE_139);
                    break;
            }
            break;
#else
        case TEXT_STATE_CLOSING:
            break;
        case TEXT_STATE_DONE:
            talkState = NPC_TALK_STATE_IDLE;
            switch (this->actor.textId) {
                case 0x4012:
                    SET_INFTABLE(INFTABLE_139);
                    talkState = NPC_TALK_STATE_ACTION;
                    break;
                case 0x401B:
                    talkState = !pad_on_check(play) ? NPC_TALK_STATE_TALKING : NPC_TALK_STATE_ACTION;
                    break;
                case 0x401F:
                    SET_INFTABLE(INFTABLE_139);
                    break;
            }
            break;
#endif
        case TEXT_STATE_DONE_FADING:
            if (this->actor.textId != 0x4014) {
                if (this->actor.textId == 0x401B && !this->sfxPlayed) {
                    Nai_FxFlagEntry(NA_SE_SY_CORRECT_CHIME, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    this->sfxPlayed = true;
                }
            } else if (!this->sfxPlayed) {
                Nai_FxFlagEntry(NA_SE_SY_TRE_BOX_APPEAR, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
                this->sfxPlayed = true;
            }
            break;
        case TEXT_STATE_CHOICE:
            if (!pad_on_check(play)) {
                break;
            }
            if (this->actor.textId == 0x4014) {
                if (play->msgCtx.choiceIndex == 0) {
#if OOT_VERSION >= PAL_1_0
                    kz_carry_request(this, play);
#endif
                    talkState = NPC_TALK_STATE_ACTION;
                } else {
                    this->actor.textId = 0x4016;
                    message_set2(play, this->actor.textId);
                }
            }
            break;
        case TEXT_STATE_EVENT:
            if (pad_on_check(play)) {
                talkState = NPC_TALK_STATE_ACTION;
            }
            break;
#if OOT_VERSION < PAL_1_0
        case TEXT_STATE_DONE:
            if (pad_on_check(play)) {
                talkState = NPC_TALK_STATE_ITEM_GIVEN;
            }
            break;
#endif
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            break;
    }
    return talkState;
}

void kz_eye_paci(EnKz* this) {
    if (DECR(this->blinkTimer) == 0) {
        this->eyeIdx++;
        if (this->eyeIdx >= 3) {
            this->blinkTimer = get_random_timer(30, 30);
            this->eyeIdx = 0;
        }
    }
}

/**
 * Custom version of npc_talk.
 *
 * @see npc_talk
 */
s32 kz_talk_sub(PlayState* play, Actor* thisx, s16* talkState, f32 interactRange, NpcGetTextIdFunc getTextId,
                       NpcUpdateTalkStateFunc updateTalkState) {
    Player* player = GET_PLAYER(play);
    s16 x;
    s16 y;
    f32 xzDistToPlayer;
    f32 yaw;

    if (Actor_talk_check(thisx, play)) {
        *talkState = NPC_TALK_STATE_TALKING;
        return true;
    }

#if OOT_VERSION >= PAL_1_0
    if (*talkState != NPC_TALK_STATE_IDLE) {
        *talkState = updateTalkState(play, thisx);
        return false;
    }

    yaw = search_position_angleY(&thisx->home.pos, &player->actor.world.pos);
    yaw -= thisx->shape.rot.y;
    if ((fabsf(yaw) > 1638.0f) || (thisx->xzDistToPlayer < 265.0f)) {
        thisx->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        return false;
    }

    thisx->flags |= ACTOR_FLAG_ATTENTION_ENABLED;
#endif

    Actor_display_position_set(play, thisx, &x, &y);
    if (!((x >= -30) && (x < 361) && (y >= -10) && (y < 241))) {
        return false;
    }

#if OOT_VERSION < PAL_1_0
    if (*talkState != NPC_TALK_STATE_IDLE) {
        *talkState = updateTalkState(play, thisx);
        return false;
    }
#endif

    xzDistToPlayer = thisx->xzDistToPlayer;
    thisx->xzDistToPlayer = search_position_distanceXZ(&thisx->home.pos, &player->actor.world.pos);
    if (Actor_talk_request2(thisx, play, interactRange) == 0) {
        thisx->xzDistToPlayer = xzDistToPlayer;
        return false;
    }
    thisx->xzDistToPlayer = xzDistToPlayer;
    thisx->textId = getTextId(play, thisx);

    return false;
}

void kz_talk(EnKz* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 yaw;

#if OOT_VERSION < PAL_1_0
    yaw = search_position_angleY(&this->actor.home.pos, &player->actor.world.pos);
    yaw -= this->actor.shape.rot.y;
    if (fabsf(yaw) > 1820.0f) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        return;
    }
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
#endif

    if (kz_talk_sub(play, &this->actor, &this->interactInfo.talkState, 340.0f, kz_set_message,
                           kz_end_message)) {
        if ((this->actor.textId == 0x401A) && !GET_EVENTCHKINF(EVENTCHKINF_GAVE_LETTER_TO_KING_ZORA)) {
            if (Actor_get_item_check(play) == EXCH_ITEM_BOTTLE_RUTOS_LETTER) {
                this->actor.textId = 0x401B;
                this->sfxPlayed = false;
            } else {
                this->actor.textId = 0x401A;
            }
            player->actor.textId = this->actor.textId;
            return;
        }

        if (LINK_IS_ADULT) {
            if ((INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_PRESCRIPTION) &&
                (Actor_get_item_check(play) == EXCH_ITEM_PRESCRIPTION)) {
                this->actor.textId = 0x4014;
                this->sfxPlayed = false;
                player->actor.textId = this->actor.textId;
#if OOT_VERSION >= PAL_1_0
                this->isTrading = true;
#endif
                return;
            }

#if OOT_VERSION >= PAL_1_0
            this->isTrading = false;
#endif
            if (GET_INFTABLE(INFTABLE_139)) {
                this->actor.textId = CHECK_QUEST_ITEM(QUEST_SONG_SERENADE) ? 0x4045 : 0x401A;
                player->actor.textId = this->actor.textId;
            } else {
                this->actor.textId = CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_ZORA) ? 0x401F : 0x4012;
                player->actor.textId = this->actor.textId;
            }
        }
    }
}

s32 kz_path_move(EnKz* this, PlayState* play) {
    Path* path;
    Vec3s* pointPos;
    f32 pathDiffX;
    f32 pathDiffZ;

    if (PARAMS_GET_NOSHIFT(this->actor.params, 8, 8) == 0xFF00) {
        return 0;
    }

    path = &play->pathList[PARAMS_GET_S(this->actor.params, 8, 8)];
    pointPos = SEGMENTED_TO_VIRTUAL(path->points);
    pointPos += this->waypoint;

    pathDiffX = pointPos->x - this->actor.world.pos.x;
    pathDiffZ = pointPos->z - this->actor.world.pos.z;
    add_calc_short_angle2(&this->actor.world.rot.y, RAD_TO_BINANG(fatan2(pathDiffX, pathDiffZ)), 0xA, 0x3E8, 1);

    if ((SQ(pathDiffX) + SQ(pathDiffZ)) < 10.0f) {
        this->waypoint++;
        if (this->waypoint >= path->count) {
            this->waypoint = 0;
        }
        return 1;
    }
    return 0;
}

s32 kz_set_path_end_pos(EnKz* this, PlayState* play) {
    Path* path;
    Vec3s* lastPointPos;

    if (PARAMS_GET_NOSHIFT(this->actor.params, 8, 8) == 0xFF00) {
        return 0;
    }

    path = &play->pathList[PARAMS_GET_S(this->actor.params, 8, 8)];
    lastPointPos = SEGMENTED_TO_VIRTUAL(path->points);
    lastPointPos += path->count - 1;

    this->actor.world.pos.x = lastPointPos->x;
    this->actor.world.pos.y = lastPointPos->y;
    this->actor.world.pos.z = lastPointPos->z;

    return 1;
}

void En_Kz_Actor_ct(Actor* thisx, PlayState* play) {
    EnKz* this = (EnKz*)thisx;
    s32 pad;

    Skeleton_Info2_SV_M_ct(play, &this->skelanime, &gKzSkel, NULL, this->jointTable, this->morphTable, 12);
    Shape_Info_init(&this->actor.shape, 0.0, NULL, 0.0);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnKzAtInfoData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, NULL, &KzStatusData);
    Actor_set_scale(&this->actor, 0.01);
    this->actor.attentionRangeType = ATTENTION_RANGE_3;
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    npc_anime_ct(&this->skelanime, anime_ct_data, ENKZ_ANIM_0);

    if (GET_EVENTCHKINF(EVENTCHKINF_GAVE_LETTER_TO_KING_ZORA)) {
        kz_set_path_end_pos(this, play);
    }

    if (LINK_IS_ADULT) {
        if (!GET_INFTABLE(INFTABLE_138)) {
            Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BG_ICE_SHELTER, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0x04FF);
        }
        this->actionFunc = kz_matsu;
    } else {
        this->actionFunc = kz_taiki;
    }
}

void En_Kz_Actor_dt(Actor* thisx, PlayState* play) {
    EnKz* this = (EnKz*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void kz_taiki(EnKz* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        npc_anime_ct(&this->skelanime, anime_ct_data, ENKZ_ANIM_2);
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        this->actionFunc = kz_demo_start;
    } else {
        program_wait(play, this->fidgetTableY, this->fidgetTableZ, 12);
    }
}

void kz_demo_start(EnKz* this, PlayState* play) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f }; // unused
    Vec3f subCamAt;
    Vec3f subCamEye;

    this->subCamId = Gama_play_make_camera(play);
    this->returnToCamId = play->activeCamId;
    Gama_play_set_camera_status(play, this->returnToCamId, CAM_STAT_WAIT);
    Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
    subCamAt = this->actor.world.pos;
    subCamEye = this->actor.home.pos;
    subCamAt.y += 60.0f;
    subCamEye.y += -100.0f;
    subCamEye.z += 260.0f;
    Gama_play_camera_setting(play, this->subCamId, &subCamAt, &subCamEye);
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
    this->actor.speed = 0.1f;
    this->actionFunc = kz_demo;
}

void kz_demo(EnKz* this, PlayState* play) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f }; // unused
    Vec3f subCamAt;
    Vec3f subCamEye;

    subCamAt = this->actor.world.pos;
    subCamEye = this->actor.home.pos;
    subCamAt.y += 60.0f;
    subCamEye.y += -100.0f;
    subCamEye.z += 260.0f;
    Gama_play_camera_setting(play, this->subCamId, &subCamAt, &subCamEye);
    if ((kz_path_move(this, play) == 1) && (this->waypoint == 0)) {
        npc_anime_ct(&this->skelanime, anime_ct_data, ENKZ_ANIM_1);
        item_change_setting(play, ITEM_BOTTLE_RUTOS_LETTER, ITEM_BOTTLE_EMPTY);
        kz_set_path_end_pos(this, play);
        SET_EVENTCHKINF(EVENTCHKINF_GAVE_LETTER_TO_KING_ZORA);
        this->actor.speed = 0.0;
        this->actionFunc = kz_demo_end;
    }
    if (this->skelanime.curFrame == 13.0f) {
        Actor_SE_set(&this->actor, NA_SE_VO_KZ_MOVE);
    }
}

void kz_demo_end(EnKz* this, PlayState* play) {
    Gama_play_set_camera_status(play, this->returnToCamId, CAM_STAT_ACTIVE);
    Gama_play_clear_camera(play, this->subCamId);
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
    this->actionFunc = kz_matsu;
}

void kz_matsu(EnKz* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
#if OOT_VERSION < PAL_1_0
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
#endif
        this->actionFunc = kz_carry_request;
        kz_carry_request(this, play);
    } else {
        program_wait(play, this->fidgetTableY, this->fidgetTableZ, 12);
    }
}

void kz_carry_request(EnKz* this, PlayState* play) {
    s32 getItemId;
    f32 xzRange;
    f32 yRange;

    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
        this->actionFunc = kz_carry_end;
    } else {
#if OOT_VERSION < PAL_1_0
        getItemId = Actor_get_item_check(play) == EXCH_ITEM_PRESCRIPTION ? GI_EYEBALL_FROG : GI_TUNIC_ZORA;
#else
        getItemId = this->isTrading == true ? GI_EYEBALL_FROG : GI_TUNIC_ZORA;
#endif
        yRange = fabsf(this->actor.yDistToPlayer) + 1.0f;
        xzRange = this->actor.xzDistToPlayer + 1.0f;
        Actor_carry_request_set2(&this->actor, play, getItemId, xzRange, yRange);
    }
}

void kz_carry_end(EnKz* this, PlayState* play) {
#if OOT_VERSION < PAL_1_0
    if (this->interactInfo.talkState == NPC_TALK_STATE_ITEM_GIVEN)
#else
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play))
#endif
    {
        if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_EYEBALL_FROG) {
            total_event_timer_set(180);
            CLEAR_EVENTINF(EVENTINF_MARATHON_ACTIVE);
        }
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        this->actionFunc = kz_matsu;
    }
}

void En_Kz_Actor_move(Actor* thisx, PlayState* play) {
    EnKz* this = (EnKz*)thisx;
    s32 pad;

    if (LINK_IS_ADULT && !GET_INFTABLE(INFTABLE_138)) {
        SET_INFTABLE(INFTABLE_138);
    }
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Skeleton_Info2_anime_play(&this->skelanime);
    kz_eye_paci(this);
    Actor_position_moveF(&this->actor);
#if OOT_VERSION < PAL_1_0
    kz_talk(this, play);
#else
    if (this->actionFunc != kz_carry_end) {
        kz_talk(this, play);
    }
#endif
    this->actionFunc(this, play);
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnKz* this = (EnKz*)thisx;

    if (limbIndex == 8 || limbIndex == 9 || limbIndex == 10) {
        rot->y += sin_s(this->fidgetTableY[limbIndex]) * FIDGET_AMPLITUDE;
        rot->z += cos_s(this->fidgetTableZ[limbIndex]) * FIDGET_AMPLITUDE;
    }
    if (limbIndex) {}
    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnKz* this = (EnKz*)thisx;
    Vec3f mult = { 2600.0f, 0.0f, 0.0f };

    if (limbIndex == 11) {
        Matrix_Position(&mult, &this->actor.focus.pos);
    }
}

void En_Kz_Actor_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = {
        gKzEyeOpenTex,
        gKzEyeHalfTex,
        gKzEyeClosedTex,
    };
    EnKz* this = (EnKz*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_kz.c", 1259);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeIdx]));
    _polygon_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelanime.skeleton, this->skelanime.jointTable, this->skelanime.dListCount,
                          before_display, after_display, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_kz.c", 1281);
}
