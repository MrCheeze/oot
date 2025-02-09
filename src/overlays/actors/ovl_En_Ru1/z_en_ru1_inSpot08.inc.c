void En_Ru1_Actor_Spot08_Init(EnRu1* this, PlayState* play) {
    En_Ru1_Change_Anime(this, &gRutoChildWaitHandsBehindBackAnim, 0, 0, 0);
    this->action = 0;
    this->drawConfig = 1;
    En_Ru1_set_eye_Num(this, 4);
    En_Ru1_set_mouth_Num(this, 0);
}

CsCmdActorCue* En_Ru1_Cryst_Get_npcdemopnt(PlayState* play) {
    return En_Ru1_Get_npcdemopnt(play, 3);
}

s32 En_Ru1_Cryst_Check_npcdemopnt(PlayState* play, u16 cueId) {
    return En_Ru1_Check_npcdemopnt(play, cueId, 3);
}

s32 En_Ru1_Cryst_Check2_npcdemopnt(PlayState* play, u16 cueId) {
    return En_Ru1_Check2_npcdemopnt(play, cueId, 3);
}

void Birth_Ripple_In_En_Ru1_Cryst(EnRu1* this, PlayState* play, s16 radiusMax, s16 life) {
    Vec3f pos;
    Actor* thisx = &this->actor;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + this->actor.depthInWater;
    pos.z = this->actor.world.pos.z;
    Effect_SS_G_Ripple_ct2(play, &pos, 100, radiusMax, life);
}

void Set_Ripple_byTimer_In_En_Ru1_Cryst(EnRu1* this, PlayState* play) {
    this->unk_270 += 1.0f;
    if (this->unk_270 >= kREG(3) + 10.0f) {
        Birth_Ripple_In_En_Ru1_Cryst(this, play, kREG(1) + 500, 0);
        this->unk_270 = 0.0f;
    }
}

void Set_Ripple_whenJump_In_En_Ru1_Cryst(EnRu1* this, PlayState* play) {
    Birth_Ripple_In_En_Ru1_Cryst(this, play, kREG(2) + 500, 0);
    Birth_Ripple_In_En_Ru1_Cryst(this, play, kREG(2) + 500, kREG(3) + 10.0f);
    Birth_Ripple_In_En_Ru1_Cryst(this, play, kREG(2) + 500, (kREG(3) + 10.0f) * 2.0f);
}

void Birth_Splash_In_En_Ru1_Cryst(EnRu1* this, PlayState* play) {
    Vec3f pos;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + this->actor.depthInWater;
    pos.z = this->actor.world.pos.z;

    Effect_SS_G_Splash_sc_cl_ct(play, &pos, NULL, NULL, 1, 0);
}

void En_Ru1_Cryst_Movement_byAnimation(EnRu1* this, PlayState* play) {
    SkelAnime* skelAnime = &this->skelAnime;

    if (skelAnime->baseTransl.y < skelAnime->jointTable[0].y) {
        skelAnime->movementFlags |= ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y;
        Skeleton_Proc_Anime_Move_init(play, &this->actor, skelAnime, 1.0f);
    }
}

void En_Ru1_Cryst_Start_Movement_byAnimation(EnRu1* this, PlayState* play) {
    SkelAnime* skelAnime = &this->skelAnime;

    skelAnime->baseTransl = skelAnime->jointTable[0];
    skelAnime->prevTransl = skelAnime->jointTable[0];
    if (skelAnime->baseTransl.y < skelAnime->jointTable[0].y) {
        skelAnime->movementFlags |= ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y;
        Skeleton_Proc_Anime_Move_init(play, &this->actor, skelAnime, 1.0f);
    }
}

void En_Ru1_Cryst_End_Movement_byAnimation(EnRu1* this) {
    this->skelAnime.movementFlags &= ~(ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y);
}

f32 En_Ru1_Cryst_CalcParam(CsCmdActorCue* cue, PlayState* play) {
    s32 csCurFrame = play->csCtx.curFrame;

    if ((csCurFrame < cue->endFrame) && (cue->endFrame - cue->startFrame > 0)) {
        return (cos_s(((csCurFrame - cue->startFrame) / (f32)(cue->endFrame - cue->startFrame)) * 32768.0f) *
                -0.5f) +
               0.5f;
    }
    return 1.0f;
}

f32 En_Ru1_Cryst_CalcPos(f32 arg0, s32 arg1, s32 arg2) {
    return (((f32)arg2 - arg1) * arg0) + arg1;
}

void En_Ru1_Cryst_SetPos_Kiss(EnRu1* this, PlayState* play) {
    CsCmdActorCue* cue = En_Ru1_Cryst_Get_npcdemopnt(play);

    if (cue != NULL) {
        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;

        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;
    }
}

void En_Ru1_Cryst_SetPos_Jump(EnRu1* this, PlayState* play) {
    En_Ru1_Cryst_SetPos_Kiss(this, play);
}

void En_Ru1_Cryst_SetPos_Float(EnRu1* this, PlayState* play) {
    En_Ru1_Cryst_SetPos_Kiss(this, play);
}

void En_Ru1_Cryst_Movement_float(EnRu1* this, PlayState* play) {
    En_Ru1_Cryst_Movement_byAnimation(this, play);
}

void En_Ru1_Cryst_Movement_approach(EnRu1* this, PlayState* play) {
    Vec3f* thisPos;
    f32 sp30;
    CsCmdActorCue* cue = En_Ru1_Cryst_Get_npcdemopnt(play);
    s32 pad;

    if (cue != NULL) {
        sp30 = En_Ru1_Cryst_CalcParam(cue, play);
        thisPos = &this->actor.world.pos;
        thisPos->x = En_Ru1_Cryst_CalcPos(sp30, cue->startPos.x, cue->endPos.x);
        thisPos->y = En_Ru1_Cryst_CalcPos(sp30, cue->startPos.y, cue->endPos.y);
        thisPos->z = En_Ru1_Cryst_CalcPos(sp30, cue->startPos.z, cue->endPos.z);
    }
}

void En_Ru1_Cryst_Movement_sink(EnRu1* this, PlayState* play) {
    En_Ru1_Cryst_Movement_byAnimation(this, play);
}

void En_Ru1_Cryst_Movement_leave(EnRu1* this, PlayState* play) {
    s32 pad;
    Vec3f* unk_364 = &this->unk_364;
    Vec3f* thisPos;
    f32 temp_ret_2;
    CsCmdActorCue* cue = En_Ru1_Cryst_Get_npcdemopnt(play);
    s32 pad2;

    if (cue != NULL) {
        temp_ret_2 = En_Ru1_Cryst_CalcParam(cue, play);
        thisPos = &this->actor.world.pos;
        thisPos->x = En_Ru1_Cryst_CalcPos(temp_ret_2, unk_364->x, cue->endPos.x);
        thisPos->y = En_Ru1_Cryst_CalcPos(temp_ret_2, unk_364->y, cue->endPos.y);
        thisPos->z = En_Ru1_Cryst_CalcPos(temp_ret_2, unk_364->z, cue->endPos.z);
    }
}

void En_Ru1_Cryst_Set_SplashSound(EnRu1* this) {
    if (this->unk_298 == 0) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_EV_DIVE_INTO_WATER);
        this->unk_298 = 1;
    }
}

void En_Ru1_Cryst_Set_AppearSound(EnRu1* this) {
    if (Skeleton_Info_frame_check(&this->skelAnime, 5.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_PL_FACE_UP);
    }
}

void En_Ru1_Cryst_Set_SwimSound(EnRu1* this) {
    SkelAnime* skelAnime = &this->skelAnime;

    if (Skeleton_Info_frame_check(skelAnime, 4.0f) || Skeleton_Info_frame_check(skelAnime, 13.0f) ||
        Skeleton_Info_frame_check(skelAnime, 22.0f) || Skeleton_Info_frame_check(skelAnime, 31.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_PL_SWIM);
    }
}

void En_Ru1_Cryst_Set_SinkSound(EnRu1* this) {
    if (Skeleton_Info_frame_check(&this->skelAnime, 8.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_PL_SUBMERGE);
    }
}

void En_Ru1_Cryst_Set_LinkSplashSound(PlayState* play) {
    Player* player;

    if (play->csCtx.curFrame == 205) {
        player = GET_PLAYER(play);
        Nai_FxFlagEntry(NA_SE_EV_DIVE_INTO_WATER, &player->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
}

void En_Ru1_Cryst_SetLaughSound(EnRu1* this, PlayState* play) {
    if (play->csCtx.curFrame == 130) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_RT_LAUGH_0);
    }
}

void En_Ru1_check_MogToSeoyogi(EnRu1* this, UNK_TYPE arg1) {
    if (arg1 != 0) {
        Skeleton_Info2_init(&this->skelAnime, &gRutoChildSwimOnBackAnim, 1.0f, 0,
                         Si2_anime_end_frame(&gRutoChildSwimOnBackAnim), ANIMMODE_LOOP, -8.0f);
    }
}

void En_Ru1_check_KissToJump(EnRu1* this, PlayState* play) {
    if (En_Ru1_Cryst_Check2_npcdemopnt(play, 2)) {
        this->action = 1;
        this->drawConfig = 0;
        En_Ru1_Cryst_SetPos_Jump(this, play);
        En_Ru1_BGcheck(this, play);
        Birth_Splash_In_En_Ru1_Cryst(this, play);
        Set_Ripple_whenJump_In_En_Ru1_Cryst(this, play);
    }
}

void En_Ru1_check_JumpToFloat(EnRu1* this, PlayState* play) {
    s32 pad[2];
    f32 frameCount;

    if (En_Ru1_Cryst_Check2_npcdemopnt(play, 3)) {
        frameCount = Si2_anime_end_frame(&gRutoChildAnim_009060);
        En_Ru1_Cryst_SetPos_Float(this, play);
        En_Ru1_Cryst_Start_Movement_byAnimation(this, play);
        Skeleton_Info2_init(&this->skelAnime, &gRutoChildAnim_009060, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
        this->action = 2;
        this->drawConfig = 1;
    }
}

void En_Ru1_check_FloatToApproach(EnRu1* this, PlayState* play, s32 arg2) {
    s32 pad[2];

    if (arg2 != 0) {
        f32 frameCount = Si2_anime_end_frame(&gRutoChildTreadWaterAnim);

        En_Ru1_Cryst_End_Movement_byAnimation(this);
        Skeleton_Info2_init(&this->skelAnime, &gRutoChildTreadWaterAnim, 1.0f, 0, frameCount, ANIMMODE_LOOP, -8.0f);
        this->action = 3;
    } else {
        En_Ru1_Cryst_Movement_float(this, play);
    }
}

void En_Ru1_check_ApproachToSink(EnRu1* this, PlayState* play) {
    s32 pad[2];
    f32 frameCount;

    if (En_Ru1_Cryst_Check_npcdemopnt(play, 6)) {
        frameCount = Si2_anime_end_frame(&gRutoChildTransitionToSwimOnBackAnim);
        En_Ru1_Cryst_Start_Movement_byAnimation(this, play);
        Skeleton_Info2_init(&this->skelAnime, &gRutoChildTransitionToSwimOnBackAnim, 1.0f, 0, frameCount, ANIMMODE_ONCE,
                         -8.0f);
        this->action = 4;
    }
}

void En_Ru1_check_SinkToLeave(EnRu1* this, PlayState* play) {
    if (En_Ru1_Cryst_Check2_npcdemopnt(play, 6)) {
        s32 pad;

        En_Ru1_Cryst_End_Movement_byAnimation(this);
        this->action = 5;
        this->unk_364 = this->actor.world.pos;
    } else {
        En_Ru1_Cryst_Movement_sink(this, play);
    }
}

void En_Ru1_check_LeaveToBrake(EnRu1* this, PlayState* play) {
    CsCmdActorCue* cue = En_Ru1_Cryst_Get_npcdemopnt(play);
    f32 frameCount;
    u16 csCurFrame;
    u16 endFrame;

    if (cue != NULL) {
        csCurFrame = play->csCtx.curFrame;
        endFrame = cue->endFrame;

        if (csCurFrame >= endFrame - 2) {
            frameCount = Si2_anime_end_frame(&gRutoChildTransitionFromSwimOnBackAnim);
            Skeleton_Info2_init(&this->skelAnime, &gRutoChildTransitionFromSwimOnBackAnim, 1.0, 0, frameCount,
                             ANIMMODE_ONCE, -8.0f);
            this->action = 6;
        }
    }
}

void En_Ru1_check_BrakeToDelete(EnRu1* this, PlayState* play, UNK_TYPE arg2) {
    if ((En_Ru1_Cryst_Check_npcdemopnt(play, 8)) && (arg2 != 0)) {
        Actor_delete(&this->actor);
    }
}

void En_Ru1_Cryst_Actor_main_kiss(EnRu1* this, PlayState* play) {
    En_Ru1_Cryst_SetPos_Kiss(this, play);
    En_Ru1_Animation_Base(this);
    En_Ru1_Cryst_SetLaughSound(this, play);
    En_Ru1_Cryst_Set_LinkSplashSound(play);
    En_Ru1_check_KissToJump(this, play);
}

void En_Ru1_Cryst_Actor_main_jump(EnRu1* this, PlayState* play) {
    En_Ru1_Cryst_Set_SplashSound(this);
    En_Ru1_check_JumpToFloat(this, play);
}

void En_Ru1_Cryst_Actor_main_float(EnRu1* this, PlayState* play) {
    s32 something = En_Ru1_Animation_Base(this);

    En_Ru1_BGcheck(this, play);
    En_Ru1_Cryst_Set_AppearSound(this);
    En_Ru1_check_FloatToApproach(this, play, something);
}

void En_Ru1_Cryst_Actor_main_approach(EnRu1* this, PlayState* play) {
    En_Ru1_Cryst_Movement_approach(this, play);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Animation_Base(this);
    Set_Ripple_byTimer_In_En_Ru1_Cryst(this, play);
    En_Ru1_check_ApproachToSink(this, play);
}

void En_Ru1_Cryst_Actor_main_sink(EnRu1* this, PlayState* play) {
    s32 something;

    something = En_Ru1_Animation_Base(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_set_eye_pattern(this);
    Set_Ripple_byTimer_In_En_Ru1_Cryst(this, play);
    En_Ru1_check_MogToSeoyogi(this, something);
    En_Ru1_Cryst_Set_SinkSound(this);
    En_Ru1_check_SinkToLeave(this, play);
}

void En_Ru1_Cryst_Actor_main_leave(EnRu1* this, PlayState* play) {
    s32 something;

    something = En_Ru1_Animation_Base(this);
    En_Ru1_Cryst_Movement_leave(this, play);
    En_Ru1_BGcheck(this, play);
    En_Ru1_set_eye_pattern(this);
    Set_Ripple_byTimer_In_En_Ru1_Cryst(this, play);
    En_Ru1_check_MogToSeoyogi(this, something);
    En_Ru1_Cryst_Set_SwimSound(this);
    En_Ru1_check_LeaveToBrake(this, play);
}

void En_Ru1_Cryst_Actor_main_brake(EnRu1* this, PlayState* play) {
    s32 something;

    something = En_Ru1_Animation_Base(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_set_eye_pattern(this);
    Set_Ripple_byTimer_In_En_Ru1_Cryst(this, play);
    En_Ru1_check_BrakeToDelete(this, play, something);
}
