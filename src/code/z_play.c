
#include "global.h"
#include "fault.h"
#include "quake.h"
#include "terminal.h"
#include "versions.h"
#include "line_numbers.h"
#if PLATFORM_N64
#include "n64dd.h"
#endif

#include "z64debug_display.h"
#include "z64frame_advance.h"

#pragma increment_block_number "gc-eu:128 gc-eu-mq:128 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128 hiratsu3:128"

TransitionTile fbdemo;
s32 fbdemo_mode;
VisMono z_play_vismono;
Color_RGBA8_u32 z_play_vismono_color;

#if DEBUG_FEATURES
FaultClient D_801614B8;
#endif

s16 demofade_flame;

#if DEBUG_FEATURES
void* gDebugCutsceneScript = NULL;
UNK_TYPE D_8012D1F4 = 0; // unused
#endif

Input* Debug_Pad = NULL;

void Gameplay_Scene_Read(PlayState* this, s32 sceneId, s32 spawn);

// This macro prints the number "1" with a file and line number if R_ENABLE_PLAY_LOGS is enabled.
// For example, it can be used to trace the play state execution at a high level.
#if DEBUG_FEATURES
#define PLAY_LOG(line)                            \
    do {                                          \
        if (R_ENABLE_PLAY_LOGS) {                 \
            LOG_NUM("1", 1, "../z_play.c", line); \
        }                                         \
    } while (0)
#else
#define PLAY_LOG(line) (void)0
#endif

void Game_play_pr_vr_camera_update(PlayState* this) {
    changeCameraID(GET_ACTIVE_CAM(this), this->viewpoint - 1);
}

void Game_play_pr_vr_switch_pr_set(PlayState* this, s16 viewpoint) {
    ASSERT(viewpoint == VIEWPOINT_LOCKED || viewpoint == VIEWPOINT_PIVOT, "point == 1 || point == 2", "../z_play.c",
           2160);

    this->viewpoint = viewpoint;

    if ((R_SCENE_CAM_TYPE != SCENE_CAM_TYPE_FIXED_SHOP_VIEWPOINT) && (z_common_data.save.cutsceneIndex < 0xFFF0)) {
        // Play a sfx when the player toggles the camera
        Nai_FxFlagEntry((viewpoint == VIEWPOINT_LOCKED) ? NA_SE_SY_CAMERA_ZOOM_DOWN : NA_SE_SY_CAMERA_ZOOM_UP,
                             &_dummy_zero_f, 4, &_dummy_one, &_dummy_one,
                             &_dummy_zero_s8);
    }

    Game_play_pr_vr_camera_update(this);
}

/**
 * @return true if the currently set viewpoint is the same as the one provided in the argument
 */
s32 Game_play_pr_vr_switch_pr_check(PlayState* this, s16 viewpoint) {
    return (viewpoint == this->viewpoint);
}

/**
 * If the scene is a shop, set the viewpoint that will set the bgCamIndex
 * to toggle the camera into a "browsing item selection" setting.
 */
void Game_play_shop_pr_vr_switch_set(PlayState* this) {
    PRINTF("Game_play_shop_pr_vr_switch_set()\n");

    if (R_SCENE_CAM_TYPE == SCENE_CAM_TYPE_FIXED_SHOP_VIEWPOINT) {
        this->viewpoint = VIEWPOINT_PIVOT;
    }
}

void Game_play_fbdemo_wipe_create(PlayState* this, s32 transitionType) {
    TransitionContext* transitionCtx = &this->transitionCtx;

    bzero(transitionCtx, sizeof(TransitionContext));

    transitionCtx->transitionType = transitionType;

    // circle types
    if ((transitionCtx->transitionType >> 5) == 1) {
        transitionCtx->init = fbdemo_wipe3_init;
        transitionCtx->destroy = fbdemo_wipe3_cleanup;
        transitionCtx->start = fbdemo_wipe3_startup;
        transitionCtx->isDone = fbdemo_wipe3_is_finish;
        transitionCtx->draw = fbdemo_wipe3_draw;
        transitionCtx->update = fbdemo_wipe3_move;
        transitionCtx->setType = fbdemo_wipe3_settype;
        transitionCtx->setColor = fbdemo_wipe3_setcolor_rgba8888;
        transitionCtx->setUnkColor = fbdemo_wipe3_setaltcolor_rgba8888;
    } else {
        switch (transitionCtx->transitionType) {
            case TRANS_TYPE_TRIFORCE:
                transitionCtx->init = fbdemo_triforce_init;
                transitionCtx->destroy = fbdemo_triforce_cleanup;
                transitionCtx->start = fbdemo_triforce_startup;
                transitionCtx->isDone = fbdemo_triforce_is_finish;
                transitionCtx->draw = fbdemo_triforce_draw;
                transitionCtx->update = fbdemo_triforce_move;
                transitionCtx->setType = fbdemo_triforce_settype;
                transitionCtx->setColor = fbdemo_triforce_setcolor_rgba8888;
                transitionCtx->setUnkColor = NULL;
                break;

            case TRANS_TYPE_WIPE:
            case TRANS_TYPE_WIPE_FAST:
                transitionCtx->init = fbdemo_wipe1_init;
                transitionCtx->destroy = fbdemo_wipe1_cleanup;
                transitionCtx->start = fbdemo_wipe1_startup;
                transitionCtx->isDone = fbdemo_wipe1_is_finish;
                transitionCtx->draw = fbdemo_wipe1_draw;
                transitionCtx->update = fbdemo_wipe1_move;
                transitionCtx->setType = fbdemo_wipe1_settype;
                transitionCtx->setColor = fbdemo_wipe1_setcolor_rgba8888;
                transitionCtx->setUnkColor = NULL;
                break;

            case TRANS_TYPE_FADE_BLACK:
            case TRANS_TYPE_FADE_WHITE:
            case TRANS_TYPE_FADE_BLACK_FAST:
            case TRANS_TYPE_FADE_WHITE_FAST:
            case TRANS_TYPE_FADE_BLACK_SLOW:
            case TRANS_TYPE_FADE_WHITE_SLOW:
            case TRANS_TYPE_FADE_WHITE_CS_DELAYED:
            case TRANS_TYPE_FADE_WHITE_INSTANT:
            case TRANS_TYPE_FADE_GREEN:
            case TRANS_TYPE_FADE_BLUE:
                transitionCtx->init = fbdemo_fade_init;
                transitionCtx->destroy = fbdemo_fade_cleanup;
                transitionCtx->start = fbdemo_fade_startup;
                transitionCtx->isDone = fbdemo_fade_is_finish;
                transitionCtx->draw = fbdemo_fade_draw;
                transitionCtx->update = fbdemo_fade_move;
                transitionCtx->setType = fbdemo_fade_settype;
                transitionCtx->setColor = fbdemo_fade_setcolor_rgba8888;
                transitionCtx->setUnkColor = NULL;
                break;

            case TRANS_TYPE_FILL_WHITE2:
            case TRANS_TYPE_FILL_WHITE:
                this->transitionMode = TRANS_MODE_FILL_WHITE_INIT;
                break;

            case TRANS_TYPE_INSTANT:
                this->transitionMode = TRANS_MODE_INSTANT;
                break;

            case TRANS_TYPE_FILL_BROWN:
                this->transitionMode = TRANS_MODE_FILL_BROWN_INIT;
                break;

            case TRANS_TYPE_SANDSTORM_PERSIST:
                this->transitionMode = TRANS_MODE_SANDSTORM_INIT;
                break;

            case TRANS_TYPE_SANDSTORM_END:
                this->transitionMode = TRANS_MODE_SANDSTORM_END_INIT;
                break;

            case TRANS_TYPE_CS_BLACK_FILL:
                this->transitionMode = TRANS_MODE_CS_BLACK_FILL_INIT;
                break;

            default:
                HUNGUP_AND_CRASH("../z_play.c", LN5(2263, 2266, 2269, 2272, 2282, 2287, 2290, 2293));
                break;
        }
    }
}

void Game_play_fbdemo_wipe_destroy(PlayState* this) {
    this->transitionCtx.transitionType = -1;
}

Gfx* game_play_set_fog(PlayState* this, Gfx* gfx) {
    return set_fog(gfx, this->lightCtx.fogColor[0], this->lightCtx.fogColor[1], this->lightCtx.fogColor[2], 0,
                       this->lightCtx.fogNear, 1000);
}

void play_cleanup(GameState* thisx) {
    PlayState* this = (PlayState*)thisx;
    Player* player = GET_PLAYER(this);

    if (1) {}

    this->state.gfxCtx->callback = NULL;
    this->state.gfxCtx->callbackParam = NULL;

    SREG(91) = 0;
    R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_OFF;

    PreRender_cleanup(&this->pauseBgPreRender);
    EffectFreeAllIndex(this);
    EffectSoftSprite_dt(this);
    CollisionCheck_dt(this, &this->colChkCtx);

    if (fbdemo_mode == TRANS_TILE_READY) {
        fbdemo_cleanup(&fbdemo);
        fbdemo_mode = TRANS_TILE_OFF;
    }

    if (this->transitionMode == TRANS_MODE_INSTANCE_RUNNING) {
        this->transitionCtx.destroy(&this->transitionCtx.instanceData);
        Game_play_fbdemo_wipe_destroy(this);
        this->transitionMode = TRANS_MODE_OFF;
    }

    shrink_window_cleanup();
    fbdemo_fade_cleanup(&this->transitionFadeFlash);
    z_vismono_cleanup(&z_play_vismono);

    if (z_common_data.save.linkAge != this->linkAgeOnLoad) {
        equip_default_set();
        player_ability_set(this, player);
    }

    Actor_info_dt(&this->actorCtx, this);
    parameter_dt(this);
    kaleido_scope_dt_ovl(this);
    KaleidoArea_cleanup();
    zelda_CleanupArena();

#if PLATFORM_N64
    if ((B_80121220 != NULL) && (B_80121220->unk_14 != NULL)) {
        B_80121220->unk_14(this);
    }
#endif

#if DEBUG_FEATURES
    fault_RemoveClient(&D_801614B8);
#endif
}

void play_init(GameState* thisx) {
    PlayState* this = (PlayState*)thisx;
    GraphicsContext* gfxCtx = this->state.gfxCtx;
    uintptr_t zAlloc;
    uintptr_t zAllocAligned;
    size_t zAllocSize;
    Player* player;
    s32 playerStartBgCamIndex;
    s32 i;
    u8 baseSceneLayer;
    s32 pad[2];

    if (z_common_data.save.entranceIndex == ENTR_LOAD_OPENING) {
        z_common_data.save.entranceIndex = 0;
        this->state.running = false;
        SET_NEXT_GAMESTATE(&this->state, opening_init, TitleSetupState);
        return;
    }

#if PLATFORM_GC && DEBUG_FEATURES
    SystemArena_Display();
#endif

    game_resize_hyral(&this->state, 0x1D4790);

#if PLATFORM_N64
    if ((B_80121220 != NULL) && (B_80121220->unk_10 != NULL)) {
        B_80121220->unk_10(this);
    }
#endif

    KaleidoArea_init(this);
    initView(&this->view, gfxCtx);
    Na_SetWaterSeModeFlag(0);
    initQuakeControl();

    for (i = 0; i < ARRAY_COUNT(this->cameraPtrs); i++) {
        this->cameraPtrs[i] = NULL;
    }

    initCamera(&this->mainCamera, &this->view, &this->colCtx, this);
    changeCameraStatus(&this->mainCamera, CAM_STAT_ACTIVE);

    for (i = 0; i < 3; i++) {
        initCamera(&this->subCameras[i], &this->view, &this->colCtx, this);
        changeCameraStatus(&this->subCameras[i], CAM_STAT_UNK100);
    }

    this->cameraPtrs[CAM_ID_MAIN] = &this->mainCamera;
    this->cameraPtrs[CAM_ID_MAIN]->uid = 0;
    this->activeCamId = CAM_ID_MAIN;
    changeCameraByPolygon(&this->mainCamera, CAM_STATE_CHECK_BG_ALT | CAM_STATE_CHECK_WATER | CAM_STATE_CHECK_BG |
                                                      CAM_STATE_EXTERNAL_FINISHED | CAM_STATE_CAM_FUNC_FINISH |
                                                      CAM_STATE_LOCK_MODE | CAM_STATE_DISTORTION | CAM_STATE_PLAY_INIT);
    sram_ct(this, &this->sramCtx);
    save_area_ct(this);
    message_ct(this);
    gameover_ct(this);
    Effect_SE_Info_ct(this);
    EffectInit(this);
    EffectSoftSprite_ct(this, 0x55);
    CollisionCheck_ct(this, &this->colChkCtx);
    Skeleton_Proc_ct(&this->animTaskQueue);
    Demo_play_ct(this, &this->csCtx);

    if (z_common_data.nextCutsceneIndex != 0xFFEF) {
        z_common_data.save.cutsceneIndex = z_common_data.nextCutsceneIndex;
        z_common_data.nextCutsceneIndex = 0xFFEF;
    }

    if (z_common_data.save.cutsceneIndex == 0xFFFD) {
        z_common_data.save.cutsceneIndex = 0;
    }

    if (z_common_data.nextDayTime != NEXT_TIME_NONE) {
        z_common_data.save.dayTime = z_common_data.nextDayTime;
        z_common_data.skyboxTime = z_common_data.nextDayTime;
    }

    if (z_common_data.save.dayTime > CLOCK_TIME(18, 0) || z_common_data.save.dayTime < CLOCK_TIME(6, 30)) {
        z_common_data.save.nightFlag = 1;
    } else {
        z_common_data.save.nightFlag = 0;
    }

    DemoStartKansi(this);

    if (z_common_data.gameMode != GAMEMODE_NORMAL || z_common_data.save.cutsceneIndex >= 0xFFF0) {
        z_common_data.nayrusLoveTimer = 0;
        magic_cancel_check(this);
        z_common_data.sceneLayer = SCENE_LAYER_CUTSCENE_FIRST + (z_common_data.save.cutsceneIndex & 0xF);
    } else if (!LINK_IS_ADULT && IS_DAY) {
        z_common_data.sceneLayer = SCENE_LAYER_CHILD_DAY;
    } else if (!LINK_IS_ADULT && !IS_DAY) {
        z_common_data.sceneLayer = SCENE_LAYER_CHILD_NIGHT;
    } else if (LINK_IS_ADULT && IS_DAY) {
        z_common_data.sceneLayer = SCENE_LAYER_ADULT_DAY;
    } else {
        z_common_data.sceneLayer = SCENE_LAYER_ADULT_NIGHT;
    }

    // save the base scene layer (before accounting for the special cases below) to use later for the transition type
    baseSceneLayer = z_common_data.sceneLayer;

    if ((scene_status_data[((void)0, z_common_data.save.entranceIndex)].sceneId == SCENE_HYRULE_FIELD) && !LINK_IS_ADULT &&
        !IS_CUTSCENE_LAYER) {
        if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) && CHECK_QUEST_ITEM(QUEST_GORON_RUBY) &&
            CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
            z_common_data.sceneLayer = 1;
        } else {
            z_common_data.sceneLayer = 0;
        }
    } else if ((scene_status_data[((void)0, z_common_data.save.entranceIndex)].sceneId == SCENE_KOKIRI_FOREST) &&
               LINK_IS_ADULT && !IS_CUTSCENE_LAYER) {
        z_common_data.sceneLayer = GET_EVENTCHKINF(EVENTCHKINF_48) ? 3 : 2;
    }

    Gameplay_Scene_Read(
        this, scene_status_data[((void)0, z_common_data.save.entranceIndex) + ((void)0, z_common_data.sceneLayer)].sceneId,
        scene_status_data[((void)0, z_common_data.save.entranceIndex) + ((void)0, z_common_data.sceneLayer)].spawn);

    PRINTF("\nSCENE_NO=%d COUNTER=%d\n", ((void)0, z_common_data.save.entranceIndex), z_common_data.sceneLayer);

#if PLATFORM_GC && OOT_VERSION != HIRATSU3
    // When entering Gerudo Valley in the credits, trigger the GC emulator to play the ending movie.
    // The emulator constantly checks whether PC is 0x81000000, so this works even though it's not a valid address.
    if ((scene_status_data[((void)0, z_common_data.save.entranceIndex)].sceneId == SCENE_GERUDO_VALLEY) &&
        z_common_data.sceneLayer == 6) {
        PRINTF(T("エンディングはじまるよー\n", "The ending starts\n"));
        ((void (*)(void))0x81000000)();
        PRINTF(T("出戻り？\n", "Return?\n"));
    }
#endif

#if PLATFORM_N64
    if ((B_80121220 != NULL && B_80121220->unk_54 != NULL && B_80121220->unk_54(this))) {
    } else {
        SpotShowKansi(this);
    }
#else
    SpotShowKansi(this);
#endif

    kaleido_scope_ct_ovl(this);
    parameter_ct(this);

    if (z_common_data.nextDayTime != NEXT_TIME_NONE) {
        if (z_common_data.nextDayTime == NEXT_TIME_DAY) {
            z_common_data.save.totalDays++;
            z_common_data.save.bgsDayCount++;
            z_common_data.dogIsLost = true;

            if (item_change_setting(this, ITEM_WEIRD_EGG, ITEM_CHICKEN) ||
                item_change_setting(this, ITEM_POCKET_EGG, ITEM_POCKET_CUCCO)) {
                message_set(this, 0x3066, NULL);
            }

            z_common_data.nextDayTime = NEXT_TIME_DAY_SET;
        } else {
            z_common_data.nextDayTime = NEXT_TIME_NIGHT_SET;
        }
    }

    SREG(91) = -1;
    R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_OFF;
    PreRender_init(&this->pauseBgPreRender);
    PreRender_setup_savebuf(&this->pauseBgPreRender, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, NULL);
    PreRender_setup_renderbuf(&this->pauseBgPreRender, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL);
    fbdemo_mode = TRANS_TILE_OFF;
    this->transitionMode = TRANS_MODE_OFF;
    Pause_ct(&this->frameAdvCtx);
    sqrand((u32)osGetTime());
    new_Matrix(&this->state);
    this->state.main = play_main;
    this->state.destroy = play_cleanup;
    this->transitionTrigger = TRANS_TRIGGER_END;
    this->unk_11E16 = 0xFF;
    this->bgCoverAlpha = 0;
    this->haltAllActors = false;

    if (z_common_data.gameMode != GAMEMODE_TITLE_SCREEN) {
        if (z_common_data.nextTransitionType == TRANS_NEXT_TYPE_DEFAULT) {
            this->transitionType = ENTRANCE_INFO_END_TRANS_TYPE(
                scene_status_data[((void)0, z_common_data.save.entranceIndex) + baseSceneLayer].field);
        } else {
            this->transitionType = z_common_data.nextTransitionType;
            z_common_data.nextTransitionType = TRANS_NEXT_TYPE_DEFAULT;
        }
    } else {
        this->transitionType = TRANS_TYPE_FADE_BLACK_SLOW;
    }

    shrink_window_init();
    fbdemo_fade_init(&this->transitionFadeFlash);
    fbdemo_fade_settype(&this->transitionFadeFlash, TRANS_INSTANCE_TYPE_FADE_FLASH);
    fbdemo_fade_setcolor_rgba8888(&this->transitionFadeFlash, RGBA8(160, 160, 160, 255));
    fbdemo_fade_startup(&this->transitionFadeFlash);
    z_vismono_init(&z_play_vismono);
    z_play_vismono_color.a = 0;
    event_ct(this);

    PRINTF("ZELDA ALLOC SIZE=%x\n", THA_getFreeBytes(&this->state.tha));
    zAllocSize = THA_getFreeBytes(&this->state.tha);
    zAlloc = (uintptr_t)GAME_STATE_ALLOC(&this->state, zAllocSize, "../z_play.c", 2918);
    zAllocAligned = (zAlloc + 8) & ~0xF;
    zelda_InitArena((void*)zAllocAligned, zAllocSize - (zAllocAligned - zAlloc));
    PRINTF(T("ゼルダヒープ %08x-%08x\n", "Zelda Heap %08x-%08x\n"), zAllocAligned,
           (u8*)zAllocAligned + zAllocSize - (s32)(zAllocAligned - zAlloc));

#if PLATFORM_GC && DEBUG_FEATURES
    fault_AddClient(&D_801614B8, ZeldaArena_Display, NULL, NULL);
#endif

    Actor_info_ct(this, &this->actorCtx, this->playerEntry);

    // Busyloop until the room loads
    while (!Room_Info_exchange_check(this, &this->roomCtx)) {
        ; // Empty Loop
    }

    player = GET_PLAYER(this);
    leaveCamera(&this->mainCamera, player);
    changeCameraMode(&this->mainCamera, CAM_MODE_NORMAL);

    playerStartBgCamIndex = PLAYER_GET_START_BG_CAM_INDEX(&player->actor);

    if (playerStartBgCamIndex != PLAYER_START_BG_CAM_DEFAULT) {
        PRINTF("player has start camera ID (" VT_FGCOL(BLUE) "%d" VT_RST ")\n", playerStartBgCamIndex);
        changeCameraID(&this->mainCamera, playerStartBgCamIndex);
    }

    if (R_SCENE_CAM_TYPE == SCENE_CAM_TYPE_FIXED_TOGGLE_VIEWPOINT) {
        this->viewpoint = VIEWPOINT_PIVOT;
    } else if (R_SCENE_CAM_TYPE == SCENE_CAM_TYPE_FIXED_SHOP_VIEWPOINT) {
        this->viewpoint = VIEWPOINT_LOCKED;
    } else {
        this->viewpoint = VIEWPOINT_NONE;
    }

    button_status_set(this);
    stagemusic_play(this);
    z_common_data.seqId = this->sceneSequences.seqId;
    z_common_data.natureAmbienceId = this->sceneSequences.natureAmbienceId;
    Actor_Horse_Set_Check(this, GET_PLAYER(this));
    Skeleton_Proc_move(this, &this->animTaskQueue);
    z_common_data.respawnFlag = 0;

#if DEBUG_FEATURES
    if (R_USE_DEBUG_CUTSCENE) {
        static u64 sDebugCutsceneScriptBuf[0xA00];

        gDebugCutsceneScript = sDebugCutsceneScriptBuf;
        PRINTF("\nkawauso_data=[%x]", gDebugCutsceneScript);

        // This hardcoded ROM address extends past the end of the ROM file.
        // Presumably the ROM was larger at a previous point in development when this debug feature was used.
        percial_DMA(0x03FEB000, gDebugCutsceneScript, sizeof(sDebugCutsceneScriptBuf));
    }
#endif
}

void Game_play_move(PlayState* this) {
    Input* input = this->state.input;
    s32 isPaused;
    s32 pad1;

#if DEBUG_FEATURES
    if ((SREG(1) < 0) || (DREG(0) != 0)) {
        SREG(1) = 0;
#if PLATFORM_GC
        ZeldaArena_Display();
#endif
    }

    if ((R_HREG_MODE == HREG_MODE_PRINT_OBJECT_TABLE) && (R_PRINT_OBJECT_TABLE_TRIGGER < 0)) {
        u32 i;
        s32 pad2;

        R_PRINT_OBJECT_TABLE_TRIGGER = 0;
        PRINTF("object_exchange_rom_address %u\n", object_exchange_bank_max);
        PRINTF("RomStart RomEnd   Size\n");

        for (i = 0; i < object_exchange_bank_max; i++) {
            s32 size = object_exchange_rom_address[i].vromEnd - object_exchange_rom_address[i].vromStart;

            PRINTF("%08x-%08x %08x(%8.3fKB)\n", object_exchange_rom_address[i].vromStart, object_exchange_rom_address[i].vromEnd, size,
                   size / 1024.0f);
        }

        PRINTF("\n");
    }

    // HREG(81) was very likely intended to be HREG(80), which would make more sense given how the
    // HREG debugging system works. If this condition used HREG(80) instead, `HREG_MODE_PRINT_OBJECT_TABLE`
    // would also include the actor overlay table and HREG(82) would be used to trigger it instead.
    if ((HREG(81) == 18) && (HREG(82) < 0)) {
        HREG(82) = 0;
        actor_dlftbls_show_info();
    }
#endif

    SegmentBaseAddress[4] = VIRTUAL_TO_PHYSICAL(this->objectCtx.slots[this->objectCtx.mainKeepSlot].segment);
    SegmentBaseAddress[5] = VIRTUAL_TO_PHYSICAL(this->objectCtx.slots[this->objectCtx.subKeepSlot].segment);
    SegmentBaseAddress[2] = VIRTUAL_TO_PHYSICAL(this->sceneSegment);

    if (Pause_proc(&this->frameAdvCtx, &input[1])) {
        if ((this->transitionMode == TRANS_MODE_OFF) && (this->transitionTrigger != TRANS_TRIGGER_OFF)) {
            this->transitionMode = TRANS_MODE_SETUP;
        }

        if (fbdemo_mode != TRANS_TILE_OFF) {
            switch (fbdemo_mode) {
                case TRANS_TILE_PROCESS:
                    if (fbdemo_init(&fbdemo, 10, 7) == NULL) {
                        PRINTF(T("fbdemo_init呼出し失敗！\n", "fbdemo_init call failed!\n"));
                        fbdemo_mode = TRANS_TILE_OFF;
                    } else {
                        fbdemo.zBuffer = (u16*)sys_zb;
                        fbdemo_mode = TRANS_TILE_READY;
                        R_UPDATE_RATE = 1;
                    }
                    break;

                case TRANS_TILE_READY:
                    fbdemo_move(&fbdemo);
                    break;

                default:
                    break;
            }
        }

        if ((u32)this->transitionMode != TRANS_MODE_OFF) {
            switch (this->transitionMode) {
                case TRANS_MODE_SETUP:
                    if (this->transitionTrigger != TRANS_TRIGGER_END) {
                        s16 sceneLayer = SCENE_LAYER_CHILD_DAY;

                        alpha_change(HUD_VISIBILITY_NOTHING);

                        if (z_common_data.save.cutsceneIndex >= 0xFFF0) {
                            sceneLayer = SCENE_LAYER_CUTSCENE_FIRST + (z_common_data.save.cutsceneIndex & 0xF);
                        }

                        // fade out bgm if "continue bgm" flag is not set
                        if (!(scene_status_data[this->nextEntranceIndex + sceneLayer].field &
                              ENTRANCE_INFO_CONTINUE_BGM_FLAG)) {
                            PRINTF(T("\n\n\nサウンドイニシャル来ました。111", "\n\n\nSound initialized. 111"));
                            if ((this->transitionType < TRANS_TYPE_MAX) && !Nottori_Bgmoff_check()) {
                                PRINTF(T("\n\n\nサウンドイニシャル来ました。222", "\n\n\nSound initialized. 222"));
                                Na_StopAllSound(0x14);
                                z_common_data.seqId = (u8)NA_BGM_DISABLED;
                                z_common_data.natureAmbienceId = NATURE_ID_DISABLED;
                            }
                        }
                    }

#if DEBUG_FEATURES
                    if (!R_TRANS_DBG_ENABLED) {
                        Game_play_fbdemo_wipe_create(this, this->transitionType);
                    } else {
                        Game_play_fbdemo_wipe_create(this, R_TRANS_DBG_TYPE);
                    }
#else
                    Game_play_fbdemo_wipe_create(this, this->transitionType);
#endif

                    if (this->transitionMode >= TRANS_MODE_FILL_WHITE_INIT) {
                        // non-instance modes break out of this switch
                        break;
                    }
                    FALLTHROUGH;
                case TRANS_MODE_INSTANCE_INIT:
                    this->transitionCtx.init(&this->transitionCtx.instanceData);

                    // circle types
                    if ((this->transitionCtx.transitionType >> 5) == 1) {
                        this->transitionCtx.setType(&this->transitionCtx.instanceData,
                                                    this->transitionCtx.transitionType | TC_SET_PARAMS);
                    }

                    z_common_data.transWipeSpeed = 14;

                    if ((this->transitionCtx.transitionType == TRANS_TYPE_WIPE_FAST) ||
                        (this->transitionCtx.transitionType == TRANS_TYPE_FILL_WHITE2)) {
                        //! @bug TRANS_TYPE_FILL_WHITE2 will never reach this code.
                        //! It is a non-instance type transition which doesn't run this case.
                        z_common_data.transWipeSpeed = 28;
                    }

                    z_common_data.transFadeDuration = 60;

                    if ((this->transitionCtx.transitionType == TRANS_TYPE_FADE_BLACK_FAST) ||
                        (this->transitionCtx.transitionType == TRANS_TYPE_FADE_WHITE_FAST)) {
                        z_common_data.transFadeDuration = 20;
                    } else if ((this->transitionCtx.transitionType == TRANS_TYPE_FADE_BLACK_SLOW) ||
                               (this->transitionCtx.transitionType == TRANS_TYPE_FADE_WHITE_SLOW)) {
                        z_common_data.transFadeDuration = 150;
                    } else if (this->transitionCtx.transitionType == TRANS_TYPE_FADE_WHITE_INSTANT) {
                        z_common_data.transFadeDuration = 2;
                    }

                    if ((this->transitionCtx.transitionType == TRANS_TYPE_FADE_WHITE) ||
                        (this->transitionCtx.transitionType == TRANS_TYPE_FADE_WHITE_FAST) ||
                        (this->transitionCtx.transitionType == TRANS_TYPE_FADE_WHITE_SLOW) ||
                        (this->transitionCtx.transitionType == TRANS_TYPE_FADE_WHITE_CS_DELAYED) ||
                        (this->transitionCtx.transitionType == TRANS_TYPE_FADE_WHITE_INSTANT)) {
                        this->transitionCtx.setColor(&this->transitionCtx.instanceData, RGBA8(160, 160, 160, 255));

                        if (this->transitionCtx.setUnkColor != NULL) {
                            this->transitionCtx.setUnkColor(&this->transitionCtx.instanceData,
                                                            RGBA8(160, 160, 160, 255));
                        }
                    } else if (this->transitionCtx.transitionType == TRANS_TYPE_FADE_GREEN) {
                        this->transitionCtx.setColor(&this->transitionCtx.instanceData, RGBA8(140, 140, 100, 255));

                        if (this->transitionCtx.setUnkColor != NULL) {
                            this->transitionCtx.setUnkColor(&this->transitionCtx.instanceData,
                                                            RGBA8(140, 140, 100, 255));
                        }
                    } else if (this->transitionCtx.transitionType == TRANS_TYPE_FADE_BLUE) {
                        this->transitionCtx.setColor(&this->transitionCtx.instanceData, RGBA8(70, 100, 110, 255));

                        if (this->transitionCtx.setUnkColor != NULL) {
                            this->transitionCtx.setUnkColor(&this->transitionCtx.instanceData,
                                                            RGBA8(70, 100, 110, 255));
                        }
                    } else {
                        this->transitionCtx.setColor(&this->transitionCtx.instanceData, RGBA8(0, 0, 0, 0));

                        if (this->transitionCtx.setUnkColor != NULL) {
                            this->transitionCtx.setUnkColor(&this->transitionCtx.instanceData, RGBA8(0, 0, 0, 0));
                        }
                    }

                    if (this->transitionTrigger == TRANS_TRIGGER_END) {
                        this->transitionCtx.setType(&this->transitionCtx.instanceData, TRANS_INSTANCE_TYPE_FILL_OUT);
                    } else {
                        this->transitionCtx.setType(&this->transitionCtx.instanceData, TRANS_INSTANCE_TYPE_FILL_IN);
                    }

                    this->transitionCtx.start(&this->transitionCtx.instanceData);

                    if (this->transitionCtx.transitionType == TRANS_TYPE_FADE_WHITE_CS_DELAYED) {
                        this->transitionMode = TRANS_MODE_INSTANCE_WAIT;
                    } else {
                        this->transitionMode = TRANS_MODE_INSTANCE_RUNNING;
                    }
                    break;

                case TRANS_MODE_INSTANCE_RUNNING:
                    if (this->transitionCtx.isDone(&this->transitionCtx.instanceData)) {
                        if (this->transitionCtx.transitionType >= TRANS_TYPE_MAX) {
                            if (this->transitionTrigger == TRANS_TRIGGER_END) {
                                this->transitionCtx.destroy(&this->transitionCtx.instanceData);
                                Game_play_fbdemo_wipe_destroy(this);
                                this->transitionMode = TRANS_MODE_OFF;
                            }
                        } else if (this->transitionTrigger != TRANS_TRIGGER_END) {
                            this->state.running = false;

                            if (z_common_data.gameMode != GAMEMODE_FILE_SELECT) {
                                SET_NEXT_GAMESTATE(&this->state, play_init, PlayState);
                                z_common_data.save.entranceIndex = this->nextEntranceIndex;

                                if (z_common_data.minigameState == 1) {
                                    z_common_data.minigameState = 3;
                                }
                            } else {
                                SET_NEXT_GAMESTATE(&this->state, file_choose_init, FileSelectState);
                            }
                        } else {
                            this->transitionCtx.destroy(&this->transitionCtx.instanceData);
                            Game_play_fbdemo_wipe_destroy(this);
                            this->transitionMode = TRANS_MODE_OFF;

                            if (fbdemo_mode == TRANS_TILE_READY) {
                                fbdemo_cleanup(&fbdemo);
                                fbdemo_mode = TRANS_TILE_OFF;
                                R_UPDATE_RATE = 3;
                            }
                        }

                        this->transitionTrigger = TRANS_TRIGGER_OFF;
                    } else {
                        this->transitionCtx.update(&this->transitionCtx.instanceData, R_UPDATE_RATE);
                    }
                    break;
            }

            // update non-instance transitions
            switch (this->transitionMode) {
                case TRANS_MODE_FILL_WHITE_INIT:
                    demofade_flame = 0;
                    this->envCtx.fillScreen = true;
                    this->envCtx.screenFillColor[0] = 160;
                    this->envCtx.screenFillColor[1] = 160;
                    this->envCtx.screenFillColor[2] = 160;

                    if (this->transitionTrigger != TRANS_TRIGGER_END) {
                        this->envCtx.screenFillColor[3] = 0;
                        this->transitionMode = TRANS_MODE_FILL_IN;
                    } else {
                        this->envCtx.screenFillColor[3] = 255;
                        this->transitionMode = TRANS_MODE_FILL_OUT;
                    }
                    break;

                case TRANS_MODE_FILL_IN:
                    this->envCtx.screenFillColor[3] = (demofade_flame / 20.0f) * 255.0f;

                    if (demofade_flame >= 20) {
                        this->state.running = false;
                        SET_NEXT_GAMESTATE(&this->state, play_init, PlayState);
                        z_common_data.save.entranceIndex = this->nextEntranceIndex;
                        this->transitionTrigger = TRANS_TRIGGER_OFF;
                        this->transitionMode = TRANS_MODE_OFF;
                    } else {
                        demofade_flame++;
                    }
                    break;

                case TRANS_MODE_FILL_OUT:
                    this->envCtx.screenFillColor[3] = (1 - demofade_flame / 20.0f) * 255.0f;

                    if (demofade_flame >= 20) {
                        fbdemo_mode = TRANS_TILE_OFF;
                        R_UPDATE_RATE = 3;
                        this->transitionTrigger = TRANS_TRIGGER_OFF;
                        this->transitionMode = TRANS_MODE_OFF;
                        this->envCtx.fillScreen = false;
                    } else {
                        demofade_flame++;
                    }
                    break;

                case TRANS_MODE_FILL_BROWN_INIT:
                    demofade_flame = 0;
                    this->envCtx.fillScreen = true;
                    this->envCtx.screenFillColor[0] = 170;
                    this->envCtx.screenFillColor[1] = 160;
                    this->envCtx.screenFillColor[2] = 150;

                    if (this->transitionTrigger != TRANS_TRIGGER_END) {
                        this->envCtx.screenFillColor[3] = 0;
                        this->transitionMode = TRANS_MODE_FILL_IN;
                    } else {
                        this->envCtx.screenFillColor[3] = 255;
                        this->transitionMode = TRANS_MODE_FILL_OUT;
                    }
                    break;

                case TRANS_MODE_INSTANT:
                    if (this->transitionTrigger != TRANS_TRIGGER_END) {
                        this->state.running = false;
                        SET_NEXT_GAMESTATE(&this->state, play_init, PlayState);
                        z_common_data.save.entranceIndex = this->nextEntranceIndex;
                        this->transitionTrigger = TRANS_TRIGGER_OFF;
                        this->transitionMode = TRANS_MODE_OFF;
                    } else {
                        fbdemo_mode = TRANS_TILE_OFF;
                        R_UPDATE_RATE = 3;
                        this->transitionTrigger = TRANS_TRIGGER_OFF;
                        this->transitionMode = TRANS_MODE_OFF;
                    }
                    break;

                case TRANS_MODE_INSTANCE_WAIT:
                    if (z_common_data.cutsceneTransitionControl != 0) {
                        this->transitionMode = TRANS_MODE_INSTANCE_RUNNING;
                    }
                    break;

                case TRANS_MODE_SANDSTORM_INIT:
                    if (this->transitionTrigger != TRANS_TRIGGER_END) {
                        this->envCtx.sandstormState = SANDSTORM_FILL;
                        this->transitionMode = TRANS_MODE_SANDSTORM;
                    } else {
                        this->envCtx.sandstormState = SANDSTORM_UNFILL;
                        this->envCtx.sandstormPrimA = 255;
                        this->envCtx.sandstormEnvA = 255;
                        this->transitionMode = TRANS_MODE_SANDSTORM;
                    }
                    break;

                case TRANS_MODE_SANDSTORM:
                    Nai_FxFlagEntry(NA_SE_EV_SAND_STORM - SFX_FLAG, &_dummy_zero_f, 4,
                                         &_dummy_one, &_dummy_one, &_dummy_zero_s8);

                    if (this->transitionTrigger == TRANS_TRIGGER_END) {
                        if (this->envCtx.sandstormPrimA < 110) {
                            fbdemo_mode = TRANS_TILE_OFF;
                            R_UPDATE_RATE = 3;
                            this->transitionTrigger = TRANS_TRIGGER_OFF;
                            this->transitionMode = TRANS_MODE_OFF;
                        }
                    } else {
                        if (this->envCtx.sandstormEnvA == 255) {
                            this->state.running = false;
                            SET_NEXT_GAMESTATE(&this->state, play_init, PlayState);
                            z_common_data.save.entranceIndex = this->nextEntranceIndex;
                            this->transitionTrigger = TRANS_TRIGGER_OFF;
                            this->transitionMode = TRANS_MODE_OFF;
                        }
                    }
                    break;

                case TRANS_MODE_SANDSTORM_END_INIT:
                    if (this->transitionTrigger == TRANS_TRIGGER_END) {
                        this->envCtx.sandstormState = SANDSTORM_DISSIPATE;
                        this->envCtx.sandstormPrimA = 255;
                        this->envCtx.sandstormEnvA = 255;
                        // "It's here!!!!!!!!!"
                        LOG_STRING("来た!!!!!!!!!!!!!!!!!!!!!", "../z_play.c", 3471);
                        this->transitionMode = TRANS_MODE_SANDSTORM_END;
                    } else {
                        this->transitionMode = TRANS_MODE_SANDSTORM_INIT;
                    }
                    break;

                case TRANS_MODE_SANDSTORM_END:
                    Nai_FxFlagEntry(NA_SE_EV_SAND_STORM - SFX_FLAG, &_dummy_zero_f, 4,
                                         &_dummy_one, &_dummy_one, &_dummy_zero_s8);

                    if (this->transitionTrigger == TRANS_TRIGGER_END) {
                        if (this->envCtx.sandstormPrimA <= 0) {
                            fbdemo_mode = TRANS_TILE_OFF;
                            R_UPDATE_RATE = 3;
                            this->transitionTrigger = TRANS_TRIGGER_OFF;
                            this->transitionMode = TRANS_MODE_OFF;
                        }
                    }
                    break;

                case TRANS_MODE_CS_BLACK_FILL_INIT:
                    demofade_flame = 0;
                    this->envCtx.fillScreen = true;
                    this->envCtx.screenFillColor[0] = 0;
                    this->envCtx.screenFillColor[1] = 0;
                    this->envCtx.screenFillColor[2] = 0;
                    this->envCtx.screenFillColor[3] = 255;
                    this->transitionMode = TRANS_MODE_CS_BLACK_FILL;
                    break;

                case TRANS_MODE_CS_BLACK_FILL:
                    if (z_common_data.cutsceneTransitionControl != 0) {
                        this->envCtx.screenFillColor[3] = z_common_data.cutsceneTransitionControl;

                        if (z_common_data.cutsceneTransitionControl <= 100) {
                            fbdemo_mode = TRANS_TILE_OFF;
                            R_UPDATE_RATE = 3;
                            this->transitionTrigger = TRANS_TRIGGER_OFF;
                            this->transitionMode = TRANS_MODE_OFF;
                        }
                    }
                    break;
            }
        }

        PLAY_LOG(3533);

        if (1 && (fbdemo_mode != TRANS_TILE_READY)) {
            PLAY_LOG(3542);

            if ((z_common_data.gameMode == GAMEMODE_NORMAL) && (this->msgCtx.msgMode == MSGMODE_NONE) &&
                (this->gameOverCtx.state == GAMEOVER_INACTIVE)) {
                kaleido_scope_pause(this);
            }

            PLAY_LOG(3551);
            isPaused = IS_PAUSED(&this->pauseCtx);

            PLAY_LOG(3555);
            Skeleton_Proc_ct(&this->animTaskQueue);

            if (!DEBUG_FEATURES) {}

            PLAY_LOG(3561);
            Object_Exchange_check(&this->objectCtx);

            PLAY_LOG(3577);

            if (!isPaused && (IREG(72) == 0)) {
                PLAY_LOG(3580);

                this->gameplayFrames++;
                z_vibctl2_pause(true);

                if (this->actorCtx.freezeFlashTimer && (this->actorCtx.freezeFlashTimer-- < 5)) {
                    PRINTF("FINISH=%d\n", this->actorCtx.freezeFlashTimer);

                    if ((this->actorCtx.freezeFlashTimer > 0) && ((this->actorCtx.freezeFlashTimer % 2) != 0)) {
                        this->envCtx.fillScreen = true;
                        this->envCtx.screenFillColor[0] = this->envCtx.screenFillColor[1] =
                            this->envCtx.screenFillColor[2] = 150;
                        this->envCtx.screenFillColor[3] = 80;
                    } else {
                        this->envCtx.fillScreen = false;
                    }
                } else {
                    PLAY_LOG(3606);
                    Room_Info_exchange_check(this, &this->roomCtx);

                    PLAY_LOG(3612);
                    CollisionCheck_ATAC(this, &this->colChkCtx);

                    PLAY_LOG(3618);
                    CollisionCheck_OC(this, &this->colChkCtx);

                    PLAY_LOG(3624);
                    CollisionCheck_Damage_Calc(this, &this->colChkCtx);

                    PLAY_LOG(3631);
                    CollisionCheck_clear(this, &this->colChkCtx);

                    PLAY_LOG(3637);

                    if (!this->haltAllActors) {
                        Actor_info_call_actor(this, &this->actorCtx);
                    }

                    PLAY_LOG(3643);
                    Demo_play_move(this, &this->csCtx);

                    PLAY_LOG(3648);
                    Tool_Demo_play_move(this, &this->csCtx);

                    PLAY_LOG(3651);
                    EffectProc(this);

                    PLAY_LOG(3657);
                    EffectSoftSprite_proc(this);

                    PLAY_LOG(3662);
                }
            } else {
                z_vibctl2_pause(false);
            }

            PLAY_LOG(3672);
            Room_Move(this, &this->roomCtx.curRoom, &input[1], 0);

            PLAY_LOG(3675);
            Room_Move(this, &this->roomCtx.prevRoom, &input[1], 1);

            PLAY_LOG(3677);

            if (this->viewpoint != VIEWPOINT_NONE) {
                if (CHECK_BTN_ALL(input[0].press.button, BTN_CUP)) {
                    if (IS_PAUSED(&this->pauseCtx)) {
                        PRINTF(VT_FGCOL(CYAN) T("カレイドスコープ中につき視点変更を禁止しております\n",
                                                "Changing viewpoint is prohibited due to the kaleidoscope\n") VT_RST);
                    } else if (player_demo_check(this)) {
                        PRINTF(VT_FGCOL(CYAN) T("デモ中につき視点変更を禁止しております\n",
                                                "Changing viewpoint is prohibited during the cutscene\n") VT_RST);
                    } else if (R_SCENE_CAM_TYPE == SCENE_CAM_TYPE_FIXED_SHOP_VIEWPOINT) {
                        Nai_FxFlagEntry(NA_SE_SY_ERROR, &_dummy_zero_f, 4, &_dummy_one,
                                             &_dummy_one, &_dummy_zero_s8);
                    } else {
                        // C-Up toggle for houses, move between pivot camera and fixed camera
                        // Toggle viewpoint between VIEWPOINT_LOCKED and VIEWPOINT_PIVOT
                        Game_play_pr_vr_switch_pr_set(this, this->viewpoint ^ (VIEWPOINT_LOCKED ^ VIEWPOINT_PIVOT));
                    }
                }

                Game_play_pr_vr_camera_update(this);
            }

            PLAY_LOG(3708);
            vr_box_move(&this->skyboxCtx);

            PLAY_LOG(3716);

            if (IS_PAUSED(&this->pauseCtx)) {
                PLAY_LOG(3721);
                kaleido_scope_move_ovl(this);
            } else if (this->gameOverCtx.state != GAMEOVER_INACTIVE) {
                PLAY_LOG(3727);
                gameover_move(this);
            } else {
                PLAY_LOG(3733);
                message_move(this);
            }

            PLAY_LOG(3737);

            PLAY_LOG(3742);
            parameter_move(this);

            PLAY_LOG(3765);
            Skeleton_Proc_move(this, &this->animTaskQueue);

            PLAY_LOG(3771);
            Effect_SE_Info_proc(this);

            PLAY_LOG(3777);
            shrink_window_move(R_UPDATE_RATE);

            PLAY_LOG(3783);
            fbdemo_fade_move(&this->transitionFadeFlash, R_UPDATE_RATE);
        } else {
            goto skip;
        }
    }

    PLAY_LOG(3799);

skip:
    PLAY_LOG(3801);

    //! @bug If frame advancing or during tile transitions, isPaused will be used uninitialized.
    if (!isPaused || debug_camera_sw) {
        s32 i;

        this->nextCamId = this->activeCamId;

        PLAY_LOG(3806);

        for (i = 0; i < NUM_CAMS; i++) {
            if ((i != this->nextCamId) && (this->cameraPtrs[i] != NULL)) {
                PLAY_LOG(3809);
                actionCameraWork(this->cameraPtrs[i]);
            }
        }

        actionCameraWork(this->cameraPtrs[this->nextCamId]);

        PLAY_LOG(3814);
    }

    PLAY_LOG(3816);
    Global_kankyo_set(this, &this->envCtx, &this->lightCtx, &this->pauseCtx, &this->msgCtx, &this->gameOverCtx,
                       this->state.gfxCtx);
}

void Game_play_draw_overlay(PlayState* this) {
#if PLATFORM_N64
    s32 pad;
#endif

    if (IS_PAUSED(&this->pauseCtx)) {
        kaleido_scope_draw_ovl(this);
    }

    if (z_common_data.gameMode == GAMEMODE_NORMAL) {
        parameter_draw(this);
    }

    message_draw(this);

    if (this->gameOverCtx.state != GAMEOVER_INACTIVE) {
        gameover_draw(this);
    }
}

void Game_play_draw(PlayState* this) {
    GraphicsContext* gfxCtx = this->state.gfxCtx;
    Lights* sp228;
    Vec3f sp21C;

    OPEN_DISPS(gfxCtx, "../z_play.c", 3907);

    SegmentBaseAddress[4] = VIRTUAL_TO_PHYSICAL(this->objectCtx.slots[this->objectCtx.mainKeepSlot].segment);
    SegmentBaseAddress[5] = VIRTUAL_TO_PHYSICAL(this->objectCtx.slots[this->objectCtx.subKeepSlot].segment);
    SegmentBaseAddress[2] = VIRTUAL_TO_PHYSICAL(this->sceneSegment);

    gSPSegment(POLY_OPA_DISP++, 0x00, NULL);
    gSPSegment(POLY_XLU_DISP++, 0x00, NULL);
    gSPSegment(OVERLAY_DISP++, 0x00, NULL);

    gSPSegment(POLY_OPA_DISP++, 0x04, this->objectCtx.slots[this->objectCtx.mainKeepSlot].segment);
    gSPSegment(POLY_XLU_DISP++, 0x04, this->objectCtx.slots[this->objectCtx.mainKeepSlot].segment);
    gSPSegment(OVERLAY_DISP++, 0x04, this->objectCtx.slots[this->objectCtx.mainKeepSlot].segment);

    gSPSegment(POLY_OPA_DISP++, 0x05, this->objectCtx.slots[this->objectCtx.subKeepSlot].segment);
    gSPSegment(POLY_XLU_DISP++, 0x05, this->objectCtx.slots[this->objectCtx.subKeepSlot].segment);
    gSPSegment(OVERLAY_DISP++, 0x05, this->objectCtx.slots[this->objectCtx.subKeepSlot].segment);

    gSPSegment(POLY_OPA_DISP++, 0x02, this->sceneSegment);
    gSPSegment(POLY_XLU_DISP++, 0x02, this->sceneSegment);
    gSPSegment(OVERLAY_DISP++, 0x02, this->sceneSegment);

    DisplayList_initialize(gfxCtx, 0, 0, 0);

    if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) || R_PLAY_RUN_DRAW) {
        POLY_OPA_DISP = game_play_set_fog(this, POLY_OPA_DISP);
        POLY_XLU_DISP = game_play_set_fog(this, POLY_XLU_DISP);

        setPerspectiveView(&this->view, this->view.fovy, this->view.zNear, this->lightCtx.zFar);
        showView(&this->view, VIEW_ALL);

        // The billboard matrix temporarily stores the viewing matrix
        Matrix_MtxtoMtxF(&this->view.viewing, &this->billboardMtxF);
        Matrix_MtxtoMtxF(&this->view.projection, &this->viewProjectionMtxF);
        Matrix_mult(&this->viewProjectionMtxF, MTXMODE_NEW);
        // The billboard is still a viewing matrix at this stage
        Matrix_mult(&this->billboardMtxF, MTXMODE_APPLY);
        Matrix_get(&this->viewProjectionMtxF);
        this->billboardMtxF.mf[0][3] = this->billboardMtxF.mf[1][3] = this->billboardMtxF.mf[2][3] =
            this->billboardMtxF.mf[3][0] = this->billboardMtxF.mf[3][1] = this->billboardMtxF.mf[3][2] = 0.0f;
        // This transpose is where the viewing matrix is properly converted into a billboard matrix
        Matrix_reverse(&this->billboardMtxF);
        this->billboardMtx = _MtxF_to_Mtx(MATRIX_CHECK_FLOATS(&this->billboardMtxF, "../z_play.c", 4005),
                                              GRAPH_ALLOC(gfxCtx, sizeof(Mtx)));

        gSPSegment(POLY_OPA_DISP++, 0x01, this->billboardMtx);

        if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) || R_PLAY_DRAW_COVER_ELEMENTS) {
            Gfx* gfxP;
            Gfx* sp1CC = POLY_OPA_DISP;

            gfxP = gfxopen(sp1CC);
            gSPDisplayList(OVERLAY_DISP++, gfxP);

            if ((this->transitionMode == TRANS_MODE_INSTANCE_RUNNING) ||
                (this->transitionMode == TRANS_MODE_INSTANCE_WAIT) || (this->transitionCtx.transitionType >= 56)) {
                View view;

                initView(&view, gfxCtx);
                view.flags = VIEW_VIEWPORT | VIEW_PROJECTION_ORTHO;

                SET_FULLSCREEN_VIEWPORT(&view);

                showView1(&view, VIEW_ALL, &gfxP);
                this->transitionCtx.draw(&this->transitionCtx.instanceData, &gfxP);
            }

            fbdemo_fade_draw(&this->transitionFadeFlash, &gfxP);

#if PLATFORM_N64
            if (z_play_vismono_color.a != 0)
#else
            if (z_play_vismono_color.a > 0)
#endif
            {
                z_play_vismono.vis.primColor.rgba = z_play_vismono_color.rgba;
                z_vismono_draw(&z_play_vismono, &gfxP);
            }

            gSPEndDisplayList(gfxP++);
            gfxclose(sp1CC, gfxP);
            POLY_OPA_DISP = gfxP;
        }

        if (fbdemo_mode == TRANS_TILE_READY) {
            Gfx* sp88 = POLY_OPA_DISP;

            fbdemo_draw(&fbdemo, &sp88);
            POLY_OPA_DISP = sp88;
            goto Play_Draw_DrawOverlayElements;
        }

        PreRender_setup_renderbuf(&this->pauseBgPreRender, SCREEN_WIDTH, SCREEN_HEIGHT, gfxCtx->curFrameBuffer, sys_zb);

        if (R_PAUSE_BG_PRERENDER_STATE == PAUSE_BG_PRERENDER_PROCESS) {
            // Wait for the previous frame's display list to be processed,
            // so that `pauseBgPreRender.fbufSave` and `pauseBgPreRender.cvgSave` are filled with the appropriate
            // content and can be used by `PreRender_ConvertFrameBuffer2` below.
            nulltask();

            PreRender_ConvertFrameBuffer2(&this->pauseBgPreRender);

            R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_READY;
        } else if (R_PAUSE_BG_PRERENDER_STATE >= PAUSE_BG_PRERENDER_MAX) {
            R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_OFF;
        }

        if (R_PAUSE_BG_PRERENDER_STATE == PAUSE_BG_PRERENDER_READY) {
            Gfx* gfxP = POLY_OPA_DISP;

            PreRender_loadFrameBufferCopy(&this->pauseBgPreRender, &gfxP);
            POLY_OPA_DISP = gfxP;

            goto Play_Draw_DrawOverlayElements;
        }

        if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) || R_PLAY_DRAW_SKYBOX) {
            if (this->skyboxId && (this->skyboxId != SKYBOX_UNSET_1D) && !this->envCtx.skyboxDisabled) {
                if ((this->skyboxId == SKYBOX_NORMAL_SKY) || (this->skyboxId == SKYBOX_CUTSCENE_MAP)) {
                    Kankyo_vrbox_change(this->skyboxId, &this->envCtx, &this->skyboxCtx);
                    vr_box_draw(&this->skyboxCtx, gfxCtx, this->skyboxId, this->envCtx.skyboxBlend, this->view.eye.x,
                                this->view.eye.y, this->view.eye.z);
                } else if (this->skyboxCtx.drawType == SKYBOX_DRAW_128) {
                    vr_box_draw(&this->skyboxCtx, gfxCtx, this->skyboxId, 0, this->view.eye.x, this->view.eye.y,
                                this->view.eye.z);
                }
            }
        }

        if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) ||
            (R_PLAY_DRAW_ENV_FLAGS & PLAY_ENV_DRAW_SUN_AND_MOON)) {
            if (!this->envCtx.sunMoonDisabled) {
                sun_moon_disp(this);
            }
        }

        if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) ||
            (R_PLAY_DRAW_ENV_FLAGS & PLAY_ENV_DRAW_SKYBOX_FILTERS)) {
            vr_kakusi(this);
        }

        if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) || (R_PLAY_DRAW_ENV_FLAGS & PLAY_ENV_DRAW_LIGHTNING)) {
            electricthander(this);
            scene_proc_thunder(this, 0);
        }

        if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) || (R_PLAY_DRAW_ENV_FLAGS & PLAY_ENV_DRAW_LIGHTS)) {
            sp228 = Global_light_read(&this->lightCtx, gfxCtx);
            LightsN_list_check(sp228, this->lightCtx.listHead, NULL);
            LightsN_disp(sp228, gfxCtx);
        }

        if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) || (R_PLAY_DRAW_ROOM_FLAGS != 0)) {
            if (VREG(94) == 0) {
                s32 roomDrawFlags;

                if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY)) {
                    roomDrawFlags = ROOM_DRAW_OPA | ROOM_DRAW_XLU;
                } else {
                    roomDrawFlags = R_PLAY_DRAW_ROOM_FLAGS;
                }
                call_scene_proc(this);
                Room_Draw(this, &this->roomCtx.curRoom, roomDrawFlags & (ROOM_DRAW_OPA | ROOM_DRAW_XLU));
                Room_Draw(this, &this->roomCtx.prevRoom, roomDrawFlags & (ROOM_DRAW_OPA | ROOM_DRAW_XLU));
            }
        }

        if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) || R_PLAY_DRAW_SKYBOX) {
            if ((this->skyboxCtx.drawType != SKYBOX_DRAW_128) &&
                (GET_ACTIVE_CAM(this)->setting != CAM_SET_PREREND_FIXED)) {
                Vec3f quakeOffset;

                quakeOffset = getCameraGap(GET_ACTIVE_CAM(this));
                vr_box_draw(&this->skyboxCtx, gfxCtx, this->skyboxId, 0, this->view.eye.x + quakeOffset.x,
                            this->view.eye.y + quakeOffset.y, this->view.eye.z + quakeOffset.z);
            }
        }

        if (this->envCtx.precipitation[PRECIP_RAIN_CUR] != 0) {
            rain_disp(this, &this->view, gfxCtx);
        }

        if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) || (R_PLAY_DRAW_ROOM_FLAGS != 0)) {
            Display_Fillter(gfxCtx, 0, 0, 0, this->bgCoverAlpha, FILL_SCREEN_OPA);
        }

        if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) || R_PLAY_DRAW_ACTORS) {
            Actor_info_draw_actor(this, &this->actorCtx);
        }

        if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) || R_PLAY_DRAW_LENS_FLARES) {
            if (!this->envCtx.sunMoonDisabled) {
                sp21C.x = this->view.eye.x + this->envCtx.sunPos.x;
                sp21C.y = this->view.eye.y + this->envCtx.sunPos.y;
                sp21C.z = this->view.eye.z + this->envCtx.sunPos.z;
                sun_lens_disp(this, &this->envCtx, &this->view, gfxCtx, sp21C, 0);
            }
            Tokusyu_lenseflare(this);
        }

        if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) || R_PLAY_DRAW_SCREEN_FILLS) {
            if (MREG(64) != 0) {
                Display_Fillter(gfxCtx, MREG(65), MREG(66), MREG(67), MREG(68),
                                       FILL_SCREEN_OPA | FILL_SCREEN_XLU);
            }

            switch (this->envCtx.fillScreen) {
                case 1:
                    Display_Fillter(gfxCtx, this->envCtx.screenFillColor[0], this->envCtx.screenFillColor[1],
                                           this->envCtx.screenFillColor[2], this->envCtx.screenFillColor[3],
                                           FILL_SCREEN_OPA | FILL_SCREEN_XLU);
                    break;
                default:
                    break;
            }
        }

        if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) || R_PLAY_DRAW_SANDSTORM) {
            if (this->envCtx.sandstormState != SANDSTORM_OFF) {
                Display_Sstorm(this, this->envCtx.sandstormState);
            }
        }

        if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) || R_PLAY_DRAW_DEBUG_OBJECTS) {
            Debug_Display_output(this);
        }

        if ((R_PAUSE_BG_PRERENDER_STATE == PAUSE_BG_PRERENDER_SETUP) || (fbdemo_mode == TRANS_TILE_SETUP)) {
            Gfx* gfxP = OVERLAY_DISP;

            // Copy the frame buffer contents at this point in the display list to the zbuffer
            // The zbuffer must then stay untouched until unpausing
            this->pauseBgPreRender.fbuf = gfxCtx->curFrameBuffer;
            this->pauseBgPreRender.fbufSave = (u16*)sys_zb;
            PreRender_saveFrameBuffer(&this->pauseBgPreRender, &gfxP);
            if (R_PAUSE_BG_PRERENDER_STATE == PAUSE_BG_PRERENDER_SETUP) {
                this->pauseBgPreRender.cvgSave = (u8*)gfxCtx->curFrameBuffer;
                PreRender_saveCVG(&this->pauseBgPreRender, &gfxP);

                R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_PROCESS;
            } else {
                fbdemo_mode = TRANS_TILE_PROCESS;
            }
            OVERLAY_DISP = gfxP;
            this->unk_121C7 = 2;
            R_GRAPH_TASKSET00_FLAGS |= 1;
            goto Play_Draw_skip;
        }

    Play_Draw_DrawOverlayElements:
        if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) || R_PLAY_DRAW_OVERLAY_ELEMENTS) {
            Game_play_draw_overlay(this);
        }
    }

Play_Draw_skip:

    if (this->view.unk_124 != 0) {
        actionCameraWork(GET_ACTIVE_CAM(this));
        showViewAgain(&this->view);
        this->view.unk_124 = 0;
        if (this->skyboxId && (this->skyboxId != SKYBOX_UNSET_1D) && !this->envCtx.skyboxDisabled) {
            vr_mtx_set(&this->skyboxCtx, this->view.eye.x, this->view.eye.y, this->view.eye.z);
        }
    }

    batontouchCamera(GET_ACTIVE_CAM(this));

    CLOSE_DISPS(gfxCtx, "../z_play.c", 4508);
}

void play_main(GameState* thisx) {
    PlayState* this = (PlayState*)thisx;

    Debug_Pad = &this->state.input[0];

    Debug_Display_init();

    PLAY_LOG(4556);

    if (DEBUG_FEATURES && (R_HREG_MODE == HREG_MODE_PLAY) && (R_PLAY_INIT != HREG_MODE_PLAY)) {
        R_PLAY_RUN_UPDATE = true;
        R_PLAY_RUN_DRAW = true;
        R_PLAY_DRAW_SKYBOX = true;
        R_PLAY_DRAW_ROOM_FLAGS = (ROOM_DRAW_OPA | ROOM_DRAW_XLU);
        R_PLAY_DRAW_ACTORS = true;
        R_PLAY_DRAW_LENS_FLARES = true;
        R_PLAY_DRAW_SCREEN_FILLS = true;
        R_PLAY_DRAW_SANDSTORM = true;
        R_PLAY_DRAW_OVERLAY_ELEMENTS = true;
        R_PLAY_DRAW_ENV_FLAGS = (PLAY_ENV_DRAW_SKYBOX_FILTERS | PLAY_ENV_DRAW_SUN_AND_MOON | PLAY_ENV_DRAW_LIGHTNING |
                                 PLAY_ENV_DRAW_LIGHTS);
        HREG(91) = 1; // reg is not used in this mode
        R_PLAY_DRAW_COVER_ELEMENTS = true;
        R_PLAY_DRAW_DEBUG_OBJECTS = true;
        R_PLAY_INIT = HREG_MODE_PLAY;
    }

    if (!DEBUG_FEATURES || (R_HREG_MODE != HREG_MODE_PLAY) || R_PLAY_RUN_UPDATE) {
        Game_play_move(this);
    }

    PLAY_LOG(4583);

    Game_play_draw(this);

    PLAY_LOG(4587);
}

// original name: "Game_play_demo_mode_check"
int Game_play_demo_mode_check(PlayState* this) {
    return (this->csCtx.state != CS_STATE_IDLE) || player_demo_check(this);
}

f32 Game_play_ground_matrix(PlayState* this, MtxF* mf, Vec3f* pos) {
    CollisionPoly poly;
    f32 temp1;
    f32 temp2;
    f32 temp3;
    f32 floorY = T_BGCheck_GroundCheck(&this->colCtx, &poly, pos);

    if (floorY > BGCHECK_Y_MIN) {
        f32 nx = COLPOLY_GET_NORMAL(poly.normal.x);
        f32 ny = COLPOLY_GET_NORMAL(poly.normal.y);
        f32 nz = COLPOLY_GET_NORMAL(poly.normal.z);
        s32 pad[5];

        temp1 = sqrtf(1.0f - SQ(nx));

        if (temp1 != 0.0f) {
            temp2 = ny * temp1;
            temp3 = -nz * temp1;
        } else {
            temp3 = 0.0f;
            temp2 = 0.0f;
        }

        mf->xx = temp1;
        mf->yx = -nx * temp2;
        mf->zx = nx * temp3;
        mf->xy = nx;
        mf->yy = ny;
        mf->zy = nz;
        mf->yz = temp3;
        mf->zz = temp2;
        mf->wx = 0.0f;
        mf->wy = 0.0f;
        mf->xz = 0.0f;
        mf->wz = 0.0f;
        mf->xw = pos->x;
        mf->yw = floorY;
        mf->zw = pos->z;
        mf->ww = 1.0f;
    } else {
        mf->xy = 0.0f;
        mf->zx = 0.0f;
        mf->yx = 0.0f;
        mf->xx = 0.0f;
        mf->wz = 0.0f;
        mf->xz = 0.0f;
        mf->wy = 0.0f;
        mf->wx = 0.0f;
        mf->zz = 0.0f;
        mf->yz = 0.0f;
        mf->zy = 0.0f;
        mf->yy = 1.0f;
        mf->xw = pos->x;
        mf->yw = pos->y;
        mf->zw = pos->z;
        mf->ww = 1.0f;
    }

    return floorY;
}

void* ROM_read(PlayState* this, RomFile* file) {
    u32 size;
    void* allocp;

    size = file->vromEnd - file->vromStart;
    allocp = GAME_STATE_ALLOC(&this->state, size, "../z_play.c", 4692);
    DMA_REQUEST_SYNC(allocp, file->vromStart, size, "../z_play.c", 4694);

    return allocp;
}

#if PLATFORM_N64
void* Play_LoadFileFromDiskDrive(PlayState* this, RomFile* file) {
    u32 size;
    void* allocp;

    size = file->vromEnd - file->vromStart;
    allocp = GAME_STATE_ALLOC(&this->state, size, "../z_play.c", UNK_LINE);
    func_801C7C1C(allocp, file->vromStart, size);

    return allocp;
}
#endif

void VR_Box_ct(PlayState* this, s16 skyboxId) {
    vr_box_ct(&this->state, &this->skyboxCtx, skyboxId);
    Global_kankyo_ct(this, &this->envCtx, 0);
}

void Gameplay_Scene_Init(PlayState* this, s32 spawn) {
    this->spawn = spawn;

    this->playerEntry = NULL;
    this->unk_11DFC = NULL;
    this->spawnList = NULL;
    this->exitList = NULL;
    this->naviQuestHints = NULL;
    this->pathList = NULL;

    this->numActorEntries = 0;

    Object_Exchange_ct(this, &this->objectCtx);
    Global_light_ct(this, &this->lightCtx);
    Door_info_ct(&this->state, &this->transitionActors);
    Room_Info_ct(this, &this->roomCtx.curRoom);
    R_SCENE_CAM_TYPE = SCENE_CAM_TYPE_DEFAULT;
    z_common_data.worldMapArea = WORLD_MAP_AREA_HYRULE_FIELD;
    Scene_ct(this, this->sceneSegment);
    VR_Box_ct(this, this->skyboxId);
}

void Gameplay_Scene_Read(PlayState* this, s32 sceneId, s32 spawn) {
    SceneTableEntry* scene;
    UNUSED_NDEBUG u32 size;

#if PLATFORM_N64
    if ((B_80121220 != NULL) && (B_80121220->unk_48 != NULL)) {
        scene = B_80121220->unk_48(sceneId, scene_data_status);
    } else {
        scene = &scene_data_status[sceneId];
        scene->unk_13 = 0;
    }
#else
    scene = &scene_data_status[sceneId];
    scene->unk_13 = 0;
#endif

    this->loadedScene = scene;
    this->sceneId = sceneId;
    this->sceneDrawConfig = scene->drawConfig;

    PRINTF("\nSCENE SIZE %fK\n", (scene->sceneFile.vromEnd - scene->sceneFile.vromStart) / 1024.0f);

#if PLATFORM_N64
    if ((B_80121220 != NULL) && (scene->unk_12 > 0)) {
        this->sceneSegment = Play_LoadFileFromDiskDrive(this, &scene->sceneFile);
        scene->unk_13 = 1;
    } else {
        this->sceneSegment = ROM_read(this, &scene->sceneFile);
        scene->unk_13 = 0;
    }
#else
    this->sceneSegment = ROM_read(this, &scene->sceneFile);
    scene->unk_13 = 0;
#endif

    ASSERT(this->sceneSegment != NULL, "this->sceneSegment != NULL", "../z_play.c", 4960);

    SegmentBaseAddress[2] = VIRTUAL_TO_PHYSICAL(this->sceneSegment);

    Gameplay_Scene_Init(this, spawn);

#if PLATFORM_N64
    if ((B_80121220 != NULL) && (B_80121220->unk_0C != NULL)) {
        B_80121220->unk_0C(this);
    }
#endif

    size = Room_Info_init(this, &this->roomCtx);

    PRINTF("ROOM SIZE=%fK\n", size / 1024.0f);
}

void Gama_play_Projection_Trans(PlayState* this, Vec3f* src, Vec3f* dest) {
    f32 w;

    Matrix_mult(&this->viewProjectionMtxF, MTXMODE_NEW);
    Matrix_Position(src, dest);

    w = this->viewProjectionMtxF.ww + (this->viewProjectionMtxF.wx * src->x + this->viewProjectionMtxF.wy * src->y +
                                       this->viewProjectionMtxF.wz * src->z);

    dest->x = (SCREEN_WIDTH / 2) + ((dest->x / w) * (SCREEN_WIDTH / 2));
    dest->y = (SCREEN_HEIGHT / 2) - ((dest->y / w) * (SCREEN_HEIGHT / 2));
}

s16 Gama_play_make_camera(PlayState* this) {
    s16 camId;

    for (camId = CAM_ID_SUB_FIRST; camId < NUM_CAMS; camId++) {
        if (this->cameraPtrs[camId] == NULL) {
            break;
        }
    }

    if (camId == NUM_CAMS) {
        PRINTF(VT_COL(RED, WHITE) "camera control: error: fulled sub camera system area\n" VT_RST);
        return CAM_ID_NONE;
    }

    PRINTF("camera control: " VT_BGCOL(CYAN) " " VT_COL(WHITE, BLUE) " create new sub camera [%d] " VT_BGCOL(
               CYAN) " " VT_RST "\n",
           camId);

    this->cameraPtrs[camId] = &this->subCameras[camId - CAM_ID_SUB_FIRST];
    initCamera(this->cameraPtrs[camId], &this->view, &this->colCtx, this);
    this->cameraPtrs[camId]->camId = camId;

    return camId;
}

s32 Gama_play_active_camera(PlayState* this) {
    return this->activeCamId;
}

s16 Gama_play_set_camera_status(PlayState* this, s16 camId, s16 status) {
    s16 camIdx = (camId == CAM_ID_NONE) ? this->activeCamId : camId;

    if (status == CAM_STAT_ACTIVE) {
        this->activeCamId = camIdx;
    }

    return changeCameraStatus(this->cameraPtrs[camIdx], status);
}

void Gama_play_clear_camera(PlayState* this, s16 camId) {
    s16 camIdx = (camId == CAM_ID_NONE) ? this->activeCamId : camId;

    if (camIdx == CAM_ID_MAIN) {
        PRINTF(VT_COL(RED, WHITE) "camera control: error: never clear camera !!\n" VT_RST);
    }

    if (this->cameraPtrs[camIdx] != NULL) {
        changeCameraStatus(this->cameraPtrs[camIdx], CAM_STAT_UNK100);
        this->cameraPtrs[camIdx] = NULL;
        PRINTF("camera control: " VT_BGCOL(CYAN) " " VT_COL(WHITE, BLUE) " clear sub camera [%d] " VT_BGCOL(
                   CYAN) " " VT_RST "\n",
               camIdx);
    } else {
        PRINTF(VT_COL(RED, WHITE) "camera control: error: camera No.%d already cleared\n" VT_RST, camIdx);
    }
}

void Gama_play_clear_camera_all(PlayState* this) {
    s16 subCamId;

    for (subCamId = CAM_ID_SUB_FIRST; subCamId < NUM_CAMS; subCamId++) {
        if (this->cameraPtrs[subCamId] != NULL) {
            Gama_play_clear_camera(this, subCamId);
        }
    }

    this->activeCamId = CAM_ID_MAIN;
}

Camera* Gama_play_get_camera(PlayState* this, s16 camId) {
    s16 camIdx = (camId == CAM_ID_NONE) ? this->activeCamId : camId;

    return this->cameraPtrs[camIdx];
}

s32 Gama_play_camera_setting(PlayState* this, s16 camId, Vec3f* at, Vec3f* eye) {
    s32 successBits = 0;
    s16 camIdx = (camId == CAM_ID_NONE) ? this->activeCamId : camId;
    Camera* camera = this->cameraPtrs[camIdx];
    Player* player;

    successBits |= lockCamera(camera, CAM_VIEW_AT, at);
    successBits <<= 1;
    successBits |= lockCamera(camera, CAM_VIEW_EYE, eye);

    camera->dist = Math3DLength(at, eye);

    player = camera->player;
    if (player != NULL) {
        camera->playerToAtOffset.x = at->x - player->actor.world.pos.x;
        camera->playerToAtOffset.y = at->y - player->actor.world.pos.y;
        camera->playerToAtOffset.z = at->z - player->actor.world.pos.z;
    } else {
        camera->playerToAtOffset.x = camera->playerToAtOffset.y = camera->playerToAtOffset.z = 0.0f;
    }

    camera->atLERPStepScale = 0.01f;

    return successBits;
}

s32 Gama_play_camera_lookat(PlayState* this, s16 camId, Vec3f* at, Vec3f* eye, Vec3f* up) {
    s32 successBits = 0;
    s16 camIdx = (camId == CAM_ID_NONE) ? this->activeCamId : camId;
    Camera* camera = this->cameraPtrs[camIdx];
    Player* player;

    successBits |= lockCamera(camera, CAM_VIEW_AT, at);
    successBits <<= 1;
    successBits |= lockCamera(camera, CAM_VIEW_EYE, eye);
    successBits <<= 1;
    successBits |= lockCamera(camera, CAM_VIEW_UP, up);

    camera->dist = Math3DLength(at, eye);

    player = camera->player;
    if (player != NULL) {
        camera->playerToAtOffset.x = at->x - player->actor.world.pos.x;
        camera->playerToAtOffset.y = at->y - player->actor.world.pos.y;
        camera->playerToAtOffset.z = at->z - player->actor.world.pos.z;
    } else {
        camera->playerToAtOffset.x = camera->playerToAtOffset.y = camera->playerToAtOffset.z = 0.0f;
    }

    camera->atLERPStepScale = 0.01f;

    return successBits;
}

s32 Gama_play_set_camera_fovy(PlayState* this, s16 camId, f32 fov) {
    s32 successBits = lockCamera(this->cameraPtrs[camId], CAM_VIEW_FOV, &fov) & 1;

    if (1) {}
    return successBits;
}

s32 Gama_play_set_camera_sz(PlayState* this, s16 camId, s16 roll) {
    s16 camIdx = (camId == CAM_ID_NONE) ? this->activeCamId : camId;
    Camera* camera = this->cameraPtrs[camIdx];

    camera->roll = roll;

    return 1;
}

void Gama_play_copy_camera_position(PlayState* this, s16 destCamId, s16 srcCamId) {
    s16 srcCamId2 = (srcCamId == CAM_ID_NONE) ? this->activeCamId : srcCamId;
    s16 destCamId1 = (destCamId == CAM_ID_NONE) ? this->activeCamId : destCamId;

    copyCameraPos(this->cameraPtrs[destCamId1], this->cameraPtrs[srcCamId2]);
}

/**
 * Initializes camera data centered around Player, and applies the requested setting.
 */
s32 Gama_play_set_camera_owner(PlayState* this, s16 camId, Player* player, s16 setting) {
    Camera* camera;
    s16 camIdx = (camId == CAM_ID_NONE) ? this->activeCamId : camId;

    camera = this->cameraPtrs[camIdx];
    leaveCamera(camera, player);
    return changeCameraSet(camera, setting);
}

s32 Gama_play_change_camera_set(PlayState* this, s16 camId, s16 setting) {
    return changeCameraSet(Gama_play_get_camera(this, camId), setting);
}

/**
 * Smoothly return control from a sub camera to the main camera by moving the subCamera's eye, at, fov through
 * interpolation from the initial subCam viewParams to the target mainCam viewParams over `duration`.
 * Setting the `duration` to 0 or less will instantly return control to the main camera.
 * This will also clear every sub camera.
 */
void Gama_play_shift2main_camera(PlayState* this, s16 camId, s16 duration) {
    s16 camIdx = (camId == CAM_ID_NONE) ? this->activeCamId : camId;
    s16 subCamId;

    Gama_play_clear_camera(this, camIdx);

    for (subCamId = CAM_ID_SUB_FIRST; subCamId < NUM_CAMS; subCamId++) {
        if (this->cameraPtrs[subCamId] != NULL) {
            PRINTF(VT_COL(RED, WHITE) "camera control: error: return to main, other camera left. %d cleared!!\n" VT_RST,
                   subCamId);
            Gama_play_clear_camera(this, subCamId);
        }
    }

    if (duration <= 0) {
        Gama_play_set_camera_status(this, CAM_ID_MAIN, CAM_STAT_ACTIVE);
        this->cameraPtrs[CAM_ID_MAIN]->childCamId = this->cameraPtrs[CAM_ID_MAIN]->parentCamId = CAM_ID_MAIN;
    } else {
        makeOnepointDemo(this, 1020, duration, NULL, CAM_ID_MAIN);
    }
}

s16 Gama_play_getUID_camera(PlayState* this, s16 camId) {
    Camera* camera = this->cameraPtrs[camId];

    if (camera != NULL) {
        return camera->uid;
    } else {
        return -1;
    }
}

// Unused, purpose is unclear (also unused and unclear in MM)
s16 Gama_play_checkUID_camera(PlayState* this, s16 camId, s16 uid) {
    Camera* camera = this->cameraPtrs[camId];

    if (camera != NULL) {
        return 0;
    }

    //! @bug this code is only reached if `camera` is NULL.
    if (camera->uid != uid) {
        return 0;
    } else if (camera->status != CAM_STAT_ACTIVE) {
        return 2;
    } else {
        return 1;
    }
}

void Game_play_room_inf_set(PlayState* this) {
    SavedSceneFlags* savedSceneFlags = &z_common_data.save.info.sceneFlags[this->sceneId];

    savedSceneFlags->chest = this->actorCtx.flags.chest;
    savedSceneFlags->swch = this->actorCtx.flags.swch;
    savedSceneFlags->clear = this->actorCtx.flags.clear;
    savedSceneFlags->collect = this->actorCtx.flags.collect;
}

void Game_play_restart_set2(PlayState* this, s32 respawnMode, s16 entranceIndex, s32 roomIndex, s32 playerParams,
                         Vec3f* pos, s16 yaw) {
    RespawnData* respawnData = &z_common_data.respawn[respawnMode];

    respawnData->entranceIndex = entranceIndex;
    respawnData->roomIndex = roomIndex;
    respawnData->pos = *pos;
    respawnData->yaw = yaw;
    respawnData->playerParams = playerParams;
    respawnData->tempSwchFlags = this->actorCtx.flags.tempSwch;
    respawnData->tempCollectFlags = this->actorCtx.flags.tempCollect;
}

void Game_play_restart_set(PlayState* this, s32 respawnMode, s32 playerParams) {
    Player* player = GET_PLAYER(this);
    s32 entranceIndex;
    s8 roomIndex;

    if ((this->sceneId != SCENE_FAIRYS_FOUNTAIN) && (this->sceneId != SCENE_GROTTOS)) {
        roomIndex = this->roomCtx.curRoom.num;
        entranceIndex = z_common_data.save.entranceIndex;
        Game_play_restart_set2(this, respawnMode, entranceIndex, roomIndex, playerParams, &player->actor.world.pos,
                            player->actor.shape.rot.y);
    }
}

void Game_play_down_restart(PlayState* this) {
    z_common_data.respawn[RESPAWN_MODE_DOWN].tempSwchFlags = this->actorCtx.flags.tempSwch;
    z_common_data.respawn[RESPAWN_MODE_DOWN].tempCollectFlags = this->actorCtx.flags.tempCollect;
    z_common_data.respawnFlag = 1;
    this->transitionTrigger = TRANS_TRIGGER_START;
    this->nextEntranceIndex = z_common_data.respawn[RESPAWN_MODE_DOWN].entranceIndex;
    this->transitionType = TRANS_TYPE_FADE_BLACK;
}

void Game_play_down_restart_top_set(PlayState* this) {
    z_common_data.respawnFlag = -1;
    this->transitionTrigger = TRANS_TRIGGER_START;

    if ((this->sceneId == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) ||
        (this->sceneId == SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) ||
        (this->sceneId == SCENE_INSIDE_GANONS_CASTLE_COLLAPSE) || (this->sceneId == SCENE_GANON_BOSS)) {
        this->nextEntranceIndex = ENTR_GANONS_TOWER_COLLAPSE_EXTERIOR_0;
        item_get_setting(this, ITEM_SWORD_MASTER);
#if OOT_VERSION >= PAL_1_1
    } else if ((z_common_data.save.entranceIndex == ENTR_HYRULE_FIELD_11) ||
               (z_common_data.save.entranceIndex == ENTR_HYRULE_FIELD_12) ||
               (z_common_data.save.entranceIndex == ENTR_HYRULE_FIELD_13) ||
               (z_common_data.save.entranceIndex == ENTR_HYRULE_FIELD_15)) {
        // Avoid re-triggering the hop over Lon Lon fence cutscenes
        this->nextEntranceIndex = ENTR_HYRULE_FIELD_6;
#endif
    } else {
        this->nextEntranceIndex = z_common_data.save.entranceIndex;
    }

    this->transitionType = TRANS_TYPE_FADE_BLACK;
}

void Game_play_down_restart_top(PlayState* this) {
    Game_play_restart_set(this, RESPAWN_MODE_DOWN, PLAYER_PARAMS(PLAYER_START_MODE_IDLE, PLAYER_START_BG_CAM_DEFAULT));
    Game_play_down_restart_top_set(this);
}

int Game_play_change_camera_check(PlayState* this) {
    // SCENE_CAM_TYPE_FIXED_SHOP_VIEWPOINT was probably intended to be in this condition,
    // but the room shape type check handles all shop cases regardless
    return (this->roomCtx.curRoom.roomShape->base.type != ROOM_SHAPE_TYPE_IMAGE) &&
           (R_SCENE_CAM_TYPE != SCENE_CAM_TYPE_FIXED_TOGGLE_VIEWPOINT) && (R_SCENE_CAM_TYPE != SCENE_CAM_TYPE_FIXED) &&
           (R_SCENE_CAM_TYPE != SCENE_CAM_TYPE_FIXED_MARKET) && (this->sceneId != SCENE_CASTLE_COURTYARD_GUARDS_DAY);
}

int _Game_play_isPause(PlayState* this) {
    return !!this->frameAdvCtx.enabled;
}

s32 Game_play_getDoorAngle(PlayState* this, Actor* actor, s16* yaw) {
    TransitionActorEntry* transitionActor;
    s32 frontRoom;

    if (actor->category != ACTORCAT_DOOR) {
        return 0;
    }

    transitionActor = &this->transitionActors.list[GET_TRANSITION_ACTOR_INDEX(actor)];
    frontRoom = transitionActor->sides[0].room;

    if (frontRoom == transitionActor->sides[1].room) {
        return 0;
    }

    if (frontRoom == actor->room) {
        *yaw = actor->shape.rot.y;
    } else {
        *yaw = actor->shape.rot.y + 0x8000;
    }

    return 1;
}

s32 Gama_play_position_in_water(PlayState* this, Vec3f* pos) {
    WaterBox* waterBox;
    CollisionPoly* poly;
    Vec3f waterSurfacePos;
    s32 bgId;

    waterSurfacePos = *pos;

    if (T_BGCheck_WaterSurfaceCheck3(this, &this->colCtx, waterSurfacePos.x, waterSurfacePos.z, &waterSurfacePos.y,
                             &waterBox) == true &&
        pos->y < waterSurfacePos.y &&
        T_BGCheck_ObjGroundCheck_ai(&this->colCtx, &poly, &bgId, &waterSurfacePos) != BGCHECK_Y_MIN) {
        return true;
    } else {
        return false;
    }
}
