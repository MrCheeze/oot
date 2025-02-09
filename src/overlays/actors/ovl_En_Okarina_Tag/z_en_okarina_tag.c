#include "z_fuusa.inc.c"
#include "z_tokinoma_data.inc.c"

/*
 * File: z_en_okarina_tag.c
 * Overlay: ovl_En_Okarina_Tag
 * Description: Music Staff (Ocarina) spot
 */

#include "z_en_okarina_tag.h"
#include "assets/scenes/misc/hakaana_ouke/hakaana_ouke_scene.h"
#include "assets/scenes/overworld/spot02/spot02_scene.h"
#include "terminal.h"
#include "versions.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void En_Okarina_Tag_actor_ct(Actor* thisx, PlayState* play);
void En_Okarina_Tag_actor_dt(Actor* thisx, PlayState* play);
void En_Okarina_Tag_actor_move(Actor* thisx, PlayState* play);

void mode_okarina_nomal_init(EnOkarinaTag* this, PlayState* play);
void mode_hakaana_demo_wait(EnOkarinaTag* this, PlayState* play);
void mode_okarina_demo_init(EnOkarinaTag* this, PlayState* play);
void mode_okarina_nomal_wait(EnOkarinaTag* this, PlayState* play);
void mode_okarina_demo_wait(EnOkarinaTag* this, PlayState* play);
void mode_hakaana_demo_check(EnOkarinaTag* this, PlayState* play);

ActorProfile En_Okarina_Tag_Profile = {
    /**/ ACTOR_EN_OKARINA_TAG,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnOkarinaTag),
    /**/ En_Okarina_Tag_actor_ct,
    /**/ En_Okarina_Tag_actor_dt,
    /**/ En_Okarina_Tag_actor_move,
    /**/ NULL,
};

extern CutsceneData fuusa_data[];
extern CutsceneData tokinoma_sekiban_data[];

void En_Okarina_Tag_actor_dt(Actor* thisx, PlayState* play) {
}

void En_Okarina_Tag_actor_ct(Actor* thisx, PlayState* play) {
    EnOkarinaTag* this = (EnOkarinaTag*)thisx;

    PRINTF("\n\n");
    // "Ocarina tag outbreak"
    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ オカリナタグ発生 ☆☆☆☆☆ %x\n" VT_RST, this->actor.params);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->type = PARAMS_GET_U(this->actor.params, 10, 6);
    this->ocarinaSong = PARAMS_GET_U(this->actor.params, 6, 4);
    this->switchFlag = PARAMS_GET_U(this->actor.params, 0, 6);
    if (this->switchFlag == 0x3F) {
        this->switchFlag = -1;
    }
    if (this->ocarinaSong == 0xF) {
        this->ocarinaSong = 0;
        this->unk_158 = 1;
    }
    this->actor.attentionRangeType = ATTENTION_RANGE_1;
    if (this->actor.world.rot.z > 0) {
        this->interactRange = this->actor.world.rot.z * 40.0f;
    }

    // "Save information"
    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ セーブ情報\t ☆☆☆☆☆ %d\n" VT_RST, this->switchFlag);
    // "Type index"
    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ 種類インデックス ☆☆☆☆☆ %d\n" VT_RST, this->type);
    // "Correct answer information"
    PRINTF(VT_FGCOL(MAGENTA) "☆☆☆☆☆ 正解情報\t ☆☆☆☆☆ %d\n" VT_RST, this->ocarinaSong);
    // "Range information"
    PRINTF(VT_FGCOL(CYAN) "☆☆☆☆☆ 範囲情報\t ☆☆☆☆☆ %d\n" VT_RST, this->actor.world.rot.z);
    // "Processing range information"
    PRINTF(VT_FGCOL(CYAN) "☆☆☆☆☆ 処理範囲情報\t ☆☆☆☆☆ %f\n" VT_RST, this->interactRange);
    // "Hit?"
    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 当り？\t\t ☆☆☆☆☆ %d\n" VT_RST, this->unk_158);
    PRINTF("\n\n");

    if ((this->switchFlag >= 0) && (Actor_Environment_sw_Check(play, this->switchFlag))) {
        Actor_delete(&this->actor);
    } else {
        switch (this->type) {
            case 7:
                this->actionFunc = mode_okarina_nomal_init;
                break;
            case 2:
                if (LINK_IS_ADULT) {
                    Actor_delete(&this->actor);
                    break;
                }
                FALLTHROUGH;
            case 1:
            case 4:
            case 6:
                this->actionFunc = mode_okarina_demo_init;
                break;
            case 5:
                this->actor.textId = 0x5021;
                this->actionFunc = mode_hakaana_demo_wait;
                break;
            default:
                Actor_delete(&this->actor);
                break;
        }
    }
}

void mode_okarina_nomal_init(EnOkarinaTag* this, PlayState* play) {
    Player* player;
    u16 ocarinaSong;

    player = GET_PLAYER(play);
    this->unk_15A++;
    if ((this->switchFlag >= 0) && (Actor_Environment_sw_Check(play, this->switchFlag))) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    } else {
        if ((this->ocarinaSong != 6) || (z_common_data.save.info.scarecrowSpawnSongSet)) {
            if (player->stateFlags2 & PLAYER_STATE2_24) {
                // "North! ! ! ! !"
                PRINTF(VT_FGCOL(RED) "☆☆☆☆☆ 北！！！！！ ☆☆☆☆☆ %f\n" VT_RST, this->actor.xzDistToPlayer);
            }
            if ((this->actor.xzDistToPlayer < (90.0f + this->interactRange)) &&
                (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 80.0f)) {
                if (player->stateFlags2 & PLAYER_STATE2_25) {
                    ocarinaSong = this->ocarinaSong;
                    if (ocarinaSong == 6) {
                        ocarinaSong = 0xA;
                    }
                    player->stateFlags2 |= PLAYER_STATE2_23;
                    ocarina_set(play, ocarinaSong + OCARINA_ACTION_CHECK_SARIA);
                    this->actionFunc = mode_okarina_nomal_wait;
                } else if ((this->actor.xzDistToPlayer < (50.0f + this->interactRange) &&
                            ((fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 40.0f)))) {
                    this->unk_15A = 0;
                    player->unk_6A8 = &this->actor;
                }
            }
        }
    }
}

void mode_okarina_nomal_wait(EnOkarinaTag* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        this->actionFunc = mode_okarina_nomal_init;
    } else {
        if (play->msgCtx.ocarinaMode == OCARINA_MODE_03) {
            if (this->switchFlag >= 0) {
                Actor_Environment_sw_On(play, this->switchFlag);
            }
            if (play->sceneId == SCENE_WATER_TEMPLE) {
                play->msgCtx.msgMode = MSGMODE_PAUSED;
            }
#if OOT_VERSION < NTSC_1_1
            play->msgCtx.ocarinaMode = OCARINA_MODE_04;
#else
            if ((play->sceneId != SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC) &&
                (play->sceneId != SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS)) {
                play->msgCtx.ocarinaMode = OCARINA_MODE_04;
            }
#endif
            Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
            this->actionFunc = mode_okarina_nomal_init;
            return;
        }
        if (this->unk_158 != 0) {
            if ((play->msgCtx.ocarinaMode == OCARINA_MODE_05) || (play->msgCtx.ocarinaMode == OCARINA_MODE_06) ||
                (play->msgCtx.ocarinaMode == OCARINA_MODE_07) || (play->msgCtx.ocarinaMode == OCARINA_MODE_08) ||
                (play->msgCtx.ocarinaMode == OCARINA_MODE_09) || (play->msgCtx.ocarinaMode == OCARINA_MODE_0A) ||
                (play->msgCtx.ocarinaMode == OCARINA_MODE_0D)) {
                if (this->switchFlag >= 0) {
                    Actor_Environment_sw_On(play, this->switchFlag);
                }
                play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
                this->actionFunc = mode_okarina_nomal_init;
                return;
            }
        }
        if ((play->msgCtx.ocarinaMode >= OCARINA_MODE_05) && (play->msgCtx.ocarinaMode < OCARINA_MODE_0E)) {
            play->msgCtx.ocarinaMode = OCARINA_MODE_04;
            this->actionFunc = mode_okarina_nomal_init;
        } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_01) {
            player->stateFlags2 |= PLAYER_STATE2_23;
        }
    }
}

void mode_okarina_demo_init(EnOkarinaTag* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->unk_15A++;
    if ((this->ocarinaSong != 6) || (z_common_data.save.info.scarecrowSpawnSongSet)) {
        if ((this->switchFlag >= 0) && Actor_Environment_sw_Check(play, this->switchFlag)) {
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        } else if (((this->type != 4) || !GET_EVENTCHKINF(EVENTCHKINF_OPENED_DOOR_OF_TIME)) &&
                   ((this->type != 6) || !GET_EVENTCHKINF(EVENTCHKINF_1D)) &&
                   (this->actor.xzDistToPlayer < (90.0f + this->interactRange)) &&
                   (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 80.0f)) {
            if (player->stateFlags2 & PLAYER_STATE2_24) {
                switch (this->type) {
                    case 1:
                        ocarina_set(play, OCARINA_ACTION_CHECK_LULLABY);
                        break;
                    case 2:
                        ocarina_set(play, OCARINA_ACTION_CHECK_STORMS);
                        break;
                    case 4:
                        ocarina_set(play, OCARINA_ACTION_CHECK_TIME);
                        break;
                    case 6:
                        ocarina_set(play, OCARINA_ACTION_CHECK_LULLABY);
                        break;
                    default:
                        // "Ocarina Invisible-kun demo start check error source"
                        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ オカリナ透明君デモ開始チェックエラー原 ☆☆☆☆☆ %d\n" VT_RST,
                               this->type);
                        Actor_delete(&this->actor);
                        break;
                }
                player->stateFlags2 |= PLAYER_STATE2_23;
                this->actionFunc = mode_okarina_demo_wait;
            } else if ((this->actor.xzDistToPlayer < (50.0f + this->interactRange)) &&
                       (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 40.0f)) {
                this->unk_15A = 0;
                player->stateFlags2 |= PLAYER_STATE2_23;
            }
        }
    }
}

void mode_okarina_demo_wait(EnOkarinaTag* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        this->actionFunc = mode_okarina_demo_init;
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_03) {
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        if (this->switchFlag >= 0) {
            Actor_Environment_sw_On(play, this->switchFlag);
        }
        switch (this->type) {
            case 1:
                Actor_Environment_sw_On(play, this->switchFlag);
                SET_EVENTCHKINF(EVENTCHKINF_39);
                break;
            case 2:
                play->csCtx.script = fuusa_data;
                z_common_data.cutsceneTrigger = 1;
                // Increase pitch by 3 semitones i.e. 2^(3/12), scale tempo by same ratio
                // Applies to the windmill bgm once the song of storms fanfare is complete
                Na_SetBgmPitch(1.18921f, 90);
                break;
            case 4:
                play->csCtx.script = tokinoma_sekiban_data;
                z_common_data.cutsceneTrigger = 1;
                break;
            case 6:
                play->csCtx.script = LINK_IS_ADULT ? SEGMENTED_TO_VIRTUAL(gGraveyardTombOpeningAdultCs)
                                                   : SEGMENTED_TO_VIRTUAL(gGraveyardTombOpeningChildCs);
                z_common_data.cutsceneTrigger = 1;
                SET_EVENTCHKINF(EVENTCHKINF_1D);
                Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
                break;
            default:
                break;
        }
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        this->actionFunc = mode_okarina_demo_init;
    } else {
        if (play->msgCtx.ocarinaMode >= OCARINA_MODE_05) {
            if (play->msgCtx.ocarinaMode < OCARINA_MODE_0E) {
                play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                this->actionFunc = mode_okarina_demo_init;
                return;
            }
        }
        if (play->msgCtx.ocarinaMode == OCARINA_MODE_01) {
            player->stateFlags2 |= PLAYER_STATE2_23;
        }
    }
}

void mode_hakaana_demo_wait(EnOkarinaTag* this, PlayState* play) {
    s16 yawDiff;
    s16 yawDiffNew;

    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = mode_hakaana_demo_check;
    } else {
        yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
        this->unk_15A++;
        if (!(this->actor.xzDistToPlayer > 120.0f)) {
            if (CHECK_QUEST_ITEM(QUEST_SONG_SUN)) {
                this->actor.textId = 0x5021;
            }
            yawDiffNew = ABS(yawDiff);
            if (yawDiffNew < 0x4300) {
                this->unk_15A = 0;
                Actor_talk_request2(&this->actor, play, 70.0f);
            }
        }
    }
}

void mode_hakaana_demo_check(EnOkarinaTag* this, PlayState* play) {
    // "Open sesame sesame!"
    PRINTF(VT_FGCOL(MAGENTA) "☆☆☆☆☆ 開けゴマゴマゴマ！ ☆☆☆☆☆ %d\n" VT_RST, message_check(&play->msgCtx));

    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_close(play);
        if (!CHECK_QUEST_ITEM(QUEST_SONG_SUN)) {
            play->csCtx.script = SEGMENTED_TO_VIRTUAL(gSunSongGraveSunSongTeachCs);
            z_common_data.cutsceneTrigger = 1;
        }
        this->actionFunc = mode_hakaana_demo_wait;
    }
}

void En_Okarina_Tag_actor_move(Actor* thisx, PlayState* play) {
    EnOkarinaTag* this = (EnOkarinaTag*)thisx;

    this->actionFunc(this, play);

    if (DEBUG_FEATURES && BREG(0) != 0) {
        if (this->unk_15A != 0) {
            if (!(this->unk_15A & 1)) {
                Debug_Display_new(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                       this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f,
                                       1.0f, 1.0f, 120, 120, 120, 255, 4, play->state.gfxCtx);
            }
        } else {
            Debug_Display_new(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                   this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f,
                                   1.0f, 1.0f, 255, 0, 0, 255, 4, play->state.gfxCtx);
        }
    }
}
