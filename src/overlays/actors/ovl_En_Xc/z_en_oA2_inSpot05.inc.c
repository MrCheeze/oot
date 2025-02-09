void En_Oa2_Actor_Spot05_Init(EnXc* this, PlayState* play) {
}

static Vec3f display_position;

void En_Oa2_Set_WalkSound(EnXc* this, PlayState* play) {
    s32 pad[2];
    u32 sfxId;
    s32 pad2;

    if (Skeleton_Info_frame_check(&this->skelAnime, 11.0f) || Skeleton_Info_frame_check(&this->skelAnime, 23.0f)) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            sfxId = NA_SE_PL_WALK_GROUND;
            sfxId += T_BGCheck_getSoundGroundLabel(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
            Na_StartObjectSe_F(&this->actor.projectedPos, sfxId);
        }
    }
}

void En_Oa2_Set_ThrowSound(EnXc* this, PlayState* play) {
    s32 pad[2];
    u32 sfxId;
    s32 pad2;

    if (Skeleton_Info_frame_check(&this->skelAnime, 7.0f)) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            sfxId = NA_SE_PL_WALK_GROUND;
            sfxId += T_BGCheck_getSoundGroundLabel(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
            Na_StartObjectSe_F(&this->actor.projectedPos, sfxId);
        }
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 20.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_SK_SHOUT);
    }
}

void En_Oa2_Set_LandSound(EnXc* this, PlayState* play) {
    u32 sfxId;
    s16 sceneId = play->sceneId;

    if ((z_common_data.sceneLayer != 4) || (sceneId != SCENE_DESERT_COLOSSUS)) {
        if (Skeleton_Info_frame_check(&this->skelAnime, 11.0f)) {
            sfxId = NA_SE_PL_WALK_GROUND;
            sfxId += T_BGCheck_getSoundGroundLabel(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
            Na_StartObjectSe_F(&this->actor.projectedPos, sfxId);
        }
    }
}

void En_Oa2_Set_CameraSound(EnXc* this, PlayState* play) {
    s16 sceneId;

    if (z_common_data.sceneLayer == 4) {
        sceneId = play->sceneId;
        if (sceneId == SCENE_DESERT_COLOSSUS) {
            CutsceneContext* csCtx = &play->csCtx;
            u16 csCurFrame = csCtx->curFrame;
            f32 wDest[2];

            if (csCurFrame == 119) {
                Vec3f pos = { -611.0f, 728.0f, -2.0f };

                Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, &pos, &display_position, wDest);
                Na_StartObjectSe_F(&display_position, NA_SE_EV_JUMP_CONC);
            } else if (csCurFrame == 164) {
                Vec3f pos = { -1069.0f, 38.0f, 0.0f };
                s32 pad;

                Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, &pos, &display_position, wDest);
                Na_StartObjectSe_F(&display_position, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_STONE);
            }
        }
    }
}

void En_Oa2_Set_AppearSound(PlayState* play) {
    s16 sceneId;

    if ((z_common_data.sceneLayer != 4) || (sceneId = play->sceneId, sceneId != SCENE_DESERT_COLOSSUS)) {
        Na_StartFixSe_F(NA_SE_PL_SKIP);
    }
}

void En_Oa2_Set_WindSound(PlayState* play) {
    if (z_common_data.sceneLayer == 4) {
        static s32 set_old = 0;
        static Vec3f zero_pos = { 0.0f, 0.0f, 0.0f };
        static Vec3f eye_old;
        s32 pad;
        s16 sceneId = play->sceneId;

        if (sceneId == SCENE_DESERT_COLOSSUS) {
            CutsceneContext* csCtx = &play->csCtx;
            u16 csCurFrame = csCtx->curFrame;

            if ((csCurFrame >= 120) && (csCurFrame < 164)) {
                s32 pad;
                Vec3f* eye = &play->view.eye;

                if (set_old != 0) {
                    f32 speed = Math3DLength(&eye_old, eye) / 7.058922f;
#if DEBUG_FEATURES
                    static f32 sMaxSpeed = 0.0f;

                    sMaxSpeed = CLAMP_MIN(sMaxSpeed, speed);
                    PRINTF("MAX speed = %f\n", sMaxSpeed);
#endif

                    speed = CLAMP_MAX(speed, 2.0f);
                    Na_SetMotorSe(&zero_pos, NA_SE_EV_FLYING_AIR - SFX_FLAG, 0.6f + (0.4f * speed));
                }

                eye_old.x = eye->x;
                eye_old.y = eye->y;
                eye_old.z = eye->z;
                set_old = 1;
            }
        }
    }
}

void Birth_Fire_In_En_Oa2(EnXc* this, PlayState* play) {
    static s32 appear = false;

    if (!appear) {
        CsCmdActorCue* cue = En_Oa2_Get_npcdemopnt(play, 0);
        f32 xPos = cue->startPos.x;
        f32 yPos = cue->startPos.y;
        f32 zPos = cue->startPos.z;

        this->flameActor = Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_LIGHT, xPos, yPos, zPos, 0, 0, 0, 5);
        appear = true;
    }
}

void Setpos_Fire_In_En_Oa2(EnXc* this, PlayState* play) {
    Vec3f* attachedPos;
    CsCmdActorCue* cue = En_Oa2_Get_npcdemopnt(play, 0);

    if (this->flameActor != NULL) {
        attachedPos = &this->flameActor->world.pos;
        if (!this) {}
        attachedPos->x = cue->startPos.x;
        attachedPos->y = cue->startPos.y;
        attachedPos->z = cue->startPos.z;
    }
}

void Delete_Fire_In_En_Oa2(EnXc* this) {
    if (this->flameActor != NULL) {
        Actor_delete(this->flameActor);
        this->flameActor = NULL;
    }
    Actor_delete(&this->actor);
}

void En_Oa2_Set_EffectFire(EnXc* this, PlayState* play) {
    static s32 old_dousa = 1;
    s32 pad;
    s16 sceneId = play->sceneId;

    if (sceneId == SCENE_DEATH_MOUNTAIN_CRATER) {
        CsCmdActorCue* cue = En_Oa2_Get_npcdemopnt(play, 0);
        if (cue != NULL) {
            s32 nextCueId = cue->id;

            if (old_dousa != nextCueId) {
                if (nextCueId != 1) {
                    Birth_Fire_In_En_Oa2(this, play);
                }

                if (nextCueId == 1) {
                    Delete_Fire_In_En_Oa2(this);
                }

                old_dousa = nextCueId;
            }

            Setpos_Fire_In_En_Oa2(this, play);
        }
    }
}

void En_Oa2_Set_En_Oa2Angle_FaceToFace(EnXc* this, PlayState* play) {
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

AnimationHeader* En_Oa2_Get_soft_anim_tbl_info(PlayState* play, s32 index) {
    AnimationHeader* animation = &gSheikPlayingHarp5Anim;
    CsCmdActorCue* cue = En_Oa2_Get_npcdemopnt(play, index);

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

void En_Oa2_Movement_Approach_accel(EnXc* this) {
    f32 timer = this->timer;
    f32* speedXZ = &this->actor.speed;

    if (timer < 9.0f) {
        *speedXZ = 0.0f;
    } else if (timer < 3.0f) {
        *speedXZ = (((kREG(2) * 0.01f) + 1.2f) / 3.0f) * (timer - 9.0f);
    } else {
        *speedXZ = (kREG(2) * 0.01f) + 1.2f;
    }

    Actor_position_moveF(&this->actor);
}

void En_Oa2_Movement_Approach_move(EnXc* this) {
    Actor_position_moveF(&this->actor);
}

void En_Oa2_Movement_Approach_brake(EnXc* this) {
    f32 timer = this->timer;
    f32* speedXZ = &this->actor.speed;

    if (timer < 3.0f) {
        *speedXZ = (((kREG(2) * 0.01f) + 1.2f) / 3.0f) * (3.0f - timer);
    } else {
        *speedXZ = 0.0f;
    }
    Actor_position_moveF(&this->actor);
}

void En_Oa2_Movement_Away_accel(EnXc* this) {
    En_Oa2_Movement_Approach_accel(this);
}

void En_Oa2_Movement_Away_move(EnXc* this) {
    Actor_position_moveF(&this->actor);
}

void En_Oa2_Movement_Away_brake(EnXc* this) {
    En_Oa2_Movement_Approach_brake(this);
}

void En_Oa2_check_WaitToHide(EnXc* this, PlayState* play) {
    if (En_Oa2_Set_DemoWind(this, play) && En_Oa2_Set_DemoFire(this, play)) {
        this->action = SHEIK_ACTION_WAIT;
    }
}

void En_Oa2_check_HideToAppear(EnXc* this, PlayState* play) {
    s32 pad;
    CutsceneContext* csCtx = &play->csCtx;

    if (csCtx->state != 0) {
        CsCmdActorCue* cue = csCtx->actorCues[4];

        if (cue != NULL && cue->id == 2) {
            s32 pad;
            Vec3f* pos = &this->actor.world.pos;
            SkelAnime* skelAnime = &this->skelAnime;
            f32 frameCount = Si2_anime_end_frame(&gSheikFallingFromSkyAnim);

            this->action = SHEIK_ACTION_GRACEFUL_FALL;
            this->drawMode = SHEIK_DRAW_DEFAULT;

            pos->x = cue->startPos.x;
            pos->y = cue->startPos.y;
            pos->z = cue->startPos.z;

            En_Oa2_Set_En_Oa2Angle_FaceToFace(this, play);
            En_Oa2_Start_Movement_byAnimation(this, play);
            Skeleton_Info2_init(skelAnime, &gSheikFallingFromSkyAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
            En_Oa2_Set_AppearSound(play);
        }
    }
}

void En_Oa2_check_AppearToApproach_accel(EnXc* this, PlayState* play, s32 animFinished) {
    if (animFinished) {
        SkelAnime* skelAnime = &this->skelAnime;
        f32 frameCount = Si2_anime_end_frame(&gSheikWalkingAnim);

        Skeleton_Info2_init(skelAnime, &gSheikWalkingAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, -8.0f);

        this->action = SHEIK_ACTION_ACCEL;
        this->timer = 0.0f;

        En_Oa2_End_Movement_byAnimation(this);
        this->actor.gravity = -((kREG(1) * 0.01f) + 13.0f);
        this->actor.minVelocityY = -((kREG(1) * 0.01f) + 13.0f);
    } else {
        En_Oa2_Movement_byAnimation(this, play);
    }
}

void En_Oa2_check_Approach_accelToApproach_move(EnXc* this) {
    f32* timer = &this->timer;

    *timer += 1.0f;
    if (*timer >= 12.0f) {
        this->actor.speed = (kREG(2) * 0.01f) + 1.2f;
        this->action = SHEIK_ACTION_WALK;
    }
}

void En_Oa2_check_Approach_moveToApproach_brake(EnXc* this) {
    SkelAnime* skelAnime = &this->skelAnime;
    f32 xzDistToPlayer = this->actor.xzDistToPlayer;

    if (xzDistToPlayer <= (kREG(3) + 95.0f)) {
        f32 frameCount = Si2_anime_end_frame(&gSheikIdleAnim);

        Skeleton_Info2_init(skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, -12.0f);
        this->action = SHEIK_ACTION_HALT;
        this->timer = 0.0f;
    }
}

void En_Oa2_check_Approach_brakeToGreet(EnXc* this) {
    f32* timer = &this->timer;

    *timer += 1.0f;
    if (*timer >= 12.0f) {
        this->action = SHEIK_ACTION_STOPPED;
        this->actor.speed = 0.0f;
    }
}

void En_Oa2_check_GreetToTake(EnXc* this, PlayState* play) {
    CsCmdActorCue* cue = En_Oa2_Get_npcdemopnt(play, 4);
    u16 cueId;

    if (cue != NULL) {
        cueId = cue->id;

        if (cueId == 3 || cueId == 11 || cueId == 12 || cueId == 13 || cueId == 23) {
            f32 frameCount = Si2_anime_end_frame(&gSheikPullingOutHarpAnim);

            Skeleton_Info2_init(&this->skelAnime, &gSheikPullingOutHarpAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, -4.0f);
            this->action = SHEIK_ACTION_7;
            this->drawMode = SHEIK_DRAW_PULLING_OUT_HARP;
        }
    }
}

void En_Oa2_check_TakeToReady(EnXc* this, s32 animFinished) {
    SkelAnime* skelAnime;
    f32 frameCount;

    if (animFinished) {
        skelAnime = &this->skelAnime;
        frameCount = Si2_anime_end_frame(&gSheikInitialHarpAnim);
        Skeleton_Info2_init(skelAnime, &gSheikInitialHarpAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
        this->action = SHEIK_ACTION_HARP_READY;
        this->drawMode = SHEIK_DRAW_HARP;
    }
}

void En_Oa2_check_ReadyToPlay(EnXc* this, PlayState* play, s32 animFinished) {
    s32 pad;
    SkelAnime* skelAnime;
    AnimationHeader* animation;
    f32 frameCount;

    if (animFinished) {
        skelAnime = &this->skelAnime;
        animation = En_Oa2_Get_soft_anim_tbl_info(play, 4);
        frameCount = Si2_anime_end_frame(animation);
        Skeleton_Info2_init(skelAnime, animation, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, -8.0f);
        this->action = SHEIK_PLAYING_HARP;
        this->drawMode = SHEIK_DRAW_HARP;
    }
}

void En_Oa2_check_PlayToPlay_stop(EnXc* this, PlayState* play) {
    f32 frameCount;

    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[4];

        if (cue != NULL && cue->id == 8) {
            frameCount = Si2_anime_end_frame(&gSheikInitialHarpAnim);
            Skeleton_Info2_init(&this->skelAnime, &gSheikInitialHarpAnim, 0.0f, frameCount, frameCount, ANIMMODE_LOOP,
                             -8.0f);
            this->action = SHEIK_ACTION_10;
        }
    }
}

void En_Oa2_check_Play_stopToPlay_end(EnXc* this, PlayState* play) {
    f32 curFrame;
    f32 animFrameCount;

    if (En_Oa2_Check_npcdemopnt(this, play, 5, 4)) {
        curFrame = this->skelAnime.curFrame;
        animFrameCount = this->skelAnime.endFrame;
        if (curFrame >= animFrameCount) {
            s32 pad;

            Skeleton_Info2_init(&this->skelAnime, &gSheikInitialHarpAnim, -1.0f,
                             Si2_anime_end_frame(&gSheikInitialHarpAnim), 0.0f, ANIMMODE_ONCE, 0.0f);
            this->action = SHEIK_ACTION_PUT_HARP_AWAY;
        }
    } else if (En_Oa2_Check2_npcdemopnt(this, play, 8, 4)) {
        En_Oa2_check_ReadyToPlay(this, play, true);
    }
}

void En_Oa2_check_Play_endToPut(EnXc* this, s32 animFinished) {
    if (animFinished) {
        Skeleton_Info2_init(&this->skelAnime, &gSheikPullingOutHarpAnim, -1.0f,
                         Si2_anime_end_frame(&gSheikPullingOutHarpAnim), 0.0f, ANIMMODE_ONCE, 0.0f);
        this->action = SHEIK_ACTION_12;
        this->drawMode = SHEIK_DRAW_PULLING_OUT_HARP;
    }
}

void En_Oa2_check_PutToGoodby(EnXc* this, s32 animFinished) {
    if (animFinished) {
        Skeleton_Info2_init(&this->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gSheikIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        this->action = SHEIK_ACTION_13;
        this->drawMode = SHEIK_DRAW_DEFAULT;
        this->timer = 0.0f;
    }
}

void En_Oa2_check_GoodbyToAway_accel(EnXc* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[4];

        if (cue != NULL && cue->id == 4) {
            Skeleton_Info2_init(&this->skelAnime, &gSheikWalkingAnim, -1.0f, Si2_anime_end_frame(&gSheikWalkingAnim),
                             0.0f, ANIMMODE_LOOP, -12.0f);
            this->action = SHEIK_ACTION_REVERSE_ACCEL;
            this->actor.world.rot.y += 0x8000;
            this->timer = 0.0f;
        }
    }
}

void En_Oa2_check_Away_accelToAway_move(EnXc* this) {
    this->timer++;
    if (this->timer >= 12.0f) {
        this->actor.speed = (kREG(2) * 0.01f) + 1.2f;
        this->action = SHEIK_ACTION_REVERSE_WALK;
    }
}

void En_Oa2_check_Away_moveToAway_brake(EnXc* this) {
    f32 xzDistToPlayer = this->actor.xzDistToPlayer;

    if (xzDistToPlayer >= kREG(5) + 140.0f) {
        Skeleton_Info2_init(&this->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gSheikIdleAnim),
                         ANIMMODE_LOOP, -12.0f);
        this->action = SHEIK_ACTION_REVERSE_HALT;
        this->timer = 0.0f;
    }
}

void En_Oa2_check_Away_brakeToThrow(EnXc* this) {
    this->timer++;
    if (this->timer >= 12.0f) {
        Skeleton_Info2_init(&this->skelAnime, &gSheikThrowingNutAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gSheikThrowingNutAnim), ANIMMODE_ONCE, 0.0f);
        this->action = SHEIK_ACTION_THROW_NUT;
        this->timer = 0.0f;
        this->actor.speed = 0.0f;
    }
}

void En_Oa2_check_ThrowToFade(EnXc* this, PlayState* play) {
    this->timer++;
    if (this->timer >= 30.0f) {
        this->action = SHEIK_ACTION_DELETE;
        Birth_Door_Deku_In_En_Oa2(this, play);
    }
}

void En_Oa2_check_FadeToDisappear(EnXc* this, PlayState* play) {
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
                Actor_delete(&this->actor);
            }
        }
    }
}

void En_Oa2_Actor_main_wait(EnXc* this, PlayState* play) {
    En_Oa2_Set_CameraSound(this, play);
    En_Oa2_Set_WindSound(play);
    En_Oa2_check_WaitToHide(this, play);
}

void En_Oa2_Actor_main_hide(EnXc* this, PlayState* play) {
    En_Oa2_Set_CameraSound(this, play);
    En_Oa2_Set_WindSound(play);
    En_Oa2_check_HideToAppear(this, play);
}

void En_Oa2_Actor_main_appear(EnXc* this, PlayState* play) {
    s32 animFinished = En_Oa2_Animation_Basic(this);

    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Set_LandSound(this, play);
    En_Oa2_Set_CameraSound(this, play);
    En_Oa2_Set_WindSound(play);
    En_Oa2_check_AppearToApproach_accel(this, play, animFinished);
}

void En_Oa2_Actor_main_approach_accel(EnXc* this, PlayState* play) {
    En_Oa2_Movement_Approach_accel(this);
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Set_WalkSound(this, play);
    En_Oa2_check_Approach_accelToApproach_move(this);
}

void En_Oa2_Actor_main_approach_move(EnXc* this, PlayState* play) {
    En_Oa2_Movement_Approach_move(this);
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Set_WalkSound(this, play);
    En_Oa2_check_Approach_moveToApproach_brake(this);
}

void En_Oa2_Actor_main_approach_brake(EnXc* this, PlayState* play) {
    En_Oa2_Movement_Approach_brake(this);
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Set_WalkSound(this, play);
    En_Oa2_check_Approach_brakeToGreet(this);
}

void En_Oa2_Actor_main_greet(EnXc* this, PlayState* play) {
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_check_GreetToTake(this, play);
}

void En_Oa2_Actor_main_take(EnXc* this, PlayState* play) {
    s32 animFinished = En_Oa2_Animation_Basic(this);

    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_check_TakeToReady(this, animFinished);
}

void En_Oa2_Actor_main_ready(EnXc* this, PlayState* play) {
    s32 animFinished = En_Oa2_Animation_Basic(this);

    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_check_ReadyToPlay(this, play, animFinished);
}

void En_Oa2_Actor_main_play(EnXc* this, PlayState* play) {
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_check_PlayToPlay_stop(this, play);
}

void En_Oa2_Actor_main_play_stop(EnXc* this, PlayState* play) {
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_check_Play_stopToPlay_end(this, play);
}

void En_Oa2_Actor_main_play_end(EnXc* this, PlayState* play) {
    s32 animFinished = En_Oa2_Animation_Basic(this);

    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_check_Play_endToPut(this, animFinished);
}

void En_Oa2_Actor_main_put(EnXc* this, PlayState* play) {
    s32 animFinished = En_Oa2_Animation_Basic(this);

    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_check_PutToGoodby(this, animFinished);
}

void En_Oa2_Actor_main_goodby(EnXc* this, PlayState* play) {
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Set_EffectFire(this, play);
    En_Oa2_check_GoodbyToAway_accel(this, play);
}

void En_Oa2_Actor_main_away_accel(EnXc* this, PlayState* play) {
    En_Oa2_Movement_Away_accel(this);
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Set_WalkSound(this, play);
    En_Oa2_Set_EffectFire(this, play);
    En_Oa2_check_Away_accelToAway_move(this);
}

void En_Oa2_Actor_main_away_move(EnXc* this, PlayState* play) {
    En_Oa2_Movement_Away_move(this);
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Set_WalkSound(this, play);
    En_Oa2_Set_EffectFire(this, play);
    En_Oa2_check_Away_moveToAway_brake(this);
}

void En_Oa2_Actor_main_away_brake(EnXc* this, PlayState* play) {
    En_Oa2_Movement_Away_brake(this);
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Set_WalkSound(this, play);
    En_Oa2_Set_EffectFire(this, play);
    En_Oa2_check_Away_brakeToThrow(this);
}

void En_Oa2_Actor_main_throw(EnXc* this, PlayState* play) {
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Set_ThrowSound(this, play);
    En_Oa2_Set_EffectFire(this, play);
    En_Oa2_check_ThrowToFade(this, play);
}

void En_Oa2_Actor_main_fade(EnXc* this, PlayState* play) {
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Set_EffectFire(this, play);
    En_Oa2_check_FadeToDisappear(this, play);
}

void En_Oa2_Actor_main_disappear(EnXc* this, PlayState* play) {
    En_Oa2_Set_EffectFire(this, play);
}

void En_Oa2_Actor_ChangeDraw_take(Gfx** dList, EnXc* this) {
    f32 currentFrame = this->skelAnime.curFrame;

    if (currentFrame >= 34.0f) {
        *dList = gSheikHarpDL;
    }
}

s32 En_Oa2_Actor_SetDraw_take(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                        void* thisx) {
    EnXc* this = (EnXc*)thisx;

    if (limbIndex == 12) {
        En_Oa2_Actor_ChangeDraw_take(dList, this);
    }

    return 0;
}

s32 En_Oa2_Actor_SetDraw_harp(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnXc* this = (EnXc*)thisx;

    if (limbIndex == 12) {
        *dList = gSheikHarpDL;
    }

    return 0;
}

void En_Oa2_Actor_draw_take(Actor* thisx, PlayState* play) {
    EnXc* this = (EnXc*)thisx;
    s32 pad;
    s16 eyePattern = this->eyeIdx;
    void* eyeTexture = en_oa2_eye[eyePattern];
    SkelAnime* skelAnime = &this->skelAnime;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad2;

    OPEN_DISPS(gfxCtx, "../z_en_oA2_inSpot05.c", 1444);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 20, 0);
    gDPSetEnvColor(POLY_OPA_DISP++, 60, 0, 0, 0);

    _texture_z_light_fog_prim(gfxCtx);
    Actor_HiliteReflect_set_init(&this->actor, play, 0);
    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          En_Oa2_Actor_SetDraw_take, NULL, this);
    CLOSE_DISPS(gfxCtx, "../z_en_oA2_inSpot05.c", 1497);
}

void En_Oa2_Actor_draw_harp(Actor* thisx, PlayState* play) {
    EnXc* this = (EnXc*)thisx;
    s32 pad;
    s16 eyePattern = this->eyeIdx;
    void* eyeTexture = en_oa2_eye[eyePattern];
    SkelAnime* skelAnime = &this->skelAnime;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad2;

    OPEN_DISPS(gfxCtx, "../z_en_oA2_inSpot05.c", 1511);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 20, 0);
    gDPSetEnvColor(POLY_OPA_DISP++, 60, 0, 0, 0);

    _texture_z_light_fog_prim(gfxCtx);
    Actor_HiliteReflect_set_init(&this->actor, play, 0);
    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          En_Oa2_Actor_SetDraw_harp, NULL, this);
    CLOSE_DISPS(gfxCtx, "../z_en_oA2_inSpot05.c", 1564);
}
