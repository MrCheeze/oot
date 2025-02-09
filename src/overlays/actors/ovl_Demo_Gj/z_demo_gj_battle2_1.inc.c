void Demo_Gj_Actor_init_battle2_1(DemoGj* this, PlayState* play) {
    Demo_Gj_Actor_init_battle_common(this, play, 1, 2, &gGanonsCastleRubble2Col);
}

void Demo_Gj_battle2_1_Movement(DemoGj* this, PlayState* play) {
    Actor_position_moveF(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(18));
    this->rotationVec.y += (s16)(kREG(19) + 1000);
    this->rotationVec.z += (s16)(kREG(20) + 3000);

    Demo_Gj_common_Reflect(this, play);
}

void Birth1_Dust_In_Demo_Gj_battle2_1(DemoGj* this, PlayState* play) {
    static Vec3f pos = { -371.0f, 1188.0f, -303.0f };
    u32 gameplayFrames;

    if (!this->isTransformedIntoGanon) {
        gameplayFrames = play->gameplayFrames % 3;

        if (1) {}
        if (gameplayFrames == 0) {
            if (!play->gameplayFrames) {}
            Birth1_Dust_In_Demo_Gj(play, &pos, 300.0f);
        }

        Demo_Gj_Check_StopDust_common(this);
    }
}

void Demo_Gj_Check_StandToMove_battle2_1(DemoGj* this, PlayState* play) {
    if (Demo_Gj_Check_StandToMove_common(this, play)) {
        Demo_Gj_Setup_Move_common(this, play);
        this->updateMode = 8;
        this->drawConfig = 9;
    }
}

void Demo_Gj_battle2_1_Check_MoveToStop(DemoGj* this, PlayState* play) {
    if (Demo_Gj_Check_MoveToStop_common(this, play)) {
        Actor_delete(&this->dyna.actor);
    }
}

// func_8097A160
void Demo_Gj_main_Stand_battle2_1(DemoGj* this, PlayState* play) {
    Demo_Gj_Search_Boss_Ganon(this, play);
    Demo_Gj_Check_StandToMove_battle2_1(this, play);
}

// func_8097A190
void Demo_Gj_main_Move_battle2_1(DemoGj* this, PlayState* play) {
    Demo_Gj_battle2_1_Movement(this, play);
    Demo_Gj_battle2_1_Check_MoveToStop(this, play);
}

void Demo_Gj_draw_normal_battle2_1(DemoGj* this, PlayState* play) {
    Demo_Gj_draw_normal_common(this, play, gGanonsCastleRubble2DL);
}

void Demo_Gj_draw_move_battle2_1(DemoGj* this, PlayState* play) {
    Demo_Gj_draw_move_common(this, play, gGanonsCastleRubble2DL);
}
