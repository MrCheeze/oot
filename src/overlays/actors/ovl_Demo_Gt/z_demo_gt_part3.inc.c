void func_80980110_Init2(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;
    func_8097EE44(this, play, 2, 3, &gTowerCollapseCsCollapsedStructureOuterCol);
}

void func_8098016C(DemoGt* this, PlayState* play) {
}

void func_80980178(DemoGt* this, PlayState* play) {
}

void func_80980184(DemoGt* this, PlayState* play) {
    s32 pad[4];
    Vec3f pos;
    Actor* actor;

    if ((play->csCtx.curFrame > 1027) && (play->csCtx.curFrame < 1031)) {
        static Actor* cloudRing = NULL;

        pos.x = this->dyna.actor.world.pos.x;
        pos.y = this->dyna.actor.world.pos.y + 247.0f;
        pos.z = this->dyna.actor.world.pos.z;

        if (cloudRing == NULL) {
            cloudRing = DemoGt_SpawnCloudRing(play, &pos, 3);
        } else {
            actor = cloudRing;
            actor->world.pos.x = pos.x;
            actor->world.pos.y = pos.y;
            actor->world.pos.z = pos.z;
        }
    }
}

void func_80980218(DemoGt* this, PlayState* play) {
    s32 pad[4];
    Vec3f pos;
    Actor* actor;

    if ((play->csCtx.curFrame > 997) && (play->csCtx.curFrame < 1001)) {
        static Actor* cloudRing = NULL;

        pos.x = this->dyna.actor.home.pos.x;
        pos.y = this->dyna.actor.home.pos.y + 38.0f;
        pos.z = this->dyna.actor.home.pos.z;

        if (cloudRing == NULL) {
            cloudRing = DemoGt_SpawnCloudRing(play, &pos, 4);
        } else {
            actor = cloudRing;
            actor->world.pos.x = pos.x;
            actor->world.pos.y = pos.y;
            actor->world.pos.z = pos.z;
        }
    }
}

void func_809802AC(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f dustPos;
    u16 csCurFrame = play->csCtx.curFrame;
    Vec3f* pos = &this->dyna.actor.world.pos;
    Vec3f velOffset = { 0.0f, 0.0f, -10.0f };
    s32 pad1[3];

    if (csCurFrame > 109 && csCurFrame < 140) {
        dustPos.x = pos->x - 100.0f;
        dustPos.y = pos->y + 1260.0f;
        dustPos.z = pos->z - 323.0f;
        func_8097D7D8(play, &dustPos, &velOffset, 4.0f, 3, 0, 20);
    }
}

void func_8098036C(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f dustPos;
    u16 csCurFrame = play->csCtx.curFrame;
    Vec3f* world = &this->dyna.actor.world.pos;
    Vec3f velOffset = { 5.0f, -3.0f, 0.0f };
    s32 pad1[3];

    if ((csCurFrame > 284) && (csCurFrame < 421)) {
        dustPos.x = world->x + 760.0f;
        dustPos.y = world->y - 40.0f;
        dustPos.z = world->z - 240.0f;
        func_8097D7D8(play, &dustPos, &velOffset, 6.0f, 6, 1, 35);
    }
}

void func_80980430(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f dustPos;
    u16 csCurFrame = play->csCtx.curFrame;
    Vec3f* pos = &this->dyna.actor.world.pos;
    Vec3f velOffset = { 5.0f, -3.0f, 0.0f };
    s32 pad1[3];

    if (csCurFrame > 709 || (DEBUG_FEATURES && (kREG(1) == 8))) {
        dustPos.x = pos->x + 760.0f;
        dustPos.y = pos->y - 40.0f;
        dustPos.z = pos->z - 240.0f;
        func_8097D7D8(play, &dustPos, &velOffset, 6.0f, 6, 1, 35);
    }
}

void func_80980504(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f dustPos;
    u16 csCurFrame = play->csCtx.curFrame;
    Vec3f* pos = &this->dyna.actor.world.pos;
    Vec3f velOffset = { 5.0f, -16.0f, -16.0f };
    s32 pad1[3];

    if ((csCurFrame > 704) || (DEBUG_FEATURES && (kREG(1) == 9))) {
        dustPos.x = pos->x + 830.0f;
        dustPos.y = pos->y + 60.0f;
        dustPos.z = pos->z + 390.0f;
        func_8097D7D8(play, &dustPos, &velOffset, 6.0f, 6, 2, 35);
    }
}

void func_809805D8(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f dustPos;
    u16 csCurFrame = play->csCtx.curFrame;
    Vec3f* homePos = &this->dyna.actor.home.pos;
    Vec3f velOffset = { 15.0f, -26.0, 0.0f };
    s32 pad1[3];

    if (((csCurFrame > 739) && (csCurFrame < 781)) || (DEBUG_FEATURES && (kREG(1) == 11))) {
        dustPos.x = homePos->x + 550.0f;
        dustPos.y = homePos->y - 110.0f;
        dustPos.z = homePos->z + 50.0f;
        func_8097D7D8(play, &dustPos, &velOffset, 6.0f, 6, 4, 35);
    }
}

void func_809806B8(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f dustPos;
    u16 csCurFrame = play->csCtx.curFrame;
    Vec3f* pos = &this->dyna.actor.world.pos;
    Vec3f velOffset = { 5.0f, -16.0f, -16.0f };
    s32 pad1[3];

    if ((csCurFrame > 964) || (DEBUG_FEATURES && (kREG(1) == 12))) {
        dustPos.x = pos->x + 460.0f;
        dustPos.y = pos->y + 60.0f;
        dustPos.z = pos->z + 760.0f;
        func_8097D7D8(play, &dustPos, &velOffset, 6.0f, 6, 3, 35);
    }
}

void func_8098078C(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f dustPos;
    u16 csCurFrame = play->csCtx.curFrame;
    Vec3f* pos = &this->dyna.actor.world.pos;
    Vec3f velOffset = { 5.0f, -16.0f, -16.0f };
    s32 pad1[3];

    if ((csCurFrame > 939) || (DEBUG_FEATURES && (kREG(1) == 14))) {
        dustPos.x = pos->x + 360.0f;
        dustPos.y = pos->y + 70.0f;
        dustPos.z = pos->z - 640.0f;
        func_8097D7D8(play, &dustPos, &velOffset, 6.0, 6, 0, 35);
    }
}

void func_8098085C(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f sp28;
    u16 csCurFrame = play->csCtx.curFrame;
    Vec3f* pos = &this->dyna.actor.world.pos;

    if ((csCurFrame == 58) || (DEBUG_FEATURES && (kREG(1) == 1))) {
        sp28.x = pos->x + 900.0f;
        sp28.y = pos->y - 50.0f;
        sp28.z = pos->z + 93.0f;
        DemoGt_SpawnExplosionWithSound(play, &sp28, 2.0f);
    } else if (csCurFrame == 80) {
        sp28.x = pos->x + 810.0f;
        sp28.y = pos->y + 200.0f;
        sp28.z = pos->z - 37.0f;
        DemoGt_SpawnExplosionWithSound(play, &sp28, 0.9f);
    } else if (csCurFrame == 90) {
        sp28.x = pos->x - 220.0f;
        sp28.y = pos->y + 1350.0f;
        sp28.z = pos->z - 287.0f;
        DemoGt_SpawnExplosionWithSound(play, &sp28, 2.0f);
    }
}

void func_809809C0(DemoGt* this, PlayState* play2) {
    PlayState* play = play2;
    DemoGt* this2 = this;
    s32 gameplayFrames = play->gameplayFrames;
    u16 csCurFrame = play->csCtx.curFrame;
    Vec3f sp54;
    s16 pad[3];

    if (((csCurFrame > 469) && (csCurFrame < 481)) || (DEBUG_FEATURES && (kREG(1) == 3))) {
        Vec3f sp40 = { 20.0f, 6.0f, 0.0f };
        Vec3f sp34 = { 0.0f, 0.0f, 0.0f };
        s16 pad2[3];

        sp54.x = this2->dyna.actor.world.pos.x + 790.0f;
        sp54.y = this2->dyna.actor.world.pos.y + 60.0f;
        sp54.z = this2->dyna.actor.world.pos.z + 23.0f;

        if (ABS(gameplayFrames % 12) == 0) {
            DemoGt_SpawnExplosionNoSound(play, &sp54, &sp40, &sp34, 2.0f);
        }
    }
}

void func_80980AD4(DemoGt* this, PlayState* play) {
    s32 pad[4];
    Vec3f pos;
    u16 csCurFrame = play->csCtx.curFrame;

    if ((csCurFrame == 477) || (DEBUG_FEATURES && (kREG(2) == 1))) {
        pos.x = this->dyna.actor.world.pos.x + 790.0f;
        pos.y = this->dyna.actor.world.pos.y + 60.0f;
        pos.z = this->dyna.actor.world.pos.z + 23.0f;

        func_8097DAC8(this, play, &pos);
        DemoGt_PlayExplosion2Sfx(play, &pos);
    }
}

void func_80980B68(DemoGt* this, PlayState* play) {
    s32 pad[4];
    Vec3f pos;
    u16 csCurFrame = play->csCtx.curFrame;

    if ((csCurFrame == 317) || (DEBUG_FEATURES && (kREG(3) == 1))) {
        pos.x = this->dyna.actor.world.pos.x + 980.0f;
        pos.y = this->dyna.actor.world.pos.y + 410.0f;
        pos.z = this->dyna.actor.world.pos.z - 177.0f;
        func_8097DD28(this, play, &pos);
        DemoGt_PlayExplosion2Sfx(play, &pos);
    }
}

void func_80980BFC(DemoGt* this, PlayState* play) {
    s32 pad[4];
    Vec3f pos;
    u16 csCurFrame = play->csCtx.curFrame;

    if ((csCurFrame == 740) || (DEBUG_FEATURES && (kREG(4) == 1))) {
        pos.x = this->dyna.actor.world.pos.x + 790.0f;
        pos.y = this->dyna.actor.world.pos.y + 60.0f;
        pos.z = this->dyna.actor.world.pos.z + 23.0f;

        func_8097DF70(this, play, &pos);
        DemoGt_PlayExplosion2Sfx(play, &pos);
    }
}

void func_80980C90(DemoGt* this, PlayState* play) {
    func_8098016C(this, play);
    func_80980178(this, play);
    func_80980184(this, play);
    func_80980218(this, play);
    func_809802AC(this, play);
    func_8098036C(this, play);
    func_80980430(this, play);
    func_80980504(this, play);
    func_809805D8(this, play);
    func_809806B8(this, play);
    func_8098078C(this, play);
    func_8098085C(this, play);
    func_809809C0(this, play);
    func_80980AD4(this, play);
    func_80980B68(this, play);
    func_80980BFC(this, play);
}

void func_80980D74(DemoGt* this, PlayState* play) {
    if (func_8097E704(play, 2, 3)) {
        this->updateMode = 10;
    }
}

void DemoGt_Update2(DemoGt* this, PlayState* play) {
    func_8097E824(this, 3);
    func_80980C90(this, play);
    func_80980D74(this, play);
}

void DemoGt_Update10(DemoGt* this, PlayState* play) {
    func_8097ED64(this, play, 3);
    func_80980C90(this, play);
}

void DemoGt_Draw3(Actor* thisx, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_demo_gt_part3.c", 1026);

    Gfx_SetupDL_25Opa(gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_FINALIZE(gfxCtx, "../z_demo_gt_part3.c", 1028),
              G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsCollapsedStructureOuterDL);
    gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx, "../z_demo_gt_part3.c", 1032);
}
