void Demo_Im_Ocarina_Init(DemoIm* this, PlayState* play) {
    Demo_Im_Change_Anime(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->action = 10;
    this->unk_280 = 1;
}

void Demo_Im_Ocarina_Calc_turn(DemoIm* this, PlayState* play) {
    if ((play->csCtx.curFrame >= 80) && (play->csCtx.curFrame < 243)) {
        Demo_Im_Calc_turn_link2(this, play);
    } else {
        Demo_Im_Calc_turn_front(this);
    }
}

void Demo_Im_Ocarina_Animation_stop(DemoIm* this) {
    if (!Skeleton_Info_frame_check(&this->skelAnime, Si2_anime_end_frame(&gImpaWhistlingAnim) - 1.0f)) {
        Demo_Im_Animation_Base(this);
    }
}

void Demo_Im_Ocarina_setup_Demo_Wait(DemoIm* this) {
    this->action = 10;
    this->drawConfig = 0;
}

void Demo_Im_Ocarina_setup_Demo_Greet(DemoIm* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gImpaIdleAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gImpaIdleAnim),
                     ANIMMODE_LOOP, 0.0f);
    Demo_Im_Set_DemoStartPosAngle(this, play, 5);
    this->action = 11;
    this->drawConfig = 1;
}

void Demo_Im_Ocarina_ChengAnime_Demo_Play(DemoIm* this, s32 arg1) {
    if (arg1 != 0) {
        Skeleton_Info2_init(&this->skelAnime, &gImpaWhistlingAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gImpaWhistlingAnim),
                         ANIMMODE_LOOP, -8.0f);
    }
}

void Demo_Im_Ocarina_setup_Demo_Play(DemoIm* this) {
    f32 frameCount = Si2_anime_end_frame(&gImpaStartWhistlingAnim);

    Skeleton_Info2_init(&this->skelAnime, &gImpaStartWhistlingAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, -8.0f);
    this->action = 12;
    this->drawConfig = 1;
    this->unk_2D0 = 1;
}

void Demo_Im_Ocarina_setup_Demo_Stop(DemoIm* this) {
    this->action = 13;
    this->drawConfig = 1;
}

void Demo_Im_Ocarina_ChengAnime_Demo_Put(DemoIm* this, s32 arg1) {
    if (arg1 != 0) {
        Skeleton_Info2_init(&this->skelAnime, &gImpaIdleAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gImpaIdleAnim),
                         ANIMMODE_LOOP, -8.0f);
        this->unk_2D0 = 0;
    }
}

void Demo_Im_Ocarina_setup_Demo_Put(DemoIm* this) {
    Skeleton_Info2_init(&this->skelAnime, &gImpaStartWhistlingAnim, -1.0f,
                     Si2_anime_end_frame(&gImpaStartWhistlingAnim), 0.0f, ANIMMODE_ONCE, -8.0f);
    this->action = 14;
    this->drawConfig = 1;
}

void Demo_Im_Ocarina_Check_DemoMode_StopToBranch(DemoIm* this, PlayState* play) {
    CsCmdActorCue* cue = Demo_Im_Get_npcdemopnt(play, 5);

    if (cue != NULL) {
        u32 nextCueId = cue->id;
        u32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 9:
                    Demo_Im_Ocarina_setup_Demo_Put(this);
                    break;
                case 7:
                    Skeleton_Info2_init(&this->skelAnime, &gImpaWhistlingAnim, 1.0f, 0.0f,
                                     Si2_anime_end_frame(&gImpaWhistlingAnim), ANIMMODE_LOOP, -8.0f);
                    this->action = 12;
                    break;
                default:
                    PRINTF("Demo_Im_Ocarina_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void Demo_Im_Ocarina_Check_DemoMode_PutToWatch(DemoIm* this, PlayState* play) {
    if (Demo_Im_Check_EndDemoMode(play)) {
        this->action = 21;
        this->drawConfig = 1;
        this->unk_280 = 1;
    }
}

void Demo_Im_Ocarina_Check_DemoMode(DemoIm* this, PlayState* play) {
    CsCmdActorCue* cue = Demo_Im_Get_npcdemopnt(play, 5);

    if (cue != NULL) {
        u32 nextCueId = cue->id;
        u32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    Demo_Im_Ocarina_setup_Demo_Wait(this);
                    break;
                case 2:
                    Demo_Im_Ocarina_setup_Demo_Greet(this, play);
                    break;
                case 7:
                    Demo_Im_Ocarina_setup_Demo_Play(this);
                    break;
                case 8:
                    Demo_Im_Ocarina_setup_Demo_Stop(this);
                    break;
                case 9:
                    Demo_Im_Ocarina_setup_Demo_Put(this);
                    break;
                default:
                    PRINTF("Demo_Im_Ocarina_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void Demo_Im_Ocarina_main_wait(DemoIm* this, PlayState* play) {
    Demo_Im_Ocarina_Check_DemoMode(this, play);
}

void Demo_Im_Ocarina_main_greet(DemoIm* this, PlayState* play) {
    Demo_Im_BGcheck(this, play);
    Demo_Im_Animation_Base(this);
    Demo_Im_Ocarina_Calc_turn(this, play);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_Ocarina_Check_DemoMode(this, play);
}

void Demo_Im_Ocarina_main_play(DemoIm* this, PlayState* play) {
    s32 sp24;

    Demo_Im_BGcheck(this, play);
    sp24 = Demo_Im_Animation_Base(this);
    Demo_Im_Ocarina_Calc_turn(this, play);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_Ocarina_ChengAnime_Demo_Play(this, sp24);
    Demo_Im_Ocarina_Check_DemoMode(this, play);
}

void Demo_Im_Ocarina_main_stop(DemoIm* this, PlayState* play) {
    Demo_Im_BGcheck(this, play);
    Demo_Im_Ocarina_Animation_stop(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_Ocarina_Check_DemoMode_StopToBranch(this, play);
}

void Demo_Im_Ocarina_main_put(DemoIm* this, PlayState* play) {
    s32 sp24;

    Demo_Im_BGcheck(this, play);
    sp24 = Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_Ocarina_ChengAnime_Demo_Put(this, sp24);
    Demo_Im_Ocarina_Check_DemoMode_PutToWatch(this, play);
}
