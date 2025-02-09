void func_80982054_Init24(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;
    func_8097EE44(this, play, 7, 8, NULL);
}

void func_809820AC(DemoGt* this, PlayState* play) {
    u16 csCurFrame = play->csCtx.curFrame;

    if (csCurFrame == 154) {
        Sfx_PlaySfxAtPos(&this->dyna.actor.projectedPos, NA_SE_EV_TOWER_PARTS_BROKEN - SFX_FLAG);
    }
}

void func_809820E0(DemoGt* this) {
    f32 temp = this->unk_172;

    this->unk_174 = (temp * ((kREG(64) * 0.001f) + 0.048f)) + (kREG(79) + 100.0f);
    this->unk_172 += this->unk_174;

    if (this->unk_172 > (s16)(kREG(81) + 0x4000)) {
        this->unk_172 = kREG(81) + 0x4000;
    }
}

void func_80982188(DemoGt* this, PlayState* play) {
    if (func_8097E704(play, 2, 9) != 0) {
        this->updateMode = 15;
    }
}

void DemoGt_Update7(DemoGt* this, PlayState* play) {
    func_8097E824(this, 9);
    func_809820AC(this, play);
    func_80982188(this, play);
}

void DemoGt_Update15(DemoGt* this, PlayState* play) {
    func_809820E0(this);
    func_8097ED64(this, play, 9);
    func_809820AC(this, play);
}

void DemoGt_Draw8(Actor* thisx, PlayState* play) {
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
    sp64 = kREG(78);
    sp62 = kREG(77) + 0xBE80;
    sp60 = kREG(77) + 0xBE80;
    sp60 += 0x4000;
    sp5C = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    sp40 = 1.0f - Math_CosS(sp6E);

    OPEN_DISPS(gfxCtx, "../z_demo_gt_part6.c", 137);

    sp50.x = Math_CosS(sp60);
    sp50.y = 0.0f;
    sp50.z = Math_SinS(sp60);

    sp44.x = Math_CosS(sp62) * sp64 * sp40;
    sp44.y = Math_SinS(sp6E) * sp64;
    sp44.z = Math_SinS(sp62) * sp64 * sp40;

    Matrix_Push();

    Matrix_RotateAxis(sp68, &sp50, MTXMODE_APPLY);
    Matrix_Translate(sp44.x, sp44.y, sp44.z, MTXMODE_APPLY);
    MATRIX_TO_MTX(sp5C, "../z_demo_gt_part6.c", 153);

    Matrix_Pop();

    Gfx_SetupDL_25Opa(gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, sp5C, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsAlternativeWalkwayDL);

    CLOSE_DISPS(gfxCtx, "../z_demo_gt_part6.c", 163);
}
