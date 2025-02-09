/**
 * Sets up Ruto in her arms-crossing pose. Used in the Water Trial in Ganon's Castle and in the
 * Chamber of Sages during the "Sealing Ganon" cutscene.
 */
void EnRu2_InitWaterTrial(EnRu2* this, PlayState* play) {
    EnRu2_AnimationChange(this, &gAdultRutoCrossingArmsAnim, 2, 0.0f, 0);
    this->action = ENRU2_WATER_TRIAL_INVISIBLE;
    this->actor.shape.shadowAlpha = 0;
}

void EnRu2_PlayWhiteOutSound(void) {
    Sfx_PlaySfxCentered2(NA_SE_SY_WHITE_OUT_T);
}

/**
 * Spawns the ball of light that replaces Ruto's actor in the Water Trial.
 */
void EnRu2_SpawnLightBall(EnRu2* this, PlayState* play) {
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       kREG(19) + 24.0f + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 8);
}

/**
 * Checks to see if it's time for Ruto to fade in while crossing her arms.
 */
void EnRu2_CheckFadeIn(EnRu2* this, PlayState* play) {
    if (EnRu2_CheckCueMatchingId(this, play, 4, 3)) {
        this->action = ENRU2_WATER_TRIAL_FADE;
        this->drawConfig = ENRU2_DRAW_XLU;
        this->alpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->fadeTimer = 0.0f;
        EnRu2_PlayWhiteOutSound();
    }
}

/**
 * Fades Ruto's actor in or out. Both happen during the Water Trial.
 */
void EnRu2_Fade(EnRu2* this, PlayState* play) {
    f32* fadeTimer = &this->fadeTimer;
    s32 alpha;

    if (EnRu2_CheckCueMatchingId(this, play, 4, 3)) {
        *fadeTimer += 1.0f;
        if (*fadeTimer >= kREG(5) + 10.0f) {
            this->action = ENRU2_AWAIT_SPAWN_LIGHT_BALL;
            this->drawConfig = ENRU2_DRAW_OPA;
            *fadeTimer = kREG(5) + 10.0f;
            this->alpha = 255;
            this->actor.shape.shadowAlpha = 0xFF;
            return;
        }
    } else {
        *fadeTimer -= 1.0f;
        if (*fadeTimer <= 0.0f) {
            this->action = ENRU2_WATER_TRIAL_INVISIBLE;
            this->drawConfig = ENRU2_DRAW_NOTHING;
            *fadeTimer = 0.0f;
            this->alpha = 0;
            this->actor.shape.shadowAlpha = 0;
            return;
        }
    }
    alpha = (*fadeTimer / (kREG(5) + 10.0f)) * 255.0f;
    this->alpha = alpha;
    this->actor.shape.shadowAlpha = alpha;
}

/**
 * Checks to see if it's time for Ruto to fade out while her arms are crossed.
 */
void EnRu2_CheckFadeOut(EnRu2* this, PlayState* play) {
    if (EnRu2_CheckCueNotMatchingId(this, play, 4, 3)) {
        this->action = ENRU2_WATER_TRIAL_FADE;
        this->drawConfig = ENRU2_DRAW_XLU;
        this->fadeTimer = kREG(5) + 10.0f;
        this->alpha = 255;
        if (!this->isLightBall) {
            EnRu2_SpawnLightBall(this, play);
            this->isLightBall = true;
        }
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void EnRu2_WaterTrialInvisible(EnRu2* this, PlayState* play) {
    EnRu2_CheckFadeIn(this, play);
#if DEBUG_FEATURES
    func_80AF26D0(this, play);
#endif
}

void EnRu2_WaterTrialFade(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_Fade(this, play);
#if DEBUG_FEATURES
    func_80AF26D0(this, play);
#endif
}

void EnRu2_AwaitSpawnLightBall(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_CheckFadeOut(this, play);
#if DEBUG_FEATURES
    func_80AF26D0(this, play);
#endif
}

void EnRu2_DrawXlu(EnRu2* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* tex = sEyeTextures[eyeIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ru2_inKenjyanomaDemo02.c", 264);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(tex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(tex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, &D_80116280[0]);

    POLY_XLU_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL,
                                       NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ru2_inKenjyanomaDemo02.c", 291);
}
