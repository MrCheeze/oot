#ifndef Z64_DMA_H
#define Z64_DMA_H

#include "ultra64.h"
#include "alignment.h"
#include "romfile.h"

typedef struct DmaRequest {
    /* 0x00 */ uintptr_t    vromAddr; // VROM address (source)
    /* 0x04 */ void*        dramAddr; // DRAM address (destination)
    /* 0x08 */ size_t       size;     // File Transfer size
    /* 0x0C */ const char*  filename; // Filename for debugging
    /* 0x10 */ int          line;     // Line number for debugging
    /* 0x14 */ s32          unk_14;
    /* 0x18 */ OSMesgQueue* notifyQueue; // Message queue for the notification message
    /* 0x1C */ OSMesg       notifyMsg;   // Completion notification message
} DmaRequest; // size = 0x20

typedef struct DmaEntry {
    /* 0x00 */ RomFile file;
    /* 0x08 */ uintptr_t romStart;
    /* 0x0C */ uintptr_t romEnd;
} DmaEntry;

extern DmaEntry dma_rom_ad[];

extern u32 _dma_verbose;
extern size_t _dma_split_size;

#define DMAMGR_DEFAULT_BUFSIZE ALIGN16(0x2000)

// Standard DMA Requests

s32 dmacopy_bg(DmaRequest* req, void* ram, uintptr_t vrom, size_t size, u32 unk5, OSMesgQueue* queue,
                        OSMesg msg);
s32 dmacopy_fg(void* ram, uintptr_t vrom, size_t size);
#if DEBUG_FEATURES
s32 DmaMgr_RequestAsyncDebug(DmaRequest* req, void* ram, uintptr_t vrom, size_t size, u32 unk5, OSMesgQueue* queue,
                             OSMesg msg, const char* file, int line);
s32 DmaMgr_RequestSyncDebug(void* ram, uintptr_t vrom, size_t size, const char* file, int line);
#endif

// Special-purpose DMA Requests

s32 percial_DMA(uintptr_t rom, void* ram, size_t size);
void dmacopy_ddrom_fg(void* ram, uintptr_t rom, size_t size);
s32 dmaSoundRomHandler(OSPiHandle* pihandle, OSIoMesg* mb, s32 direction);

// Initialization

void CreateDmaManager(void);

#endif
