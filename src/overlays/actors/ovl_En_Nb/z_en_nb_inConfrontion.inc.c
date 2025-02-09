void En_Nb_Confrontion_Init(EnNb* this, PlayState* play) {
    AnimationHeader* animation = &gNabooruCollapseFromStandingToKneelingTransitionAnim;

    En_Nb_Change_Anime(this, animation, 0, 0.0f, 0);
    this->action = NB_IN_CONFRONTATION;
    this->actor.shape.shadowAlpha = 0;
}

void En_Nb_Confrontion_Set_CrySound(EnNb* this, PlayState* play) {
    s32 pad[2];

    if (play->csCtx.curFrame == 548) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_NB_CRY_0);
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_EN_FANTOM_HIT_THUNDER);
    }
}

void En_Nb_Confrontion_SetSound_Kneel(EnNb* this) {
    s32 pad[2];

    if ((this->skelAnime.mode == 2) &&
        (Skeleton_Info_frame_check(&this->skelAnime, 18.0f) || Skeleton_Info_frame_check(&this->skelAnime, 25.0f))) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_EV_HUMAN_BOUND);
    }
}

void En_Nb_Confrontion_SetSound_Turn_right(EnNb* this) {
    s32 pad[2];

    if ((this->skelAnime.mode == 2) && Skeleton_Info_frame_check(&this->skelAnime, 9.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_STONE);
    }
}

void En_Nb_Confrontion_SetSound_Away(EnNb* this) {
    s32 pad[2];

    if (Skeleton_Info_frame_check(&this->skelAnime, 9.0f) || Skeleton_Info_frame_check(&this->skelAnime, 13.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_STONE);
    }
}

void Birth_Demo_6k_In_En_Nb_Confrontion(EnNb* this, PlayState* play) {
    Actor_info_make_actor(&play->actorCtx, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                kREG(21) + 22.0f + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0xB);
}

void En_Nb_Confrontion_Movement_byAnimation(EnNb* this, PlayState* play) {
    this->skelAnime.movementFlags |= ANIM_FLAG_UPDATE_XZ;
    Skeleton_Proc_Anime_Move_init(play, &this->actor, &this->skelAnime, 1.0f);
}

void En_Nb_Confrontion_setup_Wait(EnNb* this) {
    this->action = NB_IN_CONFRONTATION;
    this->drawMode = NB_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

void En_Nb_Confrontion_setup_Absence(EnNb* this, PlayState* play) {
    s32 pad;
    AnimationHeader* animation = &gNabooruCollapseFromStandingToKneelingTransitionAnim;
    f32 lastFrame = Si2_anime_end_frame(animation);

    En_Nb_Set_DemoStartPosAngle(this, play, 1);
    Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    this->action = NB_ACTION_14;
    this->drawMode = NB_DRAW_KNEEL;
    this->actor.shape.shadowAlpha = 0xFF;
}

void En_Nb_Confrontion_setup_Kneel(EnNb* this) {
    AnimationHeader* animation = &gNabooruCollapseFromStandingToKneelingTransitionAnim;
    f32 lastFrame = Si2_anime_end_frame(animation);

    Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    this->action = NB_KNEEL;
    this->drawMode = NB_DRAW_KNEEL;
    this->actor.shape.shadowAlpha = 0xFF;
}

void En_Nb_Kidnap_Check_Animation_kneel(EnNb* this, s32 animFinished) {
    AnimationHeader* animation = &gNabooruOnAllFoursAnim;

    if (animFinished) {
        Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, Si2_anime_end_frame(animation), ANIMMODE_LOOP,
                         0.0f);
        this->drawMode = NB_DRAW_KNEEL;
    }
}

void En_Nb_Confrontion_setup_Turn_right(EnNb* this) {
    AnimationHeader* animation = &gNabooruOnAllFoursToOnOneKneeLookingRightTransitionAnim;
    f32 lastFrame = Si2_anime_end_frame(animation);

    Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -8.0f);
    this->action = NB_LOOK_RIGHT;
    this->drawMode = NB_DRAW_DEFAULT;
    this->actor.shape.shadowAlpha = 0xFF;
}

void En_Nb_Kidnap_Check_Animation_turn_right(EnNb* this, s32 animFinished) {
    AnimationHeader* animation = &gNabooruOnOneKneeLookingRightAnim;

    if (animFinished) {
        Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, Si2_anime_end_frame(animation), ANIMMODE_LOOP,
                         0.0f);
        this->drawMode = NB_DRAW_LOOK_DIRECTION;
    }
}

void En_Nb_Confrontion_setup_Turn_left(EnNb* this) {
    AnimationHeader* animation = &gNabooruOnOneKneeTurningHeadRightToLeftTransitionAnim;
    f32 lastFrame = Si2_anime_end_frame(animation);

    Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -8.0f);
    this->action = NB_LOOK_LEFT;
    this->drawMode = NB_DRAW_LOOK_DIRECTION;
    this->actor.shape.shadowAlpha = 0xFF;
}

void En_Nb_Kidnap_Check_Animation_turn_left(EnNb* this, s32 animFinished) {
    AnimationHeader* animation = &gNabooruOnOneKneeLookingLeftAnim;

    if (animFinished) {
        Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, Si2_anime_end_frame(animation), ANIMMODE_LOOP,
                         0.0f);
    }
}

void Set_Demo_6k_In_En_Nb_Confrontion(EnNb* this, PlayState* play, s32 animFinished) {
    if (!this->flag && animFinished) {
        Birth_Demo_6k_In_En_Nb_Confrontion(this, play);
        this->flag = 1;
    }
}

void En_Nb_Confrontion_setup_Away(EnNb* this) {
    AnimationHeader* animation = &gNabooruKneeingToRunningToHitAnim;
    f32 lastFrame = Si2_anime_end_frame(animation);

    Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -8.0f);
    this->action = NB_RUN;
    this->drawMode = NB_DRAW_LOOK_DIRECTION;
    this->actor.shape.shadowAlpha = 0xFF;
}

void En_Nb_Confrontion_setup_Disappear(EnNb* this) {
    this->action = NB_CONFRONTATION_DESTROYED;
    this->drawMode = NB_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

void En_Nb_Confrontion_Check_DemoMode(EnNb* this, PlayState* play) {
    CsCmdActorCue* cue;
    s32 nextCueId;
    s32 currentCueId;

    cue = En_Nb_Get_npcdemopnt(play, 1);

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    En_Nb_Confrontion_setup_Wait(this);
                    break;
                case 10:
                    En_Nb_Confrontion_setup_Absence(this, play);
                    break;
                case 11:
                    En_Nb_Confrontion_setup_Kneel(this);
                    break;
                case 12:
                    En_Nb_Confrontion_setup_Turn_right(this);
                    break;
                case 13:
                    En_Nb_Confrontion_setup_Turn_left(this);
                    break;
                case 14:
                    En_Nb_Confrontion_setup_Away(this);
                    break;
                case 9:
                    En_Nb_Confrontion_setup_Disappear(this);
                    break;
                default:
                    // "En_Nb_Confrontion_Check_DemoMode: Operation doesn't exist!!!!!!!!"
                    PRINTF("En_Nb_Confrontion_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->cueId = nextCueId;
        }
    }
}

void En_Nb_Confrontion_Actor_main_hide(EnNb* this, PlayState* play) {
    En_Nb_Confrontion_Check_DemoMode(this, play);
}

void En_Nb_Confrontion_Actor_main_Absence(EnNb* this, PlayState* play) {
    En_Nb_set_eye_pattern(this);
    En_Nb_Confrontion_Movement_byAnimation(this, play);
    En_Nb_BGcheck(this, play);
    En_Nb_Confrontion_Check_DemoMode(this, play);
}

void En_Nb_Confrontion_Actor_main_Kneel(EnNb* this, PlayState* play) {
    s32 animFinished;

    En_Nb_set_eye_pattern(this);
    animFinished = En_Nb_Animation_Base(this);
    En_Nb_Kidnap_Check_Animation_kneel(this, animFinished);
    En_Nb_Confrontion_SetSound_Kneel(this);
    En_Nb_Confrontion_Movement_byAnimation(this, play);
    En_Nb_BGcheck(this, play);
    En_Nb_Confrontion_Check_DemoMode(this, play);
}

void En_Nb_Confrontion_Actor_main_Turn_right(EnNb* this, PlayState* play) {
    s32 animFinished;

    En_Nb_set_eye_pattern(this);
    animFinished = En_Nb_Animation_Base(this);
    En_Nb_Kidnap_Check_Animation_turn_right(this, animFinished);
    En_Nb_Confrontion_SetSound_Turn_right(this);
    En_Nb_Confrontion_Movement_byAnimation(this, play);
    En_Nb_BGcheck(this, play);
    En_Nb_Confrontion_Check_DemoMode(this, play);
}

void En_Nb_Confrontion_Actor_main_Turn_left(EnNb* this, PlayState* play) {
    s32 animFinished;

    En_Nb_set_eye_pattern(this);
    animFinished = En_Nb_Animation_Base(this);
    En_Nb_Kidnap_Check_Animation_turn_left(this, animFinished);
    En_Nb_Confrontion_Movement_byAnimation(this, play);
    En_Nb_BGcheck(this, play);
    En_Nb_Confrontion_Check_DemoMode(this, play);
}

void En_Nb_Confrontion_Actor_main_Away(EnNb* this, PlayState* play) {
    s32 animFinished;

    En_Nb_Confrontion_Set_CrySound(this, play);
    En_Nb_set_eye_pattern(this);
    animFinished = En_Nb_Animation_Base(this);
    En_Nb_Confrontion_SetSound_Away(this);
    En_Nb_Confrontion_Movement_byAnimation(this, play);
    En_Nb_BGcheck(this, play);
    Set_Demo_6k_In_En_Nb_Confrontion(this, play, animFinished);
    En_Nb_Confrontion_Check_DemoMode(this, play);
}

void En_Nb_Confrontion_Actor_main_Disappear(EnNb* this, PlayState* play) {
    this->timer++;

    if (this->timer > 60.0f) {
        Actor_delete(&this->actor);
    }
}

void En_Nb_draw_surprise_eye(EnNb* this, PlayState* play) {
    s32 pad;
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_nb_inConfrontion.c", 572);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gNabooruEyeWideTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gNabooruEyeWideTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &Actor_change_render_mode[2]);
    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL, NULL,
                          &this->actor);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_nb_inConfrontion.c", 593);
}

s32 En_Nb_SetDraw_head(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnNb* this = (EnNb*)thisx;

    if (limbIndex == NB_LIMB_HEAD) {
        *dList = gNabooruHeadMouthOpenDL;
    }

    return 0;
}

void En_Nb_draw_surprise_head(EnNb* this, PlayState* play) {
    s32 pad;
    s16 eyeIdx = this->eyeIdx;
    SkelAnime* skelAnime = &this->skelAnime;
    void* eyeTexture = en_nb_eye[eyeIdx];
    s32 pad1;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_nb_inConfrontion.c", 623);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &Actor_change_render_mode[2]);
    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, En_Nb_SetDraw_head, NULL,
                          &this->actor);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_nb_inConfrontion.c", 644);
}
