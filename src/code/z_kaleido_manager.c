#include "global.h"
#include "terminal.h"

#define KALEIDO_OVERLAY(name, nameString) \
    { NULL, ROM_FILE(ovl_##name), _ovl_##name##SegmentStart, _ovl_##name##SegmentEnd, 0, nameString, }

KaleidoMgrOverlay KaleidoArea_dlftbl[] = {
    KALEIDO_OVERLAY(kaleido_scope, "kaleido_scope"),
    KALEIDO_OVERLAY(player_actor, "player_actor"),
};

void* KaleidoArea_allocp = NULL;
KaleidoMgrOverlay* KaleidoArea_visit = NULL;
u8 LargeMapMark_MarkScaleMode = 0;

void KaleidoArea_DoLink(KaleidoMgrOverlay* ovl) {
    LOG_UTILS_CHECK_NULL_POINTER("KaleidoArea_allocp", KaleidoArea_allocp, "../z_kaleido_manager.c", 99);

    ovl->loadedRamAddr = KaleidoArea_allocp;
    LoadFragmentFix2(ovl->file.vromStart, ovl->file.vromEnd, ovl->vramStart, ovl->vramEnd, ovl->loadedRamAddr);

    PRINTF_COLOR_GREEN();
    PRINTF("OVL(k):Seg:%08x-%08x Ram:%08x-%08x Off:%08x %s\n", ovl->vramStart, ovl->vramEnd, ovl->loadedRamAddr,
           (uintptr_t)ovl->loadedRamAddr + (uintptr_t)ovl->vramEnd - (uintptr_t)ovl->vramStart,
           (uintptr_t)ovl->vramStart - (uintptr_t)ovl->loadedRamAddr, ovl->name);
    PRINTF_RST();

    ovl->offset = (uintptr_t)ovl->loadedRamAddr - (uintptr_t)ovl->vramStart;
    KaleidoArea_visit = ovl;
}

void KaleidoArea_DoUnlink(KaleidoMgrOverlay* ovl) {
    if (ovl->loadedRamAddr != NULL) {
        ovl->offset = 0;
        bzero(ovl->loadedRamAddr, (uintptr_t)ovl->vramEnd - (uintptr_t)ovl->vramStart);
        ovl->loadedRamAddr = NULL;
        KaleidoArea_visit = NULL;
    }
}

void KaleidoArea_init(PlayState* play) {
    s32 largestSize = 0;
    s32 size;
    u32 i;

    for (i = 0; i < ARRAY_COUNT(KaleidoArea_dlftbl); i++) {
        size = (uintptr_t)KaleidoArea_dlftbl[i].vramEnd - (uintptr_t)KaleidoArea_dlftbl[i].vramStart;
        if (size > largestSize) {
            largestSize = size;
        }
    }

    PRINTF_COLOR_GREEN();
    PRINTF(T("KaleidoArea の最大サイズは %d バイトを確保します\n", "The maximum size of KaleidoArea is %d bytes\n"),
           largestSize);
    PRINTF_RST();

    KaleidoArea_allocp = GAME_STATE_ALLOC(&play->state, largestSize, "../z_kaleido_manager.c", 150);
    LOG_UTILS_CHECK_NULL_POINTER("KaleidoArea_allocp", KaleidoArea_allocp, "../z_kaleido_manager.c", 151);

    PRINTF_COLOR_GREEN();
    PRINTF("KaleidoArea %08x - %08x\n", KaleidoArea_allocp, (uintptr_t)KaleidoArea_allocp + largestSize);
    PRINTF_RST();

    KaleidoArea_visit = NULL;
}

void KaleidoArea_cleanup(void) {
    if (KaleidoArea_visit != NULL) {
        KaleidoArea_DoUnlink(KaleidoArea_visit);
        KaleidoArea_visit = NULL;
    }

    KaleidoArea_allocp = NULL;
}

// NOTE: this function looks messed up and probably doesn't work how it was intended to
void* KaleidoArea_dllcnv(void* vram) {
    KaleidoMgrOverlay* iter = KaleidoArea_visit;
    KaleidoMgrOverlay* ovl = iter;

    if (ovl == NULL) {
        u32 i;

        iter = &KaleidoArea_dlftbl[0];
        for (i = 0; i < ARRAY_COUNT(KaleidoArea_dlftbl); i++) {
            if (((uintptr_t)vram >= (uintptr_t)iter->vramStart) && ((uintptr_t)iter->vramEnd >= (uintptr_t)vram)) {
                KaleidoArea_DoLink(iter);
                ovl = iter;
                goto KaleidoArea_dllcnv_end;
            }
            //! @bug Probably missing iter++ here
        }

        PRINTF(T("異常\n", "Abnormal\n"));
        return NULL;
    }

KaleidoArea_dllcnv_end:
    if ((ovl == NULL) || ((uintptr_t)vram < (uintptr_t)ovl->vramStart) ||
        ((uintptr_t)vram >= (uintptr_t)ovl->vramEnd)) {
        return NULL;
    }

    return (void*)((uintptr_t)vram + ovl->offset);
}
