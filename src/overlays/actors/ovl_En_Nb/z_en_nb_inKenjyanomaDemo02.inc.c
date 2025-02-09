void EnNb_SetupLightArrowOrSealingCs(EnNb* this, PlayState* play) {
    EnNb_SetCurrentAnim(this, &gNabooruPuttingHandsTogetherCastingMagicAnim, 2, 0.0f, 0);
    this->action = NB_ACTION_7;
    this->actor.shape.shadowAlpha = 0;
}

void EnNb_PlaySealingSfx(void) {
    Sfx_PlaySfxCentered2(NA_SE_SY_WHITE_OUT_T);
}

void EnNb_InitializeDemo6K(EnNb* this, PlayState* play) {
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       kREG(21) + 22.0f + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 7);
}

void EnNb_SetupHide(EnNb* this, PlayState* play) {
    if (func_80AB1390(this, play, 4, 1)) {
        this->action = NB_SEAL_HIDE;
        this->drawMode = NB_DRAW_HIDE;
        this->alpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->alphaTimer = 0.0f;
        EnNb_PlaySealingSfx();
    }
}

void EnNb_CheckToFade(EnNb* this, PlayState* play) {
    f32* alphaTimer = &this->alphaTimer;
    s32 alpha;

    if (func_80AB1390(this, play, 4, 1)) {
        *alphaTimer += 1.0f;
        if (*alphaTimer >= kREG(5) + 10.0f) {
            this->action = NB_ACTION_9;
            this->drawMode = NB_DRAW_DEFAULT;
            *alphaTimer = kREG(5) + 10.0f;
            this->alpha = 255;
            this->actor.shape.shadowAlpha = 0xFF;
            return;
        }
    } else {
        *alphaTimer -= 1.0f;
        if (*alphaTimer <= 0.0f) {
            this->action = NB_ACTION_7;
            this->drawMode = NB_DRAW_NOTHING;
            *alphaTimer = 0.0f;
            this->alpha = 0;
            this->actor.shape.shadowAlpha = 0;
            return;
        }
    }

    alpha = (*alphaTimer / (kREG(5) + 10.0f)) * 255.0f;
    this->alpha = alpha;
    this->actor.shape.shadowAlpha = alpha;
}

void EnNb_SetupLightOrb(EnNb* this, PlayState* play) {
    if (func_80AB13D8(this, play, 4, 1)) {
        this->action = NB_SEAL_HIDE;
        this->drawMode = NB_DRAW_HIDE;
        this->alphaTimer = kREG(5) + 10.0f;
        this->alpha = 255;

        if (this->flag == 0) {
            EnNb_InitializeDemo6K(this, play);
            this->flag = 1;
        }

        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void EnNb_Hide(EnNb* this, PlayState* play) {
    EnNb_SetupHide(this, play);
#if DEBUG_FEATURES
    func_80AB1210(this, play);
#endif
}

void EnNb_Fade(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_CheckToFade(this, play);
#if DEBUG_FEATURES
    func_80AB1210(this, play);
#endif
}

void EnNb_CreateLightOrb(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_SetupLightOrb(this, play);
#if DEBUG_FEATURES
    func_80AB1210(this, play);
#endif
}

void EnNb_DrawTransparency(EnNb* this, PlayState* play) {
    s32 pad[2];
    s16 eyeSegIdx = this->eyeIdx;
    void* eyeTex = sEyeTextures[eyeSegIdx];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_nb_inKenjyanomaDemo02.c", 263);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, &D_80116280[0]);
    POLY_XLU_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL,
                                       NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_nb_inKenjyanomaDemo02.c", 290);
}
