void Demo_Ec_main_init_Ahg(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &gHylianMan1Skel);
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcOldManAnim, 0, 0.0f, false);
    Demo_Ec_Start_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_BEARDED_MAN;
    this->drawConfig = EC_DRAW_BEARDED_MAN;
}

void Demo_Ec_main_Ahg_Wait(DemoEc* this, PlayState* play) {
    Demo_Ec_Animation_Base(this);
    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_set_eye_pattern(this);
    Demo_Ec_BGcheck(this, play);
}

void Demo_Ec_draw_normal_Ahg(DemoEc* this, PlayState* play) {
    static void* Demo_Ec_inAhg_eye[] = {
        gHylianMan1BeardedEyeOpenTex,
        gHylianMan1BeardedEyeHalfTex,
        gHylianMan1BeardedEyeClosedTex,
    };
    static u8 color1[] = { 255, 255, 255, 255 };
    static u8 color2[] = { 255, 255, 255, 255 };
    s32 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = Demo_Ec_inAhg_eye[eyeTexIndex];

    Demo_Ec_draw_normal_2(this, play, eyeTexture, NULL, color1, color2, NULL, NULL);
}
