void En_Ru1_Actor_Option_Init(EnRu1* this, PlayState* play) {
    if (GET_INFTABLE(INFTABLE_141) && !GET_INFTABLE(INFTABLE_145) && !GET_INFTABLE(INFTABLE_140) &&
        !GET_INFTABLE(INFTABLE_147)) {
        if (!En_Ru1_Search_Ru1(this, play)) {
            s8 actorRoom;

            En_Ru1_Change_Anime(this, &gRutoChildWait2Anim, 0, 0, 0);
            actorRoom = this->actor.room;
            this->action = 22;
            this->actor.room = -1;
            this->roomNum1 = actorRoom;
            this->roomNum3 = actorRoom;
            this->roomNum2 = actorRoom;
        } else {
            Actor_delete(&this->actor);
        }
    } else {
        Actor_delete(&this->actor);
    }
}

void En_Ru1_Option_Set_BoundSound(EnRu1* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_EV_LAND_DIRT);
}

void En_Ru1_Option_Set_HangSound_forLink(EnRu1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Nai_FxFlagEntry(NA_SE_PL_PULL_UP_RUTO, &player->actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_RT_LIFT);
}

void En_Ru1_Option_Set_ThrowSound(EnRu1* this) {
    if (this->actor.speed != 0.0f) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_RT_THROW);
    }
}

void En_Ru1_Option_Set_CrashSound(EnRu1* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_RT_CRASH);
}

void En_Ru1_Option_Set_UnballanceSound(EnRu1* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_RT_UNBALLANCE);
}

void En_Ru1_Option_Set_DiscoverSound(EnRu1* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_VO_RT_DISCOVER);
}

s32 En_Ru1_Option_check_Alone(EnRu1* this, PlayState* play) {
    s8 curRoomNum = play->roomCtx.curRoom.num;

    if (this->roomNum2 != curRoomNum) {
        Actor_delete(&this->actor);
        return false;
    } else if (((this->roomNum1 != curRoomNum) || (this->roomNum2 != curRoomNum)) &&
               (this->actor.depthInWater > kREG(16) + 50.0f) && (this->action != 33)) {
        this->action = 33;
        this->drawConfig = 2;
        this->alpha = 0xFF;
        this->unk_2A4 = 0.0f;
    }
    return true;
}

void En_Ru1_Option_reset_Alone(EnRu1* this, PlayState* play) {
    s8 curRoomNum = play->roomCtx.curRoom.num;

    this->roomNum2 = curRoomNum;
    this->unk_288 = 0.0f;
}

void En_Ru1_Option_check_Room02(PlayState* play) {
    s8 curRoomNum;

    if (!GET_INFTABLE(INFTABLE_147)) {
        curRoomNum = play->roomCtx.curRoom.num;
        if (curRoomNum == 2) {
            SET_INFTABLE(INFTABLE_147);
        }
    }
}

void En_Ru1_Option_check_Alpha(EnRu1* this, PlayState* play) {
    if (En_Ru1_Option_check_Alone(this, play)) {
        s32 pad;

        this->unk_2A4 += 1.0f;
        if (this->unk_2A4 < 20.0f) {
            u32 temp_v0 = ((20.0f - this->unk_2A4) * 255.0f) / 20.0f;

            this->alpha = temp_v0;
            this->actor.shape.shadowAlpha = temp_v0;
        } else {
            Actor_delete(&this->actor);
        }
    }
}

void En_Ru1_Calc_turn_front(EnRu1* this) {
    s32 pad[2];
    Vec3s* headRot;
    Vec3s* torsoRot;

    headRot = &this->interactInfo.headRot;
    add_calc_short_angle2(&headRot->x, 0, 0x14, 0x1838, 0x64);
    add_calc_short_angle2(&headRot->y, 0, 0x14, 0x1838, 0x64);
    torsoRot = &this->interactInfo.torsoRot;
    add_calc_short_angle2(&torsoRot->x, 0, 0x14, 0x1838, 0x64);
    add_calc_short_angle2(&torsoRot->y, 0, 0x14, 0x1838, 0x64);
}

void En_Ru1_Calc_turn_around(EnRu1* this) {
    s32 temp_hi;
    s16* unk_2AC = &this->unk_2AC;
    s16* headRotY = &this->interactInfo.headRot.y;
    s16* unk_29E = &this->unk_29E;
    s32 pad[2];

    if (DECR(*unk_2AC) == 0) {
        *unk_2AC = get_random_timer(0xA, 0x19);
        temp_hi = *unk_2AC % 5;
        if (temp_hi == 0) {
            this->unk_2B0 = 1;
        } else if (temp_hi == 1) {
            this->unk_2B0 = 2;
        } else {
            this->unk_2B0 = 0;
        }
        *unk_29E = 0;
    }

    if (this->unk_2B0 == 0) {
        add_calc_short_angle2(unk_29E, 0 - *headRotY, 1, 0x190, 0x190);
        add_calc_short_angle2(headRotY, 0, 3, ABS(*unk_29E), 0x64);
    } else if (this->unk_2B0 == 1) {
        add_calc_short_angle2(unk_29E, -0x2AAA - *headRotY, 1, 0x190, 0x190);
        add_calc_short_angle2(headRotY, -0x2AAA, 3, ABS(*unk_29E), 0x64);
    } else {
        add_calc_short_angle2(unk_29E, 0x2AAA - *headRotY, 1, 0x190, 0x190);
        add_calc_short_angle2(headRotY, 0x2AAA, 3, ABS(*unk_29E), 0x64);
    }
}

void En_Ru1_Option_BGcheck_bitclr(EnRu1* this, PlayState* play) {
    DynaPolyActor* dynaPolyActor = DynaPolyInfo_actor_index2pointer(&play->colCtx, this->actor.floorBgId);

    if (dynaPolyActor == NULL || dynaPolyActor->actor.id == ACTOR_EN_BOX) {
        this->actor.bgCheckFlags &= ~(BGCHECKFLAG_GROUND | BGCHECKFLAG_WALL | BGCHECKFLAG_CEILING);
    }
}

void En_Ru1_Option_Reflect_alone_move(EnRu1* this, PlayState* play) {
    f32* velocityY;
    f32* speedXZ;
    f32* gravity;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        DynaPolyActor* dynaPolyActor;

        velocityY = &this->actor.velocity.y;
        dynaPolyActor = DynaPolyInfo_actor_index2pointer(&play->colCtx, this->actor.floorBgId);
        if (*velocityY <= 0.0f) {
            speedXZ = &this->actor.speed;
            if (dynaPolyActor != NULL) {
                if (dynaPolyActor->actor.id != ACTOR_EN_BOX) {
                    *speedXZ = 0.0f;
                }
            } else {
                if (*speedXZ >= (kREG(27) * 0.01f) + 3.0f) {
                    *speedXZ *= (kREG(19) * 0.01f) + 0.8f;
                } else {
                    *speedXZ = 0.0f;
                }
            }
            gravity = &this->actor.gravity;
            if (dynaPolyActor != NULL) {
                if (dynaPolyActor->actor.id != ACTOR_EN_BOX) {
                    *velocityY = 0.0f;
                    this->actor.minVelocityY = 0.0f;
                    *gravity = 0.0f;
                } else {
                    *velocityY *= -1.0f;
                }
            } else {
                *velocityY *= -((kREG(20) * 0.01f) + 0.6f);
                if (*velocityY <= -*gravity * ((kREG(20) * 0.01f) + 0.6f)) {
                    *velocityY = 0.0f;
                    this->actor.minVelocityY = 0.0f;
                    *gravity = 0.0f;
                }
            }
            En_Ru1_Option_Set_BoundSound(this);
        }
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) {
        s32 pad;

        speedXZ = &this->actor.speed;
        velocityY = &this->actor.velocity.y;
        if (*speedXZ >= (kREG(27) * 0.01f) + 3.0f) {
            *speedXZ *= (kREG(19) * 0.01f) + 0.8f;
        } else {
            *speedXZ = 0.0f;
        }
        if (*velocityY >= 0.0f) {
            *velocityY *= -((kREG(20) * 0.01f) + 0.6f);
            En_Ru1_Option_Set_BoundSound(this);
        }
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        speedXZ = &this->actor.speed;
        if (*speedXZ != 0.0f) {
            s16 wallYaw;
            s16 rotY;
            s32 temp_a1_2;
            s32 temp_a0;
            s32 phi_v1;

            rotY = this->actor.world.rot.y;
            wallYaw = this->actor.wallYaw;
            temp_a0 = (wallYaw * 2) - rotY;
            temp_a1_2 = temp_a0 + 0x8000;
            if ((s16)((temp_a0 - wallYaw) + 0x8000) >= 0) {
                phi_v1 = (s16)(temp_a1_2 - wallYaw);
            } else {
                phi_v1 = -(s16)(temp_a1_2 - wallYaw);
            }
            if (phi_v1 < 0x4001) {
                if (*speedXZ >= (kREG(27) * 0.01f) + 3.0f) {
                    *speedXZ *= (kREG(21) * 0.01f) + 0.6f;
                } else {
                    *speedXZ = 0.0f;
                }
                this->actor.world.rot.y = temp_a1_2;
                En_Ru1_Option_Set_BoundSound(this);
                En_Ru1_Option_Set_CrashSound(this);
            }
        }
    }
}

void En_Ru1_Option_Accel_alone_move(EnRu1* this, PlayState* play) {
    f32* speedXZ = &this->actor.speed;
    DynaPolyActor* dynaPolyActor = DynaPolyInfo_actor_index2pointer(&play->colCtx, this->actor.floorBgId);

    if (dynaPolyActor != NULL && dynaPolyActor->actor.id == ACTOR_EN_BOX) {
        if (*speedXZ != 0.0f) {
            *speedXZ *= 1.1f;
        } else {
            *speedXZ = 1.0f;
        }
    }
    if (*speedXZ >= (kREG(27) * 0.01f) + 3.0f) {
        *speedXZ *= (kREG(22) * 0.01f) + 0.98f;
    } else {
        *speedXZ = 0.0f;
    }
}

void En_Ru1_Option_Movement_alone_move(EnRu1* this, PlayState* play) {
    En_Ru1_Option_Reflect_alone_move(this, play);
    En_Ru1_Option_Accel_alone_move(this, play);
    Actor_position_moveF(&this->actor);
}

void En_Ru1_Option_Reset_Movement(EnRu1* this) {
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.velocity.z = 0.0f;
    this->actor.speed = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.minVelocityY = 0.0f;
}

void En_Ru1_Option_Movement_alone_float(EnRu1* this) {
    s32 pad;
    f32 sp28;
    f32 sp24;
    EnRu1* thisx = this; // necessary to match

    if (this->unk_350 == 0) {
        if ((this->actor.minVelocityY == 0.0f) && (this->actor.speed == 0.0f)) {
            this->unk_350 = 1;
            En_Ru1_Option_Reset_Movement(this);
            this->unk_35C = 0;
            this->unk_358 = (this->actor.depthInWater - 10.0f) * 0.5f;
            this->unk_354 = this->actor.world.pos.y + thisx->unk_358; // thisx only used here
        } else {
            this->actor.gravity = 0.0f;
            this->actor.minVelocityY *= 0.2f;
            this->actor.velocity.y *= 0.2f;
            if (this->actor.minVelocityY >= -0.1f) {
                this->actor.minVelocityY = 0.0f;
                this->actor.velocity.y = 0.0f;
            }
            this->actor.speed *= 0.5f;
            if (this->actor.speed <= 0.1f) {
                this->actor.speed = 0.0f;
            }
            this->actor.velocity.x = sin_s(this->actor.world.rot.y) * this->actor.speed;
            this->actor.velocity.z = cos_s(this->actor.world.rot.y) * this->actor.speed;
            Actor_position_move(&this->actor);
        }
    } else {
        if (this->unk_350 == 1) {
            if (this->unk_358 <= 1.0f) {
                En_Ru1_Option_Reset_Movement(this);
                this->unk_350 = 2;
                this->unk_360 = 0.0f;
            } else {
                f32 temp_f10;

                sp28 = this->unk_358;
                sp24 = this->unk_354;
                temp_f10 = cos_s(this->unk_35C) * -sp28;
                this->actor.world.pos.y = temp_f10 + sp24;
                this->unk_35C += 0x3E8;
                this->unk_358 *= 0.95f;
            }
        } else {
            this->unk_360 += 1.0f;
            if (this->unk_360 > 0.0f) {
                this->unk_350 = 3;
            }
        }
    }
}

s32 En_Ru1_Option_SetTalk(EnRu1* this, PlayState* play) {
    if (!Actor_talk_check(&this->actor, play)) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        if (GET_INFTABLE(INFTABLE_143)) {
            this->actor.textId = 0x404E;
            Actor_talk_request(&this->actor, play);
        } else if (GET_INFTABLE(INFTABLE_142)) {
            this->actor.textId = 0x404D;
            Actor_talk_request(&this->actor, play);
        } else {
            this->actor.textId = 0x404C;
            Actor_talk_request(&this->actor, play);
        }
        return false;
    }
    return true;
}

void En_Ru1_Option_check_RideSwitch(EnRu1* this, PlayState* play) {
    DynaPolyActor* dynaPolyActor;
    s32 floorBgId;

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.floorBgId != BGCHECK_SCENE)) {
        floorBgId = this->actor.floorBgId;
        dynaPolyActor = DynaPolyInfo_actor_index2pointer(&play->colCtx, floorBgId);
        if ((dynaPolyActor != NULL) && (dynaPolyActor->actor.id == ACTOR_BG_BDAN_SWITCH)) {
            if (PARAMS_GET_U(dynaPolyActor->actor.params, 8, 6) == 0x38) {
                SET_INFTABLE(INFTABLE_140);
                return;
            }
        }
    }
    CLEAR_INFTABLE(INFTABLE_140);
}

s32 En_Ru1_Option_check_RideBdan_object(EnRu1* this, PlayState* play) {
    s32 pad[2];
    CollisionContext* colCtx;
    DynaPolyActor* dynaPolyActor;
    s32 floorBgId;

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && this->actor.floorBgId != BGCHECK_SCENE) {
        colCtx = &play->colCtx;
        floorBgId = this->actor.floorBgId; // necessary match, can't move this out of this block unfortunately
        dynaPolyActor = DynaPolyInfo_actor_index2pointer(colCtx, floorBgId);
        if (dynaPolyActor != NULL && dynaPolyActor->actor.id == ACTOR_BG_BDAN_OBJECTS &&
            dynaPolyActor->actor.params == 0 && !player_demo_check(play) && play->msgCtx.msgLength == 0) {
            En_Ru1_Option_Reset_Movement(this);
            play->csCtx.script = En_Ru1_Data1_In_BdanDemoOption;
            z_common_data.cutsceneTrigger = 1;
            this->action = 36;
            this->drawConfig = 0;
            this->unk_28C = (BgBdanObjects*)dynaPolyActor;
            this->actor.shape.shadowAlpha = 0;
            return true;
        }
    }
    return false;
}

void En_Ru1_Option_check_Alone_stopToCurry(EnRu1* this, PlayState* play) {
    s8 curRoomNum;

    if (Actor_carry_check(&this->actor, play)) {
        curRoomNum = play->roomCtx.curRoom.num;
        this->roomNum3 = curRoomNum;
        this->action = 31;
        En_Ru1_Option_Set_HangSound_forLink(this, play);
    } else if (!En_Ru1_Option_check_RideBdan_object(this, play) && !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        s32 pad;

        this->actor.minVelocityY = -((kREG(24) * 0.01f) + 6.8f);
        this->actor.gravity = -((kREG(23) * 0.01f) + 1.3f);
        this->action = 28;
    }
}

void En_Ru1_Option_check_Alone_moveToAlone_stop(EnRu1* this, PlayState* play) {
    if (!En_Ru1_Option_check_RideBdan_object(this, play)) {
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.speed == 0.0f) &&
            (this->actor.minVelocityY == 0.0f)) {
            s32 pad;

            En_Ru1_Option_Reset_Movement(this);
            Actor_carry_request(&this->actor, play);
            this->action = 27;
            En_Ru1_Option_Reset_Corect_forOption(this);
            return;
        }

        if (this->actor.depthInWater > 0.0f) {
            this->action = 29;
            this->unk_350 = 0;
        }
    }
}

void En_Ru1_Option_check_CheerToCurry(EnRu1* this, PlayState* play) {
    s32 pad[2];
    s8 curRoomNum = play->roomCtx.curRoom.num;

    if (En_Ru1_Check_EndDemoMode(play)) {
        Skeleton_Info2_init(&this->skelAnime, &gRutoChildSittingAnim, 1.0f, 0,
                         Si2_anime_end_frame(&gRutoChildSittingAnim), ANIMMODE_LOOP, -8.0f);
        SET_INFTABLE(INFTABLE_144);
        this->action = 31;
    }
    this->roomNum3 = curRoomNum;
}

s32 En_Ru1_Option_check_CurryToCheer(EnRu1* this, PlayState* play) {
    s32 pad;
    s8 curRoomNum = play->roomCtx.curRoom.num;

    if (!GET_INFTABLE(INFTABLE_144) && (En_Ru1_Search_Juwel_Actor(play) != NULL)) {
        if (!player_demo_check(play)) {
            Skeleton_Info2_init(&this->skelAnime, &gRutoChildSeesSapphireAnim, 1.0f, 0,
                             Si2_anime_end_frame(&gRutoChildSquirmAnim), ANIMMODE_LOOP, -8.0f);
            En_Ru1_Option_Set_DiscoverSound(this);
            this->action = 34;
            this->unk_26C = 0.0f;
            play->csCtx.script = En_Ru1_Data1_In_BdanDemoFind;
            z_common_data.cutsceneTrigger = 1;
        }
        this->roomNum3 = curRoomNum;
        return true;
    }
    this->roomNum3 = curRoomNum;
    return false;
}

void En_Ru1_Option_check_CurryToAlone_move(EnRu1* this, PlayState* play) {
    s32 pad[9];
    Player* player;
    f32* unk_370 = &this->unk_370;

    if (Actor_carry_end_check(&this->actor, play)) {
        f32 frameCount = Si2_anime_end_frame(&gRutoChildSittingAnim);

        Skeleton_Info2_init(&this->skelAnime, &gRutoChildSittingAnim, 1.0f, 0, frameCount, ANIMMODE_LOOP, -8.0f);
        En_Ru1_Option_reset_Alone(this, play);
        this->actor.speed *= (kREG(25) * 0.01f) + 1.0f;
        this->actor.velocity.y *= (kREG(26) * 0.01f) + 1.0f;
        this->actor.minVelocityY = -((kREG(24) * 0.01f) + 6.8f);
        this->actor.gravity = -((kREG(23) * 0.01f) + 1.3f);
        En_Ru1_Option_Set_ThrowSound(this);
        this->action = 28;
        *unk_370 = 0.0f;
    } else if (En_Ru1_Option_check_CurryToCheer(this, play)) {
        s32 pad;

        *unk_370 = 0.0f;
    } else {
        player = GET_PLAYER(play);
        if (player->stateFlags2 & PLAYER_STATE2_IDLE_FIDGET) {
            this->unk_370 += 1.0f;
            if (this->action != 32) {
                if (*unk_370 > 30.0f) {
                    if (get_random_timer(0, 3) == 0) {
                        f32 frameCount = Si2_anime_end_frame(&gRutoChildSquirmAnim);

                        Skeleton_Info2_init(&this->skelAnime, &gRutoChildSquirmAnim, 1.0f, 0, frameCount, ANIMMODE_LOOP,
                                         -8.0f);
                        En_Ru1_Option_Set_UnballanceSound(this);
                        this->action = 32;
                    }
                    *unk_370 = 0.0f;
                }
            } else {
                if (*unk_370 > 50.0f) {
                    f32 frameCount = Si2_anime_end_frame(&gRutoChildSittingAnim);

                    Skeleton_Info2_init(&this->skelAnime, &gRutoChildSittingAnim, 1.0f, 0, frameCount, ANIMMODE_LOOP,
                                     -8.0f);
                    this->action = 31;
                    *unk_370 = 0.0f;
                }
            }
        } else {
            f32 frameCount = Si2_anime_end_frame(&gRutoChildSittingAnim);

            Skeleton_Info2_init(&this->skelAnime, &gRutoChildSittingAnim, 1.0f, 0, frameCount, ANIMMODE_LOOP, -8.0f);
            *unk_370 = 0.0f;
        }
    }
}

s32 En_Ru1_Option_check_Alone_sinkToAlone_stop(EnRu1* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        s32 pad;

        En_Ru1_Option_Reset_Movement(this);
        Actor_carry_request(&this->actor, play);
        this->action = 27;
        En_Ru1_Option_Reset_Corect_forOption(this);
        return true;
    }
    return false;
}

void En_Ru1_Option_check_Alone_floatToBranch(EnRu1* this, PlayState* play) {
    if ((En_Ru1_Option_check_Alone_sinkToAlone_stop(this, play) == 0) && (this->unk_350 == 3)) {
        this->action = 30;
        En_Ru1_Option_Reset_Movement(this);
        this->actor.gravity = -0.1f;
        this->actor.minVelocityY = -((kREG(18) * 0.1f) + 0.7f);
    }
}

void En_Ru1_Option_check_Alone_stopByLink(EnRu1* this, PlayState* play) {
    Actor_carry_request(&this->actor, play);
}

void En_Ru1_Option_Actor_main_alone_stop(EnRu1* this, PlayState* play) {
    En_Ru1_Calc_turn_front(this);
    En_Ru1_Option_Excute_Corect_forOption(this, play);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_Option_check_Alone_stopByLink(this, play);
    En_Ru1_Option_check_Alone_stopToCurry(this, play);
    En_Ru1_Option_check_Alone(this, play);
    En_Ru1_Option_BGcheck_bitclr(this, play);
}

void En_Ru1_Option_Actor_main_alone_move(EnRu1* this, PlayState* play) {
    En_Ru1_Calc_turn_front(this);
    En_Ru1_Option_Excute_Attck_forOption(this, play);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Option_check_RideSwitch(this, play);
    En_Ru1_Option_Movement_alone_move(this, play);
    En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_Option_check_Alone_moveToAlone_stop(this, play);
    En_Ru1_Option_check_Alone(this, play);
    En_Ru1_Option_BGcheck_bitclr(this, play);
}

void En_Ru1_Option_Actor_main_alone_float(EnRu1* this, PlayState* play) {
    En_Ru1_Calc_turn_front(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Option_Movement_alone_float(this);
    En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_Option_check_Alone_floatToBranch(this, play);
    En_Ru1_Option_check_Alone(this, play);
}

void En_Ru1_Option_Actor_main_alone_sink(EnRu1* this, PlayState* play) {
    En_Ru1_Calc_turn_front(this);
    En_Ru1_BGcheck(this, play);
    Actor_position_moveF(&this->actor);
    En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_Option_check_Alone_sinkToAlone_stop(this, play);
    En_Ru1_Option_check_Alone(this, play);
    En_Ru1_Option_BGcheck_bitclr(this, play);
}

void En_Ru1_Option_Actor_main_curry(EnRu1* this, PlayState* play) {
    En_Ru1_Calc_turn_around(this);
    En_Ru1_Animation_Base(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Option_check_RideSwitch(this, play);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_Option_check_Room02(play);
    En_Ru1_Option_check_CurryToAlone_move(this, play);
}

void En_Ru1_Option_Actor_main_strech(EnRu1* this, PlayState* play) {
    En_Ru1_Calc_turn_front(this);
    En_Ru1_Animation_Base(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Option_check_RideSwitch(this, play);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_Option_check_Room02(play);
    En_Ru1_Option_check_CurryToAlone_move(this, play);
}

void En_Ru1_Option_Actor_main_alpha(EnRu1* this, PlayState* play) {
    En_Ru1_Calc_turn_front(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_Option_Movement_alone_move(this, play);
    En_Ru1_Animation_Base(this);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_Option_check_Alpha(this, play);
    En_Ru1_Option_check_Alone(this, play);
}

void En_Ru1_Option_Actor_main_cheer(EnRu1* this, PlayState* play) {
    En_Ru1_Calc_turn_front(this);
    En_Ru1_Animation_Base(this);
    En_Ru1_BGcheck(this, play);
    En_Ru1_set_eye_pattern(this);
    En_Ru1_Option_check_CheerToCurry(this, play);
}

void En_Ru1_Option_Actor_main_hurry(EnRu1* this, PlayState* play) {
}
