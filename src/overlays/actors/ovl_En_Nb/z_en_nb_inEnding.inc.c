void EnNb_SetupCreditsSpawn(EnNb* this, PlayState* play) {
    EnNb_SetCurrentAnim(this, &gNabooruSittingCrossLeggedAnim, 0, 0.0f, 0);
    this->action = NB_CREDITS_INIT;
    this->drawMode = NB_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

void EnNb_SetAlphaInCredits(EnNb* this) {
    f32* alphaTimer = &this->alphaTimer;
    s32 alpha;

    this->alphaTimer++;

    if ((kREG(17) + 10.0f) <= this->alphaTimer) {
        this->alpha = 255;
        this->actor.shape.shadowAlpha = 255;
    } else {
        alpha = (*alphaTimer / (kREG(17) + 10.0f)) * 255.0f;
        this->alpha = alpha;
        this->actor.shape.shadowAlpha = alpha;
    }
}

void EnNb_SetupCreditsFadeIn(EnNb* this, PlayState* play) {
    EnNb_SetStartPosRotFromCue2(this, play, 1);
    this->action = NB_CREDITS_FADEIN;
    this->drawMode = NB_DRAW_HIDE;
}

void EnNb_SetupCreditsSit(EnNb* this) {
    if (this->alphaTimer >= kREG(17) + 10.0f) {
        this->action = NB_CREDITS_SIT;
        this->drawMode = NB_DRAW_DEFAULT;
    }
}

void EnNb_SetupCreditsHeadTurn(EnNb* this) {
    EnNb_SetCurrentAnim(this, &gNabooruSittingCrossLeggedTurningToLookUpRightTransitionAnim, 2, -8.0f, 0);
    this->action = NB_CREDITS_HEAD_TURN;
}

void EnNb_CheckIfLookingUp(EnNb* this, s32 animFinished) {
    if (animFinished) {
        EnNb_SetCurrentAnim(this, &gNabooruSittingCrossLeggedLookingUpRightAnim, 0, 0.0f, 0);
    }
}

void EnNb_CheckCreditsCsModeImpl(EnNb* this, PlayState* play) {
    CsCmdActorCue* cue = EnNb_GetCue(play, 1);
    s32 nextCueId;
    s32 currentCueId;

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 15:
                    EnNb_SetupCreditsFadeIn(this, play);
                    break;
                case 16:
                    EnNb_SetupCreditsHeadTurn(this);
                    break;
                default:
                    // "En_Nb_inEnding_Check_DemoMode: Operation doesn't exist!!!!!!!!"
                    PRINTF("En_Nb_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->cueId = nextCueId;
        }
    }
}

void EnNb_CheckCreditsCsMode(EnNb* this, PlayState* play) {
    EnNb_CheckCreditsCsModeImpl(this, play);
}

void EnNb_CreditsFade(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_SetAlphaInCredits(this);
    EnNb_SetupCreditsSit(this);
}

void func_80AB3428(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_CheckCreditsCsModeImpl(this, play);
}

void EnNb_LookUp(EnNb* this, PlayState* play) {
    s32 animFinished;

    func_80AB1284(this, play);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_CheckIfLookingUp(this, animFinished);
}
