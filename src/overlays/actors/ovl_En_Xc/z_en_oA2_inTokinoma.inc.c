void En_Oa2_Actor_Tokinoma_Init(EnXc* this, PlayState* play) {
    this->action = SHEIK_ACTION_20;
}

void En_Oa2_Toki_check_WaitToHide(EnXc* this) {
    this->action = SHEIK_ACTION_21;
}

void En_Oa2_Toki_check_HideToGreet(EnXc* this, PlayState* play) {
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

void En_Oa2_Toki_check_GreetToBloking(EnXc* this, PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;

    if (csCtx->state != 0) {
        CsCmdActorCue* cue = csCtx->actorCues[4];

        if (cue != NULL && cue->id != 6) {
            En_Oa2_Setup_TokinomaToWall(this);
        }
    }
}

void En_Oa2_Toki_check_Away_accelToAway_move(EnXc* this) {
    this->timer++;
    if (this->timer >= 12.0f) {
        this->actor.speed = kREG(2) * 0.01f + 1.2f;
        this->action = SHEIK_ACTION_24;
    }
}

void En_Oa2_Toki_Actor_main_wait(EnXc* this, PlayState* play) {
    En_Oa2_Toki_check_WaitToHide(this);
}

void En_Oa2_Toki_Actor_main_hide(EnXc* this, PlayState* play) {
    En_Oa2_Toki_check_HideToGreet(this, play);
}

void En_Oa2_Toki_Actor_main_greet(EnXc* this, PlayState* play) {
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Toki_check_GreetToBloking(this, play);
}

void En_Oa2_Toki_Actor_main_away_accel(EnXc* this, PlayState* play) {
    En_Oa2_Movement_Away_accel(this);
    En_Oa2_Animation_Basic(this);
    En_Oa2_BGcheck(this, play);
    En_Oa2_set_eye_pattern(this);
    En_Oa2_Set_WalkSound(this, play);
    En_Oa2_Toki_check_Away_accelToAway_move(this);
}

void En_Oa2_Toki_Actor_main_away_move(EnXc* this, PlayState* play) {
}

void En_Oa2_Toki_Actor_main_away_brake(EnXc* this, PlayState* play) {
}

void En_Oa2_Toki_Actor_main_throw(EnXc* this, PlayState* play) {
}

void En_Oa2_Toki_Actor_main_fade(EnXc* this, PlayState* play) {
}

void En_Oa2_Toki_Actor_main_disappear(EnXc* this, PlayState* play) {
}
