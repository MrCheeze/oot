void DemoEc_InitIngo(DemoEc* this, PlayState* play) {
    DemoEc_UseDrawObject(this, play);
    DemoEc_InitSkelAnime(this, play, &gIngoSkel);
    DemoEc_UseAnimationObject(this, play);
    DemoEc_ChangeAnimation(this, &gDemoEcIngoAnim, 0, 0.0f, false);
    func_8096D64C(this, play);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->updateMode = EC_UPDATE_INGO;
    this->drawConfig = EC_DRAW_INGO;
}

void DemoEc_UpdateIngo(DemoEc* this, PlayState* play) {
    DemoEc_UpdateSkelAnime(this);
    func_8096D594(this, play);
    DemoEc_UpdateBgFlags(this, play);
}

void DemoEc_DrawIngo(DemoEc* this, PlayState* play) {
    DemoEc_DrawSkeleton(this, play, gIngoEyeClosed2Tex, gIngoRedTex, NULL, NULL);
}
