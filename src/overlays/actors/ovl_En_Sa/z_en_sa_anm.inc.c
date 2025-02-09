static f32 anime_revers(EnSa* this) {
    f32 endFrame = this->skelAnime.endFrame;
    f32 startFrame = this->skelAnime.startFrame;

    this->skelAnime.startFrame = endFrame;
    this->skelAnime.curFrame = endFrame;
    this->skelAnime.endFrame = startFrame;
    this->skelAnime.playSpeed = -1.0f;
    return startFrame;
}

static void ply_1_2(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENSA_ANIM2_3);
            this->unk_20A++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENSA_ANIM2_2);
                this->unk_20A++;
            }
            break;
    }
}

void ply_3_k(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENSA_ANIM2_8);
            this->unk_20A++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENSA_ANIM2_9);
                this->unk_20A++;
            }
            break;
    }
}

void ply_4_5(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENSA_ANIM2_1);
            this->unk_20A++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENSA_ANIM2_7);
                this->unk_20A++;
            }
            break;
    }
}

void ply_4rv_k(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENSA_ANIM2_1);
            anime_revers(this);
            this->unk_20A++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENSA_ANIM2_9);
                this->unk_20A++;
            }
            break;
    }
}

void ply_6_7(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENSA_ANIM2_6);
            this->unk_20A++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENSA_ANIM2_4);
                this->unk_20A++;
            }
            break;
    }
}

void ply_6rv_k(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENSA_ANIM2_6);
            anime_revers(this);
            this->unk_20A++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENSA_ANIM2_9);
                this->unk_20A++;
            }
            break;
    }
}

void ply_8_9_k(EnSa* this) {
    switch (this->unk_20A) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENSA_ANIM2_5);
            this->unk_20A++;
            FALLTHROUGH;
        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                npc_anime_ct(&this->skelAnime, animetbl, ENSA_ANIM2_0);
                this->unk_20A++;
            }
            break;
    }
}

static void set_play_anime_mode(EnSa* this, u8 arg1) {
    this->unk_20B = arg1;
    this->unk_20A = 0;
}

void sa_chg_anime(EnSa* this) {
    switch (this->unk_20B) {
        case 1:
            ply_1_2(this);
            break;
        case 2:
            ply_3_k(this);
            break;
        case 3:
            ply_4_5(this);
            break;
        case 4:
            ply_4rv_k(this);
            break;
        case 5:
            ply_6_7(this);
            break;
        case 6:
            ply_6rv_k(this);
            break;
        case 7:
            ply_8_9_k(this);
            break;
    }
}
