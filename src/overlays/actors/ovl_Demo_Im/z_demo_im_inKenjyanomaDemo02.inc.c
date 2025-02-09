void func_80985860(DemoIm* this, PlayState* play) {
    DemoIm_ChangeAnim(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->action = 7;
    this->actor.shape.shadowAlpha = 0;
}

void func_809858A8(void) {
    Sfx_PlaySfxCentered2(NA_SE_SY_WHITE_OUT_T);
}

void DemoIm_SpawnLightBall(DemoIm* this, PlayState* play) {
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       (kREG(17) + 24.0f) + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 6);
}

void func_80985948(DemoIm* this, PlayState* play) {
    if (func_809850E8(this, play, 4, 5)) {
        Animation_Change(&this->skelAnime, &gImpaPrepareSealGanonAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gImpaPrepareSealGanonAnim), ANIMMODE_ONCE, 0.0f);
        this->action = 8;
        this->drawConfig = 2;
        this->alpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->unk_268 = 0.0f;
        func_809858A8();
    }
}

void func_809859E0(DemoIm* this, PlayState* play) {
    f32* unk_268 = &this->unk_268;
    s32 alpha = 255;

    if (func_809850E8(this, play, 4, 5)) {
        *unk_268 += 1.0f;
        if (*unk_268 >= kREG(5) + 10.0f) {
            this->action = 9;
            this->drawConfig = 1;
            *unk_268 = kREG(5) + 10.0f;
            this->alpha = this->actor.shape.shadowAlpha = alpha;
            return;
        }
    } else {
        *unk_268 -= 1.0f;
        if (*unk_268 <= 0.0f) {
            this->action = 7;
            this->drawConfig = 0;
            *unk_268 = 0.0f;
            this->alpha = 0;
            this->actor.shape.shadowAlpha = 0;
            return;
        }
    }
    this->actor.shape.shadowAlpha = this->alpha = (*unk_268 / (kREG(5) + 10.0f)) * 255.0f;
}

void func_80985B34(DemoIm* this, PlayState* play) {
    if (func_80985134(this, play, 4, 5)) {
        Animation_Change(&this->skelAnime, &gImpaSealGanonAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaSealGanonAnim),
                         ANIMMODE_ONCE, -8.0f);
        this->action = 8;
        this->drawConfig = 2;
        this->unk_268 = kREG(5) + 10.0f;
        this->alpha = 255;
        if (this->unk_270 == 0) {
            DemoIm_SpawnLightBall(this, play);
            this->unk_270 = 1;
        }
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_80985C10(DemoIm* this, PlayState* play) {
    func_80985948(this, play);
#if DEBUG_FEATURES
    func_80984C8C(this, play);
#endif
}

void func_80985C40(DemoIm* this, PlayState* play) {
    DemoIm_UpdateBgCheckInfo(this, play);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_809859E0(this, play);
#if DEBUG_FEATURES
    func_80984C8C(this, play);
#endif
}

void func_80985C94(DemoIm* this, PlayState* play) {
    DemoIm_UpdateBgCheckInfo(this, play);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_80985B34(this, play);
#if DEBUG_FEATURES
    func_80984C8C(this, play);
#endif
}

void DemoIm_DrawTranslucent(DemoIm* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* eyeTex = sEyeTextures[eyeIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_im_inKenjyanomaDemo02.c", 281);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, &D_80116280[0]);

    POLY_XLU_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL,
                                       NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_im_inKenjyanomaDemo02.c", 308);
}
