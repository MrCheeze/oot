#include "global.h"
#include "fault.h"
#include "terminal.h"
#include "ucode_disas.h"
#include "versions.h"
#include "line_numbers.h"

#define GFXPOOL_HEAD_MAGIC 0x1234
#define GFXPOOL_TAIL_MAGIC 0x5678

#pragma increment_block_number "gc-eu:0 gc-eu-mq:0 gc-jp:0 gc-jp-ce:0 gc-jp-mq:0 gc-us:0 gc-us-mq:0 ntsc-1.0:160" \
                               "ntsc-1.1:160 ntsc-1.2:160 pal-1.0:160 pal-1.1:160 hiratsu3:0"

/**
 * The time at which the previous `graph_main` ended.
 */
OSTime frame_start_time;

/**
 * The time at which the previous graphics task was scheduled to run.
 */
OSTime graph_cpu_start_time;

#if DEBUG_FEATURES
FaultClient sGraphFaultClient;

UCodeInfo D_8012D230[3] = {
    { UCODE_TYPE_F3DZEX, gspF3DZEX2_NoN_PosLight_fifoTextStart },
    { UCODE_TYPE_UNK, NULL },
    { UCODE_TYPE_S2DEX, gspS2DEX2d_fifoTextStart },
};

UCodeInfo D_8012D248[3] = {
    { UCODE_TYPE_F3DZEX, gspF3DZEX2_NoN_PosLight_fifoTextStart },
    { UCODE_TYPE_UNK, NULL },
    { UCODE_TYPE_S2DEX, gspS2DEX2d_fifoTextStart },
};

void Graph_FaultClient(void) {
    void* nextFb = osViGetNextFramebuffer();
    void* newFb = (sys_cfb_getptr(0) != nextFb) ? sys_cfb_getptr(0) : sys_cfb_getptr(1);

    osViSwapBuffer(newFb);
    fault_KeyWait();
    osViSwapBuffer(nextFb);
}

// TODO: merge Gfx and GfxMod to make this function's arguments consistent
void UCodeDisas_Disassemble(UCodeDisas*, Gfx*);

void Graph_DisassembleUCode(Gfx* workBuf) {
    UCodeDisas disassembler;

    if (R_HREG_MODE == HREG_MODE_UCODE_DISAS && R_UCODE_DISAS_TOGGLE != 0) {
        UCodeDisas_Init(&disassembler);
        disassembler.enableLog = R_UCODE_DISAS_LOG_LEVEL;

        UCodeDisas_RegisterUCode(&disassembler, ARRAY_COUNT(D_8012D230), D_8012D230);
        UCodeDisas_SetCurUCode(&disassembler, gspF3DZEX2_NoN_PosLight_fifoTextStart);

        UCodeDisas_Disassemble(&disassembler, workBuf);

        R_UCODE_DISAS_DL_COUNT = disassembler.dlCnt;
        R_UCODE_DISAS_TOTAL_COUNT =
            disassembler.tri2Cnt * 2 + disassembler.tri1Cnt + (disassembler.quadCnt * 2) + disassembler.lineCnt;
        R_UCODE_DISAS_VTX_COUNT = disassembler.vtxCnt;
        R_UCODE_DISAS_SPVTX_COUNT = disassembler.spvtxCnt;
        R_UCODE_DISAS_TRI1_COUNT = disassembler.tri1Cnt;
        R_UCODE_DISAS_TRI2_COUNT = disassembler.tri2Cnt;
        R_UCODE_DISAS_QUAD_COUNT = disassembler.quadCnt;
        R_UCODE_DISAS_LINE_COUNT = disassembler.lineCnt;
        R_UCODE_DISAS_SYNC_ERROR_COUNT = disassembler.syncErr;
        R_UCODE_DISAS_LOAD_COUNT = disassembler.loaducodeCnt;

        if (R_UCODE_DISAS_LOG_MODE == 1 || R_UCODE_DISAS_LOG_MODE == 2) {
            PRINTF("vtx_cnt=%d\n", disassembler.vtxCnt);
            PRINTF("spvtx_cnt=%d\n", disassembler.spvtxCnt);
            PRINTF("tri1_cnt=%d\n", disassembler.tri1Cnt);
            PRINTF("tri2_cnt=%d\n", disassembler.tri2Cnt);
            PRINTF("quad_cnt=%d\n", disassembler.quadCnt);
            PRINTF("line_cnt=%d\n", disassembler.lineCnt);
            PRINTF("sync_err=%d\n", disassembler.syncErr);
            PRINTF("loaducode_cnt=%d\n", disassembler.loaducodeCnt);
            PRINTF("dl_depth=%d\n", disassembler.dlDepth);
            PRINTF("dl_cnt=%d\n", disassembler.dlCnt);
        }

        UCodeDisas_Destroy(&disassembler);
    }
}

void Graph_UCodeFaultClient(Gfx* workBuf) {
    UCodeDisas disassembler;

    UCodeDisas_Init(&disassembler);
    disassembler.enableLog = true;
    UCodeDisas_RegisterUCode(&disassembler, ARRAY_COUNT(D_8012D248), D_8012D248);
    UCodeDisas_SetCurUCode(&disassembler, gspF3DZEX2_NoN_PosLight_fifoTextStart);
    UCodeDisas_Disassemble(&disassembler, workBuf);
    UCodeDisas_Destroy(&disassembler);
}
#endif

void graph_setup_double_buffer(GraphicsContext* gfxCtx) {
    GfxPool* pool = &sys_dynamic[gfxCtx->gfxPoolIdx & 1];

    pool->headMagic = GFXPOOL_HEAD_MAGIC;
    pool->tailMagic = GFXPOOL_TAIL_MAGIC;
    THA_GA_ct(&gfxCtx->polyOpa, pool->polyOpaBuffer, sizeof(pool->polyOpaBuffer));
    THA_GA_ct(&gfxCtx->polyXlu, pool->polyXluBuffer, sizeof(pool->polyXluBuffer));
    THA_GA_ct(&gfxCtx->overlay, pool->overlayBuffer, sizeof(pool->overlayBuffer));
    THA_GA_ct(&gfxCtx->work, pool->workBuffer, sizeof(pool->workBuffer));

    gfxCtx->polyOpaBuffer = pool->polyOpaBuffer;
    gfxCtx->polyXluBuffer = pool->polyXluBuffer;
    gfxCtx->overlayBuffer = pool->overlayBuffer;
    gfxCtx->workBuffer = pool->workBuffer;

    gfxCtx->curFrameBuffer = sys_cfb_getptr(gfxCtx->fbIdx % 2);
    gfxCtx->unk_014 = 0;
}

GameStateOverlay* game_get_next_game_dlftbl(GameState* gameState) {
    void* gameStateInitFunc = game_get_next_game_init(gameState);

    // Generates code to match gameStateInitFunc to a gamestate entry and returns it if found
#define DEFINE_GAMESTATE_INTERNAL(typeName, enumName, name) \
    if (gameStateInitFunc == name##_init) {                 \
        return &game_dlftbls[enumName];           \
    }
#define DEFINE_GAMESTATE(typeName, enumName, name) DEFINE_GAMESTATE_INTERNAL(typeName, enumName, name)
#include "tables/gamestate_table.h"
#undef DEFINE_GAMESTATE
#undef DEFINE_GAMESTATE_INTERNAL

    LOG_ADDRESS("game_init_func", gameStateInitFunc, "../graph.c", 696);
    return NULL;
}

void graph_ct(GraphicsContext* gfxCtx) {
    bzero(gfxCtx, sizeof(GraphicsContext));
    gfxCtx->gfxPoolIdx = 0;
    gfxCtx->fbIdx = 0;
    gfxCtx->viMode = NULL;

#if OOT_VERSION < PAL_1_0
    gfxCtx->viFeatures = 0;
#else
    gfxCtx->viFeatures = vispecial;
    gfxCtx->xScale = vixscale;
    gfxCtx->yScale = viyscale;
#endif

    osCreateMesgQueue(&gfxCtx->queue, gfxCtx->msgBuff, ARRAY_COUNT(gfxCtx->msgBuff));

#if DEBUG_FEATURES
    func_800D31F0();
    fault_AddClient(&sGraphFaultClient, Graph_FaultClient, NULL, NULL);
#endif
}

void graph_dt(GraphicsContext* gfxCtx) {
#if DEBUG_FEATURES
    func_800D3210();
    fault_RemoveClient(&sGraphFaultClient);
#endif
}

void graph_task_set00(GraphicsContext* gfxCtx) {
#if DEBUG_FEATURES
    static Gfx* sPrevTaskWorkBuffer = NULL;
#endif
    OSTask_t* task = &gfxCtx->task.list.t;
    OSScTask* scTask = &gfxCtx->task;

    graph_cpu_time =
        osGetTime() - graph_cpu_start_time - audio_cpu_sum_time;

    {
        OSTimer timer;
        OSMesg msg;

        // Schedule a message to be handled in 3 seconds, for RCP timeout
        osSetTimer(&timer, OS_USEC_TO_CYCLES(3000000), 0, &gfxCtx->queue, (OSMesg)666);

        osRecvMesg(&gfxCtx->queue, &msg, OS_MESG_BLOCK);
        osStopTimer(&timer);

        if (msg == (OSMesg)666) {
#if DEBUG_FEATURES
            PRINTF_COLOR_RED();
            PRINTF(T("RCPが帰ってきませんでした。", "RCP did not return."));
            PRINTF_RST();

            LogUtils_LogHexDump((void*)PHYS_TO_K1(SP_BASE_REG), 0x20);
            LogUtils_LogHexDump((void*)PHYS_TO_K1(DPC_BASE_REG), 0x20);
            LogUtils_LogHexDump(sys_yield_buffer, sizeof(sys_yield_buffer));

            SREG(6) = -1;
            if (sPrevTaskWorkBuffer != NULL) {
                R_HREG_MODE = HREG_MODE_UCODE_DISAS;
                R_UCODE_DISAS_TOGGLE = 1;
                R_UCODE_DISAS_LOG_LEVEL = 2;
                Graph_DisassembleUCode(sPrevTaskWorkBuffer);
            }
#endif

            fault_HungUpMsg("RCP is HUNG UP!!", "Oh! MY GOD!!");
        }

        osRecvMesg(&gfxCtx->queue, &msg, OS_MESG_NOBLOCK);

#if DEBUG_FEATURES
        sPrevTaskWorkBuffer = gfxCtx->workBuffer;
#endif
    }

    if (gfxCtx->callback != NULL) {
        gfxCtx->callback(gfxCtx, gfxCtx->callbackParam);
    }

    {
        OSTime timeNow = osGetTime();

        if (audio_cpu_start_time != 0) {
            // The audio thread update is running
            // Add the time already spent to the accumulator and leave the rest for the next cycle

            audio_cpu_sum_time += timeNow - audio_cpu_start_time;
            audio_cpu_start_time = timeNow;
        }
        audio_cpu_time = audio_cpu_sum_time;
        audio_cpu_sum_time = 0;

        graph_cpu_start_time = osGetTime();
    }

    task->type = M_GFXTASK;
    task->flags = OS_SC_DRAM_DLIST;
    task->ucode_boot = ucode_GetRspBootTextStart();
    task->ucode_boot_size = ucode_GetRspBootTextSize();
    task->ucode = ucode_GetPolyTextStart();
    task->ucode_data = ucode_GetPolyDataStart();
    task->ucode_size = SP_UCODE_SIZE;
    task->ucode_data_size = SP_UCODE_DATA_SIZE;
    task->dram_stack = sys_dram_stack;
    task->dram_stack_size = sizeof(sys_dram_stack);
    task->output_buff = sys_fifo_buffer;
    task->output_buff_size = sys_fifo_buffer + ARRAY_COUNT(sys_fifo_buffer);
    task->data_ptr = (u64*)gfxCtx->workBuffer;

    OPEN_DISPS(gfxCtx, "../graph.c", 828);
    task->data_size = (uintptr_t)WORK_DISP - (uintptr_t)gfxCtx->workBuffer;
    CLOSE_DISPS(gfxCtx, "../graph.c", 830);

    task->yield_data_ptr = sys_yield_buffer;

    task->yield_data_size = sizeof(sys_yield_buffer);

    scTask->next = NULL;
    scTask->flags = OS_SC_NEEDS_RSP | OS_SC_NEEDS_RDP | OS_SC_SWAPBUFFER | OS_SC_LAST_TASK;
    if (R_GRAPH_TASKSET00_FLAGS & 1) {
        R_GRAPH_TASKSET00_FLAGS &= ~1;
        scTask->flags &= ~OS_SC_SWAPBUFFER;
        gfxCtx->fbIdx--;
    }

    scTask->msgQueue = &gfxCtx->queue;
    scTask->msg = NULL;

    {
        static CfbInfo cfbinfo_tbl[3];
        static s32 cfbinfo_idx = 0;
        CfbInfo* cfb;

        cfb = &cfbinfo_tbl[cfbinfo_idx];

        cfbinfo_idx = (cfbinfo_idx + 1) % ARRAY_COUNT(cfbinfo_tbl);
        cfb->framebuffer = gfxCtx->curFrameBuffer;
        cfb->swapBuffer = gfxCtx->curFrameBuffer;

        cfb->viMode = gfxCtx->viMode;
        cfb->viFeatures = gfxCtx->viFeatures;
#if OOT_VERSION >= PAL_1_0
        cfb->xScale = gfxCtx->xScale;
        cfb->yScale = gfxCtx->yScale;
#endif
        cfb->unk_10 = 0;
        cfb->updateRate = R_UPDATE_RATE;

        scTask->framebuffer = cfb;
    }

    gfxCtx->schedMsgQueue = &_sched.cmdQueue;

    osSendMesg(&_sched.cmdQueue, (OSMesg)scTask, OS_MESG_BLOCK);
    osScKickEntryMsg(&_sched);
}

void graph_main(GraphicsContext* gfxCtx, GameState* gameState) {
    u32 problem;

    gameState->inPreNMIState = false;
    graph_setup_double_buffer(gfxCtx);

#if DEBUG_FEATURES
    OPEN_DISPS(gfxCtx, "../graph.c", 966);

    gDPNoOpString(WORK_DISP++, "WORK_DISP 開始", 0);
    gDPNoOpString(POLY_OPA_DISP++, "POLY_OPA_DISP 開始", 0);
    gDPNoOpString(POLY_XLU_DISP++, "POLY_XLU_DISP 開始", 0);
    gDPNoOpString(OVERLAY_DISP++, "OVERLAY_DISP 開始", 0);

    CLOSE_DISPS(gfxCtx, "../graph.c", 975);
#endif

    game_get_controller(gameState);
    game_main(gameState);

#if DEBUG_FEATURES
    OPEN_DISPS(gfxCtx, "../graph.c", 987);

    gDPNoOpString(WORK_DISP++, "WORK_DISP 終了", 0);
    gDPNoOpString(POLY_OPA_DISP++, "POLY_OPA_DISP 終了", 0);
    gDPNoOpString(POLY_XLU_DISP++, "POLY_XLU_DISP 終了", 0);
    gDPNoOpString(OVERLAY_DISP++, "OVERLAY_DISP 終了", 0);

    CLOSE_DISPS(gfxCtx, "../graph.c", 996);
#endif

    OPEN_DISPS(gfxCtx, "../graph.c", 999);

    gSPBranchList(WORK_DISP++, gfxCtx->polyOpaBuffer);
    gSPBranchList(POLY_OPA_DISP++, gfxCtx->polyXluBuffer);
    gSPBranchList(POLY_XLU_DISP++, gfxCtx->overlayBuffer);
    gDPPipeSync(OVERLAY_DISP++);
    gDPFullSync(OVERLAY_DISP++);
    gSPEndDisplayList(OVERLAY_DISP++);

    CLOSE_DISPS(gfxCtx, "../graph.c", 1028);

#if DEBUG_FEATURES
    if (R_HREG_MODE == HREG_MODE_PLAY && R_PLAY_ENABLE_UCODE_DISAS == 2) {
        R_HREG_MODE = HREG_MODE_UCODE_DISAS;
        R_UCODE_DISAS_TOGGLE = -1;
        R_UCODE_DISAS_LOG_LEVEL = R_PLAY_UCODE_DISAS_LOG_LEVEL;
    }

    if (R_HREG_MODE == HREG_MODE_UCODE_DISAS && R_UCODE_DISAS_TOGGLE != 0) {
        static FaultClient sGraphUcodeFaultClient;

        if (R_UCODE_DISAS_LOG_MODE == 3) {
            fault_AddClient(&sGraphUcodeFaultClient, Graph_UCodeFaultClient, gfxCtx->workBuffer, "do_count_fault");
        }

        Graph_DisassembleUCode(gfxCtx->workBuffer);

        if (R_UCODE_DISAS_LOG_MODE == 3) {
            fault_RemoveClient(&sGraphUcodeFaultClient);
        }

        if (R_UCODE_DISAS_TOGGLE < 0) {
            LogUtils_LogHexDump((void*)PHYS_TO_K1(SP_BASE_REG), 0x20);
            LogUtils_LogHexDump((void*)PHYS_TO_K1(DPC_BASE_REG), 0x20);
        }

        if (R_UCODE_DISAS_TOGGLE < 0) {
            R_UCODE_DISAS_TOGGLE = 0;
        }
    }
#endif

    problem = false;

    {
        GfxPool* pool = &sys_dynamic[gfxCtx->gfxPoolIdx & 1];

        if (pool->headMagic != GFXPOOL_HEAD_MAGIC) {
            //! @bug (?) : "problem = true;" may be missing
            PRINTF("%c", BEL);
            PRINTF(VT_COL(RED, WHITE) T("ダイナミック領域先頭が破壊されています\n", "Dynamic area head is destroyed\n")
                       VT_RST);
            fault_HungUp("../graph.c", LN4(937, 940, 951, 1067, 1070));
        }

        if (pool->tailMagic != GFXPOOL_TAIL_MAGIC) {
            problem = true;
            PRINTF("%c", BEL);
            PRINTF(VT_COL(RED, WHITE)
                       T("ダイナミック領域末尾が破壊されています\n", "Dynamic region tail is destroyed\n") VT_RST);
            fault_HungUp("../graph.c", LN4(943, 946, 957, 1073, 1076));
        }
    }

    if (THA_GA_isCrash(&gfxCtx->polyOpa)) {
        problem = true;
        PRINTF("%c", BEL);
        PRINTF(VT_COL(RED, WHITE) T("ゼルダ0は死んでしまった(graph_alloc is empty)\n",
                                    "Zelda 0 is dead (graph_alloc is empty)\n") VT_RST);
    }
    if (THA_GA_isCrash(&gfxCtx->polyXlu)) {
        problem = true;
        PRINTF("%c", BEL);
        PRINTF(VT_COL(RED, WHITE) T("ゼルダ1は死んでしまった(graph_alloc is empty)\n",
                                    "Zelda 1 is dead (graph_alloc is empty)\n") VT_RST);
    }
    if (THA_GA_isCrash(&gfxCtx->overlay)) {
        problem = true;
        PRINTF("%c", BEL);
        PRINTF(VT_COL(RED, WHITE) T("ゼルダ4は死んでしまった(graph_alloc is empty)\n",
                                    "Zelda 4 is dead (graph_alloc is empty)\n") VT_RST);
    }

    if (!problem) {
        graph_task_set00(gfxCtx);
        gfxCtx->gfxPoolIdx++;
        gfxCtx->fbIdx++;
    }

    Nas_AudioInput();

    {
        OSTime timeNow = osGetTime();
        s32 pad;

        graph_rsp_time = graph_rsp_sum_time;
        audio_rsp_time = audio_rsp_sum_time;
        rdp_time = rdp_sum_time;
        graph_rsp_sum_time = 0;
        audio_rsp_sum_time = 0;
        rdp_sum_time = 0;

        if (frame_start_time != 0) {
            frame_time = timeNow - frame_start_time;
        }
        frame_start_time = timeNow;
    }

#if DEBUG_FEATURES
    if (gIsCtrlr2Valid && CHECK_BTN_ALL(gameState->input[0].press.button, BTN_Z) &&
        CHECK_BTN_ALL(gameState->input[0].cur.button, BTN_L | BTN_R)) {
        z_common_data.gameMode = GAMEMODE_NORMAL;
        SET_NEXT_GAMESTATE(gameState, select_init, MapSelectState);
        gameState->running = false;
    }

    if (gIsCtrlr2Valid && z_nmibuf_isprenmi(z_nmibuf) && !gameState->inPreNMIState) {
        PRINTF(VT_COL(YELLOW, BLACK) T("PRE-NMIによりリセットモードに移行します\n",
                                       "PRE-NMI causes the system to transition to reset mode\n") VT_RST);
        SET_NEXT_GAMESTATE(gameState, prenmi_init, PreNMIState);
        gameState->running = false;
    }
#endif
}

void graph_proc(void* arg0) {
    GraphicsContext gfxCtx;
    GameState* gameState;
    u32 size;
    GameStateOverlay* nextOvl = &game_dlftbls[GAMESTATE_SETUP];
    GameStateOverlay* ovl;

    PRINTF(T("グラフィックスレッド実行開始\n", "Start graphic thread execution\n"));
    graph_ct(&gfxCtx);

    while (nextOvl != NULL) {
        ovl = nextOvl;
        DLFTBL_link(ovl);

        size = ovl->instanceSize;
        PRINTF(T("クラスサイズ＝%dバイト\n", "Class size = %d bytes\n"), size);

        gameState = SYSTEM_ARENA_MALLOC(size, "../graph.c", 1196);

        if (gameState == NULL) {
#if DEBUG_FEATURES
            char faultMsg[0x50];

            PRINTF(T("確保失敗\n", "Failure to secure\n"));

            sprintf(faultMsg, "CLASS SIZE= %d bytes", size);
            fault_HungUpMsg("GAME CLASS MALLOC FAILED", faultMsg);
#else
            fault_HungUp("../graph.c", LN4(1067, 1070, 1081, 1197, 1200));
#endif
        }

        game_ct(gameState, ovl->init, &gfxCtx);

        while (game_is_doing(gameState)) {
            graph_main(&gfxCtx, gameState);
        }

        nextOvl = game_get_next_game_dlftbl(gameState);
        game_dt(gameState);
        SYSTEM_ARENA_FREE(gameState, "../graph.c", 1227);
        DLFTBL_unlink(ovl);
    }
    graph_dt(&gfxCtx);
    PRINTF(T("グラフィックスレッド実行終了\n", "End of graphic thread execution\n"));
}

void* _graph_alloc(GraphicsContext* gfxCtx, size_t size) {
    TwoHeadGfxArena* thga = &gfxCtx->polyOpa;

    if (HREG(59) == 1) {
        PRINTF("graph_alloc siz=%d thga size=%08x bufp=%08x head=%08x tail=%08x\n", size, thga->size, thga->start,
               thga->p, thga->d);
    }
    return THA_GA_alloc(&gfxCtx->polyOpa, ALIGN16(size));
}

void* graph_alloc(GraphicsContext* gfxCtx, size_t size) {
    TwoHeadGfxArena* thga = &gfxCtx->polyOpa;

    if (HREG(59) == 1) {
        PRINTF("graph_alloc siz=%d thga size=%08x bufp=%08x head=%08x tail=%08x\n", size, thga->size, thga->start,
               thga->p, thga->d);
    }
    return THA_GA_alloc(&gfxCtx->polyOpa, ALIGN16(size));
}

#if DEBUG_FEATURES
void Graph_OpenDisps(Gfx** dispRefs, GraphicsContext* gfxCtx, const char* file, int line) {
    if (R_HREG_MODE == HREG_MODE_UCODE_DISAS && R_UCODE_DISAS_LOG_MODE != 4) {
        dispRefs[0] = gfxCtx->polyOpa.p;
        dispRefs[1] = gfxCtx->polyXlu.p;
        dispRefs[2] = gfxCtx->overlay.p;

        gDPNoOpOpenDisp(gfxCtx->polyOpa.p++, file, line);
        gDPNoOpOpenDisp(gfxCtx->polyXlu.p++, file, line);
        gDPNoOpOpenDisp(gfxCtx->overlay.p++, file, line);
    }
}

void Graph_CloseDisps(Gfx** dispRefs, GraphicsContext* gfxCtx, const char* file, int line) {
    if (R_HREG_MODE == HREG_MODE_UCODE_DISAS && R_UCODE_DISAS_LOG_MODE != 4) {
        if (dispRefs[0] + 1 == gfxCtx->polyOpa.p) {
            gfxCtx->polyOpa.p = dispRefs[0];
        } else {
            gDPNoOpCloseDisp(gfxCtx->polyOpa.p++, file, line);
        }

        if (dispRefs[1] + 1 == gfxCtx->polyXlu.p) {
            gfxCtx->polyXlu.p = dispRefs[1];
        } else {
            gDPNoOpCloseDisp(gfxCtx->polyXlu.p++, file, line);
        }

        if (dispRefs[2] + 1 == gfxCtx->overlay.p) {
            gfxCtx->overlay.p = dispRefs[2];
        } else {
            gDPNoOpCloseDisp(gfxCtx->overlay.p++, file, line);
        }
    }
}
#endif
