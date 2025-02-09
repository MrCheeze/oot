// Inits the three cylinders with `Demo_Gj_after2_OcInfoData_forStand`
void Demo_Gj_Actor_init_after2(DemoGj* this, PlayState* play) {
    Demo_Gj_Actor_init_battle_common(this, play, 16, 0, NULL);
    Demo_Gj_ct_forCorect(this, play, &this->cylinders[0], &Demo_Gj_after2_OcInfoData_forStand);
    Demo_Gj_ct_forCorect(this, play, &this->cylinders[1], &Demo_Gj_after2_OcInfoData_forStand);
    Demo_Gj_ct_forCorect(this, play, &this->cylinders[2], &Demo_Gj_after2_OcInfoData_forStand);
}

#if DEBUG_FEATURES
void DemoGj_DoNothing2(DemoGj* this, PlayState* play) {
}
#endif

// Moves the ColliderCylinder's relative to the actor's position.
void Demo_Gj_after2_SetPos_forCorect(DemoGj* this, PlayState* play) {
    ColliderCylinder* cylinder0 = &this->cylinders[0];
    ColliderCylinder* cylinder1 = &this->cylinders[1];
    ColliderCylinder* cylinder2 = &this->cylinders[2];
    Vec3f* actorPos = &this->dyna.actor.world.pos;
    s32 pad;
    s16 theta = this->dyna.actor.world.rot.y;
    f32 cos_theta = cos_s(theta);
    f32 sin_theta = sin_s(theta);

    cylinder0->dim.pos.z = actorPos->z - (35.0f * sin_theta);
    cylinder0->dim.pos.x = actorPos->x + (35.0f * cos_theta);
    cylinder0->dim.pos.y = actorPos->y;

    cylinder1->dim.pos.z = actorPos->z - (-10.0f * sin_theta);
    cylinder1->dim.pos.x = actorPos->x + (-10.0f * cos_theta);
    cylinder1->dim.pos.y = actorPos->y;

    cylinder2->dim.pos.z = actorPos->z - (-55.0f * sin_theta);
    cylinder2->dim.pos.x = actorPos->x + (-55.0f * cos_theta);
    cylinder2->dim.pos.y = actorPos->y;
}

void Demo_Gj_after2_SetAC(DemoGj* this, PlayState* play) {
    s32 pad[2];
    Collider* cylinder0 = &this->cylinders[0].base;
    Collider* cylinder1 = &this->cylinders[1].base;
    Collider* cylinder2 = &this->cylinders[2].base;
    s32 pad2[3];

    CollisionCheck_setAC(play, &play->colChkCtx, cylinder0);
    CollisionCheck_setAC(play, &play->colChkCtx, cylinder1);
    CollisionCheck_setAC(play, &play->colChkCtx, cylinder2);
}

// Does the same as `Demo_Gj_after1_Check_Break_forStand`
s32 Demo_Gj_after2_Check_Break_forStand(DemoGj* this, PlayState* play) {
    if (Demo_Gj_Check_Break_forStand(this, play, &this->cylinders[0])) {
        return true;
    }
    if (Demo_Gj_Check_Break_forStand(this, play, &this->cylinders[1])) {
        return true;
    }
    if (Demo_Gj_Check_Break_forStand(this, play, &this->cylinders[2])) {
        return true;
    }
    return false;
}

void Demo_Gj_after2_Set_Piece(DemoGj* this, PlayState* play, Vec3f* direction) {
    Vec3f pos;

    pos.x = this->dyna.actor.world.pos.x;
    pos.y = this->dyna.actor.world.pos.y;
    pos.z = this->dyna.actor.world.pos.z;

    Birth_Effect_Piece_In_Demo_Gj(this, play, &pos, direction);
}

void Demo_Gj_Check_WaitToBattle_after2(DemoGj* this, PlayState* play) {
    if (Demo_Gj_Check_MoveToStop_common(this, play)) {
        Vec3f* scale = &this->dyna.actor.scale;

        Demo_Gj_Actor_init_BGdata_common(this, play, &gGanonsCastleRubble3Col);
        this->updateMode = 19;
        this->drawConfig = 17;
        scale->x *= 0.8f;
        scale->y *= 0.8f;
        scale->z *= 0.8f;
    }
}

/*
 * Checks if should kill the actor and drop collectibles
 * Kills the actor if ganon->unk_314==4 (Ganon killed), this rubble was hit by an explosion or killFlag==true
 * Used by DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_2
 */
void Demo_Gj_Check_BattleToBranch_after2(DemoGj* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    if (Demo_Gj_Check_demo_info(this, 4)) {
        Actor_delete(thisx);
    } else if (Demo_Gj_after2_Check_Break_forStand(this, play)) {
        Vec3f vec1 = { 0.0f, 0.0f, 0.0f };

        Birth_Item_In_Demo_Gj(this, play);
        Demo_Gj_after2_Set_Piece(this, play, &vec1);

        Actor_delete(thisx);
    } else if (this->killFlag) {
        Vec3f vec2 = this->unk_26C;
        vec2.y = 0.0f;

        Birth_Item_In_Demo_Gj(this, play);
        Demo_Gj_after2_Set_Piece(this, play, &vec2);

        Actor_delete(thisx);
    }

    Demo_Gj_after2_SetPos_forCorect(this, play);
    Demo_Gj_after2_SetAC(this, play);
}

// func_8097B864
void Demo_Gj_main_Wait_after2(DemoGj* this, PlayState* play) {
    Demo_Gj_Search_Boss_Ganon(this, play);
    Demo_Gj_Check_WaitToBattle_after2(this, play);
}

// func_8097B894
void Demo_Gj_main_Battle_after2(DemoGj* this, PlayState* play) {
    Demo_Gj_Check_BattleToBranch_after2(this, play);
#if DEBUG_FEATURES
    DemoGj_DoNothing2(this, play);
#endif
}

void Demo_Gj_draw_normal_after2(DemoGj* this, PlayState* play) {
    Demo_Gj_draw_normal_common(this, play, gGanonsCastleRubble3DL);
}
