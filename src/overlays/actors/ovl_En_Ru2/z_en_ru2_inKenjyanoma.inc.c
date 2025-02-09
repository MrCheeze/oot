/**
 * Gradually increases Ruto's model's Y-offset as she rises up through the blue warp in the Chamber of Sages.
 */
void En_Ru2_Movement_Up(EnRu2* this, PlayState* play) {
    this->actor.shape.yOffset += 250.0f / 3.0f;
}

/**
 * Sets up Ruto's actor in the Chamber of Sages.
 * Note: All sages actors are present in the Chamber of Sages, regardless of which dungeon was just completed.
 * This function runs unconditionally, even if it is not relevant for Ruto.
 */
void En_Ru2_Kenjyanoma_Init(EnRu2* this, PlayState* play) {
    En_Ru2_Change_Anime(this, &gAdultRutoIdleAnim, 0, 0.0f, 0);
    this->actor.shape.yOffset = -10000.0f;
}

/**
 * Spawns the blue warp for Ruto to rise up through in the Chamber of Sages.
 */
void Birth_Door_Warp1_In_En_Ru2(EnRu2* this, PlayState* play) {
    Actor* thisx = &this->actor;
    f32 posX = thisx->world.pos.x;
    f32 posY = thisx->world.pos.y;
    f32 posZ = thisx->world.pos.z;

    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_SAGES);
}

/**
 * Spawns the Water Medallion.
 */
void Birth_Effect_Medal_In_En_Ru2(EnRu2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 posX = player->actor.world.pos.x;
    f32 posY = player->actor.world.pos.y + 50.0f;
    f32 posZ = player->actor.world.pos.z;

    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0, 10);
    // Give the water medallion. This is redundant as it was already given in `En_Ru2_check_WaitToHide`
    item_get_setting(play, ITEM_MEDALLION_WATER);
}

/**
 * Sets up the Water Medallion Cutscene if coming from Water Temple.
 * All sage actors are present in the Chamber of Sages regardless of which dungeon was just completed.
 * This function will loop endlessly if the current sage cutscene is not for the Water Medallion.
 */
void En_Ru2_check_WaitToHide(EnRu2* this, PlayState* play) {
    s32 pad[2];
    Player* player;
    s16 yaw;

    if ((z_common_data.chamberCutsceneNum == CHAMBER_CS_WATER) && !IS_CUTSCENE_LAYER) {
        player = GET_PLAYER(play);
        this->action = ENRU2_AWAIT_BLUE_WARP;
        play->csCtx.script = En_Ru1_Data1_In_Demodt_Kenjyanoma;
        z_common_data.cutsceneTrigger = 2;
        item_get_setting(play, ITEM_MEDALLION_WATER);
        yaw = this->actor.world.rot.y + 0x8000;
        player->actor.shape.rot.y = yaw;
        player->actor.world.rot.y = yaw;
    }
}

void En_Ru2_check_HideToUp(EnRu2* this, PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;
    CsCmdActorCue* cue;

    if (csCtx->state != CS_STATE_IDLE) {
        cue = csCtx->actorCues[3];

        if ((cue != NULL) && (cue->id == 2)) {
            this->action = ENRU2_RISE_THROUGH_BLUE_WARP;
            this->drawConfig = ENRU2_DRAW_OPA;
            Birth_Door_Warp1_In_En_Ru2(this, play);
        }
    }
}

/**
 * Halts Ruto's rise up through the blue warp in the Chamber of Sages once finished.
 */
void En_Ru2_check_UpToGreet(EnRu2* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->action = ENRU2_SAGE_OF_WATER_DIALOG;
        this->actor.shape.yOffset = 0.0f;
    }
}

/**
 * Sets up the animation for Ruto to raise her arms to give Link the Water Medallion.
 */
void En_Ru2_check_GreetToHandup(EnRu2* this, PlayState* play) {
    AnimationHeader* animation = &gAdultRutoRaisingArmsUpAnim;
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[3];

        if ((cue != NULL) && (cue->id == 3)) {
            Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, Si2_anime_end_frame(animation), ANIMMODE_ONCE,
                             0.0f);
            this->action = ENRU2_RAISE_ARMS;
        }
    }
}

/**
 * At the end of Ruto's arms-raising animation, cues the next action: spawning the
 * Water Medallion.
 */
void En_Ru2_check_HandupToCheer(EnRu2* this, s32 doneRaising) {
    if (doneRaising != 0) {
        this->action = ENRU2_AWAIT_SPAWN_WATER_MEDALLION;
    }
}

/**
 * Checks to see if the Water Medallion should spawn.
 */
void En_Ru2_check_CheerToStop(EnRu2* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[6];

        if ((cue != NULL) && (cue->id == 2)) {
            this->action = ENRU2_FINISH_WATER_MEDALLION_CS;
            Birth_Effect_Medal_In_En_Ru2(this, play);
        }
    }
}

void En_Ru2_Actor_main_wait(EnRu2* this, PlayState* play) {
    En_Ru2_check_WaitToHide(this, play);
}

void En_Ru2_Actor_main_hide(EnRu2* this, PlayState* play) {
    En_Ru2_check_HideToUp(this, play);
}

void En_Ru2_Actor_main_up(EnRu2* this, PlayState* play) {
    En_Ru2_Movement_Up(this, play);
    En_Ru2_Animation_Base(this);
    En_Ru2_set_eye_pattern(this);
    En_Ru2_check_UpToGreet(this);
}

void En_Ru2_Actor_main_greet(EnRu2* this, PlayState* play) {
    En_Ru2_BGcheck(this, play);
    En_Ru2_Animation_Base(this);
    En_Ru2_set_eye_pattern(this);
    En_Ru2_check_GreetToHandup(this, play);
}

void En_Ru2_Actor_main_handup(EnRu2* this, PlayState* play) {
    s32 animDone;

    En_Ru2_BGcheck(this, play);
    animDone = En_Ru2_Animation_Base(this);
    En_Ru2_set_eye_pattern(this);
    En_Ru2_check_HandupToCheer(this, animDone);
}

void En_Ru2_Actor_main_cheer(EnRu2* this, PlayState* play) {
    En_Ru2_BGcheck(this, play);
    En_Ru2_Animation_Base(this);
    En_Ru2_set_eye_pattern(this);
    En_Ru2_check_CheerToStop(this, play);
}

void En_Ru2_Actor_main_stop(EnRu2* this, PlayState* play) {
    En_Ru2_BGcheck(this, play);
    En_Ru2_Animation_Base(this);
    En_Ru2_set_eye_pattern(this);
}
