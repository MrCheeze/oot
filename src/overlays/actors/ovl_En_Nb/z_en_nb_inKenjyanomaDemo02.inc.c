void En_Nb_KenjyanomaDemo02_Init(EnNb* this, PlayState* play) {
    En_Nb_Change_Anime(this, &gNabooruPuttingHandsTogetherCastingMagicAnim, 2, 0.0f, 0);
    this->action = NB_ACTION_7;
    this->actor.shape.shadowAlpha = 0;
}

void En_Nb_Seal_Set_AppearSound(void) {
    Na_StartFixSe_F(NA_SE_SY_WHITE_OUT_T);
}

void Birth_Demo_6k_In_En_Nb_Seal(EnNb* this, PlayState* play) {
    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       kREG(21) + 22.0f + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 7);
}

void En_Nb_Seal_Check_AlphaAndMode_hide(EnNb* this, PlayState* play) {
    if (En_Nb_Check_npcdemopnt(this, play, 4, 1)) {
        this->action = NB_SEAL_HIDE;
        this->drawMode = NB_DRAW_HIDE;
        this->alpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->alphaTimer = 0.0f;
        En_Nb_Seal_Set_AppearSound();
    }
}

void En_Nb_Seal_Check_AlphaAndMode_fade(EnNb* this, PlayState* play) {
    f32* alphaTimer = &this->alphaTimer;
    s32 alpha;

    if (En_Nb_Check_npcdemopnt(this, play, 4, 1)) {
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

void En_Nb_Seal_Check_AlphaAndMode_pray(EnNb* this, PlayState* play) {
    if (En_Nb_Check2_npcdemopnt(this, play, 4, 1)) {
        this->action = NB_SEAL_HIDE;
        this->drawMode = NB_DRAW_HIDE;
        this->alphaTimer = kREG(5) + 10.0f;
        this->alpha = 255;

        if (this->flag == 0) {
            Birth_Demo_6k_In_En_Nb_Seal(this, play);
            this->flag = 1;
        }

        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void En_Nb_Seal_Actor_main_hide(EnNb* this, PlayState* play) {
    En_Nb_Seal_Check_AlphaAndMode_hide(this, play);
#if DEBUG_FEATURES
    func_80AB1210(this, play);
#endif
}

void En_Nb_Seal_Actor_main_fade(EnNb* this, PlayState* play) {
    En_Nb_BGcheck(this, play);
    En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
    En_Nb_Seal_Check_AlphaAndMode_fade(this, play);
#if DEBUG_FEATURES
    func_80AB1210(this, play);
#endif
}

void En_Nb_Seal_Actor_main_pray(EnNb* this, PlayState* play) {
    En_Nb_BGcheck(this, play);
    En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
    En_Nb_Seal_Check_AlphaAndMode_pray(this, play);
#if DEBUG_FEATURES
    func_80AB1210(this, play);
#endif
}

void En_Nb_Actor_draw_alpha(EnNb* this, PlayState* play) {
    s32 pad[2];
    s16 eyeSegIdx = this->eyeIdx;
    void* eyeTex = en_nb_eye[eyeSegIdx];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_nb_inKenjyanomaDemo02.c", 263);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, &Actor_change_render_mode[0]);
    POLY_XLU_DISP = Si2_draw2_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL,
                                       NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_nb_inKenjyanomaDemo02.c", 290);
}
