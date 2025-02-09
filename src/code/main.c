#include "ultra64.h"
#include "versions.h"

// Declared before including other headers for BSS ordering
extern uintptr_t SegmentBaseAddress[NUM_SEGMENTS];

#pragma increment_block_number "gc-eu:252 gc-eu-mq:252 gc-jp:252 gc-jp-ce:252 gc-jp-mq:252 gc-us:252 gc-us-mq:252" \
                               "ique-cn:252 ntsc-1.0:128 ntsc-1.1:128 ntsc-1.2:128 pal-1.0:128 pal-1.1:128 hiratsu3:252"

extern struct PreNmiBuff* z_nmibuf;
extern struct Scheduler _sched;
extern struct PadMgr padmgr;
extern struct IrqMgr _irqmgr;

#include "global.h"
#include "fault.h"
#include "segmented_address.h"
#include "stack.h"
#include "terminal.h"
#include "versions.h"
#if PLATFORM_N64
#include "cic6105.h"
#include "n64dd.h"
#endif

#pragma increment_block_number "gc-eu:144 gc-eu-mq:144 gc-jp:144 gc-jp-ce:144 gc-jp-mq:144 gc-us:144 gc-us-mq:144" \
                               "ique-cn:128 ntsc-1.0:122 ntsc-1.1:122 ntsc-1.2:122 pal-1.0:120 pal-1.1:120 hiratsu3:144"

extern u8 _buffersSegmentEnd[];

s32 ScreenWidth = SCREEN_WIDTH;
s32 ScreenHeight = SCREEN_HEIGHT;
u32 SystemHeapSize = 0;

PreNmiBuff* z_nmibuf;
Scheduler _sched;
PadMgr padmgr;
IrqMgr _irqmgr;
uintptr_t SegmentBaseAddress[NUM_SEGMENTS];

OSThread graphThread;
STACK(graphStack, 0x1800);
#if OOT_VERSION < PAL_1_0
STACK(schedStack, 0x400);
#else
STACK(schedStack, 0x600);
#endif
STACK(audioStack, 0x800);
STACK(padmgrStack, 0x500);
STACK(irqmgrStack, 0x500);
StackEntry graph_stackcheck;
StackEntry sched_stackcheck;
StackEntry audio_stackcheck;
StackEntry padmgr_stackcheck;
StackEntry irqmgr_stackcheck;
AudioMgr amAudioMgr;
OSMesgQueue serialMsgQ;
OSMesg serialMsgBuf[1];

#if DEBUG_FEATURES
void Main_LogSystemHeap(void) {
    PRINTF_COLOR_GREEN();
    PRINTF(
        T("システムヒープサイズ %08x(%dKB) 開始アドレス %08x\n", "System heap size %08x (%dKB) Start address %08x\n"),
        SystemHeapSize, SystemHeapSize / 1024, _buffersSegmentEnd);
    PRINTF_RST();
}
#endif

void mainproc(void* arg) {
    IrqMgrClient irqClient;
    OSMesgQueue irqMgrMsgQueue;
    OSMesg irqMgrMsgBuf[60];
    uintptr_t systemHeapStart;
    uintptr_t fb;

    PRINTF(T("mainproc 実行開始\n", "mainproc Start running\n"));
    ScreenWidth = SCREEN_WIDTH;
    ScreenHeight = SCREEN_HEIGHT;
    z_nmibuf = (PreNmiBuff*)osAppNMIBuffer;
    z_nmibuf_init(z_nmibuf);
    DbStartDebugger();
#if PLATFORM_N64
    func_800AD410();
    if (D_80121211 != 0) {
        systemHeapStart = (uintptr_t)_n64ddSegmentEnd;
        sys_cfb_init(1);
    } else {
        func_800AD488();
        systemHeapStart = (uintptr_t)_buffersSegmentEnd;
        sys_cfb_init(0);
    }
#else
    sys_cfb_init(0);
    systemHeapStart = (uintptr_t)_buffersSegmentEnd;
#endif
    fb = (uintptr_t)sys_cfb_getptr(0);
    SystemHeapSize = fb - systemHeapStart;
    PRINTF(T("システムヒープ初期化 %08x-%08x %08x\n", "System heap initialization %08x-%08x %08x\n"), systemHeapStart,
           fb, SystemHeapSize);
    osInitializeCPP((void*)systemHeapStart, SystemHeapSize); // initializes the system heap

#if DEBUG_FEATURES
    {
        void* debugHeapStart;
        u32 debugHeapSize;

        if (osMemSize >= 0x800000) {
            debugHeapStart = sys_cfb_get_bottom();
            debugHeapSize = PHYS_TO_K0(0x600000) - (uintptr_t)debugHeapStart;
        } else {
            debugHeapSize = 0x400;
            debugHeapStart = SYSTEM_ARENA_MALLOC(debugHeapSize, "../main.c", 565);
        }

        PRINTF("debug_InitArena(%08x, %08x)\n", debugHeapStart, debugHeapSize);
        DebugArena_Init(debugHeapStart, debugHeapSize);
    }
#endif

    new_Debug_mode();

    R_ENABLE_ARENA_DBG = 0;

    osCreateMesgQueue(&serialMsgQ, serialMsgBuf, ARRAY_COUNT(serialMsgBuf));
    osSetEventMesg(OS_EVENT_SI, &serialMsgQ, NULL);

#if DEBUG_FEATURES
    Main_LogSystemHeap();
#endif

    osCreateMesgQueue(&irqMgrMsgQueue, irqMgrMsgBuf, ARRAY_COUNT(irqMgrMsgBuf));
    stackcheck_init(&irqmgr_stackcheck, irqmgrStack, STACK_TOP(irqmgrStack), 0, 0x100, "irqmgr");
    CreateIRQManager(&_irqmgr, STACK_TOP(irqmgrStack), THREAD_PRI_IRQMGR, 1);

    PRINTF(T("タスクスケジューラの初期化\n", "Initialize the task scheduler\n"));
    stackcheck_init(&sched_stackcheck, schedStack, STACK_TOP(schedStack), 0, 0x100, "sched");
    osCreateScheduler(&_sched, STACK_TOP(schedStack), THREAD_PRI_SCHED, vimode_no, 1, &_irqmgr);

#if PLATFORM_N64
    CIC6105_AddFaultClient();
    func_80001640();
#endif

    irqmgr_AddClient(&_irqmgr, &irqClient, &irqMgrMsgQueue);

    stackcheck_init(&audio_stackcheck, audioStack, STACK_TOP(audioStack), 0, 0x100, "audio");
    amInit(&amAudioMgr, STACK_TOP(audioStack), THREAD_PRI_AUDIOMGR, THREAD_ID_AUDIOMGR, &_sched, &_irqmgr);

    stackcheck_init(&padmgr_stackcheck, padmgrStack, STACK_TOP(padmgrStack), 0, 0x100, "padmgr");
    padmgr_Create(&padmgr, &serialMsgQ, &_irqmgr, THREAD_ID_PADMGR, THREAD_PRI_PADMGR, STACK_TOP(padmgrStack));

    amInitSync(&amAudioMgr);

    stackcheck_init(&graph_stackcheck, graphStack, STACK_TOP(graphStack), 0, 0x100, "graph");
    osCreateThread(&graphThread, THREAD_ID_GRAPH, graph_proc, arg, STACK_TOP(graphStack), THREAD_PRI_GRAPH);
    osStartThread(&graphThread);

#if OOT_VERSION >= PAL_1_0
    osSetThreadPri(NULL, THREAD_PRI_MAIN);
#endif

    while (true) {
        s16* msg = NULL;

        osRecvMesg(&irqMgrMsgQueue, (OSMesg*)&msg, OS_MESG_BLOCK);
        if (msg == NULL) {
            break;
        }
        switch (*msg) {
            case OS_SC_PRE_NMI_MSG:
                PRINTF(T("main.c: リセットされたみたいだよ\n", "main.c: Looks like it's been reset\n"));
#if OOT_VERSION < PAL_1_0
                stackcheck_check_stack(NULL);
#endif
                z_nmibuf_prenmi(z_nmibuf);
                break;
        }
    }

    PRINTF(T("mainproc 後始末\n", "mainproc Cleanup\n"));
    osDestroyThread(&graphThread);
    KillRspRdp();
#if PLATFORM_N64
    CIC6105_RemoveFaultClient();
#endif
    PRINTF(T("mainproc 実行終了\n", "mainproc End of execution\n"));
}
