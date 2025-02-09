#ifndef THGA_H
#define THGA_H

#include "tha.h"

typedef union TwoHeadGfxArena {
    /* 0x00 */ TwoHeadArena tha;
    struct {       // Same as TwoHeadArena, with different types and field names for the head and tail pointers
        /* 0x00 */ size_t size;
        /* 0x04 */ void* start;
        /* 0x08 */ Gfx* p;
        /* 0x0C */ void* d;
    };
} TwoHeadGfxArena; // size = 0x10

void THA_GA_ct(TwoHeadGfxArena* thga, void* start, size_t size);
void THA_GA_dt(TwoHeadGfxArena* thga);
u32 THA_GA_isCrash(TwoHeadGfxArena* thga);
void THA_GA_init(TwoHeadGfxArena* thga);
s32 THA_GA_getFreeBytes(TwoHeadGfxArena* thga);
Gfx* THA_GA_getHeadPtr(TwoHeadGfxArena* thga);
void THA_GA_setHeadPtr(TwoHeadGfxArena* thga, Gfx* newHead);
void* THA_GA_getTailPtr(TwoHeadGfxArena* thga);
Gfx* THA_GA_nextPtrN(TwoHeadGfxArena* thga, size_t num);
Gfx* THA_GA_nextPtr1(TwoHeadGfxArena* thga);
Gfx* THA_GA_NEXT_DISP(TwoHeadGfxArena* thga);
void* THA_GA_alloc(TwoHeadGfxArena* thga, size_t size);
Mtx* THA_GA_allocMtxN(TwoHeadGfxArena* thga, size_t num);
Mtx* THA_GA_allocMtx1(TwoHeadGfxArena* thga);
Vtx* THA_GA_allocVtxN(TwoHeadGfxArena* thga, size_t num);
Vtx* THA_GA_allocVtx1(TwoHeadGfxArena* thga);

#endif
