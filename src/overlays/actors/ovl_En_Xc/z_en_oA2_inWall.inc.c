void En_Oa2_Actor_Wall_Init(EnXc* this, PlayState* play) {
    if (LINK_IS_ADULT) {
        if (!GET_EVENTCHKINF(EVENTCHKINF_C5)) {
            SET_EVENTCHKINF(EVENTCHKINF_C5);
            play->csCtx.script = SEGMENTED_TO_VIRTUAL(gTempleOfTimeFirstAdultCs);
            z_common_data.cutsceneTrigger = 1;
            En_Oa2_Actor_Tokinoma_Init(this, play);
        } else if (!GET_EVENTCHKINF(EVENTCHKINF_55) && GET_EVENTCHKINF(EVENTCHKINF_48)) {
            SET_EVENTCHKINF(EVENTCHKINF_55);
            item_get_setting(play, ITEM_SONG_PRELUDE);
            play->csCtx.script = SEGMENTED_TO_VIRTUAL(gTempleOfTimePreludeCs);
            z_common_data.cutsceneTrigger = 1;
            this->action = SHEIK_ACTION_30;
        } else if (!GET_EVENTCHKINF(EVENTCHKINF_55)) {
            En_Oa2_Setup_TokinomaToWall(this);
        } else {
            Actor_delete(&this->actor);
        }
    } else {
        Actor_delete(&this->actor);
    }
}

void En_Oa2_inStand_check_BlockingToGreeting(EnXc* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        s32 pad;

        this->action = SHEIK_ACTION_IN_DIALOGUE;
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        if (INV_CONTENT(ITEM_HOOKSHOT) != ITEM_NONE) {
            this->actor.textId = 0x7010;
        } else {
            this->actor.textId = 0x700F;
        }
        Actor_talk_request(&this->actor, play);
    }
}

void En_Oa2_inStand_check_GreetingToBlocking(EnXc* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->action = SHEIK_ACTION_BLOCK_PEDESTAL;
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    }
}

void En_Oa2_Wall_main_blocking(EnXc* this, PlayState* play) {
    En_Oa2_BGcheck(this, play);
    En_Oa2_Excute_Corect(&this->actor, play);
    En_Oa2_Calc_turn_link(this, play);
    En_Oa2_Animation_Basic(this);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_inStand_check_BlockingToGreeting(this, play);
}

void En_Oa2_Wall_main_greeting(EnXc* this, PlayState* play) {
    En_Oa2_BGcheck(this, play);
    En_Oa2_Excute_Corect(&this->actor, play);
    En_Oa2_Calc_turn_link(this, play);
    En_Oa2_Animation_Basic(this);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_inStand_check_GreetingToBlocking(this, play);
}
