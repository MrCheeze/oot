void En_Nb_Ending_Init(EnNb* this, PlayState* play) {
    En_Nb_Change_Anime(this, &gNabooruSittingCrossLeggedAnim, 0, 0.0f, 0);
    this->action = NB_CREDITS_INIT;
    this->drawMode = NB_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

void En_Nb_inEnding_Set_Alpha(EnNb* this) {
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

void En_Nb_inEnding_setup_Appear(EnNb* this, PlayState* play) {
    En_Nb_Set_StartPos_npcdemopnt(this, play, 1);
    this->action = NB_CREDITS_FADEIN;
    this->drawMode = NB_DRAW_HIDE;
}

void En_Nb_inEnding_check_AppearToStand(EnNb* this) {
    if (this->alphaTimer >= kREG(17) + 10.0f) {
        this->action = NB_CREDITS_SIT;
        this->drawMode = NB_DRAW_DEFAULT;
    }
}

void En_Nb_inEnding_setup_Lookup(EnNb* this) {
    En_Nb_Change_Anime(this, &gNabooruSittingCrossLeggedTurningToLookUpRightTransitionAnim, 2, -8.0f, 0);
    this->action = NB_CREDITS_HEAD_TURN;
}

void En_Nb_inEnding_Check_Animation_Lookup(EnNb* this, s32 animFinished) {
    if (animFinished) {
        En_Nb_Change_Anime(this, &gNabooruSittingCrossLeggedLookingUpRightAnim, 0, 0.0f, 0);
    }
}

void En_Nb_inEnding_Check_DemoMode(EnNb* this, PlayState* play) {
    CsCmdActorCue* cue = En_Nb_Get_npcdemopnt(play, 1);
    s32 nextCueId;
    s32 currentCueId;

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 15:
                    En_Nb_inEnding_setup_Appear(this, play);
                    break;
                case 16:
                    En_Nb_inEnding_setup_Lookup(this);
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

void En_Nb_inEnding_main_wait(EnNb* this, PlayState* play) {
    En_Nb_inEnding_Check_DemoMode(this, play);
}

void En_Nb_inEnding_main_alpha(EnNb* this, PlayState* play) {
    En_Nb_BGcheck(this, play);
    En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
    En_Nb_inEnding_Set_Alpha(this);
    En_Nb_inEnding_check_AppearToStand(this);
}

void En_Nb_inEnding_main_stand(EnNb* this, PlayState* play) {
    En_Nb_BGcheck(this, play);
    En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
    En_Nb_inEnding_Check_DemoMode(this, play);
}

void En_Nb_inEnding_main_lookup(EnNb* this, PlayState* play) {
    s32 animFinished;

    En_Nb_BGcheck(this, play);
    animFinished = En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
    En_Nb_inEnding_Check_Animation_Lookup(this, animFinished);
}
