/**
 * Sets up Ruto in her arms-crossing pose. Used in the Water Trial in Ganon's Castle and in the
 * Chamber of Sages during the "Sealing Ganon" cutscene.
 */
void En_Ru2_KenjyanomaDemo02_Init(EnRu2* this, PlayState* play) {
    En_Ru2_Change_Anime(this, &gAdultRutoCrossingArmsAnim, 2, 0.0f, 0);
    this->action = ENRU2_WATER_TRIAL_INVISIBLE;
    this->actor.shape.shadowAlpha = 0;
}

void En_Ru2_Seal_Set_AppearSound(void) {
    Na_StartFixSe_F(NA_SE_SY_WHITE_OUT_T);
}

/**
 * Spawns the ball of light that replaces Ruto's actor in the Water Trial.
 */
void Birth_Demo_6k_In_En_Ru2_Seal(EnRu2* this, PlayState* play) {
    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       kREG(19) + 24.0f + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 8);
}

/**
 * Checks to see if it's time for Ruto to fade in while crossing her arms.
 */
void En_Ru2_Seal_Check_AlphaAndMode_hide(EnRu2* this, PlayState* play) {
    if (En_Ru2_Check_npcdemopnt(this, play, 4, 3)) {
        this->action = ENRU2_WATER_TRIAL_FADE;
        this->drawConfig = ENRU2_DRAW_XLU;
        this->alpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->fadeTimer = 0.0f;
        En_Ru2_Seal_Set_AppearSound();
    }
}

/**
 * Fades Ruto's actor in or out. Both happen during the Water Trial.
 */
void En_Ru2_Seal_Check_AlphaAndMode_fade(EnRu2* this, PlayState* play) {
    f32* fadeTimer = &this->fadeTimer;
    s32 alpha;

    if (En_Ru2_Check_npcdemopnt(this, play, 4, 3)) {
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
void En_Ru2_Seal_Check_AlphaAndMode_pray(EnRu2* this, PlayState* play) {
    if (En_Ru2_Check2_npcdemopnt(this, play, 4, 3)) {
        this->action = ENRU2_WATER_TRIAL_FADE;
        this->drawConfig = ENRU2_DRAW_XLU;
        this->fadeTimer = kREG(5) + 10.0f;
        this->alpha = 255;
        if (!this->isLightBall) {
            Birth_Demo_6k_In_En_Ru2_Seal(this, play);
            this->isLightBall = true;
        }
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void En_Ru2_Seal_Actor_main_hide(EnRu2* this, PlayState* play) {
    En_Ru2_Seal_Check_AlphaAndMode_hide(this, play);
#if DEBUG_FEATURES
    func_80AF26D0(this, play);
#endif
}

void En_Ru2_Seal_Actor_main_fade(EnRu2* this, PlayState* play) {
    En_Ru2_BGcheck(this, play);
    En_Ru2_Animation_Base(this);
    En_Ru2_set_eye_pattern(this);
    En_Ru2_Seal_Check_AlphaAndMode_fade(this, play);
#if DEBUG_FEATURES
    func_80AF26D0(this, play);
#endif
}

void En_Ru2_Seal_Actor_main_pray(EnRu2* this, PlayState* play) {
    En_Ru2_BGcheck(this, play);
    En_Ru2_Animation_Base(this);
    En_Ru2_set_eye_pattern(this);
    En_Ru2_Seal_Check_AlphaAndMode_pray(this, play);
#if DEBUG_FEATURES
    func_80AF26D0(this, play);
#endif
}

void En_Ru2_Actor_draw_alpha(EnRu2* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* tex = en_ru2_eye[eyeIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ru2_inKenjyanomaDemo02.c", 264);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(tex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(tex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, &Actor_change_render_mode[0]);

    POLY_XLU_DISP = Si2_draw2_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL,
                                       NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ru2_inKenjyanomaDemo02.c", 291);
}
