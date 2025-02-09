#include "global.h"

void* LoadFragment2(uintptr_t vromStart, uintptr_t vromEnd, void* vramStart, void* vramEnd) {
    void* allocatedRamAddr = SYSTEM_ARENA_MALLOC_R((intptr_t)vramEnd - (intptr_t)vramStart, "../loadfragment2.c", 31);

    if (loadfragment_verbose >= 3) {
        PRINTF("OVL:SPEC(%08x-%08x) REAL(%08x-%08x) OFFSET(%08x)\n", vramStart, vramEnd, allocatedRamAddr,
               ((uintptr_t)vramEnd - (uintptr_t)vramStart) + (uintptr_t)allocatedRamAddr,
               (uintptr_t)vramStart - (uintptr_t)allocatedRamAddr);
    }

    if (allocatedRamAddr != NULL) {
        LoadFragmentFix2(vromStart, vromEnd, vramStart, vramEnd, allocatedRamAddr);
    }

    return allocatedRamAddr;
}
