#ifndef ZELDA_ARENA_H
#define ZELDA_ARENA_H

#include "ultra64.h"

void* zelda_malloc(u32 size);
void* zelda_malloc_r(u32 size);
void* zelda_realloc(void* ptr, u32 newSize);
void zelda_free(void* ptr);
void* zelda_calloc(u32 num, u32 size);
void zelda_GetFreeArena(u32* outMaxFree, u32* outFree, u32* outAlloc);
void zelda_CheckArena(void);
void zelda_InitArena(void* start, u32 size);
void zelda_CleanupArena(void);
s32 zelda_MallocIsInitalized(void);

#if DEBUG_FEATURES
void zelda_CheckArenaPointer(void* ptr, u32 size, const char* name, const char* action);
void* zelda_mallocDebug(u32 size, const char* file, int line);
void* zelda_malloc_rDebug(u32 size, const char* file, int line);
void* zelda_reallocDebug(void* ptr, u32 newSize, const char* file, int line);
void ZeldaArena_FreeDebug(void* ptr, const char* file, int line);
void ZeldaArena_Display(void);

#define ZELDA_ARENA_MALLOC(size, file, line) zelda_mallocDebug(size, file, line)
#define ZELDA_ARENA_MALLOC_R(size, file, line) zelda_malloc_rDebug(size, file, line)
#define ZELDA_ARENA_FREE(size, file, line) ZeldaArena_FreeDebug(size, file, line)

#else

#define ZELDA_ARENA_MALLOC(size, file, line) zelda_malloc(size)
#define ZELDA_ARENA_MALLOC_R(size, file, line) zelda_malloc_r(size)
#define ZELDA_ARENA_FREE(size, file, line) zelda_free(size)

#endif

#endif
