void Demo_Gj_Actor_init_battle2_2(DemoGj* this, PlayState* play) {
    Demo_Gj_Actor_init_battle_common(this, play, 2, 3, &gGanonsCastleRubble3Col);
}

void Demo_Gj_battle2_2_Movement(DemoGj* this, PlayState* play) {
    Actor_position_moveF(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(31));
    this->rotationVec.y += (s16)(kREG(32) + 1000);
    this->rotationVec.z += (s16)(kREG(33) + 3000);

    Demo_Gj_common_Reflect(this, play);
}

void Birth1_Dust_In_Demo_Gj_battle2_2(DemoGj* this, PlayState* play) {
    static Vec3f pos = { -119.0f, 1056.0f, -147.0f };
    u32 gameplayFrames;

    if (!this->isTransformedIntoGanon) {
        gameplayFrames = play->gameplayFrames % 3;

        if (1) {}
        if (gameplayFrames == 1) {
            if (!play->gameplayFrames) {}
            Birth1_Dust_In_Demo_Gj(play, &pos, 300.0f);
        }

        Demo_Gj_Check_StopDust_common(this);
    }
}

void Demo_Gj_Check_StandToMove_battle2_2(DemoGj* this, PlayState* play) {
    if (Demo_Gj_Check_StandToMove_common(this, play)) {
        Demo_Gj_Setup_Move_common(this, play);
        this->updateMode = 9;
        this->drawConfig = 10;
    }
}

void Demo_Gj_battle2_2_Check_MoveToStop(DemoGj* this, PlayState* play) {
    if (Demo_Gj_Check_MoveToStop_common(this, play)) {
        Actor_delete(&this->dyna.actor);
    }
}

// func_8097A39C
void Demo_Gj_main_Stand_battle2_2(DemoGj* this, PlayState* play) {
    Demo_Gj_Search_Boss_Ganon(this, play);
    Demo_Gj_Check_StandToMove_battle2_2(this, play);
}

// func_8097A3CC
void Demo_Gj_main_Move_battle2_2(DemoGj* this, PlayState* play) {
    Demo_Gj_battle2_2_Movement(this, play);
    Demo_Gj_battle2_2_Check_MoveToStop(this, play);
}

void Demo_Gj_draw_normal_battle2_2(DemoGj* this, PlayState* play) {
    Demo_Gj_draw_normal_common(this, play, gGanonsCastleRubble3DL);
}

void Demo_Gj_draw_move_battle2_2(DemoGj* this, PlayState* play) {
    Demo_Gj_draw_move_common(this, play, gGanonsCastleRubble3DL);
}
