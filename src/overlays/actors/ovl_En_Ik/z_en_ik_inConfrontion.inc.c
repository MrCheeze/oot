void EnIk_StartMinibossBgm(void) {
    func_800F5ACC(NA_BGM_MINI_BOSS);
}

// Cutscene: Nabooru Knuckle Wakes up
void EnIk_UpdateAction2Sfx(EnIk* this) {
    if (Animation_OnFrame(&this->skelAnime, 1.0f)) {
        Audio_PlaySfxGeneral(NA_SE_EN_IRONNACK_WAKEUP, &this->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    } else if (Animation_OnFrame(&this->skelAnime, 33.0f)) {
        Audio_PlaySfxGeneral(NA_SE_EN_IRONNACK_WALK, &this->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    } else if (Animation_OnFrame(&this->skelAnime, 68.0f) || Animation_OnFrame(&this->skelAnime, 80.0f)) {
        Audio_PlaySfxGeneral(NA_SE_EN_IRONNACK_ARMOR_DEMO, &this->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    } else if (Animation_OnFrame(&this->skelAnime, 107.0f)) {
        Audio_PlaySfxGeneral(NA_SE_EN_IRONNACK_FINGER_DEMO, &this->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    } else if (Animation_OnFrame(&this->skelAnime, 156.0f)) {
        Audio_PlaySfxGeneral(NA_SE_EN_IRONNACK_ARMOR_DEMO, &this->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    } else if (Animation_OnFrame(&this->skelAnime, 188.0f)) {
        Audio_PlaySfxGeneral(NA_SE_EN_IRONNACK_WAVE_DEMO, &this->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }
}

// Cutscene: Summons Axe for Nabooru Knuckle
void EnIk_PlayAxeSpawnSfx(EnIk* this, PlayState* play, Vec3f* pos) {
    Audio_PlaySfxGeneral(NA_SE_EN_TWINROBA_TRANSFORM, &this->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                         &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

void EnIk_SpawnAxeSmoke(EnIk* this, PlayState* play, Vec3f* pos) {
    static Vec3f sAxeSmokeEffectData[] = {
        { 1000.0, -1000.0, 1000.0 },  { 0.0, -1000.0, 0.0 },        { -1000.0, -5000.0, -4000.0 },
        { 1000.0, -5000.0, -3000.0 }, { -1000.0, 1000.0, -6000.0 }, { -1000.0, 3000.0, -5000.0 },
        { -800.0, 1000.0, -3000.0 },  { 0.0, -4000.0, -2000.0 },    { -1000.0, -2000.0, -6000.0 },
        { 1000.0, -3000.0, 0.0 },     { 2000.0, -2000.0, -4000.0 }, { -1000.0, 0.0, -6000.0 },
        { 1000.0, -2000.0, -2000.0 }, { 0.0, -2000.0, 2100.0 },     { 0.0, 0.0, 0.0 },
        { 1000.0, -1000.0, -6000.0 }, { 2000.0, 0.0, -3000.0 },     { -1000.0, -1000.0, -4000.0 },
        { 900.0, -800.0, 2700.0 },    { 720.0f, 900.0f, 2500.0f },
    };

    if (!this->isAxeSummoned) {
        s32 pad;
        Vec3f effectVelocity = { 0.0f, 0.0f, 0.0f };
        Vec3f effectAccel = { 0.0f, 0.3f, 0.0f };
        s32 i;

        for (i = ARRAY_COUNT(sAxeSmokeEffectData) - 1; i >= 0; i--) {
            Color_RGBA8 primColor = { 200, 200, 200, 255 };
            Color_RGBA8 envColor = { 150, 150, 150, 0 };
            s32 randColorOffset;
            Vec3f effectPos;

            Matrix_MultVec3f(&sAxeSmokeEffectData[i], &effectPos);
            randColorOffset = (Rand_ZeroOne() * 20.0f) - 10.0f;

            primColor.r += randColorOffset;
            primColor.g += randColorOffset;
            primColor.b += randColorOffset;

            envColor.r += randColorOffset;
            envColor.g += randColorOffset;
            envColor.b += randColorOffset;

            func_8002829C(play, &effectPos, &effectVelocity, &effectAccel, &primColor, &envColor,
                          (Rand_ZeroOne() * 60.0f) + 300.0f, 0);
        }

        this->isAxeSummoned = true;
        EnIk_PlayAxeSpawnSfx(this, play, pos);
    }
}

void EnIk_UpdateBgCheckInfo(EnIk* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

s32 EnIk_UpdateSkelAnime(EnIk* this) {
    return SkelAnime_Update(&this->skelAnime);
}

CsCmdActorCue* EnIk_GetCue(PlayState* play, s32 cueChannel) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];

        return cue;
    }

    return NULL;
}

void EnIk_SetStartPosRotFromCue(EnIk* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = EnIk_GetCue(play, cueChannel);

    if (cue != NULL) {
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;

        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;
    }
}

f32 EnIk_GetAnimCurFrame(Actor* thisx) {
    EnIk* this = (EnIk*)thisx;

    return this->skelAnime.curFrame;
}

// unused
void EnIk_SetupCsAction0(EnIk* this) {
    this->csAction = IK_CS_ACTION_0;
    this->csDrawMode = IK_CS_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

// Cutscene: Nabooru sitting and Kotake and Koume looking at her
void EnIk_SetupCsAction1(EnIk* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gIronKnuckleNabooruSummonAxeAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gIronKnuckleNabooruSummonAxeAnim), ANIMMODE_ONCE, 0.0f);
    EnIk_SetStartPosRotFromCue(this, play, 4);
    this->csAction = IK_CS_ACTION_1;
    this->csDrawMode = IK_CS_DRAW_INTRO;
    this->actor.shape.shadowAlpha = 255;
}

// Cutscene: Nabooru Knuckle starts to stand up
void EnIk_SetupCsAction2(EnIk* this) {
    Animation_Change(&this->skelAnime, &gIronKnuckleNabooruSummonAxeAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gIronKnuckleNabooruSummonAxeAnim), ANIMMODE_ONCE, 0.0f);
    this->csAction = IK_CS_ACTION_2;
    this->csDrawMode = IK_CS_DRAW_INTRO;
    this->isAxeSummoned = false;
    this->actor.shape.shadowAlpha = 255;
}

void EnIk_HandleEnemyChange(EnIk* this, PlayState* play, s32 animFinished) {
    if (animFinished && (EnIk_GetCue(play, 4) != NULL)) {
        EnIk_ChangeToEnemy(this, play);
    }
}

#include "z_en_ik_inAwake.inc.c"

void EnIk_HandleCsCues(EnIk* this, PlayState* play) {
    CsCmdActorCue* cue = EnIk_GetCue(play, 4);
    u32 nextCueId;
    u32 currentCueId;

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    EnIk_SetupCsAction0(this);
                    break;

                case 2:
                    EnIk_SetupCsAction1(this, play);
                    break;

                case 3:
                    EnIk_SetupCsAction2(this);
                    break;

                case 4:
                    EnIk_ChangeToEnemy(this, play);
                    break;

                case 5:
                    EnIk_SetupCsAction3(this, play);
                    break;

                case 6:
                    EnIk_SetupCsAction4(this, play);
                    break;

                case 7:
                    EnIk_SetupCsAction5(this, play);
                    break;

                default:
                    PRINTF("En_Ik_inConfrontion_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }

            this->cueId = nextCueId;
        }
    }
}

void EnIk_CsAction0(EnIk* this, PlayState* play) {
    EnIk_HandleCsCues(this, play);
}

void EnIk_CsAction1(EnIk* this, PlayState* play) {
    EnIk_UpdateBgCheckInfo(this, play);
    EnIk_HandleCsCues(this, play);
}

void EnIk_CsAction2(EnIk* this, PlayState* play) {
    s32 animFinished;

    animFinished = EnIk_UpdateSkelAnime(this);
    EnIk_UpdateAction2Sfx(this);
    EnIk_UpdateBgCheckInfo(this, play);
    EnIk_HandleCsCues(this, play);
    EnIk_HandleEnemyChange(this, play, animFinished);
}

static EnIkActionFunc sCsActionFuncs[] = {
    EnIk_CsAction0, EnIk_CsAction1, EnIk_CsAction2, EnIk_CsAction3, EnIk_CsAction4, EnIk_CsAction5,
};

void EnIk_UpdateCutscene(Actor* thisx, PlayState* play) {
    EnIk* this = (EnIk*)thisx;

    if (this->csAction < 0 || this->csAction >= ARRAY_COUNT(sCsActionFuncs) || sCsActionFuncs[this->csAction] == NULL) {
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    sCsActionFuncs[this->csAction](this, play);
}

s32 EnIk_OverrideLimbDrawIntro(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnIk* this = (EnIk*)thisx;
    f32 curFrame;

    switch (limbIndex) {
        case IRON_KNUCKLE_LIMB_AXE:
            curFrame = this->skelAnime.curFrame;
            if (curFrame < 120.0f) {
                *dList = NULL;
            } else {
                EnIk_SpawnAxeSmoke(this, play, pos);
            }
            break;

        case IRON_KNUCKLE_LIMB_WAIST:
        case IRON_KNUCKLE_LIMB_MAX:
            *dList = NULL;
            break;
    }

    return false;
}

void EnIk_PostLimbDrawIntro(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_en_ik_inConfrontion.c", 571);

    switch (limbIndex) {
        case IRON_KNUCKLE_LIMB_HELMET_ARMOR:
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_en_ik_inConfrontion.c", 575);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016D88);
            break;

        case IRON_KNUCKLE_LIMB_UPPER_LEFT_PAULDRON:
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_en_ik_inConfrontion.c", 581);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016F88);
            break;

        case IRON_KNUCKLE_LIMB_UPPER_RIGHT_PAULDRON:
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_en_ik_inConfrontion.c", 587);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016EE8);
            break;

        case IRON_KNUCKLE_LIMB_CHEST_ARMOR_FRONT:
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_en_ik_inConfrontion.c", 593);
            gSPDisplayList(POLY_XLU_DISP++, gIronKnuckleArmorRivetAndSymbolDL);
            break;

        case IRON_KNUCKLE_LIMB_CHEST_ARMOR_BACK:
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_en_ik_inConfrontion.c", 599);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016CD8);
            break;
    }

    CLOSE_DISPS(gfxCtx, "../z_en_ik_inConfrontion.c", 604);
}

void EnIk_CsDrawNothing(EnIk* this, PlayState* play) {
}

void EnIk_CsDrawIntro(EnIk* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    SkelAnime* skelAnime = &this->skelAnime;
    s32 pad[2];

    OPEN_DISPS(gfxCtx, "../z_en_ik_inConfrontion.c", 630);

    func_8002EBCC(&this->actor, play, 0);
    Gfx_SetupDL_25Opa(gfxCtx);
    Gfx_SetupDL_25Xlu(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, EnIk_SetPrimEnvColors(gfxCtx, 245, 225, 155, 30, 30, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, EnIk_SetPrimEnvColors(gfxCtx, 255, 40, 0, 40, 0, 0));
    gSPSegment(POLY_OPA_DISP++, 0x0A, EnIk_SetPrimEnvColors(gfxCtx, 255, 255, 255, 20, 40, 30));
    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          EnIk_OverrideLimbDrawIntro, EnIk_PostLimbDrawIntro, this);

    CLOSE_DISPS(gfxCtx, "../z_en_ik_inConfrontion.c", 653);
}

static EnIkDrawFunc sCsDrawFuncs[] = { EnIk_CsDrawNothing, EnIk_CsDrawIntro, EnIk_CsDrawDefeat };

void EnIk_DrawCutscene(Actor* thisx, PlayState* play) {
    EnIk* this = (EnIk*)thisx;

    if (this->csDrawMode < 0 || this->csDrawMode >= ARRAY_COUNT(sCsDrawFuncs) ||
        sCsDrawFuncs[this->csDrawMode] == NULL) {
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    sCsDrawFuncs[this->csDrawMode](this, play);
}

void EnIk_CsInit(EnIk* this, PlayState* play) {
    if (this->actor.params == IK_TYPE_NABOORU) {
        if (!GET_EVENTCHKINF(EVENTCHKINF_3B)) {
            this->actor.update = EnIk_UpdateCutscene;
            this->actor.draw = EnIk_DrawCutscene;
            Actor_SetScale(&this->actor, 0.01f);
        } else {
            EnIk_ChangeToEnemy(this, play);
            EnIk_StartMinibossBgm();
        }
    }

    PRINTF("En_Ik_inConfrontion_Init : %d !!!!!!!!!!!!!!!!\n", this->actor.params);
}
