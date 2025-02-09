void Demo_Ik_inFace_Init(DemoIk* this, PlayState* play) {
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

    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, skeleton, NULL, this->jointTable, this->morphTable, 2);
    Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, Si2_anime_end_frame(animation), ANIMMODE_ONCE, 0.0f);
    this->actionMode = 3;
    this->drawMode = 0;
}

void Demo_Ik_inFace_SetSound_Off(DemoIk* this, f32 frame) {
    if (Skeleton_Info_frame_check(&this->skelAnime, frame)) {
        Nai_FxFlagEntry(NA_SE_EN_IRONNACK_ARMOR_OFF_DEMO, &this->actor.projectedPos, 4,
                             &_dummy_one, &_dummy_one, &_dummy_zero_s8);
    }
}

void Demo_Ik_inFace_SetSound_Drop(DemoIk* this) {
    switch (this->actor.params) {
        case 3:
            Demo_Ik_inFace_SetSound_Off(this, 33.0f);
            break;
        case 5:
            Demo_Ik_inFace_SetSound_Off(this, 44.0f);
            break;
    }
}

void Demo_Ik_inFace_setup_Wait(DemoIk* this) {
    this->actionMode = 3;
    this->drawMode = 0;
}

void Demo_Ik_inFace_setup_Stick(DemoIk* this, PlayState* play) {
    Demo_Ik_Set_DemoStartPosAngle(this, play, 4);
    this->actionMode = 4;
    this->drawMode = 2;
    this->skelAnime.curFrame = 0.0f;
}

void Demo_Ik_inFace_setup_Drop(DemoIk* this) {
    this->actionMode = 5;
    this->drawMode = 2;
    this->skelAnime.curFrame = 0.0f;
}

void Demo_Ik_inFace_Check_DemoMode(DemoIk* this, PlayState* play) {
    CsCmdActorCue* cue = Demo_Ik_Get_npcdemopnt(play, 4);

    if (cue != NULL) {
        s32 nextCueId = cue->id;
        s32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    Demo_Ik_inFace_setup_Wait(this);
                    break;
                case 5:
                    Demo_Ik_inFace_setup_Stick(this, play);
                    break;
                case 6:
                    Demo_Ik_inFace_setup_Drop(this);
                    break;
                case 7:
                    Actor_delete(&this->actor);
                    break;
                default:
                    // "there is no such action"
                    PRINTF("Demo_Ik_inFace_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void Demo_Ik_inFace_main_wait(DemoIk* this, PlayState* play) {
    Demo_Ik_inFace_Check_DemoMode(this, play);
}

void Demo_Ik_inFace_main_stick(DemoIk* this, PlayState* play) {
    Demo_Ik_inFace_Check_DemoMode(this, play);
}

void Demo_Ik_inFace_main_drop(DemoIk* this, PlayState* play) {
    Demo_Ik_Animation_Base(this);
    Demo_Ik_inFace_SetSound_Drop(this);
    Demo_Ik_inFace_Check_DemoMode(this, play);
}

s32 Demo_Ik_inFace_before(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    DemoIk* this = (DemoIk*)thisx;

    if ((limbIndex == 1) && (Demo_Ik_Get_anime_frame(this) < 30.0f)) {
        *dList = NULL;
    }
    return 0;
}

void Demo_Ik_inFace_after(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    DemoIk* this = (DemoIk*)thisx;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 frame = Demo_Ik_Get_anime_frame(this);

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

void Demo_Ik_inFace_draw_normal(DemoIk* this, PlayState* play) {
    s32 pad[2];
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(gfxCtx, "../z_demo_ik_inFace.c", 318);
    Actor_HiliteReflect_set_init(&this->actor, play, 0);
    _texture_z_light_fog_prim(gfxCtx);
    _texture_z_light_fog_prim_xlu(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Demo_Ik_Setcolor(gfxCtx, 245, 225, 155, 30, 30, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, Demo_Ik_Setcolor(gfxCtx, 255, 40, 0, 40, 0, 0));
    gSPSegment(POLY_OPA_DISP++, 0x0A, Demo_Ik_Setcolor(gfxCtx, 255, 255, 255, 20, 40, 30));
    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          Demo_Ik_inFace_before, Demo_Ik_inFace_after, this);
    CLOSE_DISPS(gfxCtx, "../z_demo_ik_inFace.c", 341);
}
