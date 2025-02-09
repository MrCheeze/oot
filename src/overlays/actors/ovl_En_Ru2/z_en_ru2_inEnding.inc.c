/**
 * Sets up Ruto's hands-on-hips pose during the credits sequence.
 */
void En_Ru2_Ending_Init(EnRu2* this, PlayState* play) {
    En_Ru2_Change_Anime(this, &gAdultRutoIdleHandsOnHipsAnim, 0, 0.0f, 0);
    this->action = ENRU2_CREDITS_INVISIBLE;
    this->drawConfig = ENRU2_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

/**
 * Fades in Ruto's actor during the credits sequence.
 */
void En_Ru2_inEnding_Set_Alpha(EnRu2* this) {
    f32* fadeTimer = &this->fadeTimer;
    f32 fadeDuration;
    s32 alpha;

    *fadeTimer += 1.0f;

    fadeDuration = kREG(17) + 10.0f;
    if (fadeDuration <= *fadeTimer) {
        this->alpha = 255;
        this->actor.shape.shadowAlpha = 0xFF;
    } else {
        alpha = (*fadeTimer / fadeDuration) * 255.0f;
        this->alpha = alpha;
        this->actor.shape.shadowAlpha = alpha;
    }
}

void En_Ru2_inEnding_setup_Appear(EnRu2* this, PlayState* play) {
    En_Ru2_Set_StartPos_npcdemopnt(this, play, 3);
    this->action = ENRU2_CREDITS_FADE_IN;
    this->drawConfig = ENRU2_DRAW_XLU;
}

/**
 * Checks for the end of Ruto's fade-in during the credits sequence.
 */
void En_Ru2_inEnding_check_AppearToStand(EnRu2* this) {
    if (this->fadeTimer >= kREG(17) + 10.0f) {
        this->action = ENRU2_CREDITS_VISIBLE;
        this->drawConfig = ENRU2_DRAW_OPA;
    }
}

/**
 * Starts Ruto's animation to look down towards Nabooru during the credits sequence.
 */
void En_Ru2_inEnding_setup_Lookup(EnRu2* this) {
    En_Ru2_Change_Anime(this, &gAdultRutoHeadTurnDownLeftAnim, 2, 0.0f, 0);
    this->action = ENRU2_CREDITS_TURN_HEAD_DOWN_LEFT;
}

/**
 * Holds Ruto's pose looking down towards Nabooru during the credits sequence.
 */
void En_Ru2_inEnding_Check_Animation_Lookup(EnRu2* this, s32 isDoneTurning) {
    if (isDoneTurning != 0) {
        En_Ru2_Change_Anime(this, &gAdultRutoLookingDownLeftAnim, 0, 0.0f, 0);
    }
}

/**
 * Advances Ruto's actions in two different places.
 */
void En_Ru2_inEnding_Check_DemoMode(EnRu2* this, PlayState* play) {
    CsCmdActorCue* cue = En_Ru2_Get_npcdemopnt(play, 3);
    s32 nextCueId;
    s32 currentCueId;

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 7:
                    En_Ru2_inEnding_setup_Appear(this, play);
                    break;
                case 8:
                    En_Ru2_inEnding_setup_Lookup(this);
                    break;
                default:
                    // "There is no such action!"
                    PRINTF("En_Ru2_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->cueId = nextCueId;
        }
    }
}

void En_Ru2_inEnding_main_wait(EnRu2* this, PlayState* play) {
    En_Ru2_inEnding_Check_DemoMode(this, play);
}

void En_Ru2_inEnding_main_alpha(EnRu2* this, PlayState* play) {
    En_Ru2_BGcheck(this, play);
    En_Ru2_Animation_Base(this);
    En_Ru2_set_eye_pattern(this);
    En_Ru2_inEnding_Set_Alpha(this);
    En_Ru2_inEnding_check_AppearToStand(this);
}

void En_Ru2_inEnding_main_stand(EnRu2* this, PlayState* play) {
    En_Ru2_BGcheck(this, play);
    En_Ru2_Animation_Base(this);
    En_Ru2_set_eye_pattern(this);
    En_Ru2_inEnding_Check_DemoMode(this, play);
}

void En_Ru2_inEnding_main_lookup(EnRu2* this, PlayState* play) {
    s32 animDone;

    En_Ru2_BGcheck(this, play);
    animDone = En_Ru2_Animation_Base(this);
    En_Ru2_set_eye_pattern(this);
    En_Ru2_inEnding_Check_Animation_Lookup(this, animDone);
}
