void func_80B40590(EnXc* this, PlayState* play) {
    this->action = SHEIK_ACTION_NOCTURNE_INIT;
    this->drawMode = SHEIK_DRAW_SQUINT;
}

void EnXc_SetThrownAroundSFX(EnXc* this) {
    SkelAnime* skelAnime = &this->skelAnime;

    if (Animation_OnFrame(skelAnime, 9.0f)) {
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_PL_BOUND + SURFACE_SFX_OFFSET_GRASS);
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_VO_SK_CRASH);
    } else if (Animation_OnFrame(skelAnime, 26.0f)) {
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_PL_BOUND + SURFACE_SFX_OFFSET_GRASS);
    } else if (Animation_OnFrame(skelAnime, 28.0f)) {
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_GRASS);
    } else if (Animation_OnFrame(skelAnime, 34.0f)) {
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_GRASS);
    }
}

void EnXc_PlayLinkScreamSFX(EnXc* this, PlayState* play) {
    if (play->csCtx.curFrame == 1455) {
        Audio_PlayCutsceneEffectsSequence(SEQ_CS_EFFECTS_BONGO_HURL_LINK);
    }
}

void EnXc_SetCrySFX(EnXc* this, PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;

    if (csCtx->curFrame == 869) {
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_VO_SK_CRY_0);
    } else if (csCtx->curFrame == 939) {
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_VO_SK_CRY_1);
    }
}

void func_80B406F8(Actor* thisx) {
    EnXc* this = (EnXc*)thisx;

    this->action = SHEIK_ACTION_NOCTURNE_INIT;
    this->drawMode = SHEIK_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

void EnXc_SetupIdleInNocturne(EnXc* this, PlayState* play) {
    s32 pad;
    ActorShape* actorShape = &this->actor.shape;
    SkelAnime* skelAnime = &this->skelAnime;
    f32 frameCount = Animation_GetLastFrame(&gSheikIdleAnim);

    func_80B3C9DC(this);
    func_80B3C588(this, play, 4);
    Animation_Change(skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    this->action = SHEIK_ACTION_NOCTURNE_IDLE;
    this->drawMode = SHEIK_DRAW_SQUINT;
    actorShape->shadowAlpha = 255;
}

void EnXc_SetupDefenseStance(Actor* thisx) {
    EnXc* this = (EnXc*)thisx;
    SkelAnime* skelAnime = &this->skelAnime;
    f32 frameCount = Animation_GetLastFrame(&gSheikDefenseStanceAnim);

    Animation_Change(skelAnime, &gSheikDefenseStanceAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, -8.0f);
    this->action = SHEIK_ACTION_DEFENSE_STANCE;
    this->drawMode = SHEIK_DRAW_DEFAULT;
}

void EnXc_SetupContortions(EnXc* this, PlayState* play) {
    s32 pad[2];
    SkelAnime* skelAnime = &this->skelAnime;

#if DEBUG_FEATURES
    Animation_Change(skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gSheikIdleAnim), ANIMMODE_LOOP,
                     0.0f);
#endif
    func_80B3C588(this, play, 4);
    func_80B3C964(this, play);
    Animation_Change(skelAnime, &gSheikContortionsAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gSheikContortionsAnim),
                     ANIMMODE_ONCE, 0.0f);
    this->action = SHEIK_ACTION_CONTORT;
    this->drawMode = SHEIK_DRAW_DEFAULT;
    this->actor.shape.shadowAlpha = 255;
}

void EnXc_SetupFallInNocturne(EnXc* this, PlayState* play) {
    s32 pad;
    SkelAnime* skelAnime = &this->skelAnime;
    f32 frameCount = Animation_GetLastFrame(&gSheikIdleAnim);

    Animation_Change(skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    func_80B3C588(this, play, 4);
    func_80B3C964(this, play);
    Animation_Change(skelAnime, &gSheikFallingFromContortionsAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gSheikFallingFromContortionsAnim), ANIMMODE_ONCE, 0.0f);
    this->action = SHEIK_ACTION_NOCTURNE_FALL;
    this->drawMode = SHEIK_DRAW_DEFAULT;
    this->actor.shape.shadowAlpha = 255;
}

void EnXc_SetupHittingGroundInNocturne(EnXc* this, PlayState* play) {
    s32 pad[3];
    f32 frameCount = Animation_GetLastFrame(&gSheikHittingGroundAnim);

    func_80B3C9DC(this);
    func_80B3C588(this, play, 4);
    Animation_Change(&this->skelAnime, &gSheikHittingGroundAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
    this->action = SHEIK_ACTION_NOCTURNE_HIT_GROUND;
    this->drawMode = SHEIK_DRAW_DEFAULT;
    this->actor.shape.shadowAlpha = 255;
}

void func_80B40A78(EnXc* this, PlayState* play) {
    s32 pad[3];
    f32 frameCount = Animation_GetLastFrame(&gSheikHittingGroundAnim);

    func_80B3C9DC(this);
    func_80B3C588(this, play, 4);
    Animation_Change(&this->skelAnime, &gSheikHittingGroundAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
    this->action = SHEIK_ACTION_63;
    this->drawMode = SHEIK_DRAW_DEFAULT;
    this->actor.shape.shadowAlpha = 255;
}

void EnXc_SetupKneelInNocturne(EnXc* this, PlayState* play) {
    s32 pad[3];
    f32 frameCount = Animation_GetLastFrame(&gSheikKneelingAnim);

    func_80B3C9DC(this);
    func_80B3C588(this, play, 4);
    Animation_Change(&this->skelAnime, &gSheikKneelingAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    this->action = SHEIK_ACTION_NOCTURNE_KNEEL;
    this->drawMode = SHEIK_DRAW_DEFAULT;
    this->actor.shape.shadowAlpha = 255;
}

void func_80B40BB4(EnXc* this, PlayState* play) {
    s32 pad[3];
    f32 frameCount = Animation_GetLastFrame(&gSheikIdleAnim);
    func_80B3C9DC(this);
    func_80B3C588(this, play, 4);
    Animation_Change(&this->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    this->action = SHEIK_ACTION_65;
    this->drawMode = SHEIK_DRAW_DEFAULT;
    this->actor.shape.shadowAlpha = 255;
}

void func_80B40C50(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_65, SHEIK_ACTION_66);
}

void func_80B40C74(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_66, SHEIK_ACTION_67);
}

void func_80B40C98(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_67, SHEIK_ACTION_68);
}

void func_80B40CBC(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_68, SHEIK_ACTION_69);
}

void func_80B40CE0(EnXc* this) {
    func_80B3C7D4(this, SHEIK_ACTION_69, SHEIK_ACTION_70, SHEIK_ACTION_68);
}

void func_80B40D08(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_70, SHEIK_ACTION_71);
}

void func_80B40D2C(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_71, SHEIK_ACTION_72);
}

void func_80B40D50(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_72, SHEIK_ACTION_NOCTURNE_REVERSE_ACCEL);
}

void func_80B40D74(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_NOCTURNE_REVERSE_ACCEL, SHEIK_ACTION_NOCTURNE_REVERSE_WALK);
}

void EnXc_SetupReverseHaltInNocturneCS(EnXc* this) {
    f32 xzDistToPlayer = this->actor.xzDistToPlayer;

    if (kREG(5) + 140.0f <= xzDistToPlayer) {
        Animation_Change(&this->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gSheikIdleAnim),
                         ANIMMODE_LOOP, -12.0f);
        this->action = SHEIK_ACTION_NOCTURNE_REVERSE_HALT;
        this->timer = 0.0f;
    }
}

void func_80B40E40(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_NOCTURNE_REVERSE_HALT, SHEIK_ACTION_NOCTURNE_THROW_NUT);
}

void func_80B40E64(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_NOCTURNE_THROW_NUT, SHEIK_ACTION_77);
}

void func_80B40E88(EnXc* this) {
    EnXc_CheckAndSetAction(this, SHEIK_ACTION_77, SHEIK_ACTION_78);
}

s32 EnXc_SetupNocturneState(Actor* thisx, PlayState* play) {
    CsCmdActorCue* cue = EnXc_GetCue(play, 4);

    if (cue != NULL) {
        s32 nextCueId = cue->id;
        EnXc* this = (EnXc*)thisx;
        s32 currentCueId = this->unk_26C;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    func_80B406F8(thisx);
                    break;
                case 6:
                    EnXc_SetupIdleInNocturne(this, play);
                    break;
                case 20:
                    EnXc_SetupDefenseStance(thisx);
                    break;
                case 18:
                    EnXc_SetupContortions(this, play);
                    break;
                case 14:
                    EnXc_SetupFallInNocturne(this, play);
                    break;
                case 19:
                    EnXc_SetupHittingGroundInNocturne(this, play);
                    break;
                case 15:
                    func_80B40A78(this, play);
                    break;
                case 16:
                    EnXc_SetupKneelInNocturne(this, play);
                    break;
                case 17:
                    func_80B40BB4(this, play);
                    break;
                case 9:
                    Actor_Kill(thisx);
                    break;
                default:
                    PRINTF("En_Oa2_Stalker_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }

            this->unk_26C = nextCueId;
            return 1;
        }
    }
    return 0;
}

void EnXc_InitialNocturneAction(EnXc* this, PlayState* play) {
    EnXc_SetupNocturneState(&this->actor, play);
}

void EnXc_IdleInNocturne(EnXc* this, PlayState* play) {
    func_80B3C588(this, play, 4);
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetupNocturneState(&this->actor, play);
}

void EnXc_DefenseStance(EnXc* this, PlayState* play) {
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetupNocturneState(&this->actor, play);
}

void EnXc_Contort(EnXc* this, PlayState* play) {
    EnXc_SetCrySFX(this, play);
    EnXc_AnimIsFinished(this);
    EnXc_SetEyePattern(this);
    if (!EnXc_SetupNocturneState(&this->actor, play)) {
        func_80B3C924(this, play);
        EnXc_BgCheck(this, play);
    }
}

void EnXc_FallInNocturne(EnXc* this, PlayState* play) {
    EnXc_AnimIsFinished(this);
    EnXc_SetEyePattern(this);
    EnXc_SetThrownAroundSFX(this);
    if (!EnXc_SetupNocturneState(&this->actor, play)) {
        func_80B3C8CC(this, play);
        EnXc_BgCheck(this, play);
    }
}

void EnXc_HitGroundInNocturne(EnXc* this, PlayState* play) {
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetupNocturneState(&this->actor, play);
}

void EnXc_ActionFunc63(EnXc* this, PlayState* play) {
    EnXc_AnimIsFinished(this);
    EnXc_PlayLinkScreamSFX(this, play);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetupNocturneState(&this->actor, play);
}

void EnXc_KneelInNocturneCS(EnXc* this, PlayState* play) {
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetupNocturneState(&this->actor, play);
}

void EnXc_ActionFunc65(EnXc* this, PlayState* play) {
    EnXc_ActionFunc6(this, play);
    func_80B3C588(this, play, 4);
    func_80B40C50(this);
}

void EnXc_ActionFunc66(EnXc* this, PlayState* play) {
    EnXc_ActionFunc7(this, play);
    func_80B40C74(this);
}

void EnXc_ActionFunc67(EnXc* this, PlayState* play) {
    EnXc_ActionFunc8(this, play);
    func_80B40C98(this);
}

void EnXc_ActionFunc68(EnXc* this, PlayState* play) {
    EnXc_ActionFunc9(this, play);
    func_80B40CBC(this);
}

void EnXc_ActionFunc69(EnXc* this, PlayState* play) {
    EnXc_ActionFunc10(this, play);
    func_80B40CE0(this);
}

void EnXc_ActionFunc70(EnXc* this, PlayState* play) {
    EnXc_ActionFunc11(this, play);
    func_80B40D08(this);
}

void EnXc_ActionFunc71(EnXc* this, PlayState* play) {
    EnXc_ActionFunc12(this, play);
    func_80B40D2C(this);
}

void EnXc_ActionFunc72(EnXc* this, PlayState* play) {
    EnXc_ActionFunc13(this, play);
    func_80B40D50(this);
}

void EnXc_ReverseAccelInNocturneCS(EnXc* this, PlayState* play) {
    EnXc_ReverseAccelerate(this, play);
    func_80B40D74(this);
}

void EnXc_ReverseWalkInNocturneCS(EnXc* this, PlayState* play) {
    func_80B3D710(this);
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetupReverseHaltInNocturneCS(this);
}

void EnXc_ReverseHaltInNocturneCS(EnXc* this, PlayState* play) {
    EnXc_HaltAndWaitToThrowNut(this, play);
    func_80B40E40(this);
}

void EnXc_ThrowNutInNocturneCS(EnXc* this, PlayState* play) {
    EnXc_ThrowNut(this, play);
    func_80B40E64(this);
}

void EnXc_DeleteInNocturneCS(EnXc* this, PlayState* play) {
    EnXc_Delete(this, play);
    func_80B40E88(this);
}

void EnXc_KillInNocturneCS(EnXc* this, PlayState* play) {
    Actor_Kill(&this->actor);
}

void EnXc_DrawSquintingEyes(Actor* thisx, PlayState* play) {
    EnXc* this = (EnXc*)thisx;
    SkelAnime* skelAnime = &this->skelAnime;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_en_oA2_inStalker.c", 839);
    Gfx_SetupDL_25Opa(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gSheikEyeSquintingTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gSheikEyeSquintingTex));
    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL, NULL, NULL);
    CLOSE_DISPS(gfxCtx, "../z_en_oA2_inStalker.c", 854);
}
