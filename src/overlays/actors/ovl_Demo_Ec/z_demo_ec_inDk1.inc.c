void Demo_Ec_main_init_Dk1(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &object_toryo_Skel_007150);
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcCarpenterAnim, 0, 0.0f, false);
    Demo_Ec_Start_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_BOSS_CARPENTER;
    this->drawConfig = EC_DRAW_BOSS_CARPENTER;
}

void Demo_Ec_main_Dk1_Wait(DemoEc* this, PlayState* play) {
    Demo_Ec_Animation_Base(this);
    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_BGcheck(this, play);
}

void Demo_Ec_draw_normal_Dk1(DemoEc* this, PlayState* play) {
    Demo_Ec_draw_normal_1(this, play, NULL, NULL, NULL, NULL);
}
