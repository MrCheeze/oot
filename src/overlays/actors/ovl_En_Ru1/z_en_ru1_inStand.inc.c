void func_80AEFC54(EnRu1* this, PlayState* play) {
    if (GET_INFTABLE(INFTABLE_145) && !GET_INFTABLE(INFTABLE_146)) {
        s32 pad;

        func_80AEB264(this, &gRutoChildWait2Anim, 0, 0, 0);
        this->action = 41;
        this->unk_28C = EnRu1_FindSwitch(play);
        func_80AEB0EC(this, 1);
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    } else {
        Actor_Kill(&this->actor);
    }
}

void func_80AEFCE8(EnRu1* this, PlayState* play) {
    this->unk_28C = EnRu1_FindSwitch(play);
    if (this->unk_28C != NULL) {
        this->action = 42;
        this->drawConfig = 1;
        func_80AEB0EC(this, 1);
    }
}
