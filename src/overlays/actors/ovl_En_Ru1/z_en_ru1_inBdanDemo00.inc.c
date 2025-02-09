void En_Ru1_Actor_Ban00_Init(EnRu1* this, PlayState* play) {
    if (!GET_INFTABLE(INFTABLE_141)) {
        En_Ru1_Change_Anime(this, &gRutoChildWait2Anim, 0, 0, 0);
        this->action = 7;
        En_Ru1_set_mouth_Num(this, 1);
    } else if (GET_INFTABLE(INFTABLE_147) && !GET_INFTABLE(INFTABLE_140) && !GET_INFTABLE(INFTABLE_145)) {
        if (!En_Ru1_Search_Ru1(this, play)) {
            s8 actorRoom;

            En_Ru1_Change_Anime(this, &gRutoChildWait2Anim, 0, 0, 0);
            actorRoom = this->actor.room;
            this->action = 22;
            this->actor.room = -1;
            this->drawConfig = 0;
            this->roomNum1 = actorRoom;
            this->roomNum3 = actorRoom;
            this->roomNum2 = actorRoom;
        } else {
            Actor_delete(&this->actor);
        }
    } else {
        Actor_delete(&this->actor);
    }
}

void En_Ru1_Bdan00_Movement_Run_accel(EnRu1* this) {
    f32 unk_26C = this->unk_26C;

    if (unk_26C < 8.0f) {
        this->actor.speed = (((kREG(3) * 0.01f) + 2.7f) / 8.0f) * unk_26C;
    } else {
        this->actor.speed = (kREG(3) * 0.01f) + 2.7f;
    }
    this->actor.velocity.y = -1.0f;
    Actor_position_moveF(&this->actor);
}

void En_Ru1_Bdan00_Movement_Rum_move(EnRu1* this) {
    this->actor.velocity.y = -1.0f;
    Actor_position_moveF(&this->actor);
}

void En_Ru1_Bdan00_Movement_Fall_move(EnRu1* this) {
    f32* speedXZ = &this->actor.speed;
    f32* unk_26C = &this->unk_26C;

    if (this->unk_26C < 8.0f) {
        *unk_26C += 1.0f;
        *speedXZ *= (8.0f - *unk_26C) / 8.0f;
        this->actor.velocity.y = -*unk_26C * (((kREG(4) * 0.01f) + 13.0f) / 8.0f);
    } else {
        *speedXZ = 0.0f;
        this->actor.velocity.y = -((kREG(4) * 0.01f) + 13.0f);
    }
    Actor_position_moveF(&this->actor);
}

s32 En_Ru1_Bdan00_check_distance(EnRu1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 thisPosZ = this->actor.world.pos.z;
    f32 playerPosZ = player->actor.world.pos.z;

    if ((playerPosZ - thisPosZ <= 265.0f) && (player->actor.world.pos.y >= this->actor.world.pos.y)) {
        return true;
    }
    return false;
}

void En_Ru1_Bdan00_Set_WalkSound(EnRu1* this) {
    s32 pad[2];

    if (this->unk_280 == 0) {
        if (Skeleton_Info_frame_check(&this->skelAnime, 2.0f) || Skeleton_Info_frame_check(&this->skelAnime, 7.0f)) {
            Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_JABU);
        }
    }
}

void En_Ru1_Bdan00_Set_FallSound(EnRu1* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_EV_FALL_DOWN_DIRT);
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_RT_FALL);
}

void En_Ru1_Bdan00_check_fallAnimation(EnRu1* this, PlayState* play) {
    if ((En_Ru1_Check_npcdemopnt(play, 4, 3)) && (this->unk_280 == 0)) {
        Skeleton_Info2_init(&this->skelAnime, &gRutoChildBringArmsUpAnim, 1.0f, 0,
                         Si2_anime_end_frame(&gRutoChildBringArmsUpAnim), ANIMMODE_ONCE, -8.0f);
        this->unk_280 = 1;
        En_Ru1_Bdan00_Set_FallSound(this);
    }
}

void En_Ru1_Bdan00_check_WaitToHide(EnRu1* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    if ((En_Ru1_Bdan00_check_distance(this, play)) && (!Game_play_demo_mode_check(play)) &&
        (!(player->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_21))) &&
        (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {

        play->csCtx.script = En_Ru1_Data1_In_BdanDemo00;
        z_common_data.cutsceneTrigger = 1;
        player->speedXZ = 0.0f;
        this->action = 8;
    }
}

void En_Ru1_Bdan00_check_HideToRefuse(EnRu1* this, PlayState* play) {
    CsCmdActorCue* cue;
    s16 newRotY;

    if (En_Ru1_Check2_npcdemopnt(play, 1, 3)) {
        cue = play->csCtx.actorCues[3];
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;
        newRotY = cue->rot.y;
        this->actor.shape.rot.y = newRotY;
        this->actor.world.rot.y = newRotY;
        this->action = 9;
        this->drawConfig = 1;
    }
}

void En_Ru1_Bdan00_check_RefuseToTurn(EnRu1* this, PlayState* play) {
    if (En_Ru1_Check_npcdemopnt(play, 3, 3)) {
        Skeleton_Info2_init(&this->skelAnime, &gRutoChildTurnAroundAnim, 1.0f, 0,
                         Si2_anime_end_frame(&gRutoChildTurnAroundAnim), ANIMMODE_ONCE, -8.0f);
        this->action = 10;
    }
}

void En_Ru1_Bdan00_check_TurnToRun_accel(EnRu1* this, UNK_TYPE arg1) {
    if (arg1 != 0) {
        Skeleton_Info2_init(&this->skelAnime, &gRutoChildWalkAnim, 1.0f, 0, Si2_anime_end_frame(&gRutoChildWalkAnim),
                         ANIMMODE_LOOP, -8.0f);
        this->actor.world.rot.y += 0x8000;
        this->action = 0xB;
        this->unk_26C = 0.0f;
    }
}

void En_Ru1_Bdan00_check_Run_accelToRun_move(EnRu1* this) {
    this->unk_26C += 1.0f;
    if (this->unk_26C >= 8.0f) {
        this->action = 12;
        this->unk_26C = 0.0f;
        this->actor.velocity.y = -1.0f;
    }
}

void En_Ru1_Bdan00_check_Run_moveToStep(EnRu1* this) {
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        s32 pad;

        this->action = 13;
        this->unk_26C = 0.0f;
        this->actor.velocity.y = 0.0f;
    }
}

void En_Ru1_Bdan00_check_StepToDisappear(EnRu1* this, PlayState* play) {
    if (En_Ru1_Check_npcdemopnt(play, 5, 3)) {
        SET_INFTABLE(INFTABLE_141);
        this->action = 14;
    }
}

void En_Ru1_Bdan00_Actor_main_wait(EnRu1* this, PlayState* play) {
    En_Ru1_Bdan00_check_WaitToHide(this, play);
}

void En_Ru1_Bdan00_Actor_main_hide(EnRu1* this, PlayState* play) {
    En_Ru1_Bdan00_check_HideToRefuse(this, play);
}

void En_Ru1_Bdan00_Actor_main_refuse(EnRu1* this, PlayState* play) {
    En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Bdan00_check_RefuseToTurn(this, play);
}

void En_Ru1_Bdan00_Actor_main_turn(EnRu1* this, PlayState* play) {
    s32 something;

    something = En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Bdan00_check_TurnToRun_accel(this, something);
}

void En_Ru1_Bdan00_Actor_main_run_accel(EnRu1* this, PlayState* play) {
    En_Ru1_Bdan00_Movement_Run_accel(this);
    En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Bdan00_Set_WalkSound(this);
    En_Ru1_Bdan00_check_Run_accelToRun_move(this);
}

void En_Ru1_Bdan00_Actor_main_run_move(EnRu1* this, PlayState* play) {
    En_Ru1_Bdan00_Movement_Rum_move(this);
    En_Ru1_Bdan00_check_fallAnimation(this, play);
    En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Bdan00_Set_WalkSound(this);
    En_Ru1_Bdan00_check_Run_moveToStep(this);
}

void En_Ru1_Bdan00_Actor_main_step(EnRu1* this, PlayState* play) {
    En_Ru1_Bdan00_Movement_Fall_move(this);
    En_Ru1_Bdan00_check_fallAnimation(this, play);
    En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Bdan00_Set_WalkSound(this);
    En_Ru1_Bdan00_check_StepToDisappear(this, play);
}

void En_Ru1_Bdan00_Actor_main_disappear(EnRu1* this, PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        Actor_delete(&this->actor);
    }
}
