void Demo_Sa_KenjyanomaDemo02_Init(DemoSa* this, PlayState* play) {
    s32 pad[2];
    SkelAnime* skelAnime = &this->skelAnime;
    f32 frameCount = Si2_anime_end_frame(&gSariaSealGanonAnim);

    Skeleton_Info2_SV_M_ct(play, skelAnime, &gSariaSkel, NULL, NULL, NULL, 0);
    Skeleton_Info2_init(skelAnime, &gSariaSealGanonAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
    this->action = 7;
    this->actor.shape.shadowAlpha = 0;
    Demo_Sa_set_eye_Num(this, SARIA_EYE_CLOSED);
    Demo_Sa_set_mouth_Num(this, SARIA_MOUTH_CLOSED);
}

void Demo_Sa_Seal_Set_SetFace(DemoSa* this) {
    f32 curFrame = this->skelAnime.curFrame;

    if ((this->skelAnime.mode == 2) && (curFrame >= 32.0f)) {
        Demo_Sa_set_eye_Num(this, SARIA_EYE_HALF);
        Demo_Sa_set_mouth_Num(this, SARIA_MOUTH_CLOSED2);
    }
}

void Demo_Sa_Seal_Set_AppearSound(void) {
    Na_StartFixSe_F(NA_SE_SY_WHITE_OUT_T);
}

void Birth_Demo_6k_In_Demo_Sa_Seal(DemoSa* this, PlayState* play) {
    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       (kREG(23) + 25.0f) + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 4);
}

void Demo_Sa_Seal_Check_AlphaAndMode_hide(DemoSa* this, PlayState* play) {
    if (Demo_Sa_Check_npcdemopnt(this, play, 4, 4)) {
        this->action = 8;
        this->drawConfig = 2;
        this->alpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->unk_1A0 = 0.0f;
        Demo_Sa_Seal_Set_AppearSound();
    }
}

void Demo_Sa_Seal_Check_AlphaAndMode_fade(DemoSa* this, PlayState* play) {
    s32 alpha = 255;
    f32* unk_1A0 = &this->unk_1A0;

    if (Demo_Sa_Check_npcdemopnt(this, play, 4, 4)) {
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

void Demo_Sa_Seal_Check_AlphaAndMode_pray(DemoSa* this, PlayState* play) {
    if (Demo_Sa_Check2_npcdemopnt(this, play, 4, 4)) {
        this->action = 8;
        this->drawConfig = 2;
        this->unk_1A0 = kREG(5) + 10.0f;
        this->alpha = 255;
        if (this->unk_1A8 == 0) {
            Birth_Demo_6k_In_Demo_Sa_Seal(this, play);
            this->unk_1A8 = 1;
        }
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void Demo_Sa_Seal_Actor_main_hide(DemoSa* this, PlayState* play) {
    Demo_Sa_Seal_Check_AlphaAndMode_hide(this, play);
#if DEBUG_FEATURES
    func_8098E554(this, play);
#endif
}

void Demo_Sa_Seal_Actor_main_fade(DemoSa* this, PlayState* play) {
    Demo_Sa_BGcheck(this, play);
    Demo_Sa_Animation_Base(this);
    Demo_Sa_set_eye_pattern(this);
    Demo_Sa_Seal_Check_AlphaAndMode_fade(this, play);
#if DEBUG_FEATURES
    func_8098E554(this, play);
#endif
}

void Demo_Sa_Seal_Actor_main_pray(DemoSa* this, PlayState* play) {
    Demo_Sa_BGcheck(this, play);
    Demo_Sa_Animation_Base(this);
    Demo_Sa_Seal_Set_SetFace(this);
    Demo_Sa_Seal_Check_AlphaAndMode_pray(this, play);
#if DEBUG_FEATURES
    func_8098E554(this, play);
#endif
}

void Demo_Sa_Actor_draw_alpha(DemoSa* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* sp78 = demo_sa_eye[eyeIndex];
    s16 mouthIndex = this->mouthIndex;
    s32 pad2;
    void* sp6C = demo_sa_mouth[mouthIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_sa_inKenjyanomaDemo02.c", 296);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sp78));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sp78));
    gSPSegment(POLY_XLU_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(sp6C));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, Actor_change_render_mode);

    POLY_XLU_DISP = Si2_draw2_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL,
                                       NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_sa_inKenjyanomaDemo02.c", 325);
}
