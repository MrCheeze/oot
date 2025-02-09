#include "ultra64.h"
#include "libu64/debug.h"
#include "attributes.h"
#include "line_numbers.h"
#include "global.h"

#pragma increment_block_number "gc-eu:128 gc-eu-mq:128 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128" \
                               "pal-1.1:128 hiratsu3:128"

uintptr_t sys_cfb_P[2];
uintptr_t sys_cfb_bottom;

void sys_cfb_init(s32 n64dd) {
    u32 screenSize;
    UNUSED_NDEBUG uintptr_t tmpFbEnd;

    if (osMemSize >= 0x800000) {
        PRINTF(T("８Ｍバイト以上のメモリが搭載されています\n", "8MB or more memory is installed\n"));
        tmpFbEnd = 0x8044BE80;
        if (n64dd == 1) {
            PRINTF(T("RAM 8M mode (N64DD対応)\n", "RAM 8M mode (N64DD compatible)\n"));
#if DEBUG_FEATURES
            sys_cfb_bottom = 0x805FB000;
#else
            sys_cfb_bottom = 0x80600000;
#endif
        } else {
            PRINTF(T("このバージョンのマージンは %dK バイトです\n", "The margin for this version is %dK bytes\n"),
                   (0x4BC00 / 1024));
#if DEBUG_FEATURES
            sys_cfb_bottom = tmpFbEnd;
#else
            sys_cfb_bottom = 0x80400000;
#endif
        }
    } else if (osMemSize >= 0x400000) {
        PRINTF("RAM4M mode\n");
        sys_cfb_bottom = 0x80400000;
    } else {
        _dbg_hungup("../sys_cfb.c", LN4(305, 308, 322, 341, 354));
    }

    screenSize = SCREEN_WIDTH * SCREEN_HEIGHT;
    sys_cfb_bottom &= ~0x3F;

    if (1) {}

    PRINTF(T("システムが使用する最終アドレスは %08x です\n", "The final address used by the system is %08x\n"),
           sys_cfb_bottom);
    sys_cfb_P[0] = sys_cfb_bottom - (screenSize * 4);
    sys_cfb_P[1] = sys_cfb_bottom - (screenSize * 2);
    PRINTF(T("フレームバッファのアドレスは %08x と %08x です\n", "Frame buffer addresses are %08x and %08x\n"),
           sys_cfb_P[0], sys_cfb_P[1]);
}

void sys_cfb_cleanup(void) {
    sys_cfb_P[0] = 0;
    sys_cfb_P[1] = 0;
    sys_cfb_bottom = 0;
}

void* sys_cfb_getptr(s32 idx) {
    if (idx < 2) {
        return (void*)sys_cfb_P[idx];
    }
    return NULL;
}

void* sys_cfb_get_bottom(void) {
    return (void*)sys_cfb_bottom;
}
