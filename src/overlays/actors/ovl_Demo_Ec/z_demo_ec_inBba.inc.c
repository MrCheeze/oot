void Demo_Ec_main_init_Bba(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &gHylianOldWomanSkel);
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcOldManAnim, 0, 0.0f, false);
    Demo_Ec_Start_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_OLD_WOMAN;
    this->drawConfig = EC_DRAW_OLD_WOMAN;
}

void Demo_Ec_main_Bba_Wait(DemoEc* this, PlayState* play) {
    Demo_Ec_Animation_Base(this);
    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_BGcheck(this, play);
}

void Demo_Ec_draw_normal_Bba(DemoEc* this, PlayState* play) {
    Demo_Ec_draw_normal_1(this, play, &gHylianOldWomanEyeTex, NULL, NULL, NULL);
}
