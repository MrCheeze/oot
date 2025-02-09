void DemoEc_InitTalon(DemoEc* this, PlayState* play) {
    DemoEc_UseDrawObject(this, play);
    DemoEc_InitSkelAnime(this, play, &gTalonSkel);
    DemoEc_UseAnimationObject(this, play);
    DemoEc_ChangeAnimation(this, &gDemoEcTalonAnim, 0, 0.0f, false);
    func_8096D64C(this, play);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->updateMode = EC_UPDATE_TALON;
    this->drawConfig = EC_DRAW_TALON;
}

void DemoEc_UpdateTalon(DemoEc* this, PlayState* play) {
    DemoEc_UpdateSkelAnime(this);
    func_8096D594(this, play);
    DemoEc_UpdateBgFlags(this, play);
}

void DemoEc_DrawTalon(DemoEc* this, PlayState* play) {
    DemoEc_DrawSkeleton(this, play, gTalonEyeClosed2Tex, gTalonRedTex, NULL, NULL);
}
