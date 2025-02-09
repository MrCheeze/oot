void En_Nb_Stand_Init(EnNb* this, PlayState* play) {
    if (!GET_EVENTCHKINF(EVENTCHKINF_95) && LINK_IS_CHILD) {
        En_Nb_Init_path_info(this, play);

        // looking into crawlspace
        if (!GET_EVENTCHKINF(EVENTCHKINF_94)) {
            En_Nb_Change_Anime(this, &gNabooruKneeingAtCrawlspaceAnim, 0, 0.0f, 0);
            this->action = NB_CROUCH_CRAWLSPACE;
            this->drawMode = NB_DRAW_DEFAULT;
        } else {
            s32 pad;

            En_Nb_Change_Anime(this, &gNabooruStandingHandsOnHipsAnim, 0, 0.0f, 0);
            this->headTurnFlag = 1;
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
            this->actor.world.pos = this->finalPos;
            this->action = NB_IDLE_AFTER_TALK;
            this->drawMode = NB_DRAW_DEFAULT;
        }
    } else {
        Actor_delete(&this->actor);
    }
}

void En_Nb_inStand_Movement(EnNb* this) {
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
        lerp = get_parcent_forAccelBrake(temp_t1, 0, this->movementTimer, 3, 3);

        world->pos.x = initialPos->x + (lerp * (finalPos->x - initialPos->x));
        world->pos.y = initialPos->y + (lerp * (finalPos->y - initialPos->y));
        world->pos.z = initialPos->z + (lerp * (finalPos->z - initialPos->z));
    }
}

void En_Nb_inStand_Set_NoticeSound(EnNb* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_NB_NOTICE);
}

s32 En_Nb_inStand_Search_Link(EnNb* this, PlayState* play) {
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

void En_Nb_inStand_SetAngle_forMoving(EnNb* this, PlayState* play) {
    u16 moveTime = this->movementTimer;

    if ((((u16)((u16)(kREG(17) + 25) - 4))) > moveTime) {
        s16 invScale = 4 - moveTime;

        if (invScale > 0) {
            add_calc_short_angle2(&this->actor.shape.rot.y, this->pathYaw, invScale, 6200, 100);
        }
    } else {
        s16 invScale = (u16)(kREG(17) + 25) - moveTime;

        if (invScale > 0) {
            add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.home.rot.y, invScale, 6200, 100);
        }
    }
}

void En_Nb_inStand_check_WaitToUp(EnNb* this, PlayState* play) {
    if (En_Nb_inStand_Search_Link(this, play)) {
        En_Nb_Change_Anime(this, &gNabooruStandingToWalkingTransitionAnim, 2, -8.0f, 0);
        this->action = NB_NOTICE_PLAYER;
        En_Nb_inStand_Set_NoticeSound(this);
    }
}

void En_Nb_inStand_check_UpToBlocking(EnNb* this, s32 animFinished) {
    if (animFinished) {
        En_Nb_Change_Anime(this, &gNabooruStandingHandsOnHipsAnim, 0, -8.0f, 0);
        this->headTurnFlag = 1;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        this->action = NB_IDLE_CRAWLSPACE;
    }
}

void En_Nb_inStand_check_BlockingToBlocking_greet(EnNb* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        s32 pad;

        this->action = NB_IN_DIALOG;
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;

        if (!GET_INFTABLE(INFTABLE_16C)) {
            this->actor.textId = 0x601D;
        } else {
            this->actor.textId = 0x6024;
        }

        Actor_talk_request(&this->actor, play);
    }
}

void En_Nb_inStand_Setup_Blocking_greetToMoving(EnNb* this, PlayState* play) {
    En_Nb_Change_Anime(this, &gNabooruStandingToWalkingTransitionAnim, 2, -8.0f, 0);
    SET_EVENTCHKINF(EVENTCHKINF_94);
    this->action = NB_IN_PATH;
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
}

void En_Nb_inStand_check_Blocking_greetToBranch(EnNb* this, PlayState* play) {
    s32 pad;
    u8 choiceIndex;
    s32 pad1;
    u16 textId;

    textId = this->actor.textId;

    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (textId == 0x6025) {
            En_Nb_inStand_Setup_Blocking_greetToMoving(this, play);
        } else {
            if (textId == 0x6027) {
                SET_INFTABLE(INFTABLE_16C);
            }
            this->action = NB_IDLE_CRAWLSPACE;
        }
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    } else if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
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

        message_set2(play, this->actor.textId);
    }
}

void En_Nb_inStand_check_MovingToWelcome(EnNb* this, PlayState* play, s32 animFinished) {
    u16 movementTimer = this->movementTimer;

    if ((u16)(kREG(17) + 25) > movementTimer) {
        if (animFinished) {
            En_Nb_Change_Anime(this, &gNabooruWalkingAnim, 0, 0.0f, 0);
        }
    } else {
        En_Nb_Change_Anime(this, &gNabooruStandingHandsOnHipsAnim, 0, -8.0f, 0);
        this->action = NB_IDLE_AFTER_TALK;
    }
}

void En_Nb_inStand_check_WelcomeToWelcome_greet(EnNb* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        s32 pad;

        this->action = NB_ACTION_30;
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        this->actor.textId = get_mask_message(play, MASK_REACTION_SET_NABOORU);

        if (this->actor.textId == 0) {
            this->actor.textId = 0x6026;
        }

        Actor_talk_request(&this->actor, play);
    }
}

void En_Nb_inStand_check_Welcome_greetToWelcome(EnNb* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->action = NB_IDLE_AFTER_TALK;
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    }
}

void En_Nb_inStand_main_wait(EnNb* this, PlayState* play) {
    En_Nb_BGcheck(this, play);
    En_Nb_Excute_Corect(this, play);
    En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
    En_Nb_inStand_check_WaitToUp(this, play);
}

void En_Nb_inStand_main_up(EnNb* this, PlayState* play) {
    s32 animFinished;

    En_Nb_BGcheck(this, play);
    En_Nb_Excute_Corect(this, play);
    animFinished = En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
    En_Nb_inStand_check_UpToBlocking(this, animFinished);
}

void En_Nb_inStand_main_blocking(EnNb* this, PlayState* play) {
    En_Nb_BGcheck(this, play);
    En_Nb_Excute_Corect(this, play);
    En_Nb_Calc_turn_link(this, play);
    En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
    En_Nb_inStand_check_BlockingToBlocking_greet(this, play);
}

void En_Nb_inStand_main_blocking_greet(EnNb* this, PlayState* play) {
    En_Nb_BGcheck(this, play);
    En_Nb_Excute_Corect(this, play);
    En_Nb_Calc_turn_link(this, play);
    En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
    En_Nb_inStand_check_Blocking_greetToBranch(this, play);
}

void En_Nb_inStand_main_moving(EnNb* this, PlayState* play) {
    s32 animFinished;

    En_Nb_inStand_Movement(this);
    En_Nb_BGcheck(this, play);
    En_Nb_Excute_Corect(this, play);
    En_Nb_inStand_SetAngle_forMoving(this, play);
    En_Nb_Calc_turn_front(this);
    animFinished = En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
    En_Nb_inStand_check_MovingToWelcome(this, play, animFinished);
}

void En_Nb_inStand_main_welcome(EnNb* this, PlayState* play) {
    En_Nb_BGcheck(this, play);
    En_Nb_Excute_Corect(this, play);
    En_Nb_Calc_turn_link(this, play);
    En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
    En_Nb_inStand_check_WelcomeToWelcome_greet(this, play);
}

void En_Nb_inStand_main_welcome_greet(EnNb* this, PlayState* play) {
    En_Nb_BGcheck(this, play);
    En_Nb_Excute_Corect(this, play);
    En_Nb_Calc_turn_link2(this, play);
    En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
    En_Nb_inStand_check_Welcome_greetToWelcome(this, play);
}
