void func_8098F390(DemoSa* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gSariaSkel, &gSariaWaitArmsToSideAnim, NULL, NULL, 0);
    this->action = 10;
    this->drawConfig = 1;
}

void func_8098F3F0(DemoSa* this, PlayState* play) {
    func_8098E5C8(this, play);
    DemoSa_UpdateSkelAnime(this);
    func_8098E480(this);
}
