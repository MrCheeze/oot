void DemoEc_InitWoman(DemoEc* this, PlayState* play) {
    DemoEc_UseDrawObject(this, play);
    DemoEc_InitSkelAnime(this, play, &gHylianWoman2Skel);
    DemoEc_UseAnimationObject(this, play);
    DemoEc_ChangeAnimation(this, &gDemoEcOldManAnim, 0, 0.0f, false);
    func_8096D5D4(this, play);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->updateMode = EC_UPDATE_WOMAN;
    this->drawConfig = EC_DRAW_WOMAN;
}

void DemoEc_UpdateWoman(DemoEc* this, PlayState* play) {
    DemoEc_UpdateSkelAnime(this);
    func_8096D594(this, play);
    DemoEc_UpdateEyes(this);
    DemoEc_UpdateBgFlags(this, play);
}

void DemoEc_DrawWoman(DemoEc* this, PlayState* play) {
    static void* eyeTextures[] = {
        gHylianWoman2EyeOpenTex,
        gHylianWoman2EyeHalfTex,
        gHylianWoman2EyeClosedTex,
    };
    s32 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeleton(this, play, eyeTexture, NULL, NULL, NULL);
}
