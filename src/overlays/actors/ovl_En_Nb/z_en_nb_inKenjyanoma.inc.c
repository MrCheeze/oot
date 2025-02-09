void En_Nb_Kenjyanoma_Init(EnNb* this, PlayState* play) {
    En_Nb_Change_Anime(this, &gNabooruStandingHandsOnHipsChamberOfSagesAnim, 0, 0, 0);
    this->actor.shape.yOffset = -10000.0f;
}

void Birth_Door_Warp1_In_En_Nb(EnNb* this, PlayState* play) {
    f32 posX = this->actor.world.pos.x;
    f32 posY = this->actor.world.pos.y;
    f32 posZ = this->actor.world.pos.z;

    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_SAGES);
}

void Birth_Effect_Medal_In_En_Nb(EnNb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 posX = player->actor.world.pos.x;
    f32 posY = player->actor.world.pos.y + 50.0f;
    f32 posZ = player->actor.world.pos.z;

    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0, 0xC);
    item_get_setting(play, ITEM_MEDALLION_SPIRIT);
}

void En_Nb_Movement_Up(EnNb* this, PlayState* play) {
    this->actor.shape.yOffset += 250.0f / 3.0f;
}

void En_Nb_check_WaitToHide(EnNb* this, PlayState* play) {
    s32 pad[2];
    Player* player;

    if ((z_common_data.chamberCutsceneNum == CHAMBER_CS_SPIRIT) && !IS_CUTSCENE_LAYER) {
        player = GET_PLAYER(play);
        this->action = NB_CHAMBER_UNDERGROUND;
        play->csCtx.script = En_Nb_Data1_In_Demodt_Kenjyanoma;
        z_common_data.cutsceneTrigger = 2;
        item_get_setting(play, ITEM_MEDALLION_SPIRIT);
        player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
    }
}

void En_Nb_check_HideToUp(EnNb* this, PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;
    CsCmdActorCue* cue;

    if (csCtx->state != CS_STATE_IDLE) {
        cue = csCtx->actorCues[1];
        if (cue != NULL && cue->id == 2) {
            this->action = NB_CHAMBER_APPEAR;
            this->drawMode = NB_DRAW_DEFAULT;
            Birth_Door_Warp1_In_En_Nb(this, play);
        }
    }
}

void En_Nb_check_UpToGreet(EnNb* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->action = NB_CHAMBER_IDLE;
        this->actor.shape.yOffset = 0.0f;
    }
}

void En_Nb_check_GreetToHandup(EnNb* this, PlayState* play) {
    AnimationHeader* animation = &gNabooruRaisingArmsGivingMedallionAnim;
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[1];
        if (cue != NULL && cue->id == 3) {
            Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, Si2_anime_end_frame(animation), ANIMMODE_ONCE,
                             0.0f);
            this->action = NB_CHAMBER_RAISE_ARM;
        }
    }
}

void En_Nb_check_HandupToCheer(EnNb* this, s32 animFinished) {
    AnimationHeader* animation = &gNabooruArmsRaisedGivingMedallionAnim;

    if (animFinished) {
        Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, Si2_anime_end_frame(animation), ANIMMODE_LOOP,
                         0.0f);
        this->action = NB_CHAMBER_RAISE_ARM_TRANSITION;
    }
}

void En_Nb_check_CheerToStop(EnNb* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[6];
        if (cue != NULL && cue->id == 2) {
            this->action = NB_GIVE_MEDALLION;
            Birth_Effect_Medal_In_En_Nb(this, play);
        }
    }
}

// Action func is never explicitly set to this, but it runs when the memory gets zero cleared
void En_Nb_Actor_main_wait(EnNb* this, PlayState* play) {
    En_Nb_check_WaitToHide(this, play);
}

void En_Nb_Actor_main_hide(EnNb* this, PlayState* play) {
    En_Nb_check_HideToUp(this, play);
}

void En_Nb_Actor_main_up(EnNb* this, PlayState* play) {
    En_Nb_Movement_Up(this, play);
    En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
    En_Nb_check_UpToGreet(this);
}

void En_Nb_Actor_main_greet(EnNb* this, PlayState* play) {
    En_Nb_BGcheck(this, play);
    En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
    En_Nb_check_GreetToHandup(this, play);
}

void En_Nb_Actor_main_handup(EnNb* this, PlayState* play) {
    s32 animFinished;

    En_Nb_BGcheck(this, play);
    animFinished = En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
    En_Nb_check_HandupToCheer(this, animFinished);
}

void En_Nb_Actor_main_cheer(EnNb* this, PlayState* play) {
    En_Nb_BGcheck(this, play);
    En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
    En_Nb_check_CheerToStop(this, play);
}

void En_Nb_Actor_main_stop(EnNb* this, PlayState* play) {
    En_Nb_BGcheck(this, play);
    En_Nb_Animation_Base(this);
    En_Nb_set_eye_pattern(this);
}
