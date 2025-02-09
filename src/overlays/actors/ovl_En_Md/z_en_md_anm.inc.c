void EnMd_ReverseAnimation(EnMd* this) {
    f32 startFrame;

    startFrame = this->skelAnime.startFrame;
    this->skelAnime.startFrame = this->skelAnime.endFrame;
    this->skelAnime.curFrame = this->skelAnime.endFrame;
    this->skelAnime.endFrame = startFrame;
    this->skelAnime.playSpeed = -1.0f;
}

void EnMd_UpdateAnimSequence_IdleToHalt(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_IDLE_TO_HALT);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_HALT);
                this->animSequenceEntry++;
            }
            break;
    }
}

void EnMd_UpdateAnimSequence_HaltToCurious(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_HALT_TO_CURIOUS);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_CURIOUS);
                this->animSequenceEntry++;
            }
            break;
    }
}

void EnMd_UpdateAnimSequence_WalkAway(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_IDLE_TO_HALT);
            EnMd_ReverseAnimation(this);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_IDLE_TO_WALK);
                this->animSequenceEntry++;
            } else {
                break;
            }
            FALLTHROUGH;
        case 2:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_WALK);
                this->animSequenceEntry++;
            }
            break;
    }
}

void EnMd_UpdateAnimSequence_TwitchIdle_Unused(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_IDLE_TO_WALK);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_IDLE);
                this->animSequenceEntry++;
            }
            break;
    }
}

void EnMd_UpdateAnimSequence_HaltToIdle(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_IDLE_TO_HALT);
            EnMd_ReverseAnimation(this);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_IDLE);
                this->animSequenceEntry++;
            }
            break;
    }
}

void EnMd_UpdateAnimSequence_SurpriseToAnnoyed(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_IDLE_TO_SURPISE);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_ANNOYED);
                this->animSequenceEntry++;
            }
            break;
    }
}

void EnMd_UpdateAnimSequence_SurpriseToIdle(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_IDLE_TO_SURPISE);
            EnMd_ReverseAnimation(this);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_IDLE);
                this->animSequenceEntry++;
            }
            break;
    }
}

void EnMd_UpdateAnimSequence_CuriousToAnnoyed(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_CURIOUS_TO_ANNOYED);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_ANNOYED);
                this->animSequenceEntry++;
            }
            break;
    }
}

void EnMd_UpdateAnimSequence_AnnoyedToHalt(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_ANNOYED_TO_HALT);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_HALT);
                this->animSequenceEntry++;
            }
            break;
    }
}

void EnMd_UpdateAnimSequence_IdleToAnnoyed(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_IDLE_TO_ANNOYED);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_ANNOYED);
                this->animSequenceEntry++;
            }
            break;
    }
}

void EnMd_UpdateAnimSequence_StopWalking(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_IDLE_TO_WALK);
            EnMd_ReverseAnimation(this);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_IDLE);
                this->animSequenceEntry++;
            }
            break;
    }
}

void EnMd_SetAnimSequence(EnMd* this, u8 state) {
    this->animSequence = state;
    this->animSequenceEntry = 0;
}

void EnMd_UpdateAnimSequence(EnMd* this) {
    switch (this->animSequence) {
        case ENMD_ANIM_SEQ_IDLE_TO_HALT:
            EnMd_UpdateAnimSequence_IdleToHalt(this);
            break;
        case ENMD_ANIM_SEQ_HALT_TO_CURIOUS:
            EnMd_UpdateAnimSequence_HaltToCurious(this);
            break;
        case ENMD_ANIM_SEQ_WALK_AWAY:
            EnMd_UpdateAnimSequence_WalkAway(this);
            break;
        case ENMD_ANIM_SEQ_TWITCH_IDLE_UNUSED:
            // unreachable
            EnMd_UpdateAnimSequence_TwitchIdle_Unused(this);
            break;
        case ENMD_ANIM_SEQ_HALT_TO_IDLE:
            EnMd_UpdateAnimSequence_HaltToIdle(this);
            break;
        case ENMD_ANIM_SEQ_SURPRISE_TO_ANNOYED:
            EnMd_UpdateAnimSequence_SurpriseToAnnoyed(this);
            break;
        case ENMD_ANIM_SEQ_SURPRISE_TO_IDLE:
            EnMd_UpdateAnimSequence_SurpriseToIdle(this);
            break;
        case ENMD_ANIM_SEQ_CURIOUS_TO_ANNOYED:
            EnMd_UpdateAnimSequence_CuriousToAnnoyed(this);
            break;
        case ENMD_ANIM_SEQ_ANNOYED_TO_HALT:
            EnMd_UpdateAnimSequence_AnnoyedToHalt(this);
            break;
        case ENMD_ANIM_SEQ_IDLE_TO_ANNOYED:
            EnMd_UpdateAnimSequence_IdleToAnnoyed(this);
            break;
        case ENMD_ANIM_SEQ_STOP_WALKING:
            EnMd_UpdateAnimSequence_StopWalking(this);
    }
}

void EnMd_UpdateAnimSequence_WithTalking(EnMd* this) {
    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        switch (this->actor.textId) {
            case 0x102F:
                if ((this->messageEntry == 0) && (this->animSequence != ENMD_ANIM_SEQ_IDLE_TO_HALT)) {
                    EnMd_SetAnimSequence(this, ENMD_ANIM_SEQ_IDLE_TO_HALT);
                }
                if ((this->messageEntry == 2) && (this->animSequence != ENMD_ANIM_SEQ_HALT_TO_CURIOUS)) {
                    EnMd_SetAnimSequence(this, ENMD_ANIM_SEQ_HALT_TO_CURIOUS);
                }
                if ((this->messageEntry == 5) && (this->animSequence != ENMD_ANIM_SEQ_CURIOUS_TO_ANNOYED)) {
                    EnMd_SetAnimSequence(this, ENMD_ANIM_SEQ_CURIOUS_TO_ANNOYED);
                }
                if ((this->messageEntry == 11) && (this->animSequence != ENMD_ANIM_SEQ_ANNOYED_TO_HALT)) {
                    EnMd_SetAnimSequence(this, ENMD_ANIM_SEQ_ANNOYED_TO_HALT);
                }
                break;
            case 0x1033:
                if ((this->messageEntry == 0) && (this->animSequence != ENMD_ANIM_SEQ_IDLE_TO_HALT)) {
                    EnMd_SetAnimSequence(this, ENMD_ANIM_SEQ_IDLE_TO_HALT);
                }
                if ((this->messageEntry == 1) && (this->animSequence != ENMD_ANIM_SEQ_HALT_TO_CURIOUS)) {
                    EnMd_SetAnimSequence(this, ENMD_ANIM_SEQ_HALT_TO_CURIOUS);
                }
                if ((this->messageEntry == 5) && (this->animSequence != ENMD_ANIM_SEQ_IDLE_TO_ANNOYED)) {
                    EnMd_SetAnimSequence(this, ENMD_ANIM_SEQ_IDLE_TO_ANNOYED);
                }
                if ((this->messageEntry == 7) && (this->animSequence != ENMD_ANIM_SEQ_ANNOYED_TO_HALT)) {
                    EnMd_SetAnimSequence(this, ENMD_ANIM_SEQ_ANNOYED_TO_HALT);
                }
                break;
            case 0x1030:
            case 0x1034:
            case 0x1045:
                if ((this->messageEntry == 0) && (this->animSequence != ENMD_ANIM_SEQ_IDLE_TO_HALT)) {
                    EnMd_SetAnimSequence(this, ENMD_ANIM_SEQ_IDLE_TO_HALT);
                }
                break;
            case 0x1046:
                if ((this->messageEntry == 0) && (this->animSequence != ENMD_ANIM_SEQ_SURPRISE_TO_ANNOYED)) {
                    EnMd_SetAnimSequence(this, ENMD_ANIM_SEQ_SURPRISE_TO_ANNOYED);
                }
                break;
        }
    } else if (this->skelAnime.animation != &gMidoIdleAnim) {
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_INDEX_IDLE);
        EnMd_SetAnimSequence(this, ENMD_ANIM_SEQ_NONE);
    }

    EnMd_UpdateAnimSequence(this);
}
