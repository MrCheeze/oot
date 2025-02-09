void Birth_Door_Warp1_In_En_Ru1_Bdanboss(EnRu1* this, PlayState* play) {
    s32 pad;
    Vec3f* pos;
    s16 yawTowardsPlayer;
    f32 spawnX;
    f32 spawnY;
    f32 spawnZ;
    s32 pad2[2];

    yawTowardsPlayer = this->actor.yawTowardsPlayer;
    pos = &this->actor.world.pos;
    spawnX = ((kREG(1) + 12.0f) * sin_s(yawTowardsPlayer)) + pos->x;
    spawnY = pos->y;
    spawnZ = ((kREG(1) + 12.0f) * cos_s(yawTowardsPlayer)) + pos->z;
    this->blueWarp = (DoorWarp1*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, spawnX,
                                                    spawnY, spawnZ, 0, yawTowardsPlayer, 0, WARP_BLUE_RUTO);
}

void En_Ru1_Actor_Bdanboss_Init(EnRu1* this, PlayState* play) {
    En_Ru1_Change_Anime(this, &gRutoChildWaitHandsOnHipsAnim, 0, 0, 0);
    this->action = 15;
    this->actor.shape.yOffset = -10000.0f;
    En_Ru1_set_eye_Num(this, 5);
    En_Ru1_set_mouth_Num(this, 2);
}

void En_Ru1_Bdanboss_Movement_Up(EnRu1* this, PlayState* play) {
    this->actor.shape.yOffset += (250.0f / 3.0f);
}

void En_Ru1_Bdanboss_Movement_Fly(EnRu1* this, PlayState* play) {
    s32 pad2;
    Player* player = GET_PLAYER(play);
    Vec3f* playerPos = &player->actor.world.pos;
    s16 shapeRotY = player->actor.shape.rot.y;
    s32 pad;
    f32 unk_27C = this->unk_27C;
    Vec3f* pos = &this->actor.world.pos;

    pos->x = (sin_s(shapeRotY) * unk_27C) + playerPos->x;
    pos->y = playerPos->y;
    pos->z = (cos_s(shapeRotY) * unk_27C) + playerPos->z;
}

void En_Ru1_SetStartPosLink(EnRu1* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    Vec3f* player_unk_450 = &player->unk_450;
    Vec3f* pos = &this->actor.world.pos;
    s16 shapeRotY = this->actor.shape.rot.y;

    player_unk_450->x = ((kREG(2) + 30.0f) * sin_s(shapeRotY)) + pos->x;
    player_unk_450->z = ((kREG(2) + 30.0f) * cos_s(shapeRotY)) + pos->z;
}

s32 En_Ru1_correctLink(EnRu1* this, PlayState* play) {
    s16* shapeRotY;
    Player* player = GET_PLAYER(play);
    Player* otherPlayer;
    s16 temp_f16;
    f32 temp1;
    f32 temp2;
    s32 pad2[5];

    this->unk_26C += 1.0f;
    if ((player->actor.speed == 0.0f) && (this->unk_26C >= 3.0f)) {
        otherPlayer = GET_PLAYER(play);
        player->actor.world.pos.x = otherPlayer->unk_450.x;
        player->actor.world.pos.y = otherPlayer->unk_450.y;
        player->actor.world.pos.z = otherPlayer->unk_450.z;
        shapeRotY = &player->actor.shape.rot.y;
        temp1 = this->actor.world.pos.x - player->actor.world.pos.x;
        temp2 = this->actor.world.pos.z - player->actor.world.pos.z;
        temp_f16 = RAD_TO_BINANG(fatan2(temp1, temp2));
        if (*shapeRotY != temp_f16) {
            add_calc_short_angle2(shapeRotY, temp_f16, 0x14, 0x1838, 0x64);
            player->actor.world.rot.y = *shapeRotY;
        } else {
            return true;
        }
    }
    return false;
}

s32 En_Ru1_Bdanboss_CheckContact_Warp1_door(EnRu1* this, s32 state) {
    if (this->blueWarp != NULL && this->blueWarp->rutoWarpState == state) {
        return true;
    }
    return false;
}

void En_Ru1_Bdanboss_SetContact_Warp1_door(EnRu1* this, s32 state) {
    if (this->blueWarp != NULL) {
        this->blueWarp->rutoWarpState = state;
    }
}

void En_Ru1_Bdanboss_check_WaitToHide(EnRu1* this, PlayState* play) {
    this->action = 16;
}

void En_Ru1_Bdanboss_check_HideToUp(EnRu1* this, PlayState* play) {
    this->action = 17;
    this->drawConfig = 1;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    Birth_Door_Warp1_In_En_Ru1_Bdanboss(this, play);
}

void En_Ru1_Bdanboss_check_UpToIrritate(EnRu1* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->action = 18;
        this->actor.shape.yOffset = 0.0f;
        En_Ru1_Bdanboss_SetContact_Warp1_door(this, WARP_BLUE_RUTO_STATE_READY);
    }
}

void En_Ru1_Bdanboss_check_IrritateToCorrect(EnRu1* this, PlayState* play) {
    if (En_Ru1_Bdanboss_CheckContact_Warp1_door(this, WARP_BLUE_RUTO_STATE_ENTERED)) {
        this->action = 0x13;
        this->unk_26C = 0.0f;
        En_Ru1_SetStartPosLink(this, play);
    }
}

void En_Ru1_Bdanboss_check_CorrectToGreet(EnRu1* this, s32 cond) {
    if (cond) {
        Skeleton_Info2_init(&this->skelAnime, &gRutoChildTransitionHandsOnHipToCrossArmsAndLegsAnim, 1.0f, 0,
                         Si2_anime_end_frame(&gRutoChildTransitionHandsOnHipToCrossArmsAndLegsAnim), ANIMMODE_ONCE,
                         -8.0f);
        this->action = 20;
        En_Ru1_Bdanboss_SetContact_Warp1_door(this, WARP_BLUE_RUTO_STATE_3);
    }
}

void En_Ru1_Bdanboss_check_GreetToFly(EnRu1* this, UNK_TYPE arg1) {
    if (En_Ru1_Bdanboss_CheckContact_Warp1_door(this, WARP_BLUE_RUTO_STATE_TALKING)) {
        if (arg1 != 0) {
            Skeleton_Info2_init(&this->skelAnime, &gRutoChildWaitSittingAnim, 1.0f, 0,
                             Si2_anime_end_frame(&gRutoChildWaitSittingAnim), ANIMMODE_LOOP, -8.0f);
        }
    } else if (En_Ru1_Bdanboss_CheckContact_Warp1_door(this, WARP_BLUE_RUTO_STATE_WARPING)) {
        Skeleton_Info2_init(&this->skelAnime, &gRutoChildWaitInBlueWarpAnim, 1.0f, 0,
                         Si2_anime_end_frame(&gRutoChildWaitInBlueWarpAnim), ANIMMODE_ONCE, -8.0f);
        this->action = 21;
        this->unk_27C = this->actor.xzDistToPlayer;
    }
}

void En_Ru1_Bdanboss_Actor_main_wait(EnRu1* this, PlayState* play) {
    En_Ru1_Bdanboss_check_WaitToHide(this, play);
}

void En_Ru1_Bdanboss_Actor_main_hide(EnRu1* this, PlayState* play) {
    En_Ru1_Bdanboss_check_HideToUp(this, play);
}

void En_Ru1_Bdanboss_Actor_main_up(EnRu1* this, PlayState* play) {
    En_Ru1_Bdanboss_Movement_Up(this, play);
    En_Ru1_Animation_Base(this);
    En_Ru1_Bdanboss_check_UpToIrritate(this);
}

void En_Ru1_Bdanboss_Actor_main_irritate(EnRu1* this, PlayState* play) {
    En_Ru1_Animation_Base(this);
    En_Ru1_Bdanboss_check_IrritateToCorrect(this, play);
}

void En_Ru1_Bdanboss_Actor_main_correct(EnRu1* this, PlayState* play) {
    En_Ru1_Animation_Base(this);
    En_Ru1_Bdanboss_check_CorrectToGreet(this, En_Ru1_correctLink(this, play));
}

void En_Ru1_Bdanboss_Actor_main_greet(EnRu1* this, PlayState* play) {
    En_Ru1_BGcheck(this, play);
    En_Ru1_Bdanboss_check_GreetToFly(this, En_Ru1_Animation_Base(this));
}

void En_Ru1_Bdanboss_Actor_main_fly(EnRu1* this, PlayState* play) {
    En_Ru1_Bdanboss_Movement_Fly(this, play);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Animation_Base(this);
}
