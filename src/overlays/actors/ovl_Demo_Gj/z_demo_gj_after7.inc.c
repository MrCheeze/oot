// Inits the first cylinder (only that one) with `Demo_Gj_after7_OcInfoData_forStand`
void Demo_Gj_Actor_init_after7(DemoGj* this, PlayState* play) {
    Demo_Gj_Actor_init_battle_common(this, play, 17, 0, NULL);
    Demo_Gj_ct_forCorect(this, play, &this->cylinders[0], &Demo_Gj_after7_OcInfoData_forStand);
}

#if DEBUG_FEATURES
void DemoGj_DoNothing3(DemoGj* this, PlayState* play) {
}
#endif

void Demo_Gj_after7_Set_Piece(DemoGj* this, PlayState* play, Vec3f* direction) {
    Vec3f pos;

    pos.x = this->dyna.actor.world.pos.x;
    pos.y = this->dyna.actor.world.pos.y;
    pos.z = this->dyna.actor.world.pos.z;
    Birth_Effect_Piece_In_Demo_Gj(this, play, &pos, direction);

    pos.x = this->dyna.actor.world.pos.x;
    pos.y = this->dyna.actor.world.pos.y + 100.0f;
    pos.z = this->dyna.actor.world.pos.z;
    Birth_Effect_Piece_In_Demo_Gj(this, play, &pos, direction);
}

void Demo_Gj_Check_WaitToBattle_after7(DemoGj* this, PlayState* play) {
    if (Demo_Gj_Check_MoveToStop_common(this, play)) {
        Vec3f* scale = &this->dyna.actor.scale;

        Demo_Gj_Actor_init_BGdata_common(this, play, &gGanonsCastleRubbleTallCol);
        this->updateMode = 20;
        this->drawConfig = 18;
        scale->x *= 0.8f;
        scale->y *= 0.8f;
        scale->z *= 0.8f;
    }
}

/*
 * Checks if should kill the actor and drop collectibles
 * Kills the actor if ganon->unk_314==4 (Ganon killed), this rubble was hit by an explosion or killFlag==true
 * Used by DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_TALL
 */
void Demo_Gj_Check_BattleToBranch_after7(DemoGj* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;
    ColliderCylinder* cylinder = &this->cylinders[0];
    s32 pad[2];

    if (Demo_Gj_Check_demo_info(this, 4)) {
        Actor_delete(thisx);
    } else if (Demo_Gj_Check_Break_forStand(this, play, cylinder)) {
        Vec3f vec1 = { 0.0f, 0.0f, 0.0f };

        Birth_Item_In_Demo_Gj(this, play);
        Demo_Gj_after7_Set_Piece(this, play, &vec1);

        Actor_delete(thisx);
    } else if (this->killFlag) {
        Vec3f vec2 = this->unk_26C;
        vec2.y = 0.0f;

        Birth_Item_In_Demo_Gj(this, play);
        Demo_Gj_after7_Set_Piece(this, play, &vec2);

        Actor_delete(thisx);
    }

    CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, cylinder);
    CollisionCheck_setAC(play, &play->colChkCtx, &cylinder->base);
}

// func_8097BB78
void Demo_Gj_main_Wait_after7(DemoGj* this, PlayState* play) {
    Demo_Gj_Search_Boss_Ganon(this, play);
    Demo_Gj_Check_WaitToBattle_after7(this, play);
}

// func_8097BBA8
void Demo_Gj_main_Battle_after7(DemoGj* this, PlayState* play) {
    Demo_Gj_Check_BattleToBranch_after7(this, play);
#if DEBUG_FEATURES
    DemoGj_DoNothing3(this, play);
#endif
}

void Demo_Gj_draw_normal_after7(DemoGj* this, PlayState* play) {
    Demo_Gj_draw_normal_common(this, play, gGanonsCastleRubbleTallDL);
}
