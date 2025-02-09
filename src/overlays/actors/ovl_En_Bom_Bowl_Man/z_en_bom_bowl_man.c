#include "z_en_bom_bowl_man.h"
#include "overlays/actors/ovl_En_Syateki_Niw/z_en_syateki_niw.h"

#include "gfx.h"
#include "gfx_setupdl.h"
#include "one_point_cutscene.h"
#include "rand.h"
#include "regs.h"
#include "segmented_address.h"
#include "sfx.h"
#include "terminal.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/object_bg/object_bg.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_LOCK_ON_DISABLED)

typedef enum BombchuGirlEyeMode {
    /* 0 */ CHU_GIRL_EYES_ASLEEP,
    /* 1 */ CHU_GIRL_EYES_OPEN_SLOWLY,
    /* 2 */ CHU_GIRL_EYES_BLINK_RAPIDLY,
    /* 3 */ CHU_GIRL_EYES_AWAKE
} BombchuGirlEyeMode;

void En_Bom_Bowl_Man_actor_ct(Actor* thisx, PlayState* play2);
void En_Bom_Bowl_Man_actor_dt(Actor* thisx, PlayState* play);
void En_Bom_Bowl_Man_actor_move(Actor* thisx, PlayState* play);
void En_Bom_Bowl_Man_actor_draw(Actor* thisx, PlayState* play);

void mode_sleeping_init(EnBomBowlMan* this, PlayState* play);
void mode_sleeping_check(EnBomBowlMan* this, PlayState* play);
void mode_wakeup_message_check(EnBomBowlMan* this, PlayState* play);
void mode_wakeup_check_init(EnBomBowlMan* this, PlayState* play);
void mode_wakeup_eye_check(EnBomBowlMan* this, PlayState* play);
void mode_wakeup_check(EnBomBowlMan* this, PlayState* play);
void mode_message_no_game_check(EnBomBowlMan* this, PlayState* play);
void mode_jyunbi_check(EnBomBowlMan* this, PlayState* play);
void mode_nomal_mes_check_init(EnBomBowlMan* this, PlayState* play);
void mode_nomal_mes_check(EnBomBowlMan* this, PlayState* play);
static void mode_message_check(EnBomBowlMan* this, PlayState* play);
void mode_next_message_check(EnBomBowlMan* this, PlayState* play);
void mode_bomb_demo(EnBomBowlMan* this, PlayState* play);
void mode_lucky_item_get(EnBomBowlMan* this, PlayState* play);
void mode_last_message_wait(EnBomBowlMan* this, PlayState* play);

ActorProfile En_Bom_Bowl_Man_Profile = {
    /**/ ACTOR_EN_BOM_BOWL_MAN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_BG,
    /**/ sizeof(EnBomBowlMan),
    /**/ En_Bom_Bowl_Man_actor_ct,
    /**/ En_Bom_Bowl_Man_actor_dt,
    /**/ En_Bom_Bowl_Man_actor_move,
    /**/ En_Bom_Bowl_Man_actor_draw,
};

void En_Bom_Bowl_Man_actor_ct(Actor* thisx, PlayState* play2) {
    static f32 niw_atari_set[][3] = { { 16.0f, 46.0f, 0.0f }, { 36.0f, 56.0f, 0.0f } };
    static Vec3f niw_set_pos[] = { { 60, -60, -430 }, { 0, -120, -620 } };
    static f32 niw_scale_data[] = { 0.01f, 0.03f };
    EnBomBowlMan* this = (EnBomBowlMan*)thisx;
    EnSyatekiNiw* cucco;
    s32 i;
    PlayState* play = play2;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gChuGirlSkel, &gChuGirlNoddingOffAnim, this->jointTable,
                       this->morphTable, 11);
    // "☆ Man, my shoulders hurt~ ☆"
    PRINTF(VT_FGCOL(GREEN) "☆ もー 肩こっちゃうよねぇ〜 \t\t ☆ \n" VT_RST);
    // "☆ Isn't there some sort of job that will pay better and be more relaxing? ☆ %d"
    PRINTF(VT_FGCOL(GREEN) "☆ もっとラクしてもうかるバイトないかしら？ ☆ %d\n" VT_RST, play->bombchuBowlingStatus);
    this->posCopy = this->actor.world.pos;
    this->actor.shape.yOffset = -60.0f;
    Actor_set_scale(&this->actor, 0.013f);

    for (i = 0; i < 2; i++) {
        cucco = (EnSyatekiNiw*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_SYATEKI_NIW, niw_set_pos[i].x,
                                           niw_set_pos[i].y, niw_set_pos[i].z, 0, 0, 0, SYATEKI_MINIGAME_ALLEY);
        if (cucco != NULL) {
            cucco->scale = niw_scale_data[i];
            cucco->collider.dim.radius = (s16)niw_atari_set[i][0];
            cucco->collider.dim.height = (s16)niw_atari_set[i][1];
        }
    }

    this->prizeSelect = (s16)rnd_f(4.99f);
    this->actor.attentionRangeType = ATTENTION_RANGE_1;
    this->actionFunc = mode_sleeping_init;
}

void En_Bom_Bowl_Man_actor_dt(Actor* thisx, PlayState* play) {
}

void mode_sleeping_init(EnBomBowlMan* this, PlayState* play) {
    this->frameCount = (f32)Si2_anime_end_frame(&gChuGirlNoddingOffAnim);
    Skeleton_Info2_init(&this->skelAnime, &gChuGirlNoddingOffAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_LOOP, -10.0f);
    this->actor.textId = 0xC0;
    this->dialogState = TEXT_STATE_EVENT;
    this->actionFunc = mode_sleeping_check;
}

void mode_sleeping_check(EnBomBowlMan* this, PlayState* play) {
    s16 yawDiff;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = mode_wakeup_message_check;
    } else {
        yawDiff = ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y));

        if (!(this->actor.xzDistToPlayer > 120.0f) && (yawDiff < 0x4300)) {
            Actor_talk_request2(&this->actor, play, 120.0f);
        }
    }
}

void mode_wakeup_message_check(EnBomBowlMan* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((message_check(&play->msgCtx) == this->dialogState) && pad_on_check(play)) {
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        this->actionFunc = mode_wakeup_check_init;
    }
}

void mode_wakeup_check_init(EnBomBowlMan* this, PlayState* play) {
    this->frameCount = (f32)Si2_anime_end_frame(&gChuGirlWakeUpAnim);
    Skeleton_Info2_init(&this->skelAnime, &gChuGirlWakeUpAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_ONCE, -10.0f);
    this->eyeMode = CHU_GIRL_EYES_OPEN_SLOWLY;
    this->actionFunc = mode_wakeup_eye_check;
}

void mode_wakeup_eye_check(EnBomBowlMan* this, PlayState* play) {
    f32 frameCount = this->skelAnime.curFrame;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (frameCount == 30.0f) {
        this->dialogState = TEXT_STATE_EVENT;

        // Check for beaten Dodongo's Cavern
        if (GET_EVENTCHKINF(EVENTCHKINF_25) || BREG(2)) {
            this->actor.textId = 0xBF;
        } else {
            this->actor.textId = 0x7058;
        }
    }
    message_set2(play, this->actor.textId);

    if ((this->eyeTextureIndex == 0) && (this->eyeMode == CHU_GIRL_EYES_BLINK_RAPIDLY) && (this->blinkTimer == 0)) {
        // Blink twice, then move on
        this->eyeTextureIndex = 2;
        this->blinkCount++;
        if (this->blinkCount >= 3) {
            this->actionFunc = mode_wakeup_check;
        }
    }
}

void mode_wakeup_check(EnBomBowlMan* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((message_check(&play->msgCtx) == this->dialogState) && pad_on_check(play)) {
        message_close(play);
        this->frameCount = (f32)Si2_anime_end_frame(&gChuGirlLeanOverCounterAnim);
        Skeleton_Info2_init(&this->skelAnime, &gChuGirlLeanOverCounterAnim, 1.0f, 0.0f, this->frameCount, ANIMMODE_LOOP,
                         -10.0f);
        this->eyeMode = CHU_GIRL_EYES_AWAKE;
        this->blinkTimer = (s16)rnd_f(60.0f) + 20;

        // Check for beaten Dodongo's Cavern
        if (!(GET_EVENTCHKINF(EVENTCHKINF_25) || BREG(2))) {
            this->actionFunc = mode_message_no_game_check;
        } else {
            this->actor.textId = 0x18;
            this->dialogState = TEXT_STATE_CHOICE;
            message_set2(play, this->actor.textId);
            this->actionFunc = mode_message_check;
        }
    }
}

void mode_message_no_game_check(EnBomBowlMan* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = mode_jyunbi_check;
    } else {
        Actor_talk_request2(&this->actor, play, 120.0f);
    }
}

void mode_jyunbi_check(EnBomBowlMan* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((message_check(&play->msgCtx) == this->dialogState) && pad_on_check(play)) {
        message_close(play);
        this->actionFunc = mode_message_no_game_check;
    }
}

void mode_nomal_mes_check_init(EnBomBowlMan* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->minigamePlayStatus == 0) {
        if (!this->startedPlaying) {
            this->actor.textId = 0x18;
        } else {
            this->actor.textId = 0x1A;
        }

        this->dialogState = TEXT_STATE_CHOICE;
    } else {
        this->actor.textId = 0x19;
        this->dialogState = TEXT_STATE_EVENT;
    }

    this->actionFunc = mode_nomal_mes_check;
}

void mode_nomal_mes_check(EnBomBowlMan* this, PlayState* play) {
    s16 yawDiff;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (BREG(3)) {
        PRINTF(VT_FGCOL(RED) "☆ game_play->bomchu_game_flag ☆ %d\n" VT_RST, play->bombchuBowlingStatus);
        // "HOW'S THE FIRST WALL DOING?"
        PRINTF(VT_FGCOL(RED) "☆ 壁１の状態どう？ ☆ %d\n" VT_RST, this->wallStatus[0]);
        // "HOW'S THE SECOND WALL DOING?"
        PRINTF(VT_FGCOL(RED) "☆ 壁２の状態どう？ ☆ %d\n" VT_RST, this->wallStatus[1]);
        // "HOLE INFORMATION"
        PRINTF(VT_FGCOL(RED) "☆ 穴情報\t     ☆ %d\n" VT_RST, this->bowlPit->status);
        PRINTF("\n\n");
    }

    this->gameResult = 0;

    if (this->bowlPit != NULL) {
        if ((this->wallStatus[0] != 1) && (this->wallStatus[1] != 1) && (this->bowlPit->status == 2)) {
            this->gameResult = 1; // Won
            this->bowlPit->status = 0;
            // "Center HIT!"
            PRINTF(VT_FGCOL(MAGENTA) "☆☆☆☆☆ 中央ＨＩＴ！！！！ ☆☆☆☆☆ \n" VT_RST);
        }

        if ((play->bombchuBowlingStatus == -1) && (play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].length == 0) &&
            (this->bowlPit->status == 0) && (this->wallStatus[0] != 1) && (this->wallStatus[1] != 1)) {
            this->gameResult = 2; // Lost
            // "Bombchu lost"
            PRINTF(VT_FGCOL(MAGENTA) "☆☆☆☆☆ ボムチュウ消化 ☆☆☆☆☆ \n" VT_RST);
        }
    }

    if (this->gameResult != 0) { // won or lost
        this->actor.textId = 0x1A;
        this->dialogState = TEXT_STATE_CHOICE;
        this->minigamePlayStatus = 0;

        if ((this->exItem != NULL) && (this->exItem->actor.update != NULL)) {
            this->exItem->killItem = true;
            this->exItem = NULL;
        }

        play->bombchuBowlingStatus = 0;
        this->playingAgain = true;
        message_set(play, this->actor.textId, NULL);

        if (this->gameResult == 2) {
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
        }
        this->actionFunc = mode_message_check;
    } else {
        if (Actor_talk_check(&this->actor, play)) {
            if (this->minigamePlayStatus == 0) {
                this->actionFunc = mode_message_check;
            } else {
                this->actionFunc = mode_next_message_check;
            }
        } else {
            yawDiff = ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y));

            if (!(this->actor.xzDistToPlayer > 120.0f) && (yawDiff < 0x4300)) {
                Actor_talk_request2(&this->actor, play, 120.0f);
            }
        }
    }
}

static void mode_message_check(EnBomBowlMan* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((message_check(&play->msgCtx) == this->dialogState) && pad_on_check(play)) {
        message_close(play);

        switch (play->msgCtx.choiceIndex) {
            case 0: // Yes
                if (z_common_data.save.info.playerData.rupees >= 30) {
                    lupy_increase(-30);
                    this->minigamePlayStatus = 1;
                    this->wallStatus[0] = this->wallStatus[1] = 0;
                    play->bombchuBowlingStatus = 10;
                    Actor_Environment_sw_On(play, 0x38);

                    if (!this->startedPlaying && !this->playingAgain) {
                        this->actor.textId = 0x19;
                        message_set2(play, this->actor.textId);
                        this->dialogState = TEXT_STATE_EVENT;
                        this->actionFunc = mode_next_message_check;
                    } else {
                        this->actor.textId = 0x1B;
                        message_set2(play, this->actor.textId);
                        this->dialogState = TEXT_STATE_EVENT;
                        makeOnepointDemo(play, 8010, -99, NULL, CAM_ID_MAIN);
                        player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
                        this->actionFunc = mode_bomb_demo;
                    }
                } else {
                    this->playingAgain = false;
                    this->actor.textId = 0x85;
                    message_set2(play, this->actor.textId);
                    this->dialogState = TEXT_STATE_EVENT;
                    this->actionFunc = mode_next_message_check;
                }
                break;

            case 1: // No
                this->playingAgain = false;
                this->actor.textId = 0x2D;
                message_set2(play, this->actor.textId);
                this->dialogState = TEXT_STATE_EVENT;
                this->actionFunc = mode_next_message_check;
        }
    }
}

void mode_next_message_check(EnBomBowlMan* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((message_check(&play->msgCtx) == this->dialogState) && pad_on_check(play)) {
        message_close(play);

        if (((this->actor.textId == 0x2D) || (this->actor.textId == 0x85)) && Actor_Environment_sw_Check(play, 0x38)) {
            Actor_Environment_sw_Off(play, 0x38);
        }

        if (this->minigamePlayStatus == 1) {
            this->actor.textId = 0x1B;
            message_set2(play, this->actor.textId);
            this->dialogState = TEXT_STATE_EVENT;
            makeOnepointDemo(play, 8010, -99, NULL, CAM_ID_MAIN);
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            this->actionFunc = mode_bomb_demo;
        } else {
            if (this->gameResult == 2) {
                player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
            }
            this->actionFunc = mode_nomal_mes_check_init;
        }
    }
}

void mode_bomb_demo(EnBomBowlMan* this, PlayState* play) {
    Vec3f accel = { 0.0f, 0.1f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((message_check(&play->msgCtx) == this->dialogState) && pad_on_check(play)) {
        pos.x = 148.0f;
        pos.y = 40.0f;
        pos.z = 300.0f;
        Effect_SS_Bomb2_2_ct(play, &pos, &velocity, &accel, 50, 15);
        Actor_SE_set(&this->actor, NA_SE_IT_GOODS_APPEAR);
        this->prizeRevealTimer = 10;
        this->actionFunc = mode_lucky_item_get;
    }
}

static Vec3f bom_set_pos[] = {
    { 0.0f, 22.0f, 0.0f }, { 0.0f, 22.0f, 0.0f }, { 0.0f, 8.0f, 0.0f }, { 0.0f, 9.0f, 0.0f }, { 0.0f, -2.0f, 0.0f },
};

static s16 bom_angle_data[] = { 0x4268, 0x4268, -0x03E8, 0x0000, 0x4268, 0x0000 };

void mode_lucky_item_get(EnBomBowlMan* this, PlayState* play) {
    s16 prizeTemp;
    s32 pad;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->prizeRevealTimer == 0) {
        switch (this->prizeSelect) {
            case 0:
                prizeTemp = EXITEM_BOMB_BAG_BOWLING;
                if (GET_ITEMGETINF(ITEMGETINF_11)) {
                    prizeTemp = EXITEM_PURPLE_RUPEE_BOWLING;
                }
                break;
            case 1:
                prizeTemp = EXITEM_PURPLE_RUPEE_BOWLING;
                break;
            case 2:
                prizeTemp = EXITEM_BOMBCHUS_BOWLING;
                break;
            case 3:
                prizeTemp = EXITEM_HEART_PIECE_BOWLING;
                if (GET_ITEMGETINF(ITEMGETINF_12)) {
                    prizeTemp = EXITEM_PURPLE_RUPEE_BOWLING;
                }
                break;
            case 4:
                prizeTemp = EXITEM_BOMBS_BOWLING;
                break;
        }

        this->prizeIndex = prizeTemp;

        if (BREG(7)) {
            this->prizeIndex = BREG(7) - 1;
        }

        this->exItem = (EnExItem*)Actor_info_make_child_actor(
            &play->actorCtx, &this->actor, play, ACTOR_EN_EX_ITEM, bom_set_pos[this->prizeIndex].x + 148.0f,
            bom_set_pos[this->prizeIndex].y + 40.0f, bom_set_pos[this->prizeIndex].z + 300.0f, 0,
            bom_angle_data[this->prizeIndex], 0, this->prizeIndex + EXITEM_BOMB_BAG_COUNTER);

        if (!this->startedPlaying) {
            this->bowlPit = (EnBomBowlPit*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play,
                                                              ACTOR_EN_BOM_BOWL_PIT, 0.0f, 90.0f, -860.0f, 0, 0, 0, 0);
            if (this->bowlPit != NULL) {
                this->bowlPit->prizeIndex = this->prizeIndex;
            }
        } else {
            this->bowlPit->prizeIndex = this->prizeIndex;
        }

        this->bowlPit->start = 1;
        this->minigamePlayStatus = 2;
        this->actor.textId = 0x405A;
        message_set2(play, this->actor.textId);
        this->dialogState = TEXT_STATE_EVENT;

        // Cycle through prizes in order
        this->prizeSelect++;
        if (this->prizeSelect >= 5) {
            this->prizeSelect = 0;
        }
        this->actionFunc = mode_last_message_wait;
    }
}

void mode_last_message_wait(EnBomBowlMan* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((message_check(&play->msgCtx) == this->dialogState) && pad_on_check(play)) {
        message_close(play);
        restartCameraStoped(GET_ACTIVE_CAM(play));
        this->startedPlaying = true;

        if (BREG(2)) {
            BREG(2) = 0;
        }

        // "Wow"
        PRINTF(VT_FGCOL(YELLOW) "☆ わー ☆ %d\n" VT_RST, play->bombchuBowlingStatus);
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
        this->actionFunc = mode_nomal_mes_check_init;
    }
}

void En_Bom_Bowl_Man_actor_move(Actor* thisx, PlayState* play) {
    EnBomBowlMan* this = (EnBomBowlMan*)thisx;

    this->timer++;
    this->actor.focus.pos.y = 60.0f;
    Actor_world_to_eye(&this->actor, 60.0f);

    switch (this->eyeMode) {
        case CHU_GIRL_EYES_ASLEEP:
            this->eyeTextureIndex = 2;
            break;
        case CHU_GIRL_EYES_OPEN_SLOWLY:
            if (this->eyeTextureIndex > 0) {
                this->eyeTextureIndex--;
            } else {
                this->blinkTimer = 30;
                this->eyeMode = CHU_GIRL_EYES_BLINK_RAPIDLY;
            }
            break;
        case CHU_GIRL_EYES_BLINK_RAPIDLY:
            if ((this->blinkTimer == 0) && (this->eyeTextureIndex > 0)) {
                this->eyeTextureIndex--;
            }
            break;
        default:
            if (this->blinkTimer == 0) {
                this->eyeTextureIndex++;
                if (this->eyeTextureIndex >= 3) {
                    this->eyeTextureIndex = 0;
                    this->blinkTimer = (s16)rnd_f(60.0f) + 20;
                }
            }

            eye_move2(play, &this->actor, &this->unk_218, &this->unk_224, this->actor.focus.pos);
            break;
    }

    DECR(this->prizeRevealTimer);
    DECR(this->blinkTimer);

    this->actionFunc(this, play);
}

s32 En_Bom_Bowl_Man_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnBomBowlMan* this = (EnBomBowlMan*)thisx;

    if (limbIndex == 4) { // head
        rot->x += this->unk_218.y;
        rot->z += this->unk_218.z;
    }

    return false;
}

void En_Bom_Bowl_Man_actor_draw(Actor* thisx, PlayState* play) {
    static void* bg_eye_txt[] = { gChuGirlEyeOpenTex, gChuGirlEyeHalfTex, gChuGirlEyeClosedTex };
    EnBomBowlMan* this = (EnBomBowlMan*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_bom_bowl_man.c", 907);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(bg_eye_txt[this->eyeTextureIndex]));
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          En_Bom_Bowl_Man_draw_sub, NULL, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_bom_bowl_man.c", 923);
}
