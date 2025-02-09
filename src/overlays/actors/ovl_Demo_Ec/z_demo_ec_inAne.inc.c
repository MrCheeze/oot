void DemoEc_InitCuccoLady(DemoEc* this, PlayState* play) {
    DemoEc_UseDrawObject(this, play);
    DemoEc_InitSkelAnime(this, play, &gCuccoLadySkel);
    DemoEc_UseAnimationObject(this, play);
    DemoEc_ChangeAnimation(this, &gDemoEcCuccoLadyAnim, 0, 0.0f, false);
    func_8096D5D4(this, play);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->updateMode = EC_UPDATE_CUCCO_LADY;
    this->drawConfig = EC_DRAW_CUCCO_LADY;
}

void DemoEc_UpdateCuccoLady(DemoEc* this, PlayState* play) {
    DemoEc_UpdateSkelAnime(this);
    func_8096D594(this, play);
    DemoEc_UpdateEyes(this);
    DemoEc_UpdateBgFlags(this, play);
}

void DemoEc_DrawCuccoLady(DemoEc* this, PlayState* play) {
    static void* eyeTextures[] = {
        gCuccoLadyEyeOpenTex,
        gCuccoLadyEyeHalfTex,
        gCuccoLadyEyeClosedTex,
    };
    s32 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeleton(this, play, eyeTexture, NULL, NULL, NULL);
}
