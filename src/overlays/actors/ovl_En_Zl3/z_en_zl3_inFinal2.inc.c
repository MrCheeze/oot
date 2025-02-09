void En_Zl3_Actor_inFinal2_Init(EnZl3* this, PlayState* play) {
    PRINTF("ゼルダ姫のEn_Zl3_Actor_inFinal2_Init通すよ!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_005A0C, 0, 0.0f, 0);
    this->action = 7;
    this->drawConfig = 1;
    PRINTF("ゼルダ姫のEn_Zl3_Actor_inFinal2_Initは通った!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    En_Zl3_set_mouth_pattern(this, 1);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void En_Zl3_inFinal2_Set_PainSound(EnZl3* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_Z1_PAIN);
}

static Vec3f pos = { 0.0f, 0.0f, 0.0f };

void En_Zl3_inFinal2_Set_Cry1Sound(EnZl3* this) {
    Na_SetRandomSeFlag(&pos, NA_SE_VO_Z1_CRY_0, NA_SE_VO_Z1_CRY_1 - NA_SE_VO_Z1_CRY_0 + 1);
}

void En_Zl3_inFinal2_Set_LookSwordSound(EnZl3* this) {
    SkelAnime* skelAnime = &this->skelAnime;

    if ((skelAnime->mode == 2) && Skeleton_Info_frame_check(skelAnime, 4.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_Z1_PAIN);
    }
}

void En_Zl3_inFinal2_Init_Movement_Frighten(EnZl3* this) {
    s32 pad[4];
    s16 thisRotY = this->actor.world.rot.y;
    Vec3f* unk_338 = &this->unk_338;

    *unk_338 = this->unk_32C = this->actor.world.pos;

    unk_338->z += (-1.6074f * cos_s(thisRotY)) - (3.1620007f * sin_s(thisRotY));
    unk_338->x += (-1.6074f * sin_s(thisRotY)) + (3.1620007f * cos_s(thisRotY));
    unk_338->y += -0.012199402f;
}

void En_Zl3_inFinal2_Movement_Frighten(EnZl3* this) {
    Vec3f* thisPos = &this->actor.world.pos;
    Vec3f* unk_32C = &this->unk_32C;
    Vec3f* unk_338 = &this->unk_338;
    f32 temp_f0 = get_parcent_forAccelBrake(Si2_anime_end_frame(&gZelda2Anime2Anim_005248), 0,
                                                   (s32)this->skelAnime.curFrame, 3, 3);

    thisPos->x = unk_32C->x + (temp_f0 * (unk_338->x - unk_32C->x));
    thisPos->z = unk_32C->z + (temp_f0 * (unk_338->z - unk_32C->z));
}

void En_Zl3_inFinal2_SetAngle_forLink_handdown(EnZl3* this, PlayState* play) {
    if (play->activeCamId == CAM_ID_MAIN) {
        En_Zl3_SetAngle_forLink(this);
    }
}

void En_Zl3_inFinal2_setup_Stand(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_005A0C, 0, 0.0f, 0);
    this->action = 7;
}

void En_Zl3_inFinal2_setup_Press(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_00499C, 2, -8.0f, 0);
    this->action = 8;
}

void En_Zl3_inFinal2_Check_Animation_Press(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_004408, 0, 0.0f, 0);
    }
}

void En_Zl3_inFinal2_setup_Lookup(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_006508, 2, -8.0f, 0);
    this->action = 9;
}

void En_Zl3_inFinal2_Check_Animation_Lookup(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_0061C4, 0, 0.0f, 0);
    }
}

void En_Zl3_inFinal2_setup_Frighten(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_005248, 2, -8.0f, 0);
    En_Zl3_inFinal2_Init_Movement_Frighten(this);
    En_Zl3_inFinal2_Set_PainSound(this);
    En_Zl3_set_mouth_pattern(this, 2);
    this->action = 10;
}

void En_Zl3_inFinal2_setup_Relief(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_0054E0, 0, 0.0f, 0);
    this->action = 11;
}

void En_Zl3_inFinal2_Check_FrightenToRelief(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl3_inFinal2_setup_Relief(this);
    }
}

void En_Zl3_inFinal2_setup_Handdown(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_008684, 2, -8.0f, 0);
    this->action = 12;
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void En_Zl3_inFinal2_Check_Animation_Handdown(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_006F04, 0, 0.0f, 0);
    }
}

void En_Zl3_inFinal2_Check_HanddownToGreet(EnZl3* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        this->action = 13;
    } else if (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) <= 0x4300) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.textId = 0x70D5;
        Actor_talk_request(&this->actor, play);
    } else {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

void En_Zl3_inFinal2_Check_GreetToHanddown(EnZl3* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->action = 12;
    }
}

void En_Zl3_inFinal2_setup_Surprise(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_006AB0, 2, -8.0f, 0);
    this->action = 14;
}

void En_Zl3_inFinal2_Check_Animation_Surprise(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_008050, 0, 0.0f, 0);
    }
}

void En_Zl3_inFinal2_setup_Looksword(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_007A78, 2, -8.0f, 0);
    this->action = 15;
}

void En_Zl3_inFinal2_Check_Animation_Looksword(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_007C84, 0, 0.0f, 0);
    }
}

void En_Zl3_inFinal2_setup_Watch(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_0082F8, 2, -8.0f, 0);
    this->action = 16;
    En_Zl3_set_mouth_pattern(this, 0);
}

void En_Zl3_inFinal2_Check_Animation_Watch(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_003FF8, 0, 0.0f, 0);
    }
}

void En_Zl3_inFinal2_Check_WatchToWatch_advice(EnZl3* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        this->action = 0x12;
    } else if (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) <= 0x4300) {
        BossGanon2* bossGanon2 = En_Zl3_Search_Boss_Ganon2(this, play);

        if ((bossGanon2 != NULL) && (bossGanon2->unk_324 <= (10.0f / 81.0f))) {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->actor.textId = 0x7059;
            Actor_talk_request(&this->actor, play);
        }
    } else {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

void En_Zl3_inFinal2_setup_Avert(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_007664, 2, -8.0f, 0);
    this->action = 17;
    En_Zl3_inFinal2_Set_Cry1Sound(this);
}

void En_Zl3_inFinal2_Check_AvertToWatch(EnZl3* this, s32 arg1) {
    s32* unk_2F0 = &this->unk_2F0;

    if (En_Zl3_Get_Command_fromGannon(this) == *unk_2F0) {
        if (arg1 != 0) {
            En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_003FF8, 0, 0.0f, 0);
            this->action = 16;
            En_Zl3_Reset_Command_fromGannon(this, 7);
            this->unk_2F0 = 7;
        }
    }
}

void En_Zl3_inFinal2_Check_Watch_adviceToWatch(EnZl3* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->action = 16;
    }
}

void En_Zl3_inFinal2_setup_Magic(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_0001D8, 0, 0.0f, 0);
    this->action = 19;
}

void En_Zl3_inFinal2_setup_Endmagic(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_001110, 2, -8.0f, 0);
    this->action = 20;
}

void En_Zl3_inFinal2_Check_Animation_Endmagic(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_0004F4, 0, 0.0f, 0);
    }
}

void En_Zl3_inFinal2_Check_EndmagicToAdvice(EnZl3* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        this->action = 21;
    } else if (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) <= 0x4300) {
        BossGanon2* bossGanon2 = En_Zl3_Search_Boss_Ganon2(this, play);

        if (bossGanon2 != NULL) {
            if (bossGanon2->unk_324 <= (10.0f / 81.0f)) {
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                this->actor.textId = 0x7059;
                Actor_talk_request(&this->actor, play);
            }
        }
    } else {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

void En_Zl3_inFinal2_Check_AdviceToEndmagic(EnZl3* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->action = 20;
    }
}

void En_Zl3_inFinal2_setup_Callsage(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_002348, 2, -8.0f, 0);
    this->action = 22;
}

void En_Zl3_inFinal2_Check_Animation_Callsage(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_00210C, 0, 0.0f, 0);
    }
}

void En_Zl3_inFinal2_setup_Seal(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_002E54, 2, -8.0f, 0);
    this->action = 23;
}

void En_Zl3_inFinal2_Check_Animation_Seal(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_002710, 0, 0.0f, 0);
    }
}

void En_Zl3_inFinal2_setup_Disheart(EnZl3* this) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_001D8C, 2, -8.0f, 0);
    this->action = 24;
}

void En_Zl3_inFinal2_Check_Animation_Disheart(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_0014DC, 0, 0.0f, 0);
    }
}

void En_Zl3_inFinal2_Check_DemoMode(EnZl3* this, PlayState* play) {
    static s32 timer = -1;
    s32 temp_v0 = En_Zl3_Get_Command_fromGannon(this);

    if (timer > 0) {
        timer--;
    } else if (timer == 0) {
        timer--;
        if (temp_v0 == 8) {
            En_Zl3_inFinal2_setup_Avert(this);
        }
    }

    if (temp_v0 >= 0) {
        s32 unk_2F0 = this->unk_2F0;

        if (temp_v0 != unk_2F0) {
            switch (temp_v0) {
                case 0:
                    En_Zl3_inFinal2_setup_Stand(this);
                    break;
                case 1:
                    En_Zl3_inFinal2_setup_Press(this);
                    break;
                case 2:
                    En_Zl3_inFinal2_setup_Lookup(this);
                    break;
                case 3:
                    En_Zl3_inFinal2_setup_Frighten(this);
                    break;
                case 4:
                    En_Zl3_inFinal2_setup_Handdown(this);
                    break;
                case 5:
                    En_Zl3_inFinal2_setup_Surprise(this);
                    break;
                case 6:
                    En_Zl3_inFinal2_setup_Looksword(this);
                    break;
                case 7:
                    En_Zl3_inFinal2_setup_Watch(this);
                    break;
                case 8:
                    timer = 10;
                    break;
                case 9:
                    En_Zl3_inFinal2_setup_Magic(this);
                    break;
                case 10:
                    En_Zl3_inFinal2_setup_Endmagic(this);
                    break;
                case 11:
                    En_Zl3_inFinal2_setup_Callsage(this);
                    break;
                case 12:
                    En_Zl3_inFinal2_setup_Seal(this);
                    break;
                case 13:
                    En_Zl3_inFinal2_setup_Disheart(this);
                    break;
                case 14:
                    Actor_delete(&this->actor);
                    break;
                default:
                    PRINTF("En_Zl3_inFinal2_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->unk_2F0 = temp_v0;
        }
    }
}

void En_Zl3_inFinal2_main_stand(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inFinal2_Check_DemoMode(this, play);
}

void En_Zl3_inFinal2_main_press(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_inFinal2_Check_Animation_Press(this, En_Zl3_Animation_Base(this));
    En_Zl3_inFinal2_Check_DemoMode(this, play);
}

void En_Zl3_inFinal2_main_lookup(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_inFinal2_Check_Animation_Lookup(this, En_Zl3_Animation_Base(this));
    En_Zl3_inFinal2_Check_DemoMode(this, play);
}

void En_Zl3_inFinal2_main_frighten(EnZl3* this, PlayState* play) {
    s32 something;

    En_Zl3_Change_Bank(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    something = En_Zl3_Animation_Base(this);
    En_Zl3_inFinal2_Movement_Frighten(this);
    En_Zl3_inFinal2_Check_FrightenToRelief(this, something);
}

void En_Zl3_inFinal2_main_relief(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inFinal2_Check_DemoMode(this, play);
}

void En_Zl3_inFinal2_main_handdown(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_inFinal2_SetAngle_forLink_handdown(this, play);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_inFinal2_Check_Animation_Handdown(this, En_Zl3_Animation_Base(this));
    En_Zl3_inFinal2_Check_DemoMode(this, play);
    En_Zl3_inFinal2_Check_HanddownToGreet(this, play);
}

void En_Zl3_inFinal2_main_greet(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_SetAngle_forLink(this);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inFinal2_Check_GreetToHanddown(this, play);
}

void En_Zl3_inFinal2_main_surprise(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_inFinal2_Check_Animation_Surprise(this, En_Zl3_Animation_Base(this));
    En_Zl3_inFinal2_Check_DemoMode(this, play);
}

void En_Zl3_inFinal2_main_looksword(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_inFinal2_Check_Animation_Looksword(this, En_Zl3_Animation_Base(this));
    En_Zl3_inFinal2_Set_LookSwordSound(this);
    En_Zl3_inFinal2_Check_DemoMode(this, play);
}

void En_Zl3_inFinal2_main_watch(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_SetAngle_forLink(this);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_inFinal2_Check_Animation_Watch(this, En_Zl3_Animation_Base(this));
    En_Zl3_inFinal2_Check_DemoMode(this, play);
    En_Zl3_inFinal2_Check_WatchToWatch_advice(this, play);
}

void En_Zl3_inFinal2_main_avert(EnZl3* this, PlayState* play) {
    s32 something;

    En_Zl3_Change_Bank(this, play);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_SetAngle_forLink(this);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    something = En_Zl3_Animation_Base(this);
    En_Zl3_inFinal2_Check_DemoMode(this, play);
    En_Zl3_inFinal2_Check_AvertToWatch(this, something);
}

void En_Zl3_inFinal2_main_watch_advice(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_SetAngle_forLink(this);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_inFinal2_Check_Animation_Watch(this, En_Zl3_Animation_Base(this));
    En_Zl3_inFinal2_Check_Watch_adviceToWatch(this, play);
}

void En_Zl3_inFinal2_main_Magic(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inFinal2_Check_DemoMode(this, play);
}

void En_Zl3_inFinal2_main_Endmagic(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_inFinal2_Check_Animation_Endmagic(this, En_Zl3_Animation_Base(this));
    En_Zl3_inFinal2_Check_DemoMode(this, play);
    En_Zl3_inFinal2_Check_EndmagicToAdvice(this, play);
}

void En_Zl3_inFinal2_main_Advice(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inFinal2_Check_AdviceToEndmagic(this, play);
}

void En_Zl3_inFinal2_main_Callsage(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_inFinal2_Check_Animation_Callsage(this, En_Zl3_Animation_Base(this));
    En_Zl3_inFinal2_Check_DemoMode(this, play);
}

void En_Zl3_inFinal2_main_Seal(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_inFinal2_Check_Animation_Seal(this, En_Zl3_Animation_Base(this));
    En_Zl3_inFinal2_Check_DemoMode(this, play);
}

void En_Zl3_inFinal2_main_Disheart(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_inFinal2_Check_Animation_Disheart(this, En_Zl3_Animation_Base(this));
    En_Zl3_inFinal2_Check_DemoMode(this, play);
}
