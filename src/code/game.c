#include "global.h"
#include "fault.h"
#include "libc64/os_malloc.h"
#include "terminal.h"
#include "versions.h"
#include "line_numbers.h"
#if PLATFORM_N64
#include "n64dd.h"
#endif

#pragma increment_block_number "gc-eu:128 gc-eu-mq:128 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128 hiratsu3:128"

SpeedMeter speed_meter;
VisCvg z_viscvg;
VisZBuf z_viszbuf;
VisMono z_vismono;
ViMode z_vimode;

#if DEBUG_FEATURES
FaultClient sGameFaultClient;
u16 sLastButtonPressed;

void GameState_FaultPrint(void) {
    static char sBtnChars[] = "ABZSuldr*+LRudlr";
    s32 i;

    PRINTF("last_button=%04x\n", sLastButtonPressed);
    faultprint_PosPrintf(120, 180, "%08x", sLastButtonPressed);
    for (i = 0; i < ARRAY_COUNT(sBtnChars); i++) {
        if (sLastButtonPressed & (1 << i)) {
            faultprint_PosPrintf((i * 8) + 120, 190, "%c", sBtnChars[i]);
        }
    }
}
#endif

void debug_filter(Gfx** gfxP) {
    Gfx* gfx = *gfxP;

    if ((R_FB_FILTER_TYPE >= FB_FILTER_CVG_RGB) && (R_FB_FILTER_TYPE <= FB_FILTER_CVG_RGB_FOG)) {
        // Visualize coverage
        z_viscvg.vis.type = FB_FILTER_TO_CVG_TYPE(R_FB_FILTER_TYPE);
        z_viscvg.vis.primColor.r = R_FB_FILTER_PRIM_COLOR(0);
        z_viscvg.vis.primColor.g = R_FB_FILTER_PRIM_COLOR(1);
        z_viscvg.vis.primColor.b = R_FB_FILTER_PRIM_COLOR(2);
        z_viscvg.vis.primColor.a = R_FB_FILTER_A;
        z_viscvg_draw(&z_viscvg, &gfx);
    } else if ((R_FB_FILTER_TYPE == FB_FILTER_ZBUF_IA) || (R_FB_FILTER_TYPE == FB_FILTER_ZBUF_RGBA)) {
        // Visualize z-buffer
        z_viszbuf.vis.type = (R_FB_FILTER_TYPE == FB_FILTER_ZBUF_RGBA);
        z_viszbuf.vis.primColor.r = R_FB_FILTER_PRIM_COLOR(0);
        z_viszbuf.vis.primColor.g = R_FB_FILTER_PRIM_COLOR(1);
        z_viszbuf.vis.primColor.b = R_FB_FILTER_PRIM_COLOR(2);
        z_viszbuf.vis.primColor.a = R_FB_FILTER_A;
        z_viszbuf.vis.envColor.r = R_FB_FILTER_ENV_COLOR(0);
        z_viszbuf.vis.envColor.g = R_FB_FILTER_ENV_COLOR(1);
        z_viszbuf.vis.envColor.b = R_FB_FILTER_ENV_COLOR(2);
        z_viszbuf.vis.envColor.a = R_FB_FILTER_A;
        z_viszbuf_draw(&z_viszbuf, &gfx);
    } else if (R_FB_FILTER_TYPE == FB_FILTER_MONO) {
        // Monochrome filter
        z_vismono.vis.type = 0;
        z_vismono.vis.primColor.r = R_FB_FILTER_PRIM_COLOR(0);
        z_vismono.vis.primColor.g = R_FB_FILTER_PRIM_COLOR(1);
        z_vismono.vis.primColor.b = R_FB_FILTER_PRIM_COLOR(2);
        z_vismono.vis.primColor.a = R_FB_FILTER_A;
        z_vismono.vis.envColor.r = R_FB_FILTER_ENV_COLOR(0);
        z_vismono.vis.envColor.g = R_FB_FILTER_ENV_COLOR(1);
        z_vismono.vis.envColor.b = R_FB_FILTER_ENV_COLOR(2);
        z_vismono.vis.envColor.a = R_FB_FILTER_A;
        z_vismono_draw(&z_vismono, &gfx);
    }
    *gfxP = gfx;
}

void game_move_first(GameState* gameState) {
#if DEBUG_FEATURES
    Input* selectedInput;
    s32 hexDumpSize;
    u16 inputCompareValue;

#if PLATFORM_GC
    if (R_HREG_MODE == HREG_MODE_HEAP_FREE_BLOCK_TEST) {
        __osMalloc_FreeBlockTest_Enable = R_HEAP_FREE_BLOCK_TEST_TOGGLE;
    }
#endif

    if (R_HREG_MODE == HREG_MODE_INPUT_TEST) {
        selectedInput =
            &gameState->input[(u32)R_INPUT_TEST_CONTROLLER_PORT < MAXCONTROLLERS ? R_INPUT_TEST_CONTROLLER_PORT : 0];

        inputCompareValue = R_INPUT_TEST_COMPARE_VALUE;
        R_INPUT_TEST_BUTTON_CUR = selectedInput->cur.button;
        R_INPUT_TEST_BUTTON_PRESS = selectedInput->press.button;
        R_INPUT_TEST_REL_STICK_X = selectedInput->rel.stick_x;
        R_INPUT_TEST_REL_STICK_Y = selectedInput->rel.stick_y;
        R_INPUT_TEST_REL_STICK_X_2 = selectedInput->rel.stick_x;
        R_INPUT_TEST_REL_STICK_Y_2 = selectedInput->rel.stick_y;
        R_INPUT_TEST_CUR_STICK_X = selectedInput->cur.stick_x;
        R_INPUT_TEST_CUR_STICK_Y = selectedInput->cur.stick_y;
        R_INPUT_TEST_COMPARE_BUTTON_CUR = (selectedInput->cur.button == inputCompareValue);
        R_INPUT_TEST_COMPARE_COMBO_CUR = CHECK_BTN_ALL(selectedInput->cur.button, inputCompareValue);
        R_INPUT_TEST_COMPARE_COMBO_PRESS = CHECK_BTN_ALL(selectedInput->press.button, inputCompareValue);
    }

    if (gIsCtrlr2Valid) {
        Regs_UpdateEditor(&gameState->input[1]);
    }

    _dma_verbose = HREG(60);
    _dma_split_size = SREG(21) != 0 ? ALIGN16(SREG(21)) : DMAMGR_DEFAULT_BUFSIZE;
    gSystemArenaLogSeverity = HREG(61);
    gZeldaArenaLogSeverity = HREG(62);

    if (R_HREG_MODE == HREG_MODE_PRINT_MEMORY) {
        if (R_PRINT_MEMORY_INIT != HREG_MODE_PRINT_MEMORY) {
            R_PRINT_MEMORY_INIT = HREG_MODE_PRINT_MEMORY;
            R_PRINT_MEMORY_TRIGGER = 0;
            R_PRINT_MEMORY_ADDR = 0;
            R_PRINT_MEMORY_SIZE = 0;
        }

        if (R_PRINT_MEMORY_TRIGGER < 0) {
            R_PRINT_MEMORY_TRIGGER = 0;
            hexDumpSize = (u32)(R_PRINT_MEMORY_SIZE == 0 ? 0x100 : R_PRINT_MEMORY_SIZE * 0x10);
            LogUtils_LogHexDump((void*)(0x80000000 + (R_PRINT_MEMORY_ADDR << 8)), hexDumpSize);
        }
    }
#endif

#if PLATFORM_N64
    if (D_80121212 != 0) {
        func_801C7E78();
    }
#endif
}

#if DEBUG_FEATURES
void game_debug_draw_lastInputDisplay(u16 input, Gfx** gfxP) {
    static const u16 sInpDispBtnColors[] = {
        GPACK_RGBA5551(255, 255, 0, 1),   GPACK_RGBA5551(255, 255, 0, 1),   GPACK_RGBA5551(255, 255, 0, 1),
        GPACK_RGBA5551(255, 255, 0, 1),   GPACK_RGBA5551(120, 120, 120, 1), GPACK_RGBA5551(120, 120, 120, 1),
        GPACK_RGBA5551(0, 255, 255, 1),   GPACK_RGBA5551(255, 0, 255, 1),   GPACK_RGBA5551(120, 120, 120, 1),
        GPACK_RGBA5551(120, 120, 120, 1), GPACK_RGBA5551(120, 120, 120, 1), GPACK_RGBA5551(120, 120, 120, 1),
        GPACK_RGBA5551(255, 0, 0, 1),     GPACK_RGBA5551(120, 120, 120, 1), GPACK_RGBA5551(0, 255, 0, 1),
        GPACK_RGBA5551(0, 0, 255, 1),
    };
    s32 i, j, k;
    Gfx* gfx = *gfxP;

    gDPPipeSync(gfx++);
    gDPSetOtherMode(gfx++,
                    G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_CONV | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_FILL | G_PM_NPRIMITIVE,
                    G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2);

    for (i = 0; i < 16; i++) {
        j = i;
        if (input & (1 << i)) {
            gDPSetFillColor(gfx++, (sInpDispBtnColors[i] << 0x10) | sInpDispBtnColors[i]);
            k = i + 1;
            gDPFillRectangle(gfx++, (j * 4) + 226, 220, (k * 4) + 225, 223);
            gDPPipeSync(gfx++);
        }
    }

    *gfxP = gfx;
}
#endif

void game_debug_draw_last(GameState* gameState, GraphicsContext* gfxCtx) {
    Gfx* newDList;
    Gfx* polyOpaP;

    OPEN_DISPS(gfxCtx, "../game.c", 746);

    newDList = gfxopen(polyOpaP = POLY_OPA_DISP);
    gSPDisplayList(OVERLAY_DISP++, newDList);

    if (R_ENABLE_FB_FILTER == 1) {
        debug_filter(&newDList);
    }

#if DEBUG_FEATURES
    sLastButtonPressed = gameState->input[0].press.button | gameState->input[0].cur.button;
    if (R_DISABLE_INPUT_DISPLAY == 0) {
        game_debug_draw_lastInputDisplay(sLastButtonPressed, &newDList);
    }

    if (R_ENABLE_AUDIO_DBG & 1) {
        s32 pad;
        GfxPrint printer;

        gfxprint_init(&printer);
        gfxprint_open(&printer, newDList);
        Na_AudioPrint(&printer);
        newDList = gfxprint_close(&printer);
        gfxprint_cleanup(&printer);
    }
#endif

    if (R_ENABLE_ARENA_DBG < 0) {
#if PLATFORM_GC && DEBUG_FEATURES
        s32 pad;
        DebugArena_Display();
        SystemArena_Display();
#endif
        PRINTF(T("ハイラル滅亡まであと %08x バイト(game_alloc)\n",
                 "%08x bytes left until Hyrule is destroyed (game_alloc)\n"),
               THA_getFreeBytes(&gameState->tha));
        R_ENABLE_ARENA_DBG = 0;
    }

    gSPEndDisplayList(newDList++);
    gfxclose(polyOpaP, newDList);
    POLY_OPA_DISP = newDList;

    CLOSE_DISPS(gfxCtx, "../game.c", 800);

    Debug_mode_output(gfxCtx);

    if (R_ENABLE_ARENA_DBG != 0) {
        speed_meter_draw(&speed_meter, gfxCtx);
        speed_meter_draw_memory(&speed_meter, gfxCtx, gameState);
    }
}

void game_draw_first(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../game.c", 814);

    gSPSegment(POLY_OPA_DISP++, 0, 0);
    gSPSegment(POLY_OPA_DISP++, 0xF, gfxCtx->curFrameBuffer);
    gSPSegment(POLY_OPA_DISP++, 0xE, sys_zb);
    gSPSegment(POLY_XLU_DISP++, 0, 0);
    gSPSegment(POLY_XLU_DISP++, 0xF, gfxCtx->curFrameBuffer);
    gSPSegment(POLY_XLU_DISP++, 0xE, sys_zb);
    gSPSegment(OVERLAY_DISP++, 0, 0);
    gSPSegment(OVERLAY_DISP++, 0xF, gfxCtx->curFrameBuffer);
    gSPSegment(OVERLAY_DISP++, 0xE, sys_zb);

    CLOSE_DISPS(gfxCtx, "../game.c", 838);
}

void game_draw_last(GraphicsContext* gfxCtx) {
    Gfx* newDlist;
    Gfx* polyOpaP;

    OPEN_DISPS(gfxCtx, "../game.c", 846);

    newDlist = gfxopen(polyOpaP = POLY_OPA_DISP);
    gSPDisplayList(OVERLAY_DISP++, newDlist);

#if PLATFORM_N64
    if (D_80121212 != 0) {
        func_801C6EA0(&newDlist);
    }
#endif

    gSPEndDisplayList(newDlist++);
    gfxclose(polyOpaP, newDlist);
    POLY_OPA_DISP = newDlist;

    CLOSE_DISPS(gfxCtx, "../game.c", 865);
}

void padmgr_RequestPadData(PadMgr* padMgr, Input* inputs, s32 gameRequest);

void game_get_controller(GameState* gameState) {
    padmgr_RequestPadData(&padmgr, gameState->input, true);
}

void game_main(GameState* gameState) {
    GraphicsContext* gfxCtx = gameState->gfxCtx;

    game_draw_first(gfxCtx);

    gameState->main(gameState);

#if PLATFORM_N64
    if (D_80121212 != 0) {
        func_801C7E78();
    }
    if ((B_80121220 != NULL) && (B_80121220->unk_74 != NULL)) {
        B_80121220->unk_74(gameState);
    }
#endif

    game_move_first(gameState);

#if OOT_VERSION < PAL_1_0
    if (R_VI_MODE_EDIT_STATE != VI_MODE_EDIT_STATE_INACTIVE) {
        z_vimode_move(&z_vimode, &gameState->input[0]);
        gfxCtx->viMode = &z_vimode.customViMode;
        gfxCtx->viFeatures = z_vimode.viFeatures;
    }
#endif

#if OOT_VERSION >= PAL_1_0 && DEBUG_FEATURES
    if (SREG(63) == 1u) {
        if (R_VI_MODE_EDIT_STATE < VI_MODE_EDIT_STATE_INACTIVE) {
            R_VI_MODE_EDIT_STATE = VI_MODE_EDIT_STATE_INACTIVE;
            gfxCtx->viMode = &vimode;
            gfxCtx->viFeatures = vispecial;
            gfxCtx->xScale = vixscale;
            gfxCtx->yScale = viyscale;
        } else if (R_VI_MODE_EDIT_STATE > VI_MODE_EDIT_STATE_INACTIVE) {
            z_vimode_move(&z_vimode, &gameState->input[0]);
            gfxCtx->viMode = &z_vimode.customViMode;
            gfxCtx->viFeatures = z_vimode.viFeatures;
            gfxCtx->xScale = 1.0f;
            gfxCtx->yScale = 1.0f;
        }
    } else if (SREG(63) >= 2) {
        gfxCtx->viMode = &vimode;
        gfxCtx->viFeatures = vispecial;
        gfxCtx->xScale = vixscale;
        gfxCtx->yScale = viyscale;

        if (SREG(63) == 6 || (SREG(63) == 2u && (u32)osTvType == OS_TV_NTSC)) {
            gfxCtx->viMode = &osViModeNtscLan1;
            gfxCtx->yScale = 1.0f;
        }

        if (SREG(63) == 5 || (SREG(63) == 2u && (u32)osTvType == OS_TV_MPAL)) {
            gfxCtx->viMode = &osViModeMpalLan1;
            gfxCtx->yScale = 1.0f;
        }

        if (SREG(63) == 4 || (SREG(63) == 2u && (u32)osTvType == OS_TV_PAL)) {
            gfxCtx->viMode = &osViModePalLan1;
            gfxCtx->yScale = 1.0f;
        }

        if (SREG(63) == 3 || (SREG(63) == 2u && (u32)osTvType == OS_TV_PAL)) {
            gfxCtx->viMode = &osViModeFpalLan1;
            gfxCtx->yScale = 0.833f;
        }
    } else {
        gfxCtx->viMode = NULL;
    }

    if (R_HREG_MODE == HREG_MODE_VI) {
        if (R_VI_INIT != HREG_MODE_VI) {
            R_VI_INIT = HREG_MODE_VI;
            R_VI_NEXT_Y_SCALE_MODE = 0;
            R_VI_NEXT_ADDI_SCAN_LINES = viextendvstart;
            R_VI_CUR_ADDI_SCAN_LINES = 0;
            R_VI_CUR_Y_SCALE_MODE = 0;
        }

        if (R_VI_NEXT_ADDI_SCAN_LINES < 0) {
            R_VI_NEXT_ADDI_SCAN_LINES = 0;
        }

        if (R_VI_NEXT_ADDI_SCAN_LINES > 0x30) {
            R_VI_NEXT_ADDI_SCAN_LINES = 0x30;
        }

        if ((R_VI_CUR_ADDI_SCAN_LINES != R_VI_NEXT_ADDI_SCAN_LINES) ||
            R_VI_CUR_Y_SCALE_MODE != R_VI_NEXT_Y_SCALE_MODE) {

            R_VI_CUR_ADDI_SCAN_LINES = R_VI_NEXT_ADDI_SCAN_LINES;
            R_VI_CUR_Y_SCALE_MODE = R_VI_NEXT_Y_SCALE_MODE;

            viextendvstart = R_VI_NEXT_ADDI_SCAN_LINES;
            viyscale = R_VI_NEXT_Y_SCALE_MODE == 0
                                  ? ((f32)SCREEN_HEIGHT) / (viextendvstart + (f32)SCREEN_HEIGHT)
                                  : 1.0f;
            vidirty = 1;
        }
    }
#endif

    if (R_PAUSE_BG_PRERENDER_STATE != (u32)PAUSE_BG_PRERENDER_PROCESS) {
        game_debug_draw_last(gameState, gfxCtx);
        game_draw_last(gfxCtx);
    }

    gameState->frames++;
}

void game_init_hyral(GameState* gameState, size_t size) {
    void* arena;

    PRINTF(T("ハイラル確保 サイズ＝%u バイト\n", "Hyrule reserved size = %u bytes\n"), size);
    arena = GAME_ALLOC_MALLOC(&gameState->alloc, size, "../game.c", 992);

    if (arena != NULL) {
        THA_ct(&gameState->tha, arena, size);
        PRINTF(T("ハイラル確保成功\n", "Hyrule successfully secured\n"));
    } else {
        THA_ct(&gameState->tha, NULL, 0);
        PRINTF(T("ハイラル確保失敗\n", "Failure to secure Hyrule\n"));
        HUNGUP_AND_CRASH("../game.c", LN4(895, 898, 985, 999, 999));
    }
}

void game_resize_hyral(GameState* gameState, size_t size) {
    GameAlloc* alloc = &gameState->alloc;
    void* gameArena;
    u32 systemMaxFree;
    u32 systemFree;
    u32 systemAlloc;
    void* thaStart = gameState->tha.start;

    THA_dt(&gameState->tha);
    gamealloc_free(alloc, thaStart);
    PRINTF(T("ハイラル一時解放!!\n", "Hyrule temporarily released!!\n"));
    GetFreeArena(&systemMaxFree, &systemFree, &systemAlloc);
    if ((systemMaxFree - 0x10) < size) {
        PRINTF("%c", BEL);
        PRINTF_COLOR_RED();

        PRINTF(T("メモリが足りません。ハイラルサイズを可能な最大値に変更します\n",
                 "Not enough memory. Change Hyrule size to maximum possible value\n"));
        PRINTF("(hyral=%08x max=%08x free=%08x alloc=%08x)\n", size, systemMaxFree, systemFree, systemAlloc);
        PRINTF_RST();
        size = systemMaxFree - 0x10;
    }

    PRINTF(T("ハイラル再確保 サイズ＝%u バイト\n", "Hyrule reallocate size = %u bytes\n"), size);

    gameArena = GAME_ALLOC_MALLOC(alloc, size, "../game.c", 1033);
    if (gameArena != NULL) {
        THA_ct(&gameState->tha, gameArena, size);
        PRINTF(T("ハイラル再確保成功\n", "Successful reacquisition of Hyrule\n"));
    } else {
        THA_ct(&gameState->tha, NULL, 0);
        PRINTF(T("ハイラル再確保失敗\n", "Failure to secure Hyrule\n"));

#if PLATFORM_GC && DEBUG_FEATURES
        SystemArena_Display();
#endif

        HUNGUP_AND_CRASH("../game.c", LN4(940, 943, 1030, 1044, 1044));
    }
}

void game_ct(GameState* gameState, GameStateFunc init, GraphicsContext* gfxCtx) {
    UNUSED_NDEBUG OSTime startTime;
    UNUSED_NDEBUG OSTime endTime;

    PRINTF(T("game コンストラクタ開始\n", "game constructor start\n"));
    gameState->gfxCtx = gfxCtx;
    gameState->frames = 0;
    gameState->main = NULL;
    gameState->destroy = NULL;
    gameState->running = 1;
    startTime = osGetTime();

    // Thse assignments must be written this way for matching and to avoid a warning due to casting a pointer to an
    // integer without a cast. This assigns init = NULL and size = 0.
    gameState->size = (u32)(gameState->init = NULL);

    {
        s32 requiredScopeTemp;
        endTime = osGetTime();
        PRINTF(T("game_set_next_game_null 処理時間 %d us\n", "game_set_next_game_null processing time %d us\n"),
               OS_CYCLES_TO_USEC(endTime - startTime));
        startTime = endTime;
        gamealloc_init(&gameState->alloc);
    }

    endTime = osGetTime();
    PRINTF(T("gamealloc_init 処理時間 %d us\n", "gamealloc_init processing time %d us\n"),
           OS_CYCLES_TO_USEC(endTime - startTime));
    startTime = endTime;
    game_init_hyral(gameState, 0x100000);

    R_UPDATE_RATE = 3;
    init(gameState);
    endTime = osGetTime();
    PRINTF(T("init 処理時間 %d us\n", "init processing time %d us\n"), OS_CYCLES_TO_USEC(endTime - startTime));

    startTime = endTime;
    LOG_UTILS_CHECK_NULL_POINTER("this->cleanup", gameState->destroy, "../game.c", 1088);
    z_viscvg_init(&z_viscvg);
    z_viszbuf_init(&z_viszbuf);
    z_vismono_init(&z_vismono);
    if ((R_VI_MODE_EDIT_STATE == VI_MODE_EDIT_STATE_INACTIVE) || !DEBUG_FEATURES) {
        z_vimode_init(&z_vimode);
    }
    speed_meter_init(&speed_meter);
    z_vibctl2_init();
    osSendMesg(&gameState->gfxCtx->queue, NULL, OS_MESG_BLOCK);
    endTime = osGetTime();
    PRINTF(T("その他初期化 処理時間 %d us\n", "Other initialization processing time %d us\n"),
           OS_CYCLES_TO_USEC(endTime - startTime));

#if DEBUG_FEATURES
    fault_AddClient(&sGameFaultClient, GameState_FaultPrint, NULL, NULL);
#endif

    PRINTF(T("game コンストラクタ終了\n", "game constructor end\n"));
}

void game_dt(GameState* gameState) {
    PRINTF(T("game デストラクタ開始\n", "game destructor start\n"));
    audio_StopAllSoundEffect();
    Nas_AudioInput();
    osRecvMesg(&gameState->gfxCtx->queue, NULL, OS_MESG_BLOCK);
    LOG_UTILS_CHECK_NULL_POINTER("this->cleanup", gameState->destroy, "../game.c", 1139);
    if (gameState->destroy != NULL) {
        gameState->destroy(gameState);
    }
    z_vibctl2_cleanup();
    speed_meter_cleanup(&speed_meter);
    z_viscvg_cleanup(&z_viscvg);
    z_viszbuf_cleanup(&z_viszbuf);
    z_vismono_cleanup(&z_vismono);
    if ((R_VI_MODE_EDIT_STATE == VI_MODE_EDIT_STATE_INACTIVE) || !DEBUG_FEATURES) {
        z_vimode_cleanup(&z_vimode);
    }
    THA_dt(&gameState->tha);
    gamealloc_cleanup(&gameState->alloc);

#if PLATFORM_GC && DEBUG_FEATURES
    SystemArena_Display();
#endif

#if DEBUG_FEATURES
    fault_RemoveClient(&sGameFaultClient);
#endif

    PRINTF(T("game デストラクタ終了\n", "game destructor end\n"));
}

GameStateFunc game_get_next_game_init(GameState* gameState) {
    return gameState->init;
}

size_t game_get_next_game_class_size(GameState* gameState) {
    return gameState->size;
}

u32 game_is_doing(GameState* gameState) {
    return gameState->running;
}

#if DEBUG_FEATURES
void* GameState_Alloc(GameState* gameState, size_t size, const char* file, int line) {
    void* ret;

    if (THA_isCrash(&gameState->tha)) {
        PRINTF(T("ハイラルは滅亡している\n", "Hyrule is destroyed\n"));
        ret = NULL;
    } else if ((u32)THA_getFreeBytes(&gameState->tha) < size) {
        PRINTF(T("滅亡寸前のハイラルには %d バイトの余力もない（滅亡まであと %d バイト）\n",
                 "Hyrule on the verge of extinction does not have %d bytes left (%d bytes until extinction)\n"),
               size, THA_getFreeBytes(&gameState->tha));
        ret = NULL;
    } else {
        ret = THA_alloc16(&gameState->tha, size);
        if (THA_isCrash(&gameState->tha)) {
            PRINTF(T("ハイラルは滅亡してしまった\n", "Hyrule has been destroyed\n"));
            ret = NULL;
        }
    }
    if (ret != NULL) {
        PRINTF_COLOR_GREEN();
        PRINTF("game_alloc(%08x) %08x-%08x [%s:%d]\n", size, ret, (uintptr_t)ret + size, file, line);
        PRINTF_RST();
    }
    return ret;
}

void* GameState_AllocEndAlign16(GameState* gameState, size_t size) {
    return THA_alloc16(&gameState->tha, size);
}
#endif

s32 game_getFreeBytes(GameState* gameState) {
    return THA_getFreeBytes(&gameState->tha);
}
