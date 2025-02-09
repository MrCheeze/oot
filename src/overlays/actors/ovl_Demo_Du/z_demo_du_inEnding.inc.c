void Demo_Du_Ending_Init(DemoDu* this, PlayState* play) {
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gDaruniaSkel, &gDaruniaCreditsIdleAnim, NULL, NULL, 0);
    this->updateIndex = CS_CREDITS_SUBSCENE(0);
    this->drawIndex = 0;
    this->actor.shape.shadowAlpha = 0;
    Demo_Du_set_mouth_Num(this, 3);
}

void Demo_Du_inEnding_Set_Alpha(DemoDu* this) {
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

void Demo_Du_inEnding_setup_Appear(DemoDu* this, PlayState* play) {
    Demo_Du_Set_StartPos_npcdemopnt(this, play, 2);
    this->updateIndex = CS_CREDITS_SUBSCENE(1);
    this->drawIndex = 2;
}

void Demo_Du_inEnding_check_AppearToStand(DemoDu* this) {
    if (this->unk_1A4 >= kREG(17) + 10.0f) {
        this->updateIndex = CS_CREDITS_SUBSCENE(2);
        this->drawIndex = 1;
    }
}

void Demo_Du_inEnding_setup_Lookup(DemoDu* this) {
    Demo_Du_Change_Anime(this, &gDaruniaLookingUpToSariaAnim, ANIMMODE_ONCE, -8.0f, 0);
    this->updateIndex = CS_CREDITS_SUBSCENE(3);
}

void Demo_Du_inEnding_setup_Lookdown(DemoDu* this) {
    Demo_Du_Change_Anime(this, &gDaruniaCreditsHitBreastAnim, ANIMMODE_ONCE, 0.0f, 0);
    this->updateIndex = CS_CREDITS_SUBSCENE(4);
}

void Demo_Du_inEnding_Check_LookdownTostand(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Demo_Du_Change_Anime(this, &gDaruniaCreditsIdleAnim, ANIMMODE_LOOP, 0.0f, 0);
        this->updateIndex = CS_CREDITS_SUBSCENE(2);
    }
}

void Demo_Du_inEnding_Check_DemoMode(DemoDu* this, PlayState* play) {
    CsCmdActorCue* cue = Demo_Du_Get_npcdemopnt(play, 2);

    if (cue != NULL) {
        s32 nextCueId = cue->id;
        s32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 9:
                    Demo_Du_inEnding_setup_Appear(this, play);
                    break;
                case 10:
                    Demo_Du_inEnding_setup_Lookup(this);
                    break;
                case 11:
                    Demo_Du_inEnding_setup_Lookdown(this);
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

void Demo_Du_inEnding_main_wait(DemoDu* this, PlayState* play) {
    Demo_Du_inEnding_Check_DemoMode(this, play);
}

void Demo_Du_inEnding_main_alpha(DemoDu* this, PlayState* play) {
    Demo_Du_BGcheck(this, play);
    Demo_Du_Animation_Base(this);
    Demo_Du_set_eye_pattern(this);
    Demo_Du_inEnding_Set_Alpha(this);
    Demo_Du_inEnding_check_AppearToStand(this);
}

void Demo_Du_inEnding_main_stand(DemoDu* this, PlayState* play) {
    Demo_Du_BGcheck(this, play);
    Demo_Du_Animation_Base(this);
    Demo_Du_set_eye_pattern(this);
    Demo_Du_inEnding_Check_DemoMode(this, play);
}

void Demo_Du_inEnding_main_lookup(DemoDu* this, PlayState* play) {
    Demo_Du_BGcheck(this, play);
    Demo_Du_Animation_Base(this);
    Demo_Du_set_eye_pattern(this);
    Demo_Du_inEnding_Check_DemoMode(this, play);
}

void Demo_Du_inEnding_main_lookdown(DemoDu* this, PlayState* play) {
    s32 animFinished;

    Demo_Du_BGcheck(this, play);
    animFinished = Demo_Du_Animation_Base(this);
    Demo_Du_set_eye_pattern(this);
    Demo_Du_inEnding_Check_LookdownTostand(this, animFinished);
}
