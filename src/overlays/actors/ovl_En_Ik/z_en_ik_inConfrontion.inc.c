void En_Ik_inConfrontion_SetSound_BGM(void) {
    Na_StartMiddleBossBgm(NA_BGM_MINI_BOSS);
}

// Cutscene: Nabooru Knuckle Wakes up
void En_Ik_inConfrontion_SetSound_Standup(EnIk* this) {
    if (Skeleton_Info_frame_check(&this->skelAnime, 1.0f)) {
        Nai_FxFlagEntry(NA_SE_EN_IRONNACK_WAKEUP, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (Skeleton_Info_frame_check(&this->skelAnime, 33.0f)) {
        Nai_FxFlagEntry(NA_SE_EN_IRONNACK_WALK, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (Skeleton_Info_frame_check(&this->skelAnime, 68.0f) || Skeleton_Info_frame_check(&this->skelAnime, 80.0f)) {
        Nai_FxFlagEntry(NA_SE_EN_IRONNACK_ARMOR_DEMO, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (Skeleton_Info_frame_check(&this->skelAnime, 107.0f)) {
        Nai_FxFlagEntry(NA_SE_EN_IRONNACK_FINGER_DEMO, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (Skeleton_Info_frame_check(&this->skelAnime, 156.0f)) {
        Nai_FxFlagEntry(NA_SE_EN_IRONNACK_ARMOR_DEMO, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (Skeleton_Info_frame_check(&this->skelAnime, 188.0f)) {
        Nai_FxFlagEntry(NA_SE_EN_IRONNACK_WAVE_DEMO, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
}

// Cutscene: Summons Axe for Nabooru Knuckle
void En_Ik_inConfrontion_SetSound_Axe(EnIk* this, PlayState* play, Vec3f* pos) {
    Nai_FxFlagEntry(NA_SE_EN_TWINROBA_TRANSFORM, &this->actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
}

void Birth_Effect_In_En_Ik_inConfrontion(EnIk* this, PlayState* play, Vec3f* pos) {
    static Vec3f array_offset[] = {
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

        for (i = ARRAY_COUNT(array_offset) - 1; i >= 0; i--) {
            Color_RGBA8 primColor = { 200, 200, 200, 255 };
            Color_RGBA8 envColor = { 150, 150, 150, 0 };
            s32 randColorOffset;
            Vec3f effectPos;

            Matrix_Position(&array_offset[i], &effectPos);
            randColorOffset = (fqrand() * 20.0f) - 10.0f;

            primColor.r += randColorOffset;
            primColor.g += randColorOffset;
            primColor.b += randColorOffset;

            envColor.r += randColorOffset;
            envColor.g += randColorOffset;
            envColor.b += randColorOffset;

            Effect_SS_Dust_sc_cl_ct(play, &effectPos, &effectVelocity, &effectAccel, &primColor, &envColor,
                          (fqrand() * 60.0f) + 300.0f, 0);
        }

        this->isAxeSummoned = true;
        En_Ik_inConfrontion_SetSound_Axe(this, play, pos);
    }
}

void En_Ik_inConfrontion_BGcheck(EnIk* this, PlayState* play) {
    Actor_BGcheck2(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

s32 En_Ik_inConfrontion_Animation_Base(EnIk* this) {
    return Skeleton_Info2_anime_play(&this->skelAnime);
}

CsCmdActorCue* En_Ik_inConfrontion_Get_npcdemopnt(PlayState* play, s32 cueChannel) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];

        return cue;
    }

    return NULL;
}

void En_Ik_inConfrontion_Set_DemoStartPosAngle(EnIk* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = En_Ik_inConfrontion_Get_npcdemopnt(play, cueChannel);

    if (cue != NULL) {
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;

        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;
    }
}

f32 En_Ik_inConfrontion_Get_anime_frame(Actor* thisx) {
    EnIk* this = (EnIk*)thisx;

    return this->skelAnime.curFrame;
}

// unused
void En_Ik_inConfrontion_setup_Wait(EnIk* this) {
    this->csAction = IK_CS_ACTION_0;
    this->csDrawMode = IK_CS_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

// Cutscene: Nabooru sitting and Kotake and Koume looking at her
void En_Ik_inConfrontion_setup_Sitdown(EnIk* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gIronKnuckleNabooruSummonAxeAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gIronKnuckleNabooruSummonAxeAnim), ANIMMODE_ONCE, 0.0f);
    En_Ik_inConfrontion_Set_DemoStartPosAngle(this, play, 4);
    this->csAction = IK_CS_ACTION_1;
    this->csDrawMode = IK_CS_DRAW_INTRO;
    this->actor.shape.shadowAlpha = 255;
}

// Cutscene: Nabooru Knuckle starts to stand up
void En_Ik_inConfrontion_setup_Standup(EnIk* this) {
    Skeleton_Info2_init(&this->skelAnime, &gIronKnuckleNabooruSummonAxeAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gIronKnuckleNabooruSummonAxeAnim), ANIMMODE_ONCE, 0.0f);
    this->csAction = IK_CS_ACTION_2;
    this->csDrawMode = IK_CS_DRAW_INTRO;
    this->isAxeSummoned = false;
    this->actor.shape.shadowAlpha = 255;
}

void En_Ik_inConfrontion_setup_Fight(EnIk* this, PlayState* play, s32 animFinished) {
    if (animFinished && (En_Ik_inConfrontion_Get_npcdemopnt(play, 4) != NULL)) {
        En_Ik_Chenge_DemoToFight(this, play);
    }
}

#include "z_en_ik_inAwake.inc.c"

void En_Ik_inConfrontion_Check_DemoMode(EnIk* this, PlayState* play) {
    CsCmdActorCue* cue = En_Ik_inConfrontion_Get_npcdemopnt(play, 4);
    u32 nextCueId;
    u32 currentCueId;

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    En_Ik_inConfrontion_setup_Wait(this);
                    break;

                case 2:
                    En_Ik_inConfrontion_setup_Sitdown(this, play);
                    break;

                case 3:
                    En_Ik_inConfrontion_setup_Standup(this);
                    break;

                case 4:
                    En_Ik_Chenge_DemoToFight(this, play);
                    break;

                case 5:
                    En_Ik_inConfrontion_setup_Awake(this, play);
                    break;

                case 6:
                    En_Ik_inConfrontion_setup_Remove(this, play);
                    break;

                case 7:
                    En_Ik_inConfrontion_setup_Disappear(this, play);
                    break;

                default:
                    PRINTF("En_Ik_inConfrontion_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }

            this->cueId = nextCueId;
        }
    }
}

void En_Ik_inConfrontion_main_wait(EnIk* this, PlayState* play) {
    En_Ik_inConfrontion_Check_DemoMode(this, play);
}

void En_Ik_inConfrontion_main_sitdown(EnIk* this, PlayState* play) {
    En_Ik_inConfrontion_BGcheck(this, play);
    En_Ik_inConfrontion_Check_DemoMode(this, play);
}

void En_Ik_inConfrontion_main_standup(EnIk* this, PlayState* play) {
    s32 animFinished;

    animFinished = En_Ik_inConfrontion_Animation_Base(this);
    En_Ik_inConfrontion_SetSound_Standup(this);
    En_Ik_inConfrontion_BGcheck(this, play);
    En_Ik_inConfrontion_Check_DemoMode(this, play);
    En_Ik_inConfrontion_setup_Fight(this, play, animFinished);
}

void En_Ik_inConfrontion_main(Actor* thisx, PlayState* play) {
    static EnIkActionFunc proc[] = {
        En_Ik_inConfrontion_main_wait, En_Ik_inConfrontion_main_sitdown, En_Ik_inConfrontion_main_standup, En_Ik_inConfrontion_main_awake, En_Ik_inConfrontion_main_remove, En_Ik_inConfrontion_main_disappear,
    };

    EnIk* this = (EnIk*)thisx;

    if (this->csAction < 0 || this->csAction >= ARRAY_COUNT(proc) || proc[this->csAction] == NULL) {
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    proc[this->csAction](this, play);
}

s32 En_Ik_inConfrontion_ChangeDraw_axe(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnIk* this = (EnIk*)thisx;
    f32 curFrame;

    switch (limbIndex) {
        case IRON_KNUCKLE_LIMB_AXE:
            curFrame = this->skelAnime.curFrame;
            if (curFrame < 120.0f) {
                *dList = NULL;
            } else {
                Birth_Effect_In_En_Ik_inConfrontion(this, play, pos);
            }
            break;

        case IRON_KNUCKLE_LIMB_WAIST:
        case IRON_KNUCKLE_LIMB_MAX:
            *dList = NULL;
            break;
    }

    return false;
}

void En_Ik_inConfrontion_AddDraw_armer(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
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

void En_Ik_inConfrontion_draw_none(EnIk* this, PlayState* play) {
}

void En_Ik_inConfrontion_draw_normal(EnIk* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    SkelAnime* skelAnime = &this->skelAnime;
    s32 pad[2];

    OPEN_DISPS(gfxCtx, "../z_en_ik_inConfrontion.c", 630);

    Actor_HiliteReflect_set_init(&this->actor, play, 0);
    _texture_z_light_fog_prim(gfxCtx);
    _texture_z_light_fog_prim_xlu(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, set_col(gfxCtx, 245, 225, 155, 30, 30, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, set_col(gfxCtx, 255, 40, 0, 40, 0, 0));
    gSPSegment(POLY_OPA_DISP++, 0x0A, set_col(gfxCtx, 255, 255, 255, 20, 40, 30));
    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          En_Ik_inConfrontion_ChangeDraw_axe, En_Ik_inConfrontion_AddDraw_armer, this);

    CLOSE_DISPS(gfxCtx, "../z_en_ik_inConfrontion.c", 653);
}

void En_Ik_inConfrontion_draw(Actor* thisx, PlayState* play) {
    static EnIkDrawFunc proc[] = { En_Ik_inConfrontion_draw_none, En_Ik_inConfrontion_draw_normal, En_Ik_inConfrontion_draw_change_head };

    EnIk* this = (EnIk*)thisx;

    if (this->csDrawMode < 0 || this->csDrawMode >= ARRAY_COUNT(proc) ||
        proc[this->csDrawMode] == NULL) {
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    proc[this->csDrawMode](this, play);
}

void En_Ik_inConfrontion_Init(EnIk* this, PlayState* play) {
    if (this->actor.params == IK_TYPE_NABOORU) {
        if (!GET_EVENTCHKINF(EVENTCHKINF_3B)) {
            this->actor.update = En_Ik_inConfrontion_main;
            this->actor.draw = En_Ik_inConfrontion_draw;
            Actor_set_scale(&this->actor, 0.01f);
        } else {
            En_Ik_Chenge_DemoToFight(this, play);
            En_Ik_inConfrontion_SetSound_BGM();
        }
    }

    PRINTF("En_Ik_inConfrontion_Init : %d !!!!!!!!!!!!!!!!\n", this->actor.params);
}
