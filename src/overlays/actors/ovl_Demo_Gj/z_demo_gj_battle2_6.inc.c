void DemoGj_InitRubblePile6(DemoGj* this, PlayState* play) {
    DemoGj_InitSetIndices(this, play, 6, 7, &gGanonsCastleRubble7Col);
}

void func_8097A9E4(DemoGj* this, PlayState* play) {
    Actor_MoveXZGravity(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(83));
    this->rotationVec.y += (s16)(kREG(84) + 1000);
    this->rotationVec.z += (s16)(kREG(85) + 3000);

    DemoGj_SetupRotation(this, play);
}

void func_8097AA60(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfRisingFromRubble(this, play)) {
        DemoGj_SetupMovement(this, play);
        this->updateMode = 13;
        this->drawConfig = 14;
    }
}

void func_8097AAAC(DemoGj* this, PlayState* play) {
    if (DemoGj_IsGanondorfFloatingInAir(this, play)) {
        Actor_Kill(&this->dyna.actor);
    }
}

// func_8097AADC
void DemoGj_Update06(DemoGj* this, PlayState* play) {
    DemoGj_FindGanon(this, play);
    func_8097AA60(this, play);
}

// func_8097AB0C
void DemoGj_Update13(DemoGj* this, PlayState* play) {
    func_8097A9E4(this, play);
    func_8097AAAC(this, play);
}

void DemoGj_DrawRubble7(DemoGj* this, PlayState* play) {
    DemoGj_DrawCommon(this, play, gGanonsCastleRubble7DL);
}

void DemoGj_DrawRotatedRubble7(DemoGj* this, PlayState* play) {
    DemoGj_DrawRotated(this, play, gGanonsCastleRubble7DL);
}
