void DemoDu_InitCs_AfterGanon(DemoDu* this, PlayState* play) {
    s32 pad[3];
    f32 lastFrame = Animation_GetLastFrame(&gDaruniaSageFormationAnim);

    SkelAnime_InitFlex(play, &this->skelAnime, &gDaruniaSkel, NULL, NULL, NULL, 0);
    Animation_Change(&this->skelAnime, &gDaruniaSageFormationAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    this->updateIndex = CS_CHAMBERAFTERGANON_SUBSCENE(0);
    this->actor.shape.shadowAlpha = 0;
}

void DemoDu_CsPlaySfx_WhiteOut(void) {
    Sfx_PlaySfxCentered2(NA_SE_SY_WHITE_OUT_T);
}

void DemoDu_CsAfterGanon_SpawnDemo6K(DemoDu* this, PlayState* play) {
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       kREG(16) + 22.0f + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 3);
}

void DemoDu_CsAfterGanon_AdvanceTo01(DemoDu* this, PlayState* play) {
    if (DemoDu_CheckForCue(this, play, 4, 2)) {
        this->updateIndex = CS_CHAMBERAFTERGANON_SUBSCENE(1);
        this->drawIndex = 2;
        this->shadowAlpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->unk_1A4 = 0.0f;
        DemoDu_CsPlaySfx_WhiteOut();
    }
}

void DemoDu_CsAfterGanon_AdvanceTo02(DemoDu* this, PlayState* play) {
    f32* unk_1A4 = &this->unk_1A4;
    s32 shadowAlpha = 255;

    if (DemoDu_CheckForCue(this, play, 4, 2)) {
        *unk_1A4 += 1.0f;
        if (*unk_1A4 >= kREG(5) + 10.0f) {
            this->updateIndex = CS_CHAMBERAFTERGANON_SUBSCENE(2);
            this->drawIndex = 1;
            *unk_1A4 = kREG(5) + 10.0f;
            this->shadowAlpha = shadowAlpha;
            this->actor.shape.shadowAlpha = shadowAlpha;
            return;
        }
    } else {
        *unk_1A4 -= 1.0f;
        if (*unk_1A4 <= 0.0f) {
            this->updateIndex = CS_CHAMBERAFTERGANON_SUBSCENE(0);
            this->drawIndex = 0;
            *unk_1A4 = 0.0f;
            this->shadowAlpha = 0;
            this->actor.shape.shadowAlpha = 0;
            return;
        }
    }
    shadowAlpha = (*unk_1A4 / (kREG(5) + 10.0f)) * 255.0f;
    this->shadowAlpha = shadowAlpha;
    this->actor.shape.shadowAlpha = shadowAlpha;
}

void DemoDu_CsAfterGanon_BackTo01(DemoDu* this, PlayState* play) {
    if (DemoDu_CheckForNoCue(this, play, 4, 2)) {
        this->updateIndex = CS_CHAMBERAFTERGANON_SUBSCENE(1);
        this->drawIndex = 2;
        this->unk_1A4 = kREG(5) + 10.0f;
        this->shadowAlpha = 255;
        if (!this->demo6KSpawned) {
            DemoDu_CsAfterGanon_SpawnDemo6K(this, play);
            this->demo6KSpawned = 1;
        }
        this->actor.shape.shadowAlpha = 255;
    }
}

void DemoDu_UpdateCs_AG_00(DemoDu* this, PlayState* play) {
    DemoDu_CsAfterGanon_AdvanceTo01(this, play);
#if DEBUG_FEATURES
    DemoDu_CsAfterGanon_CheckIfShouldReset(this, play);
#endif
}

void DemoDu_UpdateCs_AG_01(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_UpdateEyes(this);
    DemoDu_CsAfterGanon_AdvanceTo02(this, play);
#if DEBUG_FEATURES
    DemoDu_CsAfterGanon_CheckIfShouldReset(this, play);
#endif
}

void DemoDu_UpdateCs_AG_02(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_UpdateEyes(this);
    DemoDu_CsAfterGanon_BackTo01(this, play);
#if DEBUG_FEATURES
    DemoDu_CsAfterGanon_CheckIfShouldReset(this, play);
#endif
}

// Similar to DemoDu_Draw_01, but this uses POLY_XLU_DISP. Also uses this->shadowAlpha for setting the env color.
void DemoDu_Draw_02(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    DemoDu* this = (DemoDu*)thisx;
    s16 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = sEyeTextures[eyeTexIndex];
    s32 pad;
    s16 mouthTexIndex = this->mouthTexIndex;
    void* mouthTexture = sMouthTextures[mouthTexIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_du_inKenjyanomaDemo02.c", 275);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouthTexture));
    gSPSegment(POLY_XLU_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gDaruniaNoseSeriousTex));

    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->shadowAlpha);

    gSPSegment(POLY_XLU_DISP++, 0x0C, &D_80116280[0]);

    POLY_XLU_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, 0, 0, 0,
                                       POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_du_inKenjyanomaDemo02.c", 304);
}
