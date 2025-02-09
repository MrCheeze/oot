void En_Oa2_Actor_Metamol_Init(EnXc* this, PlayState* play) {
    this->action = SHEIK_ACTION_53;
    this->triforceAngle = kREG(24) + 0x53FC;
}

void En_Oa2_Metamol_Set_TransformSound(void) {
    Na_StartDemoSe(SEQ_CS_EFFECTS_SHEIK_TRANSFORM);
}

void En_Oa2_Metamol_Set_TryforceSound(Actor* thisx, PlayState* play) {
    EnXc* this = (EnXc*)thisx;

    if (this->unk_2A8) {
        s32 pad;
        Vec3f src;
        Vec3f pos;
        Vec3f sp1C = { 0.0f, 0.0f, 0.0f };
        f32 wDest;

        Matrix_Position(&sp1C, &src);
        Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, &src, &pos, &wDest);
        Effect_SE_Info_new(play, &pos, 80, NA_SE_EV_TRIFORCE_MARK);
        this->unk_2A8 = 0;
    }
}

void En_Oa2_Metamol_Set_ShoutSound(EnXc* this) {
    if (Skeleton_Info_frame_check(&this->skelAnime, 38.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_SK_SHOUT);
        En_Oa2_Metamol_Set_TransformSound();
    }
}

void En_Oa2_calc_tryforce(Actor* thisx, PlayState* play) {
    EnXc* this = (EnXc*)thisx;

    if (En_Oa2_Check_npcdemopnt(this, play, 21, 4)) {
        this->unk_274 = 1;
        if (this->unk_2AC == 0) {
            this->unk_2AC = 1;
            this->unk_2A8 = 1;
        }
    } else if (En_Oa2_Check_npcdemopnt(this, play, 19, 4)) {
        this->unk_274 = 2;
    }
    if (this->unk_274 != 0) {
        f32* timer = &this->timer;
        s32* prim = this->triforcePrimColor;
        s32* env = this->triforceEnvColor;
        f32* scale = this->triforceScale;

        if (this->unk_274 == 1) {
            if (*timer < kREG(25) + 40.0f) {
                f32 div = *timer / (kREG(25) + 40.0f);

                prim[2] = -85.0f * div + 255;
                prim[3] = 255.0f * div;
                env[1] = 100.0f * div + 100;
                *timer += 1.0f;
            } else {
                prim[2] = 170;
                prim[3] = 255;
                env[1] = 200;
            }
            scale[0] = kREG(19) * 0.1f + 40.0f;
            scale[1] = kREG(20) * 0.1f + 40.0f;
            scale[2] = kREG(21) * 0.1f + 40.0f;
        } else if (this->unk_274 == 2) {
            f32 maxTime = (kREG(25) + 40.0f) + (kREG(27) + 90.0f);

            if (*timer < maxTime) {
                f32 div = (*timer - (kREG(25) + 40.0f)) / (kREG(27) + 90.0f);
                scale[0] = (kREG(19) * 0.1f + 40.0f) + div * ((kREG(26) + 50.0f) * (kREG(19) * 0.1f + 40.0f));
                scale[1] = (kREG(20) * 0.1f + 40.0f) + div * ((kREG(26) + 50.0f) * (kREG(20) * 0.1f + 40.0f));
                scale[2] = (kREG(21) * 0.1f + 40.0f) + div * ((kREG(26) + 50.0f) * (kREG(21) * 0.1f + 40.0f));
                *timer += 1.0f;
            } else {
                scale[0] = (kREG(19) * 0.1f + 40.0f) * (kREG(26) + 50.0f);
                scale[1] = (kREG(20) * 0.1f + 40.0f) * (kREG(26) + 50.0f);
                scale[2] = (kREG(21) * 0.1f + 40.0f) * (kREG(26) + 50.0f);
            }
            this->triforceAngle += (s16)(kREG(28) + 0x2EE0);
        }
    }
}

void En_Oa2_Metamol_check_WaitToGreet(EnXc* this, PlayState* play) {
    if (En_Oa2_Check2_npcdemopnt(this, play, 1, 4)) {
        CutsceneContext* csCtx = &play->csCtx;

        if (csCtx->state != 0) {
            CsCmdActorCue* cue = play->csCtx.actorCues[4];

            if (cue != NULL) {
                PosRot* posRot = &this->actor.world;
                ActorShape* shape = &this->actor.shape;
                Vec3i* startPos = &cue->startPos;

                posRot->pos.x = startPos->x;
                posRot->pos.y = startPos->y;
                posRot->pos.z = startPos->z;

                posRot->rot.y = shape->rot.y = cue->rot.y;
            }
        }

        this->action = SHEIK_ACTION_54;
        this->drawMode = SHEIK_DRAW_DEFAULT;
    }
}

void En_Oa2_Metamol_check_GreetToChenge(EnXc* this, PlayState* play) {
    if (En_Oa2_Check_npcdemopnt(this, play, 10, 4)) {
        Skeleton_Info2_init(&this->skelAnime, &gSheikShowingTriforceOnHandAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gSheikShowingTriforceOnHandAnim), ANIMMODE_ONCE, -8.0f);
        this->action = SHEIK_ACTION_SHOW_TRIFORCE;
        this->drawMode = SHEIK_DRAW_TRIFORCE;
    }
}

void En_Oa2_Metamol_check_ChengeToPosing(EnXc* this, s32 animFinished) {
    if (animFinished) {
        Skeleton_Info2_init(&this->skelAnime, &gSheikShowingTriforceOnHandIdleAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gSheikShowingTriforceOnHandIdleAnim), ANIMMODE_LOOP, 0.0f);
        this->action = SHEIK_ACTION_SHOW_TRIFORCE_IDLE;
    }
}
void En_Oa2_Metamol_check_PosingToDisappear(EnXc* this, PlayState* play) {
    if (En_Oa2_Check_npcdemopnt(this, play, 9, 4)) {
        Actor_delete(&this->actor);
    }
}

void En_Oa2_Metamol_main_wait(EnXc* this, PlayState* play) {
    En_Oa2_Metamol_check_WaitToGreet(this, play);
}

void En_Oa2_Metamol_main_greet(EnXc* this, PlayState* play) {
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Metamol_check_GreetToChenge(this, play);
#if DEBUG_FEATURES
    func_80B3C888(this, play);
#endif
}

void En_Oa2_Metamol_main_chenge(EnXc* this, PlayState* play) {
    s32 animFinished = En_Oa2_Animation_Basic(this);

    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_calc_tryforce(&this->actor, play);
    En_Oa2_Metamol_Set_ShoutSound(this);
    En_Oa2_Metamol_check_ChengeToPosing(this, animFinished);
#if DEBUG_FEATURES
    func_80B3C888(this, play);
#endif
}

void En_Oa2_Metamol_main_posing(EnXc* this, PlayState* play) {
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_calc_tryforce(&this->actor, play);
    En_Oa2_Metamol_check_PosingToDisappear(this, play);
}

s32 En_Oa2_SetDraw_par(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    if (limbIndex == 15) {
        *dList = gSheikDL_011620;
    }
    return 0;
}

void En_Oa2_Metamol_AfterDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    s32 pad[2];
    EnXc* this = (EnXc*)thisx;

    if (limbIndex == 15) {
        Vec3f vec = { 0.0f, 0.0f, 0.0f };
        En_Oa2_Metamol_Set_TryforceSound(&this->actor, play);
        Matrix_Position(&vec, &this->handPos);
        this->unk_2BC = 1;
    }
}

void En_Oa2_Actor_draw_tryforce(Actor* thisx, PlayState* play) {
    EnXc* this = (EnXc*)thisx;
    s32 pad;
    s16 eyeIdx = this->eyeIdx;
    void* eyeTexture = en_oa2_eye[eyeIdx];
    SkelAnime* skelAnime = &this->skelAnime;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad2;

    OPEN_DISPS(gfxCtx, "../z_en_oA2_inMetamol.c", 565);
    if (this->unk_2BC != 0) {
        Mtx* mtx = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
        s32* primColor = this->triforcePrimColor;
        s32* envColor = this->triforceEnvColor;
        f32* scale = this->triforceScale;

        Matrix_push();
        Matrix_translate(kREG(16) + 100.0f, kREG(17) + 4460.0f, kREG(18) + 1190.0f, MTXMODE_APPLY);
        Matrix_rotateXYZ(kREG(22), kREG(23), this->triforceAngle, MTXMODE_APPLY);
        Matrix_scale(scale[0], scale[1], scale[2], MTXMODE_APPLY);
        MATRIX_TO_MTX(mtx, "../z_en_oA2_inMetamol.c", 602);
        Matrix_pull();
        _texture_z_light_fog_prim_xlu(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, primColor[2], primColor[3]);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, envColor[1], 0, 128);
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gSheikDL_012970);
    }

    Actor_HiliteReflect_set_init(thisx, play, 0);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          En_Oa2_SetDraw_par, En_Oa2_Metamol_AfterDraw, this);
    CLOSE_DISPS(gfxCtx, "../z_en_oA2_inMetamol.c", 668);
}
