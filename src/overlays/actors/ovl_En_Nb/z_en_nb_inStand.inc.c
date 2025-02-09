void EnNb_CrawlspaceSpawnCheck(EnNb* this, PlayState* play) {
    if (!GET_EVENTCHKINF(EVENTCHKINF_95) && LINK_IS_CHILD) {
        EnNb_UpdatePath(this, play);

        // looking into crawlspace
        if (!GET_EVENTCHKINF(EVENTCHKINF_94)) {
            EnNb_SetCurrentAnim(this, &gNabooruKneeingAtCrawlspaceAnim, 0, 0.0f, 0);
            this->action = NB_CROUCH_CRAWLSPACE;
            this->drawMode = NB_DRAW_DEFAULT;
        } else {
            s32 pad;

            EnNb_SetCurrentAnim(this, &gNabooruStandingHandsOnHipsAnim, 0, 0.0f, 0);
            this->headTurnFlag = 1;
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
            this->actor.world.pos = this->finalPos;
            this->action = NB_IDLE_AFTER_TALK;
            this->drawMode = NB_DRAW_DEFAULT;
        }
    } else {
        Actor_Kill(&this->actor);
    }
}

void func_80AB359C(EnNb* this) {
    PosRot* world = &this->actor.world;
    Vec3f* initialPos = &this->initialPos;
    Vec3f* finalPos = &this->finalPos;
    f32 lerp;
    u16 temp_t1;
    s16 temp_2;

    this->movementTimer++;
    temp_2 = kREG(17);
    temp_t1 = temp_2;
    temp_t1 += 25;

    if (temp_t1 >= this->movementTimer) {
        lerp = Environment_LerpWeightAccelDecel(temp_t1, 0, this->movementTimer, 3, 3);

        world->pos.x = initialPos->x + (lerp * (finalPos->x - initialPos->x));
        world->pos.y = initialPos->y + (lerp * (finalPos->y - initialPos->y));
        world->pos.z = initialPos->z + (lerp * (finalPos->z - initialPos->z));
    }
}

void EnNb_SetNoticeSFX(EnNb* this) {
    Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_VO_NB_NOTICE);
}

s32 EnNb_GetNoticedStatus(EnNb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 playerX = player->actor.world.pos.x;
    f32 playerZ = player->actor.world.pos.z;
    f32 thisX = this->actor.world.pos.x;
    f32 thisZ = this->actor.world.pos.z;

    if (SQ(playerX - thisX) + SQ(playerZ - thisZ) < SQ(80.0f)) {
        return true;
    } else {
        return false;
    }
}

void func_80AB36DC(EnNb* this, PlayState* play) {
    u16 moveTime = this->movementTimer;

    if ((((u16)((u16)(kREG(17) + 25) - 4))) > moveTime) {
        s16 invScale = 4 - moveTime;

        if (invScale > 0) {
            Math_SmoothStepToS(&this->actor.shape.rot.y, this->pathYaw, invScale, 6200, 100);
        }
    } else {
        s16 invScale = (u16)(kREG(17) + 25) - moveTime;

        if (invScale > 0) {
            Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, invScale, 6200, 100);
        }
    }
}

void EnNb_CheckNoticed(EnNb* this, PlayState* play) {
    if (EnNb_GetNoticedStatus(this, play)) {
        EnNb_SetCurrentAnim(this, &gNabooruStandingToWalkingTransitionAnim, 2, -8.0f, 0);
        this->action = NB_NOTICE_PLAYER;
        EnNb_SetNoticeSFX(this);
    }
}

void EnNb_SetupIdleCrawlspace(EnNb* this, s32 animFinished) {
    if (animFinished) {
        EnNb_SetCurrentAnim(this, &gNabooruStandingHandsOnHipsAnim, 0, -8.0f, 0);
        this->headTurnFlag = 1;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        this->action = NB_IDLE_CRAWLSPACE;
    }
}

void func_80AB3838(EnNb* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, play)) {
        s32 pad;

        this->action = NB_IN_DIALOG;
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;

        if (!GET_INFTABLE(INFTABLE_16C)) {
            this->actor.textId = 0x601D;
        } else {
            this->actor.textId = 0x6024;
        }

        Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
    }
}

void EnNb_SetupPathMovement(EnNb* this, PlayState* play) {
    EnNb_SetCurrentAnim(this, &gNabooruStandingToWalkingTransitionAnim, 2, -8.0f, 0);
    SET_EVENTCHKINF(EVENTCHKINF_94);
    this->action = NB_IN_PATH;
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
}

void EnNb_SetTextIdAsChild(EnNb* this, PlayState* play) {
    s32 pad;
    u8 choiceIndex;
    s32 pad1;
    u16 textId;

    textId = this->actor.textId;

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (textId == 0x6025) {
            EnNb_SetupPathMovement(this, play);
        } else {
            if (textId == 0x6027) {
                SET_INFTABLE(INFTABLE_16C);
            }
            this->action = NB_IDLE_CRAWLSPACE;
        }
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    } else if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        choiceIndex = play->msgCtx.choiceIndex;

        if (textId == 0x601D) {
            switch (choiceIndex) {
                case 0:
                    this->actor.textId = 0x601E;
                    break;
                case 1:
                    this->actor.textId = 0x601F;
                    break;
                default:
                    this->actor.textId = 0x6020;
            }
        } else if (textId == 0x6020) {
            switch (choiceIndex) {
                case 0:
                    this->actor.textId = 0x6021;
                    break;
                default:
                    this->actor.textId = 0x6022;
                    break;
            }
        } else {
            switch (choiceIndex) {
                case 0:
                    this->actor.textId = 0x6025;
                    break;
                default:
                    this->actor.textId = 0x6027;
                    break;
            }
        }

        Message_ContinueTextbox(play, this->actor.textId);
    }
}

void func_80AB3A7C(EnNb* this, PlayState* play, s32 animFinished) {
    u16 movementTimer = this->movementTimer;

    if ((u16)(kREG(17) + 25) > movementTimer) {
        if (animFinished) {
            EnNb_SetCurrentAnim(this, &gNabooruWalkingAnim, 0, 0.0f, 0);
        }
    } else {
        EnNb_SetCurrentAnim(this, &gNabooruStandingHandsOnHipsAnim, 0, -8.0f, 0);
        this->action = NB_IDLE_AFTER_TALK;
    }
}

void func_80AB3B04(EnNb* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, play)) {
        s32 pad;

        this->action = NB_ACTION_30;
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        this->actor.textId = MaskReaction_GetTextId(play, MASK_REACTION_SET_NABOORU);

        if (this->actor.textId == 0) {
            this->actor.textId = 0x6026;
        }

        Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
    }
}

void func_80AB3B7C(EnNb* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->action = NB_IDLE_AFTER_TALK;
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    }
}

void EnNb_WaitForNotice(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateCollider(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_CheckNoticed(this, play);
}

void EnNb_StandUpAfterNotice(EnNb* this, PlayState* play) {
    s32 animFinished;

    func_80AB1284(this, play);
    EnNb_UpdateCollider(this, play);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_SetupIdleCrawlspace(this, animFinished);
}

void EnNb_BlockCrawlspace(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateCollider(this, play);
    func_80AB0FBC(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    func_80AB3838(this, play);
}

void EnNb_InitCrawlspaceDialogue(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateCollider(this, play);
    func_80AB0FBC(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_SetTextIdAsChild(this, play);
}

void EnNb_FollowPath(EnNb* this, PlayState* play) {
    s32 animFinished;

    func_80AB359C(this);
    func_80AB1284(this, play);
    EnNb_UpdateCollider(this, play);
    func_80AB36DC(this, play);
    func_80AB10C4(this);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    func_80AB3A7C(this, play, animFinished);
}

void func_80AB3DB0(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateCollider(this, play);
    func_80AB0FBC(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    func_80AB3B04(this, play);
}

void func_80AB3E10(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateCollider(this, play);
    func_80AB1040(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    func_80AB3B7C(this, play);
}
