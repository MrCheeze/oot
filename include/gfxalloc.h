#ifndef GFXALLOC_H
#define GFXALLOC_H

#include "ultra64.h"

Gfx* gfxopen(Gfx* gfx);
Gfx* gfxclose(Gfx* gfx, Gfx* dst);
void* gfxalloc(Gfx** gfxP, u32 size);

#endif
