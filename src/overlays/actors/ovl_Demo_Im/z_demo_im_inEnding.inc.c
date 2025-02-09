void func_80987018(DemoIm* this, PlayState* play) {
    DemoIm_ChangeAnim(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->action = 27;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_80987064(DemoIm* this) {
    f32* unk_268 = &this->unk_268;
    f32 temp;
    s32 alpha = 255;

    *unk_268 += 1.0f;
    temp = kREG(17) + 10.0f;

    if (*unk_268 >= temp) {
        this->actor.shape.shadowAlpha = this->alpha = alpha;
    } else {
        this->actor.shape.shadowAlpha = this->alpha = (*unk_268 / temp) * 255.0f;
    }
}

void func_809870F0(DemoIm* this, PlayState* play) {
    func_80985200(this, play, 5);
    this->action = 28;
    this->drawConfig = 2;
}

void func_80987128(DemoIm* this) {
    if (this->unk_268 >= kREG(17) + 10.0f) {
        this->action = 29;
        this->drawConfig = 1;
    }
}

void func_80987174(DemoIm* this) {
    DemoIm_ChangeAnim(this, &object_im_Anim_0101C8, ANIMMODE_ONCE, -8.0f, false);
    this->action = 30;
}

void func_809871B4(DemoIm* this, s32 arg1) {
    if (arg1 != 0) {
        DemoIm_ChangeAnim(this, &object_im_Anim_00FB10, ANIMMODE_LOOP, 0.0f, false);
    }
}

void func_809871E8(DemoIm* this, PlayState* play) {
    CsCmdActorCue* cue = DemoIm_GetCue(play, 5);

    if (cue != NULL) {
        u32 nextCueId = cue->id;
        u32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 12:
                    func_809870F0(this, play);
                    break;
                case 13:
                    func_80987174(this);
                    break;
                default:
                    PRINTF("Demo_Im_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void func_80987288(DemoIm* this, PlayState* play) {
    func_809871E8(this, play);
}

void func_809872A8(DemoIm* this, PlayState* play) {
    DemoIm_UpdateBgCheckInfo(this, play);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_80987064(this);
    func_80987128(this);
}

void func_809872F0(DemoIm* this, PlayState* play) {
    DemoIm_UpdateBgCheckInfo(this, play);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_809871E8(this, play);
}

void func_80987330(DemoIm* this, PlayState* play) {
    s32 sp1C;

    DemoIm_UpdateBgCheckInfo(this, play);
    sp1C = DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_809871B4(this, sp1C);
}
