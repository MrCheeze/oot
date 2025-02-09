#include "global.h"
#include "libc64/os_malloc.h"

#define LOG_SEVERITY_NOLOG 0
#define LOG_SEVERITY_ERROR 2
#define LOG_SEVERITY_VERBOSE 3

Arena zelda_arena;

#if DEBUG_FEATURES
s32 gZeldaArenaLogSeverity = LOG_SEVERITY_ERROR;

void zelda_CheckArenaPointer(void* ptr, u32 size, const char* name, const char* action) {
    if (ptr == NULL) {
        if (gZeldaArenaLogSeverity >= LOG_SEVERITY_ERROR) {
            PRINTF(T("%s: %u バイトの%sに失敗しました\n", "%s: %u bytes %s failed\n"), name, size, action);
#if PLATFORM_GC
            __osDisplayArena(&zelda_arena);
#endif
        }
    } else if (gZeldaArenaLogSeverity >= LOG_SEVERITY_VERBOSE) {
        PRINTF(T("%s: %u バイトの%sに成功しました\n", "%s: %u bytes %s succeeded\n"), name, size, action);
    }
}

#define ZELDA_ARENA_CHECK_POINTER(ptr, size, name, action) zelda_CheckArenaPointer(ptr, size, name, action)
#else
#define ZELDA_ARENA_CHECK_POINTER(ptr, size, name, action) (void)0
#endif

void* zelda_malloc(u32 size) {
    void* ptr = __osMalloc(&zelda_arena, size);

    // TODO re-evaluate "secure" as a translation (in this file and others using "確保")
    ZELDA_ARENA_CHECK_POINTER(ptr, size, "zelda_malloc", T("確保", "Secure"));
    return ptr;
}

#if DEBUG_FEATURES
void* zelda_mallocDebug(u32 size, const char* file, int line) {
    void* ptr = __osMallocDebug(&zelda_arena, size, file, line);

    ZELDA_ARENA_CHECK_POINTER(ptr, size, "zelda_malloc_DEBUG", T("確保", "Secure"));
    return ptr;
}
#endif

void* zelda_malloc_r(u32 size) {
    void* ptr = __osMallocR(&zelda_arena, size);

    ZELDA_ARENA_CHECK_POINTER(ptr, size, "zelda_malloc_r", T("確保", "Secure"));
    return ptr;
}

#if DEBUG_FEATURES
void* zelda_malloc_rDebug(u32 size, const char* file, int line) {
    void* ptr = __osMallocRDebug(&zelda_arena, size, file, line);

    ZELDA_ARENA_CHECK_POINTER(ptr, size, "zelda_malloc_r_DEBUG", T("確保", "Secure"));
    return ptr;
}
#endif

void* zelda_realloc(void* ptr, u32 newSize) {
    ptr = __osRealloc(&zelda_arena, ptr, newSize);
    ZELDA_ARENA_CHECK_POINTER(ptr, newSize, "zelda_realloc", T("再確保", "Re-securing"));
    return ptr;
}

#if DEBUG_FEATURES
void* zelda_reallocDebug(void* ptr, u32 newSize, const char* file, int line) {
    ptr = __osReallocDebug(&zelda_arena, ptr, newSize, file, line);
    ZELDA_ARENA_CHECK_POINTER(ptr, newSize, "zelda_realloc_DEBUG", T("再確保", "Re-securing"));
    return ptr;
}
#endif

void zelda_free(void* ptr) {
    __osFree(&zelda_arena, ptr);
}

#if DEBUG_FEATURES
void ZeldaArena_FreeDebug(void* ptr, const char* file, int line) {
    __osFreeDebug(&zelda_arena, ptr, file, line);
}
#endif

void* zelda_calloc(u32 num, u32 size) {
    void* ret;
    u32 n = num * size;

    ret = __osMalloc(&zelda_arena, n);
    if (ret != NULL) {
        bzero(ret, n);
    }

    ZELDA_ARENA_CHECK_POINTER(ret, n, "zelda_calloc", T("確保", "Secure"));
    return ret;
}

#if PLATFORM_GC && DEBUG_FEATURES
void ZeldaArena_Display(void) {
    PRINTF(T("ゼルダヒープ表示\n", "Zelda heap display\n"));
    __osDisplayArena(&zelda_arena);
}
#endif

void zelda_GetFreeArena(u32* outMaxFree, u32* outFree, u32* outAlloc) {
    __osGetFreeArena(&zelda_arena, outMaxFree, outFree, outAlloc);
}

void zelda_CheckArena(void) {
    __osCheckArena(&zelda_arena);
}

void zelda_InitArena(void* start, u32 size) {
#if DEBUG_FEATURES
    gZeldaArenaLogSeverity = LOG_SEVERITY_NOLOG;
#endif
    __osMallocInit(&zelda_arena, start, size);
}

void zelda_CleanupArena(void) {
#if DEBUG_FEATURES
    gZeldaArenaLogSeverity = LOG_SEVERITY_NOLOG;
#endif
    __osMallocCleanup(&zelda_arena);
}

s32 zelda_MallocIsInitalized(void) {
    return __osMallocIsInitalized(&zelda_arena);
}
