void DemoEc_InitBeardedMan(DemoEc* this, PlayState* play) {
    DemoEc_UseDrawObject(this, play);
    DemoEc_InitSkelAnime(this, play, &gHylianMan1Skel);
    DemoEc_UseAnimationObject(this, play);
    DemoEc_ChangeAnimation(this, &gDemoEcOldManAnim, 0, 0.0f, false);
    func_8096D5D4(this, play);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->updateMode = EC_UPDATE_BEARDED_MAN;
    this->drawConfig = EC_DRAW_BEARDED_MAN;
}

void DemoEc_UpdateBeardedMan(DemoEc* this, PlayState* play) {
    DemoEc_UpdateSkelAnime(this);
    func_8096D594(this, play);
    DemoEc_UpdateEyes(this);
    DemoEc_UpdateBgFlags(this, play);
}

void DemoEc_DrawBeardedMan(DemoEc* this, PlayState* play) {
    static void* eyeTextures[] = {
        gHylianMan1BeardedEyeOpenTex,
        gHylianMan1BeardedEyeHalfTex,
        gHylianMan1BeardedEyeClosedTex,
    };
    static u8 color1[] = { 255, 255, 255, 255 };
    static u8 color2[] = { 255, 255, 255, 255 };
    s32 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeletonCustomColor(this, play, eyeTexture, NULL, color1, color2, NULL, NULL);
}
