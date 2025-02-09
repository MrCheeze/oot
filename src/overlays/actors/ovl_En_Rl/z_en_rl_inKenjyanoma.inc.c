void En_Rl_Kenjyanoma_Init(EnRl* this, PlayState* play) {
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_rl_Skel_007B38, &object_rl_Anim_000A3C, NULL, NULL, 0);
}

void Birth_Effect_Medal_In_En_Rl(EnRl* this, PlayState* play) {
    s32 pad;
    Player* player;
    Vec3f pos;
    s16 sceneId = play->sceneId;

    if ((z_common_data.sceneLayer == 4) && (sceneId == SCENE_CHAMBER_OF_THE_SAGES) &&
        (play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[6] != NULL) &&
        (play->csCtx.actorCues[6]->id == 2) && !this->lightMedallionGiven) {
        player = GET_PLAYER(play);
        pos.x = player->actor.world.pos.x;
        pos.y = player->actor.world.pos.y + 80.0f;
        pos.z = player->actor.world.pos.z;
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_DEMO_EFFECT, pos.x, pos.y, pos.z, 0, 0, 0, 0xE);
        item_get_setting(play, ITEM_MEDALLION_LIGHT);
        this->lightMedallionGiven = 1;
    }
}

void En_Rl_check_WaitToGreet(EnRl* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->drawConfig = 1;
    this->action = 1;
    player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
}

void En_Rl_check_GreetToHandup(EnRl* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[0];

        if (cue != NULL && cue->id == 3) {
            Skeleton_Info2_init(&this->skelAnime, &object_rl_Anim_00040C, 1.0f, 0.0f,
                             Si2_anime_end_frame(&object_rl_Anim_00040C), ANIMMODE_ONCE, 0.0f);
            this->action = 2;
        }
    }
}

void En_Rl_check_HandupToCheer(EnRl* this, s32 arg1) {
    if (arg1) {
        Skeleton_Info2_init(&this->skelAnime, &object_rl_Anim_000830, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_rl_Anim_000830), ANIMMODE_LOOP, 0.0f);
        this->action = 3;
    }
}

void En_Rl_Actor_main_wait(EnRl* this, PlayState* play) {
    En_Rl_check_WaitToGreet(this, play);
}

void En_Rl_Actor_main_greet(EnRl* this, PlayState* play) {
    En_Rl_BGcheck(this, play);
    En_Rl_Animation_Base(this);
    En_Rl_set_eye_pattern(this);
    En_Rl_check_GreetToHandup(this, play);
}

void En_Rl_Actor_main_handup(EnRl* this, PlayState* play) {
    s32 temp;

    En_Rl_BGcheck(this, play);
    temp = En_Rl_Animation_Base(this);
    En_Rl_set_eye_pattern(this);
    En_Rl_check_HandupToCheer(this, temp);
}

void En_Rl_Actor_main_cheer(EnRl* this, PlayState* play) {
    En_Rl_BGcheck(this, play);
    En_Rl_Animation_Base(this);
    En_Rl_set_eye_pattern(this);
    Birth_Effect_Medal_In_En_Rl(this, play);
}
