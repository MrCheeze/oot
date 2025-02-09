void Demo_Du_Actor_Kenjyanoma_Init(DemoDu* this, PlayState* play) {
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gDaruniaSkel, &gDaruniaIdleAnim, NULL, NULL, 0);
    this->actor.shape.yOffset = -10000.0f;
    Demo_Du_set_eye_Num(this, 1);
    Demo_Du_set_mouth_Num(this, 3);
}

// A.k.a Warp portal
void chBirth_Door_Warp1_In_Demo_Du(DemoDu* this, PlayState* play) {
    f32 posX = this->actor.world.pos.x;
    f32 posY = this->actor.world.pos.y;
    f32 posZ = this->actor.world.pos.z;

    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_SAGES);
}

// Gives the Fire Medallion to Link.
void chBirth_Effect_Medal_In_Demo_Du(DemoDu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 posX = player->actor.world.pos.x;
    f32 posY = player->actor.world.pos.y + 80.0f;
    f32 posZ = player->actor.world.pos.z;

    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0,
                       DEMO_EFFECT_MEDAL_FIRE);
    item_get_setting(play, ITEM_MEDALLION_FIRE);
}

void Demo_Du_Movement_Up(DemoDu* this, PlayState* play) {
    this->actor.shape.yOffset += 250.0f / 3.0f;
}

// Gives the Fire Medallion to Link too.
void Demo_Du_check_WaitToHide(DemoDu* this, PlayState* play) {
    s32 pad[2];

    if ((z_common_data.chamberCutsceneNum == CHAMBER_CS_FIRE) && !IS_CUTSCENE_LAYER) {
        Player* player = GET_PLAYER(play);

        this->updateIndex = CS_FIREMEDALLION_SUBSCENE(1);
        play->csCtx.script = Demo_Du_Data1_In_Demodt_Kenjyanoma;
        z_common_data.cutsceneTrigger = 2;
        item_get_setting(play, ITEM_MEDALLION_FIRE);

        player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
    }
}

void Demo_Du_check_HideToUp(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if ((cue != NULL) && (cue->id != 1)) {
            this->updateIndex = CS_FIREMEDALLION_SUBSCENE(2);
            this->drawIndex = 1;
            chBirth_Door_Warp1_In_Demo_Du(this, play);
        }
    }
}

void Demo_Du_check_UpToGreet(DemoDu* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->updateIndex = CS_FIREMEDALLION_SUBSCENE(3);
        this->actor.shape.yOffset = 0.0f;
    }
}

void Demo_Du_check_GreetToHandup(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if ((cue != NULL) && (cue->id != 2)) {
            Skeleton_Info2_init(&this->skelAnime, &gDaruniaItemGiveAnim, 1.0f, 0.0f,
                             Si2_anime_end_frame(&gDaruniaItemGiveAnim), 2, 0.0f);
            this->updateIndex = CS_FIREMEDALLION_SUBSCENE(4);
        }
    }
}

void Demo_Du_check_HandupToCheer(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Skeleton_Info2_init(&this->skelAnime, &gDaruniaItemGiveIdleAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gDaruniaItemGiveIdleAnim), 0, 0.0f);
        this->updateIndex = CS_FIREMEDALLION_SUBSCENE(5);
    }
}

void Demo_Du_check_CheerToStop(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[6];

        if ((cue != NULL) && (cue->id == 2)) {
            this->updateIndex = CS_FIREMEDALLION_SUBSCENE(6);
            chBirth_Effect_Medal_In_Demo_Du(this, play);
        }
    }
}

void Demo_Du_Actor_main_wait(DemoDu* this, PlayState* play) {
    Demo_Du_check_WaitToHide(this, play);
}

void Demo_Du_Actor_main_hide(DemoDu* this, PlayState* play) {
    Demo_Du_check_HideToUp(this, play);
}

void Demo_Du_Actor_main_up(DemoDu* this, PlayState* play) {
    Demo_Du_Movement_Up(this, play);
    Demo_Du_Animation_Base(this);
    Demo_Du_check_UpToGreet(this);
}

void Demo_Du_Actor_main_greet(DemoDu* this, PlayState* play) {
    Demo_Du_BGcheck(this, play);
    Demo_Du_Animation_Base(this);
    Demo_Du_check_GreetToHandup(this, play);
}

void Demo_Du_Actor_main_handup(DemoDu* this, PlayState* play) {
    s32 animFinished;

    Demo_Du_BGcheck(this, play);
    animFinished = Demo_Du_Animation_Base(this);
    Demo_Du_check_HandupToCheer(this, animFinished);
}

void Demo_Du_Actor_main_cheer(DemoDu* this, PlayState* play) {
    Demo_Du_BGcheck(this, play);
    Demo_Du_Animation_Base(this);
    Demo_Du_check_CheerToStop(this, play);
}

void Demo_Du_Actor_main_stop(DemoDu* this, PlayState* play) {
    Demo_Du_BGcheck(this, play);
    Demo_Du_Animation_Base(this);
}
