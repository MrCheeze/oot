void EnNb_SetupConfrontation(EnNb* this, PlayState* play) {
    AnimationHeader* animation = &gNabooruCollapseFromStandingToKneelingTransitionAnim;

    EnNb_SetCurrentAnim(this, animation, 0, 0.0f, 0);
    this->action = NB_IN_CONFRONTATION;
    this->actor.shape.shadowAlpha = 0;
}

void EnNb_PlayKnuckleDefeatSFX(EnNb* this, PlayState* play) {
    s32 pad[2];

    if (play->csCtx.curFrame == 548) {
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_VO_NB_CRY_0);
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_EN_FANTOM_HIT_THUNDER);
    }
}

void EnNb_PlayKneelingOnGroundSFX(EnNb* this) {
    s32 pad[2];

    if ((this->skelAnime.mode == 2) &&
        (Animation_OnFrame(&this->skelAnime, 18.0f) || Animation_OnFrame(&this->skelAnime, 25.0f))) {
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_EV_HUMAN_BOUND);
    }
}

void EnNb_PlayLookRightSFX(EnNb* this) {
    s32 pad[2];

    if ((this->skelAnime.mode == 2) && Animation_OnFrame(&this->skelAnime, 9.0f)) {
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_STONE);
    }
}

void EnNb_PlayLookLeftSFX(EnNb* this) {
    s32 pad[2];

    if (Animation_OnFrame(&this->skelAnime, 9.0f) || Animation_OnFrame(&this->skelAnime, 13.0f)) {
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_STONE);
    }
}

void EnNb_InitDemo6KInConfrontation(EnNb* this, PlayState* play) {
    Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                kREG(21) + 22.0f + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0xB);
}

void func_80AB2688(EnNb* this, PlayState* play) {
    this->skelAnime.movementFlags |= ANIM_FLAG_UPDATE_XZ;
    AnimTaskQueue_AddActorMovement(play, &this->actor, &this->skelAnime, 1.0f);
}

void func_80AB26C8(EnNb* this) {
    this->action = NB_IN_CONFRONTATION;
    this->drawMode = NB_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

void func_80AB26DC(EnNb* this, PlayState* play) {
    s32 pad;
    AnimationHeader* animation = &gNabooruCollapseFromStandingToKneelingTransitionAnim;
    f32 lastFrame = Animation_GetLastFrame(animation);

    EnNb_SetStartPosRotFromCue1(this, play, 1);
    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    this->action = NB_ACTION_14;
    this->drawMode = NB_DRAW_KNEEL;
    this->actor.shape.shadowAlpha = 0xFF;
}

void EnNb_SetupKneel(EnNb* this) {
    AnimationHeader* animation = &gNabooruCollapseFromStandingToKneelingTransitionAnim;
    f32 lastFrame = Animation_GetLastFrame(animation);

    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    this->action = NB_KNEEL;
    this->drawMode = NB_DRAW_KNEEL;
    this->actor.shape.shadowAlpha = 0xFF;
}

void EnNb_CheckIfKneeling(EnNb* this, s32 animFinished) {
    AnimationHeader* animation = &gNabooruOnAllFoursAnim;

    if (animFinished) {
        Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP,
                         0.0f);
        this->drawMode = NB_DRAW_KNEEL;
    }
}

void EnNb_SetupLookRight(EnNb* this) {
    AnimationHeader* animation = &gNabooruOnAllFoursToOnOneKneeLookingRightTransitionAnim;
    f32 lastFrame = Animation_GetLastFrame(animation);

    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -8.0f);
    this->action = NB_LOOK_RIGHT;
    this->drawMode = NB_DRAW_DEFAULT;
    this->actor.shape.shadowAlpha = 0xFF;
}

void EnNb_CheckIfLookingRight(EnNb* this, s32 animFinished) {
    AnimationHeader* animation = &gNabooruOnOneKneeLookingRightAnim;

    if (animFinished) {
        Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP,
                         0.0f);
        this->drawMode = NB_DRAW_LOOK_DIRECTION;
    }
}

void EnNb_SetupLookLeft(EnNb* this) {
    AnimationHeader* animation = &gNabooruOnOneKneeTurningHeadRightToLeftTransitionAnim;
    f32 lastFrame = Animation_GetLastFrame(animation);

    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -8.0f);
    this->action = NB_LOOK_LEFT;
    this->drawMode = NB_DRAW_LOOK_DIRECTION;
    this->actor.shape.shadowAlpha = 0xFF;
}

void EnNb_CheckIfLookLeft(EnNb* this, s32 animFinished) {
    AnimationHeader* animation = &gNabooruOnOneKneeLookingLeftAnim;

    if (animFinished) {
        Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP,
                         0.0f);
    }
}

void EnNb_SetupDemo6KInConfrontation(EnNb* this, PlayState* play, s32 animFinished) {
    if (!this->flag && animFinished) {
        EnNb_InitDemo6KInConfrontation(this, play);
        this->flag = 1;
    }
}

void EnNb_SetupRun(EnNb* this) {
    AnimationHeader* animation = &gNabooruKneeingToRunningToHitAnim;
    f32 lastFrame = Animation_GetLastFrame(animation);

    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -8.0f);
    this->action = NB_RUN;
    this->drawMode = NB_DRAW_LOOK_DIRECTION;
    this->actor.shape.shadowAlpha = 0xFF;
}

void EnNb_SetupConfrontationDestroy(EnNb* this) {
    this->action = NB_CONFRONTATION_DESTROYED;
    this->drawMode = NB_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

void EnNb_CheckConfrontationCsMode(EnNb* this, PlayState* play) {
    CsCmdActorCue* cue;
    s32 nextCueId;
    s32 currentCueId;

    cue = EnNb_GetCue(play, 1);

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    func_80AB26C8(this);
                    break;
                case 10:
                    func_80AB26DC(this, play);
                    break;
                case 11:
                    EnNb_SetupKneel(this);
                    break;
                case 12:
                    EnNb_SetupLookRight(this);
                    break;
                case 13:
                    EnNb_SetupLookLeft(this);
                    break;
                case 14:
                    EnNb_SetupRun(this);
                    break;
                case 9:
                    EnNb_SetupConfrontationDestroy(this);
                    break;
                default:
                    // "En_Nb_Confrontion_Check_DemoMode: Operation doesn't exist!!!!!!!!"
                    PRINTF("En_Nb_Confrontion_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->cueId = nextCueId;
        }
    }
}

void EnNb_CheckConfrontationCsModeWrapper(EnNb* this, PlayState* play) {
    EnNb_CheckConfrontationCsMode(this, play);
}

void func_80AB2C18(EnNb* this, PlayState* play) {
    EnNb_UpdateEyes(this);
    func_80AB2688(this, play);
    func_80AB1284(this, play);
    EnNb_CheckConfrontationCsMode(this, play);
}

void EnNb_Kneel(EnNb* this, PlayState* play) {
    s32 animFinished;

    EnNb_UpdateEyes(this);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_CheckIfKneeling(this, animFinished);
    EnNb_PlayKneelingOnGroundSFX(this);
    func_80AB2688(this, play);
    func_80AB1284(this, play);
    EnNb_CheckConfrontationCsMode(this, play);
}

void EnNb_LookRight(EnNb* this, PlayState* play) {
    s32 animFinished;

    EnNb_UpdateEyes(this);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_CheckIfLookingRight(this, animFinished);
    EnNb_PlayLookRightSFX(this);
    func_80AB2688(this, play);
    func_80AB1284(this, play);
    EnNb_CheckConfrontationCsMode(this, play);
}

void EnNb_LookLeft(EnNb* this, PlayState* play) {
    s32 animFinished;

    EnNb_UpdateEyes(this);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_CheckIfLookLeft(this, animFinished);
    func_80AB2688(this, play);
    func_80AB1284(this, play);
    EnNb_CheckConfrontationCsMode(this, play);
}

void EnNb_Run(EnNb* this, PlayState* play) {
    s32 animFinished;

    EnNb_PlayKnuckleDefeatSFX(this, play);
    EnNb_UpdateEyes(this);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_PlayLookLeftSFX(this);
    func_80AB2688(this, play);
    func_80AB1284(this, play);
    EnNb_SetupDemo6KInConfrontation(this, play, animFinished);
    EnNb_CheckConfrontationCsMode(this, play);
}

void EnNb_ConfrontationDestroy(EnNb* this, PlayState* play) {
    this->timer++;

    if (this->timer > 60.0f) {
        Actor_Kill(&this->actor);
    }
}

void func_80AB2E70(EnNb* this, PlayState* play) {
    s32 pad;
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_nb_inConfrontion.c", 572);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gNabooruEyeWideTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gNabooruEyeWideTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);
    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL, NULL,
                          &this->actor);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_nb_inConfrontion.c", 593);
}

s32 func_80AB2FC0(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnNb* this = (EnNb*)thisx;

    if (limbIndex == NB_LIMB_HEAD) {
        *dList = gNabooruHeadMouthOpenDL;
    }

    return 0;
}

void func_80AB2FE4(EnNb* this, PlayState* play) {
    s32 pad;
    s16 eyeIdx = this->eyeIdx;
    SkelAnime* skelAnime = &this->skelAnime;
    void* eyeTexture = sEyeTextures[eyeIdx];
    s32 pad1;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_nb_inConfrontion.c", 623);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);
    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, func_80AB2FC0, NULL,
                          &this->actor);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_nb_inConfrontion.c", 644);
}
