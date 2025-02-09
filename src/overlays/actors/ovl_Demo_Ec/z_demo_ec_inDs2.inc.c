void Demo_Ec_main_init_Ds2(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &object_ds2_Skel_004258);
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcPotionShopOwnerAnim, 0, 0.0f, false);
    Demo_Ec_Start_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_POTION_SHOP_OWNER;
    this->drawConfig = EC_DRAW_POTION_SHOP_OWNER;
}

void Demo_Ec_main_Ds2_Wait(DemoEc* this, PlayState* play) {
    Demo_Ec_Animation_Base(this);
    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_set_eye_pattern(this);
    Demo_Ec_BGcheck(this, play);
}

void Demo_Ec_draw_normal_Ds2(DemoEc* this, PlayState* play) {
    static void* Demo_Ec_inDs2_eye[] = {
        gPotionShopkeeperEyeOpenTex,
        gPotionShopkeeperEyeHalfTex,
        gPotionShopkeeperEyeClosedTex,
    };
    s32 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = Demo_Ec_inDs2_eye[eyeTexIndex];

    Demo_Ec_draw_normal_1(this, play, eyeTexture, NULL, NULL, NULL);
}
