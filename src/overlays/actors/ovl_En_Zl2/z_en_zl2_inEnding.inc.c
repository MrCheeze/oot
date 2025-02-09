void func_80B5135C(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_00A15C, 0, 0.0f, 0);
    this->action = 25;
    this->unk_254 = 1;
}

void func_80B513A8(EnZl2* this, PlayState* play) {
    Player* player;
    f32 posX;
    f32 posY;
    f32 posZ;

    if (this->unk_250 == 0) {
        player = GET_PLAYER(play);
        posX = player->actor.world.pos.x;
        posY = player->actor.world.pos.y;
        posZ = player->actor.world.pos.z;
        Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_UNK_7);
        this->unk_250 = 1;
    }
}

void func_80B51418(EnZl2* this, PlayState* play) {
    EnZl2_UpdateEyes(this);
    if (play->csCtx.curFrame < 431) {
        EnZl2_setMouthIndex(this, 1);
    } else {
        EnZl2_setMouthIndex(this, 0);
    }
}

void func_80B5146C(EnZl2* this, PlayState* play) {
    func_80B4EA40(this);
    EnZl2_setMouthIndex(this, 0);
}

void func_80B5149C(EnZl2* this, PlayState* play) {
    if (play->csCtx.curFrame < 988) {
        EnZl2_setEyesIndex(this, 7);
        EnZl2_setEyeIndex2(this, 8);
    } else {
        EnZl2_UpdateEyes(this);
    }
    EnZl2_setMouthIndex(this, 0);
}

void func_80B514F8(EnZl2* this, PlayState* play) {
    EnZl2_UpdateEyes(this);
    if (play->csCtx.curFrame < 1190) {
        EnZl2_setMouthIndex(this, 1);
    } else {
        EnZl2_setMouthIndex(this, 0);
    }
}

void func_80B5154C(EnZl2* this, PlayState* play) {
    CutsceneContext* csCtx;

    if (this->skelAnime.mode != 0) {
        EnZl2_UpdateEyes(this);
    } else {
        csCtx = &play->csCtx;
        if (csCtx->curFrame < 1520) {
            func_80B4EBB8(this);
        } else if (csCtx->curFrame == 1520) {
            this->unk_27C = 0.0f;
        } else {
            func_80B4EC48(this);
        }
    }
}

void func_80B515C4(EnZl2* this) {
    this->action = 25;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_80B515D8(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_00A15C, 0, -8.0f, 0);
    func_80B4EDB8(this, play, 0);
    this->action = 26;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void func_80B51644(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_0087B8, 0, -8.0f, 0);
    }
}

void func_80B51678(EnZl2* this) {
    func_80B4FD00(this, &gZelda2Anime1Anim_007D0C, 2, -8.0f, 0);
    this->action = 27;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void func_80B516D0(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_009AD4, 0, -8.0f, 0);
    }
}

void func_80B51704(EnZl2* this) {
    func_80B4FD00(this, &gZelda2Anime1Anim_0090D8, 2, -8.0f, 0);
    this->action = 28;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void func_80B5175C(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_006778, 0, -8.0f, 0);
    }
}

void func_80B51790(EnZl2* this) {
    func_80B4FD00(this, &gZelda2Anime1Anim_005F40, 2, -8.0f, 0);
    this->action = 29;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
}

void func_80B517E0(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_002750, 0, -8.0f, 0);
        this->action = 31;
    }
}

void func_80B51824(EnZl2* this) {
    func_80B4FD00(this, &gZelda2Anime1Anim_0022D0, 2, -8.0f, 0);
    this->action = 30;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void func_80B5187C(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_00A79C, 0, -8.0f, 0);
        this->unk_27C = 0.0f;
    }
}

void func_80B518C0(EnZl2* this) {
    func_80B4FD00(this, SEGMENTED_TO_VIRTUAL(&gZelda2Anime1Anim_004900), 2, -8.0f, 0);
    this->action = 32;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void func_80B51948(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = EnZl2_GetCue(play, 0);
    s32 nextCueId;
    s32 currentCueId;

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    func_80B515C4(this);
                    break;
                case 2:
                    func_80B515D8(this, play);
                    break;
                case 16:
                    func_80B51678(this);
                    break;
                case 17:
                    func_80B51704(this);
                    break;
                case 18:
                    func_80B51790(this);
                    break;
                case 4:
                    func_80B51824(this);
                    break;
                case 20:
                    func_80B518C0(this);
                    break;
                case 21:
                    func_80B513A8(this, play);
                    break;
                default:
                    PRINTF("En_Zl2_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void func_80B51A5C(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B51948(this, play);
}

void func_80B51A8C(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    func_80B51418(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B51948(this, play);
}

void func_80B51AE4(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    func_80B51644(this, EnZl2_UpdateSkelAnime(this));
    func_80B51948(this, play);
}

void func_80B51B44(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    func_80B5149C(this, play);
    func_80B516D0(this, EnZl2_UpdateSkelAnime(this));
    func_80B51948(this, play);
}

void func_80B51BA8(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    func_80B514F8(this, play);
    func_80B5175C(this, EnZl2_UpdateSkelAnime(this));
    func_80B51948(this, play);
}

void func_80B51C0C(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    func_80B5146C(this, play);
    func_80B517E0(this, EnZl2_UpdateSkelAnime(this));
}

void func_80B51C64(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B51948(this, play);
}

void func_80B51CA8(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    func_80B5154C(this, play);
    func_80B5187C(this, EnZl2_UpdateSkelAnime(this));
    func_80B51948(this, play);
}
