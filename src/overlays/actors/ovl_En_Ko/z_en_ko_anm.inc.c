s32 ko_period_check(EnKo* this) {
    s32 result;

    if (!LINK_IS_ADULT) {
        // Obtained Zelda's Letter
        if (GET_EVENTCHKINF(EVENTCHKINF_40)) {
            return ENKO_FQS_CHILD_SARIA;
        }
        if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
            return ENKO_FQS_CHILD_STONE;
        }
        return ENKO_FQS_CHILD_START;
    }

    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
        result = ENKO_FQS_ADULT_SAVED;
    } else {
        result = ENKO_FQS_ADULT_ENEMY;
    }
    return result;
}

f32 get_anchor_offs(EnKo* this) {
    f32 D_80A9A62C[13][5] = {
        /* ENKO_TYPE_CHILD_0    */ { 0.0f, 0.0f, 0.0f, -30.0f, -20.0f },
        /* ENKO_TYPE_CHILD_1    */ { 0.0f, 0.0f, 0.0f, -20.0f, -10.0f },
        /* ENKO_TYPE_CHILD_2    */ { 0.0f, 0.0f, 0.0f, -30.0f, -20.0f },
        /* ENKO_TYPE_CHILD_3    */ { -10.0f, 10.0f, 10.0f, -10.0f, -30.0f },
        /* ENKO_TYPE_CHILD_4    */ { 0.0f, 0.0f, 0.0f, -10.0f, -20.0f },
        /* ENKO_TYPE_CHILD_5    */ { 0.0f, 0.0f, 0.0f, -20.0f, -20.0f },
        /* ENKO_TYPE_CHILD_6    */ { 0.0f, 0.0f, 0.0f, -10.0f, -20.0f },
        /* ENKO_TYPE_CHILD_7    */ { 10.0f, 10.0f, 10.0f, -60.0f, -20.0f },
        /* ENKO_TYPE_CHILD_8    */ { -10.0f, -10.0f, -20.0f, -30.0f, -30.0f },
        /* ENKO_TYPE_CHILD_9    */ { -10.0f, -10.0f, -10.0f, -40.0f, -40.0f },
        /* ENKO_TYPE_CHILD_10   */ { 0.0f, 0.0f, 0.0f, -10.0f, -20.0f },
        /* ENKO_TYPE_CHILD_11   */ { -10.0f, -10.0f, -20.0f, -30.0f, -30.0f },
        /* ENKO_TYPE_CHILD_FADO */ { 0.0f, 0.0f, 0.0f, -20.0f, -20.0f },
    };

    if (LINK_IS_ADULT && ENKO_TYPE == ENKO_TYPE_CHILD_FADO) {
        return -20.0f;
    }
    return D_80A9A62C[ENKO_TYPE][ko_period_check(this)];
}

u8 ko_bgcheck_cnd(EnKo* this) {
    u8 D_80A9A730[13][5] = {
        /* ENKO_TYPE_CHILD_0    */ { 1, 1, 1, 0, 1 },
        /* ENKO_TYPE_CHILD_1    */ { 1, 1, 1, 1, 1 },
        /* ENKO_TYPE_CHILD_2    */ { 1, 1, 1, 0, 1 },
        /* ENKO_TYPE_CHILD_3    */ { 1, 1, 1, 0, 1 },
        /* ENKO_TYPE_CHILD_4    */ { 1, 1, 1, 0, 1 },
        /* ENKO_TYPE_CHILD_5    */ { 0, 0, 0, 0, 0 },
        /* ENKO_TYPE_CHILD_6    */ { 1, 1, 1, 1, 1 },
        /* ENKO_TYPE_CHILD_7    */ { 1, 1, 1, 0, 1 },
        /* ENKO_TYPE_CHILD_8    */ { 0, 0, 0, 0, 0 },
        /* ENKO_TYPE_CHILD_9    */ { 0, 0, 0, 0, 0 },
        /* ENKO_TYPE_CHILD_10   */ { 1, 1, 1, 1, 1 },
        /* ENKO_TYPE_CHILD_11   */ { 0, 0, 0, 0, 0 },
        /* ENKO_TYPE_CHILD_FADO */ { 1, 1, 1, 1, 1 },
    };

    return D_80A9A730[ENKO_TYPE][ko_period_check(this)];
}

s32 ko_angle_check(EnKo* this) {
    s16 yawDiff;
    s16 yawDiffAbs;
    s32 result;

    yawDiff = this->actor.yawTowardsPlayer - (f32)this->actor.shape.rot.y;
    yawDiffAbs = ABS(yawDiff);

    if (yawDiffAbs < 0x3FFC) {
        result = true;
    } else {
        result = false;
    }
    return result;
}

s32 ko_eye_move_01(EnKo* this, PlayState* play) {
    s16 trackingMode;

    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        if ((this->skelAnime.animation == &gKokiriWipingForeheadAnim) == false) {
            npc_anime_ct(&this->skelAnime, animetbl, ENKO_ANIM_WIPING_FOREHEAD);
        }
        trackingMode = NPC_TRACKING_HEAD_AND_TORSO;
    } else {
        if ((this->skelAnime.animation == &gKokiriLiftingRockAnim) == false) {
            npc_anime_ct(&this->skelAnime, animetbl, ENKO_ANIM_LIFTING_ROCK);
        }
        trackingMode = NPC_TRACKING_NONE;
    }
    eye_moveM(&this->actor, &this->interactInfo, 2, trackingMode);
    return ko_angle_check(this);
}

s32 ko_eye_move_02(EnKo* this, PlayState* play) {
    s16 trackingMode;

    program_wait(play, this->fidgetTableY, this->fidgetTableZ, 16);
    if (ko_angle_check(this) == true) {
        trackingMode = NPC_TRACKING_HEAD_AND_TORSO;
    } else {
        trackingMode = NPC_TRACKING_NONE;
    }
    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        trackingMode = NPC_TRACKING_FULL_BODY;
    } else if (this->lookDist < this->actor.xzDistToPlayer) {
        trackingMode = NPC_TRACKING_NONE;
    }
    eye_moveM(&this->actor, &this->interactInfo, 2, trackingMode);
    return 1;
}

s32 ko_eye_move_03(EnKo* this, PlayState* play) {
    s16 trackingMode;
    s32 result;

    program_wait(play, this->fidgetTableY, this->fidgetTableZ, 16);
    result = ko_angle_check(this);
    trackingMode = (result == true) ? NPC_TRACKING_HEAD_AND_TORSO : NPC_TRACKING_NONE;
    eye_moveM(&this->actor, &this->interactInfo, 2, trackingMode);
    return result;
}

s32 ko_eye_move_04(EnKo* this, PlayState* play) {
    program_wait(play, this->fidgetTableY, this->fidgetTableZ, 16);
    eye_moveM(&this->actor, &this->interactInfo, 2, NPC_TRACKING_FULL_BODY);
    return 1;
}

s32 ko_eye_move_05(EnKo* this, PlayState* play) {
    s16 trackingMode;

    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        if ((this->skelAnime.animation == &gKokiriBlockingAnim) == false) {
            npc_anime_ct(&this->skelAnime, animetbl, ENKO_ANIM_BLOCKING_STATIC);
        }
        program_wait(play, this->fidgetTableY, this->fidgetTableZ, 16);
        trackingMode = NPC_TRACKING_HEAD_AND_TORSO;
    } else {
        if ((this->skelAnime.animation == &gKokiriCuttingGrassAnim) == false) {
            npc_anime_ct(&this->skelAnime, animetbl, ENKO_ANIM_CUTTING_GRASS);
        }
        trackingMode = NPC_TRACKING_NONE;
    }
    eye_moveM(&this->actor, &this->interactInfo, 5, trackingMode);
    return ko_angle_check(this);
}

s32 ko_eye_move_06(EnKo* this, PlayState* play) {
    s16 trackingMode;
    s32 result;

    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        if ((this->skelAnime.animation == &gKokiriBlockingAnim) == false) {
            npc_anime_ct(&this->skelAnime, animetbl, ENKO_ANIM_BLOCKING_STATIC);
        }
        program_wait(play, this->fidgetTableY, this->fidgetTableZ, 16);
        result = ko_angle_check(this);
        trackingMode = (result == true) ? NPC_TRACKING_HEAD_AND_TORSO : NPC_TRACKING_NONE;
    } else {
        if ((this->skelAnime.animation == &gKokiriPunchingAnim) == false) {
            npc_anime_ct(&this->skelAnime, animetbl, ENKO_ANIM_PUNCHING);
        }
        trackingMode = NPC_TRACKING_NONE;
        result = ko_angle_check(this);
    }
    eye_moveM(&this->actor, &this->interactInfo, 5, trackingMode);
    return result;
}

// Same as ko_eye_move_04
s32 ko_eye_move_07(EnKo* this, PlayState* play) {
    program_wait(play, this->fidgetTableY, this->fidgetTableZ, 16);
    eye_moveM(&this->actor, &this->interactInfo, 2, NPC_TRACKING_FULL_BODY);
    return 1;
}

s32 ko_eye_move_08(EnKo* this, PlayState* play) {
    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        if (Skeleton_Info_frame_check(&this->skelAnime, 18.0f)) {
            this->skelAnime.playSpeed = 0.0f;
        }
    } else if (this->skelAnime.playSpeed != 1.0f) {
        this->skelAnime.playSpeed = 1.0f;
    }
    if (this->skelAnime.playSpeed == 0.0f) {
        program_wait(play, this->fidgetTableY, this->fidgetTableZ, 16);
    }
    eye_moveM(&this->actor, &this->interactInfo, 2,
                   (this->skelAnime.playSpeed == 0.0f) ? NPC_TRACKING_HEAD_AND_TORSO : NPC_TRACKING_NONE);
    return ko_angle_check(this);
}

s32 ko_sub_action_00(EnKo* this, PlayState* play) {
    switch (ENKO_TYPE) {
        case ENKO_TYPE_CHILD_0:
            return ko_eye_move_01(this, play);
        case ENKO_TYPE_CHILD_1:
            return ko_eye_move_02(this, play);
        case ENKO_TYPE_CHILD_2:
            return ko_eye_move_06(this, play);
        case ENKO_TYPE_CHILD_3:
            return ko_eye_move_02(this, play);
        case ENKO_TYPE_CHILD_4:
            return ko_eye_move_05(this, play);
        case ENKO_TYPE_CHILD_5:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_6:
            return ko_eye_move_04(this, play);
        case ENKO_TYPE_CHILD_7:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_8:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_9:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_10:
            return ko_eye_move_02(this, play);
        case ENKO_TYPE_CHILD_11:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_FADO:
            return ko_eye_move_02(this, play);
    }
    // Note this function assumes the kokiri type is valid
    UNREACHABLE();
}

s32 ko_sub_action_01(EnKo* this, PlayState* play) {
    switch (ENKO_TYPE) {
        case ENKO_TYPE_CHILD_0:
            return ko_eye_move_07(this, play);
        case ENKO_TYPE_CHILD_1:
            return ko_eye_move_07(this, play);
        case ENKO_TYPE_CHILD_2:
            return ko_eye_move_06(this, play);
        case ENKO_TYPE_CHILD_3:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_4:
            return ko_eye_move_05(this, play);
        case ENKO_TYPE_CHILD_5:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_6:
            return ko_eye_move_04(this, play);
        case ENKO_TYPE_CHILD_7:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_8:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_9:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_10:
            return ko_eye_move_02(this, play);
        case ENKO_TYPE_CHILD_11:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_FADO:
            return ko_eye_move_02(this, play);
    }
    // Note this function assumes the kokiri type is valid
    UNREACHABLE();
}

s32 ko_sub_action_02(EnKo* this, PlayState* play) {
    switch (ENKO_TYPE) {
        case ENKO_TYPE_CHILD_0:
            return ko_eye_move_07(this, play);
        case ENKO_TYPE_CHILD_1:
            return ko_eye_move_07(this, play);
        case ENKO_TYPE_CHILD_2:
            return ko_eye_move_06(this, play);
        case ENKO_TYPE_CHILD_3:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_4:
            return ko_eye_move_08(this, play);
        case ENKO_TYPE_CHILD_5:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_6:
            return ko_eye_move_04(this, play);
        case ENKO_TYPE_CHILD_7:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_8:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_9:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_10:
            return ko_eye_move_02(this, play);
        case ENKO_TYPE_CHILD_11:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_FADO:
            return ko_eye_move_02(this, play);
    }
    // Note this function assumes the kokiri type is valid
    UNREACHABLE();
}

s32 ko_sub_action_03(EnKo* this, PlayState* play) {
    switch (ENKO_TYPE) {
        case ENKO_TYPE_CHILD_0:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_1:
            return ko_eye_move_07(this, play);
        case ENKO_TYPE_CHILD_2:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_3:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_4:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_5:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_6:
            return ko_eye_move_04(this, play);
        case ENKO_TYPE_CHILD_7:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_8:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_9:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_10:
            return ko_eye_move_02(this, play);
        case ENKO_TYPE_CHILD_11:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_FADO:
            return ko_eye_move_02(this, play);
    }
    // Note this function assumes the kokiri type is valid
    UNREACHABLE();
}

s32 ko_sub_action_04(EnKo* this, PlayState* play) {
    switch (ENKO_TYPE) {
        case ENKO_TYPE_CHILD_0:
            return ko_eye_move_06(this, play);
        case ENKO_TYPE_CHILD_1:
            return ko_eye_move_02(this, play);
        case ENKO_TYPE_CHILD_2:
            return ko_eye_move_02(this, play);
        case ENKO_TYPE_CHILD_3:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_4:
            return ko_eye_move_02(this, play);
        case ENKO_TYPE_CHILD_5:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_6:
            return ko_eye_move_04(this, play);
        case ENKO_TYPE_CHILD_7:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_8:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_9:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_10:
            return ko_eye_move_02(this, play);
        case ENKO_TYPE_CHILD_11:
            return ko_eye_move_03(this, play);
        case ENKO_TYPE_CHILD_FADO:
            return ko_eye_move_02(this, play);
    }
    // Note this function assumes the kokiri type is valid
    UNREACHABLE();
}
