void Demo_Ec_main_init_Kz(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &gKzSkel);
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcKingZoraAnim, 0, 0.0f, false);
    Demo_Ec_Start_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_KING_ZORA;
    this->drawConfig = EC_DRAW_KING_ZORA;
    Demo_Ec_set_eye_Num(this, 3);
}

void Demo_Ec_inKz_SetEyePattern_forStandup(DemoEc* this) {
    f32 currentFrame = this->skelAnime.curFrame;

    if (currentFrame <= 32.0f) {
        Demo_Ec_set_eye_Num(this, 3);
    } else {
        Demo_Ec_set_eye_pattern(this);
    }
}

void Demo_Ec_inKz_Setup_Stand(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcAnim_006930, 2, -8.0f, false);
    this->updateMode = EC_UPDATE_17;
}

void Demo_Ec_inKz_Check_StandupToStand(DemoEc* this, s32 arg1) {
    if (arg1 != 0) {
        Demo_Ec_Change_Anime(this, &gDemoEcAnim_006220, 0, 0.0f, false);
        this->updateMode = EC_UPDATE_18;
    }
}

void Demo_Ec_inKz_Check_DemoMode(DemoEc* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = Demo_Ec_Get_npcdemopnt(play, cueChannel);

    if (cue != NULL) {
        s32 nextCueId = cue->id;
        s32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 2:
                    Demo_Ec_inKz_Setup_Stand(this, play);
                    break;
            }

            this->cueId = nextCueId;
        }
    }
}

void Demo_Ec_main_Kz_Wait(DemoEc* this, PlayState* play) {
    Demo_Ec_Animation_Base(this);
    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_Set_StartPos_npcdemopnt(this, play, 6);
    Demo_Ec_BGcheck(this, play);
    Demo_Ec_inKz_Check_DemoMode(this, play, 6);
}

void Demo_Ec_main_Kz_Standup(DemoEc* this, PlayState* play) {
    s32 animDone = Demo_Ec_Animation_Base(this);

    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_inKz_SetEyePattern_forStandup(this);
    Demo_Ec_BGcheck(this, play);
    Demo_Ec_inKz_Check_StandupToStand(this, animDone);
}

void Demo_Ec_main_Kz_Stand(DemoEc* this, PlayState* play) {
    Demo_Ec_Animation_Base(this);
    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_set_eye_pattern(this);
    Demo_Ec_BGcheck(this, play);
}

void Demo_Ec_draw_normal_Kz(DemoEc* this, PlayState* play) {
    static void* Demo_Ec_inKz_eye[] = { gKzEyeOpenTex, gKzEyeHalfTex, gKzEyeClosedTex, gKzEyeOpen2Tex };
    s32 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = Demo_Ec_inKz_eye[eyeTexIndex];

    Demo_Ec_draw_normal_1(this, play, eyeTexture, NULL, NULL, NULL);
}
