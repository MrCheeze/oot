#include "global.h"

Gfx* gfxopen(Gfx* gfx) {
    return gfx + 1;
}

Gfx* gfxclose(Gfx* gfx, Gfx* dst) {
    gSPBranchList(gfx, dst);
    return dst;
}

void* gfxalloc(Gfx** gfxP, u32 size) {
    u8* ptr;
    Gfx* dst;

    size = ALIGN8(size);

    ptr = (u8*)(*gfxP + 1);

    dst = (Gfx*)(ptr + size);
    gSPBranchList(*gfxP, dst);

    *gfxP = dst;
    return ptr;
}
