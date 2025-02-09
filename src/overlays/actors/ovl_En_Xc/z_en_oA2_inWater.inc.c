void En_Oa2_Actor_Water_Init(EnXc* this, PlayState* play) {
    if (!(CHECK_OWNED_EQUIP(EQUIP_TYPE_BOOTS, EQUIP_INV_BOOTS_IRON) && DEBUG_FEATURES) &&
        !GET_EVENTCHKINF(EVENTCHKINF_52) && LINK_IS_ADULT) {
        s32 pad;

        this->action = SHEIK_ACTION_SERENADE;
        PRINTF("水のセレナーデ シーク誕生!!!!!!!!!!!!!!!!!!\n");
    } else {
        Actor_delete(&this->actor);
        PRINTF("水のセレナーデ シーク消滅!!!!!!!!!!!!!!!!!!\n");
    }
}

s32 En_Oa2_Set_DemoWater(EnXc* this, PlayState* play) {
    if (this->actor.params == SHEIK_TYPE_SERENADE) {
        Player* player = GET_PLAYER(play);
        s32 stateFlags = player->stateFlags1;

        if (CHECK_OWNED_EQUIP(EQUIP_TYPE_BOOTS, EQUIP_INV_BOOTS_IRON) && !GET_EVENTCHKINF(EVENTCHKINF_52) &&
            !(stateFlags & PLAYER_STATE1_29) && !Game_play_demo_mode_check(play)) {
            s32 pad;

            set_showdata(play, gIceCavernSerenadeCs);
            z_common_data.cutsceneTrigger = 1;
            SET_EVENTCHKINF(EVENTCHKINF_52); // Learned Serenade of Water Flag
            item_get_setting(play, ITEM_SONG_SERENADE);
            PRINTF("ブーツを取った!!!!!!!!!!!!!!!!!!\n");
            return true;
        } else {
            PRINTF("はやくブーツを取るべし!!!!!!!!!!!!!!!!!!\n");
            return false;
        }
    }
    return true;
}
