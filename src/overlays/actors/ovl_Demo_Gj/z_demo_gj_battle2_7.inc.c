void DemoGj_InitRubblePile7(DemoGj* this, PlayState* play) {
    DemoGj_InitSetIndices(this, play, 7, 8, &gGanonsCastleRubbleTallCol);
}

void func_8097ABB4(DemoGj* this, PlayState* play) {
    Actor_MoveXZGravity(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(15));
    this->rotationVec.y += (s16)(kREG(14) + 1000);
    this->rotationVec.z += (s16)(kREG(13) + 3000);

    DemoGj_SetupRotation(this, play);
}

void DemoGj_SpawnSmokePreBattle3(DemoGj* this, PlayState* play) {
    static Vec3f pos = { -6.0f, 1053.0f, -473.0f };
    u32 gameplayFrames;

    if (!this->isTransformedIntoGanon) {
        gameplayFrames = play->gameplayFrames % 3;

        if (1) {}
        if (gameplayFrames == 2) {
            if (!play->gameplayFrames) {}
            DemoGj_SpawnSmoke(play, &pos, 300.0f);
        }

        DemoGj_CheckIfTransformedIntoGanon(this);
    }
}

void func_8097AC9C(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfRisingFromRubble(this, play)) {
        DemoGj_SetupMovement(this, play);
        this->updateMode = 14;
        this->drawConfig = 15;
    }
}

void func_8097ACE8(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfFloatingInAir(this, play)) {
        Actor_Kill(&this->dyna.actor);
    }
}

// func_8097AD18
void DemoGj_Update07(DemoGj* this, PlayState* play) {
    DemoGj_FindGanon(this, play);
    func_8097AC9C(this, play);
}

// func_8097AD48
void DemoGj_Update14(DemoGj* this, PlayState* play) {
    func_8097ABB4(this, play);
    func_8097ACE8(this, play);
}

void DemoGj_DrawRubbleTall(DemoGj* this, PlayState* play) {
    DemoGj_DrawCommon(this, play, gGanonsCastleRubbleTallDL);
}

void DemoGj_DrawRotatedRubbleTall(DemoGj* this, PlayState* play) {
    DemoGj_DrawRotated(this, play, gGanonsCastleRubbleTallDL);
}
