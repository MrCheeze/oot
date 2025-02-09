/**
 * Sets up Ruto's hands-on-hips pose during the credits sequence.
 */
void EnRu2_InitCredits(EnRu2* this, PlayState* play) {
    EnRu2_AnimationChange(this, &gAdultRutoIdleHandsOnHipsAnim, 0, 0.0f, 0);
    this->action = ENRU2_CREDITS_INVISIBLE;
    this->drawConfig = ENRU2_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

/**
 * Fades in Ruto's actor during the credits sequence.
 */
void EnRu2_FadeInCredits(EnRu2* this) {
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

void EnRu2_InitCreditsPosition(EnRu2* this, PlayState* play) {
    EnRu2_InitPositionFromCue(this, play, 3);
    this->action = ENRU2_CREDITS_FADE_IN;
    this->drawConfig = ENRU2_DRAW_XLU;
}

/**
 * Checks for the end of Ruto's fade-in during the credits sequence.
 */
void EnRu2_CheckVisibleInCredits(EnRu2* this) {
    if (this->fadeTimer >= kREG(17) + 10.0f) {
        this->action = ENRU2_CREDITS_VISIBLE;
        this->drawConfig = ENRU2_DRAW_OPA;
    }
}

/**
 * Starts Ruto's animation to look down towards Nabooru during the credits sequence.
 */
void EnRu2_SetupTurnHeadDownLeftAnimation(EnRu2* this) {
    EnRu2_AnimationChange(this, &gAdultRutoHeadTurnDownLeftAnim, 2, 0.0f, 0);
    this->action = ENRU2_CREDITS_TURN_HEAD_DOWN_LEFT;
}

/**
 * Holds Ruto's pose looking down towards Nabooru during the credits sequence.
 */
void EnRu2_HoldLookingDownLeftPose(EnRu2* this, s32 isDoneTurning) {
    if (isDoneTurning != 0) {
        EnRu2_AnimationChange(this, &gAdultRutoLookingDownLeftAnim, 0, 0.0f, 0);
    }
}

/**
 * Advances Ruto's actions in two different places.
 */
void EnRu2_NextCreditsAction(EnRu2* this, PlayState* play) {
    CsCmdActorCue* cue = EnRu2_GetCue(play, 3);
    s32 nextCueId;
    s32 currentCueId;

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 7:
                    EnRu2_InitCreditsPosition(this, play);
                    break;
                case 8:
                    EnRu2_SetupTurnHeadDownLeftAnimation(this);
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

void EnRu2_CreditsInvisible(EnRu2* this, PlayState* play) {
    EnRu2_NextCreditsAction(this, play);
}

void EnRu2_CreditsFadeIn(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_FadeInCredits(this);
    EnRu2_CheckVisibleInCredits(this);
}

void EnRu2_CreditsVisible(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_NextCreditsAction(this, play);
}

void EnRu2_CreditsTurnHeadDownLeft(EnRu2* this, PlayState* play) {
    s32 animDone;

    EnRu2_UpdateBgCheckInfo(this, play);
    animDone = EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_HoldLookingDownLeftPose(this, animDone);
}
