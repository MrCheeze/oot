void EnXc_DoNothing(EnXc* this, PlayState* play) {
}

static Vec3f sSfxPos;

void EnXc_SetWalkingSFX(EnXc* this, PlayState* play) {
    s32 pad[2];
    u32 sfxId;
    s32 pad2;

    if (Animation_OnFrame(&this->skelAnime, 11.0f) || Animation_OnFrame(&this->skelAnime, 23.0f)) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            sfxId = NA_SE_PL_WALK_GROUND;
            sfxId += SurfaceType_GetSfxOffset(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
            Sfx_PlaySfxAtPos(&this->actor.projectedPos, sfxId);
        }
    }
}

void EnXc_SetNutThrowSFX(EnXc* this, PlayState* play) {
    s32 pad[2];
    u32 sfxId;
    s32 pad2;

    if (Animation_OnFrame(&this->skelAnime, 7.0f)) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            sfxId = NA_SE_PL_WALK_GROUND;
            sfxId += SurfaceType_GetSfxOffset(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
            Sfx_PlaySfxAtPos(&this->actor.projectedPos, sfxId);
        }
    }
    if (Animation_OnFrame(&this->skelAnime, 20.0f)) {
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_VO_SK_SHOUT);
    }
}

void EnXc_SetLandingSFX(EnXc* this, PlayState* play) {
    u32 sfxId;
    s16 sceneId = play->sceneId;

    if ((gSaveContext.sceneLayer != 4) || (sceneId != SCENE_DESERT_COLOSSUS)) {
        if (Animation_OnFrame(&this->skelAnime, 11.0f)) {
            sfxId = NA_SE_PL_WALK_GROUND;
            sfxId += SurfaceType_GetSfxOffset(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
            Sfx_PlaySfxAtPos(&this->actor.projectedPos, sfxId);
        }
    }
}

void EnXc_SetColossusAppearSFX(EnXc* this, PlayState* play) {
    s16 sceneId;

    if (gSaveContext.sceneLayer == 4) {
        sceneId = play->sceneId;
        if (sceneId == SCENE_DESERT_COLOSSUS) {
            CutsceneContext* csCtx = &play->csCtx;
            u16 csCurFrame = csCtx->curFrame;
            f32 wDest[2];

            if (csCurFrame == 119) {
                Vec3f pos = { -611.0f, 728.0f, -2.0f };

                SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &pos, &sSfxPos, wDest);
                Sfx_PlaySfxAtPos(&sSfxPos, NA_SE_EV_JUMP_CONC);
            } else if (csCurFrame == 164) {
                Vec3f pos = { -1069.0f, 38.0f, 0.0f };
                s32 pad;

                SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &pos, &sSfxPos, wDest);
                Sfx_PlaySfxAtPos(&sSfxPos, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_STONE);
            }
        }
    }
}

void func_80B3D118(PlayState* play) {
    s16 sceneId;

    if ((gSaveContext.sceneLayer != 4) || (sceneId = play->sceneId, sceneId != SCENE_DESERT_COLOSSUS)) {
        Sfx_PlaySfxCentered2(NA_SE_PL_SKIP);
    }
}

void EnXc_SetColossusWindSFX(PlayState* play) {
    if (gSaveContext.sceneLayer == 4) {
        static s32 D_80B41D90 = 0;
        static Vec3f sPos = { 0.0f, 0.0f, 0.0f };
        static Vec3f D_80B42DB0;
        s32 pad;
        s16 sceneId = play->sceneId;

        if (sceneId == SCENE_DESERT_COLOSSUS) {
            CutsceneContext* csCtx = &play->csCtx;
            u16 csCurFrame = csCtx->curFrame;

            if ((csCurFrame >= 120) && (csCurFrame < 164)) {
                s32 pad;
                Vec3f* eye = &play->view.eye;

                if (D_80B41D90 != 0) {
                    f32 speed = Math3D_Vec3f_DistXYZ(&D_80B42DB0, eye) / 7.058922f;
#if DEBUG_FEATURES
                    static f32 sMaxSpeed = 0.0f;

                    sMaxSpeed = CLAMP_MIN(sMaxSpeed, speed);
                    PRINTF("MAX speed = %f\n", sMaxSpeed);
#endif

                    speed = CLAMP_MAX(speed, 2.0f);
                    func_800F436C(&sPos, NA_SE_EV_FLYING_AIR - SFX_FLAG, 0.6f + (0.4f * speed));
                }

                D_80B42DB0.x = eye->x;
                D_80B42DB0.y = eye->y;
                D_80B42DB0.z = eye->z;
                D_80B41D90 = 1;
            }
        }
    }
}

void EnXc_SpawnFlame(EnXc* this, PlayState* play) {
    static s32 sFlameSpawned = false;

    if (!sFlameSpawned) {
        CsCmdActorCue* cue = EnXc_GetCue(play, 0);
        f32 xPos = cue->startPos.x;
        f32 yPos = cue->startPos.y;
        f32 zPos = cue->startPos.z;

        this->flameActor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_LIGHT, xPos, yPos, zPos, 0, 0, 0, 5);
        sFlameSpawned = true;
    }
}

void EnXc_SetupFlamePos(EnXc* this, PlayState* play) {
    Vec3f* attachedPos;
    CsCmdActorCue* cue = EnXc_GetCue(play, 0);

    if (this->flameActor != NULL) {
        attachedPos = &this->flameActor->world.pos;
        if (!this) {}
        attachedPos->x = cue->startPos.x;
        attachedPos->y = cue->startPos.y;
        attachedPos->z = cue->startPos.z;
    }
}

void EnXc_DestroyFlame(EnXc* this) {
    if (this->flameActor != NULL) {
        Actor_Kill(this->flameActor);
        this->flameActor = NULL;
    }
    Actor_Kill(&this->actor);
}

void EnXc_InitFlame(EnXc* this, PlayState* play) {
    static s32 D_80B41DA8 = 1;
    s32 pad;
    s16 sceneId = play->sceneId;

    if (sceneId == SCENE_DEATH_MOUNTAIN_CRATER) {
        CsCmdActorCue* cue = EnXc_GetCue(play, 0);
        if (cue != NULL) {
            s32 nextCueId = cue->id;

            if (D_80B41DA8 != nextCueId) {
                if (nextCueId != 1) {
                    EnXc_SpawnFlame(this, play);
                }

                if (nextCueId == 1) {
                    EnXc_DestroyFlame(this);
                }

                D_80B41DA8 = nextCueId;
            }

            EnXc_SetupFlamePos(this, play);
        }
    }
}

void func_80B3D48C(EnXc* this, PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;
    CsCmdActorCue* playerCue = csCtx->playerCue;
    s16 yaw;

    if (playerCue != NULL) {
        yaw = playerCue->rot.y + 0x8000;
    } else {
        Player* player = GET_PLAYER(play);
        yaw = player->actor.world.rot.y + 0x8000;
    }

    this->actor.shape.rot.y = this->actor.world.rot.y = yaw;
}

AnimationHeader* EnXc_GetCurrentHarpAnim(PlayState* play, s32 index) {
    AnimationHeader* animation = &gSheikPlayingHarp5Anim;
    CsCmdActorCue* cue = EnXc_GetCue(play, index);

    if (cue != NULL) {
        u16 cueId = cue->id;

        if (cueId == 11) {
            animation = &gSheikPlayingHarp3Anim;
        } else if (cueId == 12) {
            animation = &gSheikPlayingHarp2Anim;
        } else if (cueId == 13) {
            animation = &gSheikPlayingHarp4Anim;
        } else if (cueId == 23) {
            animation = &gSheikPlayingHarpAnim;
        } else {
            animation = &gSheikPlayingHarp5Anim;
        }
    }
    return animation;
}

void EnXc_CalcXZAccel(EnXc* this) {
    f32 timer = this->timer;
    f32* speedXZ = &this->actor.speed;

    if (timer < 9.0f) {
        *speedXZ = 0.0f;
    } else if (timer < 3.0f) {
        *speedXZ = (((kREG(2) * 0.01f) + 1.2f) / 3.0f) * (timer - 9.0f);
    } else {
        *speedXZ = (kREG(2) * 0.01f) + 1.2f;
    }

    Actor_MoveXZGravity(&this->actor);
}

void func_80B3D644(EnXc* this) {
    Actor_MoveXZGravity(&this->actor);
}

void EnXc_CalcXZSpeed(EnXc* this) {
    f32 timer = this->timer;
    f32* speedXZ = &this->actor.speed;

    if (timer < 3.0f) {
        *speedXZ = (((kREG(2) * 0.01f) + 1.2f) / 3.0f) * (3.0f - timer);
    } else {
        *speedXZ = 0.0f;
    }
    Actor_MoveXZGravity(&this->actor);
}

void func_80B3D6F0(EnXc* this) {
    EnXc_CalcXZAccel(this);
}

void func_80B3D710(EnXc* this) {
    Actor_MoveXZGravity(&this->actor);
}

void func_80B3D730(EnXc* this) {
    EnXc_CalcXZSpeed(this);
}

void func_80B3D750(EnXc* this, PlayState* play) {
    if (EnXc_MinuetCS(this, play) && EnXc_BoleroCS(this, play)) {
        this->action = SHEIK_ACTION_WAIT;
    }
}

void EnXc_SetupFallFromSkyAction(EnXc* this, PlayState* play) {
    s32 pad;
    CutsceneContext* csCtx = &play->csCtx;

    if (csCtx->state != 0) {
        CsCmdActorCue* cue = csCtx->actorCues[4];

        if (cue != NULL && cue->id == 2) {
            s32 pad;
            Vec3f* pos = &this->actor.world.pos;
            SkelAnime* skelAnime = &this->skelAnime;
            f32 frameCount = Animation_GetLastFrame(&gSheikFallingFromSkyAnim);

            this->action = SHEIK_ACTION_GRACEFUL_FALL;
            this->drawMode = SHEIK_DRAW_DEFAULT;

            pos->x = cue->startPos.x;
            pos->y = cue->startPos.y;
            pos->z = cue->startPos.z;

            func_80B3D48C(this, play);
            func_80B3C964(this, play);
            Animation_Change(skelAnime, &gSheikFallingFromSkyAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
            func_80B3D118(play);
        }
    }
}

void func_80B3D8A4(EnXc* this, PlayState* play, s32 animFinished) {
    if (animFinished) {
        SkelAnime* skelAnime = &this->skelAnime;
        f32 frameCount = Animation_GetLastFrame(&gSheikWalkingAnim);

        Animation_Change(skelAnime, &gSheikWalkingAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, -8.0f);

        this->action = SHEIK_ACTION_ACCEL;
        this->timer = 0.0f;

        func_80B3C9DC(this);
        this->actor.gravity = -((kREG(1) * 0.01f) + 13.0f);
        this->actor.minVelocityY = -((kREG(1) * 0.01f) + 13.0f);
    } else {
        func_80B3C8CC(this, play);
    }
}

void EnXc_SetupWalkAction(EnXc* this) {
    f32* timer = &this->timer;

    *timer += 1.0f;
    if (*timer >= 12.0f) {
        this->actor.speed = (kREG(2) * 0.01f) + 1.2f;
        this->action = SHEIK_ACTION_WALK;
    }
}

void EnXc_SetupHaltAction(EnXc* this) {
    SkelAnime* skelAnime = &this->skelAnime;
    f32 xzDistToPlayer = this->actor.xzDistToPlayer;

    if (xzDistToPlayer <= (kREG(3) + 95.0f)) {
        f32 frameCount = Animation_GetLastFrame(&gSheikIdleAnim);

        Animation_Change(skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, -12.0f);
        this->action = SHEIK_ACTION_HALT;
        this->timer = 0.0f;
    }
}

void EnXc_SetupStoppedAction(EnXc* this) {
    f32* timer = &this->timer;

    *timer += 1.0f;
    if (*timer >= 12.0f) {
        this->action = SHEIK_ACTION_STOPPED;
        this->actor.speed = 0.0f;
    }
}

void func_80B3DAF0(EnXc* this, PlayState* play) {
    CsCmdActorCue* cue = EnXc_GetCue(play, 4);
    u16 cueId;

    if (cue != NULL) {
        cueId = cue->id;

        if (cueId == 3 || cueId == 11 || cueId == 12 || cueId == 13 || cueId == 23) {
            f32 frameCount = Animation_GetLastFrame(&gSheikPullingOutHarpAnim);

            Animation_Change(&this->skelAnime, &gSheikPullingOutHarpAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, -4.0f);
            this->action = SHEIK_ACTION_7;
            this->drawMode = SHEIK_DRAW_PULLING_OUT_HARP;
        }
    }
}

void EnXc_SetupInitialHarpAction(EnXc* this, s32 animFinished) {
    SkelAnime* skelAnime;
    f32 frameCount;

    if (animFinished) {
        skelAnime = &this->skelAnime;
        frameCount = Animation_GetLastFrame(&gSheikInitialHarpAnim);
        Animation_Change(skelAnime, &gSheikInitialHarpAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
        this->action = SHEIK_ACTION_HARP_READY;
        this->drawMode = SHEIK_DRAW_HARP;
    }
}

void EnXc_SetupPlayingHarpAction(EnXc* this, PlayState* play, s32 animFinished) {
    s32 pad;
    SkelAnime* skelAnime;
    AnimationHeader* animation;
    f32 frameCount;

    if (animFinished) {
        skelAnime = &this->skelAnime;
        animation = EnXc_GetCurrentHarpAnim(play, 4);
        frameCount = Animation_GetLastFrame(animation);
        Animation_Change(skelAnime, animation, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, -8.0f);
        this->action = SHEIK_PLAYING_HARP;
        this->drawMode = SHEIK_DRAW_HARP;
    }
}

void func_80B3DCA8(EnXc* this, PlayState* play) {
    f32 frameCount;

    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[4];

        if (cue != NULL && cue->id == 8) {
            frameCount = Animation_GetLastFrame(&gSheikInitialHarpAnim);
            Animation_Change(&this->skelAnime, &gSheikInitialHarpAnim, 0.0f, frameCount, frameCount, ANIMMODE_LOOP,
                             -8.0f);
            this->action = SHEIK_ACTION_10;
        }
    }
}

void EnXc_SetupHarpPutawayAction(EnXc* this, PlayState* play) {
    f32 curFrame;
    f32 animFrameCount;

    if (EnXc_CheckForCue(this, play, 5, 4)) {
        curFrame = this->skelAnime.curFrame;
        animFrameCount = this->skelAnime.endFrame;
        if (curFrame >= animFrameCount) {
            s32 pad;

            Animation_Change(&this->skelAnime, &gSheikInitialHarpAnim, -1.0f,
                             Animation_GetLastFrame(&gSheikInitialHarpAnim), 0.0f, ANIMMODE_ONCE, 0.0f);
            this->action = SHEIK_ACTION_PUT_HARP_AWAY;
        }
    } else if (EnXc_CheckForNoCue(this, play, 8, 4)) {
        EnXc_SetupPlayingHarpAction(this, play, true);
    }
}

void func_80B3DE00(EnXc* this, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&this->skelAnime, &gSheikPullingOutHarpAnim, -1.0f,
                         Animation_GetLastFrame(&gSheikPullingOutHarpAnim), 0.0f, ANIMMODE_ONCE, 0.0f);
        this->action = SHEIK_ACTION_12;
        this->drawMode = SHEIK_DRAW_PULLING_OUT_HARP;
    }
}

void func_80B3DE78(EnXc* this, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&this->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gSheikIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        this->action = SHEIK_ACTION_13;
        this->drawMode = SHEIK_DRAW_DEFAULT;
        this->timer = 0.0f;
    }
}

void EnXc_SetupReverseAccel(EnXc* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[4];

        if (cue != NULL && cue->id == 4) {
            Animation_Change(&this->skelAnime, &gSheikWalkingAnim, -1.0f, Animation_GetLastFrame(&gSheikWalkingAnim),
                             0.0f, ANIMMODE_LOOP, -12.0f);
            this->action = SHEIK_ACTION_REVERSE_ACCEL;
            this->actor.world.rot.y += 0x8000;
            this->timer = 0.0f;
        }
    }
}

void EnXc_SetupReverseWalkAction(EnXc* this) {
    this->timer++;
    if (this->timer >= 12.0f) {
        this->actor.speed = (kREG(2) * 0.01f) + 1.2f;
        this->action = SHEIK_ACTION_REVERSE_WALK;
    }
}

void EnXc_SetupReverseHaltAction(EnXc* this) {
    f32 xzDistToPlayer = this->actor.xzDistToPlayer;

    if (xzDistToPlayer >= kREG(5) + 140.0f) {
        Animation_Change(&this->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gSheikIdleAnim),
                         ANIMMODE_LOOP, -12.0f);
        this->action = SHEIK_ACTION_REVERSE_HALT;
        this->timer = 0.0f;
    }
}

void EnXc_SetupNutThrow(EnXc* this) {
    this->timer++;
    if (this->timer >= 12.0f) {
        Animation_Change(&this->skelAnime, &gSheikThrowingNutAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gSheikThrowingNutAnim), ANIMMODE_ONCE, 0.0f);
        this->action = SHEIK_ACTION_THROW_NUT;
        this->timer = 0.0f;
        this->actor.speed = 0.0f;
    }
}

void func_80B3E164(EnXc* this, PlayState* play) {
    this->timer++;
    if (this->timer >= 30.0f) {
        this->action = SHEIK_ACTION_DELETE;
        EnXc_SpawnNut(this, play);
    }
}

void EnXc_SetupDisappear(EnXc* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[4];

        if (cue != NULL && cue->id == 9) {
            s16 sceneId = play->sceneId;

            // Sheik fades away if end of Bolero CS, kill actor otherwise
            if (sceneId == SCENE_DEATH_MOUNTAIN_CRATER) {
                s32 pad;

                this->action = SHEIK_ACTION_FADE;
                this->drawMode = SHEIK_DRAW_NOTHING;
                this->actor.shape.shadowAlpha = 0;
            } else {
                Actor_Kill(&this->actor);
            }
        }
    }
}

void EnXc_ActionFunc0(EnXc* this, PlayState* play) {
    EnXc_SetColossusAppearSFX(this, play);
    EnXc_SetColossusWindSFX(play);
    func_80B3D750(this, play);
}

void EnXc_ActionFunc1(EnXc* this, PlayState* play) {
    EnXc_SetColossusAppearSFX(this, play);
    EnXc_SetColossusWindSFX(play);
    EnXc_SetupFallFromSkyAction(this, play);
}

void EnXc_GracefulFall(EnXc* this, PlayState* play) {
    s32 animFinished = EnXc_AnimIsFinished(this);

    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetLandingSFX(this, play);
    EnXc_SetColossusAppearSFX(this, play);
    EnXc_SetColossusWindSFX(play);
    func_80B3D8A4(this, play, animFinished);
}

void EnXc_Accelerate(EnXc* this, PlayState* play) {
    EnXc_CalcXZAccel(this);
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetWalkingSFX(this, play);
    EnXc_SetupWalkAction(this);
}

void EnXc_Walk(EnXc* this, PlayState* play) {
    func_80B3D644(this);
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetWalkingSFX(this, play);
    EnXc_SetupHaltAction(this);
}

void EnXc_Stopped(EnXc* this, PlayState* play) {
    EnXc_CalcXZSpeed(this);
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetWalkingSFX(this, play);
    EnXc_SetupStoppedAction(this);
}

void EnXc_ActionFunc6(EnXc* this, PlayState* play) {
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    func_80B3DAF0(this, play);
}

void EnXc_ActionFunc7(EnXc* this, PlayState* play) {
    s32 animFinished = EnXc_AnimIsFinished(this);

    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetupInitialHarpAction(this, animFinished);
}

void EnXc_ActionFunc8(EnXc* this, PlayState* play) {
    s32 animFinished = EnXc_AnimIsFinished(this);

    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetupPlayingHarpAction(this, play, animFinished);
}

void EnXc_ActionFunc9(EnXc* this, PlayState* play) {
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    func_80B3DCA8(this, play);
}

void EnXc_ActionFunc10(EnXc* this, PlayState* play) {
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetupHarpPutawayAction(this, play);
}

void EnXc_ActionFunc11(EnXc* this, PlayState* play) {
    s32 animFinished = EnXc_AnimIsFinished(this);

    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    func_80B3DE00(this, animFinished);
}

void EnXc_ActionFunc12(EnXc* this, PlayState* play) {
    s32 animFinished = EnXc_AnimIsFinished(this);

    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    func_80B3DE78(this, animFinished);
}

void EnXc_ActionFunc13(EnXc* this, PlayState* play) {
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_InitFlame(this, play);
    EnXc_SetupReverseAccel(this, play);
}

void EnXc_ReverseAccelerate(EnXc* this, PlayState* play) {
    func_80B3D6F0(this);
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetWalkingSFX(this, play);
    EnXc_InitFlame(this, play);
    EnXc_SetupReverseWalkAction(this);
}

void EnXc_ActionFunc15(EnXc* this, PlayState* play) {
    func_80B3D710(this);
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetWalkingSFX(this, play);
    EnXc_InitFlame(this, play);
    EnXc_SetupReverseHaltAction(this);
}

void EnXc_HaltAndWaitToThrowNut(EnXc* this, PlayState* play) {
    func_80B3D730(this);
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetWalkingSFX(this, play);
    EnXc_InitFlame(this, play);
    EnXc_SetupNutThrow(this);
}

void EnXc_ThrowNut(EnXc* this, PlayState* play) {
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetNutThrowSFX(this, play);
    EnXc_InitFlame(this, play);
    func_80B3E164(this, play);
}

void EnXc_Delete(EnXc* this, PlayState* play) {
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_InitFlame(this, play);
    EnXc_SetupDisappear(this, play);
}

void EnXc_Fade(EnXc* this, PlayState* play) {
    EnXc_InitFlame(this, play);
}

void func_80B3E87C(Gfx** dList, EnXc* this) {
    f32 currentFrame = this->skelAnime.curFrame;

    if (currentFrame >= 34.0f) {
        *dList = gSheikHarpDL;
    }
}

s32 EnXc_PullingOutHarpOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                        void* thisx) {
    EnXc* this = (EnXc*)thisx;

    if (limbIndex == 12) {
        func_80B3E87C(dList, this);
    }

    return 0;
}

s32 EnXc_HarpOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnXc* this = (EnXc*)thisx;

    if (limbIndex == 12) {
        *dList = gSheikHarpDL;
    }

    return 0;
}

void EnXc_DrawPullingOutHarp(Actor* thisx, PlayState* play) {
    EnXc* this = (EnXc*)thisx;
    s32 pad;
    s16 eyePattern = this->eyeIdx;
    void* eyeTexture = sEyeTextures[eyePattern];
    SkelAnime* skelAnime = &this->skelAnime;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad2;

    OPEN_DISPS(gfxCtx, "../z_en_oA2_inSpot05.c", 1444);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 20, 0);
    gDPSetEnvColor(POLY_OPA_DISP++, 60, 0, 0, 0);

    Gfx_SetupDL_25Opa(gfxCtx);
    func_8002EBCC(&this->actor, play, 0);
    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          EnXc_PullingOutHarpOverrideLimbDraw, NULL, this);
    CLOSE_DISPS(gfxCtx, "../z_en_oA2_inSpot05.c", 1497);
}

void EnXc_DrawHarp(Actor* thisx, PlayState* play) {
    EnXc* this = (EnXc*)thisx;
    s32 pad;
    s16 eyePattern = this->eyeIdx;
    void* eyeTexture = sEyeTextures[eyePattern];
    SkelAnime* skelAnime = &this->skelAnime;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad2;

    OPEN_DISPS(gfxCtx, "../z_en_oA2_inSpot05.c", 1511);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 20, 0);
    gDPSetEnvColor(POLY_OPA_DISP++, 60, 0, 0, 0);

    Gfx_SetupDL_25Opa(gfxCtx);
    func_8002EBCC(&this->actor, play, 0);
    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          EnXc_HarpOverrideLimbDraw, NULL, this);
    CLOSE_DISPS(gfxCtx, "../z_en_oA2_inSpot05.c", 1564);
}
