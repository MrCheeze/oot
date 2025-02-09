void Demo_Ec_main_init_Os(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &gHappyMaskSalesmanSkel);
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcPotionShopOwnerAnim, 0, 0.0f, false);
    Demo_Ec_Start_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_MASK_SHOP_OWNER;
    this->drawConfig = EC_DRAW_MASK_SHOP_OWNER;
}

void Demo_Ec_main_Os_Wait(DemoEc* this, PlayState* play) {
    Demo_Ec_Animation_Base(this);
    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_BGcheck(this, play);
}

void Demo_Ec_draw_normal_Os(DemoEc* this, PlayState* play) {
    Demo_Ec_draw_normal_1(this, play, gHappyMaskSalesmanEyeClosedTex, NULL, NULL, NULL);
}
