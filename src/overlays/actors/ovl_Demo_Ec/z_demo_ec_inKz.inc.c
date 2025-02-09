void DemoEc_InitKingZora(DemoEc* this, PlayState* play) {
    DemoEc_UseDrawObject(this, play);
    DemoEc_InitSkelAnime(this, play, &gKzSkel);
    DemoEc_UseAnimationObject(this, play);
    DemoEc_ChangeAnimation(this, &gDemoEcKingZoraAnim, 0, 0.0f, false);
    func_8096D5D4(this, play);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->updateMode = EC_UPDATE_KING_ZORA;
    this->drawConfig = EC_DRAW_KING_ZORA;
    DemoEc_SetEyeTexIndex(this, 3);
}

void func_8096F1D4(DemoEc* this) {
    f32 currentFrame = this->skelAnime.curFrame;

    if (currentFrame <= 32.0f) {
        DemoEc_SetEyeTexIndex(this, 3);
    } else {
        DemoEc_UpdateEyes(this);
    }
}

void func_8096F224(DemoEc* this, PlayState* play) {
    DemoEc_UseAnimationObject(this, play);
    DemoEc_ChangeAnimation(this, &gDemoEcAnim_006930, 2, -8.0f, false);
    this->updateMode = EC_UPDATE_17;
}

void func_8096F26C(DemoEc* this, s32 arg1) {
    if (arg1 != 0) {
        DemoEc_ChangeAnimation(this, &gDemoEcAnim_006220, 0, 0.0f, false);
        this->updateMode = EC_UPDATE_18;
    }
}

void func_8096F2B0(DemoEc* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = DemoEc_GetCue(play, cueChannel);

    if (cue != NULL) {
        s32 nextCueId = cue->id;
        s32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 2:
                    func_8096F224(this, play);
                    break;
            }

            this->cueId = nextCueId;
        }
    }
}

void DemoEc_UpdateKingZora(DemoEc* this, PlayState* play) {
    DemoEc_UpdateSkelAnime(this);
    func_8096D594(this, play);
    DemoEc_SetStartPosRotFromCue(this, play, 6);
    DemoEc_UpdateBgFlags(this, play);
    func_8096F2B0(this, play, 6);
}

void func_8096F378(DemoEc* this, PlayState* play) {
    s32 animDone = DemoEc_UpdateSkelAnime(this);

    func_8096D594(this, play);
    func_8096F1D4(this);
    DemoEc_UpdateBgFlags(this, play);
    func_8096F26C(this, animDone);
}

void func_8096F3D4(DemoEc* this, PlayState* play) {
    DemoEc_UpdateSkelAnime(this);
    func_8096D594(this, play);
    DemoEc_UpdateEyes(this);
    DemoEc_UpdateBgFlags(this, play);
}

void DemoEc_DrawKingZora(DemoEc* this, PlayState* play) {
    static void* eyeTextures[] = { gKzEyeOpenTex, gKzEyeHalfTex, gKzEyeClosedTex, gKzEyeOpen2Tex };
    s32 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeleton(this, play, eyeTexture, NULL, NULL, NULL);
}
