void DemoGj_InitRubblePile5(DemoGj* this, PlayState* play) {
    DemoGj_InitSetIndices(this, play, 5, 6, &gGanonsCastleRubble6Col);
}

void func_8097A814(DemoGj* this, PlayState* play) {
    Actor_MoveXZGravity(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(70));
    this->rotationVec.y += (s16)(kREG(71) + 1000);
    this->rotationVec.z += (s16)(kREG(72) + 3000);

    DemoGj_SetupRotation(this, play);
}

void func_8097A890(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfRisingFromRubble(this, play)) {
        DemoGj_SetupMovement(this, play);
        this->updateMode = 12;
        this->drawConfig = 13;
    }
}

void func_8097A8DC(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfFloatingInAir(this, play)) {
        Actor_Kill(&this->dyna.actor);
    }
}

// func_8097A90C
void DemoGj_Update05(DemoGj* this, PlayState* play) {
    DemoGj_FindGanon(this, play);
    func_8097A890(this, play);
}

// func_8097A93C
void DemoGj_Update12(DemoGj* this, PlayState* play) {
    func_8097A814(this, play);
    func_8097A8DC(this, play);
}

void DemoGj_DrawRubble6(DemoGj* this, PlayState* play) {
    DemoGj_DrawCommon(this, play, gGanonsCastleRubble6DL);
}

void DemoGj_DrawRotatedRubble6(DemoGj* this, PlayState* play) {
    DemoGj_DrawRotated(this, play, gGanonsCastleRubble6DL);
}
