void Demo_Sa_Stand_Init(DemoSa* this, PlayState* play) {
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gSariaSkel, &gSariaWaitArmsToSideAnim, NULL, NULL, 0);
    this->action = 10;
    this->drawConfig = 1;
}

void Demo_Sa_Stand_Actor_main_gaze(DemoSa* this, PlayState* play) {
    Demo_Sa_BGcheck(this, play);
    Demo_Sa_Animation_Base(this);
    Demo_Sa_set_eye_pattern(this);
}
