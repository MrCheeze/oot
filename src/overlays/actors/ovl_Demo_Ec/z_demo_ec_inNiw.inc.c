void DemoEc_InitCucco(DemoEc* this, PlayState* play) {
    AnimationHeader* animation;

    DemoEc_UseDrawObject(this, play);
    DemoEc_InitSkelAnime(this, play, &gCuccoSkel);
    DemoEc_UseAnimationObject(this, play);

    if (this->actor.params == 22) {
        animation = &gDemoEcJumpingCuccoAnim;
    } else if (this->actor.params == 23) {
        animation = &gDemoEcJumpingCucco2Anim;
    } else {
        animation = &gDemoEcWalkingCuccoAnim;
    }

    DemoEc_ChangeAnimation(this, animation, 0, 0.0f, false);
    func_8096D64C(this, play);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->updateMode = EC_UPDATE_CUCCO;
    this->drawConfig = EC_DRAW_CUCCO;
}

void DemoEc_UpdateCucco(DemoEc* this, PlayState* play) {
    DemoEc_UpdateSkelAnime(this);
    func_8096D594(this, play);
    DemoEc_UpdateBgFlags(this, play);
}

void DemoEc_DrawCucco(DemoEc* this, PlayState* play) {
    DemoEc_DrawSkeleton(this, play, NULL, NULL, NULL, NULL);
}
