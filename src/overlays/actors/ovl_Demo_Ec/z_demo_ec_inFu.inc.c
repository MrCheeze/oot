void DemoEc_InitWindmillMan(DemoEc* this, PlayState* play) {
    DemoEc_UseDrawObject(this, play);
    DemoEc_InitSkelAnime(this, play, &gWindmillManSkel);
    DemoEc_UseAnimationObject(this, play);
    DemoEc_ChangeAnimation(this, &gDemoEcWindmillManAnim, 0, 0.0f, false);
    func_8096D5D4(this, play);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->updateMode = EC_UPDATE_WINDMILL_MAN;
    this->drawConfig = EC_DRAW_WINDMILL_MAN;
}

void DemoEc_UpdateWindmillMan(DemoEc* this, PlayState* play) {
    DemoEc_UpdateSkelAnime(this);
    func_8096D594(this, play);
    DemoEc_UpdateBgFlags(this, play);
}

void DemoEc_DrawWindmillMan(DemoEc* this, PlayState* play) {
    DemoEc_DrawSkeleton(this, play, gWindmillManEyeClosedTex, gWindmillManMouthAngryTex, NULL, NULL);
}
