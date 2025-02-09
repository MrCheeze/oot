void func_8098ECF4(DemoSa* this, PlayState* play) {
    s32 pad[2];
    SkelAnime* skelAnime = &this->skelAnime;
    f32 frameCount = Animation_GetLastFrame(&gSariaSealGanonAnim);

    SkelAnime_InitFlex(play, skelAnime, &gSariaSkel, NULL, NULL, NULL, 0);
    Animation_Change(skelAnime, &gSariaSealGanonAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
    this->action = 7;
    this->actor.shape.shadowAlpha = 0;
    DemoSa_SetEyeIndex(this, SARIA_EYE_CLOSED);
    DemoSa_SetMouthIndex(this, SARIA_MOUTH_CLOSED);
}

void func_8098EDB0(DemoSa* this) {
    f32 curFrame = this->skelAnime.curFrame;

    if ((this->skelAnime.mode == 2) && (curFrame >= 32.0f)) {
        DemoSa_SetEyeIndex(this, SARIA_EYE_HALF);
        DemoSa_SetMouthIndex(this, SARIA_MOUTH_CLOSED2);
    }
}

void func_8098EE08(void) {
    Sfx_PlaySfxCentered2(NA_SE_SY_WHITE_OUT_T);
}

void func_8098EE28(DemoSa* this, PlayState* play) {
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       (kREG(23) + 25.0f) + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 4);
}

void func_8098EEA8(DemoSa* this, PlayState* play) {
    if (func_8098E654(this, play, 4, 4)) {
        this->action = 8;
        this->drawConfig = 2;
        this->alpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->unk_1A0 = 0.0f;
        func_8098EE08();
    }
}

void func_8098EEFC(DemoSa* this, PlayState* play) {
    s32 alpha = 255;
    f32* unk_1A0 = &this->unk_1A0;

    if (func_8098E654(this, play, 4, 4)) {
        *unk_1A0 += 1.0f;
        if ((kREG(5) + 10.0f) <= *unk_1A0) {
            this->action = 9;
            this->drawConfig = 1;
            *unk_1A0 = kREG(5) + 10.0f;
            this->alpha = alpha;
            this->actor.shape.shadowAlpha = alpha;
            return;
        }
    } else {
        *unk_1A0 -= 1.0f;
        if (*unk_1A0 <= 0.0f) {
            this->action = 7;
            this->drawConfig = 0;
            *unk_1A0 = 0.0f;
            this->alpha = 0;
            this->actor.shape.shadowAlpha = 0;
            return;
        }
    }
    this->actor.shape.shadowAlpha = this->alpha = (*unk_1A0 / (kREG(5) + 10.0f)) * 255.0f;
}

void func_8098F050(DemoSa* this, PlayState* play) {
    if (func_8098E6A0(this, play, 4, 4)) {
        this->action = 8;
        this->drawConfig = 2;
        this->unk_1A0 = kREG(5) + 10.0f;
        this->alpha = 255;
        if (this->unk_1A8 == 0) {
            func_8098EE28(this, play);
            this->unk_1A8 = 1;
        }
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_8098F0E8(DemoSa* this, PlayState* play) {
    func_8098EEA8(this, play);
#if DEBUG_FEATURES
    func_8098E554(this, play);
#endif
}

void func_8098F118(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098E480(this);
    func_8098EEFC(this, play);
#if DEBUG_FEATURES
    func_8098E554(this, play);
#endif
}

void func_8098F16C(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098EDB0(this);
    func_8098F050(this, play);
#if DEBUG_FEATURES
    func_8098E554(this, play);
#endif
}

void DemoSa_DrawXlu(DemoSa* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* sp78 = sEyeTextures[eyeIndex];
    s16 mouthIndex = this->mouthIndex;
    s32 pad2;
    void* sp6C = sMouthTextures[mouthIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_sa_inKenjyanomaDemo02.c", 296);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sp78));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sp78));
    gSPSegment(POLY_XLU_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(sp6C));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, D_80116280);

    POLY_XLU_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL,
                                       NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_sa_inKenjyanomaDemo02.c", 325);
}
