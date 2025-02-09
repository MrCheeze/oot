void En_Zl3_inEscape_Set_PowerSound(EnZl3* this) {
    Na_StartFixSe_F(NA_SE_EV_ZELDA_POWER);
}

void En_Zl3_inEscape_Set_PainSound(EnZl3* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_Z1_PAIN);
}

void En_Zl3_inEscape_Set_OpenDoorSound(EnZl3* this) {
    SkelAnime* skelAnime = &this->skelAnime;

    if ((skelAnime->mode == 2) && Skeleton_Info_frame_check(skelAnime, 9.0f) != 0) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_Z1_OPENDOOR);
    }
}

void En_Zl3_inEscape_Set_RunSound(EnZl3* this, PlayState* play) {
    s32 pad[2];
    s32 sfxId;
    SkelAnime* sp20 = &this->skelAnime;

    if ((Skeleton_Info_frame_check(sp20, 6.0f) || Skeleton_Info_frame_check(sp20, 0.0f)) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        sfxId = NA_SE_PL_WALK_GROUND;
        sfxId += T_BGCheck_getSoundGroundLabel(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
        Na_StartObjectSe_F(&this->actor.projectedPos, sfxId);
    }
}

void En_Zl3_inEscape_Set_SaveBit(EnZl3* this, PlayState* play) {
    Actor_Environment_sw_On(play, En_Zl3_GetUpper_arg_data(this));
}

s32 En_Zl3_inEscape_Check_SaveBit(EnZl3* this, PlayState* play) {
    return Actor_Environment_sw_Check(play, En_Zl3_GetUpper_arg_data(this));
}

void En_Zl3_Init_path_info(EnZl3* this, PlayState* play) {
    s32 waypoint;
    Path* pathHead = play->pathList;

    if (pathHead != NULL) {
        waypoint = En_Zl3_GetMidder_arg_data(this);
        pathHead += waypoint;
        this->unk_30C = pathHead;
        this->unk_310 = pathHead->count;
        PRINTF("En_Zl3_Get_path_info レールデータをゲットだぜ = %d!!!!!!!!!!!!!!\n", waypoint);
    } else {
        PRINTF("En_Zl3_Get_path_info レールデータが無い!!!!!!!!!!!!!!!!!!!!\n");
    }
}

s32 En_Zl3_Check_path_index(EnZl3* this, s32 arg1) {
    s32 unk_310 = this->unk_310;

    if (unk_310 > arg1) {
        return 1;
    }
    return 0;
}

Vec3s* En_Zl3_Get_path_pos(EnZl3* this, s32 arg1) {
    Vec3s* point;
    Path* pathList = this->unk_30C;

    if ((pathList != NULL) && En_Zl3_Check_path_index(this, arg1)) {
        point = &((Vec3s*)SEGMENTED_TO_VIRTUAL(pathList->points))[arg1];
        return point;
    }
    return NULL;
}

s32 En_Zl3_Get_path_angle(EnZl3* this, s32 arg1, s32 arg2) {
    Vec3s* vec1 = En_Zl3_Get_path_pos(this, arg1);
    Vec3s* vec2 = En_Zl3_Get_path_pos(this, arg2);

    if ((vec2 != NULL) && (vec1 != NULL)) {
        f32 xDiff = vec2->x - vec1->x;
        f32 zDiff = vec2->z - vec1->z;

        return ((xDiff == 0.0f) && (zDiff == 0.0f)) ? 0 : RAD_TO_BINANG(fatan2(xDiff, zDiff));
    }
    return 0;
}

s16 En_Zl3_Get_NowToNextPos_angle(EnZl3* this, s32 arg1) {
    Vec3s* point = En_Zl3_Get_path_pos(this, arg1);

    if (point != NULL) {
        f32 xDiff = point->x - this->actor.world.pos.x;
        f32 zDiff = point->z - this->actor.world.pos.z;

        if ((xDiff != 0.0f) || (zDiff != 0.0f)) {
            return RAD_TO_BINANG(fatan2(xDiff, zDiff));
        }
    }
    return 0;
}

s32 En_Zl3_Get_next_path_angle(EnZl3* this) {
    s32 pad;
    s32 unk_314 = this->unk_314;
    s32 pad2;

    if (En_Zl3_Check_path_index(this, unk_314 + 1) == 0) {
        return this->actor.shape.rot.y;
    } else {
        return En_Zl3_Get_path_angle(this, unk_314, unk_314 + 1);
    }
}

s32 En_Zl3_Get_now_path_angle(EnZl3* this) {
    s32 pad;
    s32 unk_314 = this->unk_314;

    if (En_Zl3_Check_path_index(this, unk_314) == 0) {
        return this->actor.shape.rot.y;
    } else {
        return En_Zl3_Get_NowToNextPos_angle(this, unk_314);
    }
}

void En_Zl3_SetAngle_forPath(EnZl3* this) {
    s32 temp_a1 = En_Zl3_Get_now_path_angle(this);
    s16* rotY = &this->actor.world.rot.y;

    add_calc_short_angle2(rotY, temp_a1, 2, 6400, 1000);
    this->actor.shape.rot.y = *rotY;
}

void En_Zl3_SetAngle_forNextPath(EnZl3* this) {
    s16* rotY = &this->actor.world.rot.y;
    s16 temp_a1 = En_Zl3_Get_next_path_angle(this);

    add_calc_short_angle2(rotY, temp_a1, 2, 6400, 1000);
    this->actor.shape.rot.y = *rotY;
}

u16 En_Zl3_Get_Message(PlayState* play) {
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

s32 En_Zl3_Check_ConversationSuccess(EnZl3* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        return 1;
    }
    return 0;
}

void En_Zl3_Request_Conversation(EnZl3* this, PlayState* play) {
    s16 temp_v0 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (ABS(temp_v0) <= 0x4300) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        this->actor.textId = En_Zl3_Get_Message(play);
        Actor_talk_request(&this->actor, play);
    }
}

s32 En_Zl3_Check_ConversationEnd(EnZl3* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        return 1;
    }
    return 0;
}

s32 En_Zl3_CalcDistance_Link(EnZl3* this, PlayState* play, f32 arg2) {
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

s32 En_Zl3_CalcDirection_Link(EnZl3* this, PlayState* play) {
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
    s16 temp_v1 = En_Zl3_Get_next_path_angle(this);

    if (temp_f12 == 0.0f && temp_f13 == 0.0f) {
        return 1;
    }

    temp_v0 = (s16)(temp_v1 - RAD_TO_BINANG(fatan2(temp_f12, temp_f13)));

    if (temp_v0 < 0x1555) {
        return 1;
    } else if ((temp_v0 < 0x4000) && En_Zl3_CalcDistance_Link(this, play, 150.0f)) {
        return 1;
    } else {
        return 0;
    }
}

s32 En_Zl3_Search_Link(EnZl3* this, PlayState* play) {
    if (En_Zl3_CalcDistance_Link(this, play, 50.0f) || En_Zl3_CalcDirection_Link(this, play)) {
        return 1;
    }
    return 0;
}

s32 En_Zl3_Search_Link2(EnZl3* this, PlayState* play) {
    return En_Zl3_CalcDistance_Link(this, play, 150.0f);
}

s32 En_Zl3_Search_Link3(EnZl3* this, PlayState* play) {
    return En_Zl3_CalcDistance_Link(this, play, 50.0f);
}

s32 En_Zl3_Check_PathPoint_forObstacle(EnZl3* this, PlayState* play) {
    s16 sceneId = play->sceneId;

    if ((sceneId == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) && (En_Zl3_GetUpper_arg_data(this) == 0x26)) {
        s32 unk_314 = this->unk_314;

        if (unk_314 == 1) {
            return 1;
        }
    }
    return 0;
}

void En_Zl3_Check_NextPathPoint_forObstacle(EnZl3* this, PlayState* play) {
    s16 sceneId = play->sceneId;

    if ((sceneId == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) && (En_Zl3_GetUpper_arg_data(this) == 0x26)) {
        s32 unk_314 = this->unk_314 + 1;

        if ((unk_314 == 1) && !Game_play_demo_mode_check(play)) {
            makeOnepointDemo(play, 1000, 40, &this->actor, CAM_ID_MAIN);
        }
    }
}

s32 En_Zl3_Search_ObstacleDemo(EnZl3* this, PlayState* play) {
    if (En_Zl3_Check_PathPoint_forObstacle(this, play)) {
        s32 pad;

        if (this->unk_3D8 == 0) {
            return 1;
        }
    }
    return 0;
}

void En_Zl3_Set_Obstacle(EnZl3* this, PlayState* play) {
    s32 unk_3C4 = this->unk_3C4;

    Actor_Environment_sw_On(play, unk_3C4);
}

void En_Zl3_Unset_Obstacle(EnZl3* this, PlayState* play) {
    s32 unk_3C4 = this->unk_3C4;

    Actor_Environment_sw_Off(play, unk_3C4);
}

void Birth_Wipe4_In_En_Zl3_inEscape(EnZl3* this, PlayState* play) {
    if (z_common_data.magicState == MAGIC_STATE_IDLE) {
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_OCEFF_WIPE4, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, 0, 0, 1);
        En_Zl3_inEscape_Set_PowerSound(this);
    }
}

void Birth_Sl2_In_En_Zl3_inEscape(PlayState* play, Vec3f* vec) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    Vec3f* playerPos = &player->actor.world.pos;
    f32 posX = vec->x;
    f32 posY = vec->y;
    f32 posZ = vec->z;

    Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_TEST, posX, posY, posZ, 0,
                RAD_TO_BINANG(fatan2(playerPos->x - posX, playerPos->z - posZ)), 0, 5);
}

static Vec3f set_pos = { 148.0f, 260.0f, -87.0f };
static Vec3f D_80B5A4A4 = { -12.0f, 260.0f, -147.0f };
static Vec3f D_80B5A4B0 = { 42.0f, 260.0f, 13.0f };

void Set_Sk2_In_En_Zl3_inEscape(PlayState* play) {
    Birth_Sl2_In_En_Zl3_inEscape(play, &set_pos);
    Birth_Sl2_In_En_Zl3_inEscape(play, &D_80B5A4A4);
}

s32 En_Zl3_inEscape_Check_LinkStart(EnZl3* this, PlayState* play) {
    s8 pad[2];
    u8 spawn = play->spawn;
    s16 sceneId = play->sceneId;
    s32 result = En_Zl3_GetUpper_arg_data(this);

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
        if ((result == 0x20) && (spawn == 0) && Actor_Environment_sw_Check(play, 0x37)) {
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

void En_Zl3_inEscape_Stop_Encount2(PlayState* play) {
    Actor* actorIt = play->actorCtx.actorLists[ACTORCAT_PROP].head;
    //! @bug checks for encount2 in ACTORCAT_PROP but encount2 is in ACTORCAT_ENEMY so this condition is never met
    while (actorIt != NULL) {
        if (actorIt->id == ACTOR_EN_ENCOUNT2) {
            ((EnEncount2*)actorIt)->collapseSpawnerInactive = true;
        }
        actorIt = actorIt->next;
    }
}

void En_Zl3_inEscape_ChengAnime_StopToRepeat(EnZl3* this, s32 arg1, AnimationHeader* arg2) {
    if (arg1 != 0) {
        En_Zl3_Change_Anime(this, arg2, 0, -8.0f, 0);
    }
}

void En_Zl3_inEscape_InitPosParam(EnZl3* this, PlayState* play) {
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
    temp_v0 = En_Zl3_Get_path_pos(this, this->unk_314);

    if (temp_v0 != NULL) {
        unk_354->x = temp_v0->x;
        unk_354->y = temp_v0->y;
        unk_354->z = temp_v0->z;
    } else {
        unk_354->x = unk_348->x + (sin_s(shapeRotY) * 200.0f);
        unk_354->y = unk_348->y;
        unk_354->z = unk_348->z + (cos_s(shapeRotY) * 200.0f);
    }

    xDiff = unk_354->x - unk_348->x;
    zDiff = unk_354->z - unk_348->z;
    this->unk_346 = (s32)(sqrtf(SQ(xDiff) + SQ(zDiff)) / (kREG(6) + 8.0f));
}

s32 En_Zl3_inEscape_Check_move_time(EnZl3* this) {
    if (this->unk_344 >= this->unk_346) {
        return 1;
    }
    return 0;
}

s32 En_Zl3_inEscape_Check_ZeldaShatter(EnZl3* this, PlayState* play) {
    return 1;
}

s32 En_Zl3_Check_ShutterPoint(EnZl3* this) {
    return !En_Zl3_Check_path_index(this, this->unk_314 + 2);
}

void En_Zl3_inEscape_MoveSetPos(EnZl3* this, PlayState* play) {
    Vec3f* unk_348 = &this->unk_348;
    Vec3f* unk_354 = &this->unk_354;
    Vec3f* thisPos = &this->actor.world.pos;
    f32 temp_f0;

    this->unk_344++;
    temp_f0 = get_parcent_forAccelBrake(this->unk_346, 0, this->unk_344, 3, 3);
    thisPos->x = unk_348->x + (temp_f0 * (unk_354->x - unk_348->x));
    thisPos->y = (unk_348->y + (temp_f0 * (unk_354->y - unk_348->y))) + this->unk_360;
    thisPos->z = unk_348->z + (temp_f0 * (unk_354->z - unk_348->z));
}

void En_Zl3_inEscape_ChangeAngle(EnZl3* this, PlayState* play) {
    En_Zl3_SetAngle_forPath(this);
}

s32 En_Zl3_inEscape_Set_TurnAngle(EnZl3* this, PlayState* play) {
    s32 pad;
    s16* sp32 = &this->actor.shape.rot.y;
    NpcInteractInfo* interactInfo = &this->interactInfo;
    Player* player = GET_PLAYER(play);
    s32 unk_314 = this->unk_314;
    s16 temp_v0 = En_Zl3_Get_NowToNextPos_angle(this, unk_314);
    s32 pad2;
    s16 phi_v1;

    interactInfo->trackPos.y = player->actor.world.pos.y;
    interactInfo->trackPos.x = (sin_s(temp_v0) * this->actor.xzDistToPlayer) + this->actor.world.pos.x;
    interactInfo->trackPos.z = (cos_s(temp_v0) * this->actor.xzDistToPlayer) + this->actor.world.pos.z;
    interactInfo->yOffset = kREG(16) - 16.0f;
    eye_moveM(&this->actor, interactInfo, kREG(17) + 0xC, NPC_TRACKING_FULL_BODY);

    phi_v1 = ABS(temp_v0 - *sp32);
    if (phi_v1 <= 0x320) {
        *sp32 = temp_v0;
        this->actor.world.rot.y = *sp32;
        phi_v1 = 0;
    }
    this->actor.world.rot.y = *sp32;
    return phi_v1;
}

void En_Zl3_inEscape_check_StartToGreet(EnZl3* this, PlayState* play) {
    if (En_Zl3_Check_ConversationSuccess(this, play)) {
        s32 pad;

        this->action = 26;
    } else {
        En_Zl3_Request_Conversation(this, play);
    }
}

void En_Zl3_inEscape_check_GreetToTurn(EnZl3* this, PlayState* play) {
    if (En_Zl3_Check_ConversationEnd(this, play)) {
        this->action = 27;
    }
}

void En_Zl3_inEscape_check_TurnToWalk(EnZl3* this, PlayState* play) {
    if (En_Zl3_inEscape_Set_TurnAngle(this, play) == 0) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_009BE4, 0, -8.0f, 0);
        this->action = 34;
        this->unk_314--;
        En_Zl3_inEscape_InitPosParam(this, play);
    }
}

s32 En_Zl3_inEscape_check_StandToKeep(EnZl3* this, PlayState* play) {
    if (En_Zl3_Search_Link3(this, play) && En_Zl3_inEscape_Check_ZeldaShatter(this, play) && !Game_play_demo_mode_check(play)) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_009FBC, 0, -8.0f, 0);
        this->action = 36;
        this->unk_2EC = 0.0f;
        En_Zl3_inEscape_Stop_Encount2(play);
        return 1;
    }
    return 0;
}

void En_Zl3_inEscape_check_StandToBranch(EnZl3* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s8 invincibilityTimer = player->invincibilityTimer;

    if (En_Zl3_Check_ConversationSuccess(this, play)) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_003FF8, 0, -11.0f, 0);
        this->action = 29;
        En_Zl3_SetAngle_forLink2(this);
    } else if (En_Zl3_Check_ShutterPoint(this) && En_Zl3_inEscape_check_StandToKeep(this, play)) {
        s32 pad;

        makeOnepointDemo(play, 4000, -99, &this->actor, CAM_ID_MAIN);
        this->unk_3D0 = 0;
    } else if (En_Zl3_Search_ObstacleDemo(this, play) && En_Zl3_Search_Link2(this, play) && !Game_play_demo_mode_check(play)) {
        this->action = 0x1F;
        this->unk_3CC = 0.0f;
        En_Zl3_SetAngle_forLink(this);
        this->unk_3D8 = 1;
        makeOnepointDemo(play, 4010, -99, &this->actor, CAM_ID_MAIN);
    } else if (!En_Zl3_Check_ShutterPoint(this) && !En_Zl3_Search_ObstacleDemo(this, play) && En_Zl3_Search_Link(this, play)) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_009BE4, 0, -8.0f, 0);
        En_Zl3_Check_NextPathPoint_forObstacle(this, play);
        this->action = 34;
        this->unk_3D0 = 0;
        En_Zl3_inEscape_InitPosParam(this, play);
    } else if ((invincibilityTimer > 0) || (player->fallDistance >= 51)) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_007664, 0, -11.0f, 0);
        this->action = 30;
        En_Zl3_SetAngle_forLink(this);
        En_Zl3_inEscape_Set_PainSound(this);
    } else {
        En_Zl3_Request_Conversation(this, play);
        En_Zl3_SetAngle_forLink2(this);
    }
}

void En_Zl3_inEscape_check_EncourageToStand(EnZl3* this, PlayState* play) {
    if (En_Zl3_Check_ConversationEnd(this, play)) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_009FBC, 0, -11.0f, 0);
        this->action = 28;
        this->unk_3D0 = 0;
    }
}

void En_Zl3_inEscape_check_AvertToStand(EnZl3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s8 invincibilityTimer = player->invincibilityTimer;

    if ((invincibilityTimer <= 0) && (player->fallDistance <= 50)) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_009FBC, 0, -11.0f, 0);
        this->action = 28;
        this->unk_3D0 = 0;
    }
}

void En_Zl3_inEscape_ControlDemo_forSurprise(EnZl3* this, PlayState* play) {
    f32* unk_3CC = &this->unk_3CC;
    s32 pad;

    if (*unk_3CC == kREG(14) + 10.0f) {
        *unk_3CC += 1.0f;
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_008050, 0, -12.0f, 0);
        En_Zl3_Set_Obstacle(this, play);
    } else if (*unk_3CC == kREG(15) + 20.0f) {
        *unk_3CC += 1.0f;
        En_Zl3_inEscape_Set_PainSound(this);
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_003FF8, 0, -12.0f, 0);
    } else if (*unk_3CC == kREG(16) + 30.0f) {
        *unk_3CC += 1.0f;
        Set_Sk2_In_En_Zl3_inEscape(play);
    } else if (*unk_3CC == kREG(17) + 40.0f) {
        restartCameraStoped(GET_ACTIVE_CAM(play));
        *unk_3CC += 1.0f;
    } else if (*unk_3CC >= ((kREG(17) + 40.0f) + 1.0f)) {
        this->action = 32;
        *unk_3CC = 0.0f;
    } else {
        *unk_3CC += 1.0f;
    }
}

static u32 kao_kaakusu = 0;

void En_Zl3_inEscape_ControlDemo_forPrizon(EnZl3* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s8 invincibilityTimer = player->invincibilityTimer;
    Actor* nearbyEnTest = ActorSearch(play, &this->actor, ACTOR_EN_TEST, ACTORCAT_ENEMY, 8000.0f);

    if (kao_kaakusu == 0) {
        if ((nearbyEnTest == NULL) && (!Game_play_demo_mode_check(play))) {
            this->action = 33;
            makeOnepointDemo(play, 4011, -99, &this->actor, CAM_ID_MAIN);
        } else if (invincibilityTimer > 0) {
            En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_003FF8, 0, -12.0f, 0);
            kao_kaakusu = 1;
            En_Zl3_inEscape_Set_PainSound(this);
        }
    } else {
        if ((nearbyEnTest == NULL) && (!Game_play_demo_mode_check(play))) {
            En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_007664, 0, -12.0f, 0);
            kao_kaakusu = 0;
            this->action = 33;
            makeOnepointDemo(play, 4011, -99, &this->actor, CAM_ID_MAIN);
        } else if (invincibilityTimer <= 0) {
            En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_007664, 0, -12.0f, 0);
            kao_kaakusu = 0;
        }
    }
}

void En_Zl3_inEscape_ControlDemo_forThanks(EnZl3* this, PlayState* play) {
    s32 pad[3];
    f32* unk_3CC = &this->unk_3CC;

    if (*unk_3CC == (kREG(18) + 10.0f)) {
        *unk_3CC += 1.0f;
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_008050, 0, -12.0f, 0);
        En_Zl3_Unset_Obstacle(this, play);
    } else if (*unk_3CC == kREG(19) + 20.0f) {
        s32 pad2;

        *unk_3CC += 1.0f;
        this->actor.textId = 0x71AC;
        message_set(play, this->actor.textId, NULL);
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_003FF8, 0, -12.0f, 0);
    } else if (*unk_3CC == ((kREG(19) + 20.0f) + 1.0f)) {
        if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
            *unk_3CC += 1.0f;
            Birth_Item2_In_En_Zl3(this, play);
            Birth_Item2_In_En_Zl3(this, play);
            Birth_Item2_In_En_Zl3(this, play);
            Birth_Item2_In_En_Zl3(this, play);
            Birth_Item2_In_En_Zl3(this, play);
        }
    } else {
        if (*unk_3CC >= kREG(20) + 30.0f) {
            this->action = 28;
            restartCameraStoped(GET_ACTIVE_CAM(play));
            En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_009FBC, 0, -12.0f, 0);
            *unk_3CC = 0.0f;
        } else {
            *unk_3CC += 1.0f;
        }
    }
}

void En_Zl3_inEscape_Setup_Pray(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_0038C0, 2, -8.0f, 0);
    this->action = 37;
    this->unk_36C = 1;
}

void En_Zl3_inEscape_Setup_Handdown(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_0038C0, 2, -8.0f, 1);
    this->action = 38;
    this->unk_374 = 1;
}

void En_Zl3_inEscape_Setup_Runaway(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_009BE4, 0, -8.0f, 0);
    En_Zl3_inEscape_InitPosParam(this, play);
    this->action = 39;
}

s32 En_Zl3_inEscape_check_WalkToStand(EnZl3* this, PlayState* play) {
    if (En_Zl3_inEscape_Check_move_time(this)) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_009FBC, 0, -8.0f, 0);
        this->action = 28;
        this->unk_3D0 = 0;
        return 1;
    }
    return 0;
}

s32 En_Zl3_inEscape_check_WalkToAnxious(EnZl3* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Player* player = GET_PLAYER(play);
        s8 invincibilityTimer = player->invincibilityTimer;

        if ((invincibilityTimer > 0) || (player->fallDistance >= 51)) {
            En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_007664, 2, -11.0f, 0);
            this->action = 35;
            En_Zl3_inEscape_Set_PainSound(this);
            return 1;
        }
    }
    return 0;
}

void En_Zl3_inEscape_check_WalkToBranch(EnZl3* this, PlayState* play) {
    if (!En_Zl3_inEscape_check_WalkToStand(this, play)) {
        En_Zl3_inEscape_check_WalkToAnxious(this, play);
    }
}

void En_Zl3_inEscape_check_AnxiousToWalk(EnZl3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s8 invincibilityTimer = player->invincibilityTimer;

    if ((invincibilityTimer <= 0) && (player->fallDistance <= 50)) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_009BE4, 0, -11.0f, 0);
        this->action = 34;
    }
}

void En_Zl3_inEscape_SetDemo(EnZl3* this, PlayState* play) {
    f32* unk_2EC = &this->unk_2EC;

    *unk_2EC += 1.0f;
    if ((*unk_2EC >= kREG(7) + 24.0f) && (this->unk_36C == 0)) {
        Birth_Wipe4_In_En_Zl3_inEscape(this, play);
        En_Zl3_inEscape_Setup_Pray(this, play);
    } else if ((*unk_2EC >= kREG(8) + 50.0f) && (this->unk_370 == 0)) {
        En_Zl3_inEscape_Set_SaveBit(this, play);
        this->unk_370 = 1;
    } else if ((*unk_2EC >= kREG(9) + 56.0f) && (this->unk_374 == 0)) {
        En_Zl3_inEscape_Setup_Handdown(this, play);
    } else if (*unk_2EC >= kREG(10) + 82.0f) {
        En_Zl3_inEscape_Setup_Runaway(this, play);
    }
}

void En_Zl3_inEscape_Control_Runaway(EnZl3* this, PlayState* play) {
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
    temp_f0 = get_parcent_forAccelBrake(unk_346, 0, unk_344, 3, 0);

    thisPos->x = unk_348->x + (temp_f0 * (unk_354->x - unk_348->x));
    thisPos->y = (unk_348->y + (temp_f0 * (unk_354->y - unk_348->y))) + this->unk_360;
    thisPos->z = unk_348->z + (temp_f0 * (unk_354->z - unk_348->z));

    if ((unk_344 - sp28) >= 0) {
        this->alpha = ((f32)(unk_346 - unk_344) / (kREG(11) + 2)) * 255.0f;
        this->drawConfig = 2;
    }

    if ((unk_346 - unk_344) <= 0) {
        Actor_delete(&this->actor);
    }
}

void En_Zl3_inEscape_main_start(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inEscape_check_StartToGreet(this, play);
}

void En_Zl3_inEscape_main_greet(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inEscape_check_GreetToTurn(this, play);
}

void En_Zl3_inEscape_main_turn(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    Actor_world_to_eye(&this->actor, 60.0f);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inEscape_check_TurnToWalk(this, play);
}

void En_Zl3_inEscape_main_stand(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_Calc_turn_link(this, play);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inEscape_check_StandToBranch(this, play);
    En_Zl3_BGcheck_bitclr(this);
}

void En_Zl3_inEscape_main_encourage(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_SetAngle_forLink2(this);
    En_Zl3_Calc_turn_link(this, play);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inEscape_check_EncourageToStand(this, play);
}

void En_Zl3_inEscape_main_avert(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_SetAngle_forLink(this);
    En_Zl3_Calc_turn_front(this);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inEscape_check_AvertToStand(this, play);
}

void En_Zl3_inEscape_main_surprise(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_SetAngle_forLink(this);
    En_Zl3_Calc_turn_front(this);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inEscape_ControlDemo_forSurprise(this, play);
}

void En_Zl3_inEscape_main_prizon(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_SetAngle_forLink(this);
    En_Zl3_Calc_turn_front(this);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inEscape_ControlDemo_forPrizon(this, play);
}

void En_Zl3_inEscape_main_thanks(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_SetAngle_forLink(this);
    En_Zl3_Calc_turn_front(this);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inEscape_ControlDemo_forThanks(this, play);
}

void En_Zl3_inEscape_main_walk(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_Calc_turn_front(this);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_inEscape_Set_RunSound(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inEscape_MoveSetPos(this, play);
    En_Zl3_inEscape_ChangeAngle(this, play);
    En_Zl3_inEscape_check_WalkToBranch(this, play);
}

void En_Zl3_inEscape_main_anxious(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_Calc_turn_front(this);
    En_Zl3_SetAngle_forLink2(this);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inEscape_check_AnxiousToWalk(this, play);
}

void En_Zl3_inEscape_main_keep(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_Calc_turn_front(this);
    En_Zl3_SetAngle_forNextPath(this);
    Actor_world_to_eye(&this->actor, 60.0f);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inEscape_SetDemo(this, play);
}

void En_Zl3_inEscape_main_pray(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_Calc_turn_front(this);
    En_Zl3_SetAngle_forNextPath(this);
    Actor_world_to_eye(&this->actor, 60.0f);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_inEscape_ChengAnime_StopToRepeat(this, En_Zl3_Animation_Base(this), &gZelda2Anime2Anim_003D20);
    En_Zl3_inEscape_Set_OpenDoorSound(this);
    En_Zl3_inEscape_SetDemo(this, play);
}

void En_Zl3_inEscape_main_handdown(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_Calc_turn_front(this);
    En_Zl3_SetAngle_forNextPath(this);
    Actor_world_to_eye(&this->actor, 60.0f);
    En_Zl3_Excute_Corect_forStand(this, play);
    En_Zl3_BGcheck(this, play);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_inEscape_ChengAnime_StopToRepeat(this, En_Zl3_Animation_Base(this), &gZelda2Anime2Anim_009FBC);
    En_Zl3_inEscape_SetDemo(this, play);
}

void En_Zl3_inEscape_main_runaway(EnZl3* this, PlayState* play) {
    En_Zl3_Change_Bank(this, play);
    En_Zl3_Calc_turn_front(this);
    En_Zl3_SetAngle_forNextPath(this);
    En_Zl3_BGcheck(this, play);
    En_Zl3_inEscape_Set_RunSound(this, play);
    Actor_world_to_eye(&this->actor, 60.0f);
    En_Zl3_set_eye_pattern(this);
    En_Zl3_Animation_Base(this);
    En_Zl3_inEscape_Control_Runaway(this, play);
}

s32 En_Zl3_DrawHeadAngle(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx, Gfx** gfx) {
    if (limbIndex == 14) {
        Mtx* mtx = GRAPH_ALLOC(play->state.gfxCtx, sizeof(Mtx) * 7);
        EnZl3* this = (EnZl3*)thisx;
        Vec3s* headRot = &this->interactInfo.headRot;

        gSPSegment(gfx[0]++, 0x0C, mtx);

        rot->x += headRot->y;
        rot->z += headRot->x;
        Matrix_push();
        Matrix_translate(pos->x, pos->y, pos->z, MTXMODE_APPLY);
        Matrix_rotateXYZ(rot->x, rot->y, rot->z, MTXMODE_APPLY);
        Matrix_push();
        Matrix_translate(174.0f, -317.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&mtx[0], "../z_en_zl3_inEscape.c", 2471);
        Matrix_translate(-410.0f, -184.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&mtx[1], "../z_en_zl3_inEscape.c", 2474);
        Matrix_translate(-1019.0f, -26.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&mtx[2], "../z_en_zl3_inEscape.c", 2477);
        Matrix_pull();
        Matrix_push();
        Matrix_translate(40.0f, 264.0f, 386.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&mtx[3], "../z_en_zl3_inEscape.c", 2483);
        Matrix_translate(-446.0f, -52.0f, 84.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&mtx[4], "../z_en_zl3_inEscape.c", 2486);
        Matrix_pull();
        Matrix_push();
        Matrix_translate(40.0f, 264.0f, -386.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&mtx[5], "../z_en_zl3_inEscape.c", 2492);
        Matrix_translate(-446.0f, -52.0f, -84.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&mtx[6], "../z_en_zl3_inEscape.c", 2495);
        Matrix_pull();
        Matrix_pull();
    }
    return false;
}

s32 En_Zl3_inEscape_Check_Time(EnZl3* this, PlayState* play) {
    s32 cond = Actor_Environment_sw_Check(play, 0x37) &&
               ((play->sceneId == SCENE_GANON_BOSS) || (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) ||
                (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) ||
                (play->sceneId == SCENE_INSIDE_GANONS_CASTLE_COLLAPSE));

    if (cond) {
        u8 spawn = play->spawn;

        if ((En_Zl3_GetUpper_arg_data(this) == 0x20) && (spawn == 0) &&
            ((z_common_data.subTimerSeconds <= 0) || (z_common_data.subTimerState == SUBTIMER_STATE_OFF))) {
            return 1;
        }
    }
    return 0;
}

s32 En_Zl3_inEscape_Check_TimeOnly(EnZl3* this, PlayState* play) {
    s32 cond = Actor_Environment_sw_Check(play, 0x37) &&
               ((play->sceneId == SCENE_GANON_BOSS) || (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) ||
                (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) ||
                (play->sceneId == SCENE_INSIDE_GANONS_CASTLE_COLLAPSE));

    if (cond) {
        u8 spawn = play->spawn;

        if ((En_Zl3_GetUpper_arg_data(this) == 0x20) && (spawn == 0) && (z_common_data.subTimerSeconds <= 0)) {
            return 1;
        }
    }
    return 0;
}

void En_Zl3_Actor_inEscape_Init(EnZl3* this, PlayState* play) {
    s16 newRotY;

    if (En_Zl3_inEscape_Check_Time(this, play) || (!En_Zl3_inEscape_Check_SaveBit(this, play) && En_Zl3_inEscape_Check_LinkStart(this, play))) {
        En_Zl3_Change_Anime(this, &gZelda2Anime2Anim_009FBC, 0, 0.0f, 0);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        En_Zl3_Init_path_info(this, play);
        newRotY = En_Zl3_Get_next_path_angle(this);
        this->actor.shape.rot.y = newRotY;
        this->actor.world.rot.y = newRotY;
        this->unk_3C4 = this->actor.world.rot.z;
        this->actor.shape.rot.z = 0;
        this->actor.world.rot.z = this->actor.shape.rot.z;
        this->action = 28;
        this->drawConfig = 1;
    } else {
        Actor_delete(&this->actor);
    }

    if (En_Zl3_inEscape_Check_Time(this, play) != 0) {
        total_event_timer_set(180);
        En_Zl3_Set_BGMStart();
        z_common_data.healthAccumulator = 320;
        magic_mode_check(play);
        if (Actor_Environment_sw_Check(play, 0x20)) {
            Actor_Environment_sw_Off(play, 0x20);
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_BG_ZG, -144.0f, 3544.0f, -43.0f, 0, 0x2000, 0, 0x2000);
        }
        Actor_Environment_sw_Off(play, 0x21);
        Actor_Environment_sw_Off(play, 0x22);
        Actor_Environment_sw_Off(play, 0x23);
        Actor_Environment_sw_Off(play, 0x24);
        Actor_Environment_sw_Off(play, 0x25);
        Actor_Environment_sw_Off(play, 0x26);
        Actor_Environment_sw_Off(play, 0x27);
        Actor_Environment_sw_Off(play, 0x28);
        Actor_Environment_sw_Off(play, 0x29);
        Actor_Environment_sw_Off(play, 0x2A);
    }

    if (En_Zl3_GetUpper_arg_data(this) == 0x20) {
        s32 cond;

        Birth_Eg_In_En_Zl3(this, play);
        cond = Actor_Environment_sw_Check(play, 0x37) &&
               ((play->sceneId == SCENE_GANON_BOSS) || (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) ||
                (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) ||
                (play->sceneId == SCENE_INSIDE_GANONS_CASTLE_COLLAPSE));
        if (cond) {
            Birth_En_River_Sound_In_En_Zl3(this, play);
        }
    }
}
