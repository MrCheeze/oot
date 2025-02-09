void EnIk_PlayArmorFallSfx(EnIk* this) {
    Audio_PlaySfxGeneral(NA_SE_EN_IRONNACK_STAGGER_DEMO, &this->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                         &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

void EnIk_PlayDeathSfx(EnIk* this, PlayState* play) {
    static Vec3f D_80A78FA0;
    s32 pad[2];
    f32 wDest;

    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &this->actor.world.pos, &D_80A78FA0, &wDest);
    Audio_PlaySfxGeneral(NA_SE_EN_IRONNACK_DEAD, &D_80A78FA0, 4, &gSfxDefaultFreqAndVolScale,
                         &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

// Cutscene: starts after final hit to Nabooru
void EnIk_SetupCsAction3(EnIk* this, PlayState* play) {
    s32 pad[3];
    f32 endFrame = Animation_GetLastFrame(&gIronKnuckleNabooruDeathAnim);

    SkelAnime_InitFlex(play, &this->skelAnime, &gIronKnuckleDefeatSkel, NULL, this->jointTable, this->morphTable,
                       IRON_KNUCKLE_LIMB_MAX);
    Animation_Change(&this->skelAnime, &gIronKnuckleNabooruDeathAnim, 1.0f, 0.0f, endFrame, ANIMMODE_ONCE, 0.0f);
    this->csAction = IK_CS_ACTION_3;
    this->csDrawMode = IK_CS_DRAW_DEFEAT;
    EnIk_SetStartPosRotFromCue(this, play, 4);
    EnIk_PlayDeathSfx(this, play);
    this->actor.shape.shadowAlpha = 255;
}

// Cutscene: Armor falling off revealing Nabooru underneath
void EnIk_SetupCsAction4(EnIk* this, PlayState* play) {
    this->csAction = IK_CS_ACTION_4;
    this->csDrawMode = IK_CS_DRAW_DEFEAT;
    EnIk_PlayArmorFallSfx(this);
    this->actor.shape.shadowAlpha = 255;
}

// Cutscene: all the armor has fallen off
void EnIk_SetupCsAction5(EnIk* this, PlayState* play) {
    this->csAction = IK_CS_ACTION_5;
    this->csDrawMode = IK_CS_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

void EnIk_CsAction3(EnIk* this, PlayState* play) {
    EnIk_UpdateBgCheckInfo(this, play);
    EnIk_HandleCsCues(this, play);
}

void EnIk_CsAction4(EnIk* this, PlayState* play) {
    EnIk_UpdateSkelAnime(this);
    EnIk_UpdateBgCheckInfo(this, play);
    EnIk_HandleCsCues(this, play);
}

void EnIk_CsAction5(EnIk* this, PlayState* play) {
    if (EnIk_GetCue(play, 4) == NULL) {
#if OOT_PAL_N64
        Flags_SetSwitch(play, this->switchFlag);
#endif
        Actor_Kill(&this->actor);
    }
}

s32 EnIk_OverrideLimbDrawDefeat(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    if ((limbIndex == IRON_KNUCKLE_DEFEAT_LIMB_HELMET_ARMOR) ||
        (limbIndex == IRON_KNUCKLE_DEFEAT_LIMB_CHEST_ARMOR_FRONT) ||
        (limbIndex == IRON_KNUCKLE_DEFEAT_LIMB_CHEST_ARMOR_BACK)) {
        if (EnIk_GetAnimCurFrame(thisx) >= 30.0f) {
            *dList = NULL;
        }
    }

    return false;
}

void EnIk_PostLimbDrawDefeat(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_en_ik_inAwake.c", 207);

    switch (limbIndex) {
        case IRON_KNUCKLE_DEFEAT_LIMB_HELMET_ARMOR: {
            EnIk* this = (EnIk*)thisx;

            if (EnIk_GetAnimCurFrame(&this->actor) < 30.0f) {
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_en_ik_inAwake.c", 267);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016D88);
            }
        } break;

        case IRON_KNUCKLE_DEFEAT_LIMB_UPPER_LEFT_PAULDRON:
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_en_ik_inAwake.c", 274);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016F88);
            break;

        case IRON_KNUCKLE_DEFEAT_LIMB_UPPER_RIGHT_PAULDRON:
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_en_ik_inAwake.c", 280);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016EE8);
            break;

        case IRON_KNUCKLE_DEFEAT_LIMB_CHEST_ARMOR_FRONT: {
            EnIk* this = (EnIk*)thisx;

            if (EnIk_GetAnimCurFrame(&this->actor) < 30.0f) {
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_en_ik_inAwake.c", 288);
                gSPDisplayList(POLY_XLU_DISP++, gIronKnuckleArmorRivetAndSymbolDL);
            }
        } break;

        case IRON_KNUCKLE_DEFEAT_LIMB_CHEST_ARMOR_BACK: {
            EnIk* this = (EnIk*)thisx;

            if (EnIk_GetAnimCurFrame(&this->actor) < 30.0f) {
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_en_ik_inAwake.c", 297);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016CD8);
            }
        } break;
    }

    CLOSE_DISPS(gfxCtx, "../z_en_ik_inAwake.c", 304);
}

void EnIk_CsDrawDefeat(EnIk* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    SkelAnime* skelAnime = &this->skelAnime;
    s32 pad[2];

    OPEN_DISPS(gfxCtx, "../z_en_ik_inAwake.c", 322);

    func_8002EBCC(&this->actor, play, 0);
    Gfx_SetupDL_25Opa(gfxCtx);
    Gfx_SetupDL_25Xlu(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, EnIk_SetPrimEnvColors(gfxCtx, 245, 225, 155, 30, 30, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, EnIk_SetPrimEnvColors(gfxCtx, 255, 40, 0, 40, 0, 0));
    gSPSegment(POLY_OPA_DISP++, 0x0A, EnIk_SetPrimEnvColors(gfxCtx, 255, 255, 255, 20, 40, 30));
    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          EnIk_OverrideLimbDrawDefeat, EnIk_PostLimbDrawDefeat, this);

    CLOSE_DISPS(gfxCtx, "../z_en_ik_inAwake.c", 345);
}
