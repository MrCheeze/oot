f32 func_80AF5894(EnSa* this) {
    f32 endFrame = this->skelAnime.endFrame;
    f32 startFrame = this->skelAnime.startFrame;

    this->skelAnime.startFrame = endFrame;
    this->skelAnime.curFrame = endFrame;
    this->skelAnime.endFrame = startFrame;
    this->skelAnime.playSpeed = -1.0f;
    return startFrame;
}

void func_80AF58B8(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_3);
            this->unk_20A++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_2);
                this->unk_20A++;
            }
            break;
    }
}

void func_80AF594C(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_8);
            this->unk_20A++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_9);
                this->unk_20A++;
            }
            break;
    }
}

void func_80AF59E0(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_1);
            this->unk_20A++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_7);
                this->unk_20A++;
            }
            break;
    }
}

void func_80AF5A74(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_1);
            func_80AF5894(this);
            this->unk_20A++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_9);
                this->unk_20A++;
            }
            break;
    }
}

void func_80AF5B10(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_6);
            this->unk_20A++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_4);
                this->unk_20A++;
            }
            break;
    }
}

void func_80AF5BA4(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_6);
            func_80AF5894(this);
            this->unk_20A++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_9);
                this->unk_20A++;
            }
            break;
    }
}

void func_80AF5C40(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_5);
            this->unk_20A++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo2, ENSA_ANIM2_0);
                this->unk_20A++;
            }
            break;
    }
}

void func_80AF5CD4(EnSa* this, u8 arg1) {
    this->unk_20B = arg1;
    this->unk_20A = 0;
}

void func_80AF5CE4(EnSa* this) {
    switch (this->unk_20B) {
        case 1:
            func_80AF58B8(this);
            break;
        case 2:
            func_80AF594C(this);
            break;
        case 3:
            func_80AF59E0(this);
            break;
        case 4:
            func_80AF5A74(this);
            break;
        case 5:
            func_80AF5B10(this);
            break;
        case 6:
            func_80AF5BA4(this);
            break;
        case 7:
            func_80AF5C40(this);
            break;
    }
}
