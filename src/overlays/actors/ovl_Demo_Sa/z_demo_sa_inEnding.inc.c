void func_8098F420(DemoSa* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gSariaSkel, &gSariaSitting3Anim, NULL, NULL, 0);
    this->action = 11;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_8098F480(DemoSa* this) {
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

void func_8098F50C(DemoSa* this, PlayState* play) {
    func_8098E6EC(this, play, 4);
    this->action = 12;
    this->drawConfig = 2;
}

void func_8098F544(DemoSa* this) {
    if (this->unk_1A0 >= kREG(17) + 10.0f) {
        this->action = 13;
        this->drawConfig = 1;
    }
}

void func_8098F590(DemoSa* this) {
    func_8098E76C(this, &gSariaSitting1Anim, 2, -8.0f, 0);
    this->action = 14;
}

void func_8098F5D0(DemoSa* this) {
    func_8098E76C(this, &gSariaSitting2Anim, 2, 0.0f, 0);
    this->action = 15;
}

void func_8098F610(DemoSa* this, s32 arg1) {
    if (arg1 != 0) {
        func_8098E76C(this, &gSariaSitting3Anim, 0, 0.0f, 0);
        this->action = 13;
    }
}

void func_8098F654(DemoSa* this, PlayState* play) {
    s32 currentCueId;
    s32 nextCueId;
    CsCmdActorCue* cue = DemoSa_GetCue(play, 4);

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 7:
                    func_8098F50C(this, play);
                    break;
                case 8:
                    func_8098F590(this);
                    break;
                case 9:
                    func_8098F5D0(this);
                    break;
                default:
                    PRINTF("Demo_Sa_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void func_8098F714(DemoSa* this, PlayState* play) {
    func_8098F654(this, play);
}

void func_8098F734(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098E480(this);
    func_8098F480(this);
    func_8098F544(this);
}

void func_8098F77C(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098E480(this);
    func_8098F654(this, play);
}

void func_8098F7BC(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098E480(this);
    func_8098F654(this, play);
}

void func_8098F7FC(DemoSa* this, PlayState* play) {
    s32 sp1C;

    func_8098E5C8(this, play);
    sp1C = DemoSa_UpdateSkelAnime(this);
    func_8098E480(this);
    func_8098F610(this, sp1C);
}
