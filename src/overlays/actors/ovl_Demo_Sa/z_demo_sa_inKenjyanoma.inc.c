void Demo_Sa_Kenjyanoma_Init(DemoSa* this, PlayState* play) {
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gSariaSkel, &gSariaWaitArmsToSideAnim, NULL, NULL, 0);
    this->actor.shape.yOffset = -10000.0f;
    Demo_Sa_set_eye_Num(this, SARIA_EYE_HALF);
    Demo_Sa_set_mouth_Num(this, SARIA_MOUTH_CLOSED2);
}

void Birth_Door_Warp1_In_Demo_Sa(DemoSa* this, PlayState* play) {
    Vec3f* world = &this->actor.world.pos;
    f32 posX = world->x;
    f32 posY = world->y;
    f32 posZ = world->z;

    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_SAGES);
}

void Birth_Effect_Medal_In_Demo_Sa(DemoSa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 posX = player->actor.world.pos.x;
    f32 posY = player->actor.world.pos.y + 80.0f;
    f32 posZ = player->actor.world.pos.z;

    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0, 0xB);
    item_get_setting(play, ITEM_MEDALLION_FOREST);
}

void Demo_Sa_Movement_Up(DemoSa* this, PlayState* play) {
    this->actor.shape.yOffset += (250.0f / 3.0f);
}

void Demo_Sa_check_WaitToHide(DemoSa* this, PlayState* play) {
    s32 pad[2];
    Player* player;

    if ((z_common_data.chamberCutsceneNum == CHAMBER_CS_FOREST) && !IS_CUTSCENE_LAYER) {
        player = GET_PLAYER(play);
        this->action = 1;
        play->csCtx.script = En_Sa_Data1_In_Demodt_Kenjyanoma;
        z_common_data.cutsceneTrigger = 2;
        item_get_setting(play, ITEM_MEDALLION_FOREST);
        player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
    }
}

void Demo_Sa_check_HideToUp(DemoSa* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[4];
        if ((cue != NULL) && (cue->id == 2)) {
            this->action = 2;
            this->drawConfig = 1;
            Birth_Door_Warp1_In_Demo_Sa(this, play);
        }
    }
}

void Demo_Sa_check_UpToGreet(DemoSa* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->action = 3;
        this->actor.shape.yOffset = 0.0f;
    }
}

void Demo_Sa_check_GreetToHandup(DemoSa* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[4];
        if ((cue != NULL) && (cue->id == 3)) {
            Skeleton_Info2_init(&this->skelAnime, &gSariaGiveForestMedallionAnim, 1.0f, 0.0f,
                             Si2_anime_end_frame(&gSariaGiveForestMedallionAnim), ANIMMODE_ONCE, -4.0f);
            this->action = 4;
        }
    }
}

void Demo_Sa_check_HandupToCheer(DemoSa* this, s32 arg1) {
    if (arg1 != 0) {
        Skeleton_Info2_init(&this->skelAnime, &gSariaGiveForestMedallionStandAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gSariaGiveForestMedallionStandAnim), ANIMMODE_LOOP, 0.0f);
        this->action = 5;
    }
}

void Demo_Sa_check_CheerToStop(DemoSa* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[6];

        if ((cue != NULL) && (cue->id == 2)) {
            this->action = 6;
            Birth_Effect_Medal_In_Demo_Sa(this, play);
        }
    }
}

void Demo_Sa_Actor_main_wait(DemoSa* this, PlayState* play) {
    Demo_Sa_check_WaitToHide(this, play);
}

void Demo_Sa_Actor_main_hide(DemoSa* this, PlayState* play) {
    Demo_Sa_check_HideToUp(this, play);
}

void Demo_Sa_Actor_main_up(DemoSa* this, PlayState* play) {
    Demo_Sa_Movement_Up(this, play);
    Demo_Sa_Animation_Base(this);
    Demo_Sa_check_UpToGreet(this);
}

void Demo_Sa_Actor_main_greet(DemoSa* this, PlayState* play) {
    Demo_Sa_BGcheck(this, play);
    Demo_Sa_Animation_Base(this);
    Demo_Sa_check_GreetToHandup(this, play);
}

void Demo_Sa_Actor_main_handup(DemoSa* this, PlayState* play) {
    Demo_Sa_BGcheck(this, play);
    Demo_Sa_check_HandupToCheer(this, Demo_Sa_Animation_Base(this));
}

void Demo_Sa_Actor_main_cheer(DemoSa* this, PlayState* play) {
    Demo_Sa_BGcheck(this, play);
    Demo_Sa_Animation_Base(this);
    Demo_Sa_check_CheerToStop(this, play);
}

void Demo_Sa_Actor_main_stop(DemoSa* this, PlayState* play) {
    Demo_Sa_BGcheck(this, play);
    Demo_Sa_Animation_Base(this);
}
