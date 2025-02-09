void func_80B3CA38(EnXc* this, PlayState* play) {
    // If Player is adult but hasn't learned Minuet of Forest
    if (!GET_EVENTCHKINF(EVENTCHKINF_50) && LINK_IS_ADULT) {
        s32 pad;

        this->action = SHEIK_ACTION_INIT;
    } else {
        Actor_Kill(&this->actor);
    }
}

s32 EnXc_MinuetCS(EnXc* this, PlayState* play) {
    Player* player;
    f32 playerPosZ;

    if (this->actor.params == SHEIK_TYPE_MINUET) {
        player = GET_PLAYER(play);
        playerPosZ = player->actor.world.pos.z;

        if (playerPosZ < -2225.0f) {
            if (!Play_InCsMode(play)) {
                s32 pad;

                play->csCtx.script = SEGMENTED_TO_VIRTUAL(gMeadowMinuetCs);
                gSaveContext.cutsceneTrigger = 1;
                SET_EVENTCHKINF(EVENTCHKINF_50);
                Item_Give(play, ITEM_SONG_MINUET);
                return true;
            }
        }
        return false;
    }
    return true;
}
