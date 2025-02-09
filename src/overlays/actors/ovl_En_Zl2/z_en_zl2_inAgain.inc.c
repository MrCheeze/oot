void func_80B4FD90(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_00B5FC, 0, 0.0f, 0);
    this->action = 1;
}

void func_80B4FDD4(EnZl2* this) {
    if (Animation_OnFrame(&this->skelAnime, 14.0f)) {
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_STONE);
    }
}

void func_80B4FE10(PlayState* play) {
    if ((play->csCtx.curFrame >= 830) && (play->csCtx.curFrame < 1081)) {
        Sfx_PlaySfxCentered2(NA_SE_EV_EARTHQUAKE - SFX_FLAG);
    }
}

void func_80B4FE48(EnZl2* this) {
    Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_EV_GOTO_HEAVEN - SFX_FLAG);
}

void func_80B4FE6C(EnZl2* this) {
    Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_EN_GANON_LAUGH);
}

void func_80B4FE90(EnZl2* this) {
    Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_VO_Z1_SURPRISE);
}

void func_80B4FEB4(EnZl2* this) {
    Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_VO_Z1_PAIN);
}

void func_80B4FED8(EnZl2* this) {
    Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_VO_Z1_CRY_0);
}

void EnZl2_GiveLightArrows(EnZl2* this, PlayState* play) {
    Player* player;
    f32 posX;
    f32 posY;
    f32 posZ;

    if (this->unk_244 == 0) {
        player = GET_PLAYER(play);
        posX = player->actor.world.pos.x;
        posY = player->actor.world.pos.y + 80.0f;
        posZ = player->actor.world.pos.z;
        Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0, 0x17);
        Item_Give(play, ITEM_ARROW_LIGHT);
        this->unk_244 = 1;
    }
}

void func_80B4FF84(EnZl2* this, PlayState* play) {
    f32 posX;
    f32 posY;
    f32 posZ;

    if (this->unk_250 == 0) {
        posX = this->actor.world.pos.x;
        posY = this->actor.world.pos.y;
        posZ = this->actor.world.pos.z;

        Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_YELLOW);
        this->unk_250 = 1;
    }
}

void func_80B4FFF0(EnZl2* this, PlayState* play) {
    f32 posX;
    f32 posY;
    f32 posZ;

    if (this->unk_248 == 0) {
        posX = this->actor.world.pos.x;
        posY = this->actor.world.pos.y + (kREG(5) + -26.0f);
        posZ = this->actor.world.pos.z;

        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0x4000, 0,
                           WARP_PURPLE_CRYSTAL);
        this->unk_248 = 1;
    }
}

void func_80B5008C(EnZl2* this) {
    Actor* child = this->actor.child;

    if (child != NULL) {
        child->world.pos.x = this->actor.world.pos.x;
        child->world.pos.y = this->actor.world.pos.y + (kREG(5) + -26.0f);
        child->world.pos.z = this->actor.world.pos.z;
    }
}

void func_80B500E0(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = EnZl2_GetCue(play, 0);
    Vec3f* worldPos = &this->actor.world.pos;
    f32 startX;
    f32 startY;
    f32 startZ;
    f32 endX;
    f32 endY;
    f32 endZ;
    f32 lerp;

    if (cue != NULL) {
        lerp = Environment_LerpWeightAccelDecel(cue->endFrame, cue->startFrame, play->csCtx.curFrame, 8, 8);

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

void func_80B501C4(EnZl2* this, s32 alpha) {
    if (this->actor.child != NULL) {
        ((DoorWarp1*)this->actor.child)->crystalAlpha = alpha;
    }
}

void func_80B501E8(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = EnZl2_GetCue(play, 0);

    if (cue != NULL) {
        this->actor.shape.shadowAlpha = this->alpha =
            (1.0f - Environment_LerpWeight(cue->endFrame, cue->startFrame, play->csCtx.curFrame)) * 255.0f;
        func_80B501C4(this, this->alpha);
    }
}

void func_80B50260(EnZl2* this, PlayState* play) {
    this->action = 1;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_80B50278(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = EnZl2_GetCue(play, 0);

    this->actor.world.pos.x = cue->startPos.x;
    this->actor.world.pos.y = cue->startPos.y;
    this->actor.world.pos.z = cue->startPos.z;

    this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;

    this->actor.shape.shadowAlpha = 255;
    this->action = 2;
    this->drawConfig = 1;
}

void func_80B50304(EnZl2* this, PlayState* play) {
    s32 pad[2];
    ActorShape* shape = &this->actor.shape;
    CsCmdActorCue* cue = EnZl2_GetCue(play, 0);
    f32 cueXDelta;
    f32 cueZDelta;

    cueXDelta = cue->endPos.x - cue->startPos.x;
    cueZDelta = cue->endPos.z - cue->startPos.z;

    func_80B4FD00(this, &gZelda2Anime1Anim_0003BC, 0, -12.0f, 0);
    this->action = 3;
    this->drawConfig = 1;
    this->unk_23C = 0.0f;
    shape->shadowAlpha = 255;
    this->actor.world.rot.y = shape->rot.y = RAD_TO_BINANG(Math_FAtan2F(cueXDelta, cueZDelta));
}

void func_80B503DC(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = EnZl2_GetCue(play, 0);

    if ((cue != NULL) && (play->csCtx.curFrame >= cue->endFrame)) {
        this->action = 4;
    }
}

void func_80B5042C(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_0022D0, 2, -8.0f, 0);
    this->action = 5;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void func_80B50488(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_002750, 0, 0.0f, 0);
        this->action = 6;
        this->drawConfig = 1;
    }
}

void func_80B504D4(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_00325C, 2, -8.0f, 0);
    this->action = 7;
    this->drawConfig = 1;
    this->unk_27C = 0.0f;
    EnZl2_setMouthIndex(this, 1);
    this->actor.shape.shadowAlpha = 0xFF;
}

void func_80B5053C(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_003538, 0, 0.0f, 0);
        this->action = 8;
    }
}

void func_80B50580(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_000A50, 2, -8.0f, 0);
    this->action = 9;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
}

void func_80B505D4(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_000EB0, 0, 0.0f, 0);
        this->action = 10;
    }
}

void func_80B50618(EnZl2* this, PlayState* play) {
    EnZl2_GiveLightArrows(this, play);
    this->action = 11;
}

void func_80B50644(EnZl2* this, PlayState* play) {
    func_80B4FF84(this, play);
    this->action = 12;
}

void func_80B50670(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_00B5FC, 0, -8.0f, 0);
    this->action = 13;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
}

void func_80B506C4(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_00AAD4, 2, -8.0f, 0);
    this->action = 14;
    this->drawConfig = 1;
    EnZl2_setEyesIndex(this, 4);
    EnZl2_setMouthIndex(this, 2);
    this->actor.shape.shadowAlpha = 0xFF;
    func_80B4FE90(this);
}

void func_80B5073C(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_00AFE0, 0, 0.0f, 0);
        this->action = 15;
    }
}

void func_80B50780(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_001670, 2, -8.0f, 0);
    this->action = 16;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    func_80B4FFF0(this, play);
    EnZl2_setEyesIndex(this, 3);
}

void func_80B507E8(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_001B48, 0, 0.0f, 0);
        this->action = 17;
    }
}

void func_80B5082C(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_002B14, 2, -8.0f, 0);
    this->action = 18;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
}

void func_80B50880(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_002F30, 0, 0.0f, 0);
        this->action = 19;
        func_80B4FEB4(this);
    }
}

void func_80B508C8(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_001010, 2, -8.0f, 0);
    this->action = 20;
    this->drawConfig = 1;
    EnZl2_setEyesIndex(this, 6);
    this->actor.shape.shadowAlpha = 0xFF;
}

void func_80B50928(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_0013A0, 0, 0.0f, 0);
        this->action = 21;
        func_80B4FED8(this);
    }
}

void func_80B50970(EnZl2* this, PlayState* play) {
    this->action = 22;
}

void func_80B50980(EnZl2* this, PlayState* play) {
    this->action = 23;
    this->drawConfig = 2;
    this->alpha = 255;
}

void func_80B509A0(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = EnZl2_GetCue(play, 0);

    if (cue != NULL) {
        if (play->csCtx.curFrame >= cue->endFrame) {
            this->action = 24;
            this->drawConfig = 0;
            func_80B4FE6C(this);
        }
    }
}

void func_80B50A04(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = EnZl2_GetCue(play, 0);
    s32 nextCueId;
    s32 currentCueId;

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    func_80B50260(this, play);
                    break;
                case 2:
                    func_80B50278(this, play);
                    break;
                case 3:
                    func_80B50304(this, play);
                    break;
                case 4:
                    func_80B5042C(this, play);
                    break;
                case 5:
                    func_80B504D4(this, play);
                    break;
                case 6:
                    func_80B50580(this, play);
                    break;
                case 7:
                    func_80B50618(this, play);
                    break;
                case 8:
                    func_80B50670(this, play);
                    break;
                case 9:
                    func_80B506C4(this, play);
                    break;
                case 10:
                    func_80B50780(this, play);
                    break;
                case 11:
                    func_80B5082C(this, play);
                    break;
                case 12:
                    func_80B508C8(this, play);
                    break;
                case 13:
                    func_80B50970(this, play);
                    break;
                case 14:
                    func_80B50980(this, play);
                    break;
                case 15:
                    func_80B50644(this, play);
                    break;
                default:
                    PRINTF("En_Zl2_inAgain_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void func_80B50BBC(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B50A04(this, play);
}

void func_80B50BEC(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B50C40(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FDD4(this);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B500E0(this, play);
    func_80B503DC(this, play);
}

void func_80B50CA8(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B50CFC(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    func_80B4EA40(this);
    func_80B50488(this, EnZl2_UpdateSkelAnime(this));
}

void func_80B50D50(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B50D94(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    func_80B4EAF4(this);
    func_80B5053C(this, EnZl2_UpdateSkelAnime(this));
}

void func_80B50DE8(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B50E3C(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    func_80B505D4(this, EnZl2_UpdateSkelAnime(this));
}

void func_80B50E90(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B50EE4(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B50F38(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B50F8C(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE10(play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B50FE8(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE10(play);
    func_80B4ED2C(this, play);
    func_80B5073C(this, EnZl2_UpdateSkelAnime(this));
}

void func_80B51034(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE10(play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B51080(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE10(play);
    func_80B4ED2C(this, play);
    func_80B507E8(this, EnZl2_UpdateSkelAnime(this));
}

void func_80B510CC(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE10(play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B51118(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE10(play);
    func_80B4ED2C(this, play);
    func_80B50880(this, EnZl2_UpdateSkelAnime(this));
}

void func_80B51164(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE10(play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B511B0(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE10(play);
    func_80B4ED2C(this, play);
    func_80B50928(this, EnZl2_UpdateSkelAnime(this));
}

void func_80B511FC(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B5008C(this);
    func_80B50A04(this, play);
}

void func_80B51250(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE48(this);
    func_80B4ED2C(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B500E0(this, play);
    func_80B5008C(this);
    func_80B50A04(this, play);
}

void func_80B512B8(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B501E8(this, play);
    func_80B509A0(this, play);
}

void func_80B51310(EnZl2* this, PlayState* play) {
    Actor* child;

    if (EnZl2_GetCue(play, 0) == NULL) {
        child = this->actor.child;
        if (child != NULL) {
            Actor_Kill(child);
        }
        Actor_Kill(&this->actor);
    }
}
