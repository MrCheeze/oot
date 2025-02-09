void DemoEc_InitMido(DemoEc* this, PlayState* play) {
    DemoEc_UseDrawObject(this, play);
    DemoEc_InitSkelAnime(this, play, &gMidoSkel);
    DemoEc_UseAnimationObject(this, play);
    DemoEc_ChangeAnimation(this, &gDemoEcMidoAnim, 0, 0.0f, false);
    func_8096D5D4(this, play);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->updateMode = EC_UPDATE_MIDO;
    this->drawConfig = EC_DRAW_MIDO;
    DemoEc_SetEyeTexIndex(this, 3);
}

void func_8096F4FC(DemoEc* this, PlayState* play) {
    DemoEc_UseAnimationObject(this, play);
    DemoEc_ChangeAnimation(this, &gDemoEcAnim_008D1C, 2, -8.0f, false);
    this->updateMode = EC_UPDATE_20;
}

void func_8096F544(DemoEc* this, s32 changeAnim) {
    if (changeAnim) {
        DemoEc_ChangeAnimation(this, &gDemoEcAnim_009234, 0, 0.0f, false);
    }
}

void func_8096F578(DemoEc* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue;

    cue = DemoEc_GetCue(play, cueChannel);

    if (cue != NULL) {
        s32 nextCueId = cue->id;
        s32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 2:
                    func_8096F4FC(this, play);
                    break;
            }

            this->cueId = nextCueId;
        }
    }
}

void DemoEc_UpdateMido(DemoEc* this, PlayState* play) {
    DemoEc_UpdateSkelAnime(this);
    func_8096D594(this, play);
    DemoEc_SetStartPosRotFromCue(this, play, 7);
    DemoEc_UpdateBgFlags(this, play);
    func_8096F578(this, play, 7);
}

void func_8096F640(DemoEc* this, PlayState* play) {
    s32 animDone = DemoEc_UpdateSkelAnime(this);

    func_8096D594(this, play);
    DemoEc_UpdateEyes(this);
    DemoEc_UpdateBgFlags(this, play);
    func_8096F544(this, animDone);
}

void DemoEc_DrawMido(DemoEc* this, PlayState* play) {
    static void* eyeTextures[] = {
        gMidoEyeOpenTex,
        gMidoEyeHalfTex,
        gMidoEyeClosedTex,
        gMidoEyeAngryTex,
    };
    s32 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeleton(this, play, eyeTexture, NULL, NULL, NULL);
}
