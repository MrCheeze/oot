void Demo_Im_Kenjyanoma_Init(DemoIm* this, PlayState* play) {
    Demo_Im_Change_Anime(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->actor.shape.yOffset = -10000.0f;
}

void Birth_Door_Warp1_In_Demo_Im(DemoIm* this, PlayState* play) {
    f32 posX = this->actor.world.pos.x;
    f32 posY = this->actor.world.pos.y;
    f32 posZ = this->actor.world.pos.z;

    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_SAGES);
}

void Birth_Effect_Medal_In_Demo_Im(DemoIm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 playerX = player->actor.world.pos.x;
    f32 playerY = player->actor.world.pos.y + 80.0f;
    f32 playerZ = player->actor.world.pos.z;

    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DEMO_EFFECT, playerX, playerY, playerZ, 0, 0, 0, 0xD);
    item_get_setting(play, ITEM_MEDALLION_SHADOW);
}

void Demo_Im_Movement_Up(DemoIm* this, PlayState* play) {
    this->actor.shape.yOffset += 250.0f / 3.0f;
}

void Demo_Im_check_WaitToHide(DemoIm* this, PlayState* play) {
    s32 pad[2];

    if ((z_common_data.chamberCutsceneNum == CHAMBER_CS_SHADOW) && !IS_CUTSCENE_LAYER) {
        Player* player = GET_PLAYER(play);

        this->action = 1;
        play->csCtx.script = Demo_Im_Data1_In_Demodt_Kenjyanoma;
        z_common_data.cutsceneTrigger = 2;
        item_get_setting(play, ITEM_MEDALLION_SHADOW);
        player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
    }
}

void Demo_Im_check_HideToUp(DemoIm* this, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[5] != NULL) &&
        (play->csCtx.actorCues[5]->id == 2)) {
        Skeleton_Info2_init(&this->skelAnime, &gImpaIdleAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gImpaIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        this->action = 2;
        this->drawConfig = 1;
        Birth_Door_Warp1_In_Demo_Im(this, play);
    }
}

void Demo_Im_check_UpToGreet(DemoIm* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->action = 3;
        this->actor.shape.yOffset = 0.0f;
    }
}

void Demo_Im_check_GreetToHandup(DemoIm* this, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[5] != NULL) &&
        (play->csCtx.actorCues[5]->id == 3)) {
        Skeleton_Info2_init(&this->skelAnime, &gImpaRaiseArmsAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gImpaRaiseArmsAnim),
                         ANIMMODE_ONCE, 4.0f);
        this->action = 4;
    }
}

void Demo_Im_check_HandupToCheer(DemoIm* this, s32 arg1) {
    if (arg1 != 0) {
        Skeleton_Info2_init(&this->skelAnime, &gImpaPresentShadowMedallionAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gImpaPresentShadowMedallionAnim), ANIMMODE_LOOP, 0.0f);
        this->action = 5;
    }
}

void Demo_Im_check_CheerToStop(DemoIm* this, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[6] != NULL) &&
        (play->csCtx.actorCues[6]->id == 2)) {
        this->action = 6;
        Birth_Effect_Medal_In_Demo_Im(this, play);
    }
}

void Demo_Im_Actor_main_wait(DemoIm* this, PlayState* play) {
    Demo_Im_check_WaitToHide(this, play);
}

void Demo_Im_Actor_main_hide(DemoIm* this, PlayState* play) {
    Demo_Im_check_HideToUp(this, play);
}

void Demo_Im_Actor_main_up(DemoIm* this, PlayState* play) {
    Demo_Im_Movement_Up(this, play);
    Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_check_UpToGreet(this);
}

void Demo_Im_Actor_main_greet(DemoIm* this, PlayState* play) {
    Demo_Im_BGcheck(this, play);
    Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_check_GreetToHandup(this, play);
}

void Demo_Im_Actor_main_handup(DemoIm* this, PlayState* play) {
    s32 sp1C;

    Demo_Im_BGcheck(this, play);
    sp1C = Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_check_HandupToCheer(this, sp1C);
}

void Demo_Im_Actor_main_cheer(DemoIm* this, PlayState* play) {
    Demo_Im_BGcheck(this, play);
    Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
    Demo_Im_check_CheerToStop(this, play);
}

void Demo_Im_Actor_main_stop(DemoIm* this, PlayState* play) {
    Demo_Im_BGcheck(this, play);
    Demo_Im_Animation_Base(this);
    Demo_Im_set_eye_pattern(this);
}
