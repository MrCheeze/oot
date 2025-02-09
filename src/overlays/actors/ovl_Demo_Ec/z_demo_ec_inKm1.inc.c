void Demo_Ec_main_init_Km1(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &gKm1Skel);
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcKokiriAnim, 0, 0.0f, false);
    Demo_Ec_Start_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_KOKIRI_BOY;
    this->drawConfig = EC_DRAW_KOKIRI_BOY;
}

void Demo_Ec_main_init_Km1_dance(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &gKm1Skel);
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcDancingKokiriAnim, 0, 0.0f, false);
    Demo_Ec_Start_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_DANCING_KOKIRI_BOY;
    this->drawConfig = EC_DRAW_KOKIRI_BOY;
}

void Demo_Ec_main_Km1_Wait(DemoEc* this, PlayState* play) {
    Demo_Ec_Animation_Base(this);
    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_BGcheck(this, play);
}

void Demo_Ec_main_Km1_Dance(DemoEc* this, PlayState* play) {
    Demo_Ec_main_Km1_Wait(this, play);
}

void Demo_Ec_draw_normal_Km1(DemoEc* this, PlayState* play) {
    static u8 color1[] = { 0, 130, 70, 255 };
    static u8 color2[] = { 110, 170, 20, 255 };

    Demo_Ec_draw_normal_2(this, play, NULL, NULL, color1, color2, NULL, NULL);
}

