void Demo_Gt_Actor_init_part4_2(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;

    Demo_Gt_Actor_init_part_common(this, play, 4, 5, NULL);
}

void Demo_Gt_part4_2_Set_BrokenSound(DemoGt* this, PlayState* play) {
    u16 csCurFrame = play->csCtx.curFrame;

    if (csCurFrame == 789) {
        Na_StartObjectSe_F(&this->dyna.actor.projectedPos, NA_SE_EV_TOWER_PARTS_BROKEN - SFX_FLAG);
    }
}

void Demo_Gt_part4_2_SetDust_inDraw(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f sp58;
    Vec3f dustPos;
    u16 csCurFrame = play->csCtx.curFrame;

    if (((csCurFrame > 855) && (csCurFrame < 891)) || (DEBUG_FEATURES && (kREG(1) == 13))) {
        Vec3f velOffset = { 0.0f, -30.0f, 0.0f };
        s32 pad1[3];

        sp58.x = 0.0f;
        sp58.y = 1170.0f;
        sp58.z = -1100.0f;

        Matrix_Position(&sp58, &dustPos);
        Birth_BirrarDust_In_Demo_Gt(play, &dustPos, &velOffset, 7.1f, 5, 1, 30);
    }
}

void Demo_Gt_check_StandToBranch_part4_2(DemoGt* this, PlayState* play) {
    if (Demo_Gt_Check_npcdemopnt(play, 2, 5)) {
        this->updateMode = 12;
    } else if (Demo_Gt_Check_npcdemopnt(play, 3, 5)) {
        this->updateMode = 17;
    }
}

void Demo_Gt_main_Stand_part4_2(DemoGt* this, PlayState* play) {
    Demo_Gt_SetPos_fromOffset(this, 5);
    Demo_Gt_part4_2_Set_BrokenSound(this, play);
    Demo_Gt_check_StandToBranch_part4_2(this, play);
}

void Demo_Gt_main_Fall_part4_2(DemoGt* this, PlayState* play) {
    Demo_Gt_SetPos_forFall(this, play, 5);
    Demo_Gt_part4_2_Set_BrokenSound(this, play);
}

void Demo_Gt_main_Lay_part4_2(DemoGt* this, PlayState* play) {
    f32 temp = this->unk_172;

    this->unk_174 = temp * ((kREG(66) * 0.001f) + 0.048f) + (kREG(67) + 50.0f);
    this->unk_172 += this->unk_174;

    if (this->unk_172 > 0x4000) {
        this->unk_172 = 0x4000;
    }
}

void Demo_Gt_draw_normal_part4_2(Actor* thisx, PlayState* play) {
    GraphicsContext* gfxCtx;
    DemoGt* this = (DemoGt*)thisx;
    s16 sp76;
    f32 sp70;
    f32 sp6C;
    s16 sp6A;
    s16 sp68;
    s16 pad1;
    Mtx* sp60;
    Vec3f sp54;
    Vec3f sp48;
    f32 sp44;

    sp76 = this->unk_172;
    sp70 = fabsf(BINANG_TO_RAD(sp76));
    sp6C = kREG(62);
    sp6A = kREG(59) - 0x4000;
    sp68 = (s16)(kREG(59) - 0x4000) + 0x4000;
    gfxCtx = play->state.gfxCtx;
    sp60 = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    sp44 = 1 - cos_s(sp76);

    OPEN_DISPS(gfxCtx, "../z_demo_gt_part4_2.c", 212);

    sp54.x = cos_s(sp68);
    sp54.y = 0.0f;
    sp54.z = sin_s(sp68);

    sp48.x = cos_s(sp6A) * sp6C * sp44;
    sp48.y = sin_s(sp76) * sp6C;
    sp48.z = sin_s(sp6A) * sp6C * sp44;

    Matrix_push();

    Matrix_rotateVector(sp70, &sp54, MTXMODE_APPLY);
    Matrix_translate(sp48.x, sp48.y, sp48.z, MTXMODE_APPLY);
    MATRIX_TO_MTX(sp60, "../z_demo_gt_part4_2.c", 227);

    if (!_Game_play_isPause(play)) {
        Demo_Gt_part4_2_SetDust_inDraw(this, play);
    }

    Matrix_pull();

    _texture_z_light_fog_prim(gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, sp60, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsStandalonePillarDL);
    gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx, "../z_demo_gt_part4_2.c", 241);
}
