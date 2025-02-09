void Demo_Ec_main_init_Niw(DemoEc* this, PlayState* play) {
    AnimationHeader* animation;

    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &gCuccoSkel);
    Demo_Ec_Change_AnimeBank(this, play);

    if (this->actor.params == 22) {
        animation = &gDemoEcJumpingCuccoAnim;
    } else if (this->actor.params == 23) {
        animation = &gDemoEcJumpingCucco2Anim;
    } else {
        animation = &gDemoEcWalkingCuccoAnim;
    }

    Demo_Ec_Change_Anime(this, animation, 0, 0.0f, false);
    Demo_Ec_Start2_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_CUCCO;
    this->drawConfig = EC_DRAW_CUCCO;
}

void Demo_Ec_main_Niw_Wait(DemoEc* this, PlayState* play) {
    Demo_Ec_Animation_Base(this);
    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_BGcheck(this, play);
}

void Demo_Ec_draw_normal_Niw(DemoEc* this, PlayState* play) {
    Demo_Ec_draw_normal_1(this, play, NULL, NULL, NULL, NULL);
}
