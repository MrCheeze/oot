void Demo_Gj_Actor_init_battle2_3(DemoGj* this, PlayState* play) {
    Demo_Gj_Actor_init_battle_common(this, play, 3, 4, &gGanonsCastleRubble4Col);
}

void Demo_Gj_battle2_3_Movement(DemoGj* this, PlayState* play) {
    Actor_position_moveF(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(44));
    this->rotationVec.y += (s16)(kREG(45) + 1000);
    this->rotationVec.z += (s16)(kREG(46) + 3000);

    Demo_Gj_common_Reflect(this, play);
}

void Demo_Gj_Check_StandToMove_battle2_3(DemoGj* this, PlayState* play) {
    if (Demo_Gj_Check_StandToMove_common(this, play)) {
        Demo_Gj_Setup_Move_common(this, play);
        this->updateMode = 10;
        this->drawConfig = 11;
    }
}

void Demo_Gj_battle2_3_Check_MoveToStop(DemoGj* this, PlayState* play) {
    if (Demo_Gj_Check_MoveToStop_common(this, play)) {
        Actor_delete(&this->dyna.actor);
    }
}

// func_8097A56C
void Demo_Gj_main_Stand_battle2_3(DemoGj* this, PlayState* play) {
    Demo_Gj_Search_Boss_Ganon(this, play);
    Demo_Gj_Check_StandToMove_battle2_3(this, play);
}

// func_8097A59C
void Demo_Gj_main_Move_battle2_3(DemoGj* this, PlayState* play) {
    Demo_Gj_battle2_3_Movement(this, play);
    Demo_Gj_battle2_3_Check_MoveToStop(this, play);
}

void Demo_Gj_draw_normal_battle2_3(DemoGj* this, PlayState* play) {
    Demo_Gj_draw_normal_common(this, play, gGanonsCastleRubble4DL);
}

void Demo_Gj_draw_move_battle2_3(DemoGj* this, PlayState* play) {
    Demo_Gj_draw_move_common(this, play, gGanonsCastleRubble4DL);
}
