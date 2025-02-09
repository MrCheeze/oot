static u32 done = 0;

void En_Ru1_Option_Set_WalkSound(EnRu1* this) {
    SkelAnime* skelAnime = &this->skelAnime;

    if (Skeleton_Info_frame_check(skelAnime, 2.0f) || Skeleton_Info_frame_check(skelAnime, 7.0f) ||
        Skeleton_Info_frame_check(skelAnime, 12.0f) || Skeleton_Info_frame_check(skelAnime, 18.0f) ||
        Skeleton_Info_frame_check(skelAnime, 25.0f) || Skeleton_Info_frame_check(skelAnime, 33.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_JABU);
    }
}

void En_Ru1_Option_SetFallSound(EnRu1* this, PlayState* play) {
    Effect_SE_Info_new(play, &this->actor.projectedPos, 20, NA_SE_VO_RT_FALL);
}

void En_Ru1_Option_SetLaughSound(EnRu1* this) {
    if (Skeleton_Info_frame_check(&this->skelAnime, 5.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_RT_LAUGH_0);
    }
}

void En_Ru1_Option_SetKidnapSound(EnRu1* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_RT_THROW);
}

void En_Ru1_Option_SetFace_waitlink(EnRu1* this) {
    if (En_Ru1_CheckConect_inStand(this) == 2) {
        En_Ru1_set_eye_Num(this, 3);
        En_Ru1_set_mouth_Num(this, 2);
        if (this->skelAnime.mode != 2) {
            En_Ru1_Change_Anime(this, &gRutoChildShutterAnim, 2, -8.0f, 0);
            En_Ru1_Option_SetKidnapSound(this);
        }
    }
}

void En_Ru1_Option_SetFace_errand(EnRu1* this) {
    f32 curFrame;

    if (done == 0) {
        curFrame = this->skelAnime.curFrame;
        if (curFrame >= 60.0f) {
            En_Ru1_set_eye_Num(this, 3);
            En_Ru1_set_mouth_Num(this, 0);
            En_Ru1_Option_Set_ThrowSound(this);
            done = 1;
        }
    }
}

void En_Ru1_Option_check_StandupToErrand(EnRu1* this, PlayState* play) {
    f32 frameCount;
    CsCmdActorCue* cue;
    CsCmdActorCue* cue2;
    s16 newRotTmp;

    if (En_Ru1_Check2_npcdemopnt(play, 1, 3)) {
        frameCount = Si2_anime_end_frame(&gRutoChildWalkToAndHoldUpSapphireAnim);
        // this weird part with the redundant variable is necessary to match for some reason
        cue2 = play->csCtx.actorCues[3];
        cue = cue2;
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;
        newRotTmp = cue->rot.x;
        this->actor.shape.rot.x = newRotTmp;
        this->actor.world.rot.x = newRotTmp;
        newRotTmp = cue->rot.y;
        this->actor.shape.rot.y = newRotTmp;
        this->actor.world.rot.y = newRotTmp;
        newRotTmp = cue->rot.z;
        this->actor.shape.rot.z = newRotTmp;
        this->actor.world.rot.z = newRotTmp;
        Skeleton_Info2_init(&this->skelAnime, &gRutoChildWalkToAndHoldUpSapphireAnim, 1.0f, 0.0f, frameCount,
                         ANIMMODE_ONCE, 0.0f);
        En_Ru1_Start_Movement_byAnimation(this, play);
        this->action = 37;
        this->drawConfig = 1;
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void En_Ru1_Option_check_ErrandToHandup(EnRu1* this, UNK_TYPE arg1) {
    if (arg1 != 0) {
        Skeleton_Info2_init(&this->skelAnime, &gRutoChildHoldArmsUpAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gRutoChildHoldArmsUpAnim), ANIMMODE_LOOP, 0.0f);
        En_Ru1_End_Movement_byAnimation(this);
        this->action = 38;
    }
}

void En_Ru1_Option_check_HandupToHanddown(EnRu1* this, PlayState* play) {
    if (En_Ru1_Check2_npcdemopnt(play, 2, 3)) {
        Skeleton_Info2_init(&this->skelAnime, &gRutoChildBringHandsDownAnim, 1.0f, 0,
                         Si2_anime_end_frame(&gRutoChildBringHandsDownAnim), ANIMMODE_ONCE, -8.0f);
        this->action = 39;
    }
}

void En_Ru1_Option_check_HanddownToOrder(EnRu1* this, UNK_TYPE arg1) {
    if (arg1 != 0) {
        Skeleton_Info2_init(&this->skelAnime, &gRutoChildWait2Anim, 1.0f, 0, Si2_anime_end_frame(&gRutoChildWait2Anim),
                         ANIMMODE_LOOP, -8.0f);
        this->action = 40;
    }
}

void En_Ru1_Option_check_OrderToWaitlink(EnRu1* this, PlayState* play) {
    s32 pad[2];
    s8 curRoomNum;

    if (!(DEBUG_FEATURES && IS_CUTSCENE_LAYER) && En_Ru1_Check_EndDemoMode(play)) {
        curRoomNum = play->roomCtx.curRoom.num;
        SET_INFTABLE(INFTABLE_145);
        Actor_Environment_sw_On(play, En_Ru1_GetUpper_arg_data(this));
        En_Ru1_SetConect_inStand(this, 1);
        this->action = 42;
        this->actor.room = curRoomNum;
    }
}

void En_Ru1_Option_check_WaitlinkToScream(EnRu1* this, PlayState* play) {
    if (En_Ru1_CheckConect_inStand(this) == 3) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        this->actor.textId = 0x4048;
#if !OOT_PAL_N64
        message_set2(play, this->actor.textId);
#else
        message_set(play, this->actor.textId, NULL);
#endif
        En_Ru1_Option_SetFallSound(this, play);
        this->action = 43;
        this->drawConfig = 0;
    }
}

void En_Ru1_Option_check_ScreamToDisappear(EnRu1* this, PlayState* play) {
    if (En_Ru1_ConversationEnd(play) != 0) {
        En_Ru1_SetConect_inStand(this, 4);
        Actor_delete(&this->actor);
    }
}

void En_Ru1_Option_Actor_main_standup(EnRu1* this, PlayState* play) {
    En_Ru1_Calc_turn_front(this);
    En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_Option_check_StandupToErrand(this, play);
#if DEBUG_FEATURES
    func_80AEB220(this, play);
#endif
}

void En_Ru1_Option_Actor_main_errand(EnRu1* this, PlayState* play) {
    s32 something;

    En_Ru1_Calc_turn_front(this);
    En_Ru1_Movement_byAnimation(this, play);
    En_Ru1_BGcheck(this, play);
    something = En_Ru1_Animation_Base(this);
    En_Ru1_Option_SetLaughSound(this);
    En_Ru1_Option_SetFace_errand(this);
    En_Ru1_Option_Set_WalkSound(this);
    En_Ru1_Option_check_ErrandToHandup(this, something);
#if DEBUG_FEATURES
    func_80AEB220(this, play);
#endif
}

void En_Ru1_Option_Actor_main_handup(EnRu1* this, PlayState* play) {
    En_Ru1_Calc_turn_front(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Animation_Base(this);
    En_Ru1_Option_check_HandupToHanddown(this, play);
#if DEBUG_FEATURES
    func_80AEB220(this, play);
#endif
}

void En_Ru1_Option_Actor_main_handdown(EnRu1* this, PlayState* play) {
    s32 something;

    En_Ru1_Calc_turn_front(this);
    En_Ru1_BGcheck(this, play);
    something = En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_Option_check_HanddownToOrder(this, something);
#if DEBUG_FEATURES
    func_80AEB220(this, play);
#endif
}

void En_Ru1_Option_Actor_main_order(EnRu1* this, PlayState* play) {
    En_Ru1_Calc_turn_front(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_Option_check_OrderToWaitlink(this, play);
#if DEBUG_FEATURES
    func_80AEB220(this, play);
#endif
}

void En_Ru1_Option_Actor_main_waitlink(EnRu1* this, PlayState* play) {
    En_Ru1_Calc_turn_front(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_Option_SetFace_waitlink(this);
    En_Ru1_Option_check_WaitlinkToScream(this, play);
}

void En_Ru1_Option_Actor_main_scream(EnRu1* this, PlayState* play) {
    En_Ru1_Calc_turn_front(this);
    En_Ru1_Option_check_ScreamToDisappear(this, play);
}
