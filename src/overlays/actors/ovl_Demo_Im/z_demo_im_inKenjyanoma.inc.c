void func_80985310(DemoIm* this, PlayState* play) {
    DemoIm_ChangeAnim(this, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->actor.shape.yOffset = -10000.0f;
}

void func_80985358(DemoIm* this, PlayState* play) {
    f32 posX = this->actor.world.pos.x;
    f32 posY = this->actor.world.pos.y;
    f32 posZ = this->actor.world.pos.z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_SAGES);
}

void func_809853B4(DemoIm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 playerX = player->actor.world.pos.x;
    f32 playerY = player->actor.world.pos.y + 80.0f;
    f32 playerZ = player->actor.world.pos.z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_EFFECT, playerX, playerY, playerZ, 0, 0, 0, 0xD);
    Item_Give(play, ITEM_MEDALLION_SHADOW);
}

void func_80985430(DemoIm* this, PlayState* play) {
    this->actor.shape.yOffset += 250.0f / 3.0f;
}

void func_8098544C(DemoIm* this, PlayState* play) {
    s32 pad[2];

    if ((gSaveContext.chamberCutsceneNum == CHAMBER_CS_SHADOW) && !IS_CUTSCENE_LAYER) {
        Player* player = GET_PLAYER(play);

        this->action = 1;
        play->csCtx.script = gShadowMedallionCs;
        gSaveContext.cutsceneTrigger = 2;
        Item_Give(play, ITEM_MEDALLION_SHADOW);
        player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
    }
}

void func_809854DC(DemoIm* this, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[5] != NULL) &&
        (play->csCtx.actorCues[5]->id == 2)) {
        Animation_Change(&this->skelAnime, &gImpaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        this->action = 2;
        this->drawConfig = 1;
        func_80985358(this, play);
    }
}

void func_8098557C(DemoIm* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->action = 3;
        this->actor.shape.yOffset = 0.0f;
    }
}

void func_809855A8(DemoIm* this, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[5] != NULL) &&
        (play->csCtx.actorCues[5]->id == 3)) {
        Animation_Change(&this->skelAnime, &gImpaRaiseArmsAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaRaiseArmsAnim),
                         ANIMMODE_ONCE, 4.0f);
        this->action = 4;
    }
}

void func_80985640(DemoIm* this, s32 arg1) {
    if (arg1 != 0) {
        Animation_Change(&this->skelAnime, &gImpaPresentShadowMedallionAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gImpaPresentShadowMedallionAnim), ANIMMODE_LOOP, 0.0f);
        this->action = 5;
    }
}

void func_809856AC(DemoIm* this, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[6] != NULL) &&
        (play->csCtx.actorCues[6]->id == 2)) {
        this->action = 6;
        func_809853B4(this, play);
    }
}

void func_809856F8(DemoIm* this, PlayState* play) {
    func_8098544C(this, play);
}

void func_80985718(DemoIm* this, PlayState* play) {
    func_809854DC(this, play);
}

void func_80985738(DemoIm* this, PlayState* play) {
    func_80985430(this, play);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_8098557C(this);
}

void func_80985770(DemoIm* this, PlayState* play) {
    DemoIm_UpdateBgCheckInfo(this, play);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_809855A8(this, play);
}

void func_809857B0(DemoIm* this, PlayState* play) {
    s32 sp1C;

    DemoIm_UpdateBgCheckInfo(this, play);
    sp1C = DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_80985640(this, sp1C);
}

void func_809857F0(DemoIm* this, PlayState* play) {
    DemoIm_UpdateBgCheckInfo(this, play);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
    func_809856AC(this, play);
}

void func_80985830(DemoIm* this, PlayState* play) {
    DemoIm_UpdateBgCheckInfo(this, play);
    DemoIm_UpdateSkelAnime(this);
    func_80984BE0(this);
}
