void func_80B3F3C8(EnXc* this, PlayState* play) {
    this->action = SHEIK_ACTION_45;
}

void func_80B3F3D8(void) {
    Sfx_PlaySfxCentered2(NA_SE_PL_SKIP);
}

void EnXc_PlayDiveSFX(Vec3f* src, PlayState* play) {
    static Vec3f D_80B42DA0;
    f32 wDest[2];

    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, src, &D_80B42DA0, wDest);
    Sfx_PlaySfxAtPos(&D_80B42DA0, NA_SE_EV_DIVE_INTO_WATER);
}

void EnXc_LakeHyliaDive(PlayState* play) {
    CsCmdActorCue* cue = cue = EnXc_GetCue(play, 0);

    if (cue != NULL) {
        Vec3f startPos;

        startPos.x = cue->startPos.x;
        startPos.y = cue->startPos.y;
        startPos.z = cue->startPos.z;

        EffectSsGRipple_Spawn(play, &startPos, 100, 500, 0);
        EffectSsGRipple_Spawn(play, &startPos, 100, 500, 10);
        EffectSsGRipple_Spawn(play, &startPos, 100, 500, 20);
        EffectSsGSplash_Spawn(play, &startPos, NULL, NULL, 1, 0);
        EnXc_PlayDiveSFX(&startPos, play);
    }
}

void func_80B3F534(PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;
    u16 csCurFrame = csCtx->curFrame;

    if (csCurFrame == 310) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, -1044.0f, -1243.0f, 7458.0f, 0, 0, 0, WARP_DESTINATION);
    }
}

void func_80B3F59C(EnXc* this, PlayState* play) {
    static s32 D_80B41DAC = 1;
    CsCmdActorCue* cue = EnXc_GetCue(play, 0);

    if (cue != NULL) {
        s32 nextCueId = cue->id;

        if (nextCueId != D_80B41DAC) {
            switch (nextCueId) {
                case 2:
                    func_80B3F3D8();
                    break;
                case 3:
                    EnXc_LakeHyliaDive(play);
                    break;
                default:
                    break;
            }
            D_80B41DAC = nextCueId;
        }
    }
}

void func_80B3F620(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_45, SHEIK_ACTION_46);
}

void func_80B3F644(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_46, SHEIK_ACTION_47);
}

void func_80B3F668(EnXc* this, PlayState* play) {
    if (EnXc_CheckForCue(this, play, 4, 4)) {
        EnXc_ChangeAnimation(this, &gSheikWalkingAnim, ANIMMODE_LOOP, -12.0f, true);
        this->action = SHEIK_ACTION_48;
        this->actor.world.rot.y += 0x8000;
        this->timer = 0.0f;
    }
}

void func_80B3F6DC(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_48, SHEIK_ACTION_49);
}

void EnXc_SetupKneelAction(EnXc* this, PlayState* play) {
    if (EnXc_CheckForCue(this, play, 16, 4)) {
        EnXc_ChangeAnimation(this, &gSheikKneelingAnim, ANIMMODE_LOOP, 0.0f, false);
        this->action = SHEIK_ACTION_KNEEL;
    }
}

void func_80B3F754(EnXc* this, PlayState* play) {
    if (EnXc_CheckForCue(this, play, 22, 4)) {
        EnXc_ChangeAnimation(this, &gSheikAnim_01A048, ANIMMODE_LOOP, 0.0f, false);
        this->action = SHEIK_ACTION_51;
        func_80B3C588(this, play, 4);
    }
}

void func_80B3F7BC(EnXc* this, PlayState* play) {
    if (EnXc_CheckForCue(this, play, 9, 4)) {
        this->action = SHEIK_ACTION_52;
        this->drawMode = SHEIK_DRAW_NOTHING;
    }
}

void EnXc_ActionFunc45(EnXc* this, PlayState* play) {
    EnXc_ActionFunc20(this, play);
    func_80B3F620(this);
}

void EnXc_ActionFunc46(EnXc* this, PlayState* play) {
    EnXc_ActionFunc21(this, play);
    func_80B3F644(this);
}

void EnXc_ActionFunc47(EnXc* this, PlayState* play) {
    func_80B3F534(play);
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    func_80B3C588(this, play, 4);
    func_80B3F668(this, play);
}

void EnXc_ActionFunc48(EnXc* this, PlayState* play) {
    EnXc_ActionFunc23(this, play);
    func_80B3F6DC(this);
}

void EnXc_ActionFunc49(EnXc* this, PlayState* play) {
    func_80B3D710(this);
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetWalkingSFX(this, play);
    EnXc_SetupKneelAction(this, play);
}

void EnXc_Kneel(EnXc* this, PlayState* play) {
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    func_80B3F59C(this, play);
    func_80B3C588(this, play, 4);
    func_80B3F754(this, play);
}

void EnXc_ActionFunc51(EnXc* this, PlayState* play) {
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    func_80B3F59C(this, play);
    func_80B3C620(this, play, 4);
    func_80B3F7BC(this, play);
}

void EnXc_ActionFunc52(EnXc* this, PlayState* play) {
    func_80B3F59C(this, play);
}
