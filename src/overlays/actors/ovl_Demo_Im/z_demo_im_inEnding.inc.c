void Demo_Im_Ending_Init(DemoIm* this, PlayState* play) {
    Demo_Im_Change_Anime(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->action = 27;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void Demo_Im_inEnding_Set_Alpha(DemoIm* this) {
    f32* unk_268 = &this->unk_268;
    f32 temp;
    s32 alpha = 255;

    *unk_268 += 1.0f;
    temp = kREG(17) + 10.0f;

    if (*unk_268 >= temp) {
        this->actor.shape.shadowAlpha = this->alpha = alpha;
    } else {
        this->actor.shape.shadowAlpha = this->alpha = (*unk_268 / temp) * 255.0f;
    }
}

void Demo_Im_inEnding_setup_Appear(DemoIm* this, PlayState* play) {
    Demo_Im_Set_StartPos_npcdemopnt(this, play, 5);
    this->action = 28;
    this->drawConfig = 2;
}

void Demo_Im_inEnding_check_AppearToStand(DemoIm* this) {
    if (this->unk_268 >= kREG(17) + 10.0f) {
        this->action = 29;
        this->drawConfig = 1;
    }
}

void Demo_Im_inEnding_setup_Lookup(DemoIm* this) {
    Demo_Im_Change_Anime(this, &object_im_Anim_0101C8, ANIMMODE_ONCE, -8.0f, false);
    this->action = 30;
}

void Demo_Im_inEnding_Check_Animation_Lookup(DemoIm* this, s32 arg1) {
    if (arg1 != 0) {
        Demo_Im_Change_Anime(this, &object_im_Anim_00FB10, ANIMMODE_LOOP, 0.0f, false);
    }
}

void Demo_Im_inEnding_Check_DemoMode(DemoIm* this, PlayState* play) {
    CsCmdActorCue* cue = Demo_Im_Get_npcdemopnt(play, 5);

    if (cue != NULL) {
        u32 nextCueId = cue->id;
        u32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 12:
                    Demo_Im_inEnding_setup_Appear(this, play);
                    break;
                case 13:
                    Demo_Im_inEnding_setup_Lookup(this);
                    break;
                default:
                    PRINTF("Demo_Im_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void Demo_Im_inEnding_main_wait(DemoIm* this, PlayState* play) {
    Demo_Im_inEnding_Check_DemoMode(this, play);
}

void Demo_Im_inEnding_main_alpha(DemoIm* this, PlayState* play) {
    Demo_Im_BGcheck(this, play);
    Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_inEnding_Set_Alpha(this);
    Demo_Im_inEnding_check_AppearToStand(this);
}

void Demo_Im_inEnding_main_stand(DemoIm* this, PlayState* play) {
    Demo_Im_BGcheck(this, play);
    Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_inEnding_Check_DemoMode(this, play);
}

void Demo_Im_inEnding_main_lookup(DemoIm* this, PlayState* play) {
    s32 sp1C;

    Demo_Im_BGcheck(this, play);
    sp1C = Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_inEnding_Check_Animation_Lookup(this, sp1C);
}
