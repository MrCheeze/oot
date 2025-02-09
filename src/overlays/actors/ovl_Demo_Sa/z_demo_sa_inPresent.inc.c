void Demo_Sa_Present_Init(DemoSa* this, PlayState* play) {
    Vec3f* thisPos = &this->actor.world.pos;

    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gSariaSkel, &gSariaWaitOnBridgeAnim, NULL, NULL, 0);
    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_ELF, thisPos->x, thisPos->y, thisPos->z, 0, 0, 0,
                       FAIRY_KOKIRI);
    this->action = 16;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
    Demo_Sa_set_eye_Num(this, SARIA_EYE_SAD);
    Demo_Sa_set_mouth_Num(this, SARIA_MOUTH_CLOSED);
}

void Demo_Sa_inPresent_Set_Alpha(DemoSa* this) {
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

void Demo_Sa_inPresent_setup_Wait(DemoSa* this) {
    this->action = 16;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void Demo_Sa_inPresent_setup_Stand(DemoSa* this, PlayState* play) {
    if (this->cueId == 4) {
        Demo_Sa_Set_StartPos_npcdemopnt(this, play, 1);
        this->action = 17;
        this->drawConfig = 2;
        this->unk_1B0 = 0;
        this->actor.shape.shadowAlpha = 0;
    } else {
        Demo_Sa_Change_Anime(this, &gSariaWaitOnBridgeAnim, 0, 0.0f, 0);
        this->action = 18;
        this->drawConfig = 1;
        this->unk_1B0 = 0;
        this->actor.shape.shadowAlpha = 0xFF;
    }
    Demo_Sa_set_eye_Num(this, SARIA_EYE_SAD);
}

void Demo_Sa_inPresent_check_AlphaToStand(DemoSa* this) {
    if (this->unk_1A0 >= kREG(17) + 10.0f) {
        this->action = 18;
        this->drawConfig = 1;
        this->unk_1B0 = 0;
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void Demo_Sa_inPresent_setup_Hold(DemoSa* this) {
    Demo_Sa_Change_Anime(this, &gSariaHoldOcarinaAnim, 0, 0.0f, 0);
    this->action = 19;
    this->drawConfig = 1;
    this->unk_1B0 = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    Demo_Sa_set_eye_Num(this, SARIA_EYE_CLOSED);
}

void Demo_Sa_inPresent_setup_Send(DemoSa* this) {
    Demo_Sa_Change_Anime(this, &gSariaGiveLinkOcarinaAnim, 2, -8.0f, 0);
    this->action = 20;
    this->drawConfig = 1;
    this->unk_1B0 = 1;
    this->actor.shape.shadowAlpha = 0xFF;
}

void Demo_Sa_inPresent_Check_LookdownTostand(DemoSa* this, s32 arg1) {
    if (arg1 != 0) {
        Demo_Sa_Change_Anime(this, &gSariaHoldOutOcarinaAnim, 0, 0, 0);
    }
}

void Demo_Sa_inPresent_Check_DemoMode(DemoSa* this, PlayState* play) {
    s32 currentCueId;
    s32 nextCueId;
    CsCmdActorCue* cue = Demo_Sa_Get_npcdemopnt(play, 1);

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 4:
                    Demo_Sa_inPresent_setup_Wait(this);
                    break;
                case 12:
                    Demo_Sa_inPresent_setup_Stand(this, play);
                    break;
                case 13:
                    Demo_Sa_inPresent_setup_Hold(this);
                    break;
                case 14:
                    Demo_Sa_inPresent_setup_Send(this);
                    break;
                default:
                    PRINTF("Demo_Sa_inPresent_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->cueId = nextCueId;
        }
    }
}

void Demo_Sa_inPresent_main_wait(DemoSa* this, PlayState* play) {
    Demo_Sa_inPresent_Check_DemoMode(this, play);
}

void Demo_Sa_inPresent_main_alpha(DemoSa* this, PlayState* play) {
    Demo_Sa_BGcheck(this, play);
    Demo_Sa_Animation_Base(this);
    Demo_Sa_inPresent_Set_Alpha(this);
    Demo_Sa_inPresent_check_AlphaToStand(this);
}

void Demo_Sa_inPresent_main_stand(DemoSa* this, PlayState* play) {
    Demo_Sa_BGcheck(this, play);
    Demo_Sa_Animation_Base(this);
    Demo_Sa_inPresent_Check_DemoMode(this, play);
}

void Demo_Sa_inPresent_main_hold(DemoSa* this, PlayState* play) {
    Demo_Sa_BGcheck(this, play);
    Demo_Sa_Animation_Base(this);
    Demo_Sa_inPresent_Check_DemoMode(this, play);
}

void Demo_Sa_inPresent_main_send(DemoSa* this, PlayState* play) {
    Demo_Sa_BGcheck(this, play);
    Demo_Sa_inPresent_Check_LookdownTostand(this, Demo_Sa_Animation_Base(this));
    Demo_Sa_inPresent_Check_DemoMode(this, play);
}
