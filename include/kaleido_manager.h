#ifndef KALEIDO_MANAGER_H
#define KALEIDO_MANAGER_H

#include "ultra64.h"
#include "romfile.h"

struct PlayState;

typedef struct KaleidoMgrOverlay {
    /* 0x00 */ void* loadedRamAddr;
    /* 0x04 */ RomFile file;
    /* 0x0C */ void* vramStart;
    /* 0x10 */ void* vramEnd;
    /* 0x14 */ u32 offset; // loadedRamAddr - vramStart
    /* 0x18 */ const char* name;
} KaleidoMgrOverlay; // size = 0x1C

typedef enum KaleidoOverlayType {
    /* 0 */ KALEIDO_OVL_KALEIDO_SCOPE,
    /* 1 */ KALEIDO_OVL_PLAYER_ACTOR,
    /* 2 */ KALEIDO_OVL_MAX
} KaleidoOverlayType;

void KaleidoArea_DoLink(KaleidoMgrOverlay* ovl);
void KaleidoArea_DoUnlink(KaleidoMgrOverlay* ovl);
void KaleidoArea_init(struct PlayState* play);
void KaleidoArea_cleanup(void);
void* KaleidoArea_dllcnv(void* vram);

extern KaleidoMgrOverlay KaleidoArea_dlftbl[KALEIDO_OVL_MAX];
extern KaleidoMgrOverlay* KaleidoArea_visit;

void load_player(void);
void kaleido_scope_ct_ovl(struct PlayState* play);
void kaleido_scope_dt_ovl(struct PlayState* play);
void kaleido_scope_move_ovl(struct PlayState* play);
void kaleido_scope_draw_ovl(struct PlayState* play);

#endif
