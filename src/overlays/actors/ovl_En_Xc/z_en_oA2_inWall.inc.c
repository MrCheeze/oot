void EnXc_InitTempleOfTime(EnXc* this, PlayState* play) {
    if (LINK_IS_ADULT) {
        if (!GET_EVENTCHKINF(EVENTCHKINF_C5)) {
            SET_EVENTCHKINF(EVENTCHKINF_C5);
            play->csCtx.script = SEGMENTED_TO_VIRTUAL(gTempleOfTimeFirstAdultCs);
            gSaveContext.cutsceneTrigger = 1;
            func_80B3EBF0(this, play);
        } else if (!GET_EVENTCHKINF(EVENTCHKINF_55) && GET_EVENTCHKINF(EVENTCHKINF_48)) {
            SET_EVENTCHKINF(EVENTCHKINF_55);
            Item_Give(play, ITEM_SONG_PRELUDE);
            play->csCtx.script = SEGMENTED_TO_VIRTUAL(gTempleOfTimePreludeCs);
            gSaveContext.cutsceneTrigger = 1;
            this->action = SHEIK_ACTION_30;
        } else if (!GET_EVENTCHKINF(EVENTCHKINF_55)) {
            func_80B3C9EC(this);
        } else {
            Actor_Kill(&this->actor);
        }
    } else {
        Actor_Kill(&this->actor);
    }
}

void EnXc_SetupDialogueAction(EnXc* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, play)) {
        s32 pad;

        this->action = SHEIK_ACTION_IN_DIALOGUE;
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        if (INV_CONTENT(ITEM_HOOKSHOT) != ITEM_NONE) {
            this->actor.textId = 0x7010;
        } else {
            this->actor.textId = 0x700F;
        }
        Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
    }
}

void func_80B41798(EnXc* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->action = SHEIK_ACTION_BLOCK_PEDESTAL;
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    }
}

void EnXc_BlockingPedestalAction(EnXc* this, PlayState* play) {
    EnXc_BgCheck(this, play);
    EnXc_UpdateCollider(&this->actor, play);
    EnXc_CalculateHeadTurn(this, play);
    EnXc_AnimIsFinished(this);
    EnXc_SetEyePattern(this);
    EnXc_SetupDialogueAction(this, play);
}

void EnXc_ActionFunc80(EnXc* this, PlayState* play) {
    EnXc_BgCheck(this, play);
    EnXc_UpdateCollider(&this->actor, play);
    EnXc_CalculateHeadTurn(this, play);
    EnXc_AnimIsFinished(this);
    EnXc_SetEyePattern(this);
    func_80B41798(this, play);
}
