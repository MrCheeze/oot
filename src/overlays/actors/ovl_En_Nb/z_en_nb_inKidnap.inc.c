void En_Nb_Kidnap_Init(EnNb* this, PlayState* play) {
    En_Nb_Change_Anime(this, &gNabooruTrappedInVortexPushingGroundAnim, 0, 0.0f, 0);
    this->action = NB_KIDNAPPED;
    this->actor.shape.shadowAlpha = 0;
    SET_EVENTCHKINF(EVENTCHKINF_95);
}

void En_Nb_Kidnap_Set_CrySound(EnNb* this, PlayState* play) {
    if (play->csCtx.curFrame == 3) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_NB_CRY_0);
    }
}

void En_Nb_Kidnap_Set_AgonySound(EnNb* this, PlayState* play) {
    if (play->csCtx.curFrame == 420) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_NB_AGONY);
    }
}

void En_Nb_Kidnap_Movement_Fall(EnNb* this, PlayState* play) {
    CsCmdActorCue* cue = En_Nb_Get_npcdemopnt(play, 1);
    Vec3f* pos = &this->actor.world.pos;
    f32 lerp;
    s32 pad;
    Vec3f startPos;
    Vec3f endPos;

    if (cue != NULL) {
        lerp = get_parcent_forAccelBrake(cue->endFrame, cue->startFrame, play->csCtx.curFrame, 4, 4);
        startPos.x = cue->startPos.x;
        startPos.y = cue->startPos.y;
        startPos.z = cue->startPos.z;

        endPos.x = cue->endPos.x;
        endPos.y = cue->endPos.y;
        endPos.z = cue->endPos.z;

        pos->x = ((endPos.x - startPos.x) * lerp) + startPos.x;
        pos->y = ((endPos.y - startPos.y) * lerp) + startPos.y;
        pos->z = ((endPos.z - startPos.z) * lerp) + startPos.z;
    }
}

void En_Nb_Kidnap_setup_Wait(EnNb* this, PlayState* play) {
    En_Nb_Set_DemoStartPosAngle(this, play, 1);
    this->action = NB_KIDNAPPED;
    this->drawMode = NB_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

void En_Nb_Kidnap_Check_Animation_fall(EnNb* this, s32 animFinished) {
    AnimationHeader* animation = &gNabooruSuckedByVortexAnim;

    if (animFinished) {
        Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, Si2_anime_end_frame(animation), ANIMMODE_LOOP,
                         0.0f);
    }
}

void En_Nb_Kidnap_setup_Struggle(EnNb* this) {
    AnimationHeader* animation = &gNabooruTrappedInVortexPushingGroundAnim;

    Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, Si2_anime_end_frame(animation), ANIMMODE_LOOP, -8.0f);
    this->action = NB_KIDNAPPED_LOOK_AROUND;
    this->drawMode = NB_DRAW_DEFAULT;
}

void En_Nb_Kidnap_setup_Fall(EnNb* this) {
    AnimationHeader* animation = &gNabooruTrappedInVortexRaisingArmAnim;

    Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, Si2_anime_end_frame(animation), ANIMMODE_ONCE, -8.0f);
    this->action = NB_PORTAL_FALLTHROUGH;
    this->drawMode = NB_DRAW_DEFAULT;
}

void En_Nb_Kidnap_Check_DemoMode(EnNb* this, PlayState* play) {
    CsCmdActorCue* cue = En_Nb_Get_npcdemopnt(play, 1);
    s32 nextCueId;
    s32 currentCueId;

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    En_Nb_Kidnap_setup_Wait(this, play);
                    break;
                case 7:
                    En_Nb_Kidnap_setup_Struggle(this);
                    break;
                case 8:
                    En_Nb_Kidnap_setup_Fall(this);
                    break;
                case 9:
                    Actor_delete(&this->actor);
                    break;
                default:
                    // "Operation Doesn't Exist!!!!!!!!"
                    PRINTF("En_Nb_Kidnap_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->cueId = nextCueId;
        }
    }
}

void En_Nb_Kidnap_Actor_main_hide(EnNb* this, PlayState* play) {
    En_Nb_Kidnap_Set_CrySound(this, play);
    En_Nb_Kidnap_Check_DemoMode(this, play);
}

void En_Nb_Kidnap_Actor_main_struggle(EnNb* this, PlayState* play) {
    En_Nb_Kidnap_Set_CrySound(this, play);
    En_Nb_Kidnap_Set_AgonySound(this, play);
    En_Nb_set_eye_pattern(this);
    En_Nb_Animation_Base(this);
    En_Nb_Kidnap_Check_DemoMode(this, play);
}

void En_Nb_Kidnap_Actor_main_Fall(EnNb* this, PlayState* play) {
    s32 animFinished;

    En_Nb_set_eye_pattern(this);
    animFinished = En_Nb_Animation_Base(this);
    En_Nb_Kidnap_Check_Animation_fall(this, animFinished);
    En_Nb_Kidnap_Movement_Fall(this, play);
    En_Nb_Kidnap_Check_DemoMode(this, play);
}
