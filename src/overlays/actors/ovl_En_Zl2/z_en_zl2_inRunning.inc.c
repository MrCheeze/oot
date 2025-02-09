void func_80B51D0C(EnZl2* this, PlayState* play) {
    this->action = 33;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_80B51D24(EnZl2* this, PlayState* play) {
    s32 pad[2];
    u32 sfxId;
    SkelAnime* skelAnime = &this->skelAnime;

    if (Animation_OnFrame(skelAnime, 6.0f) || Animation_OnFrame(skelAnime, 0.0f)) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            sfxId = NA_SE_PL_WALK_GROUND;
            sfxId += SurfaceType_GetSfxOffset(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
            Sfx_PlaySfxAtPos(&this->actor.projectedPos, sfxId);
        }
    }
}

void func_80B51DA4(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = EnZl2_GetCue(play, 0);
    Vec3f* worldPos = &this->actor.world.pos;
    f32 startX;
    f32 startY;
    f32 startZ;
    f32 endX;
    f32 endY;
    f32 endZ;
    f32 lerp;

    if (cue != NULL) {
        lerp = Environment_LerpWeightAccelDecel(cue->endFrame, cue->startFrame, play->csCtx.curFrame, 0, 8);
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
            Actor_Kill(&this->actor);
        }
    }
}

void func_80B51EA8(EnZl2* this) {
    this->action = 33;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_80B51EBC(EnZl2* this, PlayState* play) {
    ActorShape* shape = &this->actor.shape;
    CsCmdActorCue* cue = EnZl2_GetCue(play, 0);
    s32 pad[2];

    this->actor.world.rot.y = shape->rot.y = cue->rot.y;
    func_80B4FD00(this, &gZelda2Anime1Anim_00B224, 0, 0.0f, 0);
    this->action = 34;
    this->drawConfig = 1;
    shape->shadowAlpha = 255;
}

void func_80B51F38(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = EnZl2_GetCue(play, 0);

    if (cue != NULL) {
        if (play->csCtx.curFrame - 8 >= cue->endFrame) {
            func_80B4FD00(this, &gZelda2Anime1Anim_00B5FC, 0, -8.0f, 0);
            this->action = 35;
        }
    }
}

void func_80B51FA8(EnZl2* this, PlayState* play) {
    CsCmdActorCue* cue = EnZl2_GetCue(play, 0);
    s32 nextCueId;
    s32 currentCueId;

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    func_80B51EA8(this);
                    break;
                case 2:
                    func_80B51EBC(this, play);
                    break;
                case 14:
                    Actor_Kill(&this->actor);
                    break;
                default:
                    PRINTF("En_Zl2_inRunning_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->cueId = nextCueId;
        }
    }
}

void func_80B52068(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B51FA8(this, play);
}

void func_80B52098(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B51D24(this, play);
    func_80B51F38(this, play);
    func_80B51DA4(this, play);
}

void func_80B52108(EnZl2* this, PlayState* play) {
}
