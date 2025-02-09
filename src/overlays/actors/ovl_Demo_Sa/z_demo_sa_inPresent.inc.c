void func_8098F83C(DemoSa* this, PlayState* play) {
    Vec3f* thisPos = &this->actor.world.pos;

    SkelAnime_InitFlex(play, &this->skelAnime, &gSariaSkel, &gSariaWaitOnBridgeAnim, NULL, NULL, 0);
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_ELF, thisPos->x, thisPos->y, thisPos->z, 0, 0, 0,
                       FAIRY_KOKIRI);
    this->action = 16;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
    DemoSa_SetEyeIndex(this, SARIA_EYE_SAD);
    DemoSa_SetMouthIndex(this, SARIA_MOUTH_CLOSED);
}

void func_8098F8F8(DemoSa* this) {
    s32 alpha = 255;
    f32* unk_1A0 = &this->unk_1A0;
    f32 temp_f0;

    *unk_1A0 += 1.0f;
    temp_f0 = kREG(17) + 10.0f;

    if (temp_f0 <= *unk_1A0) {
        this->actor.shape.shadowAlpha = this->alpha = alpha;
    } else {
        this->actor.shape.shadowAlpha = this->alpha = (*unk_1A0 / temp_f0) * 255.0f;
    }
}

void func_8098F984(DemoSa* this) {
    this->action = 16;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_8098F998(DemoSa* this, PlayState* play) {
    if (this->cueId == 4) {
        func_8098E6EC(this, play, 1);
        this->action = 17;
        this->drawConfig = 2;
        this->unk_1B0 = 0;
        this->actor.shape.shadowAlpha = 0;
    } else {
        func_8098E76C(this, &gSariaWaitOnBridgeAnim, 0, 0.0f, 0);
        this->action = 18;
        this->drawConfig = 1;
        this->unk_1B0 = 0;
        this->actor.shape.shadowAlpha = 0xFF;
    }
    DemoSa_SetEyeIndex(this, SARIA_EYE_SAD);
}

void func_8098FA2C(DemoSa* this) {
    if (this->unk_1A0 >= kREG(17) + 10.0f) {
        this->action = 18;
        this->drawConfig = 1;
        this->unk_1B0 = 0;
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_8098FA84(DemoSa* this) {
    func_8098E76C(this, &gSariaHoldOcarinaAnim, 0, 0.0f, 0);
    this->action = 19;
    this->drawConfig = 1;
    this->unk_1B0 = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    DemoSa_SetEyeIndex(this, SARIA_EYE_CLOSED);
}

void func_8098FAE0(DemoSa* this) {
    func_8098E76C(this, &gSariaGiveLinkOcarinaAnim, 2, -8.0f, 0);
    this->action = 20;
    this->drawConfig = 1;
    this->unk_1B0 = 1;
    this->actor.shape.shadowAlpha = 0xFF;
}

void func_8098FB34(DemoSa* this, s32 arg1) {
    if (arg1 != 0) {
        func_8098E76C(this, &gSariaHoldOutOcarinaAnim, 0, 0, 0);
    }
}

void func_8098FB68(DemoSa* this, PlayState* play) {
    s32 currentCueId;
    s32 nextCueId;
    CsCmdActorCue* cue = DemoSa_GetCue(play, 1);

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 4:
                    func_8098F984(this);
                    break;
                case 12:
                    func_8098F998(this, play);
                    break;
                case 13:
                    func_8098FA84(this);
                    break;
                case 14:
                    func_8098FAE0(this);
                    break;
                default:
                    PRINTF("Demo_Sa_inPresent_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void func_8098FC44(DemoSa* this, PlayState* play) {
    func_8098FB68(this, play);
}

void func_8098FC64(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098F8F8(this);
    func_8098FA2C(this);
}

void func_8098FC9C(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098FB68(this, play);
}

void func_8098FCD4(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098FB68(this, play);
}

void func_8098FD0C(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    func_8098FB34(this, DemoSa_UpdateSkelAnime(this));
    func_8098FB68(this, play);
}
