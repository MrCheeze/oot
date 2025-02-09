void Demo_Gt_Actor_init_part4_3(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;

    Demo_Gt_Actor_init_part_common(this, play, 5, 6, NULL);
}

void Demo_Gt_part4_3_Set_BrokenSound(DemoGt* this, PlayState* play) {
    u16 csCurFrame = play->csCtx.curFrame;

    if (csCurFrame == 845) {
        Na_StartObjectSe_F(&this->dyna.actor.projectedPos, NA_SE_EV_TOWER_PARTS_BROKEN - SFX_FLAG);
    }
}
void Demo_Gt_check_StandToBranch_part4_3(DemoGt* this, PlayState* play) {
    if (Demo_Gt_Check_npcdemopnt(play, 2, 6)) {
        this->updateMode = 13;
    } else if (Demo_Gt_Check_npcdemopnt(play, 3, 6)) {
        this->updateMode = 18;
    }
}
void Demo_Gt_main_Stand_part4_3(DemoGt* this, PlayState* play) {
    Demo_Gt_SetPos_fromOffset(this, 6);
    Demo_Gt_part4_3_Set_BrokenSound(this, play);
    Demo_Gt_check_StandToBranch_part4_3(this, play);
}

void Demo_Gt_main_Fall_part4_3(DemoGt* this, PlayState* play) {
    Demo_Gt_SetPos_forFall(this, play, 6);
    Demo_Gt_part4_3_Set_BrokenSound(this, play);
}

void Demo_Gt_main_Lay_part4_3(DemoGt* this, PlayState* play) {
    f32 temp = this->unk_172;

    this->unk_174 = (temp * ((kREG(68) * 0.001f) + 0.005f)) + (kREG(69) + 50.0f);
    this->unk_172 += this->unk_174;

    if (this->unk_172 > 0x4000) {
        this->unk_172 = 0x4000;
    }
}

void Demo_Gt_draw_normal_part4_3(Actor* thisx, PlayState* play) {
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
    sp48 = 1.0f - cos_s(sp78);

    OPEN_DISPS(gfxCtx, "../z_demo_gt_part4_3.c", 276);

    sp58.x = cos_s(sp6C);
    sp58.y = 0.0f;
    sp58.z = sin_s(sp6C);

    sp4C.x = cos_s(sp6E) * sp70 * sp48;
    sp4C.y = sin_s(sp78) * sp70;
    sp4C.z = sin_s(sp6E) * sp70 * sp48;

    Matrix_push();

    Matrix_rotateVector(sp74, &sp58, MTXMODE_APPLY);
    Matrix_translate(sp4C.x, sp4C.y, sp4C.z, MTXMODE_APPLY);
    MATRIX_TO_MTX(sp64, "../z_demo_gt_part4_3.c", 291);

    Matrix_pull();

    _texture_z_light_fog_prim(gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, sp64, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsStandalonePillarDL);
    gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx, "../z_demo_gt_part4_3.c", 307);
}
