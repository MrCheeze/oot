/*
 * File: z_en_door.c
 * Overlay: ovl_En_Door
 * Description: Doors with handles
 */

#include "z_en_door.h"
#include "global.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/gameplay_field_keep/gameplay_field_keep.h"
#include "assets/objects/object_hidan_objects/object_hidan_objects.h"
#include "assets/objects/object_mizu_objects/object_mizu_objects.h"
#include "assets/objects/object_haka_door/object_haka_door.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

#define DOOR_AJAR_SLAM_RANGE 120.0f
#define DOOR_AJAR_OPEN_RANGE (2 * DOOR_AJAR_SLAM_RANGE)

#define DOOR_CHECK_RANGE 40.0f

void En_Door_actor_ct(Actor* thisx, PlayState* play2);
void En_Door_actor_dt(Actor* thisx, PlayState* play);
void En_Door_actor_move(Actor* thisx, PlayState* play);
void En_Door_actor_draw(Actor* thisx, PlayState* play);

static void move_dma_wait(EnDoor* this, PlayState* play);
static void move_wait(EnDoor* this, PlayState* play);
void move_talk_wait(EnDoor* this, PlayState* play);
static void move_talk(EnDoor* this, PlayState* play);
void move_half_wait(EnDoor* this, PlayState* play);
void move_half_open(EnDoor* this, PlayState* play);
void move_half_close(EnDoor* this, PlayState* play);
static void move_demo(EnDoor* this, PlayState* play);

ActorProfile En_Door_Profile = {
    /**/ ACTOR_EN_DOOR,
    /**/ ACTORCAT_DOOR,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnDoor),
    /**/ En_Door_actor_ct,
    /**/ En_Door_actor_dt,
    /**/ En_Door_actor_move,
    /**/ En_Door_actor_draw,
};

typedef struct EnDoorInfo {
    /* 0x00 */ s16 sceneId;
    /* 0x02 */ u8 dListIndex;
    /* 0x04 */ s16 objectId;
} EnDoorInfo;

typedef enum EnDoorDListIndex {
    /* 0 */ DOOR_DL_DEFAULT,
    /* 1 */ DOOR_DL_FIRE_TEMPLE,
    /* 2 */ DOOR_DL_WATER_TEMPLE,
    /* 3 */ DOOR_DL_SHADOW,
    /* 4 */ DOOR_DL_DEFAULT_FIELD_KEEP,
    /* 5 */ DOOR_DL_MAX
} EnDoorDListIndex;

/**
 * Controls which object and display lists to use in a given scene
 */
static EnDoorInfo scene_door_status[] = {
    { SCENE_FIRE_TEMPLE, DOOR_DL_FIRE_TEMPLE, OBJECT_HIDAN_OBJECTS },
    { SCENE_WATER_TEMPLE, DOOR_DL_WATER_TEMPLE, OBJECT_MIZU_OBJECTS },
    { SCENE_SHADOW_TEMPLE, DOOR_DL_SHADOW, OBJECT_HAKA_DOOR },
    { SCENE_BOTTOM_OF_THE_WELL, DOOR_DL_SHADOW, OBJECT_HAKA_DOOR },
    // KEEP objects should remain last and in this order
    { -1, DOOR_DL_DEFAULT, OBJECT_GAMEPLAY_KEEP },
    { -1, DOOR_DL_DEFAULT_FIELD_KEEP, OBJECT_GAMEPLAY_FIELD_KEEP },
};

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_0, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_STOP),
};

static AnimationHeader* door_open_anm[DOOR_OPEN_ANIM_MAX] = {
    &gDoorAdultOpeningLeftAnim,  // DOOR_OPEN_ANIM_ADULT_L
    &gDoorChildOpeningLeftAnim,  // DOOR_OPEN_ANIM_CHILD_L
    &gDoorAdultOpeningRightAnim, // DOOR_OPEN_ANIM_ADULT_R
    &gDoorChildOpeningRightAnim, // DOOR_OPEN_ANIM_CHILD_R
};

static u8 door_open_SE_set_frame[DOOR_OPEN_ANIM_MAX] = {
    25, // DOOR_OPEN_ANIM_ADULT_L
    25, // DOOR_OPEN_ANIM_CHILD_L
    25, // DOOR_OPEN_ANIM_ADULT_R
    25, // DOOR_OPEN_ANIM_CHILD_R
};

static u8 door_close_SE_set_frame[DOOR_OPEN_ANIM_MAX] = {
    60, // DOOR_OPEN_ANIM_ADULT_L
    70, // DOOR_OPEN_ANIM_CHILD_L
    60, // DOOR_OPEN_ANIM_ADULT_R
    70, // DOOR_OPEN_ANIM_CHILD_R
};

static Gfx* shape_type_data[DOOR_DL_MAX][2] = {
    { gDoorLeftDL, gDoorRightDL },                                         // DOOR_DL_DEFAULT
    { gFireTempleDoorWithHandleLeftDL, gFireTempleDoorWithHandleRightDL }, // DOOR_DL_FIRE_TEMPLE
    { gWaterTempleDoorLeftDL, gWaterTempleDoorRightDL },                   // DOOR_DL_WATER_TEMPLE
    { gShadowDoorLeftDL, gShadowDoorRightDL },                             // DOOR_DL_SHADOW
    { gFieldDoorLeftDL, gFieldDoorRightDL },                               // DOOR_DL_DEFAULT_FIELD_KEEP
};

void En_Door_actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnDoor* this = (EnDoor*)thisx;
    EnDoorInfo* objectInfo;
    s32 i;
    s32 objectSlot;
    f32 xOffset;
    f32 zOffset;

    objectInfo = &scene_door_status[0];
    ValueSet_process(&this->actor, value_init);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gDoorSkel, &gDoorAdultOpeningLeftAnim, this->jointTable, this->morphTable,
                   5);
    for (i = 0; i < ARRAY_COUNT(scene_door_status) - 2; i++, objectInfo++) {
        if (play->sceneId == objectInfo->sceneId) {
            break;
        }
    }
    if (i >= ARRAY_COUNT(scene_door_status) - 2 && Object_Exchange_bank_check(&play->objectCtx, OBJECT_GAMEPLAY_FIELD_KEEP) >= 0) {
        objectInfo++;
    }

    this->dListIndex = objectInfo->dListIndex;
    objectSlot = Object_Exchange_bank_check(&play->objectCtx, objectInfo->objectId);
    if (objectSlot < 0) {
        Actor_delete(&this->actor);
        return;
    }

    this->requiredObjectSlot = objectSlot;
    this->dListIndex = objectInfo->dListIndex;
    if (this->actor.objectSlot == this->requiredObjectSlot) {
        move_dma_wait(this, play);
    } else {
        this->actionFunc = move_dma_wait;
    }

    // Double doors
    if (ENDOOR_GET_IS_DOUBLE_DOOR(&this->actor)) {
        EnDoor* other;

        xOffset = cos_s(this->actor.shape.rot.y) * 30.0f;
        zOffset = sin_s(this->actor.shape.rot.y) * 30.0f;
        other = (EnDoor*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_DOOR,
                                            this->actor.world.pos.x + xOffset, this->actor.world.pos.y,
                                            this->actor.world.pos.z - zOffset, 0, this->actor.shape.rot.y + 0x8000, 0,
                                            this->actor.params & ~ENDOOR_PARAMS_IS_DOUBLE_DOOR_MASK);
        if (other != NULL) {
            other->unk_192 = 1;
        }
        this->actor.world.pos.x -= xOffset;
        this->actor.world.pos.z += zOffset;
    }
    Actor_world_to_eye(&this->actor, 70.0f);
}

void En_Door_actor_dt(Actor* thisx, PlayState* play) {
    TransitionActorEntry* transitionEntry;
    EnDoor* this = (EnDoor*)thisx;

    transitionEntry = &play->transitionActors.list[GET_TRANSITION_ACTOR_INDEX(&this->actor)];
    if (transitionEntry->id < 0) {
        transitionEntry->id = -transitionEntry->id;
    }
}

static void move_dma_wait(EnDoor* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        s32 doorType = ENDOOR_GET_TYPE(&this->actor);

        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actor.objectSlot = this->requiredObjectSlot;
        this->actionFunc = move_wait;
        if (doorType == DOOR_EVENING) {
            doorType = (z_common_data.save.dayTime > CLOCK_TIME(18, 0) && z_common_data.save.dayTime < CLOCK_TIME(21, 0))
                           ? DOOR_SCENEEXIT
                           : DOOR_CHECKABLE;
        }
        this->actor.world.rot.y = 0x0000;
        if (doorType == DOOR_LOCKED) {
            if (!Actor_Environment_sw_Check(play, ENDOOR_GET_LOCKED_SWITCH_FLAG(&this->actor))) {
                this->lockTimer = 10;
            }
        } else if (doorType == DOOR_AJAR) {
            Player* player = GET_PLAYER(play);

            if (Actor_search_actor_distanceXZ(&this->actor, &player->actor) > DOOR_AJAR_SLAM_RANGE) {
                this->actionFunc = move_half_wait;
                this->actor.world.rot.y = -0x1800;
            }
        } else if (doorType == DOOR_CHECKABLE) {
            this->actor.textId = ENDOOR_GET_CHECKABLE_TEXT_ID(&this->actor) + 0x0200;
            if (this->actor.textId == 0x0229 && !GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                // Talon's house door. If Talon has not been woken up at Hyrule Castle
                // this door should be openable at any time of day.
                // Note that there is no check for time of day, as the night layers for Lon Lon
                // have a door with a different text ID.
                doorType = DOOR_SCENEEXIT;
            } else {
                this->actionFunc = move_talk_wait;
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_LOCK_ON_DISABLED;
            }
        }
        // Replace the door type it was loaded with by the new type
        this->actor.params = (this->actor.params & ~ENDOOR_PARAMS_TYPE_MASK) | (doorType << ENDOOR_PARAMS_TYPE_SHIFT);
    }
}

static void move_wait(EnDoor* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 doorType;
    Vec3f playerPosRelToDoor;

    doorType = ENDOOR_GET_TYPE(&this->actor);
    Actor_search_position_project_distanceXZ(&this->actor, &playerPosRelToDoor, &player->actor.world.pos);
    if (this->playerIsOpening) {
        this->actionFunc = move_demo;
        Skeleton_Info2_init_standard_speedset_stop(&this->skelAnime, door_open_anm[this->openAnim],
                                   (player->stateFlags1 & PLAYER_STATE1_27) ? 0.75f : 1.5f);
        if (this->lockTimer != 0) {
            z_common_data.save.info.inventory.dungeonKeys[z_common_data.mapIndex] -= 1;
            Actor_Environment_sw_On(play, ENDOOR_GET_LOCKED_SWITCH_FLAG(&this->actor));
            Actor_SE_set(&this->actor, NA_SE_EV_CHAIN_KEY_UNLOCK);
        }
    } else if (!player_demo_check(play)) {
        if (fabsf(playerPosRelToDoor.y) < 20.0f && fabsf(playerPosRelToDoor.x) < 20.0f &&
            fabsf(playerPosRelToDoor.z) < 50.0f) {
            s16 yawDiff = player->actor.shape.rot.y - this->actor.shape.rot.y;

            if (playerPosRelToDoor.z > 0.0f) {
                yawDiff = 0x8000 - yawDiff;
            }
            if (ABS(yawDiff) < 0x3000) {
                if (this->lockTimer != 0) {
                    if (z_common_data.save.info.inventory.dungeonKeys[z_common_data.mapIndex] <= 0) {
                        Player* player2 = GET_PLAYER(play);

                        player2->naviTextId = -0x203;
                        return;
                    } else {
                        player->doorTimer = 10;
                    }
                }
                player->doorType = (doorType == DOOR_AJAR) ? PLAYER_DOORTYPE_AJAR : PLAYER_DOORTYPE_HANDLE;
                player->doorDirection = (playerPosRelToDoor.z >= 0.0f) ? 1.0f : -1.0f;
                player->doorActor = &this->actor;
            }
        } else if (doorType == DOOR_AJAR && this->actor.xzDistToPlayer > DOOR_AJAR_OPEN_RANGE) {
            this->actionFunc = move_half_open;
        }
    }
}

void move_talk_wait(EnDoor* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = move_talk;
    } else {
        Actor_talk_request2(&this->actor, play, DOOR_CHECK_RANGE);
    }
}

static void move_talk(EnDoor* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        this->actionFunc = move_talk_wait;
    }
}

void move_half_wait(EnDoor* this, PlayState* play) {
    if (this->actor.xzDistToPlayer < DOOR_AJAR_SLAM_RANGE) {
        this->actionFunc = move_half_close;
    }
}

void move_half_open(EnDoor* this, PlayState* play) {
    if (this->actor.xzDistToPlayer < DOOR_AJAR_SLAM_RANGE) {
        this->actionFunc = move_half_close;
    } else if (chase_angle(&this->actor.world.rot.y, -0x1800, 0x100)) {
        this->actionFunc = move_half_wait;
    }
}

void move_half_close(EnDoor* this, PlayState* play) {
    if (chase_angle(&this->actor.world.rot.y, 0, 0x700)) {
        this->actionFunc = move_wait;
    }
}

static void move_demo(EnDoor* this, PlayState* play) {
    if (DECR(this->lockTimer) == 0) {
        if (Skeleton_Info2_anime_play(&this->skelAnime)) {
            this->actionFunc = move_wait;
            this->playerIsOpening = false;
        } else if (Skeleton_Info_frame_check(&this->skelAnime, door_open_SE_set_frame[this->openAnim])) {
            Actor_SE_set(&this->actor,
                          (play->sceneId == SCENE_SHADOW_TEMPLE || play->sceneId == SCENE_BOTTOM_OF_THE_WELL ||
                           play->sceneId == SCENE_FIRE_TEMPLE)
                              ? NA_SE_EV_IRON_DOOR_OPEN
                              : NA_SE_OC_DOOR_OPEN);
            if (this->skelAnime.playSpeed < 1.5f) {
                s32 numEffects = (s32)(fqrand() * 30.0f) + 50;
                s32 i;

                for (i = 0; i < numEffects; i++) {
                    Effect_SS_Bubble_ct(play, &this->actor.world.pos, 60.0f, 100.0f, 50.0f, 0.15f);
                }
            }
        } else if (Skeleton_Info_frame_check(&this->skelAnime, door_close_SE_set_frame[this->openAnim])) {
            Actor_SE_set(&this->actor,
                          (play->sceneId == SCENE_SHADOW_TEMPLE || play->sceneId == SCENE_BOTTOM_OF_THE_WELL ||
                           play->sceneId == SCENE_FIRE_TEMPLE)
                              ? NA_SE_EV_IRON_DOOR_CLOSE
                              : NA_SE_EV_DOOR_CLOSE);
        }
    }
}

void En_Door_actor_move(Actor* thisx, PlayState* play) {
    EnDoor* this = (EnDoor*)thisx;

    this->actionFunc(this, play);
}

static s32 before_draw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    if (limbIndex == 4) {
        EnDoor* this = (EnDoor*)thisx;
        TransitionActorEntry* transitionEntry;
        Gfx** doorDLists = shape_type_data[this->dListIndex];

        transitionEntry = &play->transitionActors.list[GET_TRANSITION_ACTOR_INDEX(&this->actor)];
        rot->z += this->actor.world.rot.y;
        if ((play->roomCtx.prevRoom.num >= 0) || (transitionEntry->sides[0].room == transitionEntry->sides[1].room)) {
            // Draw the side of the door that is visible to the camera
#if OOT_VERSION < NTSC_1_1
            s16 rotDiff = this->actor.shape.rot.y + rot->z - search_position_angleY(&play->view.eye, &this->actor.world.pos);
#else
            s16 rotDiff = this->actor.shape.rot.y + this->skelAnime.jointTable[3].z + rot->z -
                          search_position_angleY(&play->view.eye, &this->actor.world.pos);
#endif

            *dList = (ABS(rotDiff) < 0x4000) ? doorDLists[0] : doorDLists[1];
        } else {
            s32 doorDListIndex = this->unk_192;

            if (transitionEntry->sides[0].room != this->actor.room) {
                doorDListIndex ^= 1;
            }
            *dList = doorDLists[doorDListIndex];
        }
    }
    return false;
}

void En_Door_actor_draw(Actor* thisx, PlayState* play) {
    EnDoor* this = (EnDoor*)thisx;

    if (this->actor.objectSlot == this->requiredObjectSlot) {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_door.c", 910);

        _texture_z_light_fog_prim(play->state.gfxCtx);
        Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, before_draw, NULL,
                          &this->actor);
        if (this->actor.world.rot.y != 0) {
            if (1) {}
            if (this->actor.world.rot.y > 0) {
                gSPDisplayList(POLY_OPA_DISP++, gDoorRightDL);
            } else {
                gSPDisplayList(POLY_OPA_DISP++, gDoorLeftDL);
            }
        }
        if (this->lockTimer != 0) {
            key_draw(play, this->lockTimer, DOORLOCK_NORMAL);
        }

        CLOSE_DISPS(play->state.gfxCtx, "../z_en_door.c", 941);
    }
}
