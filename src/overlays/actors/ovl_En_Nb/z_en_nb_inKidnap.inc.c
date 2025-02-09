void EnNb_InitKidnap(EnNb* this, PlayState* play) {
    EnNb_SetCurrentAnim(this, &gNabooruTrappedInVortexPushingGroundAnim, 0, 0.0f, 0);
    this->action = NB_KIDNAPPED;
    this->actor.shape.shadowAlpha = 0;
    SET_EVENTCHKINF(EVENTCHKINF_95);
}

void EnNb_PlayCrySFX(EnNb* this, PlayState* play) {
    if (play->csCtx.curFrame == 3) {
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_VO_NB_CRY_0);
    }
}

void EnNb_PlayAgonySFX(EnNb* this, PlayState* play) {
    if (play->csCtx.curFrame == 420) {
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_VO_NB_AGONY);
    }
}

void EnNb_SetPosInPortal(EnNb* this, PlayState* play) {
    CsCmdActorCue* cue = EnNb_GetCue(play, 1);
    Vec3f* pos = &this->actor.world.pos;
    f32 lerp;
    s32 pad;
    Vec3f startPos;
    Vec3f endPos;

    if (cue != NULL) {
        lerp = Environment_LerpWeightAccelDecel(cue->endFrame, cue->startFrame, play->csCtx.curFrame, 4, 4);
        startPos.x = cue->startPos.x;
        startPos.y = cue->startPos.y;
        startPos.z = cue->startPos.z;

        endPos.x = cue->endPos.x;
        endPos.y = cue->endPos.y;
        endPos.z = cue->endPos.z;

        pos->x = ((endPos.x - startPos.x) * lerp) + startPos.x;
        pos->y = ((endPos.y - startPos.y) * lerp) + startPos.y;
        pos->z = ((endPos.z - startPos.z) * lerp) + startPos.z;
    }
}

void EnNb_SetupCaptureCutsceneState(EnNb* this, PlayState* play) {
    EnNb_SetStartPosRotFromCue1(this, play, 1);
    this->action = NB_KIDNAPPED;
    this->drawMode = NB_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

void EnNb_SetRaisedArmCaptureAnim(EnNb* this, s32 animFinished) {
    AnimationHeader* animation = &gNabooruSuckedByVortexAnim;

    if (animFinished) {
        Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP,
                         0.0f);
    }
}

void EnNb_SetupLookAroundInKidnap(EnNb* this) {
    AnimationHeader* animation = &gNabooruTrappedInVortexPushingGroundAnim;

    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP, -8.0f);
    this->action = NB_KIDNAPPED_LOOK_AROUND;
    this->drawMode = NB_DRAW_DEFAULT;
}

void EnNb_SetupKidnap(EnNb* this) {
    AnimationHeader* animation = &gNabooruTrappedInVortexRaisingArmAnim;

    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE, -8.0f);
    this->action = NB_PORTAL_FALLTHROUGH;
    this->drawMode = NB_DRAW_DEFAULT;
}

void EnNb_CheckKidnapCsMode(EnNb* this, PlayState* play) {
    CsCmdActorCue* cue = EnNb_GetCue(play, 1);
    s32 nextCueId;
    s32 currentCueId;

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    EnNb_SetupCaptureCutsceneState(this, play);
                    break;
                case 7:
                    EnNb_SetupLookAroundInKidnap(this);
                    break;
                case 8:
                    EnNb_SetupKidnap(this);
                    break;
                case 9:
                    Actor_Kill(&this->actor);
                    break;
                default:
                    // "Operation Doesn't Exist!!!!!!!!"
                    PRINTF("En_Nb_Kidnap_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->cueId = nextCueId;
        }
    }
}

void func_80AB23A8(EnNb* this, PlayState* play) {
    EnNb_PlayCrySFX(this, play);
    EnNb_CheckKidnapCsMode(this, play);
}

void EnNb_MovingInPortal(EnNb* this, PlayState* play) {
    EnNb_PlayCrySFX(this, play);
    EnNb_PlayAgonySFX(this, play);
    EnNb_UpdateEyes(this);
    EnNb_UpdateSkelAnime(this);
    EnNb_CheckKidnapCsMode(this, play);
}

void EnNb_SuckedInByPortal(EnNb* this, PlayState* play) {
    s32 animFinished;

    EnNb_UpdateEyes(this);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_SetRaisedArmCaptureAnim(this, animFinished);
    EnNb_SetPosInPortal(this, play);
    EnNb_CheckKidnapCsMode(this, play);
}
