void DemoDu_InitCs_FireMedallion(DemoDu* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gDaruniaSkel, &gDaruniaIdleAnim, NULL, NULL, 0);
    this->actor.shape.yOffset = -10000.0f;
    DemoDu_SetEyeTexIndex(this, 1);
    DemoDu_SetMouthTexIndex(this, 3);
}

// A.k.a Warp portal
void DemoDu_CsFireMedallion_SpawnDoorWarp(DemoDu* this, PlayState* play) {
    f32 posX = this->actor.world.pos.x;
    f32 posY = this->actor.world.pos.y;
    f32 posZ = this->actor.world.pos.z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_SAGES);
}

// Gives the Fire Medallion to Link.
void func_80969F38(DemoDu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 posX = player->actor.world.pos.x;
    f32 posY = player->actor.world.pos.y + 80.0f;
    f32 posZ = player->actor.world.pos.z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0,
                       DEMO_EFFECT_MEDAL_FIRE);
    Item_Give(play, ITEM_MEDALLION_FIRE);
}

void func_80969FB4(DemoDu* this, PlayState* play) {
    this->actor.shape.yOffset += 250.0f / 3.0f;
}

// Gives the Fire Medallion to Link too.
void DemoDu_CsFireMedallion_AdvanceTo01(DemoDu* this, PlayState* play) {
    s32 pad[2];

    if ((gSaveContext.chamberCutsceneNum == CHAMBER_CS_FIRE) && !IS_CUTSCENE_LAYER) {
        Player* player = GET_PLAYER(play);

        this->updateIndex = CS_FIREMEDALLION_SUBSCENE(1);
        play->csCtx.script = gFireMedallionCs;
        gSaveContext.cutsceneTrigger = 2;
        Item_Give(play, ITEM_MEDALLION_FIRE);

        player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
    }
}

void DemoDu_CsFireMedallion_AdvanceTo02(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if ((cue != NULL) && (cue->id != 1)) {
            this->updateIndex = CS_FIREMEDALLION_SUBSCENE(2);
            this->drawIndex = 1;
            DemoDu_CsFireMedallion_SpawnDoorWarp(this, play);
        }
    }
}

void DemoDu_CsFireMedallion_AdvanceTo03(DemoDu* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->updateIndex = CS_FIREMEDALLION_SUBSCENE(3);
        this->actor.shape.yOffset = 0.0f;
    }
}

void DemoDu_CsFireMedallion_AdvanceTo04(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[2];

        if ((cue != NULL) && (cue->id != 2)) {
            Animation_Change(&this->skelAnime, &gDaruniaItemGiveAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gDaruniaItemGiveAnim), 2, 0.0f);
            this->updateIndex = CS_FIREMEDALLION_SUBSCENE(4);
        }
    }
}

void DemoDu_CsFireMedallion_AdvanceTo05(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&this->skelAnime, &gDaruniaItemGiveIdleAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gDaruniaItemGiveIdleAnim), 0, 0.0f);
        this->updateIndex = CS_FIREMEDALLION_SUBSCENE(5);
    }
}

void DemoDu_CsFireMedallion_AdvanceTo06(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[6];

        if ((cue != NULL) && (cue->id == 2)) {
            this->updateIndex = CS_FIREMEDALLION_SUBSCENE(6);
            func_80969F38(this, play);
        }
    }
}

void DemoDu_UpdateCs_FM_00(DemoDu* this, PlayState* play) {
    DemoDu_CsFireMedallion_AdvanceTo01(this, play);
}

void DemoDu_UpdateCs_FM_01(DemoDu* this, PlayState* play) {
    DemoDu_CsFireMedallion_AdvanceTo02(this, play);
}

void DemoDu_UpdateCs_FM_02(DemoDu* this, PlayState* play) {
    func_80969FB4(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_CsFireMedallion_AdvanceTo03(this);
}

void DemoDu_UpdateCs_FM_03(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_CsFireMedallion_AdvanceTo04(this, play);
}

void DemoDu_UpdateCs_FM_04(DemoDu* this, PlayState* play) {
    s32 animFinished;

    DemoDu_UpdateBgCheckInfo(this, play);
    animFinished = DemoDu_UpdateSkelAnime(this);
    DemoDu_CsFireMedallion_AdvanceTo05(this, animFinished);
}

void DemoDu_UpdateCs_FM_05(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_CsFireMedallion_AdvanceTo06(this, play);
}

void DemoDu_UpdateCs_FM_06(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
}
