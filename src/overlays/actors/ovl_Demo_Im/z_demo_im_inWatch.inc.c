void Demo_Im_Watch_Init(DemoIm* this, PlayState* play) {
    Demo_Im_Change_Anime(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->action = 18;
    this->actor.shape.shadowAlpha = 0;
}

s32 Demo_Im_Watch_check_appear(DemoIm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 playerPosX = player->actor.world.pos.x;
    f32 thisPosX = this->actor.world.pos.x;

    if ((thisPosX - (kREG(16) + 30.0f) > playerPosX) && !(this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME)) {
        return true;
    } else {
        return false;
    }
}

s32 Demo_Im_Watch_check_demo(DemoIm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 playerPosX = player->actor.world.pos.x;
    f32 thisPosX = this->actor.world.pos.x;

    if ((thisPosX - (kREG(17) + 130.0f) < playerPosX) && (!Game_play_demo_mode_check(play))) {
        return true;
    } else {
        return false;
    }
}

s32 Demo_Im_SetTalk(DemoIm* this, PlayState* play) {
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
    if (!Actor_talk_check(&this->actor, play)) {
        this->actor.textId = 0x708E;
        Actor_talk_request(&this->actor, play);
    } else {
        return true;
    }
    return false;
}

void Demo_Im_WarpLonly(PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        Player* player = GET_PLAYER(play);

        play->nextEntranceIndex = ENTR_HYRULE_FIELD_0;
        play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_BLACK, TCS_FAST);
        play->transitionTrigger = TRANS_TRIGGER_START;
        player_demo_mode_set(play, &player->actor, PLAYER_CSACTION_8);
    }
}

void Demo_Im_Watch_check_wait_warpTostand_warp(DemoIm* this, PlayState* play) {
    if (Demo_Im_Watch_check_appear(this, play)) {
        this->action = 21;
        this->drawConfig = 1;
        this->unk_280 = 1;
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void Demo_Im_Watch_check_stand_warpTobranch_warp(DemoIm* this, s32 arg1) {
    if (arg1 != 0) {
        this->action = 22;
    }
}

void Demo_Im_Watch_check_wait_ocarinaTostand_ocarina(DemoIm* this, PlayState* play) {
    if (GET_EVENTCHKINF(EVENTCHKINF_40)) {
        this->action = 24;
        this->drawConfig = 1;
        this->unk_280 = 1;
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void Demo_Im_Watch_check_stand_ocarinaTodemo_ocarina(DemoIm* this, PlayState* play) {
    if (Demo_Im_Watch_check_demo(this, play)) {
        s32 pad;

        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gZeldasCourtyardLullabyCs);
        z_common_data.cutsceneTrigger = 1;
        SET_EVENTCHKINF(EVENTCHKINF_59);
        item_get_setting(play, ITEM_SONG_LULLABY);
        Demo_Im_Ocarina_setup_Demo_Wait(this);
    }
}

void Demo_Im_Watch_check_wait_spot00Tostand_spot00(DemoIm* this) {
    if (GET_EVENTCHKINF(EVENTCHKINF_40)) {
        this->action = 26;
        this->drawConfig = 1;
        this->unk_280 = 1;
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void Demo_Im_Watch_check_stand_spot00Todemo_spot00(DemoIm* this, PlayState* play) {
    if (Demo_Im_Watch_check_demo(this, play)) {
        SET_EVENTCHKINF(EVENTCHKINF_4C);
        this->action = 19;
    }
}

void Demo_Im_Watch_main_wait(DemoIm* this, PlayState* play) {
#if DEBUG_FEATURES
    if (z_common_data.sceneLayer == 6) {
        this->action = 19;
        this->drawConfig = 1;
        return;
    }
#endif

    if (GET_EVENTCHKINF(EVENTCHKINF_80)) {
        Actor_delete(&this->actor);
    } else if (!GET_EVENTCHKINF(EVENTCHKINF_59)) {
        this->action = 23;
    } else {
        this->action = 20;
    }
}

void Demo_Im_Watch_main_stand(DemoIm* this, PlayState* play) {
    Demo_Im_BGcheck(this, play);
    Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_Calc_turn_link(this, play);
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
}

void Demo_Im_Watch_main_wait_warp(DemoIm* this, PlayState* play) {
    Demo_Im_Watch_check_wait_warpTostand_warp(this, play);
}

void Demo_Im_Watch_main_stand_warp(DemoIm* this, PlayState* play) {
    Demo_Im_BGcheck(this, play);
    Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_Calc_turn_link(this, play);
    Demo_Im_Excute_Corect_forStand(this, play);
    Demo_Im_Watch_check_stand_warpTobranch_warp(this, Demo_Im_SetTalk(this, play));
}

void Demo_Im_Watch_main_branch_warp(DemoIm* this, PlayState* play) {
    Demo_Im_BGcheck(this, play);
    Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_Calc_turn_link3(this, play);
    Demo_Im_Excute_Corect_forStand(this, play);
    Demo_Im_WarpLonly(play);
}

void Demo_Im_Watch_main_wait_ocarina(DemoIm* this, PlayState* play) {
    Demo_Im_Watch_check_wait_ocarinaTostand_ocarina(this, play);
}

void Demo_Im_Watch_main_stand_ocarina(DemoIm* this, PlayState* play) {
    Demo_Im_BGcheck(this, play);
    Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_Calc_turn_link(this, play);
    Demo_Im_Excute_Corect_forStand(this, play);
    Demo_Im_Watch_check_stand_ocarinaTodemo_ocarina(this, play);
}

void Demo_Im_Watch_main_wait_spot00(DemoIm* this, PlayState* play) {
    Demo_Im_Watch_check_wait_spot00Tostand_spot00(this);
}

void Demo_Im_Watch_main_stand_spot00(DemoIm* this, PlayState* play) {
    Demo_Im_BGcheck(this, play);
    Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_Calc_turn_link(this, play);
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    Demo_Im_Excute_Corect_forStand(this, play);
    Demo_Im_Watch_check_stand_spot00Todemo_spot00(this, play);
}
