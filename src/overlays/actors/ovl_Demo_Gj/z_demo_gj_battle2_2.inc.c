void DemoGj_InitRubblePile2(DemoGj* this, PlayState* play) {
    DemoGj_InitSetIndices(this, play, 2, 3, &gGanonsCastleRubble3Col);
}

void func_8097A238(DemoGj* this, PlayState* play) {
    Actor_MoveXZGravity(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(31));
    this->rotationVec.y += (s16)(kREG(32) + 1000);
    this->rotationVec.z += (s16)(kREG(33) + 3000);

    DemoGj_SetupRotation(this, play);
}

void DemoGj_SpawnSmokePreBattle2(DemoGj* this, PlayState* play) {
    static Vec3f pos = { -119.0f, 1056.0f, -147.0f };
    u32 gameplayFrames;

    if (!this->isTransformedIntoGanon) {
        gameplayFrames = play->gameplayFrames % 3;

        if (1) {}
        if (gameplayFrames == 1) {
            if (!play->gameplayFrames) {}
            DemoGj_SpawnSmoke(play, &pos, 300.0f);
        }

        DemoGj_CheckIfTransformedIntoGanon(this);
    }
}

void func_8097A320(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfRisingFromRubble(this, play)) {
        DemoGj_SetupMovement(this, play);
        this->updateMode = 9;
        this->drawConfig = 10;
    }
}

void func_8097A36C(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfFloatingInAir(this, play)) {
        Actor_Kill(&this->dyna.actor);
    }
}

// func_8097A39C
void DemoGj_Update02(DemoGj* this, PlayState* play) {
    DemoGj_FindGanon(this, play);
    func_8097A320(this, play);
}

// func_8097A3CC
void DemoGj_Update09(DemoGj* this, PlayState* play) {
    func_8097A238(this, play);
    func_8097A36C(this, play);
}

void DemoGj_DrawRubble3(DemoGj* this, PlayState* play) {
    DemoGj_DrawCommon(this, play, gGanonsCastleRubble3DL);
}

void DemoGj_DrawRotatedRubble3(DemoGj* this, PlayState* play) {
    DemoGj_DrawRotated(this, play, gGanonsCastleRubble3DL);
}
