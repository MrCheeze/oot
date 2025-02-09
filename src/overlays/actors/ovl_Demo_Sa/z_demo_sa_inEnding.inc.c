void Demo_Sa_Ending_Init(DemoSa* this, PlayState* play) {
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gSariaSkel, &gSariaSitting3Anim, NULL, NULL, 0);
    this->action = 11;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void Demo_Sa_inEnding_Set_Alpha(DemoSa* this) {
    s32 alpha = 255;
    f32* unk_1A0 = &this->unk_1A0;
    f32 temp_f0;

    *unk_1A0 += 1.0f;
    temp_f0 = kREG(17) + 10.0f;

    if (temp_f0 <= *unk_1A0) {
        this->actor.shape.shadowAlpha = this->alpha = alpha;
    } else {
        this->actor.shape.shadowAlpha = this->alpha = (*unk_1A0 / temp_f0) * 255.0f;
    }
}

void Demo_Sa_inEnding_setup_Appear(DemoSa* this, PlayState* play) {
    Demo_Sa_Set_StartPos_npcdemopnt(this, play, 4);
    this->action = 12;
    this->drawConfig = 2;
}

void Demo_Sa_inEnding_check_AppearToStand(DemoSa* this) {
    if (this->unk_1A0 >= kREG(17) + 10.0f) {
        this->action = 13;
        this->drawConfig = 1;
    }
}

void Demo_Sa_inEnding_setup_Lookup(DemoSa* this) {
    Demo_Sa_Change_Anime(this, &gSariaSitting1Anim, 2, -8.0f, 0);
    this->action = 14;
}

void Demo_Sa_inEnding_setup_Lookdown(DemoSa* this) {
    Demo_Sa_Change_Anime(this, &gSariaSitting2Anim, 2, 0.0f, 0);
    this->action = 15;
}

void Demo_Sa_inEnding_Check_LookdownTostand(DemoSa* this, s32 arg1) {
    if (arg1 != 0) {
        Demo_Sa_Change_Anime(this, &gSariaSitting3Anim, 0, 0.0f, 0);
        this->action = 13;
    }
}

void Demo_Sa_inEnding_Check_DemoMode(DemoSa* this, PlayState* play) {
    s32 currentCueId;
    s32 nextCueId;
    CsCmdActorCue* cue = Demo_Sa_Get_npcdemopnt(play, 4);

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 7:
                    Demo_Sa_inEnding_setup_Appear(this, play);
                    break;
                case 8:
                    Demo_Sa_inEnding_setup_Lookup(this);
                    break;
                case 9:
                    Demo_Sa_inEnding_setup_Lookdown(this);
                    break;
                default:
                    PRINTF("Demo_Sa_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void Demo_Sa_inEnding_main_wait(DemoSa* this, PlayState* play) {
    Demo_Sa_inEnding_Check_DemoMode(this, play);
}

void Demo_Sa_inEnding_main_alpha(DemoSa* this, PlayState* play) {
    Demo_Sa_BGcheck(this, play);
    Demo_Sa_Animation_Base(this);
    Demo_Sa_set_eye_pattern(this);
    Demo_Sa_inEnding_Set_Alpha(this);
    Demo_Sa_inEnding_check_AppearToStand(this);
}

void Demo_Sa_inEnding_main_stand(DemoSa* this, PlayState* play) {
    Demo_Sa_BGcheck(this, play);
    Demo_Sa_Animation_Base(this);
    Demo_Sa_set_eye_pattern(this);
    Demo_Sa_inEnding_Check_DemoMode(this, play);
}

void Demo_Sa_inEnding_main_lookup(DemoSa* this, PlayState* play) {
    Demo_Sa_BGcheck(this, play);
    Demo_Sa_Animation_Base(this);
    Demo_Sa_set_eye_pattern(this);
    Demo_Sa_inEnding_Check_DemoMode(this, play);
}

void Demo_Sa_inEnding_main_lookdown(DemoSa* this, PlayState* play) {
    s32 sp1C;

    Demo_Sa_BGcheck(this, play);
    sp1C = Demo_Sa_Animation_Base(this);
    Demo_Sa_set_eye_pattern(this);
    Demo_Sa_inEnding_Check_LookdownTostand(this, sp1C);
}
