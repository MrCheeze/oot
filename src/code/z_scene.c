#include "global.h"
#include "terminal.h"
#include "versions.h"

SceneCmdHandlerFunc Scene_Proc[SCENE_CMD_ID_MAX];
RomFile elf_message_rom_address[];

/**
 * Spawn an object file of a specified ID that will persist through room changes.
 *
 * This waits for the file to be fully loaded, the data is available when the function returns.
 *
 * @return The new object slot corresponding to the requested object ID.
 *
 * @note This function is not meant to be called externally to spawn object files on the fly.
 * When an object is spawned with this function, all objects that come before it in the entry list will be treated as
 * persistent, which will likely cause either the amount of free slots or object space memory to run out.
 * This function is only meant to be called internally on scene load, before the object list from any room is processed.
 */
s32 Object_Exchange_keep_new(ObjectContext* objectCtx, s16 objectId) {
    u32 size;

    objectCtx->slots[objectCtx->numEntries].id = objectId;
    size = object_exchange_rom_address[objectId].vromEnd - object_exchange_rom_address[objectId].vromStart;

    PRINTF("OBJECT[%d] SIZE %fK SEG=%x\n", objectId, size / 1024.0f, objectCtx->slots[objectCtx->numEntries].segment);

    PRINTF("num=%d adrs=%x end=%x\n", objectCtx->numEntries,
           (uintptr_t)objectCtx->slots[objectCtx->numEntries].segment + size, objectCtx->spaceEnd);

    ASSERT(((objectCtx->numEntries < ARRAY_COUNT(objectCtx->slots)) &&
            (((uintptr_t)objectCtx->slots[objectCtx->numEntries].segment + size) < (uintptr_t)objectCtx->spaceEnd)),
           "this->num < OBJECT_EXCHANGE_BANK_MAX && (this->status[this->num].Segment + size) < this->endSegment",
           "../z_scene.c", 142);

    DMA_REQUEST_SYNC(objectCtx->slots[objectCtx->numEntries].segment, object_exchange_rom_address[objectId].vromStart, size,
                     "../z_scene.c", 145);

    if (objectCtx->numEntries < (ARRAY_COUNT(objectCtx->slots) - 1)) {
        objectCtx->slots[objectCtx->numEntries + 1].segment =
            (void*)ALIGN16((uintptr_t)objectCtx->slots[objectCtx->numEntries].segment + size);
    }

    objectCtx->numEntries++;
    objectCtx->numPersistentEntries = objectCtx->numEntries;

    return objectCtx->numEntries - 1;
}

// PAL N64 versions reduce the size of object space by 4 KiB in order to give some space back to
// the Zelda arena, which can help prevent an issue where actors fail to spawn in specific areas
// (sometimes referred to as the "Hyrule Field Glitch" although it can happen in more places than Hyrule Field).
#if !OOT_PAL_N64
#define OBJECT_SPACE_ADJUSTMENT 0
#else
#define OBJECT_SPACE_ADJUSTMENT (4 * 1024)
#endif

void Object_Exchange_ct(PlayState* play, ObjectContext* objectCtx) {
    PlayState* play2 = play;
    s32 pad;
    u32 spaceSize;
    s32 i;

    if (play2->sceneId == SCENE_HYRULE_FIELD) {
        spaceSize = 1000 * 1024 - OBJECT_SPACE_ADJUSTMENT;
    } else if (play2->sceneId == SCENE_GANON_BOSS) {
        if (z_common_data.sceneLayer != 4) {
            spaceSize = 1150 * 1024 - OBJECT_SPACE_ADJUSTMENT;
        } else {
            spaceSize = 1000 * 1024 - OBJECT_SPACE_ADJUSTMENT;
        }
    } else if (play2->sceneId == SCENE_SPIRIT_TEMPLE_BOSS) {
        spaceSize = 1050 * 1024 - OBJECT_SPACE_ADJUSTMENT;
    } else if (play2->sceneId == SCENE_CHAMBER_OF_THE_SAGES) {
        spaceSize = 1050 * 1024 - OBJECT_SPACE_ADJUSTMENT;
    } else if (play2->sceneId == SCENE_GANONDORF_BOSS) {
        spaceSize = 1050 * 1024 - OBJECT_SPACE_ADJUSTMENT;
    } else {
        spaceSize = 1000 * 1024 - OBJECT_SPACE_ADJUSTMENT;
    }

    objectCtx->numEntries = objectCtx->numPersistentEntries = 0;
    objectCtx->mainKeepSlot = objectCtx->subKeepSlot = 0;

    for (i = 0; i < ARRAY_COUNT(objectCtx->slots); i++) {
        objectCtx->slots[i].id = OBJECT_INVALID;
    }

    PRINTF_COLOR_GREEN();
    PRINTF(T("オブジェクト入れ替えバンク情報 %8.3fKB\n", "Object exchange bank data %8.3fKB\n"), spaceSize / 1024.0f);
    PRINTF_RST();

    objectCtx->spaceStart = objectCtx->slots[0].segment =
        GAME_STATE_ALLOC(&play->state, spaceSize, "../z_scene.c", 219);
    objectCtx->spaceEnd = (void*)((uintptr_t)objectCtx->spaceStart + spaceSize);

    objectCtx->mainKeepSlot = Object_Exchange_keep_new(objectCtx, OBJECT_GAMEPLAY_KEEP);
    SegmentBaseAddress[4] = VIRTUAL_TO_PHYSICAL(objectCtx->slots[objectCtx->mainKeepSlot].segment);
}

void Object_Exchange_check(ObjectContext* objectCtx) {
    s32 i;
    ObjectEntry* entry = &objectCtx->slots[0];
    RomFile* objectFile;
    u32 size;

    for (i = 0; i < objectCtx->numEntries; i++) {
        if (entry->id < 0) {
            if (entry->dmaRequest.vromAddr == 0) {
                osCreateMesgQueue(&entry->loadQueue, &entry->loadMsg, 1);
                objectFile = &object_exchange_rom_address[-entry->id];
                size = objectFile->vromEnd - objectFile->vromStart;

                PRINTF("OBJECT EXCHANGE BANK-%2d SIZE %8.3fK SEG=%08x\n", i, size / 1024.0f, entry->segment);

                DMA_REQUEST_ASYNC(&entry->dmaRequest, entry->segment, objectFile->vromStart, size, 0, &entry->loadQueue,
                                  NULL, "../z_scene.c", 266);
            } else if (osRecvMesg(&entry->loadQueue, NULL, OS_MESG_NOBLOCK) == 0) {
                entry->id = -entry->id;
            }
        }
        entry++;
    }
}

s32 Object_Exchange_bank_check(ObjectContext* objectCtx, s16 objectId) {
    s32 i;

    for (i = 0; i < objectCtx->numEntries; i++) {
        if (ABS(objectCtx->slots[i].id) == objectId) {
            return i;
        }
    }

    return -1;
}

s32 Object_Exchange_bank_dma_check(ObjectContext* objectCtx, s32 slot) {
    if (objectCtx->slots[slot].id > 0) {
        return true;
    } else {
        return false;
    }
}

void Object_Exchange_read_all(ObjectContext* objectCtx) {
    s32 i;
    s32 id;
    u32 size;

    for (i = 0; i < objectCtx->numEntries; i++) {
        id = objectCtx->slots[i].id;
        size = object_exchange_rom_address[id].vromEnd - object_exchange_rom_address[id].vromStart;
        PRINTF("OBJECT[%d] SIZE %fK SEG=%x\n", objectCtx->slots[i].id, size / 1024.0f, objectCtx->slots[i].segment);
        PRINTF("num=%d adrs=%x end=%x\n", objectCtx->numEntries, (uintptr_t)objectCtx->slots[i].segment + size,
               objectCtx->spaceEnd);
        DMA_REQUEST_SYNC(objectCtx->slots[i].segment, object_exchange_rom_address[id].vromStart, size, "../z_scene.c", 342);
    }
}

void* Object_Exchange_change(ObjectContext* objectCtx, s32 slot, s16 objectId) {
    ObjectEntry* entry = &objectCtx->slots[slot];
    RomFile* objectFile = &object_exchange_rom_address[objectId];
    u32 size;
    void* nextPtr;

    entry->id = -objectId;
    entry->dmaRequest.vromAddr = 0;

    size = objectFile->vromEnd - objectFile->vromStart;
    PRINTF("OBJECT EXCHANGE NO=%2d BANK=%3d SIZE=%8.3fK\n", slot, objectId, size / 1024.0f);

    nextPtr = (void*)ALIGN16((uintptr_t)entry->segment + size);

    ASSERT(nextPtr < objectCtx->spaceEnd, "nextptr < this->endSegment", "../z_scene.c", 381);

    PRINTF(T("オブジェクト入れ替え空きサイズ=%08x\n", "Object exchange free size=%08x\n"),
           (uintptr_t)objectCtx->spaceEnd - (uintptr_t)nextPtr);

    return nextPtr;
}

s32 Scene_ct(PlayState* play, SceneCmd* sceneCmd) {
    while (true) {
        u32 cmdCode = sceneCmd->base.code;

        PRINTF("*** Scene_Word = { code=%d, data1=%02x, data2=%04x } ***\n", cmdCode, sceneCmd->base.data1,
               sceneCmd->base.data2);

        if (cmdCode == SCENE_CMD_ID_END) {
            break;
        }

        if (cmdCode < ARRAY_COUNT(Scene_Proc)) {
            Scene_Proc[cmdCode](play, sceneCmd);
        } else {
            PRINTF_COLOR_RED();
            PRINTF(T("code の値が異常です\n", "code variable is abnormal\n"));
            PRINTF_RST();
        }

        sceneCmd++;
    }

    return 0;
}

BAD_RETURN(s32) Scene_Proc_Player_Ptr(PlayState* play, SceneCmd* cmd) {
    ActorEntry* playerEntry = play->playerEntry =
        (ActorEntry*)SEGMENTED_TO_VIRTUAL(cmd->playerEntryList.data) + play->spawnList[play->spawn].playerEntryIndex;
    s16 linkObjectId;

    play->linkAgeOnLoad = ((void)0, z_common_data.save.linkAge);

    linkObjectId = object_exchange_player_bank[((void)0, z_common_data.save.linkAge)];

    actor_dlftbls[playerEntry->id].profile->objectId = linkObjectId;
    Object_Exchange_keep_new(&play->objectCtx, linkObjectId);
}

BAD_RETURN(s32) Scene_Proc_Actor_Ptr(PlayState* play, SceneCmd* cmd) {
    play->numActorEntries = cmd->actorEntryList.length;
    play->actorEntryList = SEGMENTED_TO_VIRTUAL(cmd->actorEntryList.data);
}

BAD_RETURN(s32) Scene_Proc_Camera_Data_Ptr(PlayState* play, SceneCmd* cmd) {
    play->unk_11DFC = SEGMENTED_TO_VIRTUAL(cmd->unused02.segment);
}

BAD_RETURN(s32) Scene_Proc_BGcheck_Ptr(PlayState* play, SceneCmd* cmd) {
    CollisionHeader* colHeader = SEGMENTED_TO_VIRTUAL(cmd->colHeader.data);

    colHeader->vtxList = SEGMENTED_TO_VIRTUAL(colHeader->vtxList);
    colHeader->polyList = SEGMENTED_TO_VIRTUAL(colHeader->polyList);
    colHeader->surfaceTypeList = SEGMENTED_TO_VIRTUAL(colHeader->surfaceTypeList);
    colHeader->bgCamList = SEGMENTED_TO_VIRTUAL(colHeader->bgCamList);
    colHeader->waterBoxes = SEGMENTED_TO_VIRTUAL(colHeader->waterBoxes);

    T_BGCheck_ct(&play->colCtx, play, colHeader);
}

BAD_RETURN(s32) Scene_Proc_Room_Rom_Address_Ptr(PlayState* play, SceneCmd* cmd) {
    play->roomList.count = cmd->roomList.length;
    play->roomList.romFiles = SEGMENTED_TO_VIRTUAL(cmd->roomList.data);
}

BAD_RETURN(s32) Scene_Proc_Room_Status_Ptr(PlayState* play, SceneCmd* cmd) {
    play->spawnList = SEGMENTED_TO_VIRTUAL(cmd->spawnList.data);
}

BAD_RETURN(s32) Scene_Proc_Exchange_Keep_Bank(PlayState* play, SceneCmd* cmd) {
    if (cmd->specialFiles.keepObjectId != OBJECT_INVALID) {
        play->objectCtx.subKeepSlot = Object_Exchange_keep_new(&play->objectCtx, cmd->specialFiles.keepObjectId);
        SegmentBaseAddress[5] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[play->objectCtx.subKeepSlot].segment);
    }

    if (cmd->specialFiles.naviQuestHintFileId != NAVI_QUEST_HINTS_NONE) {
        play->naviQuestHints = ROM_read(play, &elf_message_rom_address[cmd->specialFiles.naviQuestHintFileId - 1]);
    }
}

BAD_RETURN(s32) Scene_Proc_Room_Type(PlayState* play, SceneCmd* cmd) {
    play->roomCtx.curRoom.type = cmd->roomBehavior.gpFlag1;
    play->roomCtx.curRoom.environmentType = cmd->roomBehavior.gpFlag2 & 0xFF;
    play->roomCtx.curRoom.lensMode = (cmd->roomBehavior.gpFlag2 >> 8) & 1;
    play->msgCtx.disableWarpSongs = (cmd->roomBehavior.gpFlag2 >> 0xA) & 1;
}

BAD_RETURN(s32) Scene_Proc_Ground_Shape_Status_Ptr(PlayState* play, SceneCmd* cmd) {
    play->roomCtx.curRoom.roomShape = SEGMENTED_TO_VIRTUAL(cmd->mesh.data);
}

BAD_RETURN(s32) Scene_Proc_Object_Exchange_Bank_Ptr(PlayState* play, SceneCmd* cmd) {
    s32 i;
    s32 j;
    s32 k;
    ObjectEntry* entry;
    ObjectEntry* invalidatedEntry;
    ObjectEntry* entries;
    s16* objectListEntry = SEGMENTED_TO_VIRTUAL(cmd->objectList.data);
    void* nextPtr;

    k = 0;
    i = play->objectCtx.numPersistentEntries;
    entries = play->objectCtx.slots;
    entry = &play->objectCtx.slots[i];

    while (i < play->objectCtx.numEntries) {
        if (entry->id != *objectListEntry) {

            invalidatedEntry = &play->objectCtx.slots[i];
            for (j = i; j < play->objectCtx.numEntries; j++) {
                invalidatedEntry->id = OBJECT_INVALID;
                invalidatedEntry++;
            }

            play->objectCtx.numEntries = i;
            Actor_info_bank_actor_check(play, &play->actorCtx);

            continue;
        }

        i++;
        k++;
        objectListEntry++;
        entry++;
    }

    ASSERT(cmd->objectList.length <= ARRAY_COUNT(play->objectCtx.slots),
           "scene_info->object_bank.num <= OBJECT_EXCHANGE_BANK_MAX", "../z_scene.c", 705);

    while (k < cmd->objectList.length) {
        nextPtr = Object_Exchange_change(&play->objectCtx, i, *objectListEntry);
        if (i < (ARRAY_COUNT(play->objectCtx.slots) - 1)) {
            entries[i + 1].segment = nextPtr;
        }
        i++;
        k++;
        objectListEntry++;
    }

    play->objectCtx.numEntries = i;
}

BAD_RETURN(s32) Scene_Proc_Light_Data_Ptr(PlayState* play, SceneCmd* cmd) {
    s32 i;
    LightInfo* lightInfo = SEGMENTED_TO_VIRTUAL(cmd->lightList.data);

    for (i = 0; i < cmd->lightList.length; i++) {
        Global_light_list_new(play, &play->lightCtx, lightInfo);
        lightInfo++;
    }
}

BAD_RETURN(s32) Scene_Proc_Path_Data_Ptr(PlayState* play, SceneCmd* cmd) {
    play->pathList = SEGMENTED_TO_VIRTUAL(cmd->pathList.data);
}

BAD_RETURN(s32) Scene_Proc_Door_Data_Ptr(PlayState* play, SceneCmd* cmd) {
    play->transitionActors.count = cmd->transiActorList.length;
    play->transitionActors.list = SEGMENTED_TO_VIRTUAL(cmd->transiActorList.data);
}

void Door_info_ct(GameState* state, TransitionActorList* transitionActors) {
    transitionActors->count = 0;
}

BAD_RETURN(s32) Scene_Proc_Kcolor_Data_Ptr(PlayState* play, SceneCmd* cmd) {
    play->envCtx.numLightSettings = cmd->lightSettingList.length;
    play->envCtx.lightSettingsList = SEGMENTED_TO_VIRTUAL(cmd->lightSettingList.data);
}

BAD_RETURN(s32) Scene_Proc_Kankyo_Scene_Type(PlayState* play, SceneCmd* cmd) {
    play->skyboxId = cmd->skyboxSettings.skyboxId;
    play->envCtx.skyboxConfig = play->envCtx.changeSkyboxNextConfig = cmd->skyboxSettings.skyboxConfig;
    play->envCtx.lightMode = cmd->skyboxSettings.envLightMode;
}

BAD_RETURN(s32) Scene_Proc_Kankyo_Room_Type(PlayState* play, SceneCmd* cmd) {
    play->envCtx.skyboxDisabled = cmd->skyboxDisables.skyboxDisabled;
    play->envCtx.sunMoonDisabled = cmd->skyboxDisables.sunMoonDisabled;
}

BAD_RETURN(s32) Scene_Proc_Time_Set(PlayState* play, SceneCmd* cmd) {
    if ((cmd->timeSettings.hour != 0xFF) && (cmd->timeSettings.min != 0xFF)) {
        z_common_data.skyboxTime = z_common_data.save.dayTime =
            ((cmd->timeSettings.hour + (cmd->timeSettings.min / 60.0f)) * 60.0f) / ((f32)(24 * 60) / 0x10000);
    }

    if (cmd->timeSettings.timeSpeed != 0xFF) {
        play->envCtx.sceneTimeSpeed = cmd->timeSettings.timeSpeed;
    } else {
        play->envCtx.sceneTimeSpeed = 0;
    }

    if (z_common_data.sunsSongState == SUNSSONG_INACTIVE) {
        E_day_time_plus = play->envCtx.sceneTimeSpeed;
    }

    play->envCtx.sunPos.x = -(sin_s(((void)0, z_common_data.save.dayTime) - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f;
    play->envCtx.sunPos.y = (cos_s(((void)0, z_common_data.save.dayTime) - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f;
    play->envCtx.sunPos.z = (cos_s(((void)0, z_common_data.save.dayTime) - CLOCK_TIME(12, 0)) * 20.0f) * 25.0f;

    if (((play->envCtx.sceneTimeSpeed == 0) && (z_common_data.save.cutsceneIndex < 0xFFF0)) ||
        (z_common_data.save.entranceIndex == ENTR_LAKE_HYLIA_8)) {
#if OOT_VERSION >= PAL_1_0
        z_common_data.skyboxTime = ((void)0, z_common_data.save.dayTime);
#endif

#if OOT_VERSION < PAL_1_0
        if ((z_common_data.skyboxTime > CLOCK_TIME(4, 0)) && (z_common_data.skyboxTime <= CLOCK_TIME(5, 0))) {
            z_common_data.skyboxTime = CLOCK_TIME(5, 0) + 1;
        } else if ((z_common_data.skyboxTime >= CLOCK_TIME(6, 0)) && (z_common_data.skyboxTime <= CLOCK_TIME(8, 0))) {
            z_common_data.skyboxTime = CLOCK_TIME(8, 0) + 1;
#else
        if ((z_common_data.skyboxTime > CLOCK_TIME(4, 0)) && (z_common_data.skyboxTime < CLOCK_TIME(6, 30))) {
            z_common_data.skyboxTime = CLOCK_TIME(5, 0) + 1;
        } else if ((z_common_data.skyboxTime >= CLOCK_TIME(6, 30)) && (z_common_data.skyboxTime <= CLOCK_TIME(8, 0))) {
            z_common_data.skyboxTime = CLOCK_TIME(8, 0) + 1;
#endif
        } else if ((z_common_data.skyboxTime >= CLOCK_TIME(16, 0)) && (z_common_data.skyboxTime <= CLOCK_TIME(17, 0))) {
            z_common_data.skyboxTime = CLOCK_TIME(17, 0) + 1;
        } else if ((z_common_data.skyboxTime >= CLOCK_TIME(18, 0) + 1) &&
                   (z_common_data.skyboxTime <= CLOCK_TIME(19, 0))) {
            z_common_data.skyboxTime = CLOCK_TIME(19, 0) + 1;
        }
    }
}

BAD_RETURN(s32) Scene_Proc_Wind_Set(PlayState* play, SceneCmd* cmd) {
    s8 x = cmd->windSettings.x;
    s8 y = cmd->windSettings.y;
    s8 z = cmd->windSettings.z;

    play->envCtx.windDirection.x = x;
    play->envCtx.windDirection.y = y;
    play->envCtx.windDirection.z = z;

    play->envCtx.windSpeed = cmd->windSettings.unk_07;
}

BAD_RETURN(s32) Scene_Proc_Next_Scene_Ptr(PlayState* play, SceneCmd* cmd) {
    play->exitList = SEGMENTED_TO_VIRTUAL(cmd->exitList.data);
}

BAD_RETURN(s32) Scene_Proc_Sound_Type(PlayState* play, SceneCmd* cmd) {
}

BAD_RETURN(s32) Scene_Proc_Scene_Sound(PlayState* play, SceneCmd* cmd) {
    play->sceneSequences.seqId = cmd->soundSettings.seqId;
    play->sceneSequences.natureAmbienceId = cmd->soundSettings.natureAmbienceId;

    if (z_common_data.seqId == (u8)NA_BGM_DISABLED) {
        SEQCMD_RESET_AUDIO_HEAP(0, cmd->soundSettings.specId);
    }
}

BAD_RETURN(s32) Scene_Proc_Room_Sound(PlayState* play, SceneCmd* cmd) {
    play->roomCtx.curRoom.echo = cmd->echoSettings.echo;
}

BAD_RETURN(s32) Scene_Proc_Demo_Scene_Data(PlayState* play, SceneCmd* cmd) {
    PRINTF("\n[ZU]sceneset age    =[%X]", ((void)0, z_common_data.save.linkAge));
    PRINTF("\n[ZU]sceneset time   =[%X]", ((void)0, z_common_data.save.cutsceneIndex));
    PRINTF("\n[ZU]sceneset counter=[%X]", ((void)0, z_common_data.sceneLayer));

    if (z_common_data.sceneLayer != 0) {
        SceneCmd* altHeader = ((SceneCmd**)SEGMENTED_TO_VIRTUAL(cmd->altHeaders.data))[z_common_data.sceneLayer - 1];

        if (altHeader != NULL) {
            Scene_ct(play, SEGMENTED_TO_VIRTUAL(altHeader));
            (cmd + 1)->base.code = SCENE_CMD_ID_END;
        } else {
            PRINTF(T("\nげぼはっ！ 指定されたデータがないでええっす！", "\nCoughh! There is no specified dataaaaa!"));

            if (z_common_data.sceneLayer == SCENE_LAYER_ADULT_NIGHT) {
                // Due to the condition above, this is equivalent to accessing altHeaders[SCENE_LAYER_ADULT_DAY - 1]
                SceneCmd* altHeader = ((SceneCmd**)SEGMENTED_TO_VIRTUAL(
                    cmd->altHeaders
                        .data))[(z_common_data.sceneLayer - SCENE_LAYER_ADULT_NIGHT) + SCENE_LAYER_ADULT_DAY - 1];

                PRINTF(T("\nそこで、大人の昼データを使用するでええっす！！", "\nUsing adult day data there!!"));

                if (altHeader != NULL) {
                    Scene_ct(play, SEGMENTED_TO_VIRTUAL(altHeader));
                    (cmd + 1)->base.code = SCENE_CMD_ID_END;
                }
            }
        }
    }
}

BAD_RETURN(s32) Scene_Proc_Demo_Data(PlayState* play, SceneCmd* cmd) {
    PRINTF("\ngame_play->demo_play.data=[%x]", play->csCtx.script);
    play->csCtx.script = SEGMENTED_TO_VIRTUAL(cmd->cutsceneData.data);
}

BAD_RETURN(s32) Scene_Proc_Scene_Type(PlayState* play, SceneCmd* cmd) {
    R_SCENE_CAM_TYPE = cmd->miscSettings.sceneCamType;
    z_common_data.worldMapArea = cmd->miscSettings.area;

    if ((play->sceneId == SCENE_BAZAAR) || (play->sceneId == SCENE_SHOOTING_GALLERY)) {
        if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
            z_common_data.worldMapArea = WORLD_MAP_AREA_KAKARIKO_VILLAGE;
        }
    }

    if (((play->sceneId >= SCENE_HYRULE_FIELD) && (play->sceneId <= SCENE_OUTSIDE_GANONS_CASTLE)) ||
        ((play->sceneId >= SCENE_MARKET_ENTRANCE_DAY) && (play->sceneId <= SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS))) {
        if (z_common_data.save.cutsceneIndex < 0xFFF0) {
            z_common_data.save.info.worldMapAreaData |= check_bit[((void)0, z_common_data.worldMapArea)];
            PRINTF("０００  ａｒｅａ＿ａｒｒｉｖａｌ＝%x (%d)\n", z_common_data.save.info.worldMapAreaData,
                   ((void)0, z_common_data.worldMapArea));
        }
    }
}

void Exit_Fade_Set(PlayState* play) {
    s16 entranceIndex;

    if (!IS_DAY) {
        if (!LINK_IS_ADULT) {
            entranceIndex = play->nextEntranceIndex + 1;
        } else {
            entranceIndex = play->nextEntranceIndex + 3;
        }
    } else {
        if (!LINK_IS_ADULT) {
            entranceIndex = play->nextEntranceIndex;
        } else {
            entranceIndex = play->nextEntranceIndex + 2;
        }
    }

    play->transitionType = ENTRANCE_INFO_START_TRANS_TYPE(scene_status_data[entranceIndex].field);
}

SceneCmdHandlerFunc Scene_Proc[SCENE_CMD_ID_MAX] = {
    Scene_Proc_Player_Ptr,          // SCENE_CMD_ID_SPAWN_LIST
    Scene_Proc_Actor_Ptr,           // SCENE_CMD_ID_ACTOR_LIST
    Scene_Proc_Camera_Data_Ptr,                  // SCENE_CMD_ID_UNUSED_2
    Scene_Proc_BGcheck_Ptr,          // SCENE_CMD_ID_COLLISION_HEADER
    Scene_Proc_Room_Rom_Address_Ptr,                 // SCENE_CMD_ID_ROOM_LIST
    Scene_Proc_Wind_Set,             // SCENE_CMD_ID_WIND_SETTINGS
    Scene_Proc_Room_Status_Ptr,                // SCENE_CMD_ID_ENTRANCE_LIST
    Scene_Proc_Exchange_Keep_Bank,             // SCENE_CMD_ID_SPECIAL_FILES
    Scene_Proc_Room_Type,             // SCENE_CMD_ID_ROOM_BEHAVIOR
    Scene_Proc_Sound_Type,               // SCENE_CMD_ID_UNDEFINED_9
    Scene_Proc_Ground_Shape_Status_Ptr,                // SCENE_CMD_ID_ROOM_SHAPE
    Scene_Proc_Object_Exchange_Bank_Ptr,               // SCENE_CMD_ID_OBJECT_LIST
    Scene_Proc_Light_Data_Ptr,                // SCENE_CMD_ID_LIGHT_LIST
    Scene_Proc_Path_Data_Ptr,                 // SCENE_CMD_ID_PATH_LIST
    Scene_Proc_Door_Data_Ptr, // SCENE_CMD_ID_TRANSITION_ACTOR_LIST
    Scene_Proc_Kcolor_Data_Ptr,        // SCENE_CMD_ID_LIGHT_SETTINGS_LIST
    Scene_Proc_Time_Set,             // SCENE_CMD_ID_TIME_SETTINGS
    Scene_Proc_Kankyo_Scene_Type,           // SCENE_CMD_ID_SKYBOX_SETTINGS
    Scene_Proc_Kankyo_Room_Type,           // SCENE_CMD_ID_SKYBOX_DISABLES
    Scene_Proc_Next_Scene_Ptr,                 // SCENE_CMD_ID_EXIT_LIST
    NULL,                                  // SCENE_CMD_ID_END
    Scene_Proc_Scene_Sound,            // SCENE_CMD_ID_SOUND_SETTINGS
    Scene_Proc_Room_Sound,             // SCENE_CMD_ID_ECHO_SETTINGS
    Scene_Proc_Demo_Data,             // SCENE_CMD_ID_CUTSCENE_DATA
    Scene_Proc_Demo_Scene_Data,      // SCENE_CMD_ID_ALTERNATE_HEADER_LIST
    Scene_Proc_Scene_Type,             // SCENE_CMD_ID_MISC_SETTINGS
};

RomFile elf_message_rom_address[] = {
    ROM_FILE(elf_message_field),
    ROM_FILE(elf_message_ydan),
};
