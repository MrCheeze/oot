void func_809813CC_Init6(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;

    func_8097EE44(this, play, 4, 5, NULL);
}

void func_80981424(DemoGt* this, PlayState* play) {
    u16 csCurFrame = play->csCtx.curFrame;

    if (csCurFrame == 789) {
        Sfx_PlaySfxAtPos(&this->dyna.actor.projectedPos, NA_SE_EV_TOWER_PARTS_BROKEN - SFX_FLAG);
    }
}

void func_80981458(DemoGt* this, PlayState* play) {
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

        Matrix_MultVec3f(&sp58, &dustPos);
        func_8097D7D8(play, &dustPos, &velOffset, 7.1f, 5, 1, 30);
    }
}

void func_80981524(DemoGt* this, PlayState* play) {
    if (func_8097E704(play, 2, 5)) {
        this->updateMode = 12;
    } else if (func_8097E704(play, 3, 5)) {
        this->updateMode = 17;
    }
}

void DemoGt_Update4(DemoGt* this, PlayState* play) {
    func_8097E824(this, 5);
    func_80981424(this, play);
    func_80981524(this, play);
}

void DemoGt_Update12(DemoGt* this, PlayState* play) {
    func_8097ED64(this, play, 5);
    func_80981424(this, play);
}

void DemoGt_Update17(DemoGt* this, PlayState* play) {
    f32 temp = this->unk_172;

    this->unk_174 = temp * ((kREG(66) * 0.001f) + 0.048f) + (kREG(67) + 50.0f);
    this->unk_172 += this->unk_174;

    if (this->unk_172 > 0x4000) {
        this->unk_172 = 0x4000;
    }
}

void DemoGt_Draw5(Actor* thisx, PlayState* play) {
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
    sp44 = 1 - Math_CosS(sp76);

    OPEN_DISPS(gfxCtx, "../z_demo_gt_part4_2.c", 212);

    sp54.x = Math_CosS(sp68);
    sp54.y = 0.0f;
    sp54.z = Math_SinS(sp68);

    sp48.x = Math_CosS(sp6A) * sp6C * sp44;
    sp48.y = Math_SinS(sp76) * sp6C;
    sp48.z = Math_SinS(sp6A) * sp6C * sp44;

    Matrix_Push();

    Matrix_RotateAxis(sp70, &sp54, MTXMODE_APPLY);
    Matrix_Translate(sp48.x, sp48.y, sp48.z, MTXMODE_APPLY);
    MATRIX_TO_MTX(sp60, "../z_demo_gt_part4_2.c", 227);

    if (!FrameAdvance_IsEnabled(play)) {
        func_80981458(this, play);
    }

    Matrix_Pop();

    Gfx_SetupDL_25Opa(gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, sp60, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsStandalonePillarDL);
    gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx, "../z_demo_gt_part4_2.c", 241);
}
