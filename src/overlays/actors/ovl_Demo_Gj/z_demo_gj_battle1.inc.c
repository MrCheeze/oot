void DemoGj_InitRubbleAroundArena(DemoGj* this, PlayState* play) {
    DemoGj_InitSetIndices(this, play, 0, 1, &gGanonsCastleRubbleAroundArenaCol);
}

// func_8097ADF0
void DemoGj_UpdateRubbleAroundArena(DemoGj* this, PlayState* play) {
    DemoGj_FindGanon(this, play);
    DemoGj_SpawnSmokePreBattle1(this, play);
    DemoGj_SpawnSmokePreBattle2(this, play);
    DemoGj_SpawnSmokePreBattle3(this, play);
}

void DemoGj_DrawRubbleAroundArena(DemoGj* this, PlayState* play) {
    DemoGj_DrawCommon(this, play, gGanonsCastleRubbleAroundArenaDL);
}
