void func_809869B0(DemoIm* this, PlayState* play) {
    DemoIm_ChangeAnim(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->action = 18;
    this->actor.shape.shadowAlpha = 0;
}

s32 func_809869F8(DemoIm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 playerPosX = player->actor.world.pos.x;
    f32 thisPosX = this->actor.world.pos.x;

    if ((thisPosX - (kREG(16) + 30.0f) > playerPosX) && !(this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME)) {
        return true;
    } else {
        return false;
    }
}

s32 func_80986A5C(DemoIm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 playerPosX = player->actor.world.pos.x;
    f32 thisPosX = this->actor.world.pos.x;

    if ((thisPosX - (kREG(17) + 130.0f) < playerPosX) && (!Play_InCsMode(play))) {
        return true;
    } else {
        return false;
    }
}

s32 func_80986AD0(DemoIm* this, PlayState* play) {
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
    if (!Actor_TalkOfferAccepted(&this->actor, play)) {
        this->actor.textId = 0x708E;
        Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
    } else {
        return true;
    }
    return false;
}

void func_80986B2C(PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        Player* player = GET_PLAYER(play);

        play->nextEntranceIndex = ENTR_HYRULE_FIELD_0;
        play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_BLACK, TCS_FAST);
        play->transitionTrigger = TRANS_TRIGGER_START;
        Player_SetCsActionWithHaltedActors(play, &player->actor, PLAYER_CSACTION_8);
    }
}

void func_80986BA0(DemoIm* this, PlayState* play) {
    if (func_809869F8(this, play)) {
        this->action = 21;
        this->drawConfig = 1;
        this->unk_280 = 1;
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_80986BE4(DemoIm* this, s32 arg1) {
    if (arg1 != 0) {
        this->action = 22;
    }
}

void func_80986BF8(DemoIm* this, PlayState* play) {
    if (GET_EVENTCHKINF(EVENTCHKINF_40)) {
        this->action = 24;
        this->drawConfig = 1;
        this->unk_280 = 1;
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_80986C30(DemoIm* this, PlayState* play) {
    if (func_80986A5C(this, play)) {
        s32 pad;

        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gZeldasCourtyardLullabyCs);
        gSaveContext.cutsceneTrigger = 1;
        SET_EVENTCHKINF(EVENTCHKINF_59);
        Item_Give(play, ITEM_SONG_LULLABY);
        func_80985F54(this);
    }
}

void func_80986CC8(DemoIm* this) {
    if (GET_EVENTCHKINF(EVENTCHKINF_40)) {
        this->action = 26;
        this->drawConfig = 1;
        this->unk_280 = 1;
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_80986CFC(DemoIm* this, PlayState* play) {
    if (func_80986A5C(this, play)) {
        SET_EVENTCHKINF(EVENTCHKINF_4C);
        this->action = 19;
    }
}

void func_80986D40(DemoIm* this, PlayState* play) {
#if DEBUG_FEATURES
    if (gSaveContext.sceneLayer == 6) {
        this->action = 19;
        this->drawConfig = 1;
        return;
    }
#endif

    if (GET_EVENTCHKINF(EVENTCHKINF_80)) {
        Actor_Kill(&this->actor);
    } else if (!GET_EVENTCHKINF(EVENTCHKINF_59)) {
        this->action = 23;
    } else {
        this->action = 20;
    }
}

void func_80986DC8(DemoIm* this, PlayState* play) {
    DemoIm_UpdateBgCheckInfo(this, play);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_80984E58(this, play);
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
}

void func_80986E20(DemoIm* this, PlayState* play) {
    func_80986BA0(this, play);
}

void func_80986E40(DemoIm* this, PlayState* play) {
    DemoIm_UpdateBgCheckInfo(this, play);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_80984E58(this, play);
    DemoIm_UpdateCollider(this, play);
    func_80986BE4(this, func_80986AD0(this, play));
}

void func_80986EAC(DemoIm* this, PlayState* play) {
    DemoIm_UpdateBgCheckInfo(this, play);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_80984F94(this, play);
    DemoIm_UpdateCollider(this, play);
    func_80986B2C(play);
}

void func_80986F08(DemoIm* this, PlayState* play) {
    func_80986BF8(this, play);
}

void func_80986F28(DemoIm* this, PlayState* play) {
    DemoIm_UpdateBgCheckInfo(this, play);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_80984E58(this, play);
    DemoIm_UpdateCollider(this, play);
    func_80986C30(this, play);
}

void func_80986F88(DemoIm* this, PlayState* play) {
    func_80986CC8(this);
}

void func_80986FA8(DemoIm* this, PlayState* play) {
    DemoIm_UpdateBgCheckInfo(this, play);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_80984E58(this, play);
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    DemoIm_UpdateCollider(this, play);
    func_80986CFC(this, play);
}
