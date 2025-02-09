void DemoGj_InitRubblePile1(DemoGj* this, PlayState* play) {
    DemoGj_InitSetIndices(this, play, 1, 2, &gGanonsCastleRubble2Col);
}

void func_8097A000(DemoGj* this, PlayState* play) {
    Actor_MoveXZGravity(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(18));
    this->rotationVec.y += (s16)(kREG(19) + 1000);
    this->rotationVec.z += (s16)(kREG(20) + 3000);

    DemoGj_SetupRotation(this, play);
}

void DemoGj_SpawnSmokePreBattle1(DemoGj* this, PlayState* play) {
    static Vec3f pos = { -371.0f, 1188.0f, -303.0f };
    u32 gameplayFrames;

    if (!this->isTransformedIntoGanon) {
        gameplayFrames = play->gameplayFrames % 3;

        if (1) {}
        if (gameplayFrames == 0) {
            if (!play->gameplayFrames) {}
            DemoGj_SpawnSmoke(play, &pos, 300.0f);
        }

        DemoGj_CheckIfTransformedIntoGanon(this);
    }
}

void func_8097A0E4(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfRisingFromRubble(this, play)) {
        DemoGj_SetupMovement(this, play);
        this->updateMode = 8;
        this->drawConfig = 9;
    }
}

void func_8097A130(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfFloatingInAir(this, play)) {
        Actor_Kill(&this->dyna.actor);
    }
}

// func_8097A160
void DemoGj_Update01(DemoGj* this, PlayState* play) {
    DemoGj_FindGanon(this, play);
    func_8097A0E4(this, play);
}

// func_8097A190
void DemoGj_Update08(DemoGj* this, PlayState* play) {
    func_8097A000(this, play);
    func_8097A130(this, play);
}

void DemoGj_DrawRubble2(DemoGj* this, PlayState* play) {
    DemoGj_DrawCommon(this, play, gGanonsCastleRubble2DL);
}

void DemoGj_DrawRotatedRubble2(DemoGj* this, PlayState* play) {
    DemoGj_DrawRotated(this, play, gGanonsCastleRubble2DL);
}
