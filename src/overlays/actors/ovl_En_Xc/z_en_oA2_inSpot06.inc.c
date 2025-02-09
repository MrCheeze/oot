void En_Oa2_Actor_Spot06_Init(EnXc* this, PlayState* play) {
    this->action = SHEIK_ACTION_45;
}

void En_Oa2_Spot06_Set_FallSound(void) {
    Na_StartFixSe_F(NA_SE_PL_SKIP);
}

void En_Oa2_Spot06_Set_DiveSound(Vec3f* src, PlayState* play) {
    static Vec3f display_position;
    f32 wDest[2];

    Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, src, &display_position, wDest);
    Na_StartObjectSe_F(&display_position, NA_SE_EV_DIVE_INTO_WATER);
}

void En_Oa2_Spot06_Set_Splash(PlayState* play) {
    CsCmdActorCue* cue = cue = En_Oa2_Get_npcdemopnt(play, 0);

    if (cue != NULL) {
        Vec3f startPos;

        startPos.x = cue->startPos.x;
        startPos.y = cue->startPos.y;
        startPos.z = cue->startPos.z;

        Effect_SS_G_Ripple_ct2(play, &startPos, 100, 500, 0);
        Effect_SS_G_Ripple_ct2(play, &startPos, 100, 500, 10);
        Effect_SS_G_Ripple_ct2(play, &startPos, 100, 500, 20);
        Effect_SS_G_Splash_sc_cl_ct(play, &startPos, NULL, NULL, 1, 0);
        En_Oa2_Spot06_Set_DiveSound(&startPos, play);
    }
}

void En_Oa2_Spot06_Set_Door_Warp1_forLink(PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;
    u16 csCurFrame = csCtx->curFrame;

    if (csCurFrame == 310) {
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_DOOR_WARP1, -1044.0f, -1243.0f, 7458.0f, 0, 0, 0, WARP_DESTINATION);
    }
}

void En_Oa2_Spot06_Effect(EnXc* this, PlayState* play) {
    static s32 old_dousa = 1;
    CsCmdActorCue* cue = En_Oa2_Get_npcdemopnt(play, 0);

    if (cue != NULL) {
        s32 nextCueId = cue->id;

        if (nextCueId != old_dousa) {
            switch (nextCueId) {
                case 2:
                    En_Oa2_Spot06_Set_FallSound();
                    break;
                case 3:
                    En_Oa2_Spot06_Set_Splash(play);
                    break;
                default:
                    break;
            }
            old_dousa = nextCueId;
        }
    }
}

void En_Oa2_Spot06_check_WaitToHide(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_45, SHEIK_ACTION_46);
}

void En_Oa2_Spot06_check_HideToGreet(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_46, SHEIK_ACTION_47);
}

void En_Oa2_Spot06_check_GreetToAway_accel(EnXc* this, PlayState* play) {
    if (En_Oa2_Check_npcdemopnt(this, play, 4, 4)) {
        En_Oa2_Change_Anime(this, &gSheikWalkingAnim, ANIMMODE_LOOP, -12.0f, true);
        this->action = SHEIK_ACTION_48;
        this->actor.world.rot.y += 0x8000;
        this->timer = 0.0f;
    }
}

void En_Oa2_Spot06_check_Away_accelToAway_move(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_48, SHEIK_ACTION_49);
}

void En_Oa2_Spot06_check_Away_moveToSitdown(EnXc* this, PlayState* play) {
    if (En_Oa2_Check_npcdemopnt(this, play, 16, 4)) {
        En_Oa2_Change_Anime(this, &gSheikKneelingAnim, ANIMMODE_LOOP, 0.0f, false);
        this->action = SHEIK_ACTION_KNEEL;
    }
}

void En_Oa2_Spot06_check_SitdownToDiving(EnXc* this, PlayState* play) {
    if (En_Oa2_Check_npcdemopnt(this, play, 22, 4)) {
        En_Oa2_Change_Anime(this, &gSheikAnim_01A048, ANIMMODE_LOOP, 0.0f, false);
        this->action = SHEIK_ACTION_51;
        En_Oa2_Set_DemoStartPosAngle(this, play, 4);
    }
}

void En_Oa2_Spot06_check_DivingToDisappear(EnXc* this, PlayState* play) {
    if (En_Oa2_Check_npcdemopnt(this, play, 9, 4)) {
        this->action = SHEIK_ACTION_52;
        this->drawMode = SHEIK_DRAW_NOTHING;
    }
}

void En_Oa2_Spot06_Actor_main_wait(EnXc* this, PlayState* play) {
    En_Oa2_Toki_Actor_main_wait(this, play);
    En_Oa2_Spot06_check_WaitToHide(this);
}

void En_Oa2_Spot06_Actor_main_hide(EnXc* this, PlayState* play) {
    En_Oa2_Toki_Actor_main_hide(this, play);
    En_Oa2_Spot06_check_HideToGreet(this);
}

void En_Oa2_Spot06_Actor_main_greet(EnXc* this, PlayState* play) {
    En_Oa2_Spot06_Set_Door_Warp1_forLink(play);
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_Set_DemoStartPosAngle(this, play, 4);
    En_Oa2_Spot06_check_GreetToAway_accel(this, play);
}

void En_Oa2_Spot06_Actor_main_away_accel(EnXc* this, PlayState* play) {
    En_Oa2_Toki_Actor_main_away_accel(this, play);
    En_Oa2_Spot06_check_Away_accelToAway_move(this);
}

void En_Oa2_Spot06_Actor_main_away_move(EnXc* this, PlayState* play) {
    En_Oa2_Movement_Away_move(this);
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Set_WalkSound(this, play);
    En_Oa2_Spot06_check_Away_moveToSitdown(this, play);
}

void En_Oa2_Spot06_Actor_main_sitdown(EnXc* this, PlayState* play) {
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_Spot06_Effect(this, play);
    En_Oa2_Set_DemoStartPosAngle(this, play, 4);
    En_Oa2_Spot06_check_SitdownToDiving(this, play);
}

void En_Oa2_Spot06_Actor_main_diving(EnXc* this, PlayState* play) {
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_Spot06_Effect(this, play);
    En_Oa2_Movement_fromData(this, play, 4);
    En_Oa2_Spot06_check_DivingToDisappear(this, play);
}

void En_Oa2_Spot06_Actor_main_disappear(EnXc* this, PlayState* play) {
    En_Oa2_Spot06_Effect(this, play);
}
