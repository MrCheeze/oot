#include "global.h"
#include "boot.h"
#include "stack.h"
#if PLATFORM_N64
#include "cic6105.h"
#endif

#pragma increment_block_number "gc-eu:128 gc-eu-mq:128 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128" \
                               "ntsc-1.2:128 hiratsu3:128"

StackEntry boot_stackcheck;
OSThread idleThread;
STACK(idleStack, 0x400);
StackEntry idle_stackcheck;
#include "bootstack.inc.c"

void ClearRDRAM(void) {
    bzero(_bootSegmentEnd, osMemSize - OS_K0_TO_PHYSICAL(_bootSegmentEnd));
}

void boot(void) {
    stackcheck_init(&boot_stackcheck, bootStack, STACK_TOP(bootStack), 0, -1, "boot");

    osMemSize = osGetMemSize();
#if PLATFORM_N64
    func_80001720();
#endif
    ClearRDRAM();
    osInitialize();

    carthandle = osCartRomInit();
    osDriveRomInit();
#if DEBUG_FEATURES
    isPrintfInit();
#endif
    z_locale_init();

    stackcheck_init(&idle_stackcheck, idleStack, STACK_TOP(idleStack), 0, 256, "idle");
    osCreateThread(&idleThread, THREAD_ID_IDLE, idleproc, NULL, STACK_TOP(idleStack),
                   THREAD_PRI_IDLE_INIT);
    osStartThread(&idleThread);
}
