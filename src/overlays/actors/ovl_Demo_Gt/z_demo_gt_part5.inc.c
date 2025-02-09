void Demo_Gt_Actor_init_part5(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;
    Demo_Gt_Actor_init_part_common(this, play, 6, 7, NULL);
}

void Demo_Gt_part5_Set_BrokenSound(DemoGt* this, PlayState* play) {
    u16 csCurFrame = play->csCtx.curFrame;

    if (csCurFrame == 183) {
        Na_StartObjectSe_F(&this->dyna.actor.projectedPos, NA_SE_EV_TOWER_PARTS_BROKEN - SFX_FLAG);
    }
}

void Demo_Gt_SetAngle_forLay_part5(DemoGt* this) {
    f32 temp = this->unk_172;

    this->unk_174 = temp * ((kREG(64) * 0.001f) + 0.048f) + (kREG(76) + 100.0f);
    this->unk_172 += this->unk_174;

    if (this->unk_172 > (s16)(kREG(80) + 0x4000)) {
        this->unk_172 = kREG(80) + 0x4000;
    }
}

void Demo_Gt_check_StandToFall_part5(DemoGt* this, PlayState* play) {
    if (Demo_Gt_Check_npcdemopnt(play, 2, 7)) {
        this->updateMode = 0xE;
    }
}

void Demo_Gt_main_Stand_part5(DemoGt* this, PlayState* play) {
    Demo_Gt_SetPos_fromOffset(this, 7);
    Demo_Gt_part5_Set_BrokenSound(this, play);
    Demo_Gt_check_StandToFall_part5(this, play);
}

void Demo_Gt_main_Fall_part5(DemoGt* this, PlayState* play) {
    Demo_Gt_SetAngle_forLay_part5(this);
    Demo_Gt_SetPos_forFall(this, play, 7);
    Demo_Gt_part5_Set_BrokenSound(this, play);
}

void Demo_Gt_draw_normal_part5(Actor* thisx, PlayState* play) {
    DemoGt* this = (DemoGt*)thisx;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 sp6E;
    f32 sp68;
    f32 sp64;
    s16 sp62;
    s16 sp60;
    Mtx* sp5C;
    Vec3f sp50;
    Vec3f sp44;
    f32 sp40;

    sp6E = this->unk_172;
    sp68 = fabsf(BINANG_TO_RAD(sp6E));
    sp64 = kREG(75);
    sp62 = kREG(74) + 0x7FEC;
    sp60 = kREG(74) + 0x7FEC;
    sp60 += 0x4000;
    sp5C = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    sp40 = 1.0f - cos_s(sp6E);

    OPEN_DISPS(gfxCtx, "../z_demo_gt_part5.c", 136);

    sp50.x = cos_s(sp60);
    sp50.y = 0.0f;
    sp50.z = sin_s(sp60);

    sp44.x = (cos_s(sp62) * sp64) * sp40;
    sp44.y = sin_s(sp6E) * sp64;
    sp44.z = (sin_s(sp62) * sp64) * sp40;

    Matrix_push();

    Matrix_rotateVector(sp68, &sp50, MTXMODE_APPLY);
    Matrix_translate(sp44.x, sp44.y, sp44.z, MTXMODE_APPLY);
    MATRIX_TO_MTX(sp5C, "../z_demo_gt_part5.c", 152);

    Matrix_pull();

    _texture_z_light_fog_prim(gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, sp5C, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsWalkwayDL);

    CLOSE_DISPS(gfxCtx, "../z_demo_gt_part5.c", 160);
}
