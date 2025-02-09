void En_Oa2_Actor_Fire_Init(EnXc* this, PlayState* play) {
    // If hasn't learned Bolero and Player is Adult
    if (!GET_EVENTCHKINF(EVENTCHKINF_51) && LINK_IS_ADULT) {
        s32 pad;

        this->action = SHEIK_ACTION_INIT;
    } else {
        Actor_delete(&this->actor);
    }
}

s32 En_Oa2_Set_DemoFire(EnXc* this, PlayState* play) {
    Player* player;
    PosRot* posRot;

    if (this->actor.params == SHEIK_TYPE_BOLERO) {
        player = GET_PLAYER(play);
        posRot = &player->actor.world;
        if ((posRot->pos.x > -784.0f) && (posRot->pos.x < -584.0f) && (posRot->pos.y > 447.0f) &&
            (posRot->pos.y < 647.0f) && (posRot->pos.z > -446.0f) && (posRot->pos.z < -246.0f) &&
            !Game_play_demo_mode_check(play)) {
            s32 pad;

            play->csCtx.script = SEGMENTED_TO_VIRTUAL(gDeathMountainCraterBoleroCs);
            z_common_data.cutsceneTrigger = 1;
            SET_EVENTCHKINF(EVENTCHKINF_51);
            item_get_setting(play, ITEM_SONG_BOLERO);
            return true;
        }
        return false;
    }
    return true;
}
