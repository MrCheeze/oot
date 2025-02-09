void DemoDu_InitCs_Credits(DemoDu* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gDaruniaSkel, &gDaruniaCreditsIdleAnim, NULL, NULL, 0);
    this->updateIndex = CS_CREDITS_SUBSCENE(0);
    this->drawIndex = 0;
    this->actor.shape.shadowAlpha = 0;
    DemoDu_SetMouthTexIndex(this, 3);
}

void DemoDu_CsCredits_UpdateShadowAlpha(DemoDu* this) {
    s32 shadowAlpha = 255;
    f32 temp_f0;
    f32* unk_1A4;

    this->unk_1A4 += 1.0f;
    temp_f0 = kREG(17) + 10.0f;
    unk_1A4 = &this->unk_1A4;

    if (temp_f0 <= *unk_1A4) {
        this->shadowAlpha = shadowAlpha;
        this->actor.shape.shadowAlpha = shadowAlpha;
    } else {
        shadowAlpha = *unk_1A4 / temp_f0 * 255.0f;
        this->shadowAlpha = shadowAlpha;
        this->actor.shape.shadowAlpha = shadowAlpha;
    }
}

void DemoDu_CsCredits_AdvanceTo01(DemoDu* this, PlayState* play) {
    DemoDu_SetStartPosRotFromCue(this, play, 2);
    this->updateIndex = CS_CREDITS_SUBSCENE(1);
    this->drawIndex = 2;
}

void DemoDu_CsCredits_AdvanceTo02(DemoDu* this) {
    if (this->unk_1A4 >= kREG(17) + 10.0f) {
        this->updateIndex = CS_CREDITS_SUBSCENE(2);
        this->drawIndex = 1;
    }
}

void DemoDu_CsCredits_AdvanceTo03(DemoDu* this) {
    func_80969DDC(this, &gDaruniaLookingUpToSariaAnim, ANIMMODE_ONCE, -8.0f, 0);
    this->updateIndex = CS_CREDITS_SUBSCENE(3);
}

void DemoDu_CsCredits_AdvanceTo04(DemoDu* this) {
    func_80969DDC(this, &gDaruniaCreditsHitBreastAnim, ANIMMODE_ONCE, 0.0f, 0);
    this->updateIndex = CS_CREDITS_SUBSCENE(4);
}

void DemoDu_CsCredits_BackTo02(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        func_80969DDC(this, &gDaruniaCreditsIdleAnim, ANIMMODE_LOOP, 0.0f, 0);
        this->updateIndex = CS_CREDITS_SUBSCENE(2);
    }
}

void DemoDu_CsCredits_HandleCues(DemoDu* this, PlayState* play) {
    CsCmdActorCue* cue = DemoDu_GetCue(play, 2);

    if (cue != NULL) {
        s32 nextCueId = cue->id;
        s32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 9:
                    DemoDu_CsCredits_AdvanceTo01(this, play);
                    break;
                case 10:
                    DemoDu_CsCredits_AdvanceTo03(this);
                    break;
                case 11:
                    DemoDu_CsCredits_AdvanceTo04(this);
                    break;
                default:
                    // "Demo_Du_inEnding_Check_DemoMode:There is no such operation!!!!!!!!"
                    PRINTF("Demo_Du_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->cueId = nextCueId;
        }
    }
}

void DemoDu_UpdateCs_CR_00(DemoDu* this, PlayState* play) {
    DemoDu_CsCredits_HandleCues(this, play);
}

void DemoDu_UpdateCs_CR_01(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_UpdateEyes(this);
    DemoDu_CsCredits_UpdateShadowAlpha(this);
    DemoDu_CsCredits_AdvanceTo02(this);
}

void DemoDu_UpdateCs_CR_02(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_UpdateEyes(this);
    DemoDu_CsCredits_HandleCues(this, play);
}

void DemoDu_UpdateCs_CR_03(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_UpdateEyes(this);
    DemoDu_CsCredits_HandleCues(this, play);
}

void DemoDu_UpdateCs_CR_04(DemoDu* this, PlayState* play) {
    s32 animFinished;

    DemoDu_UpdateBgCheckInfo(this, play);
    animFinished = DemoDu_UpdateSkelAnime(this);
    DemoDu_UpdateEyes(this);
    DemoDu_CsCredits_BackTo02(this, animFinished);
}
