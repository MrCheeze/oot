#include "global.h"
#include "stack.h"
#include "terminal.h"
#include "versions.h"

#pragma increment_block_number "gc-eu:64 gc-eu-mq:64 gc-jp:64 gc-jp-ce:64 gc-jp-mq:64 gc-us:64 gc-us-mq:64 ntsc-1.2:64 hiratsu3:64"

OSThread mainThread;
#if OOT_VERSION < PAL_1_0
STACK(mainStack, 0x800);
#else
STACK(mainStack, 0x900);
#endif
StackEntry main_stackcheck;
OSMesg PiMsgs[50];
OSMesgQueue _PiMsgQ;
OSViMode vimode;

#if OOT_VERSION < PAL_1_0
u8 vimode_no = OS_VI_NTSC_LPN1;
#else
u8 vimode_no;
#endif

// Unused
void* _dummy[] = {
    osStopThread,  __osSetHWIntrRoutine,
#if PLATFORM_N64
    osEPiWriteIo,  osEPiReadIo,
#endif
    __osSetFpcCsr, __osGetFpcCsr,        __osGetHWIntrRoutine, __osSetHWIntrRoutine, osViGetNextFramebuffer,
#if !PLATFORM_N64
    bcmp,
#endif
};

void mainx(void* arg) {
    OSTime time;

    PRINTF(T("mainx 実行開始\n", "mainx execution started\n"));
    CreateDmaManager();
    PRINTF(T("codeセグメントロード中...", "code segment loading..."));
    time = osGetTime();
    DMA_REQUEST_SYNC(_codeSegmentStart, (uintptr_t)_codeSegmentRomStart, _codeSegmentRomEnd - _codeSegmentRomStart,
                     "../idle.c", 238);
    time -= osGetTime();
    PRINTF(T("\rcodeセグメントロード中...完了\n", "\rcode segment loading... Done\n"));
    PRINTF(T("転送時間 %6.3f\n", "Transfer time %6.3f\n"));
    bzero(_codeSegmentBssStart, _codeSegmentBssEnd - _codeSegmentBssStart);
    PRINTF(T("codeセグメントBSSクリア完了\n", "code segment BSS cleared\n"));
    mainproc(arg);
    PRINTF(T("mainx 実行終了\n", "mainx execution finished\n"));
}

void idleproc(void* arg) {
    PRINTF(T("アイドルスレッド(idleproc)実行開始\n", "Idle thread (idleproc) execution started\n"));
    PRINTF(T("作製者    : %s\n", "Created by: %s\n"), __Creator__);
    PRINTF(T("作成日時  : %s\n", "Created   : %s\n"), __DateTime__);
    PRINTF("MAKEOPTION: %s\n", __MakeOption__);
    PRINTF_COLOR_GREEN();
    PRINTF(T("ＲＡＭサイズは %d キロバイトです(osMemSize/osGetMemSize)\n",
             "RAM size is %d kilobytes (osMemSize/osGetMemSize)\n"),
           (s32)osMemSize / 1024);
    PRINTF(T("_bootSegmentEnd(%08x) 以降のＲＡＭ領域はクリアされました(boot)\n",
             "The RAM area after _bootSegmentEnd(%08x) has been cleared (boot)\n"),
           _bootSegmentEnd);
    PRINTF(T("Ｚバッファのサイズは %d キロバイトです\n", "Z buffer size is %d kilobytes\n"), 0x96);
    PRINTF(T("ダイナミックバッファのサイズは %d キロバイトです\n", "The dynamic buffer size is %d kilobytes\n"), 0x92);
    PRINTF(T("ＦＩＦＯバッファのサイズは %d キロバイトです\n", "FIFO buffer size is %d kilobytes\n"), 0x60);
    PRINTF(T("ＹＩＥＬＤバッファのサイズは %d キロバイトです\n", "YIELD buffer size is %d kilobytes\n"), 3);
    PRINTF(T("オーディオヒープのサイズは %d キロバイトです\n", "Audio heap size is %d kilobytes\n"),
           ((intptr_t)&AUDIOHP[ARRAY_COUNT(AUDIOHP)] - (intptr_t)AUDIOHP) / 1024);
    PRINTF_RST();

    osCreateViManager(OS_PRIORITY_VIMGR);

#if OOT_VERSION >= PAL_1_0
    vispecial = OS_VI_GAMMA_OFF | OS_VI_DITHER_FILTER_ON;
    vixscale = 1.0f;
    viyscale = 1.0f;
#endif

#if DEBUG_FEATURES
    // Allow both 60 Hz and 50 Hz
    switch (osTvType) {
        case OS_TV_NTSC:
            vimode_no = OS_VI_NTSC_LAN1;
            vimode = osViModeNtscLan1;
            break;

        case OS_TV_MPAL:
            vimode_no = OS_VI_MPAL_LAN1;
            vimode = osViModeMpalLan1;
            break;

        case OS_TV_PAL:
            vimode_no = OS_VI_FPAL_LAN1;
            vimode = osViModeFpalLan1;
#if OOT_VERSION >= PAL_1_0
            viyscale = 0.833f;
#endif
            break;
    }
#elif !OOT_PAL_N64
    // 60 Hz only (GameCube and NTSC N64)
    switch (osTvType) {
        case OS_TV_PAL:
        case OS_TV_NTSC:
            vimode_no = OS_VI_NTSC_LAN1;
            vimode = osViModeNtscLan1;
            break;

        case OS_TV_MPAL:
            vimode_no = OS_VI_MPAL_LAN1;
            vimode = osViModeMpalLan1;
            break;
    }
#else
    // 50 Hz only (PAL N64)
    switch (osTvType) {
        case OS_TV_NTSC:
        case OS_TV_MPAL:
        case OS_TV_PAL:
            vimode_no = OS_VI_FPAL_LAN1;
            vimode = osViModeFpalLan1;
            viyscale = 0.833f;
            break;
    }
#endif

#if OOT_VERSION < PAL_1_0
    osViSetMode(&vimode);
    osViBlack(true);
#else
    vidirty = 1;
    osViSetMode(&vimode);
    viBlack(true);
    osViBlack(true);
    osViSwapBuffer((void*)0x803DA80); //! @bug Invalid vram address (probably intended to be 0x803DA800)
#endif

    osCreatePiManager(OS_PRIORITY_PIMGR, &_PiMsgQ, PiMsgs, ARRAY_COUNT(PiMsgs));
    stackcheck_init(&main_stackcheck, mainStack, STACK_TOP(mainStack), 0, 0x400, "main");
    osCreateThread(&mainThread, THREAD_ID_MAIN, mainx, arg, STACK_TOP(mainStack), THREAD_PRI_MAIN_INIT);
    osStartThread(&mainThread);
    osSetThreadPri(NULL, OS_PRIORITY_IDLE);

    for (;;) {}
}
