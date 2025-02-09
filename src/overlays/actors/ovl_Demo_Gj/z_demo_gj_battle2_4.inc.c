void DemoGj_InitRubblePile4(DemoGj* this, PlayState* play) {
    DemoGj_InitSetIndices(this, play, 4, 5, &gGanonsCastleRubble5Col);
}

void func_8097A644(DemoGj* this, PlayState* play) {
    Actor_MoveXZGravity(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(57));
    this->rotationVec.y += (s16)(kREG(58) + 1000);
    this->rotationVec.z += (s16)(kREG(59) + 3000);

    DemoGj_SetupRotation(this, play);
}

void func_8097A6C0(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfRisingFromRubble(this, play)) {
        DemoGj_SetupMovement(this, play);
        this->updateMode = 11;
        this->drawConfig = 12;
    }
}

void func_8097A70C(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfFloatingInAir(this, play)) {
        Actor_Kill(&this->dyna.actor);
    }
}

// func_8097A73C
void DemoGj_Update04(DemoGj* this, PlayState* play) {
    DemoGj_FindGanon(this, play);
    func_8097A6C0(this, play);
}

// func_8097A76C
void DemoGj_Update11(DemoGj* this, PlayState* play) {
    func_8097A644(this, play);
    func_8097A70C(this, play);
}

void DemoGj_DrawRubble5(DemoGj* this, PlayState* play) {
    DemoGj_DrawCommon(this, play, gGanonsCastleRubble5DL);
}

void DemoGj_DrawRotatedRubble5(DemoGj* this, PlayState* play) {
    DemoGj_DrawRotated(this, play, gGanonsCastleRubble5DL);
}
