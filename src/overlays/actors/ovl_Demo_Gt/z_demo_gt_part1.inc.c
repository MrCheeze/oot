void Demo_Gt_Actor_init_part1(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;

    Demo_Gt_Actor_init_part_common(this, play, 0, 1, NULL);
}

void Demo_Gt_part1_Set_SinkSound(DemoGt* this, PlayState* play) {
    u16 csCurFrame = play->csCtx.curFrame;

    if (csCurFrame == 527) {
        Na_StartDemoSe(SEQ_CS_EFFECTS_TOWER_COLLAPSE);
    }
}

void Demo_Gt_SetDust1_part1(DemoGt* this, PlayState* play) {
}

void Demo_Gt_SetDust3_part1(DemoGt* this, PlayState* play) {
    u16 csCurFrame = play->csCtx.curFrame;
    s32 pad1[3];
    Vec3f dustPos;
    Vec3f velocity = { 0.0f, -16.0f, 0.0f };
    Vec3f accel = { 0.0f, 1.2f, 0.0f };
    Vec3f* pos = &this->dyna.actor.world.pos;
    s32 pad;

    if ((DEBUG_FEATURES && (kREG(1) == 20)) || (csCurFrame == 220)) {
        dustPos.x = pos->x + 256.0f;
        dustPos.y = pos->y + 679.0f;
        dustPos.z = pos->z + 82.0f;

        Birth_SingleDust_In_Demo_Gt(play, &dustPos, &velocity, &accel, 1700.0f, 15, 30);

        dustPos.x = pos->x + 256.0f;
        dustPos.y = pos->y + 679.0f;
        dustPos.z = pos->z - 60.0f;

        Birth_SingleDust_In_Demo_Gt(play, &dustPos, &velocity, &accel, 1700.0f, 15, 30);
    }
}

void Demo_Gt_SetBomb1_part1(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f sp38;
    s16 pad1[3];
    Vec3f sp24;
    u16 csCurFrame = play->csCtx.curFrame;
    s32 pad2;

#if DEBUG_FEATURES
    if (csCurFrame == 140 || kREG(1) == 19)
#else
    if (csCurFrame == 140)
#endif
    {
        sp38.x = this->dyna.actor.world.pos.x + 260.0f;
        sp38.y = this->dyna.actor.world.pos.y + 340.0f;
        sp38.z = this->dyna.actor.world.pos.z + 45.0f;
        Birth_SingleBomb_In_Demo_Gt(play, &sp38, 2.0f);
    }

    if (csCurFrame == 176) {
        sp24.x = this->dyna.actor.world.pos.x + 260.0f;
        sp24.y = this->dyna.actor.world.pos.y + 840.0f;
        sp24.z = this->dyna.actor.world.pos.z + 45.0f;
        Birth_SingleBomb_In_Demo_Gt(play, &sp24, 2.0f);
    }
}

void Demo_Gt_SetDust_part1(DemoGt* this, PlayState* play) {
    Demo_Gt_SetDust1_part1(this, play);
    Demo_Gt_SetDust3_part1(this, play);
    Demo_Gt_SetBomb1_part1(this, play);
}

void Demo_Gt_SetAngle_forLay_part1(DemoGt* this) {
    f32 temp_v0 = this->unk_172;

    this->unk_174 = (temp_v0 * ((kREG(64) * 0.001f) + 0.048f)) + (kREG(72) + 10.0f);
    this->unk_172 += this->unk_174;

    if (this->unk_172 > (s16)(kREG(73) + 0x250)) {
        this->unk_172 = kREG(73) + 0x250;
    }
}

void Demo_Gt_CalcScroll_part1(DemoGt* this, PlayState* play) {
    s32* unk178 = this->unk_178;
    s32* unk188 = this->unk_188;
    s32* unk198 = this->unk_198;

    if (play->csCtx.curFrame < 160) {
        unk178[0] = 100;
        unk178[1] = 255;
        unk178[2] = 200;

        unk188[0] = 255;
        unk188[1] = 120;
        unk188[2] = 100;

        unk198[0]++;
        unk198[1]--;
    } else if (play->csCtx.curFrame < 170) {
        f32 temp_f0 = get_parcent_forAccelBrake(170, 160, play->csCtx.curFrame, 0, 0);

        unk178[0] = (temp_f0 * -63.0f) + 163.0f;
        unk178[1] = (temp_f0 * -155.0f) + 255.0f;
        unk178[2] = temp_f0 * -100.0f + 200.0f;

        unk188[0] = (temp_f0 * -155.0f) + 255.0f;
        unk188[1] = (temp_f0 * -20.0f) + 120.0f;
        unk188[2] = 100;
    } else {
        unk178[0] = 100;
        unk178[1] = 100;
        unk178[2] = 100;

        unk188[0] = 100;
        unk188[1] = 100;
        unk188[2] = 100;
    }
}

void Demo_Gt_check_StandToFall_part1(DemoGt* this, PlayState* play) {
    if (Demo_Gt_Check_npcdemopnt(play, 2, 1)) {
        this->updateMode = 8;
    }
}

void Demo_Gt_main_Stand_part1(DemoGt* this, PlayState* play) {
    Demo_Gt_CalcScroll_part1(this, play);
    Demo_Gt_SetPos_fromOffset(this, 1);
    Demo_Gt_SetDust_part1(this, play);
    Demo_Gt_check_StandToFall_part1(this, play);
    Demo_Gt_Set_EarthQuakeSound();
    Demo_Gt_Set_Viblation(play);
    Demo_Gt_part1_Set_SinkSound(this, play);
}

void Demo_Gt_main_Fall_part1(DemoGt* this, PlayState* play) {
    Demo_Gt_CalcScroll_part1(this, play);
    Demo_Gt_SetAngle_forLay_part1(this);
    Demo_Gt_SetPos_forFall(this, play, 1);
    Demo_Gt_SetDust_part1(this, play);
    Demo_Gt_Set_EarthQuakeSound();
    Demo_Gt_Set_Viblation(play);
    Demo_Gt_part1_Set_SinkSound(this, play);
}

void Demo_Gt_draw_normal_part1(Actor* thisx, PlayState* play) {
    s32 pad;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    u32 gameplayFrames = play->gameplayFrames;
    DemoGt* this = (DemoGt*)thisx;
    s16 spC6;
    f32 spC0;
    f32 spBC;
    s16 spBA;
    s16 spB8;
    Mtx* spB4;
    Vec3f spA8;
    Vec3f sp9C;
    f32 sp98;
    s32* unk198;
    s32* unk188;
    s32* unk178;

    spC6 = this->unk_172;
    spC0 = fabsf(BINANG_TO_RAD(spC6));
    spBC = kREG(71);
    spB8 = (s16)((s32)kREG(70)) + 0x4000;
    spBA = kREG(70);
    spB4 = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    sp98 = 1.0f - cos_s(spC6);

    OPEN_DISPS(gfxCtx, "../z_demo_gt_part1.c", 458);

    spA8.x = cos_s(spB8);
    spA8.y = 0.0f;
    spA8.z = sin_s(spB8);
    sp9C.x = cos_s(spBA) * spBC * sp98;
    sp9C.y = sin_s(spC6) * spBC;
    sp9C.z = sin_s(spBA) * spBC * sp98;

    Matrix_push();

    Matrix_rotateVector(spC0, &spA8, MTXMODE_APPLY);
    Matrix_translate(sp9C.x, sp9C.y, sp9C.z, MTXMODE_APPLY);
    MATRIX_TO_MTX(spB4, "../z_demo_gt_part1.c", 474);
    unk198 = this->unk_198;
    unk188 = this->unk_188;
    unk178 = this->unk_178;

    Matrix_pull();

    _texture_z_light_fog_prim(gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08,
               two_tex_scroll_env(gfxCtx, 0, 0, unk198[0], 0x20, 0x40, 1, 0, unk198[1], 0x20, 0x40, unk178[0],
                                        unk178[1], unk178[2], 0x80));
    gSPSegment(POLY_OPA_DISP++, 0x0A,
               two_tex_scroll_env(gfxCtx, 0, 0, unk198[0], 0x20, 0x40, 1, 0, unk198[1], 0x20, 0x40, unk188[0],
                                        unk188[1], unk188[2], 0x80));
    gSPMatrix(POLY_OPA_DISP++, spB4, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsExteriorStructureDL);
    _texture_z_light_fog_prim_xlu(gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
    gSPSegment(
        POLY_XLU_DISP++, 0x09,
        two_tex_scroll(gfxCtx, 0, 0, gameplayFrames * 0x14, 0x10, 0x200, 1, 0, gameplayFrames * 0x1E, 0x10, 0x200));
    gSPMatrix(POLY_XLU_DISP++, spB4, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gTowerCollapseCsFlameSmokeDL);

    CLOSE_DISPS(gfxCtx, "../z_demo_gt_part1.c", 557);
}
