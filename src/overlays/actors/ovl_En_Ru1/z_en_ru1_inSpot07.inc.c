void En_Ru1_Actor_Spot07_Init(EnRu1* this, PlayState* play) {
    if (GET_EVENTCHKINF(EVENTCHKINF_37) && LINK_IS_CHILD) {
        En_Ru1_Change_Anime(this, &gRutoChildWait2Anim, 0, 0, 0);
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->action = 44;
        this->drawConfig = 1;
    } else {
        Actor_delete(&this->actor);
    }
}

s32 En_Ru1_Spot07_SetTalk(EnRu1* this, PlayState* play) {
    if (!Actor_talk_check(&this->actor, play)) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        this->actor.textId = get_mask_message(play, MASK_REACTION_SET_RUTO);
        if (this->actor.textId == 0) {
            this->actor.textId = 0x402C;
        }
        Actor_talk_request(&this->actor, play);
        return false;
    }
    return true;
}

s32 En_Ru1_Spot07_ConversationEnd(EnRu1* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        return true;
    }
    return false;
}

void En_Ru1_Spot07_check_StandToGreet(EnRu1* this, PlayState* play, s32 cond) {
    if (cond) {
        this->action = 45;
    }
}

void En_Ru1_Spot07_check_GreetToStand(EnRu1* this, PlayState* play) {
    if (En_Ru1_Spot07_ConversationEnd(this, play)) {
        this->action = 44;
    }
}

void En_Ru1_Spot07_main_stand(EnRu1* this, PlayState* play) {
    En_Ru1_Calc_turn_link(this, play);
    En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_Option_Excute_Corect_forStand(this, play);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Spot07_check_StandToGreet(this, play, En_Ru1_Spot07_SetTalk(this, play));
}

void En_Ru1_Spot07_main_greet(EnRu1* this, PlayState* play) {
    En_Ru1_Calc_turn_link2(this, play);
    En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Spot07_check_GreetToStand(this, play);
}
