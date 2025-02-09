void DemoGj_InitRubblePile3(DemoGj* this, PlayState* play) {
    DemoGj_InitSetIndices(this, play, 3, 4, &gGanonsCastleRubble4Col);
}

void func_8097A474(DemoGj* this, PlayState* play) {
    Actor_MoveXZGravity(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(44));
    this->rotationVec.y += (s16)(kREG(45) + 1000);
    this->rotationVec.z += (s16)(kREG(46) + 3000);

    DemoGj_SetupRotation(this, play);
}

void func_8097A4F0(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfRisingFromRubble(this, play)) {
        DemoGj_SetupMovement(this, play);
        this->updateMode = 10;
        this->drawConfig = 11;
    }
}

void func_8097A53C(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfFloatingInAir(this, play)) {
        Actor_Kill(&this->dyna.actor);
    }
}

// func_8097A56C
void DemoGj_Update03(DemoGj* this, PlayState* play) {
    DemoGj_FindGanon(this, play);
    func_8097A4F0(this, play);
}

// func_8097A59C
void DemoGj_Update10(DemoGj* this, PlayState* play) {
    func_8097A474(this, play);
    func_8097A53C(this, play);
}

void DemoGj_DrawRubble4(DemoGj* this, PlayState* play) {
    DemoGj_DrawCommon(this, play, gGanonsCastleRubble4DL);
}

void DemoGj_DrawRotatedRubble4(DemoGj* this, PlayState* play) {
    DemoGj_DrawRotated(this, play, gGanonsCastleRubble4DL);
}
