void Demo_Im_KenjyanomaDemo02_Init(DemoIm* this, PlayState* play) {
    Demo_Im_Change_Anime(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->action = 7;
    this->actor.shape.shadowAlpha = 0;
}

void Demo_Im_Seal_Set_AppearSound(void) {
    Na_StartFixSe_F(NA_SE_SY_WHITE_OUT_T);
}

void Birth_Demo_6k_In_Demo_Im_Seal(DemoIm* this, PlayState* play) {
    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       (kREG(17) + 24.0f) + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 6);
}

void Demo_Im_Seal_Check_AlphaAndMode_hide(DemoIm* this, PlayState* play) {
    if (Demo_Im_Check_npcdemopnt(this, play, 4, 5)) {
        Skeleton_Info2_init(&this->skelAnime, &gImpaPrepareSealGanonAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gImpaPrepareSealGanonAnim), ANIMMODE_ONCE, 0.0f);
        this->action = 8;
        this->drawConfig = 2;
        this->alpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->unk_268 = 0.0f;
        Demo_Im_Seal_Set_AppearSound();
    }
}

void Demo_Im_Seal_Check_AlphaAndMode_fade(DemoIm* this, PlayState* play) {
    f32* unk_268 = &this->unk_268;
    s32 alpha = 255;

    if (Demo_Im_Check_npcdemopnt(this, play, 4, 5)) {
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

void Demo_Im_Seal_Check_AlphaAndMode_pray(DemoIm* this, PlayState* play) {
    if (Demo_Im_Check2_npcdemopnt(this, play, 4, 5)) {
        Skeleton_Info2_init(&this->skelAnime, &gImpaSealGanonAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gImpaSealGanonAnim),
                         ANIMMODE_ONCE, -8.0f);
        this->action = 8;
        this->drawConfig = 2;
        this->unk_268 = kREG(5) + 10.0f;
        this->alpha = 255;
        if (this->unk_270 == 0) {
            Birth_Demo_6k_In_Demo_Im_Seal(this, play);
            this->unk_270 = 1;
        }
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void Demo_Im_Seal_Actor_main_hide(DemoIm* this, PlayState* play) {
    Demo_Im_Seal_Check_AlphaAndMode_hide(this, play);
#if DEBUG_FEATURES
    func_80984C8C(this, play);
#endif
}

void Demo_Im_Seal_Actor_main_fade(DemoIm* this, PlayState* play) {
    Demo_Im_BGcheck(this, play);
    Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_Seal_Check_AlphaAndMode_fade(this, play);
#if DEBUG_FEATURES
    func_80984C8C(this, play);
#endif
}

void Demo_Im_Seal_Actor_main_pray(DemoIm* this, PlayState* play) {
    Demo_Im_BGcheck(this, play);
    Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_Seal_Check_AlphaAndMode_pray(this, play);
#if DEBUG_FEATURES
    func_80984C8C(this, play);
#endif
}

void Demo_Im_Actor_draw_alpha(DemoIm* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* eyeTex = demo_im_eye[eyeIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_im_inKenjyanomaDemo02.c", 281);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, &Actor_change_render_mode[0]);

    POLY_XLU_DISP = Si2_draw2_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL,
                                       NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_im_inKenjyanomaDemo02.c", 308);
}
