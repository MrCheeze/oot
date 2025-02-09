void EnRu2_SetEncounterSwitchFlag(EnRu2* this, PlayState* play) {
    Flags_SetSwitch(play, EnRu2_GetSwitchFlag(this));
}

s32 EnRu2_GetEncounterSwitchFlag(EnRu2* this, PlayState* play) {
    return Flags_GetSwitch(play, EnRu2_GetSwitchFlag(this));
}

/**
 * Initializes Ruto's actor in the Water Temple, or destroys it if the encounter already happened.
 */
void EnRu2_InitWaterTempleEncounter(EnRu2* this, PlayState* play) {
    if (EnRu2_GetEncounterSwitchFlag(this, play)) {
        Actor_Kill(&this->actor);
    } else {
        EnRu2_AnimationChange(this, &gAdultRutoIdleAnim, 0, 0.0f, 0);
        this->action = ENRU2_WATER_TEMPLE_ENCOUNTER_RANGE_CHECK;
        this->drawConfig = ENRU2_DRAW_OPA;
    }
}

void EnRu2_PlayFanfare(void) {
    Audio_PlayFanfare(NA_BGM_APPEAR);
}

/**
 * Accelerates Ruto's actor upwards as she swims.
 */
void EnRu2_SwimUpProgress(EnRu2* this) {
    f32 funcFloat;

    this->swimmingUpFrame++;
    funcFloat = Environment_LerpWeightAccelDecel((kREG(2) + 0x96) & 0xFFFF, 0, this->swimmingUpFrame, 8, 0);
    this->actor.world.pos.y = this->actor.home.pos.y + (300.0f * funcFloat);
}

s32 EnRu2_IsPlayerInRangeForEncounter(EnRu2* this, PlayState* play) {
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
void EnRu2_CheckRangeToStartEncounter(EnRu2* this, PlayState* play) {
    if (EnRu2_IsPlayerInRangeForEncounter(this, play) && !Play_InCsMode(play)) {
        this->action = ENRU2_WATER_TEMPLE_ENCOUNTER_BEGINNING;
        OnePointCutscene_Init(play, 3130, -99, &this->actor, CAM_ID_MAIN);
    }
}

/**
 * Triggers the encounter cutscene in the Water Temple, unconditionally. Appears to be unused.
 */
void EnRu2_StartEncounter(EnRu2* this, PlayState* play) {
    this->action = ENRU2_WATER_TEMPLE_ENCOUNTER_BEGINNING;
    OnePointCutscene_Init(play, 3130, -99, &this->actor, CAM_ID_MAIN);
}

/**
 * Handles the starting moments of Ruto's encounter with Link at the Water Temple. Responds to a running timer to
 * initiate, on cue, both the fanfare and Ruto's dialogue.
 */
void EnRu2_EncounterBeginningHandler(EnRu2* this, PlayState* play) {
    f32* encounterTimer = &this->encounterTimer;

    *encounterTimer += 1.0f;
    if (*encounterTimer == kREG(6) + 40.0f) {
        EnRu2_PlayFanfare();
    } else if (*encounterTimer > kREG(4) + 50.0f) {
        this->actor.textId = 0x403E;
        Message_StartTextbox(play, this->actor.textId, NULL);
        this->action = ENRU2_WATER_TEMPLE_ENCOUNTER_DIALOG;
    }
}

void EnRu2_DialogCameraHandler(EnRu2* this, PlayState* play) {
    s32 pad;
    MessageContext* msgCtx;
    s32 pad2;
    u8 dialogState;
    Player* player;
    s32 pad3;

    msgCtx = &play->msgCtx;
    dialogState = Message_GetState(msgCtx);

    if (dialogState == TEXT_STATE_DONE_FADING) {
        if (this->lastDialogState != TEXT_STATE_DONE_FADING) {
            // "I'm Komatsu!" (cinema scene dev)
            PRINTF("おれが小松だ！ \n");
            this->textboxCount++;
            if (this->textboxCount % 6 == 3) {
                player = GET_PLAYER(play);
                // "uorya-!" (screeming sound)
                PRINTF("うおりゃー！ \n");
                Camera_SetFinishedFlag(GET_ACTIVE_CAM(play));
                player->actor.world.pos.x = 820.0f;
                player->actor.world.pos.y = 0.0f;
                player->actor.world.pos.z = 180.0f;
            }
        }
    }

    this->lastDialogState = dialogState;
    if (Message_GetState(msgCtx) == TEXT_STATE_CLOSING) {
        this->action = ENRU2_WATER_TEMPLE_ENCOUNTER_END;
        Camera_SetFinishedFlag(GET_ACTIVE_CAM(play));
    }
}

void EnRu2_StartSwimmingUp(EnRu2* this, PlayState* play) {
    this->encounterTimer += 1.0f;
    if (this->encounterTimer > kREG(5) + 100.0f) {
        EnRu2_AnimationChange(this, &gAdultRutoSwimmingUpAnim, 0, -12.0f, 0);
        this->action = ENRU2_WATER_TEMPLE_SWIMMING_UP;
        EnRu2_SetEncounterSwitchFlag(this, play);
    }
}

void EnRu2_EndSwimmingUp(EnRu2* this, PlayState* play) {
    if (this->swimmingUpFrame > ((((u16)(kREG(3) + 0x28)) + ((u16)(kREG(2) + 0x96))) & 0xFFFF)) {
        Actor_Kill(&this->actor);
    }
}

void EnRu2_WaterTempleEncounterRangeCheck(EnRu2* this, PlayState* play) {
    EnRu2_CheckRangeToStartEncounter(this, play);
    Actor_SetFocus(&this->actor, 50.0f);
    EnRu2_UpdateCollider(this, play);
}

void EnRu2_WaterTempleEncounterUnconditional(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateCollider(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    Actor_SetFocus(&this->actor, 50.0f);
    EnRu2_StartEncounter(this, play);
}

void EnRu2_WaterTempleEncounterBegin(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    Actor_SetFocus(&this->actor, 50.0f);
    EnRu2_EncounterBeginningHandler(this, play);
}

void EnRu2_WaterTempleEncounterDialog(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    Actor_SetFocus(&this->actor, 50.0f);
    EnRu2_DialogCameraHandler(this, play);
}

void EnRu2_WaterTempleEncounterEnd(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    Actor_SetFocus(&this->actor, 50.0f);
    EnRu2_StartSwimmingUp(this, play);
}

void EnRu2_WaterTempleSwimmingUp(EnRu2* this, PlayState* play) {
    EnRu2_SwimUpProgress(this);
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    Actor_SetFocus(&this->actor, 50.0f);
    EnRu2_EndSwimmingUp(this, play);
}
