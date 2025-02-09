s32 DemoIk_GetCueChannel(s32 params) {
    s32 ret;

    if (params == 0) {
        ret = 5;
    } else if (params == 1) {
        ret = 6;
    } else {
        ret = 7;
    }
    return ret;
}

void DemoIk_Type1PlaySfx(DemoIk* this) {
    switch (this->actor.params) {
        case 0:
            if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
                Audio_PlaySfxGeneral(NA_SE_EN_IRONNACK_ARMOR_LAND1_DEMO, &this->actor.projectedPos, 4,
                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            }
            break;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, 10.0f)) {
                Audio_PlaySfxGeneral(NA_SE_EN_IRONNACK_ARMOR_LAND3_DEMO, &this->actor.projectedPos, 4,
                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            }
            break;
        case 2:
            if (Animation_OnFrame(&this->skelAnime, 9.0f)) {
                Audio_PlaySfxGeneral(NA_SE_EN_IRONNACK_ARMOR_LAND2_DEMO, &this->actor.projectedPos, 4,
                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            }
            break;
    }
}

void DemoIk_SpawnDeadDb(DemoIk* this, PlayState* play) {
    static Vec3f deadDbOffsets[] = {
        { -14.0f, 5.0f, 5.0f },  { -20.0f, 12.0f, 0.0f }, { -5.0f, 10.0f, -1.0f }, { -10.0f, 8.0f, 14.0f },
        { -3.0f, 10.0f, 7.0f },  { -10.0f, 11.0f, 0.0f }, { 9.0f, 10.0f, -8.0f },  { 4.0f, 10.0f, 3.0f },
        { -6.0f, 13.0f, -5.0f }, { 1.0f, 9.0f, 3.0f },    { -10.0f, 9.0f, 1.0f },
    };
    s32 i;
    s32 cueChannel = DemoIk_GetCueChannel(this->actor.params);

    if (DemoIk_CheckForCue(play, 5, cueChannel)) {
        Vec3f pos;
        Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
        s32 startIndex;
        s32 endIndex;

        if (cueChannel == 5) {
            startIndex = 0;
            endIndex = 4;
        } else if (cueChannel == 7) {
            startIndex = 4;
            endIndex = 8;
        } else {
            startIndex = 8;
            endIndex = 11;
        }
        for (i = startIndex; i < endIndex; i++) {
            pos.x = deadDbOffsets[i].x + this->actor.world.pos.x;
            pos.y = deadDbOffsets[i].y + this->actor.world.pos.y;
            pos.z = deadDbOffsets[i].z + this->actor.world.pos.z;
            EffectSsDeadDb_Spawn(play, &pos, &zeroVec, &zeroVec, 10, 7, 255, 255, 255, 255, 0, 0, 255, 1, 9, true);
        }
    }
}

void DemoIk_MoveToStartPos(DemoIk* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = DemoIk_GetCue(play, cueChannel);

    if (cue != NULL) {
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;
        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;
    }
}

void DemoIk_Type1Init(DemoIk* this, PlayState* play) {
    s32 pad[3];
    SkeletonHeader* skeleton;
    AnimationHeader* animation;
    f32 phi_f0;

    switch (this->actor.params) {
        case 0:
            skeleton = &object_ik_Skel_000C90;
            animation = &object_ik_Anim_000C6C;
            phi_f0 = 30.0f;
            break;
        case 1:
            skeleton = &object_ik_Skel_000660;
            animation = &object_ik_Anim_000634;
            phi_f0 = 10.0f;
            break;
        default:
            skeleton = &object_ik_Skel_000380;
            animation = &object_ik_Anim_00035C;
            phi_f0 = 20.0f;
            // No break is required for matching
    }
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, phi_f0);
    SkelAnime_Init(play, &this->skelAnime, skeleton, NULL, this->jointTable, this->morphTable, 2);
    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE, 0.0f);
}

void func_8098393C(DemoIk* this) {
    this->actionMode = 0;
    this->drawMode = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_8098394C(DemoIk* this, PlayState* play) {
    DemoIk_EndMove(this);
    DemoIk_MoveToStartPos(this, play, DemoIk_GetCueChannel(this->actor.params));
    this->actionMode = 1;
    this->drawMode = 1;
    this->actor.shape.shadowAlpha = 255;
    this->skelAnime.curFrame = 0.0f;
}

void func_809839AC(DemoIk* this) {
    this->actionMode = 2;
    this->drawMode = 1;
    this->actor.shape.shadowAlpha = 255;
    this->skelAnime.curFrame = 0.0f;
}

void func_809839D0(DemoIk* this, PlayState* play) {
    CsCmdActorCue* cue = DemoIk_GetCue(play, DemoIk_GetCueChannel(this->actor.params));

    if (cue != NULL) {
        s32 nextCueId = cue->id;
        s32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    func_8098393C(this);
                    break;
                case 2:
                    func_8098394C(this, play);
                    break;
                case 3:
                    func_809839AC(this);
                    break;
                case 4:
                    Actor_Kill(&this->actor);
                    break;
                case 5:
                case 6:
                    break;
                default:
                    // "there is no such action"
                    PRINTF("Demo_Ik_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void DemoIk_Type1Action0(DemoIk* this, PlayState* play) {
    func_809839D0(this, play);
}

void DemoIk_Type1Action1(DemoIk* this, PlayState* play) {
    DemoIk_BgCheck(this, play);
    func_809839D0(this, play);
}

void DemoIk_Type1Action2(DemoIk* this, PlayState* play) {
    DemoIk_UpdateSkelAnime(this);
    DemoIk_Type1PlaySfx(this);
    DemoIk_SetMove(this, play);
    DemoIk_BgCheck(this, play);
    DemoIk_SpawnDeadDb(this, play);
    func_809839D0(this, play);
}

void DemoIk_Type1PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    DemoIk* this = (DemoIk*)thisx;

    OPEN_DISPS(gfxCtx, "../z_demo_ik_inArmer.c", 385);
    if (limbIndex == 1) {
        switch (this->actor.params) {
            case 0:
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_demo_ik_inArmer.c", 390);
                gSPDisplayList(POLY_XLU_DISP++, gIronKnuckleArmorRivetAndSymbolDL);
                break;
            case 2:
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_demo_ik_inArmer.c", 396);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016F88);
                break;
        }
    }
    CLOSE_DISPS(gfxCtx, "../z_demo_ik_inArmer.c", 404);
}

void DemoIk_Type1Draw(DemoIk* this, PlayState* play) {
    s32 pad[2];
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(gfxCtx, "../z_demo_ik_inArmer.c", 422);
    func_8002EBCC(&this->actor, play, 0);
    Gfx_SetupDL_25Opa(gfxCtx);
    Gfx_SetupDL_25Xlu(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, DemoIk_SetColors(gfxCtx, 245, 225, 155, 30, 30, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, DemoIk_SetColors(gfxCtx, 255, 40, 0, 40, 0, 0));
    gSPSegment(POLY_OPA_DISP++, 0x0A, DemoIk_SetColors(gfxCtx, 255, 255, 255, 20, 40, 30));
    SkelAnime_DrawOpa(play, skelAnime->skeleton, skelAnime->jointTable, NULL, DemoIk_Type1PostLimbDraw, this);
    CLOSE_DISPS(gfxCtx, "../z_demo_ik_inArmer.c", 444);
}
