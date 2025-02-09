void En_Ru1_Actor_Switch_Init(EnRu1* this, PlayState* play) {
    s8 actorRoom;

    if (GET_INFTABLE(INFTABLE_141) && GET_INFTABLE(INFTABLE_140) && !GET_INFTABLE(INFTABLE_145) &&
        (!(En_Ru1_Search_Ru1(this, play)))) {
        En_Ru1_Change_Anime(this, &gRutoChildWait2Anim, 0, 0, 0);
        actorRoom = this->actor.room;
        this->action = 22;
        this->actor.room = -1;
        this->drawConfig = 0;
        this->roomNum1 = actorRoom;
        this->roomNum3 = actorRoom;
        this->roomNum2 = actorRoom;
        // "Ruto switch set"
        PRINTF("スイッチルトセット!!!!!!!!!!!!!!!!!!!!!!\n");
    } else {
        // "Ruto switch not set"
        PRINTF("スイッチルトセットしない!!!!!!!!!!!!!!!!!!!!!!\n");
        Actor_delete(&this->actor);
    }
}
