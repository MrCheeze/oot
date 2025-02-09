void func_80B3EBF0(EnXc* this, PlayState* play) {
    this->action = SHEIK_ACTION_20;
}

void func_80B3EC00(EnXc* this) {
    this->action = SHEIK_ACTION_21;
}

void func_80B3EC0C(EnXc* this, PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;

    if (csCtx->state != 0) {
        CsCmdActorCue* cue = csCtx->actorCues[4];

        if ((cue != NULL) && (cue->id != 1)) {
            PosRot* posRot = &this->actor.world;
            Vec3i* startPos = &cue->startPos;
            ActorShape* shape = &this->actor.shape;

            posRot->pos.x = startPos->x;
            posRot->pos.y = startPos->y;
            posRot->pos.z = startPos->z;

            posRot->rot.y = shape->rot.y = cue->rot.y;

            this->action = SHEIK_ACTION_22;
            this->drawMode = SHEIK_DRAW_DEFAULT;
        }
    }
}

void func_80B3EC90(EnXc* this, PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;

    if (csCtx->state != 0) {
        CsCmdActorCue* cue = csCtx->actorCues[4];

        if (cue != NULL && cue->id != 6) {
            func_80B3C9EC(this);
        }
    }
}

void func_80B3ECD8(EnXc* this) {
    this->timer++;
    if (this->timer >= 12.0f) {
        this->actor.speed = kREG(2) * 0.01f + 1.2f;
        this->action = SHEIK_ACTION_24;
    }
}

void EnXc_ActionFunc20(EnXc* this, PlayState* play) {
    func_80B3EC00(this);
}

void EnXc_ActionFunc21(EnXc* this, PlayState* play) {
    func_80B3EC0C(this, play);
}

void EnXc_ActionFunc22(EnXc* this, PlayState* play) {
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    func_80B3EC90(this, play);
}

void EnXc_ActionFunc23(EnXc* this, PlayState* play) {
    func_80B3D6F0(this);
    EnXc_AnimIsFinished(this);
    EnXc_BgCheck(this, play);
    EnXc_SetEyePattern(this);
    EnXc_SetWalkingSFX(this, play);
    func_80B3ECD8(this);
}

void EnXc_ActionFunc24(EnXc* this, PlayState* play) {
}

void EnXc_ActionFunc25(EnXc* this, PlayState* play) {
}

void EnXc_ActionFunc26(EnXc* this, PlayState* play) {
}

void EnXc_ActionFunc27(EnXc* this, PlayState* play) {
}

void EnXc_ActionFunc28(EnXc* this, PlayState* play) {
}
