void En_Zl2_Actor_inEnding_Init(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_00A15C, 0, 0.0f, 0);
    this->action = 25;
    this->unk_254 = 1;
}

void Birth_Effect_Light_In_En_Zl2_inEnding(EnZl2* this, PlayState* play) {
    Player* player;
    f32 posX;
    f32 posY;
    f32 posZ;

    if (this->unk_250 == 0) {
        player = GET_PLAYER(play);
        posX = player->actor.world.pos.x;
        posY = player->actor.world.pos.y;
        posZ = player->actor.world.pos.z;
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_UNK_7);
        this->unk_250 = 1;
    }
}

void En_Zl2_inEnding_SetFace_Stand(EnZl2* this, PlayState* play) {
    En_Zl2_set_eye_pattern(this);
    if (play->csCtx.curFrame < 431) {
        En_Zl2_set_mouth_pattern(this, 1);
    } else {
        En_Zl2_set_mouth_pattern(this, 0);
    }
}

void En_Zl2_inEnding_SetFace_Hesitate(EnZl2* this, PlayState* play) {
    En_Zl2_set_NormalToClose_eye_pattern(this);
    En_Zl2_set_mouth_pattern(this, 0);
}

void En_Zl2_inEnding_SetFace_Pile(EnZl2* this, PlayState* play) {
    if (play->csCtx.curFrame < 988) {
        En_Zl2_setNum_eye_pattern(this, 7);
        En_Zl2_setNum_eye_pattern2(this, 8);
    } else {
        En_Zl2_set_eye_pattern(this);
    }
    En_Zl2_set_mouth_pattern(this, 0);
}

void En_Zl2_inEnding_SetFace_Chest(EnZl2* this, PlayState* play) {
    En_Zl2_set_eye_pattern(this);
    if (play->csCtx.curFrame < 1190) {
        En_Zl2_set_mouth_pattern(this, 1);
    } else {
        En_Zl2_set_mouth_pattern(this, 0);
    }
}

void En_Zl2_inEnding_SetFace_Play(EnZl2* this, PlayState* play) {
    CutsceneContext* csCtx;

    if (this->skelAnime.mode != 0) {
        En_Zl2_set_eye_pattern(this);
    } else {
        csCtx = &play->csCtx;
        if (csCtx->curFrame < 1520) {
            En_Zl2_set_NormalToClose2_eye_pattern(this);
        } else if (csCtx->curFrame == 1520) {
            this->unk_27C = 0.0f;
        } else {
            En_Zl2_set_Close2ToNormal_eye_pattern(this);
        }
    }
}

void En_Zl2_inEnding_setup_Wait(EnZl2* this) {
    this->action = 25;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void En_Zl2_inEnding_setup_Stand(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_00A15C, 0, -8.0f, 0);
    En_Zl2_Set_DemoStartPosAngle(this, play, 0);
    this->action = 26;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void En_Zl2_inEnding_ChengAnime_Require(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_0087B8, 0, -8.0f, 0);
    }
}

void En_Zl2_inEnding_setup_Require(EnZl2* this) {
    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_007D0C, 2, -8.0f, 0);
    this->action = 27;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void En_Zl2_inEnding_ChengAnime_Pile(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_009AD4, 0, -8.0f, 0);
    }
}

void En_Zl2_inEnding_setup_Pile(EnZl2* this) {
    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_0090D8, 2, -8.0f, 0);
    this->action = 28;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void En_Zl2_inEnding_ChengAnime_Chest(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_006778, 0, -8.0f, 0);
    }
}

void En_Zl2_inEnding_setup_Chest(EnZl2* this) {
    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_005F40, 2, -8.0f, 0);
    this->action = 29;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
}

void En_Zl2_inEnding_ChengAnime_Hesitate(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_002750, 0, -8.0f, 0);
        this->action = 31;
    }
}

void En_Zl2_inEnding_setup_Hesitate(EnZl2* this) {
    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_0022D0, 2, -8.0f, 0);
    this->action = 30;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void En_Zl2_inEnding_ChengAnime_Play(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_00A79C, 0, -8.0f, 0);
        this->unk_27C = 0.0f;
    }
}

void En_Zl2_inEnding_setup_Play(EnZl2* this) {
    En_Zl2_Change_Anime(this, SEGMENTED_TO_VIRTUAL(&gZelda2Anime1Anim_004900), 2, -8.0f, 0);
    this->action = 32;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void En_Zl2_inEnding_Check_DemoMode(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = En_Zl2_Get_npcdemopnt(play, 0);
    s32 nextCueId;
    s32 currentCueId;

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    En_Zl2_inEnding_setup_Wait(this);
                    break;
                case 2:
                    En_Zl2_inEnding_setup_Stand(this, play);
                    break;
                case 16:
                    En_Zl2_inEnding_setup_Require(this);
                    break;
                case 17:
                    En_Zl2_inEnding_setup_Pile(this);
                    break;
                case 18:
                    En_Zl2_inEnding_setup_Chest(this);
                    break;
                case 4:
                    En_Zl2_inEnding_setup_Hesitate(this);
                    break;
                case 20:
                    En_Zl2_inEnding_setup_Play(this);
                    break;
                case 21:
                    Birth_Effect_Light_In_En_Zl2_inEnding(this, play);
                    break;
                default:
                    PRINTF("En_Zl2_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void En_Zl2_inEnding_main_wait(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_inEnding_Check_DemoMode(this, play);
}

void En_Zl2_inEnding_main_stand(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_inEnding_SetFace_Stand(this, play);
    En_Zl2_Animation_Base(this);
    En_Zl2_inEnding_Check_DemoMode(this, play);
}

void En_Zl2_inEnding_main_require(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_set_eye_pattern(this);
    En_Zl2_inEnding_ChengAnime_Require(this, En_Zl2_Animation_Base(this));
    En_Zl2_inEnding_Check_DemoMode(this, play);
}

void En_Zl2_inEnding_main_pile(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_inEnding_SetFace_Pile(this, play);
    En_Zl2_inEnding_ChengAnime_Pile(this, En_Zl2_Animation_Base(this));
    En_Zl2_inEnding_Check_DemoMode(this, play);
}

void En_Zl2_inEnding_main_chest(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_inEnding_SetFace_Chest(this, play);
    En_Zl2_inEnding_ChengAnime_Chest(this, En_Zl2_Animation_Base(this));
    En_Zl2_inEnding_Check_DemoMode(this, play);
}

void En_Zl2_inEnding_main_hesitate(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_inEnding_SetFace_Hesitate(this, play);
    En_Zl2_inEnding_ChengAnime_Hesitate(this, En_Zl2_Animation_Base(this));
}

void En_Zl2_inEnding_main_downforward(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_Animation_Base(this);
    En_Zl2_inEnding_Check_DemoMode(this, play);
}

void En_Zl2_inEnding_main_play(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_inEnding_SetFace_Play(this, play);
    En_Zl2_inEnding_ChengAnime_Play(this, En_Zl2_Animation_Base(this));
    En_Zl2_inEnding_Check_DemoMode(this, play);
}
