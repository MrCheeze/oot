void Demo_Ec_main_init_In(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &gIngoSkel);
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcIngoAnim, 0, 0.0f, false);
    Demo_Ec_Start2_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_INGO;
    this->drawConfig = EC_DRAW_INGO;
}

void Demo_Ec_main_In_Wait(DemoEc* this, PlayState* play) {
    Demo_Ec_Animation_Base(this);
    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_BGcheck(this, play);
}

void Demo_Ec_draw_normal_In(DemoEc* this, PlayState* play) {
    Demo_Ec_draw_normal_1(this, play, gIngoEyeClosed2Tex, gIngoRedTex, NULL, NULL);
}
