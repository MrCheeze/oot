void func_80AE7878(EnRl* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &object_rl_Skel_007B38, &object_rl_Anim_000A3C, NULL, NULL, 0);
    this->action = 4;
    this->actor.shape.shadowAlpha = 0;
}

void func_80AE78D4(EnRl* this, PlayState* play) {
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       kREG(18) + 22.0f + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 5);
}

void func_80AE7954(EnRl* this, PlayState* play) {
    if (func_80AE74B4(this, play, 4, 0)) {
        this->action = 5;
        this->drawConfig = 2;
        this->alpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->unk_19C = 0.0f;
    }
}

void func_80AE79A4(EnRl* this, PlayState* play) {
    f32* unk_19C = &this->unk_19C;
    s32 alpha = 255;

    if (func_80AE74B4(this, play, 4, 0)) {
        *unk_19C += 1.0f;
        if (*unk_19C >= kREG(5) + 10.0f) {
            this->action = 7;
            this->drawConfig = 1;
            *unk_19C = kREG(5) + 10.0f;
            this->alpha = alpha;
            this->actor.shape.shadowAlpha = alpha;
            return;
        }
    } else {
        *unk_19C -= 1.0f;
        if (*unk_19C <= 0.0f) {
            this->action = 4;
            this->drawConfig = 0;
            *unk_19C = 0.0f;
            this->alpha = 0;
            this->actor.shape.shadowAlpha = 0;
            return;
        }
    }
    alpha = (*unk_19C / (kREG(5) + 10.0f)) * 255.0f;
    this->alpha = alpha;
    this->actor.shape.shadowAlpha = alpha;
}

void func_80AE7AF8(EnRl* this, PlayState* play) {
    if (func_80AE74B4(this, play, 3, 0)) {
        f32 frame = Animation_GetLastFrame(&object_rl_Anim_00040C);

        Animation_Change(&this->skelAnime, &object_rl_Anim_00040C, 1.0f, 0.0f, frame, ANIMMODE_ONCE, -8.0f);
        this->action = 6;
    } else if (func_80AE74FC(this, play, 4, 0)) {
        this->action = 5;
        this->drawConfig = 2;
        this->unk_19C = kREG(5) + 10.0f;
        this->alpha = 255;
        if (!this->lightBallSpawned) {
            func_80AE78D4(this, play);
            this->lightBallSpawned = 1;
        }
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_80AE7BF8(EnRl* this, s32 arg1) {
    if (arg1 != 0) {
        Animation_Change(&this->skelAnime, &object_rl_Anim_000830, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_rl_Anim_000830), ANIMMODE_LOOP, 0.0f);
        this->action = 7;
    }
}

void func_80AE7C64(EnRl* this, PlayState* play) {
    func_80AE7954(this, play);
#if DEBUG_FEATURES
    func_80AE73D8(this, play);
#endif
}

void func_80AE7C94(EnRl* this, PlayState* play) {
    func_80AE744C(this, play);
    func_80AE7494(this);
    func_80AE72D0(this);
    func_80AE79A4(this, play);
#if DEBUG_FEATURES
    func_80AE73D8(this, play);
#endif
}

void func_80AE7CE8(EnRl* this, PlayState* play) {
    s32 temp;

    func_80AE744C(this, play);
    temp = func_80AE7494(this);
    func_80AE72D0(this);
    func_80AE7BF8(this, temp);
#if DEBUG_FEATURES
    func_80AE73D8(this, play);
#endif
}

void func_80AE7D40(EnRl* this, PlayState* play) {
    func_80AE744C(this, play);
    func_80AE7494(this);
    func_80AE72D0(this);
    func_80AE7AF8(this, play);
#if DEBUG_FEATURES
    func_80AE73D8(this, play);
#endif
}

void func_80AE7D94(EnRl* this, PlayState* play) {
    s32 pad[2];
    s16 temp = this->eyeTextureIndex;
    void* tex = D_80AE81A0[temp];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_rl_inKenjyanomaDemo02.c", 304);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(tex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(tex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, D_80116280);

    POLY_XLU_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL,
                                       NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_rl_inKenjyanomaDemo02.c", 331);
}
