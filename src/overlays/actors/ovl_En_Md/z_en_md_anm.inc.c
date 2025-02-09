static void anime_revers(EnMd* this) {
    f32 startFrame;

    startFrame = this->skelAnime.startFrame;
    this->skelAnime.startFrame = this->skelAnime.endFrame;
    this->skelAnime.curFrame = this->skelAnime.endFrame;
    this->skelAnime.endFrame = startFrame;
    this->skelAnime.playSpeed = -1.0f;
}

static void ply_1_2(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_IDLE_TO_HALT);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_HALT);
                this->animSequenceEntry++;
            }
            break;
    }
}

void ply_3_4(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_HALT_TO_CURIOUS);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_CURIOUS);
                this->animSequenceEntry++;
            }
            break;
    }
}

void ply_1rv_8_9(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_IDLE_TO_HALT);
            anime_revers(this);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_IDLE_TO_WALK);
                this->animSequenceEntry++;
            } else {
                break;
            }
            FALLTHROUGH;
        case 2:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_WALK);
                this->animSequenceEntry++;
            }
            break;
    }
}

void ply_8_k(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_IDLE_TO_WALK);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_IDLE);
                this->animSequenceEntry++;
            }
            break;
    }
}

void ply_1rv_k(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_IDLE_TO_HALT);
            anime_revers(this);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_IDLE);
                this->animSequenceEntry++;
            }
            break;
    }
}

void ply_10_6(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_IDLE_TO_SURPISE);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_ANNOYED);
                this->animSequenceEntry++;
            }
            break;
    }
}

void ply_10rv_k(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_IDLE_TO_SURPISE);
            anime_revers(this);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_IDLE);
                this->animSequenceEntry++;
            }
            break;
    }
}

void ply_5_6(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_CURIOUS_TO_ANNOYED);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_ANNOYED);
                this->animSequenceEntry++;
            }
            break;
    }
}

void ply_7_2(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_ANNOYED_TO_HALT);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_HALT);
                this->animSequenceEntry++;
            }
            break;
    }
}

void ply_8_6(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_IDLE_TO_ANNOYED);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_ANNOYED);
                this->animSequenceEntry++;
            }
            break;
    }
}

void ply_8rv_k(EnMd* this) {
    switch (this->animSequenceEntry) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_IDLE_TO_WALK);
            anime_revers(this);
            this->animSequenceEntry++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_IDLE);
                this->animSequenceEntry++;
            }
            break;
    }
}

static void set_play_anime_mode(EnMd* this, u8 state) {
    this->animSequence = state;
    this->animSequenceEntry = 0;
}

void md_chg_anime_sub(EnMd* this) {
    switch (this->animSequence) {
        case ENMD_ANIM_SEQ_IDLE_TO_HALT:
            ply_1_2(this);
            break;
        case ENMD_ANIM_SEQ_HALT_TO_CURIOUS:
            ply_3_4(this);
            break;
        case ENMD_ANIM_SEQ_WALK_AWAY:
            ply_1rv_8_9(this);
            break;
        case ENMD_ANIM_SEQ_TWITCH_IDLE_UNUSED:
            // unreachable
            ply_8_k(this);
            break;
        case ENMD_ANIM_SEQ_HALT_TO_IDLE:
            ply_1rv_k(this);
            break;
        case ENMD_ANIM_SEQ_SURPRISE_TO_ANNOYED:
            ply_10_6(this);
            break;
        case ENMD_ANIM_SEQ_SURPRISE_TO_IDLE:
            ply_10rv_k(this);
            break;
        case ENMD_ANIM_SEQ_CURIOUS_TO_ANNOYED:
            ply_5_6(this);
            break;
        case ENMD_ANIM_SEQ_ANNOYED_TO_HALT:
            ply_7_2(this);
            break;
        case ENMD_ANIM_SEQ_IDLE_TO_ANNOYED:
            ply_8_6(this);
            break;
        case ENMD_ANIM_SEQ_STOP_WALKING:
            ply_8rv_k(this);
    }
}

void md_chg_anime(EnMd* this) {
    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        switch (this->actor.textId) {
            case 0x102F:
                if ((this->messageEntry == 0) && (this->animSequence != ENMD_ANIM_SEQ_IDLE_TO_HALT)) {
                    set_play_anime_mode(this, ENMD_ANIM_SEQ_IDLE_TO_HALT);
                }
                if ((this->messageEntry == 2) && (this->animSequence != ENMD_ANIM_SEQ_HALT_TO_CURIOUS)) {
                    set_play_anime_mode(this, ENMD_ANIM_SEQ_HALT_TO_CURIOUS);
                }
                if ((this->messageEntry == 5) && (this->animSequence != ENMD_ANIM_SEQ_CURIOUS_TO_ANNOYED)) {
                    set_play_anime_mode(this, ENMD_ANIM_SEQ_CURIOUS_TO_ANNOYED);
                }
                if ((this->messageEntry == 11) && (this->animSequence != ENMD_ANIM_SEQ_ANNOYED_TO_HALT)) {
                    set_play_anime_mode(this, ENMD_ANIM_SEQ_ANNOYED_TO_HALT);
                }
                break;
            case 0x1033:
                if ((this->messageEntry == 0) && (this->animSequence != ENMD_ANIM_SEQ_IDLE_TO_HALT)) {
                    set_play_anime_mode(this, ENMD_ANIM_SEQ_IDLE_TO_HALT);
                }
                if ((this->messageEntry == 1) && (this->animSequence != ENMD_ANIM_SEQ_HALT_TO_CURIOUS)) {
                    set_play_anime_mode(this, ENMD_ANIM_SEQ_HALT_TO_CURIOUS);
                }
                if ((this->messageEntry == 5) && (this->animSequence != ENMD_ANIM_SEQ_IDLE_TO_ANNOYED)) {
                    set_play_anime_mode(this, ENMD_ANIM_SEQ_IDLE_TO_ANNOYED);
                }
                if ((this->messageEntry == 7) && (this->animSequence != ENMD_ANIM_SEQ_ANNOYED_TO_HALT)) {
                    set_play_anime_mode(this, ENMD_ANIM_SEQ_ANNOYED_TO_HALT);
                }
                break;
            case 0x1030:
            case 0x1034:
            case 0x1045:
                if ((this->messageEntry == 0) && (this->animSequence != ENMD_ANIM_SEQ_IDLE_TO_HALT)) {
                    set_play_anime_mode(this, ENMD_ANIM_SEQ_IDLE_TO_HALT);
                }
                break;
            case 0x1046:
                if ((this->messageEntry == 0) && (this->animSequence != ENMD_ANIM_SEQ_SURPRISE_TO_ANNOYED)) {
                    set_play_anime_mode(this, ENMD_ANIM_SEQ_SURPRISE_TO_ANNOYED);
                }
                break;
        }
    } else if (this->skelAnime.animation != &gMidoIdleAnim) {
        npc_anime_ct(&this->skelAnime, animetbl, ENMD_ANIM_INDEX_IDLE);
        set_play_anime_mode(this, ENMD_ANIM_SEQ_NONE);
    }

    md_chg_anime_sub(this);
}
