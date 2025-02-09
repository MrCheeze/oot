void En_Ru1_Calc_turn_link(EnRu1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 trackingPreset;

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = kREG(16) - 3.0f;
    trackingPreset = kREG(17) + 0xC;
    eye_moveM(&this->actor, &this->interactInfo, trackingPreset, NPC_TRACKING_HEAD_AND_TORSO);
}

void En_Ru1_Calc_turn_link2(EnRu1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 trackingPreset;

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = kREG(16) - 3.0f;
    trackingPreset = kREG(17) + 0xC;
    eye_moveM(&this->actor, &this->interactInfo, trackingPreset, NPC_TRACKING_FULL_BODY);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void En_Ru1_Option_Set_SitdownSound(EnRu1* this) {
    if (Skeleton_Info_frame_check(&this->skelAnime, 11.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_EV_LAND_DIRT);
    }
}

s32 En_Ru1_Option_check_GreetToSitdown(EnRu1* this, PlayState* play) {
    if (GET_INFTABLE(INFTABLE_142)) {
        f32 frameCount = Si2_anime_end_frame(&gRutoChildSitAnim);

        Skeleton_Info2_init(&this->skelAnime, &gRutoChildSitAnim, 1.0f, 0, frameCount, ANIMMODE_ONCE, -8.0f);
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        this->action = 26;
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        return true;
    }
    return false;
}

void En_Ru1_Option_check_GreetToGreeting(EnRu1* this, PlayState* play, s32 cond) {
    if (cond) {
        this->action = 25;
    }
}

void En_Ru1_Option_check_GreetingBranch(EnRu1* this, PlayState* play) {
    if (En_Ru1_ConversationAndPad_Trigger(play) && !En_Ru1_Option_check_GreetToSitdown(this, play)) {
        message_close(play);
        SET_INFTABLE(INFTABLE_142);
        this->action = 24;
    }
}

void En_Ru1_Option_check_SitdownToAlone_stop(EnRu1* this, PlayState* play, UNK_TYPE arg2) {
    if (arg2 != 0) {
        Skeleton_Info2_init(&this->skelAnime, &gRutoChildSittingAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gRutoChildSittingAnim), ANIMMODE_LOOP, 0.0f);
        message_close(play);
        SET_INFTABLE(INFTABLE_143);
        En_Ru1_Option_reset_Alone(this, play);
        Actor_carry_request(&this->actor, play);
        this->action = 27;
        En_Ru1_Option_Reset_Corect_forOption(this);
    }
}

void En_Ru1_Option_Actor_main_wait(EnRu1* this, PlayState* play) {
    this->action = 23;
}

void En_Ru1_Option_Actor_main_hide(EnRu1* this, PlayState* play) {
    this->action = 24;
    this->drawConfig = 1;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
}

void En_Ru1_Option_Actor_main_greet(EnRu1* this, PlayState* play) {
    s32 cond;

    En_Ru1_Calc_turn_link(this, play);
    En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_Option_Excute_Corect_forStand(this, play);
    En_Ru1_BGcheck(this, play);
    cond = En_Ru1_Option_SetTalk(this, play);
    En_Ru1_Option_check_Alone(this, play);
    En_Ru1_Option_check_GreetToGreeting(this, play, cond);
}

void En_Ru1_Option_Actor_main_greeting(EnRu1* this, PlayState* play) {
    En_Ru1_Calc_turn_link2(this, play);
    En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Option_check_GreetingBranch(this, play);
}

void En_Ru1_Option_Actor_main_sitdown(EnRu1* this, PlayState* play) {
    s32 something;

    En_Ru1_Calc_turn_front(this);
    something = En_Ru1_Animation_Base(this);
    En_Ru1_Option_Set_SitdownSound(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Option_check_SitdownToAlone_stop(this, play, something);
}
