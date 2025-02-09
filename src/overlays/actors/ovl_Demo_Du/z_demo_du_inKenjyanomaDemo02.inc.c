void Demo_Du_KenjyanomaDemo02_Init(DemoDu* this, PlayState* play) {
    s32 pad[3];
    f32 lastFrame = Si2_anime_end_frame(&gDaruniaSageFormationAnim);

    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gDaruniaSkel, NULL, NULL, NULL, 0);
    Skeleton_Info2_init(&this->skelAnime, &gDaruniaSageFormationAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    this->updateIndex = CS_CHAMBERAFTERGANON_SUBSCENE(0);
    this->actor.shape.shadowAlpha = 0;
}

void Demo_Du_Seal_Set_AppearSound(void) {
    Na_StartFixSe_F(NA_SE_SY_WHITE_OUT_T);
}

void Birth_Demo_6k_In_Demo_Du_Seal(DemoDu* this, PlayState* play) {
    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       kREG(16) + 22.0f + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 3);
}

void Demo_Du_Seal_Check_AlphaAndMode_hide(DemoDu* this, PlayState* play) {
    if (Demo_Du_Check_npcdemopnt(this, play, 4, 2)) {
        this->updateIndex = CS_CHAMBERAFTERGANON_SUBSCENE(1);
        this->drawIndex = 2;
        this->shadowAlpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->unk_1A4 = 0.0f;
        Demo_Du_Seal_Set_AppearSound();
    }
}

void Demo_Du_Seal_Check_AlphaAndMode_fade(DemoDu* this, PlayState* play) {
    f32* unk_1A4 = &this->unk_1A4;
    s32 shadowAlpha = 255;

    if (Demo_Du_Check_npcdemopnt(this, play, 4, 2)) {
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

void Demo_Du_Seal_Check_AlphaAndMode_pray(DemoDu* this, PlayState* play) {
    if (Demo_Du_Check2_npcdemopnt(this, play, 4, 2)) {
        this->updateIndex = CS_CHAMBERAFTERGANON_SUBSCENE(1);
        this->drawIndex = 2;
        this->unk_1A4 = kREG(5) + 10.0f;
        this->shadowAlpha = 255;
        if (!this->demo6KSpawned) {
            Birth_Demo_6k_In_Demo_Du_Seal(this, play);
            this->demo6KSpawned = 1;
        }
        this->actor.shape.shadowAlpha = 255;
    }
}

void Demo_Du_Seal_Actor_main_hide(DemoDu* this, PlayState* play) {
    Demo_Du_Seal_Check_AlphaAndMode_hide(this, play);
#if DEBUG_FEATURES
    DemoDu_CsAfterGanon_CheckIfShouldReset(this, play);
#endif
}

void Demo_Du_Seal_Actor_main_fade(DemoDu* this, PlayState* play) {
    Demo_Du_BGcheck(this, play);
    Demo_Du_Animation_Base(this);
    Demo_Du_set_eye_pattern(this);
    Demo_Du_Seal_Check_AlphaAndMode_fade(this, play);
#if DEBUG_FEATURES
    DemoDu_CsAfterGanon_CheckIfShouldReset(this, play);
#endif
}

void Demo_Du_Seal_Actor_main_pray(DemoDu* this, PlayState* play) {
    Demo_Du_BGcheck(this, play);
    Demo_Du_Animation_Base(this);
    Demo_Du_set_eye_pattern(this);
    Demo_Du_Seal_Check_AlphaAndMode_pray(this, play);
#if DEBUG_FEATURES
    DemoDu_CsAfterGanon_CheckIfShouldReset(this, play);
#endif
}

// Similar to Demo_Du_Actor_draw_normal, but this uses POLY_XLU_DISP. Also uses this->shadowAlpha for setting the env color.
void Demo_Du_Actor_draw_alpha(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    DemoDu* this = (DemoDu*)thisx;
    s16 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = demo_du_eye[eyeTexIndex];
    s32 pad;
    s16 mouthTexIndex = this->mouthTexIndex;
    void* mouthTexture = demo_du_mouth[mouthTexIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_du_inKenjyanomaDemo02.c", 275);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouthTexture));
    gSPSegment(POLY_XLU_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gDaruniaNoseSeriousTex));

    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->shadowAlpha);

    gSPSegment(POLY_XLU_DISP++, 0x0C, &Actor_change_render_mode[0]);

    POLY_XLU_DISP = Si2_draw2_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, 0, 0, 0,
                                       POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_du_inKenjyanomaDemo02.c", 304);
}
