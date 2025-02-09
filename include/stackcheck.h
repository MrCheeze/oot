#ifndef STACKCHECK_H
#define STACKCHECK_H

#include "ultra64.h"

typedef enum StackStatus {
    /* 0 */ STACK_STATUS_OK,
    /* 1 */ STACK_STATUS_WARNING,
    /* 2 */ STACK_STATUS_OVERFLOW
} StackStatus;

typedef struct StackEntry {
    /* 0x00 */ struct StackEntry* next;
    /* 0x04 */ struct StackEntry* prev;
    /* 0x08 */ u32* head;
    /* 0x0C */ u32* tail;
    /* 0x10 */ u32 initValue;
    /* 0x14 */ s32 minSpace;
    /* 0x18 */ const char* name;
} StackEntry;

void stackcheck_init(StackEntry* entry, void* stackBottom, void* stackTop, u32 initValue, s32 minSpace,
                     const char* name);
void stackcheck_cleanup(StackEntry* entry);
u32 stackcheck_check_stack(StackEntry* entry);

#endif
