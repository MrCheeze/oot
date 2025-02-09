void func_80B55780(EnZl3* this, PlayState* play) {
    PRINTF("ゼルダ姫のEn_Zl3_Actor_inFinal2_Init通すよ!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    func_80B54E14(this, &gZelda2Anime2Anim_005A0C, 0, 0.0f, 0);
    this->action = 7;
    this->drawConfig = 1;
    PRINTF("ゼルダ姫のEn_Zl3_Actor_inFinal2_Initは通った!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    EnZl3_setMouthIndex(this, 1);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void func_80B55808(EnZl3* this) {
    Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_VO_Z1_PAIN);
}

static Vec3f D_80B5A488 = { 0.0f, 0.0f, 0.0f };

void func_80B5582C(EnZl3* this) {
    Audio_PlaySfxRandom(&D_80B5A488, NA_SE_VO_Z1_CRY_0, NA_SE_VO_Z1_CRY_1 - NA_SE_VO_Z1_CRY_0 + 1);
}

void func_80B5585C(EnZl3* this) {
    SkelAnime* skelAnime = &this->skelAnime;

    if ((skelAnime->mode == 2) && Animation_OnFrame(skelAnime, 4.0f)) {
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_VO_Z1_PAIN);
    }
}

void func_80B558A8(EnZl3* this) {
    s32 pad[4];
    s16 thisRotY = this->actor.world.rot.y;
    Vec3f* unk_338 = &this->unk_338;

    *unk_338 = this->unk_32C = this->actor.world.pos;

    unk_338->z += (-1.6074f * Math_CosS(thisRotY)) - (3.1620007f * Math_SinS(thisRotY));
    unk_338->x += (-1.6074f * Math_SinS(thisRotY)) + (3.1620007f * Math_CosS(thisRotY));
    unk_338->y += -0.012199402f;
}

void func_80B559C4(EnZl3* this) {
    Vec3f* thisPos = &this->actor.world.pos;
    Vec3f* unk_32C = &this->unk_32C;
    Vec3f* unk_338 = &this->unk_338;
    f32 temp_f0 = Environment_LerpWeightAccelDecel(Animation_GetLastFrame(&gZelda2Anime2Anim_005248), 0,
                                                   (s32)this->skelAnime.curFrame, 3, 3);

    thisPos->x = unk_32C->x + (temp_f0 * (unk_338->x - unk_32C->x));
    thisPos->z = unk_32C->z + (temp_f0 * (unk_338->z - unk_32C->z));
}

void func_80B55A58(EnZl3* this, PlayState* play) {
    if (play->activeCamId == CAM_ID_MAIN) {
        func_80B537E8(this);
    }
}

void func_80B55A84(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_005A0C, 0, 0.0f, 0);
    this->action = 7;
}

void func_80B55AC4(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_00499C, 2, -8.0f, 0);
    this->action = 8;
}

void func_80B55B04(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(this, &gZelda2Anime2Anim_004408, 0, 0.0f, 0);
    }
}

void func_80B55B38(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_006508, 2, -8.0f, 0);
    this->action = 9;
}

void func_80B55B78(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(this, &gZelda2Anime2Anim_0061C4, 0, 0.0f, 0);
    }
}

void func_80B55BAC(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_005248, 2, -8.0f, 0);
    func_80B558A8(this);
    func_80B55808(this);
    EnZl3_setMouthIndex(this, 2);
    this->action = 10;
}

void func_80B55C0C(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_0054E0, 0, 0.0f, 0);
    this->action = 11;
}

void func_80B55C4C(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B55C0C(this);
    }
}

void func_80B55C70(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_008684, 2, -8.0f, 0);
    this->action = 12;
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void func_80B55CCC(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(this, &gZelda2Anime2Anim_006F04, 0, 0.0f, 0);
    }
}

void func_80B55D00(EnZl3* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, play)) {
        this->action = 13;
    } else if (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) <= 0x4300) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.textId = 0x70D5;
        Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
    } else {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

void func_80B55DB0(EnZl3* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->action = 12;
    }
}

void func_80B55E08(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_006AB0, 2, -8.0f, 0);
    this->action = 14;
}

void func_80B55E48(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(this, &gZelda2Anime2Anim_008050, 0, 0.0f, 0);
    }
}

void func_80B55E7C(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_007A78, 2, -8.0f, 0);
    this->action = 15;
}

void func_80B55EBC(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(this, &gZelda2Anime2Anim_007C84, 0, 0.0f, 0);
    }
}

void func_80B55EF0(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_0082F8, 2, -8.0f, 0);
    this->action = 16;
    EnZl3_setMouthIndex(this, 0);
}

void func_80B55F38(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(this, &gZelda2Anime2Anim_003FF8, 0, 0.0f, 0);
    }
}

void func_80B55F6C(EnZl3* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, play)) {
        this->action = 0x12;
    } else if (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) <= 0x4300) {
        BossGanon2* bossGanon2 = func_80B53488(this, play);

        if ((bossGanon2 != NULL) && (bossGanon2->unk_324 <= (10.0f / 81.0f))) {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->actor.textId = 0x7059;
            Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
        }
    } else {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

void func_80B5604C(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_007664, 2, -8.0f, 0);
    this->action = 17;
    func_80B5582C(this);
}

void func_80B56090(EnZl3* this, s32 arg1) {
    s32* unk_2F0 = &this->unk_2F0;

    if (func_80B5396C(this) == *unk_2F0) {
        if (arg1 != 0) {
            func_80B54E14(this, &gZelda2Anime2Anim_003FF8, 0, 0.0f, 0);
            this->action = 16;
            func_80B53974(this, 7);
            this->unk_2F0 = 7;
        }
    }
}

void func_80B56108(EnZl3* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->action = 16;
    }
}

void func_80B56160(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_0001D8, 0, 0.0f, 0);
    this->action = 19;
}

void func_80B561A0(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_001110, 2, -8.0f, 0);
    this->action = 20;
}

void func_80B561E0(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(this, &gZelda2Anime2Anim_0004F4, 0, 0.0f, 0);
    }
}

void func_80B56214(EnZl3* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, play)) {
        this->action = 21;
    } else if (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) <= 0x4300) {
        BossGanon2* bossGanon2 = func_80B53488(this, play);

        if (bossGanon2 != NULL) {
            if (bossGanon2->unk_324 <= (10.0f / 81.0f)) {
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                this->actor.textId = 0x7059;
                Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
            }
        }
    } else {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

void func_80B562F4(EnZl3* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->action = 20;
    }
}

void func_80B5634C(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_002348, 2, -8.0f, 0);
    this->action = 22;
}

void func_80B5638C(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(this, &gZelda2Anime2Anim_00210C, 0, 0.0f, 0);
    }
}

void func_80B563C0(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_002E54, 2, -8.0f, 0);
    this->action = 23;
}

void func_80B56400(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(this, &gZelda2Anime2Anim_002710, 0, 0.0f, 0);
    }
}

void func_80B56434(EnZl3* this) {
    func_80B54E14(this, &gZelda2Anime2Anim_001D8C, 2, -8.0f, 0);
    this->action = 24;
}

void func_80B56474(EnZl3* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(this, &gZelda2Anime2Anim_0014DC, 0, 0.0f, 0);
    }
}

void func_80B564A8(EnZl3* this, PlayState* play) {
    static s32 D_80B5A494 = -1;
    s32 temp_v0 = func_80B5396C(this);

    if (D_80B5A494 > 0) {
        D_80B5A494--;
    } else if (D_80B5A494 == 0) {
        D_80B5A494--;
        if (temp_v0 == 8) {
            func_80B5604C(this);
        }
    }

    if (temp_v0 >= 0) {
        s32 unk_2F0 = this->unk_2F0;

        if (temp_v0 != unk_2F0) {
            switch (temp_v0) {
                case 0:
                    func_80B55A84(this);
                    break;
                case 1:
                    func_80B55AC4(this);
                    break;
                case 2:
                    func_80B55B38(this);
                    break;
                case 3:
                    func_80B55BAC(this);
                    break;
                case 4:
                    func_80B55C70(this);
                    break;
                case 5:
                    func_80B55E08(this);
                    break;
                case 6:
                    func_80B55E7C(this);
                    break;
                case 7:
                    func_80B55EF0(this);
                    break;
                case 8:
                    D_80B5A494 = 10;
                    break;
                case 9:
                    func_80B56160(this);
                    break;
                case 10:
                    func_80B561A0(this);
                    break;
                case 11:
                    func_80B5634C(this);
                    break;
                case 12:
                    func_80B563C0(this);
                    break;
                case 13:
                    func_80B56434(this);
                    break;
                case 14:
                    Actor_Kill(&this->actor);
                    break;
                default:
                    PRINTF("En_Zl3_inFinal2_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->unk_2F0 = temp_v0;
        }
    }
}

void func_80B56658(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B564A8(this, play);
}

void func_80B566AC(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    func_80B55B04(this, EnZl3_UpdateSkelAnime(this));
    func_80B564A8(this, play);
}

void func_80B5670C(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    func_80B55B78(this, EnZl3_UpdateSkelAnime(this));
    func_80B564A8(this, play);
}

void func_80B5676C(EnZl3* this, PlayState* play) {
    s32 something;

    func_80B54DE0(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    something = EnZl3_UpdateSkelAnime(this);
    func_80B559C4(this);
    func_80B55C4C(this, something);
}

void func_80B567CC(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B564A8(this, play);
}

void func_80B5682C(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B55A58(this, play);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    func_80B55CCC(this, EnZl3_UpdateSkelAnime(this));
    func_80B564A8(this, play);
    func_80B55D00(this, play);
}

void func_80B568B4(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B537E8(this);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B55DB0(this, play);
}

void func_80B5691C(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    func_80B55E48(this, EnZl3_UpdateSkelAnime(this));
    func_80B564A8(this, play);
}

void func_80B5697C(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    func_80B55EBC(this, EnZl3_UpdateSkelAnime(this));
    func_80B5585C(this);
    func_80B564A8(this, play);
}

void func_80B569E4(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B533FC(this, play);
    func_80B537E8(this);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    func_80B55F38(this, EnZl3_UpdateSkelAnime(this));
    func_80B564A8(this, play);
    func_80B55F6C(this, play);
}

void func_80B56A68(EnZl3* this, PlayState* play) {
    s32 something;

    func_80B54DE0(this, play);
    func_80B533FC(this, play);
    func_80B537E8(this);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    something = EnZl3_UpdateSkelAnime(this);
    func_80B564A8(this, play);
    func_80B56090(this, something);
}

void func_80B56AE0(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B533FC(this, play);
    func_80B537E8(this);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    func_80B55F38(this, EnZl3_UpdateSkelAnime(this));
    func_80B56108(this, play);
}

void func_80B56B54(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B564A8(this, play);
}

void func_80B56BA8(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    func_80B561E0(this, EnZl3_UpdateSkelAnime(this));
    func_80B564A8(this, play);
    func_80B56214(this, play);
}

void func_80B56C24(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B562F4(this, play);
}

void func_80B56C84(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    func_80B5638C(this, EnZl3_UpdateSkelAnime(this));
    func_80B564A8(this, play);
}

void func_80B56CE4(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    func_80B56400(this, EnZl3_UpdateSkelAnime(this));
    func_80B564A8(this, play);
}

void func_80B56D44(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    func_80B56474(this, EnZl3_UpdateSkelAnime(this));
    func_80B564A8(this, play);
}
