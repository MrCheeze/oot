void Demo_Ec_main_init_Fs(DemoEc* this, PlayState* play) {
    Demo_Ec_Change_ShapeBank(this, play);
    Demo_Ec_Setup_Mdl(this, play, &gFishingOwnerSkel);
    Demo_Ec_Change_AnimeBank(this, play);
    Demo_Ec_Change_Anime(this, &gDemoEcPotionShopOwnerAnim, 0, 0.0f, false);
    Demo_Ec_Start_Movement_byAnimation(this, play);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->updateMode = EC_UPDATE_FISHING_MAN;
    this->drawConfig = EC_DRAW_FISHING_MAN;
}

void Demo_Ec_main_Fs_Wait(DemoEc* this, PlayState* play) {
    Demo_Ec_Animation_Base(this);
    Demo_Ec_Movement_byAnimation_CorrectNone(this, play);
    Demo_Ec_set_eye_pattern(this);
    Demo_Ec_BGcheck(this, play);
}

void Demo_Ec_AfterDraw_Fs(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    DemoEc* this = (DemoEc*)thisx;

    if ((limbIndex == 8) && !(HIGH_SCORE(HS_FISHING) & HS_FISH_STOLE_HAT)) {
        gSPDisplayList((*gfx)++, SEGMENTED_TO_VIRTUAL(gFishingOwnerHatDL));
    }
}

void Demo_Ec_draw_normal_Fs(DemoEc* this, PlayState* play) {
    static void* Demo_Ec_inFs_eye[] = {
        gFishingOwnerEyeOpenTex,
        gFishingOwnerEyeHalfTex,
        gFishingOwnerEyeClosedTex,
    };
    s32 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = Demo_Ec_inFs_eye[eyeTexIndex];

    Demo_Ec_draw_normal_1(this, play, eyeTexture, NULL, NULL, Demo_Ec_AfterDraw_Fs);
}
