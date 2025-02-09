void EnNb_SetChamberAnim(EnNb* this, PlayState* play) {
    EnNb_SetCurrentAnim(this, &gNabooruStandingHandsOnHipsChamberOfSagesAnim, 0, 0, 0);
    this->actor.shape.yOffset = -10000.0f;
}

void EnNb_SpawnBlueWarp(EnNb* this, PlayState* play) {
    f32 posX = this->actor.world.pos.x;
    f32 posY = this->actor.world.pos.y;
    f32 posZ = this->actor.world.pos.z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_SAGES);
}

void EnNb_GiveMedallion(EnNb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 posX = player->actor.world.pos.x;
    f32 posY = player->actor.world.pos.y + 50.0f;
    f32 posZ = player->actor.world.pos.z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0, 0xC);
    Item_Give(play, ITEM_MEDALLION_SPIRIT);
}

void EnNb_ComeUpImpl(EnNb* this, PlayState* play) {
    this->actor.shape.yOffset += 250.0f / 3.0f;
}

void EnNb_SetupChamberCsImpl(EnNb* this, PlayState* play) {
    s32 pad[2];
    Player* player;

    if ((gSaveContext.chamberCutsceneNum == CHAMBER_CS_SPIRIT) && !IS_CUTSCENE_LAYER) {
        player = GET_PLAYER(play);
        this->action = NB_CHAMBER_UNDERGROUND;
        play->csCtx.script = gSpiritMedallionCs;
        gSaveContext.cutsceneTrigger = 2;
        Item_Give(play, ITEM_MEDALLION_SPIRIT);
        player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
    }
}

void EnNb_SetupChamberWarpImpl(EnNb* this, PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;
    CsCmdActorCue* cue;

    if (csCtx->state != CS_STATE_IDLE) {
        cue = csCtx->actorCues[1];
        if (cue != NULL && cue->id == 2) {
            this->action = NB_CHAMBER_APPEAR;
            this->drawMode = NB_DRAW_DEFAULT;
            EnNb_SpawnBlueWarp(this, play);
        }
    }
}

void EnNb_SetupDefaultChamberIdle(EnNb* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->action = NB_CHAMBER_IDLE;
        this->actor.shape.yOffset = 0.0f;
    }
}

void EnNb_SetupArmRaise(EnNb* this, PlayState* play) {
    AnimationHeader* animation = &gNabooruRaisingArmsGivingMedallionAnim;
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[1];
        if (cue != NULL && cue->id == 3) {
            Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE,
                             0.0f);
            this->action = NB_CHAMBER_RAISE_ARM;
        }
    }
}

void EnNb_SetupRaisedArmTransition(EnNb* this, s32 animFinished) {
    AnimationHeader* animation = &gNabooruArmsRaisedGivingMedallionAnim;

    if (animFinished) {
        Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP,
                         0.0f);
        this->action = NB_CHAMBER_RAISE_ARM_TRANSITION;
    }
}

void EnNb_SetupMedallion(EnNb* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[6];
        if (cue != NULL && cue->id == 2) {
            this->action = NB_GIVE_MEDALLION;
            EnNb_GiveMedallion(this, play);
        }
    }
}

// Action func is never explicitly set to this, but it runs when the memory gets zero cleared
void EnNb_SetupChamberCs(EnNb* this, PlayState* play) {
    EnNb_SetupChamberCsImpl(this, play);
}

void EnNb_SetupChamberWarp(EnNb* this, PlayState* play) {
    EnNb_SetupChamberWarpImpl(this, play);
}

void EnNb_ComeUp(EnNb* this, PlayState* play) {
    EnNb_ComeUpImpl(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_SetupDefaultChamberIdle(this);
}

void func_80AB193C(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_SetupArmRaise(this, play);
}

void EnNb_RaiseArm(EnNb* this, PlayState* play) {
    s32 animFinished;

    func_80AB1284(this, play);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_SetupRaisedArmTransition(this, animFinished);
}

void func_80AB19BC(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_SetupMedallion(this, play);
}

void func_80AB19FC(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
}
