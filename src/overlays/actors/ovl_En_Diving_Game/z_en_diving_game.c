/*
 * File: z_en_diving_game.c
 * Overlay: ovl_En_Diving_Game
 * Description: Diving minigame
 */

#include "z_en_diving_game.h"
#include "overlays/actors/ovl_En_Ex_Ruppy/z_en_ex_ruppy.h"
#include "assets/objects/object_zo/object_zo.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Diving_Game_actor_ct(Actor* thisx, PlayState* play);
void En_Diving_Game_actor_dt(Actor* thisx, PlayState* play);
void En_Diving_Game_actor_move(Actor* thisx, PlayState* play2);
void En_Diving_Game_actor_draw(Actor* thisx, PlayState* play);

static void mode_start_demo_init(EnDivingGame* this, PlayState* play);
void mode_start_message_set(EnDivingGame* this, PlayState* play);
void mode_select_wait(EnDivingGame* this, PlayState* play);
static void mode_message_check(EnDivingGame* this, PlayState* play);
void mode_anime_change_init(EnDivingGame* this, PlayState* play);
void mode_anime_change(EnDivingGame* this, PlayState* play);
void mode_camera_demo_init(EnDivingGame* this, PlayState* play);
void mode_camera_demo(EnDivingGame* this, PlayState* play);
void mode_baramaki_wait(EnDivingGame* this, PlayState* play);
void mode_water_stop_wait(EnDivingGame* this, PlayState* play);
static void mode_start_message_check(EnDivingGame* this, PlayState* play);
void mode_game_play_wait(EnDivingGame* this, PlayState* play);
void mode_all_get_happy_wait(EnDivingGame* this, PlayState* play);
static void mode_player_item_request(EnDivingGame* this, PlayState* play);
static void mode_player_item_up(EnDivingGame* this, PlayState* play);
void mode_player_item_up_two(EnDivingGame* this, PlayState* play);

ActorProfile En_Diving_Game_Profile = {
    /**/ ACTOR_EN_DIVING_GAME,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ZO,
    /**/ sizeof(EnDivingGame),
    /**/ En_Diving_Game_actor_ct,
    /**/ En_Diving_Game_actor_dt,
    /**/ En_Diving_Game_actor_move,
    /**/ En_Diving_Game_actor_draw,
};

// used to ensure there's only one instance of this actor.
static u8 izure_sakujyo = false;

static ColliderCylinderInit OcInfoData = {
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
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 10, 10, 0, { 0, 0, 0 } },
};

static void* eye_txt[] = {
    gZoraEyeOpenTex,
    gZoraEyeHalfTex,
    gZoraEyeClosedTex,
};

void En_Diving_Game_actor_ct(Actor* thisx, PlayState* play) {
    EnDivingGame* this = (EnDivingGame*)thisx;

    this->actor.gravity = -3.0f;
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gZoraSkel, &gZoraIdleAnim, this->jointTable, this->morphTable, 20);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 素もぐりＧＯ ☆☆☆☆☆ \n" VT_RST);
    this->actor.room = -1;
    this->actor.scale.x = 0.01f;
    this->actor.scale.y = 0.012999999f;
    this->actor.scale.z = 0.0139999995f;
    if (izure_sakujyo) {
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ もういてる原 ☆☆☆☆☆ \n" VT_RST);
        this->unk_31F = 1;
        Actor_delete(&this->actor);
    } else {
        izure_sakujyo = true;
        this->actor.attentionRangeType = ATTENTION_RANGE_0;
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        this->actionFunc = mode_start_demo_init;
    }
}

void En_Diving_Game_actor_dt(Actor* thisx, PlayState* play) {
    EnDivingGame* this = (EnDivingGame*)thisx;

    if (this->unk_31F == 0) {
        z_common_data.timerState = TIMER_STATE_OFF;
    }
    ClObjPipe_dt(play, &this->collider);
}

void Ruppy_set(EnDivingGame* this, PlayState* play) {
    EnExRuppy* rupee;
    Vec3f rupeePos;

    rupeePos.x = (fqrand() - 0.5f) * 30.0f + this->actor.world.pos.x;
    rupeePos.y = (fqrand() - 0.5f) * 20.0f + (this->actor.world.pos.y + 30.0f);
    rupeePos.z = (fqrand() - 0.5f) * 20.0f + this->actor.world.pos.z;
    rupee = (EnExRuppy*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_EX_RUPPY, rupeePos.x,
                                           rupeePos.y, rupeePos.z, 0, (s16)rnd_fx(3500.0f) - 1000,
                                           this->rupeesLeftToThrow, 0);
    if (rupee != NULL) {
        rupee->actor.speed = 12.0f;
        rupee->actor.velocity.y = 6.0f;
    }
}

s32 Game_end_check(EnDivingGame* this, PlayState* play) {
    s32 rupeesNeeded;

    if ((z_common_data.timerState == TIMER_STATE_STOP) && !Game_play_demo_mode_check(play)) {
        // Failed.
        z_common_data.timerState = TIMER_STATE_OFF;
        Na_StopMiddleBossBgm();
        Na_StartSystemSe_F(NA_SE_SY_FOUND);
        this->actor.textId = 0x71AD;
        message_set(play, this->actor.textId, NULL);
        this->unk_292 = TEXT_STATE_EVENT;
        this->allRupeesThrown = this->state = this->phase = this->unk_2A2 = this->grabbedRupeesCounter = 0;
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
        this->actionFunc = mode_message_check;
        return true;
    } else {
        rupeesNeeded = 5;

        if (GET_EVENTCHKINF(EVENTCHKINF_38)) {
            rupeesNeeded = 10;
        }
        if (this->grabbedRupeesCounter >= rupeesNeeded) {
            // Won.
            z_common_data.timerState = TIMER_STATE_OFF;
            this->allRupeesThrown = this->state = this->phase = this->unk_2A2 = this->grabbedRupeesCounter = 0;
            if (!GET_EVENTCHKINF(EVENTCHKINF_38)) {
                this->actor.textId = 0x4055;
            } else {
                this->actor.textId = 0x405D;
                if (this->extraWinCount < 100) {
                    this->extraWinCount++;
                }
            }
            message_set(play, this->actor.textId, NULL);
            this->unk_292 = TEXT_STATE_EVENT;
            Na_StopMiddleBossBgm();
            Na_StartFanfare(NA_BGM_SMALL_ITEM_GET);
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            if (!GET_EVENTCHKINF(EVENTCHKINF_38)) {
                this->actionFunc = mode_all_get_happy_wait;
            } else {
                this->actionFunc = mode_message_check;
            }
            return true;
        }
    }
    return false;
}

// EnDivingGame_FinishMinigame ? // Reset probably
static void mode_start_demo_init(EnDivingGame* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&gZoraIdleAnim);

    Skeleton_Info2_init(&this->skelAnime, &gZoraIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    this->notPlayingMinigame = true;
    this->actionFunc = mode_start_message_set;
}

void mode_start_message_set(EnDivingGame* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->state != ENDIVINGGAME_STATE_PLAYING || !Game_end_check(this, play)) {
        if (Actor_talk_check(&this->actor, play)) {
            if (this->unk_292 != TEXT_STATE_DONE) {
                switch (this->state) {
                    case ENDIVINGGAME_STATE_NOTPLAYING:
                        player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
                        this->actionFunc = mode_select_wait;
                        break;
                    case ENDIVINGGAME_STATE_AWARDPRIZE:
                        this->actionFunc = mode_player_item_request;
                        break;
                    case ENDIVINGGAME_STATE_PLAYING:
                        this->actionFunc = mode_game_play_wait;
                        break;
                }
            }
        } else {
            if (get_mask_message(play, MASK_REACTION_SET_ZORA) != 0) {
                this->actor.textId = get_mask_message(play, MASK_REACTION_SET_ZORA);
                this->unk_292 = TEXT_STATE_DONE;
            } else {
                switch (this->state) {
                    case ENDIVINGGAME_STATE_NOTPLAYING:
                        this->unk_292 = TEXT_STATE_CHOICE;
                        if (!GET_EVENTCHKINF(EVENTCHKINF_38)) {
                            this->actor.textId = 0x4053;
                            this->phase = ENDIVINGGAME_PHASE_1;
                        } else {
                            this->actor.textId = 0x405C;
                            this->phase = ENDIVINGGAME_PHASE_2;
                        }
                        break;
                    case ENDIVINGGAME_STATE_AWARDPRIZE:
                        this->actor.textId = 0x4056;
                        this->unk_292 = TEXT_STATE_EVENT;
                        break;
                    case ENDIVINGGAME_STATE_PLAYING:
                        this->actor.textId = 0x405B;
                        this->unk_292 = TEXT_STATE_EVENT;
                        break;
                }
            }
            Actor_talk_request2(&this->actor, play, 80.0f);
        }
    }
}

void mode_select_wait(EnDivingGame* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->unk_292 == message_check(&play->msgCtx) &&
        pad_on_check(play)) { // Did the player select an answer?
        switch (play->msgCtx.choiceIndex) {
            case 0: // Yes
                if (z_common_data.save.info.playerData.rupees >= 20) {
                    lupy_increase(-20);
                    this->actor.textId = 0x4054;
                } else {
                    this->actor.textId = 0x85;
                    this->allRupeesThrown = this->state = this->phase = this->unk_2A2 = this->grabbedRupeesCounter = 0;
                }
                break;
            case 1: // No
                this->actor.textId = 0x2D;
                this->allRupeesThrown = this->state = this->phase = this->unk_2A2 = this->grabbedRupeesCounter = 0;
                break;
        }
        if (!GET_EVENTCHKINF(EVENTCHKINF_38) || this->actor.textId == 0x85 || this->actor.textId == 0x2D) {
            message_set2(play, this->actor.textId);
            this->unk_292 = TEXT_STATE_EVENT;
            this->actionFunc = mode_message_check;
        } else {
            play->msgCtx.msgMode = MSGMODE_PAUSED;
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            this->actionFunc = mode_anime_change_init;
        }
    }
}

// Waits for the message to close
static void mode_message_check(EnDivingGame* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->unk_292 == message_check(&play->msgCtx) && pad_on_check(play)) {
        if (this->phase == ENDIVINGGAME_PHASE_ENDED) {
            message_close(play);
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
            this->actionFunc = mode_start_demo_init;
        } else {
            play->msgCtx.msgMode = MSGMODE_PAUSED;
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            this->actionFunc = mode_anime_change_init;
        }
    }
}

// another "start minigame" step
void mode_anime_change_init(EnDivingGame* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&gZoraThrowRupeesAnim);

    Skeleton_Info2_init(&this->skelAnime, &gZoraThrowRupeesAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_ONCE, -10.0f);
    this->notPlayingMinigame = false;
    this->actionFunc = mode_anime_change;
}

// Wait a bit before start throwing the rupees.
void mode_anime_change(EnDivingGame* this, PlayState* play) {
    f32 currentFrame = this->skelAnime.curFrame;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (currentFrame >= 15.0f) {
        this->actionFunc = mode_camera_demo_init;
    }
}

void mode_camera_demo_init(EnDivingGame* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->subCamId = Gama_play_make_camera(play);
    Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
    Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
    this->spawnRuppyTimer = 10;
    this->subCamAtNext.x = -210.0f;
    this->subCamAtNext.y = -80.0f;
    this->subCamAtNext.z = -1020.0f;
    this->subCamEyeNext.x = -280.0f;
    this->subCamEyeNext.y = -20.0f;
    this->subCamEyeNext.z = -240.0f;
    if (!GET_EVENTCHKINF(EVENTCHKINF_38)) {
        this->rupeesLeftToThrow = 5;
    } else {
        this->rupeesLeftToThrow = 10;
    }
    this->subCamEyeMaxVelFrac.x = this->subCamEyeMaxVelFrac.y = this->subCamEyeMaxVelFrac.z =
        this->subCamAtMaxVelFrac.x = this->subCamAtMaxVelFrac.y = this->subCamAtMaxVelFrac.z = 0.1f;
    this->subCamAt.x = play->view.at.x;
    this->subCamAt.y = play->view.at.y;
    this->subCamAt.z = play->view.at.z;
    this->subCamEye.x = play->view.eye.x;
    this->subCamEye.y = play->view.eye.y + 80.0f;
    this->subCamEye.z = play->view.eye.z + 250.0f;
    this->subCamEyeVel.x = fabsf(this->subCamEye.x - this->subCamEyeNext.x) * 0.04f;
    this->subCamEyeVel.y = fabsf(this->subCamEye.y - this->subCamEyeNext.y) * 0.04f;
    this->subCamEyeVel.z = fabsf(this->subCamEye.z - this->subCamEyeNext.z) * 0.04f;
    this->subCamAtVel.x = fabsf(this->subCamAt.x - this->subCamAtNext.x) * 0.04f;
    this->subCamAtVel.y = fabsf(this->subCamAt.y - this->subCamAtNext.y) * 0.04f;
    this->subCamAtVel.z = fabsf(this->subCamAt.z - this->subCamAtNext.z) * 0.04f;
    Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    Gama_play_set_camera_fovy(play, this->subCamId, play->mainCamera.fov);
    this->subCamTimer = 60;
    this->actionFunc = mode_camera_demo;
    this->subCamVelFactor = 0.0f;
}

// Throws rupee when this->spawnRuppyTimer == 0
void mode_camera_demo(EnDivingGame* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Gama_play_position_in_water(play, &this->actor.projectedPos)) {
        Na_SetWaterSeModeFlag(0);
    }
    if (this->subCamId != SUB_CAM_ID_DONE) {
        add_calc2(&this->subCamEye.x, this->subCamEyeNext.x, this->subCamEyeMaxVelFrac.x,
                       this->subCamEyeVel.x * this->subCamVelFactor);
        add_calc2(&this->subCamEye.z, this->subCamEyeNext.z, this->subCamEyeMaxVelFrac.z,
                       this->subCamEyeVel.z * this->subCamVelFactor);
        add_calc2(&this->subCamAt.x, this->subCamAtNext.x, this->subCamAtMaxVelFrac.x,
                       this->subCamAtVel.x * this->subCamVelFactor);
        add_calc2(&this->subCamAt.y, this->subCamAtNext.y, this->subCamAtMaxVelFrac.y,
                       this->subCamAtVel.y * this->subCamVelFactor);
        add_calc2(&this->subCamAt.z, this->subCamAtNext.z, this->subCamAtMaxVelFrac.z,
                       this->subCamAtVel.z * this->subCamVelFactor);
        add_calc2(&this->subCamVelFactor, 1.0f, 1.0f, 0.02f);
    }
    Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    if (!this->allRupeesThrown && this->spawnRuppyTimer == 0) {
        this->spawnRuppyTimer = 5;
        Ruppy_set(this, play);
        this->rupeesLeftToThrow--;
        if (!GET_EVENTCHKINF(EVENTCHKINF_38)) {
            this->unk_296 = 30;
        } else {
            this->unk_296 = 5;
        }
        if (this->rupeesLeftToThrow <= 0) {
            this->rupeesLeftToThrow = 0;
            this->allRupeesThrown = true;
        }
    }
    if (this->subCamTimer == 0 || ((fabsf(this->subCamEye.x - this->subCamEyeNext.x) < 2.0f) &&
                                   (fabsf(this->subCamEye.y - this->subCamEyeNext.y) < 2.0f) &&
                                   (fabsf(this->subCamEye.z - this->subCamEyeNext.z) < 2.0f) &&
                                   (fabsf(this->subCamAt.x - this->subCamAtNext.x) < 2.0f) &&
                                   (fabsf(this->subCamAt.y - this->subCamAtNext.y) < 2.0f) &&
                                   (fabsf(this->subCamAt.z - this->subCamAtNext.z) < 2.0f))) {
        if (this->unk_2A2 != 0) {
            this->subCamTimer = 70;
            this->unk_2A2 = 2;
            this->actionFunc = mode_water_stop_wait;
        } else {
            this->actionFunc = mode_baramaki_wait;
        }
    }
}

// Called just before changing the camera to focus the underwater rupees.
void mode_baramaki_wait(EnDivingGame* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->unk_296 == 0) {
        this->unk_2A2 = 1;
        this->subCamTimer = 100;
        this->actionFunc = mode_camera_demo;
        this->subCamAt.x = this->subCamAtNext.x = -210.0f;
        this->subCamAt.y = this->subCamAtNext.y = -80.0f;
        this->subCamAt.z = this->subCamAtNext.z = -1020.0f;
        this->subCamEye.x = this->subCamEyeNext.x = -280.0f;
        this->subCamEye.y = this->subCamEyeNext.y = -20.0f;
        this->subCamEye.z = this->subCamEyeNext.z = -240.0f;
    }
}

// EnDivingGame_SayStartAndWait ?
void mode_water_stop_wait(EnDivingGame* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->subCamTimer == 0) {
        Gama_play_clear_camera(play, this->subCamId);
        Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_ACTIVE);
        this->actor.textId = 0x405A;
        message_set2(play, this->actor.textId);
        this->unk_292 = TEXT_STATE_EVENT;
        this->actionFunc = mode_start_message_check;
    }
}

// mode_start_message_setDuringMinigame
static void mode_start_message_check(EnDivingGame* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->unk_292 == message_check(&play->msgCtx) && pad_on_check(play)) {
        message_close(play);
        if (!GET_EVENTCHKINF(EVENTCHKINF_38)) {
            event_timer_set(50 + BREG(2));
        } else {
            event_timer_set(50 + BREG(2));
        }
        Na_StartMiddleBossBgm(NA_BGM_TIMED_MINI_GAME);
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
        this->actor.textId = 0x405B;
        this->unk_292 = TEXT_STATE_EVENT;
        this->state = ENDIVINGGAME_STATE_PLAYING;
        this->actionFunc = mode_start_message_set;
    }
}

void mode_game_play_wait(EnDivingGame* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((this->unk_292 == message_check(&play->msgCtx) && pad_on_check(play))) {
        message_close(play);
        this->actionFunc = mode_start_message_set;
    } else {
        Game_end_check(this, play);
    }
}

// EnDivingGame_SayCongratsAndWait ? // EnDivingGame_PlayerWonPhase1
void mode_all_get_happy_wait(EnDivingGame* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((this->unk_292 == message_check(&play->msgCtx) && pad_on_check(play))) {
        message_close(play);
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
        this->actor.textId = 0x4056;
        this->unk_292 = TEXT_STATE_EVENT;
        this->state = ENDIVINGGAME_STATE_AWARDPRIZE;
        this->actionFunc = mode_start_message_set;
    }
}

static void mode_player_item_request(EnDivingGame* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((this->unk_292 == message_check(&play->msgCtx) && pad_on_check(play))) {
        message_close(play);
        this->actor.parent = NULL;
        Actor_carry_request_set2(&this->actor, play, GI_SCALE_SILVER, 90.0f, 10.0f);
        this->actionFunc = mode_player_item_up;
    }
}

static void mode_player_item_up(EnDivingGame* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Actor_carry_check(&this->actor, play)) {
        this->actionFunc = mode_player_item_up_two;
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_SCALE_SILVER, 90.0f, 10.0f);
    }
}

// Award the scale?
void mode_player_item_up_two(EnDivingGame* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (message_check(&play->msgCtx) == TEXT_STATE_DONE && pad_on_check(play)) {
        // "Successful completion"
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 正常終了 ☆☆☆☆☆ \n" VT_RST);
        this->allRupeesThrown = this->state = this->phase = this->unk_2A2 = this->grabbedRupeesCounter = 0;
        SET_EVENTCHKINF(EVENTCHKINF_38);
        this->actionFunc = mode_start_demo_init;
    }
}

void En_Diving_Game_actor_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnDivingGame* this = (EnDivingGame*)thisx;
    Player* player = GET_PLAYER(play);
    Vec3f pos;

    if (this->subCamTimer != 0) {
        this->subCamTimer--;
    }
    if (this->unk_296 != 0) {
        this->unk_296--;
    }
    if (this->eyeTimer != 0) {
        this->eyeTimer--;
    }
    if (this->spawnRuppyTimer != 0) {
        this->spawnRuppyTimer--;
    }

    if (1) {}

    if (z_common_data.timerSeconds == 10) {
        Na_SetMiniGameBgmTempoUp();
    }

    if (this->eyeTimer == 0) {
        this->eyeTimer = 2;
        this->eyeTexIndex++;
        if (this->eyeTexIndex >= 3) {
            this->eyeTexIndex = 0;
            this->eyeTimer = (s16)rnd_f(60.0f) + 20;
        }
    }
    this->actionFunc(this, play);
    Actor_world_to_eye(&this->actor, 80.0f);
    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.trackPos.y = player->actor.world.pos.y;
    eye_moveM(&this->actor, &this->interactInfo, 2, NPC_TRACKING_FULL_BODY);
    this->headRot = this->interactInfo.headRot;
    this->torsoRot = this->interactInfo.torsoRot;
    if ((play->gameplayFrames % 16) == 0) {
        pos = this->actor.world.pos;
        pos.y += 20.0f;
        Effect_SS_G_Ripple_ct2(play, &pos, 100, 500, 30);
    }
    this->unk_290++;
    Actor_BGcheck2(play, &this->actor, 20.0f, 20.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

static Gfx* npc_nothing(GraphicsContext* gfxCtx) {
    Gfx* displayList = GRAPH_ALLOC(gfxCtx, sizeof(Gfx));

    gSPEndDisplayList(displayList);
    return displayList;
}

s32 En_Diving_Game_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnDivingGame* this = (EnDivingGame*)thisx;
    s32 pad;

    if (limbIndex == 6) {
        rot->x += this->torsoRot.y;
    }

    if (limbIndex == 15) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.z;
    }

    if (this->notPlayingMinigame && (limbIndex == 8 || limbIndex == 9 || limbIndex == 12)) {
        rot->y += sin_s((play->state.frames * (limbIndex * FIDGET_FREQ_LIMB + FIDGET_FREQ_Y))) * FIDGET_AMPLITUDE;
        rot->z += cos_s((play->state.frames * (limbIndex * FIDGET_FREQ_LIMB + FIDGET_FREQ_Z))) * FIDGET_AMPLITUDE;
    }

    return 0;
}

void En_Diving_Game_actor_draw(Actor* thisx, PlayState* play) {
    EnDivingGame* this = (EnDivingGame*)thisx;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_diving_game.c", 1212);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, npc_nothing(play->state.gfxCtx));
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeTexIndex]));

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          En_Diving_Game_draw_sub, NULL, this);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_diving_game.c", 1232);
}
