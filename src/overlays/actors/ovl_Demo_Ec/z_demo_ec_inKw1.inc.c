void Demo_Ec_main_init_Kw1(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &gKw1Skel);
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcKokiriAnim, 0, 0.0f, false);
    Demo_Ec_Start_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_KOKIRI_GIRL;
    this->drawConfig = EC_DRAW_KOKIRI_GIRL;
}

void Demo_Ec_main_init_Kw1_dance(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &gKw1Skel);
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcDancingKokiriAnim, 0, 0.0f, false);
    Demo_Ec_Start_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_DANCING_KOKIRI_GIRL;
    this->drawConfig = EC_DRAW_KOKIRI_GIRL;
}

void Demo_Ec_main_Kw1_Wait(DemoEc* this, PlayState* play) {
    Demo_Ec_Animation_Base(this);
    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_set_eye_pattern(this);
    Demo_Ec_BGcheck(this, play);
}

void Demo_Ec_main_Kw1_Dance(DemoEc* this, PlayState* play) {
    Demo_Ec_main_Kw1_Wait(this, play);
}

void Demo_Ec_draw_normal_Kw1(DemoEc* this, PlayState* play) {
    static void* Demo_Ec_inKw1_eye[] = {
        gKw1EyeOpenTex,
        gKw1EyeHalfTex,
        gKw1EyeClosedTex,
    };
    static u8 color1[] = { 70, 190, 60, 255 };
    static u8 color2[] = { 100, 30, 0, 255 };
    s32 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = Demo_Ec_inKw1_eye[eyeTexIndex];

    Demo_Ec_draw_normal_2(this, play, eyeTexture, NULL, color1, color2, NULL, NULL);
}
