void func_80B56DA4(EnZl3* this) {
    Sfx_PlaySfxCentered2(NA_SE_EV_ZELDA_POWER);
}

void func_80B56DC8(EnZl3* this) {
    Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_VO_Z1_PAIN);
}

void func_80B56DEC(EnZl3* this) {
    SkelAnime* skelAnime = &this->skelAnime;

    if ((skelAnime->mode == 2) && Animation_OnFrame(skelAnime, 9.0f) != 0) {
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, NA_SE_VO_Z1_OPENDOOR);
    }
}

void func_80B56E38(EnZl3* this, PlayState* play) {
    s32 pad[2];
    s32 sfxId;
    SkelAnime* sp20 = &this->skelAnime;

    if ((Animation_OnFrame(sp20, 6.0f) || Animation_OnFrame(sp20, 0.0f)) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        sfxId = NA_SE_PL_WALK_GROUND;
        sfxId += SurfaceType_GetSfxOffset(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
        Sfx_PlaySfxAtPos(&this->actor.projectedPos, sfxId);
    }
}

void func_80B56EB8(EnZl3* this, PlayState* play) {
    Flags_SetSwitch(play, func_80B54DB4(this));
}

s32 func_80B56EE4(EnZl3* this, PlayState* play) {
    return Flags_GetSwitch(play, func_80B54DB4(this));
}

void func_80B56F10(EnZl3* this, PlayState* play) {
    s32 waypoint;
    Path* pathHead = play->pathList;

    if (pathHead != NULL) {
        waypoint = func_80B54DC4(this);
        pathHead += waypoint;
        this->unk_30C = pathHead;
        this->unk_310 = pathHead->count;
        PRINTF("En_Zl3_Get_path_info レールデータをゲットだぜ = %d!!!!!!!!!!!!!!\n", waypoint);
    } else {
        PRINTF("En_Zl3_Get_path_info レールデータが無い!!!!!!!!!!!!!!!!!!!!\n");
    }
}

s32 func_80B56F8C(EnZl3* this, s32 arg1) {
    s32 unk_310 = this->unk_310;

    if (unk_310 > arg1) {
        return 1;
    }
    return 0;
}

Vec3s* func_80B56FAC(EnZl3* this, s32 arg1) {
    Vec3s* point;
    Path* pathList = this->unk_30C;

    if ((pathList != NULL) && func_80B56F8C(this, arg1)) {
        point = &((Vec3s*)SEGMENTED_TO_VIRTUAL(pathList->points))[arg1];
        return point;
    }
    return NULL;
}

s32 func_80B57034(EnZl3* this, s32 arg1, s32 arg2) {
    Vec3s* vec1 = func_80B56FAC(this, arg1);
    Vec3s* vec2 = func_80B56FAC(this, arg2);

    if ((vec2 != NULL) && (vec1 != NULL)) {
        f32 xDiff = vec2->x - vec1->x;
        f32 zDiff = vec2->z - vec1->z;

        return ((xDiff == 0.0f) && (zDiff == 0.0f)) ? 0 : RAD_TO_BINANG(Math_FAtan2F(xDiff, zDiff));
    }
    return 0;
}

s16 func_80B57104(EnZl3* this, s32 arg1) {
    Vec3s* point = func_80B56FAC(this, arg1);

    if (point != NULL) {
        f32 xDiff = point->x - this->actor.world.pos.x;
        f32 zDiff = point->z - this->actor.world.pos.z;

        if ((xDiff != 0.0f) || (zDiff != 0.0f)) {
            return RAD_TO_BINANG(Math_FAtan2F(xDiff, zDiff));
        }
    }
    return 0;
}

s32 func_80B571A8(EnZl3* this) {
    s32 pad;
    s32 unk_314 = this->unk_314;
    s32 pad2;

    if (func_80B56F8C(this, unk_314 + 1) == 0) {
        return this->actor.shape.rot.y;
    } else {
        return func_80B57034(this, unk_314, unk_314 + 1);
    }
}

s32 func_80B571FC(EnZl3* this) {
    s32 pad;
    s32 unk_314 = this->unk_314;

    if (func_80B56F8C(this, unk_314) == 0) {
        return this->actor.shape.rot.y;
    } else {
        return func_80B57104(this, unk_314);
    }
}

void func_80B57240(EnZl3* this) {
    s32 temp_a1 = func_80B571FC(this);
    s16* rotY = &this->actor.world.rot.y;

    Math_SmoothStepToS(rotY, temp_a1, 2, 6400, 1000);
    this->actor.shape.rot.y = *rotY;
}

void func_80B57298(EnZl3* this) {
    s16* rotY = &this->actor.world.rot.y;
    s16 temp_a1 = func_80B571A8(this);

    Math_SmoothStepToS(rotY, temp_a1, 2, 6400, 1000);
    this->actor.shape.rot.y = *rotY;
}

u16 func_80B572F0(PlayState* play) {
    s16 sceneId = play->sceneId;
    u16 ret;

    if (sceneId == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) {
        ret = 0x71A8;
    } else if (sceneId == SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) {
        ret = 0x71A9;
    } else {
        ret = 0x71AB;
    }
    return ret;
}

s32 func_80B57324(EnZl3* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, play)) {
        return 1;
    }
    return 0;
}

void func_80B57350(EnZl3* this, PlayState* play) {
    s16 temp_v0 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (ABS(temp_v0) <= 0x4300) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        this->actor.textId = func_80B572F0(play);
        Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
    }
}

s32 func_80B573C8(EnZl3* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        return 1;
    }
    return 0;
}

s32 func_80B573FC(EnZl3* this, PlayState* play, f32 arg2) {
    Player* player = GET_PLAYER(play);
    f32 playerX = player->actor.world.pos.x;
    f32 playerZ = player->actor.world.pos.z;
    f32 thisX = this->actor.world.pos.x;
    f32 thisZ = this->actor.world.pos.z;

    if (SQ(playerX - thisX) + SQ(playerZ - thisZ) < SQ(arg2)) {
        return 1;
    }
    return 0;
}

s32 func_80B57458(EnZl3* this, PlayState* play) {
    Vec3f* thisPos = &this->actor.world.pos;
    f32 thisX = thisPos->x;
    f32 thisZ = thisPos->z;
    Player* player = GET_PLAYER(play);
    Vec3f* playerPos = &player->actor.world.pos;
    s32 pad;
    f32 playerX = playerPos->x;
    f32 playerZ = playerPos->z;
    f32 temp_f12 = playerX - thisX;
    f32 temp_f13 = playerZ - thisZ;
    s16 temp_v0;
    s16 temp_v1 = func_80B571A8(this);

    if (temp_f12 == 0.0f && temp_f13 == 0.0f) {
        return 1;
    }

    temp_v0 = (s16)(temp_v1 - RAD_TO_BINANG(Math_FAtan2F(temp_f12, temp_f13)));

    if (temp_v0 < 0x1555) {
        return 1;
    } else if ((temp_v0 < 0x4000) && func_80B573FC(this, play, 150.0f)) {
        return 1;
    } else {
        return 0;
    }
}

s32 func_80B57564(EnZl3* this, PlayState* play) {
    if (func_80B573FC(this, play, 50.0f) || func_80B57458(this, play)) {
        return 1;
    }
    return 0;
}

s32 func_80B575B0(EnZl3* this, PlayState* play) {
    return func_80B573FC(this, play, 150.0f);
}

s32 func_80B575D0(EnZl3* this, PlayState* play) {
    return func_80B573FC(this, play, 50.0f);
}

s32 func_80B575F0(EnZl3* this, PlayState* play) {
    s16 sceneId = play->sceneId;

    if ((sceneId == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) && (func_80B54DB4(this) == 0x26)) {
        s32 unk_314 = this->unk_314;

        if (unk_314 == 1) {
            return 1;
        }
    }
    return 0;
}

void func_80B5764C(EnZl3* this, PlayState* play) {
    s16 sceneId = play->sceneId;

    if ((sceneId == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) && (func_80B54DB4(this) == 0x26)) {
        s32 unk_314 = this->unk_314 + 1;

        if ((unk_314 == 1) && !Play_InCsMode(play)) {
            OnePointCutscene_Init(play, 1000, 40, &this->actor, CAM_ID_MAIN);
        }
    }
}

s32 func_80B576C8(EnZl3* this, PlayState* play) {
    if (func_80B575F0(this, play)) {
        s32 pad;

        if (this->unk_3D8 == 0) {
            return 1;
        }
    }
    return 0;
}

void func_80B57704(EnZl3* this, PlayState* play) {
    s32 unk_3C4 = this->unk_3C4;

    Flags_SetSwitch(play, unk_3C4);
}

void func_80B5772C(EnZl3* this, PlayState* play) {
    s32 unk_3C4 = this->unk_3C4;

    Flags_UnsetSwitch(play, unk_3C4);
}

void func_80B57754(EnZl3* this, PlayState* play) {
    if (gSaveContext.magicState == MAGIC_STATE_IDLE) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_OCEFF_WIPE4, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, 0, 0, 1);
        func_80B56DA4(this);
    }
}

void func_80B577BC(PlayState* play, Vec3f* vec) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    Vec3f* playerPos = &player->actor.world.pos;
    f32 posX = vec->x;
    f32 posY = vec->y;
    f32 posZ = vec->z;

    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_TEST, posX, posY, posZ, 0,
                RAD_TO_BINANG(Math_FAtan2F(playerPos->x - posX, playerPos->z - posZ)), 0, 5);
}

static Vec3f D_80B5A498 = { 148.0f, 260.0f, -87.0f };
static Vec3f D_80B5A4A4 = { -12.0f, 260.0f, -147.0f };
static Vec3f D_80B5A4B0 = { 42.0f, 260.0f, 13.0f };

void func_80B57858(PlayState* play) {
    func_80B577BC(play, &D_80B5A498);
    func_80B577BC(play, &D_80B5A4A4);
}

s32 func_80B57890(EnZl3* this, PlayState* play) {
    s8 pad[2];
    u8 spawn = play->spawn;
    s16 sceneId = play->sceneId;
    s32 result = func_80B54DB4(this);

    if (play) {} // Needed to match, this if can be almost anywhere and it still matches

    if (sceneId == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) {
        if ((result == 0x24) && (spawn == 0)) {
            return 1;
        }
        if ((result == 0x25) && (spawn == 2)) {
            return 1;
        }
        if ((result == 0x26) && (spawn == 4)) {
            return 1;
        }
        if ((result == 0x27) && (spawn == 6)) {
            return 1;
        }
        if ((result == 0x28) && (spawn == 6)) {
            return 1;
        }
    } else if (sceneId == SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) {
        if ((result == 0x20) && (spawn == 0) && Flags_GetSwitch(play, 0x37)) {
            if ((play->sceneId == SCENE_GANON_BOSS) || (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) ||
                (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) ||
                (play->sceneId == SCENE_INSIDE_GANONS_CASTLE_COLLAPSE)) {
                return 1;
            }
        }
        if ((result == 0x21) && (spawn == 2)) {
            return 1;
        }
        if ((result == 0x22) && (spawn == 4)) {
            return 1;
        }
        if ((result == 0x23) && (spawn == 6)) {
            return 1;
        }
    } else if (sceneId == SCENE_INSIDE_GANONS_CASTLE_COLLAPSE) {
        if ((result == 0x29) && (spawn == 0)) {
            return 1;
        }
        if ((result == 0x2A) && (spawn == 0)) {
            return 1;
        }
    }
    return 0;
}

void func_80B57A74(PlayState* play) {
    Actor* actorIt = play->actorCtx.actorLists[ACTORCAT_PROP].head;
    //! @bug checks for encount2 in ACTORCAT_PROP but encount2 is in ACTORCAT_ENEMY so this condition is never met
    while (actorIt != NULL) {
        if (actorIt->id == ACTOR_EN_ENCOUNT2) {
            ((EnEncount2*)actorIt)->collapseSpawnerInactive = true;
        }
        actorIt = actorIt->next;
    }
}

void func_80B57AAC(EnZl3* this, s32 arg1, AnimationHeader* arg2) {
    if (arg1 != 0) {
        func_80B54E14(this, arg2, 0, -8.0f, 0);
    }
}

void func_80B57AE0(EnZl3* this, PlayState* play) {
    s32 pad;
    s16 shapeRotY = this->actor.shape.rot.y;
    s32 pad2;
    Vec3f* unk_354 = &this->unk_354;
    Vec3f* unk_348 = &this->unk_348;
    Vec3s* temp_v0;
    f32 xDiff;
    f32 zDiff;

    this->unk_344 = 0;
    this->unk_314++;
    this->unk_360 = 0.0f;
    this->unk_364 = 0.0f;
    this->unk_368 = 0.0f;
    *unk_348 = this->actor.world.pos;
    temp_v0 = func_80B56FAC(this, this->unk_314);

    if (temp_v0 != NULL) {
        unk_354->x = temp_v0->x;
        unk_354->y = temp_v0->y;
        unk_354->z = temp_v0->z;
    } else {
        unk_354->x = unk_348->x + (Math_SinS(shapeRotY) * 200.0f);
        unk_354->y = unk_348->y;
        unk_354->z = unk_348->z + (Math_CosS(shapeRotY) * 200.0f);
    }

    xDiff = unk_354->x - unk_348->x;
    zDiff = unk_354->z - unk_348->z;
    this->unk_346 = (s32)(sqrtf(SQ(xDiff) + SQ(zDiff)) / (kREG(6) + 8.0f));
}

s32 func_80B57C54(EnZl3* this) {
    if (this->unk_344 >= this->unk_346) {
        return 1;
    }
    return 0;
}

s32 func_80B57C7C(EnZl3* this, PlayState* play) {
    return 1;
}

s32 func_80B57C8C(EnZl3* this) {
    return !func_80B56F8C(this, this->unk_314 + 2);
}

void func_80B57CB4(EnZl3* this, PlayState* play) {
    Vec3f* unk_348 = &this->unk_348;
    Vec3f* unk_354 = &this->unk_354;
    Vec3f* thisPos = &this->actor.world.pos;
    f32 temp_f0;

    this->unk_344++;
    temp_f0 = Environment_LerpWeightAccelDecel(this->unk_346, 0, this->unk_344, 3, 3);
    thisPos->x = unk_348->x + (temp_f0 * (unk_354->x - unk_348->x));
    thisPos->y = (unk_348->y + (temp_f0 * (unk_354->y - unk_348->y))) + this->unk_360;
    thisPos->z = unk_348->z + (temp_f0 * (unk_354->z - unk_348->z));
}

void func_80B57D60(EnZl3* this, PlayState* play) {
    func_80B57240(this);
}

s32 func_80B57D80(EnZl3* this, PlayState* play) {
    s32 pad;
    s16* sp32 = &this->actor.shape.rot.y;
    NpcInteractInfo* interactInfo = &this->interactInfo;
    Player* player = GET_PLAYER(play);
    s32 unk_314 = this->unk_314;
    s16 temp_v0 = func_80B57104(this, unk_314);
    s32 pad2;
    s16 phi_v1;

    interactInfo->trackPos.y = player->actor.world.pos.y;
    interactInfo->trackPos.x = (Math_SinS(temp_v0) * this->actor.xzDistToPlayer) + this->actor.world.pos.x;
    interactInfo->trackPos.z = (Math_CosS(temp_v0) * this->actor.xzDistToPlayer) + this->actor.world.pos.z;
    interactInfo->yOffset = kREG(16) - 16.0f;
    Npc_TrackPoint(&this->actor, interactInfo, kREG(17) + 0xC, NPC_TRACKING_FULL_BODY);

    phi_v1 = ABS(temp_v0 - *sp32);
    if (phi_v1 <= 0x320) {
        *sp32 = temp_v0;
        this->actor.world.rot.y = *sp32;
        phi_v1 = 0;
    }
    this->actor.world.rot.y = *sp32;
    return phi_v1;
}

void func_80B57EAC(EnZl3* this, PlayState* play) {
    if (func_80B57324(this, play)) {
        s32 pad;

        this->action = 26;
    } else {
        func_80B57350(this, play);
    }
}

void func_80B57EEC(EnZl3* this, PlayState* play) {
    if (func_80B573C8(this, play)) {
        this->action = 27;
    }
}

void func_80B57F1C(EnZl3* this, PlayState* play) {
    if (func_80B57D80(this, play) == 0) {
        func_80B54E14(this, &gZelda2Anime2Anim_009BE4, 0, -8.0f, 0);
        this->action = 34;
        this->unk_314--;
        func_80B57AE0(this, play);
    }
}

s32 func_80B57F84(EnZl3* this, PlayState* play) {
    if (func_80B575D0(this, play) && func_80B57C7C(this, play) && !Play_InCsMode(play)) {
        func_80B54E14(this, &gZelda2Anime2Anim_009FBC, 0, -8.0f, 0);
        this->action = 36;
        this->unk_2EC = 0.0f;
        func_80B57A74(play);
        return 1;
    }
    return 0;
}

void func_80B58014(EnZl3* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s8 invincibilityTimer = player->invincibilityTimer;

    if (func_80B57324(this, play)) {
        func_80B54E14(this, &gZelda2Anime2Anim_003FF8, 0, -11.0f, 0);
        this->action = 29;
        func_80B538B0(this);
    } else if (func_80B57C8C(this) && func_80B57F84(this, play)) {
        s32 pad;

        OnePointCutscene_Init(play, 4000, -99, &this->actor, CAM_ID_MAIN);
        this->unk_3D0 = 0;
    } else if (func_80B576C8(this, play) && func_80B575B0(this, play) && !Play_InCsMode(play)) {
        this->action = 0x1F;
        this->unk_3CC = 0.0f;
        func_80B537E8(this);
        this->unk_3D8 = 1;
        OnePointCutscene_Init(play, 4010, -99, &this->actor, CAM_ID_MAIN);
    } else if (!func_80B57C8C(this) && !func_80B576C8(this, play) && func_80B57564(this, play)) {
        func_80B54E14(this, &gZelda2Anime2Anim_009BE4, 0, -8.0f, 0);
        func_80B5764C(this, play);
        this->action = 34;
        this->unk_3D0 = 0;
        func_80B57AE0(this, play);
    } else if ((invincibilityTimer > 0) || (player->fallDistance >= 51)) {
        func_80B54E14(this, &gZelda2Anime2Anim_007664, 0, -11.0f, 0);
        this->action = 30;
        func_80B537E8(this);
        func_80B56DC8(this);
    } else {
        func_80B57350(this, play);
        func_80B538B0(this);
    }
}

void func_80B58214(EnZl3* this, PlayState* play) {
    if (func_80B573C8(this, play)) {
        func_80B54E14(this, &gZelda2Anime2Anim_009FBC, 0, -11.0f, 0);
        this->action = 28;
        this->unk_3D0 = 0;
    }
}

void func_80B58268(EnZl3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s8 invincibilityTimer = player->invincibilityTimer;

    if ((invincibilityTimer <= 0) && (player->fallDistance <= 50)) {
        func_80B54E14(this, &gZelda2Anime2Anim_009FBC, 0, -11.0f, 0);
        this->action = 28;
        this->unk_3D0 = 0;
    }
}

void func_80B582C8(EnZl3* this, PlayState* play) {
    f32* unk_3CC = &this->unk_3CC;
    s32 pad;

    if (*unk_3CC == kREG(14) + 10.0f) {
        *unk_3CC += 1.0f;
        func_80B54E14(this, &gZelda2Anime2Anim_008050, 0, -12.0f, 0);
        func_80B57704(this, play);
    } else if (*unk_3CC == kREG(15) + 20.0f) {
        *unk_3CC += 1.0f;
        func_80B56DC8(this);
        func_80B54E14(this, &gZelda2Anime2Anim_003FF8, 0, -12.0f, 0);
    } else if (*unk_3CC == kREG(16) + 30.0f) {
        *unk_3CC += 1.0f;
        func_80B57858(play);
    } else if (*unk_3CC == kREG(17) + 40.0f) {
        Camera_SetFinishedFlag(GET_ACTIVE_CAM(play));
        *unk_3CC += 1.0f;
    } else if (*unk_3CC >= ((kREG(17) + 40.0f) + 1.0f)) {
        this->action = 32;
        *unk_3CC = 0.0f;
    } else {
        *unk_3CC += 1.0f;
    }
}

static u32 D_80B5A4BC = 0;

void func_80B584B4(EnZl3* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s8 invincibilityTimer = player->invincibilityTimer;
    Actor* nearbyEnTest = Actor_FindNearby(play, &this->actor, ACTOR_EN_TEST, ACTORCAT_ENEMY, 8000.0f);

    if (D_80B5A4BC == 0) {
        if ((nearbyEnTest == NULL) && (!Play_InCsMode(play))) {
            this->action = 33;
            OnePointCutscene_Init(play, 4011, -99, &this->actor, CAM_ID_MAIN);
        } else if (invincibilityTimer > 0) {
            func_80B54E14(this, &gZelda2Anime2Anim_003FF8, 0, -12.0f, 0);
            D_80B5A4BC = 1;
            func_80B56DC8(this);
        }
    } else {
        if ((nearbyEnTest == NULL) && (!Play_InCsMode(play))) {
            func_80B54E14(this, &gZelda2Anime2Anim_007664, 0, -12.0f, 0);
            D_80B5A4BC = 0;
            this->action = 33;
            OnePointCutscene_Init(play, 4011, -99, &this->actor, CAM_ID_MAIN);
        } else if (invincibilityTimer <= 0) {
            func_80B54E14(this, &gZelda2Anime2Anim_007664, 0, -12.0f, 0);
            D_80B5A4BC = 0;
        }
    }
}

void func_80B58624(EnZl3* this, PlayState* play) {
    s32 pad[3];
    f32* unk_3CC = &this->unk_3CC;

    if (*unk_3CC == (kREG(18) + 10.0f)) {
        *unk_3CC += 1.0f;
        func_80B54E14(this, &gZelda2Anime2Anim_008050, 0, -12.0f, 0);
        func_80B5772C(this, play);
    } else if (*unk_3CC == kREG(19) + 20.0f) {
        s32 pad2;

        *unk_3CC += 1.0f;
        this->actor.textId = 0x71AC;
        Message_StartTextbox(play, this->actor.textId, NULL);
        func_80B54E14(this, &gZelda2Anime2Anim_003FF8, 0, -12.0f, 0);
    } else if (*unk_3CC == ((kREG(19) + 20.0f) + 1.0f)) {
        if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
            *unk_3CC += 1.0f;
            func_80B5357C(this, play);
            func_80B5357C(this, play);
            func_80B5357C(this, play);
            func_80B5357C(this, play);
            func_80B5357C(this, play);
        }
    } else {
        if (*unk_3CC >= kREG(20) + 30.0f) {
            this->action = 28;
            Camera_SetFinishedFlag(GET_ACTIVE_CAM(play));
            func_80B54E14(this, &gZelda2Anime2Anim_009FBC, 0, -12.0f, 0);
            *unk_3CC = 0.0f;
        } else {
            *unk_3CC += 1.0f;
        }
    }
}

void func_80B5884C(EnZl3* this, PlayState* play) {
    func_80B54E14(this, &gZelda2Anime2Anim_0038C0, 2, -8.0f, 0);
    this->action = 37;
    this->unk_36C = 1;
}

void func_80B58898(EnZl3* this, PlayState* play) {
    func_80B54E14(this, &gZelda2Anime2Anim_0038C0, 2, -8.0f, 1);
    this->action = 38;
    this->unk_374 = 1;
}

void func_80B588E8(EnZl3* this, PlayState* play) {
    func_80B54E14(this, &gZelda2Anime2Anim_009BE4, 0, -8.0f, 0);
    func_80B57AE0(this, play);
    this->action = 39;
}

s32 func_80B58938(EnZl3* this, PlayState* play) {
    if (func_80B57C54(this)) {
        func_80B54E14(this, &gZelda2Anime2Anim_009FBC, 0, -8.0f, 0);
        this->action = 28;
        this->unk_3D0 = 0;
        return 1;
    }
    return 0;
}

s32 func_80B5899C(EnZl3* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Player* player = GET_PLAYER(play);
        s8 invincibilityTimer = player->invincibilityTimer;

        if ((invincibilityTimer > 0) || (player->fallDistance >= 51)) {
            func_80B54E14(this, &gZelda2Anime2Anim_007664, 2, -11.0f, 0);
            this->action = 35;
            func_80B56DC8(this);
            return 1;
        }
    }
    return 0;
}

void func_80B58A1C(EnZl3* this, PlayState* play) {
    if (!func_80B58938(this, play)) {
        func_80B5899C(this, play);
    }
}

void func_80B58A50(EnZl3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s8 invincibilityTimer = player->invincibilityTimer;

    if ((invincibilityTimer <= 0) && (player->fallDistance <= 50)) {
        func_80B54E14(this, &gZelda2Anime2Anim_009BE4, 0, -11.0f, 0);
        this->action = 34;
    }
}

void func_80B58AAC(EnZl3* this, PlayState* play) {
    f32* unk_2EC = &this->unk_2EC;

    *unk_2EC += 1.0f;
    if ((*unk_2EC >= kREG(7) + 24.0f) && (this->unk_36C == 0)) {
        func_80B57754(this, play);
        func_80B5884C(this, play);
    } else if ((*unk_2EC >= kREG(8) + 50.0f) && (this->unk_370 == 0)) {
        func_80B56EB8(this, play);
        this->unk_370 = 1;
    } else if ((*unk_2EC >= kREG(9) + 56.0f) && (this->unk_374 == 0)) {
        func_80B58898(this, play);
    } else if (*unk_2EC >= kREG(10) + 82.0f) {
        func_80B588E8(this, play);
    }
}

void func_80B58C08(EnZl3* this, PlayState* play) {
    s32 pad[2];
    Vec3f* unk_348 = &this->unk_348;
    Vec3f* unk_354 = &this->unk_354;
    Vec3f* thisPos = &this->actor.world.pos;
    s32 unk_344;
    s32 unk_346;
    s32 sp28;
    f32 temp_f0;

    this->unk_344++;

    unk_344 = this->unk_344;
    unk_346 = this->unk_346;
    sp28 = unk_346 - kREG(11) - 2;
    temp_f0 = Environment_LerpWeightAccelDecel(unk_346, 0, unk_344, 3, 0);

    thisPos->x = unk_348->x + (temp_f0 * (unk_354->x - unk_348->x));
    thisPos->y = (unk_348->y + (temp_f0 * (unk_354->y - unk_348->y))) + this->unk_360;
    thisPos->z = unk_348->z + (temp_f0 * (unk_354->z - unk_348->z));

    if ((unk_344 - sp28) >= 0) {
        this->alpha = ((f32)(unk_346 - unk_344) / (kREG(11) + 2)) * 255.0f;
        this->drawConfig = 2;
    }

    if ((unk_346 - unk_344) <= 0) {
        Actor_Kill(&this->actor);
    }
}

void func_80B58D50(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B57EAC(this, play);
}

void func_80B58DB0(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B57EEC(this, play);
}

void func_80B58E10(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    Actor_SetFocus(&this->actor, 60.0f);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B57F1C(this, play);
}

void func_80B58E7C(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B53764(this, play);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B58014(this, play);
    func_80B536B4(this);
}

void func_80B58EF4(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B538B0(this);
    func_80B53764(this, play);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B58214(this, play);
}

void func_80B58F6C(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B537E8(this);
    func_80B536C4(this);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B58268(this, play);
}

void func_80B58FDC(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B537E8(this);
    func_80B536C4(this);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B582C8(this, play);
}

void func_80B5904C(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B537E8(this);
    func_80B536C4(this);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B584B4(this, play);
}

void func_80B590BC(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B537E8(this);
    func_80B536C4(this);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B58624(this, play);
}

void func_80B5912C(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B536C4(this);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    func_80B56E38(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B57CB4(this, play);
    func_80B57D60(this, play);
    func_80B58A1C(this, play);
}

void func_80B591BC(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B536C4(this);
    func_80B538B0(this);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B58A50(this, play);
}

void func_80B5922C(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B536C4(this);
    func_80B57298(this);
    Actor_SetFocus(&this->actor, 60.0f);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B58AAC(this, play);
}

void func_80B592A8(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B536C4(this);
    func_80B57298(this);
    Actor_SetFocus(&this->actor, 60.0f);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    func_80B57AAC(this, EnZl3_UpdateSkelAnime(this), &gZelda2Anime2Anim_003D20);
    func_80B56DEC(this);
    func_80B58AAC(this, play);
}

void func_80B59340(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B536C4(this);
    func_80B57298(this);
    Actor_SetFocus(&this->actor, 60.0f);
    func_80B533FC(this, play);
    func_80B5366C(this, play);
    EnZl3_UpdateEyes(this);
    func_80B57AAC(this, EnZl3_UpdateSkelAnime(this), &gZelda2Anime2Anim_009FBC);
    func_80B58AAC(this, play);
}

void func_80B593D0(EnZl3* this, PlayState* play) {
    func_80B54DE0(this, play);
    func_80B536C4(this);
    func_80B57298(this);
    func_80B5366C(this, play);
    func_80B56E38(this, play);
    Actor_SetFocus(&this->actor, 60.0f);
    EnZl3_UpdateEyes(this);
    EnZl3_UpdateSkelAnime(this);
    func_80B58C08(this, play);
}

s32 func_80B5944C(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx, Gfx** gfx) {
    if (limbIndex == 14) {
        Mtx* mtx = GRAPH_ALLOC(play->state.gfxCtx, sizeof(Mtx) * 7);
        EnZl3* this = (EnZl3*)thisx;
        Vec3s* headRot = &this->interactInfo.headRot;

        gSPSegment(gfx[0]++, 0x0C, mtx);

        rot->x += headRot->y;
        rot->z += headRot->x;
        Matrix_Push();
        Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_APPLY);
        Matrix_RotateZYX(rot->x, rot->y, rot->z, MTXMODE_APPLY);
        Matrix_Push();
        Matrix_Translate(174.0f, -317.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&mtx[0], "../z_en_zl3_inEscape.c", 2471);
        Matrix_Translate(-410.0f, -184.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&mtx[1], "../z_en_zl3_inEscape.c", 2474);
        Matrix_Translate(-1019.0f, -26.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&mtx[2], "../z_en_zl3_inEscape.c", 2477);
        Matrix_Pop();
        Matrix_Push();
        Matrix_Translate(40.0f, 264.0f, 386.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&mtx[3], "../z_en_zl3_inEscape.c", 2483);
        Matrix_Translate(-446.0f, -52.0f, 84.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&mtx[4], "../z_en_zl3_inEscape.c", 2486);
        Matrix_Pop();
        Matrix_Push();
        Matrix_Translate(40.0f, 264.0f, -386.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&mtx[5], "../z_en_zl3_inEscape.c", 2492);
        Matrix_Translate(-446.0f, -52.0f, -84.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&mtx[6], "../z_en_zl3_inEscape.c", 2495);
        Matrix_Pop();
        Matrix_Pop();
    }
    return false;
}

s32 func_80B59698(EnZl3* this, PlayState* play) {
    s32 cond = Flags_GetSwitch(play, 0x37) &&
               ((play->sceneId == SCENE_GANON_BOSS) || (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) ||
                (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) ||
                (play->sceneId == SCENE_INSIDE_GANONS_CASTLE_COLLAPSE));

    if (cond) {
        u8 spawn = play->spawn;

        if ((func_80B54DB4(this) == 0x20) && (spawn == 0) &&
            ((gSaveContext.subTimerSeconds <= 0) || (gSaveContext.subTimerState == SUBTIMER_STATE_OFF))) {
            return 1;
        }
    }
    return 0;
}

s32 func_80B59768(EnZl3* this, PlayState* play) {
    s32 cond = Flags_GetSwitch(play, 0x37) &&
               ((play->sceneId == SCENE_GANON_BOSS) || (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) ||
                (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) ||
                (play->sceneId == SCENE_INSIDE_GANONS_CASTLE_COLLAPSE));

    if (cond) {
        u8 spawn = play->spawn;

        if ((func_80B54DB4(this) == 0x20) && (spawn == 0) && (gSaveContext.subTimerSeconds <= 0)) {
            return 1;
        }
    }
    return 0;
}

void func_80B59828(EnZl3* this, PlayState* play) {
    s16 newRotY;

    if (func_80B59698(this, play) || (!func_80B56EE4(this, play) && func_80B57890(this, play))) {
        func_80B54E14(this, &gZelda2Anime2Anim_009FBC, 0, 0.0f, 0);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        func_80B56F10(this, play);
        newRotY = func_80B571A8(this);
        this->actor.shape.rot.y = newRotY;
        this->actor.world.rot.y = newRotY;
        this->unk_3C4 = this->actor.world.rot.z;
        this->actor.shape.rot.z = 0;
        this->actor.world.rot.z = this->actor.shape.rot.z;
        this->action = 28;
        this->drawConfig = 1;
    } else {
        Actor_Kill(&this->actor);
    }

    if (func_80B59698(this, play) != 0) {
        Interface_SetSubTimer(180);
        func_80B53468();
        gSaveContext.healthAccumulator = 320;
        Magic_Fill(play);
        if (Flags_GetSwitch(play, 0x20)) {
            Flags_UnsetSwitch(play, 0x20);
            Actor_Spawn(&play->actorCtx, play, ACTOR_BG_ZG, -144.0f, 3544.0f, -43.0f, 0, 0x2000, 0, 0x2000);
        }
        Flags_UnsetSwitch(play, 0x21);
        Flags_UnsetSwitch(play, 0x22);
        Flags_UnsetSwitch(play, 0x23);
        Flags_UnsetSwitch(play, 0x24);
        Flags_UnsetSwitch(play, 0x25);
        Flags_UnsetSwitch(play, 0x26);
        Flags_UnsetSwitch(play, 0x27);
        Flags_UnsetSwitch(play, 0x28);
        Flags_UnsetSwitch(play, 0x29);
        Flags_UnsetSwitch(play, 0x2A);
    }

    if (func_80B54DB4(this) == 0x20) {
        s32 cond;

        func_80B54EA4(this, play);
        cond = Flags_GetSwitch(play, 0x37) &&
               ((play->sceneId == SCENE_GANON_BOSS) || (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) ||
                (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) ||
                (play->sceneId == SCENE_INSIDE_GANONS_CASTLE_COLLAPSE));
        if (cond) {
            func_80B53614(this, play);
        }
    }
}
