void Demo_Ec_main_init_Md(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &gMidoSkel);
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcMidoAnim, 0, 0.0f, false);
    Demo_Ec_Start_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_MIDO;
    this->drawConfig = EC_DRAW_MIDO;
    Demo_Ec_set_eye_Num(this, 3);
}

void Demo_Ec_inMd_Setup_Stand(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcAnim_008D1C, 2, -8.0f, false);
    this->updateMode = EC_UPDATE_20;
}

void Demo_Ec_inMd_Check_Animation_Stand(DemoEc* this, s32 changeAnim) {
    if (changeAnim) {
        Demo_Ec_Change_Anime(this, &gDemoEcAnim_009234, 0, 0.0f, false);
    }
}

void Demo_Ec_inMd_Check_DemoMode(DemoEc* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue;

    cue = Demo_Ec_Get_npcdemopnt(play, cueChannel);

    if (cue != NULL) {
        s32 nextCueId = cue->id;
        s32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 2:
                    Demo_Ec_inMd_Setup_Stand(this, play);
                    break;
            }

            this->cueId = nextCueId;
        }
    }
}

void Demo_Ec_main_Md_Wait(DemoEc* this, PlayState* play) {
    Demo_Ec_Animation_Base(this);
    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_Set_StartPos_npcdemopnt(this, play, 7);
    Demo_Ec_BGcheck(this, play);
    Demo_Ec_inMd_Check_DemoMode(this, play, 7);
}

void Demo_Ec_main_Md_Stand(DemoEc* this, PlayState* play) {
    s32 animDone = Demo_Ec_Animation_Base(this);

    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_set_eye_pattern(this);
    Demo_Ec_BGcheck(this, play);
    Demo_Ec_inMd_Check_Animation_Stand(this, animDone);
}

void Demo_Ec_draw_normal_Md(DemoEc* this, PlayState* play) {
    static void* Demo_Ec_inMd_eye[] = {
        gMidoEyeOpenTex,
        gMidoEyeHalfTex,
        gMidoEyeClosedTex,
        gMidoEyeAngryTex,
    };
    s32 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = Demo_Ec_inMd_eye[eyeTexIndex];

    Demo_Ec_draw_normal_1(this, play, eyeTexture, NULL, NULL, NULL);
}
