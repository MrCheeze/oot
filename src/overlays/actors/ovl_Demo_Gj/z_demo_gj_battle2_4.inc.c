void Demo_Gj_Actor_init_battle2_4(DemoGj* this, PlayState* play) {
    Demo_Gj_Actor_init_battle_common(this, play, 4, 5, &gGanonsCastleRubble5Col);
}

void Demo_Gj_battle2_4_Movement(DemoGj* this, PlayState* play) {
    Actor_position_moveF(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(57));
    this->rotationVec.y += (s16)(kREG(58) + 1000);
    this->rotationVec.z += (s16)(kREG(59) + 3000);

    Demo_Gj_common_Reflect(this, play);
}

void Demo_Gj_Check_StandToMove_battle2_4(DemoGj* this, PlayState* play) {
    if (Demo_Gj_Check_StandToMove_common(this, play)) {
        Demo_Gj_Setup_Move_common(this, play);
        this->updateMode = 11;
        this->drawConfig = 12;
    }
}

void Demo_Gj_battle2_4_Check_MoveToStop(DemoGj* this, PlayState* play) {
    if (Demo_Gj_Check_MoveToStop_common(this, play)) {
        Actor_delete(&this->dyna.actor);
    }
}

// func_8097A73C
void Demo_Gj_main_Stand_battle2_4(DemoGj* this, PlayState* play) {
    Demo_Gj_Search_Boss_Ganon(this, play);
    Demo_Gj_Check_StandToMove_battle2_4(this, play);
}

// func_8097A76C
void Demo_Gj_main_Move_battle2_4(DemoGj* this, PlayState* play) {
    Demo_Gj_battle2_4_Movement(this, play);
    Demo_Gj_battle2_4_Check_MoveToStop(this, play);
}

void Demo_Gj_draw_normal_battle2_4(DemoGj* this, PlayState* play) {
    Demo_Gj_draw_normal_common(this, play, gGanonsCastleRubble5DL);
}

void Demo_Gj_draw_move_battle2_4(DemoGj* this, PlayState* play) {
    Demo_Gj_draw_move_common(this, play, gGanonsCastleRubble5DL);
}
