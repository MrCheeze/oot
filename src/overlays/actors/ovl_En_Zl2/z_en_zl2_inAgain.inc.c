void En_Zl2_Actor_inAgain_Init(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_00B5FC, 0, 0.0f, 0);
    this->action = 1;
}

void En_Zl2_inAgain_Set_WalkSound(EnZl2* this) {
    if (Skeleton_Info_frame_check(&this->skelAnime, 14.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_STONE);
    }
}

void En_Zl2_inAgain_Set_QuakeSound(PlayState* play) {
    if ((play->csCtx.curFrame >= 830) && (play->csCtx.curFrame < 1081)) {
        Na_StartFixSe_F(NA_SE_EV_EARTHQUAKE - SFX_FLAG);
    }
}

void En_Zl2_inAgain_Set_HeavenSound(EnZl2* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_EV_GOTO_HEAVEN - SFX_FLAG);
}

void En_Zl2_inAgain_Set_GanonSound(EnZl2* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_EN_GANON_LAUGH);
}

void En_Zl2_inAgain_Set_SurpriseSound(EnZl2* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_Z1_SURPRISE);
}

void En_Zl2_inAgain_Set_PainSound(EnZl2* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_Z1_PAIN);
}

void En_Zl2_inAgain_Set_Cry0Sound(EnZl2* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_Z1_CRY_0);
}

void Birth_Effect_Medal_In_En_Zl2_inAgain(EnZl2* this, PlayState* play) {
    Player* player;
    f32 posX;
    f32 posY;
    f32 posZ;

    if (this->unk_244 == 0) {
        player = GET_PLAYER(play);
        posX = player->actor.world.pos.x;
        posY = player->actor.world.pos.y + 80.0f;
        posZ = player->actor.world.pos.z;
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0, 0x17);
        item_get_setting(play, ITEM_ARROW_LIGHT);
        this->unk_244 = 1;
    }
}

void Birth_Effect_Light_In_En_Zl2_inAgain(EnZl2* this, PlayState* play) {
    f32 posX;
    f32 posY;
    f32 posZ;

    if (this->unk_250 == 0) {
        posX = this->actor.world.pos.x;
        posY = this->actor.world.pos.y;
        posZ = this->actor.world.pos.z;

        Actor_info_make_actor(&play->actorCtx, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_YELLOW);
        this->unk_250 = 1;
    }
}

void Birth_Cryst_In_En_Zl2_inAgain(EnZl2* this, PlayState* play) {
    f32 posX;
    f32 posY;
    f32 posZ;

    if (this->unk_248 == 0) {
        posX = this->actor.world.pos.x;
        posY = this->actor.world.pos.y + (kREG(5) + -26.0f);
        posZ = this->actor.world.pos.z;

        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0x4000, 0,
                           WARP_PURPLE_CRYSTAL);
        this->unk_248 = 1;
    }
}

void SetPos_Cryst_In_En_Zl2_inAgain(EnZl2* this) {
    Actor* child = this->actor.child;

    if (child != NULL) {
        child->world.pos.x = this->actor.world.pos.x;
        child->world.pos.y = this->actor.world.pos.y + (kREG(5) + -26.0f);
        child->world.pos.z = this->actor.world.pos.z;
    }
}

void En_Zl2_Movement_Approach(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = En_Zl2_Get_npcdemopnt(play, 0);
    Vec3f* worldPos = &this->actor.world.pos;
    f32 startX;
    f32 startY;
    f32 startZ;
    f32 endX;
    f32 endY;
    f32 endZ;
    f32 lerp;

    if (cue != NULL) {
        lerp = get_parcent_forAccelBrake(cue->endFrame, cue->startFrame, play->csCtx.curFrame, 8, 8);

        startX = cue->startPos.x;
        startY = cue->startPos.y;
        startZ = cue->startPos.z;

        endX = cue->endPos.x;
        endY = cue->endPos.y;
        endZ = cue->endPos.z;

        worldPos->x = ((endX - startX) * lerp) + startX;
        worldPos->y = ((endY - startY) * lerp) + startY;
        worldPos->z = ((endZ - startZ) * lerp) + startZ;
    }
}

void En_Zl2_inAgain_SetCrystAlpha(EnZl2* this, s32 alpha) {
    if (this->actor.child != NULL) {
        ((DoorWarp1*)this->actor.child)->crystalAlpha = alpha;
    }
}

void En_Zl2_inAgain_SetAlpha(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = En_Zl2_Get_npcdemopnt(play, 0);

    if (cue != NULL) {
        this->actor.shape.shadowAlpha = this->alpha =
            (1.0f - get_parcent(cue->endFrame, cue->startFrame, play->csCtx.curFrame)) * 255.0f;
        En_Zl2_inAgain_SetCrystAlpha(this, this->alpha);
    }
}

void En_Zl2_inAgain_setup_Wait(EnZl2* this, PlayState* play) {
    this->action = 1;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void En_Zl2_inAgain_setup_Greet(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = En_Zl2_Get_npcdemopnt(play, 0);

    this->actor.world.pos.x = cue->startPos.x;
    this->actor.world.pos.y = cue->startPos.y;
    this->actor.world.pos.z = cue->startPos.z;

    this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;

    this->actor.shape.shadowAlpha = 255;
    this->action = 2;
    this->drawConfig = 1;
}

void En_Zl2_inAgain_setup_Approach(EnZl2* this, PlayState* play) {
    s32 pad[2];
    ActorShape* shape = &this->actor.shape;
    CsCmdActorCue* cue = En_Zl2_Get_npcdemopnt(play, 0);
    f32 cueXDelta;
    f32 cueZDelta;

    cueXDelta = cue->endPos.x - cue->startPos.x;
    cueZDelta = cue->endPos.z - cue->startPos.z;

    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_0003BC, 0, -12.0f, 0);
    this->action = 3;
    this->drawConfig = 1;
    this->unk_23C = 0.0f;
    shape->shadowAlpha = 255;
    this->actor.world.rot.y = shape->rot.y = RAD_TO_BINANG(fatan2(cueXDelta, cueZDelta));
}

void En_Zl2_inAgain_check_ApproachToConfess(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = En_Zl2_Get_npcdemopnt(play, 0);

    if ((cue != NULL) && (play->csCtx.curFrame >= cue->endFrame)) {
        this->action = 4;
    }
}

void En_Zl2_inAgain_setup_Gloom(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_0022D0, 2, -8.0f, 0);
    this->action = 5;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void En_Zl2_inAgain_check_GloomToWaver(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_002750, 0, 0.0f, 0);
        this->action = 6;
        this->drawConfig = 1;
    }
}

void En_Zl2_inAgain_setup_Forward(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_00325C, 2, -8.0f, 0);
    this->action = 7;
    this->drawConfig = 1;
    this->unk_27C = 0.0f;
    En_Zl2_set_mouth_pattern(this, 1);
    this->actor.shape.shadowAlpha = 0xFF;
}

void En_Zl2_inAgain_check_ForwardToEncourage(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_003538, 0, 0.0f, 0);
        this->action = 8;
    }
}

void En_Zl2_inAgain_setup_Handup(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_000A50, 2, -8.0f, 0);
    this->action = 9;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
}

void En_Zl2_inAgain_check_HandupToCheer(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_000EB0, 0, 0.0f, 0);
        this->action = 10;
    }
}

void En_Zl2_inAgain_setup_Give(EnZl2* this, PlayState* play) {
    Birth_Effect_Medal_In_En_Zl2_inAgain(this, play);
    this->action = 11;
}

void En_Zl2_inAgain_setup_Effect(EnZl2* this, PlayState* play) {
    Birth_Effect_Light_In_En_Zl2_inAgain(this, play);
    this->action = 12;
}

void En_Zl2_inAgain_setup_Gaze(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_00B5FC, 0, -8.0f, 0);
    this->action = 13;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
}

void En_Zl2_inAgain_setup_Turn(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_00AAD4, 2, -8.0f, 0);
    this->action = 14;
    this->drawConfig = 1;
    En_Zl2_setNum_eye_pattern(this, 4);
    En_Zl2_set_mouth_pattern(this, 2);
    this->actor.shape.shadowAlpha = 0xFF;
    En_Zl2_inAgain_Set_SurpriseSound(this);
}

void En_Zl2_inAgain_check_TurnToAround(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_00AFE0, 0, 0.0f, 0);
        this->action = 15;
    }
}

void En_Zl2_inAgain_setup_Raise(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_001670, 2, -8.0f, 0);
    this->action = 16;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    Birth_Cryst_In_En_Zl2_inAgain(this, play);
    En_Zl2_setNum_eye_pattern(this, 3);
}

void En_Zl2_inAgain_check_RaiseToAttention(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_001B48, 0, 0.0f, 0);
        this->action = 17;
    }
}

void En_Zl2_inAgain_setup_Arrest(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_002B14, 2, -8.0f, 0);
    this->action = 18;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
}

void En_Zl2_inAgain_check_ArrestToPrison(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_002F30, 0, 0.0f, 0);
        this->action = 19;
        En_Zl2_inAgain_Set_PainSound(this);
    }
}

void En_Zl2_inAgain_setup_Collapse(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_001010, 2, -8.0f, 0);
    this->action = 20;
    this->drawConfig = 1;
    En_Zl2_setNum_eye_pattern(this, 6);
    this->actor.shape.shadowAlpha = 0xFF;
}

void En_Zl2_inAgain_check_CollapseToFaint(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_0013A0, 0, 0.0f, 0);
        this->action = 21;
        En_Zl2_inAgain_Set_Cry0Sound(this);
    }
}

void En_Zl2_inAgain_setup_Fly(EnZl2* this, PlayState* play) {
    this->action = 22;
}

void En_Zl2_inAgain_setup_Fade(EnZl2* this, PlayState* play) {
    this->action = 23;
    this->drawConfig = 2;
    this->alpha = 255;
}

void En_Zl2_inAgain_check_FadeToDisappear(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = En_Zl2_Get_npcdemopnt(play, 0);

    if (cue != NULL) {
        if (play->csCtx.curFrame >= cue->endFrame) {
            this->action = 24;
            this->drawConfig = 0;
            En_Zl2_inAgain_Set_GanonSound(this);
        }
    }
}

void En_Zl2_inAgain_Check_DemoMode(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = En_Zl2_Get_npcdemopnt(play, 0);
    s32 nextCueId;
    s32 currentCueId;

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    En_Zl2_inAgain_setup_Wait(this, play);
                    break;
                case 2:
                    En_Zl2_inAgain_setup_Greet(this, play);
                    break;
                case 3:
                    En_Zl2_inAgain_setup_Approach(this, play);
                    break;
                case 4:
                    En_Zl2_inAgain_setup_Gloom(this, play);
                    break;
                case 5:
                    En_Zl2_inAgain_setup_Forward(this, play);
                    break;
                case 6:
                    En_Zl2_inAgain_setup_Handup(this, play);
                    break;
                case 7:
                    En_Zl2_inAgain_setup_Give(this, play);
                    break;
                case 8:
                    En_Zl2_inAgain_setup_Gaze(this, play);
                    break;
                case 9:
                    En_Zl2_inAgain_setup_Turn(this, play);
                    break;
                case 10:
                    En_Zl2_inAgain_setup_Raise(this, play);
                    break;
                case 11:
                    En_Zl2_inAgain_setup_Arrest(this, play);
                    break;
                case 12:
                    En_Zl2_inAgain_setup_Collapse(this, play);
                    break;
                case 13:
                    En_Zl2_inAgain_setup_Fly(this, play);
                    break;
                case 14:
                    En_Zl2_inAgain_setup_Fade(this, play);
                    break;
                case 15:
                    En_Zl2_inAgain_setup_Effect(this, play);
                    break;
                default:
                    PRINTF("En_Zl2_inAgain_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void En_Zl2_inAgain_main_wait(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_inAgain_Check_DemoMode(this, play);
}

void En_Zl2_inAgain_main_greet(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_set_eye_pattern(this);
    En_Zl2_Animation_Base(this);
    En_Zl2_inAgain_Check_DemoMode(this, play);
}

void En_Zl2_inAgain_main_approach(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_inAgain_Set_WalkSound(this);
    En_Zl2_BGcheck(this, play);
    En_Zl2_set_eye_pattern(this);
    En_Zl2_Animation_Base(this);
    En_Zl2_Movement_Approach(this, play);
    En_Zl2_inAgain_check_ApproachToConfess(this, play);
}

void En_Zl2_inAgain_main_confess(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_set_eye_pattern(this);
    En_Zl2_Animation_Base(this);
    En_Zl2_inAgain_Check_DemoMode(this, play);
}

void En_Zl2_inAgain_main_gloom(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_set_NormalToClose_eye_pattern(this);
    En_Zl2_inAgain_check_GloomToWaver(this, En_Zl2_Animation_Base(this));
}

void En_Zl2_inAgain_main_waver(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_Animation_Base(this);
    En_Zl2_inAgain_Check_DemoMode(this, play);
}

void En_Zl2_inAgain_main_forward(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_set_CloseToNormal_eye_pattern(this);
    En_Zl2_inAgain_check_ForwardToEncourage(this, En_Zl2_Animation_Base(this));
}

void En_Zl2_inAgain_main_encourage(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_set_eye_pattern(this);
    En_Zl2_Animation_Base(this);
    En_Zl2_inAgain_Check_DemoMode(this, play);
}

void En_Zl2_inAgain_main_handup(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_set_eye_pattern(this);
    En_Zl2_inAgain_check_HandupToCheer(this, En_Zl2_Animation_Base(this));
}

void En_Zl2_inAgain_main_cheer(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_set_eye_pattern(this);
    En_Zl2_Animation_Base(this);
    En_Zl2_inAgain_Check_DemoMode(this, play);
}

void En_Zl2_inAgain_main_give(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_set_eye_pattern(this);
    En_Zl2_Animation_Base(this);
    En_Zl2_inAgain_Check_DemoMode(this, play);
}

void En_Zl2_inAgain_main_effect(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_set_eye_pattern(this);
    En_Zl2_Animation_Base(this);
    En_Zl2_inAgain_Check_DemoMode(this, play);
}

void En_Zl2_inAgain_main_gaze(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_inAgain_Set_QuakeSound(play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_set_eye_pattern(this);
    En_Zl2_Animation_Base(this);
    En_Zl2_inAgain_Check_DemoMode(this, play);
}

void En_Zl2_inAgain_main_turn(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_inAgain_Set_QuakeSound(play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_inAgain_check_TurnToAround(this, En_Zl2_Animation_Base(this));
}

void En_Zl2_inAgain_main_around(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_inAgain_Set_QuakeSound(play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_Animation_Base(this);
    En_Zl2_inAgain_Check_DemoMode(this, play);
}

void En_Zl2_inAgain_main_raise(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_inAgain_Set_QuakeSound(play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_inAgain_check_RaiseToAttention(this, En_Zl2_Animation_Base(this));
}

void En_Zl2_inAgain_main_attention(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_inAgain_Set_QuakeSound(play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_Animation_Base(this);
    En_Zl2_inAgain_Check_DemoMode(this, play);
}

void En_Zl2_inAgain_main_arrest(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_inAgain_Set_QuakeSound(play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_inAgain_check_ArrestToPrison(this, En_Zl2_Animation_Base(this));
}

void En_Zl2_inAgain_main_prison(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_inAgain_Set_QuakeSound(play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_Animation_Base(this);
    En_Zl2_inAgain_Check_DemoMode(this, play);
}

void En_Zl2_inAgain_main_collapse(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_inAgain_Set_QuakeSound(play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_inAgain_check_CollapseToFaint(this, En_Zl2_Animation_Base(this));
}

void En_Zl2_inAgain_main_faint(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_Animation_Base(this);
    SetPos_Cryst_In_En_Zl2_inAgain(this);
    En_Zl2_inAgain_Check_DemoMode(this, play);
}

void En_Zl2_inAgain_main_fly(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_inAgain_Set_HeavenSound(this);
    En_Zl2_BGcheck(this, play);
    En_Zl2_Animation_Base(this);
    En_Zl2_Movement_Approach(this, play);
    SetPos_Cryst_In_En_Zl2_inAgain(this);
    En_Zl2_inAgain_Check_DemoMode(this, play);
}

void En_Zl2_inAgain_main_fade(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_Animation_Base(this);
    En_Zl2_inAgain_SetAlpha(this, play);
    En_Zl2_inAgain_check_FadeToDisappear(this, play);
}

void En_Zl2_inAgain_main_disappear(EnZl2* this, PlayState* play) {
    Actor* child;

    if (En_Zl2_Get_npcdemopnt(play, 0) == NULL) {
        child = this->actor.child;
        if (child != NULL) {
            Actor_delete(child);
        }
        Actor_delete(&this->actor);
    }
}
