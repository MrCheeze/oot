#include "global.h"
#include "terminal.h"

StackEntry* root = NULL;
StackEntry* last = NULL;

void stackcheck_init(StackEntry* entry, void* stackBottom, void* stackTop, u32 initValue, s32 minSpace,
                     const char* name) {
    StackEntry* iter;
    u32* addr;

    if (entry == NULL) {
        root = NULL;
    } else {
        entry->head = stackBottom;
        entry->tail = stackTop;
        entry->initValue = initValue;
        entry->minSpace = minSpace;
        entry->name = name;

#if !PLATFORM_N64
        iter = root;
        while (iter) {
            if (iter == entry) {
                PRINTF(VT_COL(RED, WHITE) T("stackcheck_init: %08x は既にリスト中にある\n",
                                            "stackcheck_init: %08x is already in the list\n") VT_RST,
                       entry);
                return;
            }
            iter = iter->next;
        }
#endif

        entry->prev = last;
        entry->next = NULL;

        if (last) {
            last->next = entry;
        }

        last = entry;
        if (!root) {
            root = entry;
        }

        if (entry->minSpace != -1) {
            addr = entry->head;
            while (addr < entry->tail) {
                *addr++ = entry->initValue;
            }
        }
    }
}

void stackcheck_cleanup(StackEntry* entry) {
#if PLATFORM_N64
    if (!entry->prev) {
        root = entry->next;
    } else {
        entry->prev->next = entry->next;
    }

    if (!entry->next) {
        last = entry->prev;
    }
#else
    u32 inconsistency = false;

    if (!entry->prev) {
        if (entry == root) {
            root = entry->next;
        } else {
            inconsistency = true;
        }
    } else {
        entry->prev->next = entry->next;
    }

    if (!entry->next) {
        if (entry == last) {
            last = entry->prev;
        } else {
            inconsistency = true;
        }
    }

    if (inconsistency) {
        PRINTF(VT_COL(RED, WHITE) T("stackcheck_cleanup: %08x リスト不整合です\n",
                                    "stackcheck_cleanup: %08x list inconsistent\n") VT_RST,
               entry);
    }
#endif
}

#if PLATFORM_N64

u32 stackcheck_check_stack(StackEntry* entry) {
    if (entry == NULL) {
        u32 ret = 0;
        StackEntry* iter = root;

        while (iter) {
            u32 state = stackcheck_check_stack(iter);

            if (state != STACK_STATUS_OK) {
                ret = 1;
            }
            iter = iter->next;
        }

        return ret;
    } else {
        u32* last;
        UNUSED_NDEBUG u32 used;
        u32 free;
        u32 ret;

        for (last = entry->head; last < entry->tail; last++) {
            if (entry->initValue != *last) {
                break;
            }
        }

        used = (uintptr_t)entry->tail - (uintptr_t)last;
        free = (uintptr_t)last - (uintptr_t)entry->head;

        if (free == 0) {
            ret = STACK_STATUS_OVERFLOW;
        } else if (free < (u32)entry->minSpace && entry->minSpace != -1) {
            ret = STACK_STATUS_WARNING;
        } else {
            ret = STACK_STATUS_OK;
        }

        osSyncPrintf("head=%08x tail=%08x last=%08x used=%08x free=%08x [%s]\n", entry->head, entry->tail, last, used,
                     free, entry->name != NULL ? entry->name : "(null)");

        return ret;
    }
}

#else

u32 stackcheck_check_stack1(StackEntry* entry) {
    u32* last;
    UNUSED_NDEBUG u32 used;
    u32 free;
    u32 ret;

    for (last = entry->head; last < entry->tail; last++) {
        if (entry->initValue != *last) {
            break;
        }
    }

    used = (uintptr_t)entry->tail - (uintptr_t)last;
    free = (uintptr_t)last - (uintptr_t)entry->head;

    if (free == 0) {
        ret = STACK_STATUS_OVERFLOW;
        PRINTF_COLOR_RED();
    } else if (free < (u32)entry->minSpace && entry->minSpace != -1) {
        ret = STACK_STATUS_WARNING;
        PRINTF_COLOR_YELLOW();
    } else {
        PRINTF_COLOR_GREEN();
        ret = STACK_STATUS_OK;
    }

#if !DEBUG_FEATURES
    // This string is still in .rodata for retail builds
    (void)"(null)";
#endif

    PRINTF("head=%08x tail=%08x last=%08x used=%08x free=%08x [%s]\n", entry->head, entry->tail, last, used, free,
           entry->name != NULL ? entry->name : "(null)");
    PRINTF_RST();

#if DEBUG_FEATURES
    if (ret != STACK_STATUS_OK) {
        LogUtils_LogHexDump(entry->head, (uintptr_t)entry->tail - (uintptr_t)entry->head);
    }
#endif

    return ret;
}

u32 stackcheck_check_stack0(void) {
    u32 ret = 0;
    StackEntry* iter = root;

    while (iter) {
        u32 state = stackcheck_check_stack1(iter);

        if (state != STACK_STATUS_OK) {
            ret = 1;
        }
        iter = iter->next;
    }

    return ret;
}

u32 stackcheck_check_stack(StackEntry* entry) {
    if (entry == NULL) {
        return stackcheck_check_stack0();
    } else {
        return stackcheck_check_stack1(entry);
    }
}

#endif
