#include "global.h"

void osInvalDCache2(void* buf, s32 size) {
    OSIntMask prevMask = osSetIntMask(OS_IM_NONE);

    osInvalDCache(buf, size);
    osSetIntMask(prevMask);
}

void osWritebackDCache2(void* buf, s32 size) {
    OSIntMask prevMask = osSetIntMask(OS_IM_NONE);

    osWritebackDCache(buf, size);
    osSetIntMask(prevMask);
}
