void En_Ru2_inMizusin_Set_SaveBit(EnRu2* this, PlayState* play) {
    Actor_Environment_sw_On(play, En_Ru2_GetUpper_arg_data(this));
}

s32 En_Ru2_inMizusin_Check_SaveBit(EnRu2* this, PlayState* play) {
    return Actor_Environment_sw_Check(play, En_Ru2_GetUpper_arg_data(this));
}

/**
 * Initializes Ruto's actor in the Water Temple, or destroys it if the encounter already happened.
 */
void En_Ru2_Mizusin_Init(EnRu2* this, PlayState* play) {
    if (En_Ru2_inMizusin_Check_SaveBit(this, play)) {
        Actor_delete(&this->actor);
    } else {
        En_Ru2_Change_Anime(this, &gAdultRutoIdleAnim, 0, 0.0f, 0);
        this->action = ENRU2_WATER_TEMPLE_ENCOUNTER_RANGE_CHECK;
        this->drawConfig = ENRU2_DRAW_OPA;
    }
}

void En_Ru2_inMizusin_Set_AppearSound(void) {
    Na_StartFanfare(NA_BGM_APPEAR);
}

/**
 * Accelerates Ruto's actor upwards as she swims.
 */
void En_Ru2_inMizusin_Movement(EnRu2* this) {
    f32 funcFloat;

    this->swimmingUpFrame++;
    funcFloat = get_parcent_forAccelBrake((kREG(2) + 0x96) & 0xFFFF, 0, this->swimmingUpFrame, 8, 0);
    this->actor.world.pos.y = this->actor.home.pos.y + (300.0f * funcFloat);
}

s32 En_Ru2_inMizusin_CheckLink(EnRu2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 thisPosX = this->actor.world.pos.x;
    f32 playerPosX = player->actor.world.pos.x;

    if (playerPosX - thisPosX >= -202.0f) {
        return 1;
    }
    return 0;
}

/**
 * Checks if Link is close enough to Ruto and conditionally triggers the encounter cutscene in the Water Temple.
 */
void En_Ru2_inMizusin_check_WaitToGreet(EnRu2* this, PlayState* play) {
    if (En_Ru2_inMizusin_CheckLink(this, play) && !Game_play_demo_mode_check(play)) {
        this->action = ENRU2_WATER_TEMPLE_ENCOUNTER_BEGINNING;
        makeOnepointDemo(play, 3130, -99, &this->actor, CAM_ID_MAIN);
    }
}

/**
 * Triggers the encounter cutscene in the Water Temple, unconditionally. Appears to be unused.
 */
void En_Ru2_inMizusin_check_StandToGreet(EnRu2* this, PlayState* play) {
    this->action = ENRU2_WATER_TEMPLE_ENCOUNTER_BEGINNING;
    makeOnepointDemo(play, 3130, -99, &this->actor, CAM_ID_MAIN);
}

/**
 * Handles the starting moments of Ruto's encounter with Link at the Water Temple. Responds to a running timer to
 * initiate, on cue, both the fanfare and Ruto's dialogue.
 */
void En_Ru2_inMizusin_check_GreetToGreeting(EnRu2* this, PlayState* play) {
    f32* encounterTimer = &this->encounterTimer;

    *encounterTimer += 1.0f;
    if (*encounterTimer == kREG(6) + 40.0f) {
        En_Ru2_inMizusin_Set_AppearSound();
    } else if (*encounterTimer > kREG(4) + 50.0f) {
        this->actor.textId = 0x403E;
        message_set(play, this->actor.textId, NULL);
        this->action = ENRU2_WATER_TEMPLE_ENCOUNTER_DIALOG;
    }
}

void En_Ru2_inMizusin_check_GreetingToGoodby(EnRu2* this, PlayState* play) {
    s32 pad;
    MessageContext* msgCtx;
    s32 pad2;
    u8 dialogState;
    Player* player;
    s32 pad3;

    msgCtx = &play->msgCtx;
    dialogState = message_check(msgCtx);

    if (dialogState == TEXT_STATE_DONE_FADING) {
        if (this->lastDialogState != TEXT_STATE_DONE_FADING) {
            // "I'm Komatsu!" (cinema scene dev)
            PRINTF("おれが小松だ！ \n");
            this->textboxCount++;
            if (this->textboxCount % 6 == 3) {
                player = GET_PLAYER(play);
                // "uorya-!" (screeming sound)
                PRINTF("うおりゃー！ \n");
                restartCameraStoped(GET_ACTIVE_CAM(play));
                player->actor.world.pos.x = 820.0f;
                player->actor.world.pos.y = 0.0f;
                player->actor.world.pos.z = 180.0f;
            }
        }
    }

    this->lastDialogState = dialogState;
    if (message_check(msgCtx) == TEXT_STATE_CLOSING) {
        this->action = ENRU2_WATER_TEMPLE_ENCOUNTER_END;
        restartCameraStoped(GET_ACTIVE_CAM(play));
    }
}

void En_Ru2_inMizusin_check_GoodbyToSwim(EnRu2* this, PlayState* play) {
    this->encounterTimer += 1.0f;
    if (this->encounterTimer > kREG(5) + 100.0f) {
        En_Ru2_Change_Anime(this, &gAdultRutoSwimmingUpAnim, 0, -12.0f, 0);
        this->action = ENRU2_WATER_TEMPLE_SWIMMING_UP;
        En_Ru2_inMizusin_Set_SaveBit(this, play);
    }
}

void En_Ru2_inMizusin_check_SwimToDisappear(EnRu2* this, PlayState* play) {
    if (this->swimmingUpFrame > ((((u16)(kREG(3) + 0x28)) + ((u16)(kREG(2) + 0x96))) & 0xFFFF)) {
        Actor_delete(&this->actor);
    }
}

void En_Ru2_inMizusin_main_wait(EnRu2* this, PlayState* play) {
    En_Ru2_inMizusin_check_WaitToGreet(this, play);
    Actor_world_to_eye(&this->actor, 50.0f);
    En_Ru2_Excute_Corect_forStand(this, play);
}

void En_Ru2_inMizusin_main_stand(EnRu2* this, PlayState* play) {
    En_Ru2_BGcheck(this, play);
    En_Ru2_Excute_Corect_forStand(this, play);
    En_Ru2_Animation_Base(this);
    En_Ru2_set_eye_pattern(this);
    Actor_world_to_eye(&this->actor, 50.0f);
    En_Ru2_inMizusin_check_StandToGreet(this, play);
}

void En_Ru2_inMizusin_main_greet(EnRu2* this, PlayState* play) {
    En_Ru2_BGcheck(this, play);
    En_Ru2_Animation_Base(this);
    En_Ru2_set_eye_pattern(this);
    Actor_world_to_eye(&this->actor, 50.0f);
    En_Ru2_inMizusin_check_GreetToGreeting(this, play);
}

void En_Ru2_inMizusin_main_greeting(EnRu2* this, PlayState* play) {
    En_Ru2_BGcheck(this, play);
    En_Ru2_Animation_Base(this);
    En_Ru2_set_eye_pattern(this);
    Actor_world_to_eye(&this->actor, 50.0f);
    En_Ru2_inMizusin_check_GreetingToGoodby(this, play);
}

void En_Ru2_inMizusin_main_gooby(EnRu2* this, PlayState* play) {
    En_Ru2_BGcheck(this, play);
    En_Ru2_Animation_Base(this);
    En_Ru2_set_eye_pattern(this);
    Actor_world_to_eye(&this->actor, 50.0f);
    En_Ru2_inMizusin_check_GoodbyToSwim(this, play);
}

void En_Ru2_inMizusin_main_swim(EnRu2* this, PlayState* play) {
    En_Ru2_inMizusin_Movement(this);
    En_Ru2_BGcheck(this, play);
    En_Ru2_Animation_Base(this);
    En_Ru2_set_eye_pattern(this);
    Actor_world_to_eye(&this->actor, 50.0f);
    En_Ru2_inMizusin_check_SwimToDisappear(this, play);
}
