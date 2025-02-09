void func_809818A4_Init7(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;

    func_8097EE44(this, play, 5, 6, NULL);
}

void func_809818FC(DemoGt* this, PlayState* play) {
    u16 csCurFrame = play->csCtx.curFrame;

    if (csCurFrame == 845) {
        Sfx_PlaySfxAtPos(&this->dyna.actor.projectedPos, NA_SE_EV_TOWER_PARTS_BROKEN - SFX_FLAG);
    }
}
void func_80981930(DemoGt* this, PlayState* play) {
    if (func_8097E704(play, 2, 6)) {
        this->updateMode = 13;
    } else if (func_8097E704(play, 3, 6)) {
        this->updateMode = 18;
    }
}
void DemoGt_Update5(DemoGt* this, PlayState* play) {
    func_8097E824(this, 6);
    func_809818FC(this, play);
    func_80981930(this, play);
}

void DemoGt_Update13(DemoGt* this, PlayState* play) {
    func_8097ED64(this, play, 6);
    func_809818FC(this, play);
}

void DemoGt_Update18(DemoGt* this, PlayState* play) {
    f32 temp = this->unk_172;

    this->unk_174 = (temp * ((kREG(68) * 0.001f) + 0.005f)) + (kREG(69) + 50.0f);
    this->unk_172 += this->unk_174;

    if (this->unk_172 > 0x4000) {
        this->unk_172 = 0x4000;
    }
}

void DemoGt_Draw6(Actor* thisx, PlayState* play) {
    DemoGt* this = (DemoGt*)thisx;
    s16 sp78 = this->unk_172;
    f32 sp74;
    f32 sp70;
    s16 sp6E;
    s16 sp6C;
    GraphicsContext* gfxCtx;
    Mtx* sp64;
    Vec3f sp58;
    Vec3f sp4C;
    f32 sp48;

    sp74 = fabsf(BINANG_TO_RAD(sp78));
    sp70 = kREG(63);
    sp6E = kREG(60) + 0x4000;
    sp6C = kREG(60) + 0x4000;
    sp6C += 0x4000;
    gfxCtx = play->state.gfxCtx;
    sp64 = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    sp48 = 1.0f - Math_CosS(sp78);

    OPEN_DISPS(gfxCtx, "../z_demo_gt_part4_3.c", 276);

    sp58.x = Math_CosS(sp6C);
    sp58.y = 0.0f;
    sp58.z = Math_SinS(sp6C);

    sp4C.x = Math_CosS(sp6E) * sp70 * sp48;
    sp4C.y = Math_SinS(sp78) * sp70;
    sp4C.z = Math_SinS(sp6E) * sp70 * sp48;

    Matrix_Push();

    Matrix_RotateAxis(sp74, &sp58, MTXMODE_APPLY);
    Matrix_Translate(sp4C.x, sp4C.y, sp4C.z, MTXMODE_APPLY);
    MATRIX_TO_MTX(sp64, "../z_demo_gt_part4_3.c", 291);

    Matrix_Pop();

    Gfx_SetupDL_25Opa(gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, sp64, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsStandalonePillarDL);
    gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx, "../z_demo_gt_part4_3.c", 307);
}
