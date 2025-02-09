/**
 * Gradually increases Ruto's model's Y-offset as she rises up through the blue warp in the Chamber of Sages.
 */
void EnRu2_Rise(EnRu2* this, PlayState* play) {
    this->actor.shape.yOffset += 250.0f / 3.0f;
}

/**
 * Sets up Ruto's actor in the Chamber of Sages.
 * Note: All sages actors are present in the Chamber of Sages, regardless of which dungeon was just completed.
 * This function runs unconditionally, even if it is not relevant for Ruto.
 */
void EnRu2_InitChamberOfSages(EnRu2* this, PlayState* play) {
    EnRu2_AnimationChange(this, &gAdultRutoIdleAnim, 0, 0.0f, 0);
    this->actor.shape.yOffset = -10000.0f;
}

/**
 * Spawns the blue warp for Ruto to rise up through in the Chamber of Sages.
 */
void EnRu2_SpawnBlueWarp(EnRu2* this, PlayState* play) {
    Actor* thisx = &this->actor;
    f32 posX = thisx->world.pos.x;
    f32 posY = thisx->world.pos.y;
    f32 posZ = thisx->world.pos.z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_SAGES);
}

/**
 * Spawns the Water Medallion.
 */
void EnRu2_SpawnWaterMedallion(EnRu2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 posX = player->actor.world.pos.x;
    f32 posY = player->actor.world.pos.y + 50.0f;
    f32 posZ = player->actor.world.pos.z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0, 10);
    // Give the water medallion. This is redundant as it was already given in `EnRu2_CheckWaterMedallionCutscene`
    Item_Give(play, ITEM_MEDALLION_WATER);
}

/**
 * Sets up the Water Medallion Cutscene if coming from Water Temple.
 * All sage actors are present in the Chamber of Sages regardless of which dungeon was just completed.
 * This function will loop endlessly if the current sage cutscene is not for the Water Medallion.
 */
void EnRu2_CheckWaterMedallionCutscene(EnRu2* this, PlayState* play) {
    s32 pad[2];
    Player* player;
    s16 yaw;

    if ((gSaveContext.chamberCutsceneNum == CHAMBER_CS_WATER) && !IS_CUTSCENE_LAYER) {
        player = GET_PLAYER(play);
        this->action = ENRU2_AWAIT_BLUE_WARP;
        play->csCtx.script = gWaterMedallionCs;
        gSaveContext.cutsceneTrigger = 2;
        Item_Give(play, ITEM_MEDALLION_WATER);
        yaw = this->actor.world.rot.y + 0x8000;
        player->actor.shape.rot.y = yaw;
        player->actor.world.rot.y = yaw;
    }
}

void EnRu2_CheckIfBlueWarpShouldSpawn(EnRu2* this, PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;
    CsCmdActorCue* cue;

    if (csCtx->state != CS_STATE_IDLE) {
        cue = csCtx->actorCues[3];

        if ((cue != NULL) && (cue->id == 2)) {
            this->action = ENRU2_RISE_THROUGH_BLUE_WARP;
            this->drawConfig = ENRU2_DRAW_OPA;
            EnRu2_SpawnBlueWarp(this, play);
        }
    }
}

/**
 * Halts Ruto's rise up through the blue warp in the Chamber of Sages once finished.
 */
void EnRu2_EndRise(EnRu2* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->action = ENRU2_SAGE_OF_WATER_DIALOG;
        this->actor.shape.yOffset = 0.0f;
    }
}

/**
 * Sets up the animation for Ruto to raise her arms to give Link the Water Medallion.
 */
void EnRu2_CheckStartRaisingArms(EnRu2* this, PlayState* play) {
    AnimationHeader* animation = &gAdultRutoRaisingArmsUpAnim;
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[3];

        if ((cue != NULL) && (cue->id == 3)) {
            Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE,
                             0.0f);
            this->action = ENRU2_RAISE_ARMS;
        }
    }
}

/**
 * At the end of Ruto's arms-raising animation, cues the next action: spawning the
 * Water Medallion.
 */
void EnRu2_HoldArmsUp(EnRu2* this, s32 doneRaising) {
    if (doneRaising != 0) {
        this->action = ENRU2_AWAIT_SPAWN_WATER_MEDALLION;
    }
}

/**
 * Checks to see if the Water Medallion should spawn.
 */
void EnRu2_CheckIfWaterMedallionShouldSpawn(EnRu2* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[6];

        if ((cue != NULL) && (cue->id == 2)) {
            this->action = ENRU2_FINISH_WATER_MEDALLION_CS;
            EnRu2_SpawnWaterMedallion(this, play);
        }
    }
}

void EnRu2_SetupWaterMedallionCutscene(EnRu2* this, PlayState* play) {
    EnRu2_CheckWaterMedallionCutscene(this, play);
}

void EnRu2_AwaitBlueWarp(EnRu2* this, PlayState* play) {
    EnRu2_CheckIfBlueWarpShouldSpawn(this, play);
}

void EnRu2_RiseThroughBlueWarp(EnRu2* this, PlayState* play) {
    EnRu2_Rise(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_EndRise(this);
}

void EnRu2_SageOfWaterDialog(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_CheckStartRaisingArms(this, play);
}

void EnRu2_RaiseArms(EnRu2* this, PlayState* play) {
    s32 animDone;

    EnRu2_UpdateBgCheckInfo(this, play);
    animDone = EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_HoldArmsUp(this, animDone);
}

void EnRu2_AwaitWaterMedallion(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_CheckIfWaterMedallionShouldSpawn(this, play);
}

void EnRu2_FinishWaterMedallionCutscene(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
}
