void Demo_Gj_Actor_init_battle1(DemoGj* this, PlayState* play) {
    Demo_Gj_Actor_init_battle_common(this, play, 0, 1, &gGanonsCastleRubbleAroundArenaCol);
}

// func_8097ADF0
void Demo_Gj_main_Stand_battle1(DemoGj* this, PlayState* play) {
    Demo_Gj_Search_Boss_Ganon(this, play);
    Birth1_Dust_In_Demo_Gj_battle2_1(this, play);
    Birth1_Dust_In_Demo_Gj_battle2_2(this, play);
    Birth1_Dust_In_Demo_Gj_battle2_7(this, play);
}

void Demo_Gj_draw_normal_battle1(DemoGj* this, PlayState* play) {
    Demo_Gj_draw_normal_common(this, play, gGanonsCastleRubbleAroundArenaDL);
}
