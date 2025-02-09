void En_Ru1_Actor_Stand_Init(EnRu1* this, PlayState* play) {
    if (GET_INFTABLE(INFTABLE_145) && !GET_INFTABLE(INFTABLE_146)) {
        s32 pad;

        En_Ru1_Change_Anime(this, &gRutoChildWait2Anim, 0, 0, 0);
        this->action = 41;
        this->unk_28C = En_Ru1_Search_Stand_Actor(play);
        En_Ru1_SetConect_inStand(this, 1);
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    } else {
        Actor_delete(&this->actor);
    }
}

void En_Ru1_Option_Actor_main_search_stand(EnRu1* this, PlayState* play) {
    this->unk_28C = En_Ru1_Search_Stand_Actor(play);
    if (this->unk_28C != NULL) {
        this->action = 42;
        this->drawConfig = 1;
        En_Ru1_SetConect_inStand(this, 1);
    }
}
