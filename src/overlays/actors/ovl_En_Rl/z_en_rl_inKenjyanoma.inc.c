void func_80AE7544(EnRl* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &object_rl_Skel_007B38, &object_rl_Anim_000A3C, NULL, NULL, 0);
}

void func_80AE7590(EnRl* this, PlayState* play) {
    s32 pad;
    Player* player;
    Vec3f pos;
    s16 sceneId = play->sceneId;

    if ((gSaveContext.sceneLayer == 4) && (sceneId == SCENE_CHAMBER_OF_THE_SAGES) &&
        (play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[6] != NULL) &&
        (play->csCtx.actorCues[6]->id == 2) && !this->lightMedallionGiven) {
        player = GET_PLAYER(play);
        pos.x = player->actor.world.pos.x;
        pos.y = player->actor.world.pos.y + 80.0f;
        pos.z = player->actor.world.pos.z;
        Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_EFFECT, pos.x, pos.y, pos.z, 0, 0, 0, 0xE);
        Item_Give(play, ITEM_MEDALLION_LIGHT);
        this->lightMedallionGiven = 1;
    }
}

void func_80AE7668(EnRl* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->drawConfig = 1;
    this->action = 1;
    player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
}

void func_80AE7698(EnRl* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[0];

        if (cue != NULL && cue->id == 3) {
            Animation_Change(&this->skelAnime, &object_rl_Anim_00040C, 1.0f, 0.0f,
                             Animation_GetLastFrame(&object_rl_Anim_00040C), ANIMMODE_ONCE, 0.0f);
            this->action = 2;
        }
    }
}

void func_80AE772C(EnRl* this, s32 arg1) {
    if (arg1) {
        Animation_Change(&this->skelAnime, &object_rl_Anim_000830, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_rl_Anim_000830), ANIMMODE_LOOP, 0.0f);
        this->action = 3;
    }
}

void func_80AE7798(EnRl* this, PlayState* play) {
    func_80AE7668(this, play);
}

void func_80AE77B8(EnRl* this, PlayState* play) {
    func_80AE744C(this, play);
    func_80AE7494(this);
    func_80AE72D0(this);
    func_80AE7698(this, play);
}

void func_80AE77F8(EnRl* this, PlayState* play) {
    s32 temp;

    func_80AE744C(this, play);
    temp = func_80AE7494(this);
    func_80AE72D0(this);
    func_80AE772C(this, temp);
}

void func_80AE7838(EnRl* this, PlayState* play) {
    func_80AE744C(this, play);
    func_80AE7494(this);
    func_80AE72D0(this);
    func_80AE7590(this, play);
}
