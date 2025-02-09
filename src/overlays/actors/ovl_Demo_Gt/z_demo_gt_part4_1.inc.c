void func_80980F00_Init5(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;

    func_8097EE44(this, play, 3, 4, NULL);
}

void func_80980F58(DemoGt* this, PlayState* play) {
    u16 csCurFrame = play->csCtx.curFrame;

    if (csCurFrame == 244) {
        Sfx_PlaySfxAtPos(&this->dyna.actor.projectedPos, NA_SE_EV_TOWER_PARTS_BROKEN - SFX_FLAG);
    }
}

void func_80980F8C(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f sp58;
    Vec3f dustPos;
    u16 csCurFrame = play->csCtx.curFrame;

    if ((csCurFrame > 259) && (csCurFrame < 289)) {
        Vec3f velOfset = { 0.0f, -17.0f, 0.0f };
        s32 pad[3];

        sp58.x = 640.0f;
        sp58.y = 2100.0f;
        sp58.z = -170.0f;

        Matrix_MultVec3f(&sp58, &dustPos);
        func_8097D7D8(play, &dustPos, &velOfset, 3.0f, 5, 0, 30);
    }
}

void func_8098103C(DemoGt* this, PlayState* play) {
    if (func_8097E704(play, 2, 4)) {
        this->updateMode = 11;
    } else if (func_8097E704(play, 3, 4)) {
        this->updateMode = 16;
    }
}

void DemoGt_Update3(DemoGt* this, PlayState* play) {
    func_8097E824(this, 4);
    func_80980F58(this, play);
    func_8098103C(this, play);
}

void DemoGt_Update11(DemoGt* this, PlayState* play) {
    func_8097ED64(this, play, 4);
    func_80980F58(this, play);
}

void DemoGt_Update16(DemoGt* this, PlayState* play) {
    f32 temp = this->unk_172;

    this->unk_174 = (temp * ((kREG(64) * 0.001f) + 0.048f)) + (kREG(65) + 98.0f);
    this->unk_172 += this->unk_174;

    if (this->unk_172 > 0x4000) {
        this->unk_172 = 0x4000;
    }
}

void DemoGt_Draw4(Actor* thisx, PlayState* play2) {
    GraphicsContext* gfxCtx;
    PlayState* play = play2;
    u16 csCurFrame = play->csCtx.curFrame;
    DemoGt* this = (DemoGt*)thisx;
    s16 sp76;
    f32 sp70;
    f32 sp6C;
    s16 sp6A;
    s16 sp68;
    s16 pad2;
    Mtx* sp60;
    Vec3f sp54;
    Vec3f sp48;
    f32 sp44;

    if (csCurFrame < 301) {

        sp76 = this->unk_172;
        sp70 = fabsf(BINANG_TO_RAD(sp76));
        sp6C = kREG(61);
        sp68 = (s16)((s32)kREG(58)) + 0x4000;
        sp6A = kREG(58);
        gfxCtx = play->state.gfxCtx;
        sp60 = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
        sp44 = 1.0f - Math_CosS(sp76);

        OPEN_DISPS(gfxCtx, "../z_demo_gt_part4_1.c", 217);

        sp54.x = Math_CosS(sp68);
        sp54.y = 0.0f;
        sp54.z = Math_SinS(sp68);

        sp48.x = (Math_CosS(sp6A) * sp6C) * sp44;
        sp48.y = Math_SinS(sp76) * sp6C;
        sp48.z = (Math_SinS(sp6A) * sp6C) * sp44;

        Matrix_Push();

        Matrix_RotateAxis(sp70, &sp54, MTXMODE_APPLY);
        Matrix_Translate(sp48.x, sp48.y, sp48.z, MTXMODE_APPLY);
        MATRIX_TO_MTX(sp60, "../z_demo_gt_part4_1.c", 232);

        if (!FrameAdvance_IsEnabled(play)) {
            func_80980F8C(this, play);
        }

        Matrix_Pop();

        Gfx_SetupDL_25Opa(gfxCtx);
        gSPMatrix(POLY_OPA_DISP++, sp60, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsStandalonePillarDL);
        gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

        CLOSE_DISPS(gfxCtx, "../z_demo_gt_part4_1.c", 246);
    }
}
