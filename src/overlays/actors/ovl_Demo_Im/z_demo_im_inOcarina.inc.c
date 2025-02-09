void func_80985E60(DemoIm* this, PlayState* play) {
    DemoIm_ChangeAnim(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->action = 10;
    this->unk_280 = 1;
}

void func_80985EAC(DemoIm* this, PlayState* play) {
    if ((play->csCtx.curFrame >= 80) && (play->csCtx.curFrame < 243)) {
        func_80984F10(this, play);
    } else {
        func_80984DB8(this);
    }
}

void func_80985EF4(DemoIm* this) {
    if (!Animation_OnFrame(&this->skelAnime, Animation_GetLastFrame(&gImpaWhistlingAnim) - 1.0f)) {
        DemoIm_UpdateSkelAnime(this);
    }
}

void func_80985F54(DemoIm* this) {
    this->action = 10;
    this->drawConfig = 0;
}

void func_80985F64(DemoIm* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gImpaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaIdleAnim),
                     ANIMMODE_LOOP, 0.0f);
    func_80985180(this, play, 5);
    this->action = 11;
    this->drawConfig = 1;
}

void func_80985FE8(DemoIm* this, s32 arg1) {
    if (arg1 != 0) {
        Animation_Change(&this->skelAnime, &gImpaWhistlingAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaWhistlingAnim),
                         ANIMMODE_LOOP, -8.0f);
    }
}

void func_8098604C(DemoIm* this) {
    f32 frameCount = Animation_GetLastFrame(&gImpaStartWhistlingAnim);

    Animation_Change(&this->skelAnime, &gImpaStartWhistlingAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, -8.0f);
    this->action = 12;
    this->drawConfig = 1;
    this->unk_2D0 = 1;
}

void func_809860C8(DemoIm* this) {
    this->action = 13;
    this->drawConfig = 1;
}

void func_809860DC(DemoIm* this, s32 arg1) {
    if (arg1 != 0) {
        Animation_Change(&this->skelAnime, &gImpaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaIdleAnim),
                         ANIMMODE_LOOP, -8.0f);
        this->unk_2D0 = 0;
    }
}

void func_80986148(DemoIm* this) {
    Animation_Change(&this->skelAnime, &gImpaStartWhistlingAnim, -1.0f,
                     Animation_GetLastFrame(&gImpaStartWhistlingAnim), 0.0f, ANIMMODE_ONCE, -8.0f);
    this->action = 14;
    this->drawConfig = 1;
}

void func_809861C4(DemoIm* this, PlayState* play) {
    CsCmdActorCue* cue = DemoIm_GetCue(play, 5);

    if (cue != NULL) {
        u32 nextCueId = cue->id;
        u32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 9:
                    func_80986148(this);
                    break;
                case 7:
                    Animation_Change(&this->skelAnime, &gImpaWhistlingAnim, 1.0f, 0.0f,
                                     Animation_GetLastFrame(&gImpaWhistlingAnim), ANIMMODE_LOOP, -8.0f);
                    this->action = 12;
                    break;
                default:
                    PRINTF("Demo_Im_Ocarina_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void func_8098629C(DemoIm* this, PlayState* play) {
    if (DemoIm_IsCutsceneIdle(play)) {
        this->action = 21;
        this->drawConfig = 1;
        this->unk_280 = 1;
    }
}

void func_809862E0(DemoIm* this, PlayState* play) {
    CsCmdActorCue* cue = DemoIm_GetCue(play, 5);

    if (cue != NULL) {
        u32 nextCueId = cue->id;
        u32 currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    func_80985F54(this);
                    break;
                case 2:
                    func_80985F64(this, play);
                    break;
                case 7:
                    func_8098604C(this);
                    break;
                case 8:
                    func_809860C8(this);
                    break;
                case 9:
                    func_80986148(this);
                    break;
                default:
                    PRINTF("Demo_Im_Ocarina_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void func_809863BC(DemoIm* this, PlayState* play) {
    func_809862E0(this, play);
}

void func_809863DC(DemoIm* this, PlayState* play) {
    DemoIm_UpdateBgCheckInfo(this, play);
    DemoIm_UpdateSkelAnime(this);
    func_80985EAC(this, play);
    func_80984BE0(this);
    func_809862E0(this, play);
}

void func_80986430(DemoIm* this, PlayState* play) {
    s32 sp24;

    DemoIm_UpdateBgCheckInfo(this, play);
    sp24 = DemoIm_UpdateSkelAnime(this);
    func_80985EAC(this, play);
    func_80984BE0(this);
    func_80985FE8(this, sp24);
    func_809862E0(this, play);
}

void func_80986494(DemoIm* this, PlayState* play) {
    DemoIm_UpdateBgCheckInfo(this, play);
    func_80985EF4(this);
    func_80984BE0(this);
    func_809861C4(this, play);
}

void func_809864D4(DemoIm* this, PlayState* play) {
    s32 sp24;

    DemoIm_UpdateBgCheckInfo(this, play);
    sp24 = DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_809860DC(this, sp24);
    func_8098629C(this, play);
}
