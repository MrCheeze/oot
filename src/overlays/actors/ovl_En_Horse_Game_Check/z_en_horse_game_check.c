/*
 * File: z_en_horse_game_check.c
 * Overlay: ovl_En_Horse_Game_Check
 * Description: Horseback Minigames
 */

#include "z_en_horse_game_check.h"
#include "overlays/actors/ovl_En_Horse/z_en_horse.h"
#include "libu64/debug.h"
#include "line_numbers.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

#define AT_FINISH_LINE(actor)                                                                                     \
    (Math3D_Check2DInArea(goal_area_check[0], goal_area_check[1], goal_area_check[2], goal_area_check[3], (actor)->world.pos.x, \
                            (actor)->world.pos.z))
#define AT_RANCH_EXIT(actor)                                                                                  \
    (Math3D_Check2DInArea(exit_area_check[0], exit_area_check[1], exit_area_check[2], exit_area_check[3], (actor)->world.pos.x, \
                            (actor)->world.pos.z))

#define INGO_RACE_PLAYER_MOVE (1 << 0)
#define INGO_RACE_SET_TIMER (1 << 1)
#define INGO_RACE_INGO_MOVE (1 << 2)

typedef enum HorseGameIngoRaceResult {
    /* 0 */ INGO_RACE_NO_RESULT,
    /* 1 */ INGO_RACE_PLAYER_WIN,
    /* 2 */ INGO_RACE_INGO_WIN,
    /* 3 */ INGO_RACE_TIME_UP
} HorseGameIngoRaceResult;

#define MALONRACE_PLAYER_MOVE (1 << 0)
#define MALONRACE_SET_TIMER (1 << 1)
#define MALONRACE_SECOND_LAP (1 << 2)
#define MALONRACE_BROKE_RULE (1 << 3)
#define MALONRACE_START_SFX (1 << 4)
#define MALONRACE_PLAYER_START (1 << 5)
#define MALONRACE_PLAYER_ON_MARK (1 << 6)

typedef enum HorseGameMalonRaceResult {
    /* 0 */ MALONRACE_NO_RESULT,
    /* 1 */ MALONRACE_SUCCESS,
    /* 2 */ MALONRACE_TIME_UP,
    /* 3 */ MALONRACE_UNUSED,
    /* 4 */ MALONRACE_FAILURE
} HorseGameMalonRaceResult;

void En_Horse_Game_Check_Actor_ct(Actor* thisx, PlayState* play);
void En_Horse_Game_Check_Actor_dt(Actor* thisx, PlayState* play);
void En_Horse_Game_Check_Actor_move(Actor* thisx, PlayState* play);
void En_Horse_Game_Check_Actor_display(Actor* thisx, PlayState* play);

ActorProfile En_Horse_Game_Check_Profile = {
    /**/ ACTOR_EN_HORSE_GAME_CHECK,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnHorseGameCheck),
    /**/ En_Horse_Game_Check_Actor_ct,
    /**/ En_Horse_Game_Check_Actor_dt,
    /**/ En_Horse_Game_Check_Actor_move,
    /**/ En_Horse_Game_Check_Actor_display,
};

static Vec3f Spot20RaceCheckPosTbl[] = {
    { 1700.0f, 1.0f, -540.0f },
    { 117.0f, 1.0f, 488.0f },
    { -1700.0f, 1.0f, -540.0f },
};

static f32 goal_area_check[4] = { -200.0f, 80.0f, -2300.0f, -1470.0f };

static f32 exit_area_check[4] = { 800.0f, 1000.0f, -2900.0f, -2700.0f };

static Vec3f pos = { 0.0f, 0.0f, 0.0f };

static Vec3f ta_pos_dat[] = {
    { 820.0f, -44.0f, -1655.0f }, { 1497.0f, -21.0f, -1198.0f },  { 1655.0f, -44.0f, -396.0f },
    { 1291.0f, -44.0f, 205.0f },  { 379.0f, -21.0f, 455.0f },     { -95.0f, -21.0f, 455.0f },
    { -939.0f, 1.0f, 455.0f },    { -1644.0f, -21.0f, -1035.0f },
};

s32 En_HGC_Spot20_Race_ct(EnHorseGameCheckBase* base, PlayState* play) {
    EnHorseGameCheckIngoRace* this = (EnHorseGameCheckIngoRace*)base;
    s32 i;

    this->base.type = HORSEGAME_INGO_RACE;
    this->startFlags = 0;
    for (i = 0; i < 3; i++) {
        this->playerCheck[i] = 0;
    }
    this->ingoHorse = Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_HORSE, -250.0f, 1.0f, -1650.0f, 0, 0x4000, 0, 0x8003);

    if (this->ingoHorse == NULL) {
        _dbg_hungup("../z_en_horse_game_check.c", LN1(382, 385));
    }

    this->startTimer = 0;
    this->finishTimer = 0;
    this->result = INGO_RACE_NO_RESULT;
    this->playerFinish = 0;
    this->ingoFinish = 0;

    return true;
}

s32 En_HGC_Spot20_Race_dt(EnHorseGameCheckBase* base, PlayState* play) {
    return true;
}

void En_HGC_Spot20_Race_end(EnHorseGameCheckIngoRace* this, PlayState* play) {
    z_common_data.save.cutsceneIndex = 0;
    if (this->result == INGO_RACE_PLAYER_WIN) {
        play->nextEntranceIndex = ENTR_LON_LON_RANCH_7;
        if (GET_EVENTINF(EVENTINF_INGO_RACE_SECOND_RACE)) {
            SET_EVENTINF_INGO_RACE_STATE(INGO_RACE_STATE_TRAPPED_WIN_EPONA);
            WRITE_EVENTINF_INGO_RACE_0F(1);
            play->transitionType = TRANS_TYPE_FADE_WHITE;
            Nottori_Bgm(NA_BGM_INGO);
        } else {
            SET_EVENTINF_INGO_RACE_STATE(INGO_RACE_STATE_FIRST_WIN);
            WRITE_EVENTINF_INGO_RACE_0F(1);
            Nottori_Bgm(NA_BGM_INGO);
            play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_WHITE, TCS_FAST);
        }
    } else {
        play->nextEntranceIndex = ENTR_LON_LON_RANCH_8;
        SET_EVENTINF_INGO_RACE_STATE(INGO_RACE_STATE_PLAYER_LOSE);
        play->transitionType = TRANS_TYPE_CIRCLE(TCA_NORMAL, TCC_BLACK, TCS_FAST);
        WRITE_EVENTINF_INGO_RACE_0F(1);
    }
    DREG(25) = 0;
    play->transitionTrigger = TRANS_TRIGGER_START;
    z_common_data.timerState = TIMER_STATE_OFF;
}

s32 En_HGC_Spot20_Race_move(EnHorseGameCheckBase* base, PlayState* play) {
    EnHorseGameCheckIngoRace* this = (EnHorseGameCheckIngoRace*)base;
    Player* player = GET_PLAYER(play);
    s32 i;
    EnHorse* ingoHorse;
    Player* player2 = player;

    if ((this->startTimer > 50) && !(this->startFlags & INGO_RACE_SET_TIMER)) {
        this->startFlags |= INGO_RACE_SET_TIMER;
        event_timer_set(0);
    } else if ((this->startTimer > 80) && (player->rideActor != NULL) && !(this->startFlags & INGO_RACE_PLAYER_MOVE)) {
        EnHorse* horse;

        this->startFlags |= INGO_RACE_PLAYER_MOVE;
        horse = (EnHorse*)player->rideActor;
        horse->inRace = 1;
    } else if ((this->startTimer > 81) && !(this->startFlags & INGO_RACE_INGO_MOVE)) {
        ingoHorse = (EnHorse*)this->ingoHorse;

        ingoHorse->inRace = 1;
        this->startFlags |= INGO_RACE_INGO_MOVE;
        Nai_FxFlagEntry(NA_SE_SY_START_SHOT, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }

    this->startTimer++;

    for (i = 0; i < 3; i++) {
        if ((player->rideActor != NULL) &&
            (Math3DLength(&Spot20RaceCheckPosTbl[i], &player->rideActor->world.pos) < 400.0f)) {
            if ((i > 0) && (this->playerCheck[i - 1] == 1)) {
                this->playerCheck[i] = 1;
            } else if (i == 0) {
                this->playerCheck[i] = 1;
            }
        }
        if (Math3DLength(&Spot20RaceCheckPosTbl[i], &this->ingoHorse->world.pos) < 400.0f) {
            if ((i > 0) && (this->ingoCheck[i - 1] == 1)) {
                this->ingoCheck[i] = 1;
            } else if (i == 0) {
                this->ingoCheck[i] = 1;
            }
        }
    }

    if (this->result == INGO_RACE_NO_RESULT) {
        if ((player2->rideActor != NULL) && (this->playerCheck[2] == 1) && AT_FINISH_LINE(player2->rideActor)) {
            this->playerFinish++;
            if (this->playerFinish > 0) {
                this->result = INGO_RACE_PLAYER_WIN;
                this->finishTimer = 55;
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_HORSE_GOAL);
                Nai_FxFlagEntry(NA_SE_SY_START_SHOT, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            }
            for (i = 0; i < 3; i++) {
                this->playerCheck[i] = 0;
            }
        }
        if ((this->ingoHorse != NULL) && (this->ingoCheck[2] == 1) && AT_FINISH_LINE(this->ingoHorse)) {
            this->ingoFinish++;
            if (this->ingoFinish > 0) {
                ingoHorse = (EnHorse*)this->ingoHorse;

                this->result = INGO_RACE_INGO_WIN;
                this->finishTimer = 70;
                ingoHorse->stateFlags |= ENHORSE_INGO_WON;
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_HORSE_GOAL);
                Nai_FxFlagEntry(NA_SE_SY_START_SHOT, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            }
            for (i = 0; i < 3; i++) {
                this->ingoCheck[i] = 0;
            }
        }
        if (((player2->rideActor != NULL) && AT_RANCH_EXIT(player2->rideActor)) || AT_RANCH_EXIT(&player2->actor)) {
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_HORSE_GOAL);
            this->result = INGO_RACE_INGO_WIN;
            this->finishTimer = 20;
        }
        if ((z_common_data.timerSeconds >= 180) && (this->startFlags & 2)) {
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_HORSE_GOAL);
            this->result = INGO_RACE_TIME_UP;
            this->finishTimer = 20;
        }
    } else {
        if (this->finishTimer > 0) {
            this->finishTimer--;
        } else {
            En_HGC_Spot20_Race_end(this, play);
        }
    }
    return true;
}

s32 En_HGC_Spot12_Yabusame_ct(EnHorseGameCheckBase* base, PlayState* play) {
    EnHorseGameCheckGerudoArchery* this = (EnHorseGameCheckGerudoArchery*)base;

    this->base.type = HORSEGAME_GERUDO_ARCHERY;
    this->unk_150 = 0;
    this->startTimer = 0;
    return true;
}

s32 En_HGC_Spot12_Yabusame_dt(EnHorseGameCheckBase* base, PlayState* play) {
    return true;
}

s32 En_HGC_Spot12_Yabusame_move(EnHorseGameCheckBase* base, PlayState* play) {
    EnHorseGameCheckGerudoArchery* this = (EnHorseGameCheckGerudoArchery*)base;
    Player* player = GET_PLAYER(play);
    EnHorse* horse = (EnHorse*)player->rideActor;

    if (horse == NULL) {
        return true;
    }

    if (this->startTimer > 90) {
        if (play) {}
        horse->hbaStarted = 1;
    }
    this->startTimer++;
    return true;
}

s32 En_HGC_Spot20_Rh_ct(EnHorseGameCheckBase* base, PlayState* play) {
    EnHorseGameCheck3* this = (EnHorseGameCheck3*)base;

    this->base.type = HORSEGAME_TYPE3;
    this->unk_150 = 0;
    return true;
}

s32 En_HGC_Spot20_Rh_dt(EnHorseGameCheckBase* base, PlayState* play) {
    return true;
}

s32 En_HGC_Spot20_Rh_move(EnHorseGameCheckBase* base, PlayState* play) {
    return true;
}

s32 En_HGC_Spot20_Ta_ct(EnHorseGameCheckBase* base, PlayState* play) {
    EnHorseGameCheckMalonRace* this = (EnHorseGameCheckMalonRace*)base;
    s32 i;

    this->base.type = HORSEGAME_MALON_RACE;
    this->raceFlags = 0;
    this->finishTimer = 0;
    this->result = MALONRACE_NO_RESULT;
    for (i = 0; i < 16; i++) {
        this->fenceCheck[i] = 0;
    }
    this->lapCount = 0;
    return true;
}

s32 En_HGC_Spot20_Ta_dt(EnHorseGameCheckBase* base, PlayState* play) {
    return true;
}

void En_HGC_Spot20_Ta_end(EnHorseGameCheckMalonRace* this, PlayState* play) {
    if ((this->result == MALONRACE_SUCCESS) || (this->result == MALONRACE_TIME_UP)) {
        z_common_data.save.cutsceneIndex = 0;
        play->nextEntranceIndex = ENTR_LON_LON_RANCH_7;
        play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_WHITE, TCS_FAST);
        play->transitionTrigger = TRANS_TRIGGER_START;
    } else if (this->result == MALONRACE_FAILURE) {
        z_common_data.timerSeconds = 240;
        z_common_data.timerState = TIMER_STATE_UP_FREEZE;
        z_common_data.save.cutsceneIndex = 0;
        play->nextEntranceIndex = ENTR_LON_LON_RANCH_7;
        play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_WHITE, TCS_FAST);
        play->transitionTrigger = TRANS_TRIGGER_START;
    } else {
        // "not supported"
        PRINTF("En_HGC_Spot20_Ta_end():対応せず\n");
        z_common_data.save.cutsceneIndex = 0;
        play->nextEntranceIndex = ENTR_LON_LON_RANCH_0;
        play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_WHITE, TCS_FAST);
        play->transitionTrigger = TRANS_TRIGGER_START;
    }
}

s32 En_HGC_Spot20_Ta_move(EnHorseGameCheckBase* base, PlayState* play) {
    EnHorseGameCheckMalonRace* this = (EnHorseGameCheckMalonRace*)base;
    s32 i;
    Player* player = GET_PLAYER(play);
    EnHorse* horse;
    Player* player2 = player;
    f32 dist;

    if (!(this->raceFlags & MALONRACE_PLAYER_ON_MARK) && AT_FINISH_LINE(player->rideActor)) {
        this->raceFlags |= MALONRACE_PLAYER_ON_MARK;
    } else if ((this->raceFlags & MALONRACE_PLAYER_ON_MARK) && !(this->raceFlags & MALONRACE_PLAYER_START) &&
               !AT_FINISH_LINE(player->rideActor)) {
        this->raceFlags |= MALONRACE_PLAYER_START;
    }
    if ((this->startTimer > 50) && !(this->raceFlags & MALONRACE_SET_TIMER)) {
        this->raceFlags |= MALONRACE_SET_TIMER;
        event_timer_set(0);
    } else if ((this->startTimer > 80) && (player->rideActor != NULL) && !(this->raceFlags & MALONRACE_PLAYER_MOVE)) {
        EnHorse* rideHorse;

        this->raceFlags |= MALONRACE_PLAYER_MOVE;
        rideHorse = (EnHorse*)player->rideActor;

        rideHorse->inRace = 1;
    } else if ((this->startTimer > 81) && !(this->raceFlags & MALONRACE_START_SFX)) {
        this->raceFlags |= MALONRACE_START_SFX;
        Nai_FxFlagEntry(NA_SE_SY_START_SHOT, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }

    this->startTimer++;
    if (this->result == MALONRACE_NO_RESULT) {
        for (i = 0; i < 16; i++) {
            if ((this->lapCount == 0) && (i >= 8)) {
                break;
            }
            dist = search_position_distanceXZ(&ta_pos_dat[i % 8], &player2->rideActor->world.pos);
            if ((player->rideActor != NULL) && (dist < 250.0f)) {
                horse = (EnHorse*)player2->rideActor;

                if (horse->stateFlags & ENHORSE_JUMPING) {
                    if ((i > 0) && (this->fenceCheck[i - 1] == 1)) {
                        this->fenceCheck[i] = 1;
                    } else if (i == 0) {
                        this->fenceCheck[i] = 1;
                    }

                    if ((this->fenceCheck[i - 1] == 0) && !(this->raceFlags & MALONRACE_BROKE_RULE)) {
                        this->raceFlags |= MALONRACE_BROKE_RULE;
                        message_set(play, 0x208C, NULL);
                        this->result = 4;
                        this->finishTimer = 30;
                    }
                }
            }
        }
        if ((player2->rideActor != NULL) && (this->raceFlags & MALONRACE_PLAYER_START) &&
            AT_FINISH_LINE(player2->rideActor)) {
            if ((this->lapCount == 1) && (this->fenceCheck[15] == 0) && (player2->rideActor->prevPos.x < -200.0f)) {
                this->raceFlags |= MALONRACE_BROKE_RULE;
                message_set(play, 0x208C, NULL);
                this->result = MALONRACE_FAILURE;
                this->finishTimer = 30;
            } else if (this->fenceCheck[15] == 1) {
                this->lapCount = 2;
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_HORSE_GOAL);
                Nai_FxFlagEntry(NA_SE_SY_START_SHOT, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
                this->result = MALONRACE_SUCCESS;
                this->finishTimer = 70;
                z_common_data.timerState = TIMER_STATE_UP_FREEZE;
            } else if ((this->fenceCheck[7] == 1) && !(this->raceFlags & MALONRACE_SECOND_LAP)) {
                this->lapCount = 1;
                this->raceFlags |= MALONRACE_SECOND_LAP;
                message_set(play, 0x208D, NULL);
            } else if (this->fenceCheck[7] == 0) {
                this->raceFlags |= MALONRACE_BROKE_RULE;
                message_set(play, 0x208C, NULL);
                this->result = MALONRACE_FAILURE;
                this->finishTimer = 30;
            } else if (player2->rideActor->prevPos.x > 80.0f) {
                this->raceFlags |= MALONRACE_BROKE_RULE;
                message_set(play, 0x208C, NULL);
                this->result = MALONRACE_FAILURE;
                this->finishTimer = 30;
            }
        }
        if ((z_common_data.timerSeconds >= 180) && (this->raceFlags & MALONRACE_SET_TIMER)) {
            z_common_data.timerSeconds = 240;
            this->result = MALONRACE_TIME_UP;
            this->finishTimer = 30;
            z_common_data.timerState = TIMER_STATE_OFF;
        }
    } else {
        if (this->finishTimer > 0) {
            this->finishTimer--;
        } else {
            En_HGC_Spot20_Ta_end(this, play);
        }
    }
    return true;
}

static EnHorseGameCheckFunc CtFunctionTbl[] = {
    NULL,
    En_HGC_Spot20_Race_ct,
    En_HGC_Spot12_Yabusame_ct,
    En_HGC_Spot20_Rh_ct,
    En_HGC_Spot20_Ta_ct,
};

static EnHorseGameCheckFunc DtFunctionTbl[] = {
    NULL,
    En_HGC_Spot20_Race_dt,
    En_HGC_Spot12_Yabusame_dt,
    En_HGC_Spot20_Rh_dt,
    En_HGC_Spot20_Ta_dt,
};

static EnHorseGameCheckFunc MoveFunctionTbl[] = {
    NULL,
    En_HGC_Spot20_Race_move,
    En_HGC_Spot12_Yabusame_move,
    En_HGC_Spot20_Rh_move,
    En_HGC_Spot20_Ta_move,
};

void En_Horse_Game_Check_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHorseGameCheckBase* this = (EnHorseGameCheckBase*)thisx;

    if ((play->sceneId == SCENE_LON_LON_RANCH) &&
        (event_check(EVENTCHKINF_EPONA_OBTAINED) || R_DEBUG_FORCE_EPONA_OBTAINED)) {
        this->actor.params = HORSEGAME_MALON_RACE;
    }
    if (CtFunctionTbl[this->actor.params] != NULL) {
        CtFunctionTbl[this->actor.params](this, play);
    }
}

void En_Horse_Game_Check_Actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHorseGameCheckBase* this = (EnHorseGameCheckBase*)thisx;

    if (DtFunctionTbl[this->actor.params] != NULL) {
        DtFunctionTbl[this->actor.params](this, play);
    }
}

void En_Horse_Game_Check_Actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHorseGameCheckBase* this = (EnHorseGameCheckBase*)thisx;

    if (MoveFunctionTbl[this->type] != NULL) {
        MoveFunctionTbl[this->type](this, play);
    }
}

void En_Horse_Game_Check_Actor_display(Actor* thisx, PlayState* play) {
}
