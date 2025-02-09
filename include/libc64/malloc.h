#ifndef LIBC64_MALLOC_H
#define LIBC64_MALLOC_H

#include "ultra64.h"
#include "libc64/os_malloc.h"

void* malloc(u32 size);
void* malloc_r(u32 size);
void* realloc(void* ptr, u32 newSize);
void free(void* ptr);
void* calloc(u32 num, u32 size);
void GetFreeArena(u32* outMaxFree, u32* outFree, u32* outAlloc);
void CheckArena(void);
void MallocInit(void* start, u32 size);
void MallocCleanup(void);
s32 MallocIsInitalized(void);

#if DEBUG_FEATURES
void* SystemArena_MallocDebug(u32 size, const char* file, int line);
void* SystemArena_MallocRDebug(u32 size, const char* file, int line);
void* SystemArena_ReallocDebug(void* ptr, u32 newSize, const char* file, int line);
void SystemArena_FreeDebug(void* ptr, const char* file, int line);
void SystemArena_Display(void);
#endif

extern Arena arena;

#endif
