void En_Oa2_Actor_Stalker_Init(EnXc* this, PlayState* play) {
    this->action = SHEIK_ACTION_NOCTURNE_INIT;
    this->drawMode = SHEIK_DRAW_SQUINT;
}

void En_Oa2_Stalker_Set_LandSound(EnXc* this) {
    SkelAnime* skelAnime = &this->skelAnime;

    if (Skeleton_Info_frame_check(skelAnime, 9.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_PL_BOUND + SURFACE_SFX_OFFSET_GRASS);
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_SK_CRASH);
    } else if (Skeleton_Info_frame_check(skelAnime, 26.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_PL_BOUND + SURFACE_SFX_OFFSET_GRASS);
    } else if (Skeleton_Info_frame_check(skelAnime, 28.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_GRASS);
    } else if (Skeleton_Info_frame_check(skelAnime, 34.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_GRASS);
    }
}

void En_Oa2_Stalker_Set_KnockSound_forLink(EnXc* this, PlayState* play) {
    if (play->csCtx.curFrame == 1455) {
        Na_StartDemoSe(SEQ_CS_EFFECTS_BONGO_HURL_LINK);
    }
}

void En_Oa2_Stalker_Set_CrySound(EnXc* this, PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;

    if (csCtx->curFrame == 869) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_SK_CRY_0);
    } else if (csCtx->curFrame == 939) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_SK_CRY_1);
    }
}

void En_Oa2_Stalker_setup_Wait(Actor* thisx) {
    EnXc* this = (EnXc*)thisx;

    this->action = SHEIK_ACTION_NOCTURNE_INIT;
    this->drawMode = SHEIK_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

void En_Oa2_Stalker_setup_Standing(EnXc* this, PlayState* play) {
    s32 pad;
    ActorShape* actorShape = &this->actor.shape;
    SkelAnime* skelAnime = &this->skelAnime;
    f32 frameCount = Si2_anime_end_frame(&gSheikIdleAnim);

    En_Oa2_End_Movement_byAnimation(this);
    En_Oa2_Set_DemoStartPosAngle(this, play, 4);
    Skeleton_Info2_init(skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    this->action = SHEIK_ACTION_NOCTURNE_IDLE;
    this->drawMode = SHEIK_DRAW_SQUINT;
    actorShape->shadowAlpha = 255;
}

void En_Oa2_Stalker_setup_Square(Actor* thisx) {
    EnXc* this = (EnXc*)thisx;
    SkelAnime* skelAnime = &this->skelAnime;
    f32 frameCount = Si2_anime_end_frame(&gSheikDefenseStanceAnim);

    Skeleton_Info2_init(skelAnime, &gSheikDefenseStanceAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, -8.0f);
    this->action = SHEIK_ACTION_DEFENSE_STANCE;
    this->drawMode = SHEIK_DRAW_DEFAULT;
}

void En_Oa2_Stalker_setup_Brandish(EnXc* this, PlayState* play) {
    s32 pad[2];
    SkelAnime* skelAnime = &this->skelAnime;

#if DEBUG_FEATURES
    Skeleton_Info2_init(skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gSheikIdleAnim), ANIMMODE_LOOP,
                     0.0f);
#endif
    En_Oa2_Set_DemoStartPosAngle(this, play, 4);
    En_Oa2_Start_Movement_byAnimation(this, play);
    Skeleton_Info2_init(skelAnime, &gSheikContortionsAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gSheikContortionsAnim),
                     ANIMMODE_ONCE, 0.0f);
    this->action = SHEIK_ACTION_CONTORT;
    this->drawMode = SHEIK_DRAW_DEFAULT;
    this->actor.shape.shadowAlpha = 255;
}

void En_Oa2_Stalker_setup_Drop(EnXc* this, PlayState* play) {
    s32 pad;
    SkelAnime* skelAnime = &this->skelAnime;
    f32 frameCount = Si2_anime_end_frame(&gSheikIdleAnim);

    Skeleton_Info2_init(skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    En_Oa2_Set_DemoStartPosAngle(this, play, 4);
    En_Oa2_Start_Movement_byAnimation(this, play);
    Skeleton_Info2_init(skelAnime, &gSheikFallingFromContortionsAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gSheikFallingFromContortionsAnim), ANIMMODE_ONCE, 0.0f);
    this->action = SHEIK_ACTION_NOCTURNE_FALL;
    this->drawMode = SHEIK_DRAW_DEFAULT;
    this->actor.shape.shadowAlpha = 255;
}

void En_Oa2_Stalker_setup_Observe(EnXc* this, PlayState* play) {
    s32 pad[3];
    f32 frameCount = Si2_anime_end_frame(&gSheikHittingGroundAnim);

    En_Oa2_End_Movement_byAnimation(this);
    En_Oa2_Set_DemoStartPosAngle(this, play, 4);
    Skeleton_Info2_init(&this->skelAnime, &gSheikHittingGroundAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
    this->action = SHEIK_ACTION_NOCTURNE_HIT_GROUND;
    this->drawMode = SHEIK_DRAW_DEFAULT;
    this->actor.shape.shadowAlpha = 255;
}

void En_Oa2_Stalker_setup_Restrain(EnXc* this, PlayState* play) {
    s32 pad[3];
    f32 frameCount = Si2_anime_end_frame(&gSheikHittingGroundAnim);

    En_Oa2_End_Movement_byAnimation(this);
    En_Oa2_Set_DemoStartPosAngle(this, play, 4);
    Skeleton_Info2_init(&this->skelAnime, &gSheikHittingGroundAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
    this->action = SHEIK_ACTION_63;
    this->drawMode = SHEIK_DRAW_DEFAULT;
    this->actor.shape.shadowAlpha = 255;
}

void En_Oa2_Stalker_setup_Nurse(EnXc* this, PlayState* play) {
    s32 pad[3];
    f32 frameCount = Si2_anime_end_frame(&gSheikKneelingAnim);

    En_Oa2_End_Movement_byAnimation(this);
    En_Oa2_Set_DemoStartPosAngle(this, play, 4);
    Skeleton_Info2_init(&this->skelAnime, &gSheikKneelingAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    this->action = SHEIK_ACTION_NOCTURNE_KNEEL;
    this->drawMode = SHEIK_DRAW_DEFAULT;
    this->actor.shape.shadowAlpha = 255;
}

void En_Oa2_Stalker_setup_Greet(EnXc* this, PlayState* play) {
    s32 pad[3];
    f32 frameCount = Si2_anime_end_frame(&gSheikIdleAnim);
    En_Oa2_End_Movement_byAnimation(this);
    En_Oa2_Set_DemoStartPosAngle(this, play, 4);
    Skeleton_Info2_init(&this->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    this->action = SHEIK_ACTION_65;
    this->drawMode = SHEIK_DRAW_DEFAULT;
    this->actor.shape.shadowAlpha = 255;
}

void En_Oa2_Stalker_check_GreetToTake(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_65, SHEIK_ACTION_66);
}

void En_Oa2_Stalker_check_TakeToReady(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_66, SHEIK_ACTION_67);
}

void En_Oa2_Stalker_check_ReadyToPlay(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_67, SHEIK_ACTION_68);
}

void En_Oa2_Stalker_check_PlayToPlay_stop(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_68, SHEIK_ACTION_69);
}

void En_Oa2_Stalker_check_Play_stopToPlay_end(EnXc* this) {
    En_Oa2_CheckAndSet_Mode_forPlay_stopToPlay(this, SHEIK_ACTION_69, SHEIK_ACTION_70, SHEIK_ACTION_68);
}

void En_Oa2_Stalker_check_Play_endToPut(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_70, SHEIK_ACTION_71);
}

void En_Oa2_Stalker_check_PutToGoodby(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_71, SHEIK_ACTION_72);
}

void En_Oa2_Stalker_check_GoodbyToAway_accel(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_72, SHEIK_ACTION_NOCTURNE_REVERSE_ACCEL);
}

void En_Oa2_Stalker_check_Away_accelToAway_move(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_NOCTURNE_REVERSE_ACCEL, SHEIK_ACTION_NOCTURNE_REVERSE_WALK);
}

void En_Oa2_Stalker_check_Away_moveToAway_brake(EnXc* this) {
    f32 xzDistToPlayer = this->actor.xzDistToPlayer;

    if (kREG(5) + 140.0f <= xzDistToPlayer) {
        Skeleton_Info2_init(&this->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gSheikIdleAnim),
                         ANIMMODE_LOOP, -12.0f);
        this->action = SHEIK_ACTION_NOCTURNE_REVERSE_HALT;
        this->timer = 0.0f;
    }
}

void En_Oa2_Stalker_check_Away_brakeToThrow(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_NOCTURNE_REVERSE_HALT, SHEIK_ACTION_NOCTURNE_THROW_NUT);
}

void En_Oa2_Stalker_check_ThrowToFade(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_NOCTURNE_THROW_NUT, SHEIK_ACTION_77);
}

void En_Oa2_Stalker_check_FadeToDisappear(EnXc* this) {
    En_Oa2_CheckAndSet_Mode(this, SHEIK_ACTION_77, SHEIK_ACTION_78);
}

s32 En_Oa2_Stalker_Check_DemoMode(Actor* thisx, PlayState* play) {
    CsCmdActorCue* cue = En_Oa2_Get_npcdemopnt(play, 4);

    if (cue != NULL) {
        s32 nextCueId = cue->id;
        EnXc* this = (EnXc*)thisx;
        s32 currentCueId = this->unk_26C;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    En_Oa2_Stalker_setup_Wait(thisx);
                    break;
                case 6:
                    En_Oa2_Stalker_setup_Standing(this, play);
                    break;
                case 20:
                    En_Oa2_Stalker_setup_Square(thisx);
                    break;
                case 18:
                    En_Oa2_Stalker_setup_Brandish(this, play);
                    break;
                case 14:
                    En_Oa2_Stalker_setup_Drop(this, play);
                    break;
                case 19:
                    En_Oa2_Stalker_setup_Observe(this, play);
                    break;
                case 15:
                    En_Oa2_Stalker_setup_Restrain(this, play);
                    break;
                case 16:
                    En_Oa2_Stalker_setup_Nurse(this, play);
                    break;
                case 17:
                    En_Oa2_Stalker_setup_Greet(this, play);
                    break;
                case 9:
                    Actor_delete(thisx);
                    break;
                default:
                    PRINTF("En_Oa2_Stalker_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }

            this->unk_26C = nextCueId;
            return 1;
        }
    }
    return 0;
}

void En_Oa2_Stalker_main_wait(EnXc* this, PlayState* play) {
    En_Oa2_Stalker_Check_DemoMode(&this->actor, play);
}

void En_Oa2_Stalker_main_standing(EnXc* this, PlayState* play) {
    En_Oa2_Set_DemoStartPosAngle(this, play, 4);
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_Stalker_Check_DemoMode(&this->actor, play);
}

void En_Oa2_Stalker_main_square(EnXc* this, PlayState* play) {
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Stalker_Check_DemoMode(&this->actor, play);
}

void En_Oa2_Stalker_main_brandish(EnXc* this, PlayState* play) {
    En_Oa2_Stalker_Set_CrySound(this, play);
    En_Oa2_Animation_Basic(this);
    En_Oa2_set_eye_pattern(this);
    if (!En_Oa2_Stalker_Check_DemoMode(&this->actor, play)) {
        En_Oa2_Movement_byAnimation_CorrectNone(this, play);
        En_Oa2_BGcheck(this, play);
    }
}

void En_Oa2_Stalker_main_drop(EnXc* this, PlayState* play) {
    En_Oa2_Animation_Basic(this);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Stalker_Set_LandSound(this);
    if (!En_Oa2_Stalker_Check_DemoMode(&this->actor, play)) {
        En_Oa2_Movement_byAnimation(this, play);
        En_Oa2_BGcheck(this, play);
    }
}

void En_Oa2_Stalker_main_observe(EnXc* this, PlayState* play) {
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Stalker_Check_DemoMode(&this->actor, play);
}

void En_Oa2_Stalker_main_restrain(EnXc* this, PlayState* play) {
    En_Oa2_Animation_Basic(this);
    En_Oa2_Stalker_Set_KnockSound_forLink(this, play);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Stalker_Check_DemoMode(&this->actor, play);
}

void En_Oa2_Stalker_main_nurse(EnXc* this, PlayState* play) {
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Stalker_Check_DemoMode(&this->actor, play);
}

void En_Oa2_Stalker_main_greet(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_greet(this, play);
    En_Oa2_Set_DemoStartPosAngle(this, play, 4);
    En_Oa2_Stalker_check_GreetToTake(this);
}

void En_Oa2_Stalker_main_take(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_take(this, play);
    En_Oa2_Stalker_check_TakeToReady(this);
}

void En_Oa2_Stalker_main_ready(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_ready(this, play);
    En_Oa2_Stalker_check_ReadyToPlay(this);
}

void En_Oa2_Stalker_main_play(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_play(this, play);
    En_Oa2_Stalker_check_PlayToPlay_stop(this);
}

void En_Oa2_Stalker_main_play_stop(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_play_stop(this, play);
    En_Oa2_Stalker_check_Play_stopToPlay_end(this);
}

void En_Oa2_Stalker_main_play_end(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_play_end(this, play);
    En_Oa2_Stalker_check_Play_endToPut(this);
}

void En_Oa2_Stalker_main_put(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_put(this, play);
    En_Oa2_Stalker_check_PutToGoodby(this);
}

void En_Oa2_Stalker_main_goodby(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_goodby(this, play);
    En_Oa2_Stalker_check_GoodbyToAway_accel(this);
}

void En_Oa2_Stalker_main_away_accel(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_away_accel(this, play);
    En_Oa2_Stalker_check_Away_accelToAway_move(this);
}

void En_Oa2_Stalker_main_away_move(EnXc* this, PlayState* play) {
    En_Oa2_Movement_Away_move(this);
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Stalker_check_Away_moveToAway_brake(this);
}

void En_Oa2_Stalker_main_away_brake(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_away_brake(this, play);
    En_Oa2_Stalker_check_Away_brakeToThrow(this);
}

void En_Oa2_Stalker_main_throw(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_throw(this, play);
    En_Oa2_Stalker_check_ThrowToFade(this);
}

void En_Oa2_Stalker_main_fade(EnXc* this, PlayState* play) {
    En_Oa2_Actor_main_fade(this, play);
    En_Oa2_Stalker_check_FadeToDisappear(this);
}

void En_Oa2_Stalker_main_disappear(EnXc* this, PlayState* play) {
    Actor_delete(&this->actor);
}

void En_Oa2_draw_glare(Actor* thisx, PlayState* play) {
    EnXc* this = (EnXc*)thisx;
    SkelAnime* skelAnime = &this->skelAnime;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_en_oA2_inStalker.c", 839);
    _texture_z_light_fog_prim(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gSheikEyeSquintingTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gSheikEyeSquintingTex));
    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL, NULL, NULL);
    CLOSE_DISPS(gfxCtx, "../z_en_oA2_inStalker.c", 854);
}
