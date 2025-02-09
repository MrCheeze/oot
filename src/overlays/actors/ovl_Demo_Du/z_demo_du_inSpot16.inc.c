void DemoDu_InitCs_GoronsRuby(DemoDu* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gDaruniaSkel, NULL, NULL, NULL, 0);
    this->updateIndex = CS_GORONSRUBY_SUBSCENE(0);
}

// Cutscene: Darunia gives Link the Goron's Ruby.
// Sfx played when Darunia lands at the floor at the start of the cutscene.
void DemoDu_CsPlaySfx_GoronLanding(DemoDu* this) {
    Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_EN_GOLON_LAND_BIG);
}

// Cutscene: Darunia gives Link the Goron's Ruby.
// Sfx played when Darunia is falling at the start of the cutscene.
void DemoDu_CsPlaySfx_DaruniaFalling(PlayState* play) {
    if (play->csCtx.curFrame == 160) {
        Sfx_PlaySfxCentered2(NA_SE_EV_OBJECT_FALL);
    }
}

// Cutscene: Darunia gives Link the Goron's Ruby.
void DemoDu_CsPlaySfx_DaruniaHitsLink(PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;

    Sfx_PlaySfxAtPos(&player->actor.projectedPos, NA_SE_EN_DARUNIA_HIT_LINK);
    Audio_PlaySfxGeneral(NA_SE_VO_LI_DAMAGE_S_KID, &player->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                         &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

// Cutscene: Darunia gives Link the Goron's Ruby.
void DemoDu_CsPlaySfx_HitBreast(DemoDu* this) {
    Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_EN_DARUNIA_HIT_BREAST - SFX_FLAG);
}

// Cutscene: Darunia gives Link the Goron's Ruby.
// Sfx played when Link is escaping from the gorons at the end of the scene.
void DemoDu_CsPlaySfx_LinkEscapeFromGorons(PlayState* play) {
    if (play->csCtx.curFrame == 1400) {
        Player* player = GET_PLAYER(play);

        Audio_PlaySfxGeneral(NA_SE_VO_LI_FALL_L_KID, &player->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }
}

// Cutscene: Darunia gives Link the Goron's Ruby.
// Sfx played when Link is surprised by Darunia falling from the sky.
void DemoDu_CsPlaySfx_LinkSurprised(PlayState* play) {
    if (play->csCtx.curFrame == 174) {
        Player* player = GET_PLAYER(play);

        Audio_PlaySfxGeneral(NA_SE_VO_LI_SURPRISE_KID, &player->actor.projectedPos, 4U, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }
}

void DemoDu_CsGoronsRuby_UpdateFaceTextures(DemoDu* this, PlayState* play) {
    u16* frames = &play->csCtx.curFrame;

    if (*frames < 260) {
        DemoDu_UpdateEyes(this);
        DemoDu_SetMouthTexIndex(this, 0);
    } else if (*frames < 335) {
        DemoDu_UpdateEyes(this);
        DemoDu_SetMouthTexIndex(this, 3);
    } else if (*frames < 365) {
        DemoDu_SetEyeTexIndex(this, 3);
        DemoDu_SetMouthTexIndex(this, 1);
    } else if (*frames < 395) {
        DemoDu_SetEyeTexIndex(this, 0);
        DemoDu_SetMouthTexIndex(this, 3);
    } else if (*frames < 410) {
        DemoDu_UpdateEyes(this);
        DemoDu_SetMouthTexIndex(this, 0);
    } else {
        DemoDu_UpdateEyes(this);
        DemoDu_SetMouthTexIndex(this, 3);
    }
}

void func_8096A630(DemoDu* this, PlayState* play) {
    s32 pad;
    Vec3f pos = this->actor.world.pos;

    pos.y += kREG(5);
    func_80033480(play, &pos, kREG(1) + 100.0f, kREG(2) + 10, kREG(3) + 300, kREG(4), 0);
    DemoDu_CsPlaySfx_GoronLanding(this);
}

void DemoDu_CsGoronsRuby_SpawnDustWhenHittingLink(DemoDu* this, PlayState* play) {
    static Vec3f dustPosOffsets[] = {
        { 11.0f, -11.0f, -6.0f }, { 0.0f, 14.0f, -13.0f },  { 14.0f, -2.0f, -10.0f }, { 10.0f, -6.0f, -8.0f },
        { 8.0f, 6.0f, 8.0f },     { 13.0f, 8.0f, -10.0f },  { -14.0f, 1.0f, -14.0f }, { 5.0f, 12.0f, -9.0f },
        { 11.0f, 6.0f, -7.0f },   { 14.0f, 14.0f, -14.0f },
    };

    if (Animation_OnFrame(&this->skelAnime, 31.0f) || Animation_OnFrame(&this->skelAnime, 41.0f)) {
        s32 pad[2];
        s32 i;
        Player* player = GET_PLAYER(play);
        Vec3f* pos = &player->bodyPartsPos[PLAYER_BODYPART_L_FOREARM];
        Vec3f velocity = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 0.3f, 0.0f };
        s32 pad2;

        for (i = 4; i >= 0; --i) {
            Color_RGBA8 primColor = { 190, 150, 110, 255 };
            Color_RGBA8 envColor = { 120, 80, 40, 255 };
            s32 colorDelta;
            Vec3f position;

            if (Animation_OnFrame(&this->skelAnime, 31.0f)) {
                position.x = dustPosOffsets[i + 5].x + pos->x;
                position.y = dustPosOffsets[i + 5].y + pos->y;
                position.z = dustPosOffsets[i + 5].z + pos->z;
            } else {
                position.x = dustPosOffsets[i + 0].x + pos->x;
                position.y = dustPosOffsets[i + 0].y + pos->y;
                position.z = dustPosOffsets[i + 0].z + pos->z;
            }

            colorDelta = Rand_ZeroOne() * 20.0f - 10.0f;

            primColor.r += colorDelta;
            primColor.g += colorDelta;
            primColor.b += colorDelta;
            envColor.r += colorDelta;
            envColor.g += colorDelta;
            envColor.b += colorDelta;

            func_8002829C(play, &position, &velocity, &accel, &primColor, &envColor, Rand_ZeroOne() * 40.0f + 200.0f,
                          0);
        }

        DemoDu_CsPlaySfx_DaruniaHitsLink(play);
    }
}

void DemoDu_CsGoronsRuby_DaruniaFalling(DemoDu* this, PlayState* play) {
    s32 pad;
    CutsceneContext* csCtx = &play->csCtx;

    if (csCtx->state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = csCtx->actorCues[2];
        Vec3f startPos;
        Vec3f endPos;
        Vec3f* pos = &this->actor.world.pos;

        if (cue != NULL) {
            f32 traveledPercent = Environment_LerpWeight(cue->endFrame, cue->startFrame, csCtx->curFrame);

            startPos.x = cue->startPos.x;
            startPos.y = cue->startPos.y;
            startPos.z = cue->startPos.z;

            endPos.x = cue->endPos.x;
            endPos.y = cue->endPos.y;
            endPos.z = cue->endPos.z;

            pos->x = LERP(startPos.x, endPos.x, traveledPercent);
            pos->y = LERP(startPos.y, endPos.y, traveledPercent);
            pos->z = LERP(startPos.z, endPos.z, traveledPercent);
        }
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo01(DemoDu* this, PlayState* play) {
    this->updateIndex = CS_GORONSRUBY_SUBSCENE(1);
}

void DemoDu_CsGoronsRuby_AdvanceTo02(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if ((cue != NULL) && (cue->id != 1)) {
            Animation_Change(&this->skelAnime, &gDaruniaStandUpAfterFallingAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gDaruniaStandUpAfterFallingAnim), 2, 0.0f);
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(2);
            this->drawIndex = 1;
            DemoDu_CsGoronsRuby_DaruniaFalling(this, play);
        }
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo03(DemoDu* this, PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;

    if (csCtx->state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = csCtx->actorCues[2];

        if ((cue != NULL) && (csCtx->curFrame >= cue->endFrame)) {
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(3);
            func_8096A630(this, play);
        }
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo04(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if ((cue != NULL) && (cue->id != 2)) {
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(4);
        }
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo05(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&this->skelAnime, &gDaruniaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gDaruniaIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        this->updateIndex = CS_GORONSRUBY_SUBSCENE(5);
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo06(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if ((cue != NULL) && (cue->id != 3)) {
            Animation_Change(&this->skelAnime, &gDaruniaHitBreastAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gDaruniaHitBreastAnim), 2, -4.0f);
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(6);
        }
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo07(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&this->skelAnime, &gDaruniaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gDaruniaIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        this->updateIndex = CS_GORONSRUBY_SUBSCENE(7);
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo08(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if ((cue != NULL) && (cue->id != 4)) {
            Animation_Change(&this->skelAnime, &gDaruniaHitLinkAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gDaruniaHitLinkAnim), 2, 0.0f);
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(8);
        }
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo09(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&this->skelAnime, &gDaruniaHitBreastAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gDaruniaHitBreastAnim), 2, 0.0f);
        this->updateIndex = CS_GORONSRUBY_SUBSCENE(9);
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo10(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&this->skelAnime, &gDaruniaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gDaruniaIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        this->updateIndex = CS_GORONSRUBY_SUBSCENE(10);
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo11(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if ((cue != NULL) && (cue->id != 5)) {
            Animation_Change(&this->skelAnime, &gDaruniaItemGiveAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gDaruniaItemGiveAnim), 2, 0.0f);
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(11);
        }
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo12(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&this->skelAnime, &gDaruniaItemGiveIdleAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gDaruniaItemGiveIdleAnim), 0, 0.0f);
        this->updateIndex = CS_GORONSRUBY_SUBSCENE(12);
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo13(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if ((cue != NULL) && (cue->id != 6)) {
            Animation_Change(&this->skelAnime, &gDaruniaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gDaruniaIdleAnim),
                             ANIMMODE_LOOP, 0.0f);
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(13);
        }
    }
}

void DemoDu_UpdateCs_GR_00(DemoDu* this, PlayState* play) {
    DemoDu_CsPlaySfx_DaruniaFalling(play);
    DemoDu_CsGoronsRuby_AdvanceTo01(this, play);
}

void DemoDu_UpdateCs_GR_01(DemoDu* this, PlayState* play) {
    DemoDu_CsPlaySfx_DaruniaFalling(play);
    DemoDu_CsPlaySfx_LinkSurprised(play);
    DemoDu_CsGoronsRuby_AdvanceTo02(this, play);
}

void DemoDu_UpdateCs_GR_02(DemoDu* this, PlayState* play) {
    DemoDu_CsGoronsRuby_DaruniaFalling(this, play);
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_CsPlaySfx_DaruniaFalling(play);
    DemoDu_CsPlaySfx_LinkSurprised(play);
    DemoDu_CsGoronsRuby_AdvanceTo03(this, play);
}

void DemoDu_UpdateCs_GR_03(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_CsPlaySfx_LinkSurprised(play);
    DemoDu_CsGoronsRuby_AdvanceTo04(this, play);
}

void DemoDu_UpdateCs_GR_04(DemoDu* this, PlayState* play) {
    s32 animFinished;

    DemoDu_UpdateBgCheckInfo(this, play);
    animFinished = DemoDu_UpdateSkelAnime(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo05(this, animFinished);
}

void DemoDu_UpdateCs_GR_05(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo06(this, play);
}

void DemoDu_UpdateCs_GR_06(DemoDu* this, PlayState* play) {
    s32 animFinished;

    DemoDu_UpdateBgCheckInfo(this, play);
    animFinished = DemoDu_UpdateSkelAnime(this);
    DemoDu_CsPlaySfx_HitBreast(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo07(this, animFinished);
}

void DemoDu_UpdateCs_GR_07(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo08(this, play);
}

void DemoDu_UpdateCs_GR_08(DemoDu* this, PlayState* play) {
    s32 animFinished;

    DemoDu_UpdateBgCheckInfo(this, play);
    animFinished = DemoDu_UpdateSkelAnime(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_SpawnDustWhenHittingLink(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo09(this, animFinished);
}

void DemoDu_UpdateCs_GR_09(DemoDu* this, PlayState* play) {
    s32 animFinished;

    DemoDu_UpdateBgCheckInfo(this, play);
    animFinished = DemoDu_UpdateSkelAnime(this);
    DemoDu_CsPlaySfx_HitBreast(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo10(this, animFinished);
}

void DemoDu_UpdateCs_GR_10(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo11(this, play);
}

void DemoDu_UpdateCs_GR_11(DemoDu* this, PlayState* play) {
    s32 animFinished;

    DemoDu_UpdateBgCheckInfo(this, play);
    animFinished = DemoDu_UpdateSkelAnime(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo12(this, animFinished);
}

void DemoDu_UpdateCs_GR_12(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo13(this, play);
}

void DemoDu_UpdateCs_GR_13(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsPlaySfx_LinkEscapeFromGorons(play);
}
