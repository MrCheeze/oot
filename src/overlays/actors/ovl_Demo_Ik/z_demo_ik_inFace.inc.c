void DemoIk_Type2Init(DemoIk* this, PlayState* play) {
    s32 pad[2];
    FlexSkeletonHeader* skeleton;
    AnimationHeader* animation;

    switch (this->actor.params) {
        case 3:
            skeleton = &object_ik_Skel_01EB40;
            animation = &object_ik_Anim_01EB14;
            break;
        case 4:
            skeleton = &object_ik_Skel_01EE60;
            animation = &object_ik_Anim_01EE34;
            break;
        case 5:
            skeleton = &object_ik_Skel_000F30;
            animation = &object_ik_Anim_000F0C;
            break;
        default:
            skeleton = &object_ik_Skel_000900;
            animation = &object_ik_Anim_0008DC;
    }

    SkelAnime_InitFlex(play, &this->skelAnime, skeleton, NULL, this->jointTable, this->morphTable, 2);
    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE, 0.0f);
    this->actionMode = 3;
    this->drawMode = 0;
}

void DemoIk_Type2PlaySfxOnFrame(DemoIk* this, f32 frame) {
    if (Animation_OnFrame(&this->skelAnime, frame)) {
        Audio_PlaySfxGeneral(NA_SE_EN_IRONNACK_ARMOR_OFF_DEMO, &this->actor.projectedPos, 4,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }
}

void DemoIk_Type2PlaySfx(DemoIk* this) {
    switch (this->actor.params) {
        case 3:
            DemoIk_Type2PlaySfxOnFrame(this, 33.0f);
            break;
        case 5:
            DemoIk_Type2PlaySfxOnFrame(this, 44.0f);
            break;
    }
}

void func_80983FDC(DemoIk* this) {
    this->actionMode = 3;
    this->drawMode = 0;
}

void func_80983FEC(DemoIk* this, PlayState* play) {
    DemoIk_MoveToStartPos(this, play, 4);
    this->actionMode = 4;
    this->drawMode = 2;
    this->skelAnime.curFrame = 0.0f;
}

void func_8098402C(DemoIk* this) {
    this->actionMode = 5;
    this->drawMode = 2;
    this->skelAnime.curFrame = 0.0f;
}

void func_80984048(DemoIk* this, PlayState* play) {
    CsCmdActorCue* cue = DemoIk_GetCue(play, 4);

    if (cue != NULL) {
        s32 nextCueId = cue->id;
        s32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    func_80983FDC(this);
                    break;
                case 5:
                    func_80983FEC(this, play);
                    break;
                case 6:
                    func_8098402C(this);
                    break;
                case 7:
                    Actor_Kill(&this->actor);
                    break;
                default:
                    // "there is no such action"
                    PRINTF("Demo_Ik_inFace_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void DemoIk_Type2Action0(DemoIk* this, PlayState* play) {
    func_80984048(this, play);
}

void DemoIk_Type2Action1(DemoIk* this, PlayState* play) {
    func_80984048(this, play);
}

void DemoIk_Type2Action2(DemoIk* this, PlayState* play) {
    DemoIk_UpdateSkelAnime(this);
    DemoIk_Type2PlaySfx(this);
    func_80984048(this, play);
}

s32 DemoIk_Type2OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    DemoIk* this = (DemoIk*)thisx;

    if ((limbIndex == 1) && (DemoIk_GetCurFrame(this) < 30.0f)) {
        *dList = NULL;
    }
    return 0;
}

void DemoIk_Type2PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    DemoIk* this = (DemoIk*)thisx;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 frame = DemoIk_GetCurFrame(this);

    OPEN_DISPS(gfxCtx, "../z_demo_ik_inFace.c", 268);
    if (limbIndex == 1 && (frame >= 30.0f)) {
        switch (this->actor.params) {
            case 3:
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_demo_ik_inFace.c", 274);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_017028);
                break;
            case 4:
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_demo_ik_inFace.c", 280);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_017170);
                break;
            case 5:
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_demo_ik_inFace.c", 286);
                gSPDisplayList(POLY_XLU_DISP++, gIronKnuckleArmorRivetAndSymbolDL);
                break;
            default:
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_demo_ik_inFace.c", 292);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016CD8);
                break;
        }
    }
    CLOSE_DISPS(gfxCtx, "../z_demo_ik_inFace.c", 300);
}

void DemoIk_Type2Draw(DemoIk* this, PlayState* play) {
    s32 pad[2];
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(gfxCtx, "../z_demo_ik_inFace.c", 318);
    func_8002EBCC(&this->actor, play, 0);
    Gfx_SetupDL_25Opa(gfxCtx);
    Gfx_SetupDL_25Xlu(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, DemoIk_SetColors(gfxCtx, 245, 225, 155, 30, 30, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, DemoIk_SetColors(gfxCtx, 255, 40, 0, 40, 0, 0));
    gSPSegment(POLY_OPA_DISP++, 0x0A, DemoIk_SetColors(gfxCtx, 255, 255, 255, 20, 40, 30));
    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          DemoIk_Type2OverrideLimbDraw, DemoIk_Type2PostLimbDraw, this);
    CLOSE_DISPS(gfxCtx, "../z_demo_ik_inFace.c", 341);
}
