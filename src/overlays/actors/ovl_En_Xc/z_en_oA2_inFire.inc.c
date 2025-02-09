void func_80B3CB58(EnXc* this, PlayState* play) {
    // If hasn't learned Bolero and Player is Adult
    if (!GET_EVENTCHKINF(EVENTCHKINF_51) && LINK_IS_ADULT) {
        s32 pad;

        this->action = SHEIK_ACTION_INIT;
    } else {
        Actor_Kill(&this->actor);
    }
}

s32 EnXc_BoleroCS(EnXc* this, PlayState* play) {
    Player* player;
    PosRot* posRot;

    if (this->actor.params == SHEIK_TYPE_BOLERO) {
        player = GET_PLAYER(play);
        posRot = &player->actor.world;
        if ((posRot->pos.x > -784.0f) && (posRot->pos.x < -584.0f) && (posRot->pos.y > 447.0f) &&
            (posRot->pos.y < 647.0f) && (posRot->pos.z > -446.0f) && (posRot->pos.z < -246.0f) &&
            !Play_InCsMode(play)) {
            s32 pad;

            play->csCtx.script = SEGMENTED_TO_VIRTUAL(gDeathMountainCraterBoleroCs);
            gSaveContext.cutsceneTrigger = 1;
            SET_EVENTCHKINF(EVENTCHKINF_51);
            Item_Give(play, ITEM_SONG_BOLERO);
            return true;
        }
        return false;
    }
    return true;
}
