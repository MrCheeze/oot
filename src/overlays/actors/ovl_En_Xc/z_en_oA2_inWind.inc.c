void En_Oa2_Actor_Wind_Init(EnXc* this, PlayState* play) {
    // If Player is adult but hasn't learned Minuet of Forest
    if (!GET_EVENTCHKINF(EVENTCHKINF_50) && LINK_IS_ADULT) {
        s32 pad;

        this->action = SHEIK_ACTION_INIT;
    } else {
        Actor_delete(&this->actor);
    }
}

s32 En_Oa2_Set_DemoWind(EnXc* this, PlayState* play) {
    Player* player;
    f32 playerPosZ;

    if (this->actor.params == SHEIK_TYPE_MINUET) {
        player = GET_PLAYER(play);
        playerPosZ = player->actor.world.pos.z;

        if (playerPosZ < -2225.0f) {
            if (!Game_play_demo_mode_check(play)) {
                s32 pad;

                play->csCtx.script = SEGMENTED_TO_VIRTUAL(gMeadowMinuetCs);
                z_common_data.cutsceneTrigger = 1;
                SET_EVENTCHKINF(EVENTCHKINF_50);
                item_get_setting(play, ITEM_SONG_MINUET);
                return true;
            }
        }
        return false;
    }
    return true;
}
