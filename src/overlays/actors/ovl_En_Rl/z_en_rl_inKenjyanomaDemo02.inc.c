void En_Rl_KenjyanomaDemo02_Init(EnRl* this, PlayState* play) {
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_rl_Skel_007B38, &object_rl_Anim_000A3C, NULL, NULL, 0);
    this->action = 4;
    this->actor.shape.shadowAlpha = 0;
}

void Birth_Demo_6k_In_En_Rl_Seal(EnRl* this, PlayState* play) {
    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       kREG(18) + 22.0f + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 5);
}

void En_Rl_Seal_Check_AlphaAndMode_hide(EnRl* this, PlayState* play) {
    if (En_Rl_Check_npcdemopnt(this, play, 4, 0)) {
        this->action = 5;
        this->drawConfig = 2;
        this->alpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->unk_19C = 0.0f;
    }
}

void En_Rl_Seal_Check_AlphaAndMode_fade(EnRl* this, PlayState* play) {
    f32* unk_19C = &this->unk_19C;
    s32 alpha = 255;

    if (En_Rl_Check_npcdemopnt(this, play, 4, 0)) {
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

void En_Rl_Seal_Check_AlphaAndMode_branch(EnRl* this, PlayState* play) {
    if (En_Rl_Check_npcdemopnt(this, play, 3, 0)) {
        f32 frame = Si2_anime_end_frame(&object_rl_Anim_00040C);

        Skeleton_Info2_init(&this->skelAnime, &object_rl_Anim_00040C, 1.0f, 0.0f, frame, ANIMMODE_ONCE, -8.0f);
        this->action = 6;
    } else if (En_Rl_Check2_npcdemopnt(this, play, 4, 0)) {
        this->action = 5;
        this->drawConfig = 2;
        this->unk_19C = kREG(5) + 10.0f;
        this->alpha = 255;
        if (!this->lightBallSpawned) {
            Birth_Demo_6k_In_En_Rl_Seal(this, play);
            this->lightBallSpawned = 1;
        }
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void En_Rl_Seal_Check_HandupToPray(EnRl* this, s32 arg1) {
    if (arg1 != 0) {
        Skeleton_Info2_init(&this->skelAnime, &object_rl_Anim_000830, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_rl_Anim_000830), ANIMMODE_LOOP, 0.0f);
        this->action = 7;
    }
}

void En_Rl_Seal_Actor_main_hide(EnRl* this, PlayState* play) {
    En_Rl_Seal_Check_AlphaAndMode_hide(this, play);
#if DEBUG_FEATURES
    func_80AE73D8(this, play);
#endif
}

void En_Rl_Seal_Actor_main_fade(EnRl* this, PlayState* play) {
    En_Rl_BGcheck(this, play);
    En_Rl_Animation_Base(this);
    En_Rl_set_eye_pattern(this);
    En_Rl_Seal_Check_AlphaAndMode_fade(this, play);
#if DEBUG_FEATURES
    func_80AE73D8(this, play);
#endif
}

void En_Rl_Seal_Actor_main_handup(EnRl* this, PlayState* play) {
    s32 temp;

    En_Rl_BGcheck(this, play);
    temp = En_Rl_Animation_Base(this);
    En_Rl_set_eye_pattern(this);
    En_Rl_Seal_Check_HandupToPray(this, temp);
#if DEBUG_FEATURES
    func_80AE73D8(this, play);
#endif
}

void En_Rl_Seal_Actor_main_pray(EnRl* this, PlayState* play) {
    En_Rl_BGcheck(this, play);
    En_Rl_Animation_Base(this);
    En_Rl_set_eye_pattern(this);
    En_Rl_Seal_Check_AlphaAndMode_branch(this, play);
#if DEBUG_FEATURES
    func_80AE73D8(this, play);
#endif
}

void En_Rl_Actor_draw_alpha(EnRl* this, PlayState* play) {
    s32 pad[2];
    s16 temp = this->eyeTextureIndex;
    void* tex = en_rl_eye[temp];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_rl_inKenjyanomaDemo02.c", 304);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(tex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(tex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, Actor_change_render_mode);

    POLY_XLU_DISP = Si2_draw2_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL,
                                       NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_rl_inKenjyanomaDemo02.c", 331);
}
