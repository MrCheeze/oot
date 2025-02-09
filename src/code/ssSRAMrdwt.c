#include "ultra64.h"
#include "global.h"

typedef struct SsSramContext {
    /* 0x00 */ OSPiHandle piHandle;
    /* 0x74 */ OSIoMesg ioMesg;
    /* 0x8C */ OSMesgQueue msgQueue;
} SsSramContext; // size = 0xA4

SsSramContext domainInstance = { 0 };

void ssDMNInitial(s32 addr, u8 handleType, u8 handleDomain, u8 handleLatency, u8 handlePageSize, u8 handleRelDuration,
                 u8 handlePulse, u32 handleSpeed) {
    u32 prevInt;
    OSPiHandle* handle = &domainInstance.piHandle;

    if ((u32)OS_PHYSICAL_TO_K1(addr) != (*handle).baseAddress) {
        domainInstance.piHandle.type = handleType;
        (*handle).baseAddress = (u32)OS_PHYSICAL_TO_K1(addr);
        domainInstance.piHandle.latency = handleLatency;
        domainInstance.piHandle.pulse = handlePulse;
        domainInstance.piHandle.pageSize = handlePageSize;
        domainInstance.piHandle.relDuration = handleRelDuration;
        domainInstance.piHandle.domain = handleDomain;
        domainInstance.piHandle.speed = handleSpeed;

        bzero(&domainInstance.piHandle.transferInfo, sizeof(__OSTranxInfo));

        prevInt = __osDisableInt();
        domainInstance.piHandle.next = __osPiTable;
        __osPiTable = &domainInstance.piHandle;
        __osRestoreInt(prevInt);

        domainInstance.ioMesg.hdr.pri = OS_MESG_PRI_NORMAL;
        domainInstance.ioMesg.hdr.retQueue = &domainInstance.msgQueue;
        domainInstance.ioMesg.devAddr = addr;
    }
}

void ssDMNReadWrite(void* dramAddr, size_t size, s32 direction) {
    OSMesg msg;

    osCreateMesgQueue(&domainInstance.msgQueue, &msg, 1);
    domainInstance.ioMesg.dramAddr = dramAddr;
    domainInstance.ioMesg.size = size;
    osWritebackDCache(dramAddr, size);
    osEPiStartDma(&domainInstance.piHandle, &domainInstance.ioMesg, direction);
    osRecvMesg(&domainInstance.msgQueue, &msg, OS_MESG_BLOCK);
    osInvalDCache(dramAddr, size);
}

void ssSRAMReadWrite(s32 addr, void* dramAddr, size_t size, s32 direction) {
    PRINTF("ssSRAMReadWrite:%08x %08x %08x %d\n", addr, dramAddr, size, direction);
    ssDMNInitial(addr, DEVICE_TYPE_SRAM, PI_DOMAIN2, 5, 0xD, 2, 0xC, 0);
    ssDMNReadWrite(dramAddr, size, direction);
}
