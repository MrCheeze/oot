void En_Oa2_Actor_IceShrine_Init(EnXc* this, PlayState* play) {
    this->action = SHEIK_ACTION_SERENADE;
}

void En_Oa2_Ice_check_WaitToHide(EnXc* this, PlayState* play) {
    if (En_Oa2_Set_DemoWater(this, play)) {
        this->action = SHEIK_ACTION_30;
    }
}

void En_Oa2_Ice_check_HideToGreet(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_30, SHEIK_ACTION_31);
}

void En_Oa2_Ice_check_GreetToTake(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_31, SHEIK_ACTION_32);
}

void En_Oa2_Ice_check_TakeToReady(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_32, SHEIK_ACTION_33);
}

void En_Oa2_Ice_check_ReadyToPlay(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_33, SHEIK_ACTION_34);
}

void En_Oa2_Ice_check_PlayToPlay_stop(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_34, SHEIK_ACTION_35);
}

void En_Oa2_Ice_check_Play_stopToPlay_end(EnXc* this) {
    En_Oa2_CheckAndSet_Mode_forPlay_stopToPlay(this, SHEIK_ACTION_35, SHEIK_ACTION_36, SHEIK_ACTION_34);
}

void En_Oa2_Ice_check_Play_endToPut(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_36, SHEIK_ACTION_37);
}

void En_Oa2_Ice_check_PutToGoodby(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_37, SHEIK_ACTION_38);
}

void En_Oa2_Ice_check_GoodbyToAway_accel(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_38, SHEIK_ACTION_39);
}

void En_Oa2_Ice_check_Away_accelToAway_move(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_39, SHEIK_ACTION_40);
}

void En_Oa2_Ice_check_Away_moveToAway_brake(EnXc* this) {
    f32 xzDistToPlayer = this->actor.xzDistToPlayer;

    if (kREG(5) + 140.0f <= xzDistToPlayer) {
        Skeleton_Info2_init(&this->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gSheikIdleAnim),
                         ANIMMODE_LOOP, -12.0f);
        this->action = SHEIK_ACTION_41;
        this->timer = 0.0f;
    }
}

void En_Oa2_Ice_check_Away_brakeToThrow(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_41, SHEIK_ACTION_42);
}

void En_Oa2_Ice_check_ThrowToFade(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_42, SHEIK_ACTION_43);
}

void En_Oa2_Ice_check_FadeToDisappear(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_43, SHEIK_ACTION_44);
}

void En_Oa2_Ice_Actor_main_wait(EnXc* this, PlayState* play) {
    En_Oa2_Ice_check_WaitToHide(this, play);
}

void En_Oa2_Ice_Actor_main_hide(EnXc* this, PlayState* play) {
    En_Oa2_Toki_Actor_main_hide(this, play);
    En_Oa2_Ice_check_HideToGreet(this);
}

void En_Oa2_Ice_Actor_main_greet(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_greet(this, play);
    En_Oa2_Set_DemoStartPosAngle(this, play, 4);
    En_Oa2_Ice_check_GreetToTake(this);
}

void En_Oa2_Ice_Actor_main_take(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_take(this, play);
    En_Oa2_Ice_check_TakeToReady(this);
}

void En_Oa2_Ice_Actor_main_ready(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_ready(this, play);
    En_Oa2_Ice_check_ReadyToPlay(this);
}

void En_Oa2_Ice_Actor_main_play(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_play(this, play);
    En_Oa2_Ice_check_PlayToPlay_stop(this);
}

void En_Oa2_Ice_Actor_main_play_stop(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_play_stop(this, play);
    En_Oa2_Ice_check_Play_stopToPlay_end(this);
}

void En_Oa2_Ice_Actor_main_play_end(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_play_end(this, play);
    En_Oa2_Ice_check_Play_endToPut(this);
}

void En_Oa2_Ice_Actor_main_put(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_put(this, play);
    En_Oa2_Ice_check_PutToGoodby(this);
}

void En_Oa2_Ice_Actor_main_goodby(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_goodby(this, play);
    En_Oa2_Ice_check_GoodbyToAway_accel(this);
}

void En_Oa2_Ice_Actor_main_away_accel(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_away_accel(this, play);
    En_Oa2_Ice_check_Away_accelToAway_move(this);
}

void En_Oa2_Ice_Actor_main_away_move(EnXc* this, PlayState* play) {
    En_Oa2_Movement_Away_move(this);
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Set_WalkSound(this, play);
    En_Oa2_Ice_check_Away_moveToAway_brake(this);
}

void En_Oa2_Ice_Actor_main_away_brake(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_away_brake(this, play);
    En_Oa2_Ice_check_Away_brakeToThrow(this);
}

void En_Oa2_Ice_Actor_main_throw(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_throw(this, play);
    En_Oa2_Ice_check_ThrowToFade(this);
}

void En_Oa2_Ice_Actor_main_fade(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_fade(this, play);
    En_Oa2_Ice_check_FadeToDisappear(this);
}

void En_Oa2_Ice_Actor_main_disappear(EnXc* this, PlayState* play) {
}
