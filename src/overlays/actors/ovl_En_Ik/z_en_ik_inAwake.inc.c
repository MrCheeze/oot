void En_Ik_inAwake_SetSound_reel(EnIk* this) {
    Nai_FxFlagEntry(NA_SE_EN_IRONNACK_STAGGER_DEMO, &this->actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
}

void En_Ik_inAwake_SetSound_dead(EnIk* this, PlayState* play) {
    static Vec3f display_position;
    s32 pad[2];
    f32 wDest;

    Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, &this->actor.world.pos, &display_position, &wDest);
    Nai_FxFlagEntry(NA_SE_EN_IRONNACK_DEAD, &display_position, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
}

// Cutscene: starts after final hit to Nabooru
void En_Ik_inConfrontion_setup_Awake(EnIk* this, PlayState* play) {
    s32 pad[3];
    f32 endFrame = Si2_anime_end_frame(&gIronKnuckleNabooruDeathAnim);

    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gIronKnuckleDefeatSkel, NULL, this->jointTable, this->morphTable,
                       IRON_KNUCKLE_LIMB_MAX);
    Skeleton_Info2_init(&this->skelAnime, &gIronKnuckleNabooruDeathAnim, 1.0f, 0.0f, endFrame, ANIMMODE_ONCE, 0.0f);
    this->csAction = IK_CS_ACTION_3;
    this->csDrawMode = IK_CS_DRAW_DEFEAT;
    En_Ik_inConfrontion_Set_DemoStartPosAngle(this, play, 4);
    En_Ik_inAwake_SetSound_dead(this, play);
    this->actor.shape.shadowAlpha = 255;
}

// Cutscene: Armor falling off revealing Nabooru underneath
void En_Ik_inConfrontion_setup_Remove(EnIk* this, PlayState* play) {
    this->csAction = IK_CS_ACTION_4;
    this->csDrawMode = IK_CS_DRAW_DEFEAT;
    En_Ik_inAwake_SetSound_reel(this);
    this->actor.shape.shadowAlpha = 255;
}

// Cutscene: all the armor has fallen off
void En_Ik_inConfrontion_setup_Disappear(EnIk* this, PlayState* play) {
    this->csAction = IK_CS_ACTION_5;
    this->csDrawMode = IK_CS_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

void En_Ik_inConfrontion_main_awake(EnIk* this, PlayState* play) {
    En_Ik_inConfrontion_BGcheck(this, play);
    En_Ik_inConfrontion_Check_DemoMode(this, play);
}

void En_Ik_inConfrontion_main_remove(EnIk* this, PlayState* play) {
    En_Ik_inConfrontion_Animation_Base(this);
    En_Ik_inConfrontion_BGcheck(this, play);
    En_Ik_inConfrontion_Check_DemoMode(this, play);
}

void En_Ik_inConfrontion_main_disappear(EnIk* this, PlayState* play) {
    if (En_Ik_inConfrontion_Get_npcdemopnt(play, 4) == NULL) {
#if OOT_PAL_N64
        Actor_Environment_sw_On(play, this->switchFlag);
#endif
        Actor_delete(&this->actor);
    }
}

s32 En_Ik_inConfrontion_ChangeDraw_head(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    if ((limbIndex == IRON_KNUCKLE_DEFEAT_LIMB_HELMET_ARMOR) ||
        (limbIndex == IRON_KNUCKLE_DEFEAT_LIMB_CHEST_ARMOR_FRONT) ||
        (limbIndex == IRON_KNUCKLE_DEFEAT_LIMB_CHEST_ARMOR_BACK)) {
        if (En_Ik_inConfrontion_Get_anime_frame(thisx) >= 30.0f) {
            *dList = NULL;
        }
    }

    return false;
}

void En_Ik_inConfrontion_AddDraw_head(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_en_ik_inAwake.c", 207);

    switch (limbIndex) {
        case IRON_KNUCKLE_DEFEAT_LIMB_HELMET_ARMOR: {
            EnIk* this = (EnIk*)thisx;

            if (En_Ik_inConfrontion_Get_anime_frame(&this->actor) < 30.0f) {
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

            if (En_Ik_inConfrontion_Get_anime_frame(&this->actor) < 30.0f) {
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_en_ik_inAwake.c", 288);
                gSPDisplayList(POLY_XLU_DISP++, gIronKnuckleArmorRivetAndSymbolDL);
            }
        } break;

        case IRON_KNUCKLE_DEFEAT_LIMB_CHEST_ARMOR_BACK: {
            EnIk* this = (EnIk*)thisx;

            if (En_Ik_inConfrontion_Get_anime_frame(&this->actor) < 30.0f) {
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_en_ik_inAwake.c", 297);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016CD8);
            }
        } break;
    }

    CLOSE_DISPS(gfxCtx, "../z_en_ik_inAwake.c", 304);
}

void En_Ik_inConfrontion_draw_change_head(EnIk* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    SkelAnime* skelAnime = &this->skelAnime;
    s32 pad[2];

    OPEN_DISPS(gfxCtx, "../z_en_ik_inAwake.c", 322);

    Actor_HiliteReflect_set_init(&this->actor, play, 0);
    _texture_z_light_fog_prim(gfxCtx);
    _texture_z_light_fog_prim_xlu(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, set_col(gfxCtx, 245, 225, 155, 30, 30, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, set_col(gfxCtx, 255, 40, 0, 40, 0, 0));
    gSPSegment(POLY_OPA_DISP++, 0x0A, set_col(gfxCtx, 255, 255, 255, 20, 40, 30));
    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          En_Ik_inConfrontion_ChangeDraw_head, En_Ik_inConfrontion_AddDraw_head, this);

    CLOSE_DISPS(gfxCtx, "../z_en_ik_inAwake.c", 345);
}
