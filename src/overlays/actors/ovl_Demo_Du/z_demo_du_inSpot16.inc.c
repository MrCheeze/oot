void Demo_Du_Actor_Cryst_Init(DemoDu* this, PlayState* play) {
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gDaruniaSkel, NULL, NULL, NULL, 0);
    this->updateIndex = CS_GORONSRUBY_SUBSCENE(0);
}

// Cutscene: Darunia gives Link the Goron's Ruby.
// Sfx played when Darunia lands at the floor at the start of the cutscene.
void Demo_Du_Cryst_Set_LandSound(DemoDu* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_EN_GOLON_LAND_BIG);
}

// Cutscene: Darunia gives Link the Goron's Ruby.
// Sfx played when Darunia is falling at the start of the cutscene.
void Demo_Du_Cryst_Set_FallSound(PlayState* play) {
    if (play->csCtx.curFrame == 160) {
        Na_StartFixSe_F(NA_SE_EV_OBJECT_FALL);
    }
}

// Cutscene: Darunia gives Link the Goron's Ruby.
void Demo_Du_Cryst_Set_HitlinkSound(PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;

    Na_StartObjectSe_F(&player->actor.projectedPos, NA_SE_EN_DARUNIA_HIT_LINK);
    Nai_FxFlagEntry(NA_SE_VO_LI_DAMAGE_S_KID, &player->actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
}

// Cutscene: Darunia gives Link the Goron's Ruby.
void Demo_Du_Cryst_Set_HitbreastSound(DemoDu* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_EN_DARUNIA_HIT_BREAST - SFX_FLAG);
}

// Cutscene: Darunia gives Link the Goron's Ruby.
// Sfx played when Link is escaping from the gorons at the end of the scene.
void Demo_Du_Cryst_Set_CrylinkSound(PlayState* play) {
    if (play->csCtx.curFrame == 1400) {
        Player* player = GET_PLAYER(play);

        Nai_FxFlagEntry(NA_SE_VO_LI_FALL_L_KID, &player->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
}

// Cutscene: Darunia gives Link the Goron's Ruby.
// Sfx played when Link is surprised by Darunia falling from the sky.
void Demo_Du_Cryst_Set_SurpriselinkSound(PlayState* play) {
    if (play->csCtx.curFrame == 174) {
        Player* player = GET_PLAYER(play);

        Nai_FxFlagEntry(NA_SE_VO_LI_SURPRISE_KID, &player->actor.projectedPos, 4U, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
}

void Demo_Du_Cryst_SetFace(DemoDu* this, PlayState* play) {
    u16* frames = &play->csCtx.curFrame;

    if (*frames < 260) {
        Demo_Du_set_eye_pattern(this);
        Demo_Du_set_mouth_Num(this, 0);
    } else if (*frames < 335) {
        Demo_Du_set_eye_pattern(this);
        Demo_Du_set_mouth_Num(this, 3);
    } else if (*frames < 365) {
        Demo_Du_set_eye_Num(this, 3);
        Demo_Du_set_mouth_Num(this, 1);
    } else if (*frames < 395) {
        Demo_Du_set_eye_Num(this, 0);
        Demo_Du_set_mouth_Num(this, 3);
    } else if (*frames < 410) {
        Demo_Du_set_eye_pattern(this);
        Demo_Du_set_mouth_Num(this, 0);
    } else {
        Demo_Du_set_eye_pattern(this);
        Demo_Du_set_mouth_Num(this, 3);
    }
}

void Birth_Dust_In_Demo_Du_Cryst_Land(DemoDu* this, PlayState* play) {
    s32 pad;
    Vec3f pos = this->actor.world.pos;

    pos.y += kREG(5);
    dust_fly_set2(play, &pos, kREG(1) + 100.0f, kREG(2) + 10, kREG(3) + 300, kREG(4), 0);
    Demo_Du_Cryst_Set_LandSound(this);
}

void Birth_Dust_In_Demo_Du_Cryst(DemoDu* this, PlayState* play) {
    static Vec3f array_offset[] = {
        { 11.0f, -11.0f, -6.0f }, { 0.0f, 14.0f, -13.0f },  { 14.0f, -2.0f, -10.0f }, { 10.0f, -6.0f, -8.0f },
        { 8.0f, 6.0f, 8.0f },     { 13.0f, 8.0f, -10.0f },  { -14.0f, 1.0f, -14.0f }, { 5.0f, 12.0f, -9.0f },
        { 11.0f, 6.0f, -7.0f },   { 14.0f, 14.0f, -14.0f },
    };

    if (Skeleton_Info_frame_check(&this->skelAnime, 31.0f) || Skeleton_Info_frame_check(&this->skelAnime, 41.0f)) {
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

            if (Skeleton_Info_frame_check(&this->skelAnime, 31.0f)) {
                position.x = array_offset[i + 5].x + pos->x;
                position.y = array_offset[i + 5].y + pos->y;
                position.z = array_offset[i + 5].z + pos->z;
            } else {
                position.x = array_offset[i + 0].x + pos->x;
                position.y = array_offset[i + 0].y + pos->y;
                position.z = array_offset[i + 0].z + pos->z;
            }

            colorDelta = fqrand() * 20.0f - 10.0f;

            primColor.r += colorDelta;
            primColor.g += colorDelta;
            primColor.b += colorDelta;
            envColor.r += colorDelta;
            envColor.g += colorDelta;
            envColor.b += colorDelta;

            Effect_SS_Dust_sc_cl_ct(play, &position, &velocity, &accel, &primColor, &envColor, fqrand() * 40.0f + 200.0f,
                          0);
        }

        Demo_Du_Cryst_Set_HitlinkSound(play);
    }
}

void Demo_Du_Cryst_Movement_Down(DemoDu* this, PlayState* play) {
    s32 pad;
    CutsceneContext* csCtx = &play->csCtx;

    if (csCtx->state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = csCtx->actorCues[2];
        Vec3f startPos;
        Vec3f endPos;
        Vec3f* pos = &this->actor.world.pos;

        if (cue != NULL) {
            f32 traveledPercent = get_parcent(cue->endFrame, cue->startFrame, csCtx->curFrame);

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

void Demo_Du_Cryst_Check_WaitToHide(DemoDu* this, PlayState* play) {
    this->updateIndex = CS_GORONSRUBY_SUBSCENE(1);
}

void Demo_Du_Cryst_Check_HideToDown(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if ((cue != NULL) && (cue->id != 1)) {
            Skeleton_Info2_init(&this->skelAnime, &gDaruniaStandUpAfterFallingAnim, 1.0f, 0.0f,
                             Si2_anime_end_frame(&gDaruniaStandUpAfterFallingAnim), 2, 0.0f);
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(2);
            this->drawIndex = 1;
            Demo_Du_Cryst_Movement_Down(this, play);
        }
    }
}

void Demo_Du_Cryst_Check_DownToLand(DemoDu* this, PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;

    if (csCtx->state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = csCtx->actorCues[2];

        if ((cue != NULL) && (csCtx->curFrame >= cue->endFrame)) {
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(3);
            Birth_Dust_In_Demo_Du_Cryst_Land(this, play);
        }
    }
}

void Demo_Du_Cryst_Check_LandToStandup(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if ((cue != NULL) && (cue->id != 2)) {
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(4);
        }
    }
}

void Demo_Du_Cryst_Check_StandupToGreet(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Skeleton_Info2_init(&this->skelAnime, &gDaruniaIdleAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gDaruniaIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        this->updateIndex = CS_GORONSRUBY_SUBSCENE(5);
    }
}

void Demo_Du_Cryst_Check_GreetToImpress(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if ((cue != NULL) && (cue->id != 3)) {
            Skeleton_Info2_init(&this->skelAnime, &gDaruniaHitBreastAnim, 1.0f, 0.0f,
                             Si2_anime_end_frame(&gDaruniaHitBreastAnim), 2, -4.0f);
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(6);
        }
    }
}

void Demo_Du_Cryst_Check_ImpressToThanks(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Skeleton_Info2_init(&this->skelAnime, &gDaruniaIdleAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gDaruniaIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        this->updateIndex = CS_GORONSRUBY_SUBSCENE(7);
    }
}

void Demo_Du_Cryst_Check_ThanksToHit(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if ((cue != NULL) && (cue->id != 4)) {
            Skeleton_Info2_init(&this->skelAnime, &gDaruniaHitLinkAnim, 1.0f, 0.0f,
                             Si2_anime_end_frame(&gDaruniaHitLinkAnim), 2, 0.0f);
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(8);
        }
    }
}

void Demo_Du_Cryst_Check_HitToImpress2(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Skeleton_Info2_init(&this->skelAnime, &gDaruniaHitBreastAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gDaruniaHitBreastAnim), 2, 0.0f);
        this->updateIndex = CS_GORONSRUBY_SUBSCENE(9);
    }
}

void Demo_Du_Cryst_Check_Impress2ToReady(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Skeleton_Info2_init(&this->skelAnime, &gDaruniaIdleAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gDaruniaIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        this->updateIndex = CS_GORONSRUBY_SUBSCENE(10);
    }
}

void Demo_Du_Cryst_Check_ReadyToHandup(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if ((cue != NULL) && (cue->id != 5)) {
            Skeleton_Info2_init(&this->skelAnime, &gDaruniaItemGiveAnim, 1.0f, 0.0f,
                             Si2_anime_end_frame(&gDaruniaItemGiveAnim), 2, 0.0f);
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(11);
        }
    }
}

void Demo_Du_Cryst_Check_HandupToRecall(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Skeleton_Info2_init(&this->skelAnime, &gDaruniaItemGiveIdleAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gDaruniaItemGiveIdleAnim), 0, 0.0f);
        this->updateIndex = CS_GORONSRUBY_SUBSCENE(12);
    }
}

void Demo_Du_Cryst_Check_RecallToStop(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if ((cue != NULL) && (cue->id != 6)) {
            Skeleton_Info2_init(&this->skelAnime, &gDaruniaIdleAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gDaruniaIdleAnim),
                             ANIMMODE_LOOP, 0.0f);
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(13);
        }
    }
}

void Demo_Du_Actor_main_cryst_wait(DemoDu* this, PlayState* play) {
    Demo_Du_Cryst_Set_FallSound(play);
    Demo_Du_Cryst_Check_WaitToHide(this, play);
}

void Demo_Du_Actor_main_cryst_hide(DemoDu* this, PlayState* play) {
    Demo_Du_Cryst_Set_FallSound(play);
    Demo_Du_Cryst_Set_SurpriselinkSound(play);
    Demo_Du_Cryst_Check_HideToDown(this, play);
}

void Demo_Du_Actor_main_cryst_down(DemoDu* this, PlayState* play) {
    Demo_Du_Cryst_Movement_Down(this, play);
    Demo_Du_BGcheck(this, play);
    Demo_Du_Cryst_Set_FallSound(play);
    Demo_Du_Cryst_Set_SurpriselinkSound(play);
    Demo_Du_Cryst_Check_DownToLand(this, play);
}

void Demo_Du_Actor_main_cryst_land(DemoDu* this, PlayState* play) {
    Demo_Du_BGcheck(this, play);
    Demo_Du_Cryst_Set_SurpriselinkSound(play);
    Demo_Du_Cryst_Check_LandToStandup(this, play);
}

void Demo_Du_Actor_main_cryst_standup(DemoDu* this, PlayState* play) {
    s32 animFinished;

    Demo_Du_BGcheck(this, play);
    animFinished = Demo_Du_Animation_Base(this);
    Demo_Du_Cryst_SetFace(this, play);
    Demo_Du_Cryst_Check_StandupToGreet(this, animFinished);
}

void Demo_Du_Actor_main_cryst_greet(DemoDu* this, PlayState* play) {
    Demo_Du_BGcheck(this, play);
    Demo_Du_Animation_Base(this);
    Demo_Du_Cryst_SetFace(this, play);
    Demo_Du_Cryst_Check_GreetToImpress(this, play);
}

void Demo_Du_Actor_main_cryst_impress(DemoDu* this, PlayState* play) {
    s32 animFinished;

    Demo_Du_BGcheck(this, play);
    animFinished = Demo_Du_Animation_Base(this);
    Demo_Du_Cryst_Set_HitbreastSound(this);
    Demo_Du_Cryst_SetFace(this, play);
    Demo_Du_Cryst_Check_ImpressToThanks(this, animFinished);
}

void Demo_Du_Actor_main_cryst_thanks(DemoDu* this, PlayState* play) {
    Demo_Du_BGcheck(this, play);
    Demo_Du_Animation_Base(this);
    Demo_Du_Cryst_SetFace(this, play);
    Demo_Du_Cryst_Check_ThanksToHit(this, play);
}

void Demo_Du_Actor_main_cryst_hit(DemoDu* this, PlayState* play) {
    s32 animFinished;

    Demo_Du_BGcheck(this, play);
    animFinished = Demo_Du_Animation_Base(this);
    Demo_Du_Cryst_SetFace(this, play);
    Birth_Dust_In_Demo_Du_Cryst(this, play);
    Demo_Du_Cryst_Check_HitToImpress2(this, animFinished);
}

void Demo_Du_Actor_main_cryst_impress2(DemoDu* this, PlayState* play) {
    s32 animFinished;

    Demo_Du_BGcheck(this, play);
    animFinished = Demo_Du_Animation_Base(this);
    Demo_Du_Cryst_Set_HitbreastSound(this);
    Demo_Du_Cryst_SetFace(this, play);
    Demo_Du_Cryst_Check_Impress2ToReady(this, animFinished);
}

void Demo_Du_Actor_main_cryst_ready(DemoDu* this, PlayState* play) {
    Demo_Du_BGcheck(this, play);
    Demo_Du_Animation_Base(this);
    Demo_Du_Cryst_SetFace(this, play);
    Demo_Du_Cryst_Check_ReadyToHandup(this, play);
}

void Demo_Du_Actor_main_cryst_handup(DemoDu* this, PlayState* play) {
    s32 animFinished;

    Demo_Du_BGcheck(this, play);
    animFinished = Demo_Du_Animation_Base(this);
    Demo_Du_Cryst_SetFace(this, play);
    Demo_Du_Cryst_Check_HandupToRecall(this, animFinished);
}

void Demo_Du_Actor_main_cryst_recall(DemoDu* this, PlayState* play) {
    Demo_Du_BGcheck(this, play);
    Demo_Du_Animation_Base(this);
    Demo_Du_Cryst_SetFace(this, play);
    Demo_Du_Cryst_Check_RecallToStop(this, play);
}

void Demo_Du_Actor_main_cryst_stop(DemoDu* this, PlayState* play) {
    Demo_Du_BGcheck(this, play);
    Demo_Du_Animation_Base(this);
    Demo_Du_Cryst_SetFace(this, play);
    Demo_Du_Cryst_Set_CrylinkSound(play);
}
