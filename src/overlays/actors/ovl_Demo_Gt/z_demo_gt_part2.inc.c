void func_8097F904_Init1(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;
    func_8097EE44(this, play, 1, 2, &gTowerCollapseCsCollapsedStructureInnerCol);
}

void func_8097F960(DemoGt* this, PlayState* play) {
}

void func_8097F96C(DemoGt* this, PlayState* play) {
    s32 pad[4];
    Vec3f pos;
    Actor* actor;
    u16 csCurFrame = play->csCtx.curFrame;

    if (((csCurFrame > 1059) && (csCurFrame < 1062)) || (DEBUG_FEATURES && (kREG(1) == 17))) {
        static Actor* cloudRing = NULL;

        pos.x = this->dyna.actor.world.pos.x;
        pos.y = this->dyna.actor.world.pos.y + 612.0f;
        pos.z = this->dyna.actor.world.pos.z;

        if (cloudRing == NULL) {
            cloudRing = DemoGt_SpawnCloudRing(play, &pos, 2);
        } else {
            actor = cloudRing;
            actor->world.pos.x = pos.x;
            actor->world.pos.y = pos.y;
            actor->world.pos.z = pos.z;
        }
    }
}

void func_8097FA1C(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f dustPos;
    u16 csCurFrame = play->csCtx.curFrame;
    Vec3f* pos = &this->dyna.actor.world.pos;
    Vec3f velOffset = { -12.0f, -17.0, 5.0 };
    s32 pad1[3];

    if (((csCurFrame > 502) && !(csCurFrame >= 581)) || (DEBUG_FEATURES && (kREG(1) == 5))) {
        dustPos.x = pos->x + 300.0f;
        dustPos.y = pos->y + 360.0f;
        dustPos.z = pos->z - 377.0f;
        func_8097D7D8(play, &dustPos, &velOffset, 6.0f, 6, 1, 35);
    }
}

void func_8097FAFC(DemoGt* this, PlayState* play) {
    s32 pad[2];
    u16 csCurFrame = play->csCtx.curFrame;
    Vec3f pos;
    f32 new_var = -200.0;

    if (((csCurFrame > 582) && (csCurFrame < 683)) || (DEBUG_FEATURES && (kREG(1) == 6))) {
        static Vec3f velocity = { 0.0f, 1.0f, 0.0f };
        static Vec3f accel = { 0.0f, 0.0f, 0.0f };
        static f32 arg4 = 280.0f;
        static f32 scale = 8.0f;
        static s32 arg6 = 11;
        static s32 arg7 = 1;
        static s16 life = 3;

        pos = this->dyna.actor.world.pos;
        pos.y += 680.0f;

        if (csCurFrame == 682) {
            velocity.y += new_var;
        } else if (csCurFrame == 681) {
            accel.y += new_var;
        }

        func_8097E454(play, &pos, &velocity, &accel, arg4, scale, arg6, arg7, life);
    }
}

void func_8097FC1C(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f dustPos;
    u16 csCurFrame = play->csCtx.curFrame;
    Vec3f* pos = &this->dyna.actor.world.pos;
    Vec3f velOffset = { 5.0f, -16.0f, -16.0f };
    s32 pad1[3];

    if (csCurFrame > 682 || (DEBUG_FEATURES && (kREG(1) == 7))) {
        dustPos.x = pos->x + 260.0f;
        dustPos.y = pos->y + 360.0f;
        dustPos.z = pos->z + 260.0f;
        func_8097D7D8(play, &dustPos, &velOffset, 6.0f, 6, 0, 35);
    }
}

void func_8097FCE4(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f vec;
    u16 csCurFrame = play->csCtx.curFrame;

#if DEBUG_FEATURES
    if (csCurFrame == 503 || kREG(1) == 4)
#else
    if (csCurFrame == 503)
#endif
    {
        vec.x = this->dyna.actor.world.pos.x + 300.0f;
        vec.y = this->dyna.actor.world.pos.y + 560.0f;
        vec.z = this->dyna.actor.world.pos.z - 377.0f;
        DemoGt_SpawnExplosionWithSound(play, &vec, 2.0f);
    }
}

void func_8097FD70(DemoGt* this, PlayState* play) {
    func_8097F960(this, play);
    func_8097F96C(this, play);
    func_8097FA1C(this, play);
    func_8097FAFC(this, play);
    func_8097FC1C(this, play);
    func_8097FCE4(this, play);
}

void func_8097FDDC(DemoGt* this, PlayState* play) {
    s32* unk178 = this->unk_178;
    s32* unk198 = this->unk_198;

    if (play->csCtx.curFrame < 610) {
        unk178[0] = 163;
        unk178[1] = 193;
        unk178[2] = 193;
        unk198[0]++;
        unk198[1]--;
    } else if (play->csCtx.curFrame < 620) {
        f32 temp_f0 = Environment_LerpWeightAccelDecel(620, 610, play->csCtx.curFrame, 0, 0);

        unk178[0] = (temp_f0 * (-13.0f)) + 163.0f;
        unk178[1] = (temp_f0 * (-43.0f)) + 193.0f;
        unk178[2] = (temp_f0 * (-43.0f)) + 193.0f;
    } else {
        unk178[0] = 150;
        unk178[1] = 150;
        unk178[2] = 150;
    }
}

void func_8097FED8(DemoGt* this, PlayState* play) {
    if (func_8097E704(play, 2, 2)) {
        this->updateMode = 9;
    }
}

void DemoGt_Update1(DemoGt* this, PlayState* play) {
    func_8097FDDC(this, play);
    func_8097E824(this, 2);
    func_8097FD70(this, play);
    func_8097FED8(this, play);
}

void DemoGt_Update9(DemoGt* this, PlayState* play) {
    func_8097FDDC(this, play);
    func_8097ED64(this, play, 2);
    func_8097FD70(this, play);
}

void DemoGt_Draw2(Actor* thisx, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32* unk198;
    s32* unk178;
    DemoGt* this = (DemoGt*)thisx;

    OPEN_DISPS(gfxCtx, "../z_demo_gt_part2.c", 470);

    Gfx_SetupDL_25Opa(gfxCtx);
    unk198 = this->unk_198;
    unk178 = this->unk_178;
    if (1) {}
    gSPSegment(POLY_OPA_DISP++, 0x08,
               Gfx_TwoTexScrollEnvColor(gfxCtx, 0, 0, unk198[0], 0x20, 0x40, 1, 0, unk198[1], 0x20, 0x40, unk178[0],
                                        unk178[1], unk178[2], 128));
    gSPMatrix(POLY_OPA_DISP++, MATRIX_FINALIZE(gfxCtx, "../z_demo_gt_part2.c", 485),
              G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsCollapsedStructureInnerDL);
    gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx, "../z_demo_gt_part2.c", 489);
}
