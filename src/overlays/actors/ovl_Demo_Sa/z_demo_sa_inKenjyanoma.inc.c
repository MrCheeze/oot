void func_8098E7FC(DemoSa* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gSariaSkel, &gSariaWaitArmsToSideAnim, NULL, NULL, 0);
    this->actor.shape.yOffset = -10000.0f;
    DemoSa_SetEyeIndex(this, SARIA_EYE_HALF);
    DemoSa_SetMouthIndex(this, SARIA_MOUTH_CLOSED2);
}

void func_8098E86C(DemoSa* this, PlayState* play) {
    Vec3f* world = &this->actor.world.pos;
    f32 posX = world->x;
    f32 posY = world->y;
    f32 posZ = world->z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_SAGES);
}

void func_8098E8C8(DemoSa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 posX = player->actor.world.pos.x;
    f32 posY = player->actor.world.pos.y + 80.0f;
    f32 posZ = player->actor.world.pos.z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0, 0xB);
    Item_Give(play, ITEM_MEDALLION_FOREST);
}

void func_8098E944(DemoSa* this, PlayState* play) {
    this->actor.shape.yOffset += (250.0f / 3.0f);
}

void func_8098E960(DemoSa* this, PlayState* play) {
    s32 pad[2];
    Player* player;

    if ((gSaveContext.chamberCutsceneNum == CHAMBER_CS_FOREST) && !IS_CUTSCENE_LAYER) {
        player = GET_PLAYER(play);
        this->action = 1;
        play->csCtx.script = gForestMedallionCs;
        gSaveContext.cutsceneTrigger = 2;
        Item_Give(play, ITEM_MEDALLION_FOREST);
        player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
    }
}

void func_8098E9EC(DemoSa* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[4];
        if ((cue != NULL) && (cue->id == 2)) {
            this->action = 2;
            this->drawConfig = 1;
            func_8098E86C(this, play);
        }
    }
}

void func_8098EA3C(DemoSa* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->action = 3;
        this->actor.shape.yOffset = 0.0f;
    }
}

void func_8098EA68(DemoSa* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[4];
        if ((cue != NULL) && (cue->id == 3)) {
            Animation_Change(&this->skelAnime, &gSariaGiveForestMedallionAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gSariaGiveForestMedallionAnim), ANIMMODE_ONCE, -4.0f);
            this->action = 4;
        }
    }
}

void func_8098EB00(DemoSa* this, s32 arg1) {
    if (arg1 != 0) {
        Animation_Change(&this->skelAnime, &gSariaGiveForestMedallionStandAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gSariaGiveForestMedallionStandAnim), ANIMMODE_LOOP, 0.0f);
        this->action = 5;
    }
}

void func_8098EB6C(DemoSa* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[6];

        if ((cue != NULL) && (cue->id == 2)) {
            this->action = 6;
            func_8098E8C8(this, play);
        }
    }
}

void func_8098EBB8(DemoSa* this, PlayState* play) {
    func_8098E960(this, play);
}

void func_8098EBD8(DemoSa* this, PlayState* play) {
    func_8098E9EC(this, play);
}

void func_8098EBF8(DemoSa* this, PlayState* play) {
    func_8098E944(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098EA3C(this);
}

void func_8098EC28(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098EA68(this, play);
}

void func_8098EC60(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    func_8098EB00(this, DemoSa_UpdateSkelAnime(this));
}

void func_8098EC94(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098EB6C(this, play);
}

void func_8098ECCC(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
}
