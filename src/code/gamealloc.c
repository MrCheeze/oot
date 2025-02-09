#include "global.h"

void gamealloc_display(GameAlloc* this) {
    GameAllocEntry* iter;

    PRINTF("this = %08x\n", this);

    iter = this->base.next;
    while (iter != &this->base) {
        PRINTF("ptr = %08x size = %d\n", iter, iter->size);
        iter = iter->next;
    }
}

#if DEBUG_FEATURES
void* gamealloc_mallocDebug(GameAlloc* this, u32 size, const char* file, int line) {
    GameAllocEntry* ptr = SystemArena_MallocDebug(size + sizeof(GameAllocEntry), file, line);

    if (ptr != NULL) {
        ptr->size = size;
        ptr->prev = this->head;
        this->head->next = ptr;
        this->head = ptr;
        ptr->next = &this->base;
        this->base.prev = this->head;
        return ptr + 1;
    } else {
        return NULL;
    }
}
#endif

void* gamealloc_malloc(GameAlloc* this, u32 size) {
    GameAllocEntry* ptr = SYSTEM_ARENA_MALLOC(size + sizeof(GameAllocEntry), "../gamealloc.c", 93);

    if (ptr != NULL) {
        ptr->size = size;
        ptr->prev = this->head;
        this->head->next = ptr;
        this->head = ptr;
        ptr->next = &this->base;
        this->base.prev = this->head;
        return ptr + 1;
    } else {
        return NULL;
    }
}

void gamealloc_free(GameAlloc* this, void* data) {
    GameAllocEntry* ptr;

    if (data != NULL) {
        ptr = &((GameAllocEntry*)data)[-1];
        LOG_UTILS_CHECK_NULL_POINTER("ptr->prev", ptr->prev, "../gamealloc.c", 120);
        LOG_UTILS_CHECK_NULL_POINTER("ptr->next", ptr->next, "../gamealloc.c", 121);
        ptr->prev->next = ptr->next;
        ptr->next->prev = ptr->prev;
        this->head = this->base.prev;
        SYSTEM_ARENA_FREE(ptr, "../gamealloc.c", 125);
    }
}

void gamealloc_cleanup(GameAlloc* this) {
    GameAllocEntry* next = this->base.next;
    GameAllocEntry* cur;

    while (&this->base != next) {
        cur = next;
        next = next->next;
        SYSTEM_ARENA_FREE(cur, "../gamealloc.c", 145);
    }

    this->head = &this->base;
    this->base.next = &this->base;
    this->base.prev = &this->base;
}

void gamealloc_init(GameAlloc* this) {
    this->head = &this->base;
    this->base.next = &this->base;
    this->base.prev = &this->base;
}
