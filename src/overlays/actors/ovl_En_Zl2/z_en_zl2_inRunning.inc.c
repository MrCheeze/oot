void En_Zl2_Actor_inRunning_Init(EnZl2* this, PlayState* play) {
    this->action = 33;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void En_Zl2_inRunning_Set_RunSound(EnZl2* this, PlayState* play) {
    s32 pad[2];
    u32 sfxId;
    SkelAnime* skelAnime = &this->skelAnime;

    if (Skeleton_Info_frame_check(skelAnime, 6.0f) || Skeleton_Info_frame_check(skelAnime, 0.0f)) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            sfxId = NA_SE_PL_WALK_GROUND;
            sfxId += T_BGCheck_getSoundGroundLabel(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
            Na_StartObjectSe_F(&this->actor.projectedPos, sfxId);
        }
    }
}

void En_Zl2_inRunning_Movement(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = En_Zl2_Get_npcdemopnt(play, 0);
    Vec3f* worldPos = &this->actor.world.pos;
    f32 startX;
    f32 startY;
    f32 startZ;
    f32 endX;
    f32 endY;
    f32 endZ;
    f32 lerp;

    if (cue != NULL) {
        lerp = get_parcent_forAccelBrake(cue->endFrame, cue->startFrame, play->csCtx.curFrame, 0, 8);
        startX = cue->startPos.x;
        startY = cue->startPos.y;
        startZ = cue->startPos.z;
        endX = cue->endPos.x;
        endY = cue->endPos.y;
        endZ = cue->endPos.z;
        worldPos->x = ((endX - startX) * lerp) + startX;
        worldPos->y = ((endY - startY) * lerp) + startY;
        worldPos->z = ((endZ - startZ) * lerp) + startZ;
        if (cue->endFrame < play->csCtx.curFrame) {
            Actor_delete(&this->actor);
        }
    }
}

void En_Zl2_inRunning_setup_Wait(EnZl2* this) {
    this->action = 33;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void En_Zl2_inRunning_setup_Run(EnZl2* this, PlayState* play) {
    ActorShape* shape = &this->actor.shape;
    CsCmdActorCue* cue = En_Zl2_Get_npcdemopnt(play, 0);
    s32 pad[2];

    this->actor.world.rot.y = shape->rot.y = cue->rot.y;
    En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_00B224, 0, 0.0f, 0);
    this->action = 34;
    this->drawConfig = 1;
    shape->shadowAlpha = 255;
}

void En_Zl2_inRunning_Check_RunToStand(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = En_Zl2_Get_npcdemopnt(play, 0);

    if (cue != NULL) {
        if (play->csCtx.curFrame - 8 >= cue->endFrame) {
            En_Zl2_Change_Anime(this, &gZelda2Anime1Anim_00B5FC, 0, -8.0f, 0);
            this->action = 35;
        }
    }
}

void En_Zl2_inRunning_Check_DemoMode(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = En_Zl2_Get_npcdemopnt(play, 0);
    s32 nextCueId;
    s32 currentCueId;

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    En_Zl2_inRunning_setup_Wait(this);
                    break;
                case 2:
                    En_Zl2_inRunning_setup_Run(this, play);
                    break;
                case 14:
                    Actor_delete(&this->actor);
                    break;
                default:
                    PRINTF("En_Zl2_inRunning_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->cueId = nextCueId;
        }
    }
}

void En_Zl2_inRunning_main_wait(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_inRunning_Check_DemoMode(this, play);
}

void En_Zl2_inRunning_main_run(EnZl2* this, PlayState* play) {
    En_Zl2_Change_Bank(this, play);
    En_Zl2_BGcheck(this, play);
    En_Zl2_set_eye_pattern(this);
    En_Zl2_Animation_Base(this);
    En_Zl2_inRunning_Set_RunSound(this, play);
    En_Zl2_inRunning_Check_RunToStand(this, play);
    En_Zl2_inRunning_Movement(this, play);
}

void En_Zl2_inRunning_main_stand(EnZl2* this, PlayState* play) {
}
