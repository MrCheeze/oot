#include "ultra64.h"
#include "global.h"
#include "versions.h"

#define MK_ASYNC_MSG(retData, tableType, id, loadStatus) \
    (((retData) << 24) | ((tableType) << 16) | ((id) << 8) | (loadStatus))
#define ASYNC_TBLTYPE(v) ((u8)(v >> 16))
#define ASYNC_ID(v) ((u8)(v >> 8))
#define ASYNC_LOAD_STATUS(v) ((u8)(v >> 0))

typedef enum SlowLoadState {
    /* 0 */ SLOW_LOAD_STATE_WAITING,
    /* 1 */ SLOW_LOAD_STATE_START,
    /* 2 */ SLOW_LOAD_STATE_LOADING,
    /* 3 */ SLOW_LOAD_STATE_DONE
} SlowLoadState;

typedef struct SampleBankRelocInfo {
    /* 0x00 */ s32 sampleBankId1;
    /* 0x04 */ s32 sampleBankId2;
    /* 0x08 */ s32 baseAddr1;
    /* 0x0C */ s32 baseAddr2;
    /* 0x10 */ u32 medium1;
    /* 0x14 */ u32 medium2;
} SampleBankRelocInfo; // size = 0x18

// opaque type for soundfont data loaded into ram (should maybe get rid of this?)
typedef void SoundFontData;

/* forward declarations */
s32 __Nas_StartSeq(s32 playerIdx, s32 seqId, s32 arg2);
SoundFontData* __Load_Ctrl(u32 fontId);
Sample* __GetWaveTable(s32 fontId, s32 instId);
void Nas_BgCopyMain(s32 resetStatus);
void __BgCopyDisk(AudioAsyncLoad* asyncLoad, s32 resetStatus);
void __BgCopySub(AudioAsyncLoad* asyncLoad, s32 resetStatus);
void Nas_BankOfsToAddr(s32 fontId, SoundFontData* fontData, SampleBankRelocInfo* sampleBankReloc,
                                             s32 isAsync);
void __WaveTouch(TunedSample* tunedSample, SoundFontData* fontData, SampleBankRelocInfo* sampleBankReloc);
void __Kill_Bank(s32 fontId);
u32 __Load_Wave(u32 sampleBankId, u32* outMedium, s32 noLoad);
void* __Load_Bank(u32 tableType, u32 id, s32* didAllocate);
u32 __Link_BankNum(s32 tableType, u32 id);
void* __Check_Cache(s32 tableType, s32 id);
AudioTable* __Get_ArcHeader(s32 tableType);
void Nas_FastCopy(u32 devAddr, u8* ramAddr, u32 size, s32 medium);
void Nas_FastDiskCopy(u32 devAddr, u8* addr, u32 size, s32 unkMediumParam);
s32 Nas_StartDma(OSIoMesg* mesg, u32 priority, s32 direction, u32 devAddr, void* ramAddr, u32 size,
                  OSMesgQueue* reqQueue, s32 medium, const char* dmaFuncType);
void* __Load_Bank_BG(s32 tableType, s32 id, s32 nChunks, s32 retData, OSMesgQueue* retQueue);
AudioAsyncLoad* Nas_BgCopyDisk(s32 unkMediumParam, u32 devAddr, void* ramAddr, s32 size, s32 medium,
                                                  s32 nChunks, OSMesgQueue* retQueue, s32 retMsg);
AudioAsyncLoad* Nas_BgCopyReq(u32 devAddr, void* ramAddr, u32 size, s32 medium, s32 nChunks,
                                         OSMesgQueue* retQueue, s32 retMsg);
void __Nas_BgCopy(AudioAsyncLoad* asyncLoad, u32 size);
void __Nas_BgDiskCopy(u32 devAddr, void* ramAddr, u32 size, s16 arg3);
u8* __Load_Seq(s32 seqId);
s32 Nas_CheckBgWave(s32 resetStatus);
void __Nas_SlowCopy(AudioSlowLoad* slowLoad, s32 size);
void LpsDma(s32 resetStatus);
void __Nas_SlowDiskCopy(s32 devAddr, u8* ramAddr, s32 size, s32 arg3);

OSMesgQueue MK_QUEUE;
OSMesg MK_QBUF[16];
s8* MK_RMES[0x10];
s32 ptr[2]; // file padding

s32 awheap;
s32 sAudioLoadPad2[4]; // double file padding?

DmaHandler NA_DMA_PROC = osEPiStartDma;
void* NA_DISK_PROC = NULL;

s32 AUDIO_SYSTEM_READY = false;

void Nas_WaveDmaFrameWork(void) {
    u32 i;

    for (i = 0; i < AG.sampleDmaListSize1; i++) {
        SampleDma* dma = &AG.sampleDmas[i];

        if (dma->ttl != 0) {
            dma->ttl--;
            if (dma->ttl == 0) {
                dma->reuseIndex = AG.sampleDmaReuseQueue1WrPos;
                AG.sampleDmaReuseQueue1[AG.sampleDmaReuseQueue1WrPos] = i;
                AG.sampleDmaReuseQueue1WrPos++;
            }
        }
    }

    for (i = AG.sampleDmaListSize1; i < AG.sampleDmaCount; i++) {
        SampleDma* dma = &AG.sampleDmas[i];

        if (dma->ttl != 0) {
            dma->ttl--;
            if (dma->ttl == 0) {
                dma->reuseIndex = AG.sampleDmaReuseQueue2WrPos;
                AG.sampleDmaReuseQueue2[AG.sampleDmaReuseQueue2WrPos] = i;
                AG.sampleDmaReuseQueue2WrPos++;
            }
        }
    }

    AG.unused2628 = 0;
}

void* Nas_WaveDmaCallBack(u32 devAddr, u32 size, s32 arg2, u8* dmaIndexRef, s32 medium) {
    s32 pad1;
    SampleDma* dma;
    s32 hasDma = false;
    u32 dmaDevAddr;
    u32 pad2;
    u32 dmaIndex;
    u32 transfer;
    s32 bufferPos;
    u32 i;

    if (arg2 != 0 || *dmaIndexRef >= AG.sampleDmaListSize1) {
        for (i = AG.sampleDmaListSize1; i < AG.sampleDmaCount; i++) {
            dma = &AG.sampleDmas[i];
            bufferPos = devAddr - dma->devAddr;
            if (0 <= bufferPos && (u32)bufferPos <= dma->size - size) {
                // We already have a DMA request for this memory range.
                if (dma->ttl == 0 && AG.sampleDmaReuseQueue2RdPos != AG.sampleDmaReuseQueue2WrPos) {
                    // Move the DMA out of the reuse queue, by swapping it with the
                    // read pos, and then incrementing the read pos.
                    if (dma->reuseIndex != AG.sampleDmaReuseQueue2RdPos) {
                        AG.sampleDmaReuseQueue2[dma->reuseIndex] =
                            AG.sampleDmaReuseQueue2[AG.sampleDmaReuseQueue2RdPos];
                        AG.sampleDmas[AG.sampleDmaReuseQueue2[AG.sampleDmaReuseQueue2RdPos]]
                            .reuseIndex = dma->reuseIndex;
                    }
                    AG.sampleDmaReuseQueue2RdPos++;
                }
                dma->ttl = 32;
                *dmaIndexRef = (u8)i;
                return &dma->ramAddr[devAddr - dma->devAddr];
            }
        }

        if (arg2 == 0) {
            goto search_short_lived;
        }

        if (AG.sampleDmaReuseQueue2RdPos != AG.sampleDmaReuseQueue2WrPos && arg2 != 0) {
            // Allocate a DMA from reuse queue 2, unless full.
            dmaIndex = AG.sampleDmaReuseQueue2[AG.sampleDmaReuseQueue2RdPos];
            AG.sampleDmaReuseQueue2RdPos++;
            dma = AG.sampleDmas + dmaIndex;
            hasDma = true;
        }
    } else {
    search_short_lived:
        dma = AG.sampleDmas + *dmaIndexRef;
        i = 0;
    again:
        bufferPos = devAddr - dma->devAddr;
        if (0 <= bufferPos && (u32)bufferPos <= dma->size - size) {
            // We already have DMA for this memory range.
            if (dma->ttl == 0) {
                // Move the DMA out of the reuse queue, by swapping it with the
                // read pos, and then incrementing the read pos.
                if (dma->reuseIndex != AG.sampleDmaReuseQueue1RdPos) {
                    AG.sampleDmaReuseQueue1[dma->reuseIndex] =
                        AG.sampleDmaReuseQueue1[AG.sampleDmaReuseQueue1RdPos];
                    AG.sampleDmas[AG.sampleDmaReuseQueue1[AG.sampleDmaReuseQueue1RdPos]]
                        .reuseIndex = dma->reuseIndex;
                }
                AG.sampleDmaReuseQueue1RdPos++;
            }
            dma->ttl = 2;
            return dma->ramAddr + (devAddr - dma->devAddr);
        }
        dma = AG.sampleDmas + i++;
        if (i <= AG.sampleDmaListSize1) {
            goto again;
        }
    }

    if (!hasDma) {
        if (AG.sampleDmaReuseQueue1RdPos == AG.sampleDmaReuseQueue1WrPos) {
            return NULL;
        }
        // Allocate a DMA from reuse queue 1.
        dmaIndex = AG.sampleDmaReuseQueue1[AG.sampleDmaReuseQueue1RdPos++];
        dma = AG.sampleDmas + dmaIndex;
        hasDma = true;
    }

    transfer = dma->size;
    dmaDevAddr = devAddr & ~0xF;
    dma->ttl = 3;
    dma->devAddr = dmaDevAddr;
    dma->sizeUnused = transfer;
    Nas_StartDma(&AG.curAudioFrameDmaIoMsgBuf[AG.curAudioFrameDmaCount++], OS_MESG_PRI_NORMAL, OS_READ,
                  dmaDevAddr, dma->ramAddr, transfer, &AG.curAudioFrameDmaQueue, medium, "SUPERDMA");
    *dmaIndexRef = dmaIndex;
    return (devAddr - dmaDevAddr) + dma->ramAddr;
}

void Nas_WaveDmaNew(s32 numNotes) {
    SampleDma* dma;
    s32 i;
    s32 t2;
    s32 j;

    AG.sampleDmaBufSize = AG.sampleDmaBufSize1;
    AG.sampleDmas = Nas_HeapAlloc(&AG.miscPool, 4 * AG.numNotes * sizeof(SampleDma) *
                                                                    AG.audioBufferParameters.specUnk4);
    t2 = 3 * AG.numNotes * AG.audioBufferParameters.specUnk4;
    for (i = 0; i < t2; i++) {
        dma = &AG.sampleDmas[AG.sampleDmaCount];
        dma->ramAddr = Nas_2ndHeapAlloc(&AG.miscPool, AG.sampleDmaBufSize);
        if (dma->ramAddr == NULL) {
            break;
        } else {
            Nas_CacheOff(dma->ramAddr, AG.sampleDmaBufSize);
            dma->size = AG.sampleDmaBufSize;
            dma->devAddr = 0;
            dma->sizeUnused = 0;
            dma->unused = 0;
            dma->ttl = 0;
            AG.sampleDmaCount++;
        }
    }

    for (i = 0; (u32)i < AG.sampleDmaCount; i++) {
        AG.sampleDmaReuseQueue1[i] = i;
        AG.sampleDmas[i].reuseIndex = i;
    }

    for (i = AG.sampleDmaCount; i < 0x100; i++) {
        AG.sampleDmaReuseQueue1[i] = 0;
    }

    AG.sampleDmaReuseQueue1RdPos = 0;
    AG.sampleDmaReuseQueue1WrPos = AG.sampleDmaCount;
    AG.sampleDmaListSize1 = AG.sampleDmaCount;
    AG.sampleDmaBufSize = AG.sampleDmaBufSize2;

    for (j = 0; j < AG.numNotes; j++) {
        dma = &AG.sampleDmas[AG.sampleDmaCount];
        dma->ramAddr = Nas_2ndHeapAlloc(&AG.miscPool, AG.sampleDmaBufSize);
        if (dma->ramAddr == NULL) {
            break;
        } else {
            Nas_CacheOff(dma->ramAddr, AG.sampleDmaBufSize);
            dma->size = AG.sampleDmaBufSize;
            dma->devAddr = 0U;
            dma->sizeUnused = 0;
            dma->unused = 0;
            dma->ttl = 0;
            AG.sampleDmaCount++;
        }
    }

    for (i = AG.sampleDmaListSize1; (u32)i < AG.sampleDmaCount; i++) {
        AG.sampleDmaReuseQueue2[i - AG.sampleDmaListSize1] = i;
        AG.sampleDmas[i].reuseIndex = i - AG.sampleDmaListSize1;
    }

    for (i = AG.sampleDmaCount; i < 0x100; i++) {
        AG.sampleDmaReuseQueue2[i] = AG.sampleDmaListSize1;
    }

    AG.sampleDmaReuseQueue2RdPos = 0;
    AG.sampleDmaReuseQueue2WrPos = AG.sampleDmaCount - AG.sampleDmaListSize1;
}

s32 Nas_CheckIDbank(s32 fontId) {
    if (fontId == 0xFF) {
        return true;
    } else if (AG.fontLoadStatus[fontId] >= LOAD_STATUS_COMPLETE) {
        return true;
    } else if (AG.fontLoadStatus[__Link_BankNum(FONT_TABLE, fontId)] >= LOAD_STATUS_COMPLETE) {
        return true;
    } else {
        return false;
    }
}

s32 Nas_CheckIDseq(s32 seqId) {
    if (seqId == 0xFF) {
        return true;
    } else if (AG.seqLoadStatus[seqId] >= LOAD_STATUS_COMPLETE) {
        return true;
    } else if (AG.seqLoadStatus[__Link_BankNum(SEQUENCE_TABLE, seqId)] >= LOAD_STATUS_COMPLETE) {
        return true;
    } else {
        return false;
    }
}

s32 Nas_CheckIDwave(s32 sampleBankId) {
    if (sampleBankId == 0xFF) {
        return true;
    } else if (AG.sampleFontLoadStatus[sampleBankId] >= LOAD_STATUS_COMPLETE) {
        return true;
    } else if (AG.sampleFontLoadStatus[__Link_BankNum(SAMPLE_TABLE, sampleBankId)] >=
               LOAD_STATUS_COMPLETE) {
        return true;
    } else {
        return false;
    }
}

void Nas_WriteIDbank(s32 fontId, s32 loadStatus) {
    if ((fontId != 0xFF) && (AG.fontLoadStatus[fontId] != LOAD_STATUS_PERMANENTLY_LOADED)) {
        AG.fontLoadStatus[fontId] = loadStatus;
    }
}

void Nas_WriteIDseq(s32 seqId, s32 loadStatus) {
    if ((seqId != 0xFF) && (AG.seqLoadStatus[seqId] != LOAD_STATUS_PERMANENTLY_LOADED)) {
        AG.seqLoadStatus[seqId] = loadStatus;
    }
}

void Nas_WriteIDwave(s32 sampleBankId, s32 loadStatus) {
    if (sampleBankId != 0xFF) {
        if (AG.sampleFontLoadStatus[sampleBankId] != LOAD_STATUS_PERMANENTLY_LOADED) {
            AG.sampleFontLoadStatus[sampleBankId] = loadStatus;
        }

        if ((AG.sampleFontLoadStatus[sampleBankId] == LOAD_STATUS_PERMANENTLY_LOADED) ||
            (AG.sampleFontLoadStatus[sampleBankId] == LOAD_STATUS_COMPLETE)) {
            EntryWave(sampleBankId);
        }
    }
}

void Nas_WriteIDwaveOnly(s32 sampleBankId, s32 loadStatus) {
    if ((sampleBankId != 0xFF) && (AG.sampleFontLoadStatus[sampleBankId] != LOAD_STATUS_PERMANENTLY_LOADED)) {
        AG.sampleFontLoadStatus[sampleBankId] = loadStatus;
    }
}

void Nas_BankHeaderInit(AudioTable* table, u32 romAddr, u16 unkMediumParam) {
    s32 i;

    table->header.unkMediumParam = unkMediumParam;
    table->header.romAddr = romAddr;

    for (i = 0; i < table->header.numEntries; i++) {
        if ((table->entries[i].size != 0) && (table->entries[i].medium == MEDIUM_CART)) {
            table->entries[i].romAddr += romAddr;
        }
    }
}

SoundFontData* Nas_PreLoadBank(s32 seqId, u32* outDefaultFontId) {
    s32 pad[2];
    s32 index;
    SoundFontData* fontData;
    s32 numFonts;
    s32 fontId;
    s32 i;

    if (seqId >= AG.numSequences) {
        return NULL;
    }

    fontId = 0xFF;
    index = ((u16*)AG.sequenceFontTable)[seqId];
    numFonts = AG.sequenceFontTable[index++];

    while (numFonts > 0) {
        fontId = AG.sequenceFontTable[index++];
        fontData = __Load_Ctrl(fontId);
        numFonts--;
    }

    *outDefaultFontId = fontId;
    return fontData;
}

void Nas_PreLoadSeq(s32 seqId, s32 arg1) {
    s32 pad;
    u32 defaultFontId;

    if (seqId < AG.numSequences) {
        if (arg1 & 2) {
            Nas_PreLoadBank(seqId, &defaultFontId);
        }
        if (arg1 & 1) {
            __Load_Seq(seqId);
        }
    }
}

s32 __Nas_LoadVoice_Inner(Sample* sample, s32 fontId) {
    void* sampleAddr;

    if (sample->isRelocated == true) {
        if (sample->medium != MEDIUM_RAM) {
            sampleAddr = Nas_Alloc_Single(sample->size, fontId, (void*)sample->sampleAddr, sample->medium,
                                                    CACHE_PERSISTENT);
            if (sampleAddr == NULL) {
                return -1;
            }

            if (sample->medium == MEDIUM_UNK) {
                Nas_FastDiskCopy((u32)sample->sampleAddr, sampleAddr, sample->size,
                                           AG.sampleBankTable->header.unkMediumParam);
            } else {
                Nas_FastCopy((u32)sample->sampleAddr, sampleAddr, sample->size, sample->medium);
            }
            sample->medium = MEDIUM_RAM;
            sample->sampleAddr = sampleAddr;
        }
    }
    //! @bug Missing return, but the return value is never used so it's fine.
}

s32 Nas_LoadVoice(s32 fontId, s32 instId, s32 drumId) {
    if (instId < 0x7F) {
        Instrument* instrument = ProgToVp(fontId, instId);

        if (instrument == NULL) {
            return -1;
        }
        if (instrument->normalRangeLo != 0) {
            __Nas_LoadVoice_Inner(instrument->lowPitchTunedSample.sample, fontId);
        }
        __Nas_LoadVoice_Inner(instrument->normalPitchTunedSample.sample, fontId);
        if (instrument->normalRangeHi != 0x7F) {
            return __Nas_LoadVoice_Inner(instrument->highPitchTunedSample.sample, fontId);
        }
        //! @bug Missing return, but the return value is never used so it's fine.
    } else if (instId == 0x7F) {
        Drum* drum = PercToPp(fontId, drumId);

        if (drum == NULL) {
            return -1;
        }
        __Nas_LoadVoice_Inner(drum->tunedSample.sample, fontId);
        return 0;
    }
}

void Nas_PreLoad_BG(s32 tableType, s32 id, s32 nChunks, s32 retData, OSMesgQueue* retQueue) {
    if (__Load_Bank_BG(tableType, id, nChunks, retData, retQueue) == NULL) {
        osSendMesg(retQueue, (OSMesg)0xFFFFFFFF, OS_MESG_NOBLOCK);
    }
}

void Nas_PreLoadSeq_BG(s32 seqId, s32 arg1, s32 retData, OSMesgQueue* retQueue) {
    Nas_PreLoad_BG(SEQUENCE_TABLE, seqId, 0, retData, retQueue);
}

void Nas_PreLoadWave_BG(s32 sampleBankId, s32 arg1, s32 retData, OSMesgQueue* retQueue) {
    Nas_PreLoad_BG(SAMPLE_TABLE, sampleBankId, 0, retData, retQueue);
}

void Nas_PreLoadBank_BG(s32 fontId, s32 arg1, s32 retData, OSMesgQueue* retQueue) {
    Nas_PreLoad_BG(FONT_TABLE, fontId, 0, retData, retQueue);
}

u8* Nas_SeqToBank(s32 seqId, u32* outNumFonts) {
    s32 index = ((u16*)AG.sequenceFontTable)[seqId];

    *outNumFonts = AG.sequenceFontTable[index++];
    if (*outNumFonts == 0) {
        return NULL;
    }
    return &AG.sequenceFontTable[index];
}

void Nas_FlushBank(s32 seqId) {
    s32 fontId;
    s32 index = ((u16*)AG.sequenceFontTable)[seqId];
    s32 numFonts = AG.sequenceFontTable[index++];

    while (numFonts > 0) {
        numFonts--;
        fontId = __Link_BankNum(FONT_TABLE, AG.sequenceFontTable[index++]);
        if (EmemOnCheck(FONT_TABLE, fontId) == NULL) {
            __Kill_Bank(fontId);
            Nas_WriteIDbank(fontId, LOAD_STATUS_NOT_LOADED);
        }
    }
}

void __Kill_Bank(s32 fontId) {
    u32 i;
    AudioCache* pool = &AG.fontCache;
    AudioPersistentCache* persistent;

    if (fontId == pool->temporary.entries[0].id) {
        pool->temporary.entries[0].id = -1;
    } else if (fontId == pool->temporary.entries[1].id) {
        pool->temporary.entries[1].id = -1;
    }

    persistent = &pool->persistent;
    for (i = 0; i < persistent->numEntries; i++) {
        if (fontId == persistent->entries[i].id) {
            persistent->entries[i].id = -1;
        }
    }

    Nas_ForceStopChannel(fontId);
}

s32 Nas_StartMySeq(s32 playerIdx, s32 seqId, s32 arg2) {
    if (AG.resetTimer != 0) {
        return 0;
    }

    AG.seqPlayers[playerIdx].skipTicks = 0;
    __Nas_StartSeq(playerIdx, seqId, arg2);
    //! @bug Missing return. Returning the result of the above function call
    //! matches but is UB because it too is missing a return, and using the
    //! result of a non-void function that has failed to return a value is UB.
    //! The callers of this function do not use the return value, so it's fine.
}

s32 Nas_StartSeq_Skip(s32 playerIdx, s32 seqId, s32 skipTicks) {
    if (AG.resetTimer != 0) {
        return 0;
    }

    AG.seqPlayers[playerIdx].skipTicks = skipTicks;
    __Nas_StartSeq(playerIdx, seqId, 0);
    //! @bug Missing return, see comment in Nas_StartMySeq above.
}

s32 __Nas_StartSeq(s32 playerIdx, s32 seqId, s32 arg2) {
    SequencePlayer* seqPlayer = &AG.seqPlayers[playerIdx];
    u8* seqData;
    s32 index;
    s32 numFonts;
    s32 fontId;

    if (seqId >= AG.numSequences) {
        return 0;
    }

    Nas_ReleaseGroup(seqPlayer);

    fontId = 0xFF;
    index = ((u16*)AG.sequenceFontTable)[seqId];
    numFonts = AG.sequenceFontTable[index++];

    while (numFonts > 0) {
        fontId = AG.sequenceFontTable[index++];
        __Load_Ctrl(fontId);
        numFonts--;
    }

    seqData = __Load_Seq(seqId);
    if (seqData == NULL) {
        return 0;
    }

    Nas_InitMySeq(seqPlayer);
    seqPlayer->seqId = seqId;
    seqPlayer->defaultFont = __Link_BankNum(FONT_TABLE, fontId);
    seqPlayer->seqData = seqData;
    seqPlayer->enabled = true;
    seqPlayer->scriptState.pc = seqData;
    seqPlayer->scriptState.depth = 0;
    seqPlayer->delay = 0;
    seqPlayer->finished = false;
    seqPlayer->playerIdx = playerIdx;
    Nas_SeqSkip(seqPlayer);
    //! @bug missing return (but the return value is not used so it's not UB)
}

u8* __Load_Seq(s32 seqId) {
    s32 pad;
    s32 didAllocate;

    if (AG.seqLoadStatus[__Link_BankNum(SEQUENCE_TABLE, seqId)] == LOAD_STATUS_IN_PROGRESS) {
        return NULL;
    }

    return __Load_Bank(SEQUENCE_TABLE, seqId, &didAllocate);
}

u32 __Load_Wave_Check(u32 sampleBankId, u32* outMedium) {
    return __Load_Wave(sampleBankId, outMedium, true);
}

u32 __Load_Wave(u32 sampleBankId, u32* outMedium, s32 noLoad) {
    void* ramAddr;
    AudioTable* sampleBankTable;
    u32 realTableId = __Link_BankNum(SAMPLE_TABLE, sampleBankId);
    s8 cachePolicy;

    sampleBankTable = __Get_ArcHeader(SAMPLE_TABLE);
    ramAddr = __Check_Cache(SAMPLE_TABLE, realTableId);
    if (ramAddr != NULL) {
        if (AG.sampleFontLoadStatus[realTableId] != LOAD_STATUS_IN_PROGRESS) {
            Nas_WriteIDwaveOnly(realTableId, LOAD_STATUS_COMPLETE);
        }
        *outMedium = MEDIUM_RAM;
        return (u32)ramAddr;
    }

    cachePolicy = sampleBankTable->entries[sampleBankId].cachePolicy;
    if (cachePolicy == 4 || noLoad == true) {
        *outMedium = sampleBankTable->entries[sampleBankId].medium;
        return sampleBankTable->entries[realTableId].romAddr;
    }

    ramAddr = __Load_Bank(SAMPLE_TABLE, sampleBankId, &noLoad);
    if (ramAddr != NULL) {
        *outMedium = MEDIUM_RAM;
        return (u32)ramAddr;
    }

    *outMedium = sampleBankTable->entries[sampleBankId].medium;
    return sampleBankTable->entries[realTableId].romAddr;
}

SoundFontData* __Load_Ctrl(u32 fontId) {
    SoundFontData* fontData;
    s32 sampleBankId1;
    s32 sampleBankId2;
    s32 didAllocate;
    SampleBankRelocInfo sampleBankReloc;
    s32 realFontId = __Link_BankNum(FONT_TABLE, fontId);

    if (AG.fontLoadStatus[realFontId] == LOAD_STATUS_IN_PROGRESS) {
        return NULL;
    }
    sampleBankId1 = AG.soundFontList[realFontId].sampleBankId1;
    sampleBankId2 = AG.soundFontList[realFontId].sampleBankId2;

    sampleBankReloc.sampleBankId1 = sampleBankId1;
    sampleBankReloc.sampleBankId2 = sampleBankId2;
    if (sampleBankId1 != 0xFF) {
        sampleBankReloc.baseAddr1 = __Load_Wave(sampleBankId1, &sampleBankReloc.medium1, false);
    } else {
        sampleBankReloc.baseAddr1 = 0;
    }

    if (sampleBankId2 != 0xFF) {
        sampleBankReloc.baseAddr2 = __Load_Wave(sampleBankId2, &sampleBankReloc.medium2, false);
    } else {
        sampleBankReloc.baseAddr2 = 0;
    }

    fontData = __Load_Bank(FONT_TABLE, fontId, &didAllocate);
    if (fontData == NULL) {
        return NULL;
    }
    if (didAllocate == true) {
        Nas_BankOfsToAddr(realFontId, fontData, &sampleBankReloc, false);
    }

    return fontData;
}

void* __Load_Bank(u32 tableType, u32 id, s32* didAllocate) {
    u32 size;
    AudioTable* table;
    s32 pad;
    u32 medium;
    s32 loadStatus;
    u32 romAddr;
    s32 cachePolicy;
    void* ramAddr;
    u32 realId;

    realId = __Link_BankNum(tableType, id);
    ramAddr = __Check_Cache(tableType, realId);
    if (ramAddr != NULL) {
        *didAllocate = false;
        loadStatus = LOAD_STATUS_COMPLETE;
    } else {
        table = __Get_ArcHeader(tableType);
        size = table->entries[realId].size;
        size = ALIGN16(size);
        medium = table->entries[id].medium;
        cachePolicy = table->entries[id].cachePolicy;
        romAddr = table->entries[realId].romAddr;
        switch (cachePolicy) {
            case 0:
                ramAddr = EmemAlloc(tableType, realId, size);
                if (ramAddr == NULL) {
                    return ramAddr;
                }
                break;

            case 1:
                ramAddr = Nas_SzHeapAlloc(tableType, size, CACHE_PERSISTENT, realId);
                if (ramAddr == NULL) {
                    return ramAddr;
                }
                break;

            case 2:
                ramAddr = Nas_SzHeapAlloc(tableType, size, CACHE_TEMPORARY, realId);
                if (ramAddr == NULL) {
                    return ramAddr;
                }
                break;

            case 3:
            case 4:
                ramAddr = Nas_SzHeapAlloc(tableType, size, CACHE_EITHER, realId);
                if (ramAddr == NULL) {
                    return ramAddr;
                }
                break;
        }

        *didAllocate = true;
        if (medium == MEDIUM_UNK) {
            Nas_FastDiskCopy(romAddr, ramAddr, size, (s16)table->header.unkMediumParam);
        } else {
            Nas_FastCopy(romAddr, ramAddr, size, medium);
        }

        loadStatus = (cachePolicy == 0) ? LOAD_STATUS_PERMANENTLY_LOADED : LOAD_STATUS_COMPLETE;
    }

    switch (tableType) {
        case SEQUENCE_TABLE:
            Nas_WriteIDseq(realId, loadStatus);
            break;

        case FONT_TABLE:
            Nas_WriteIDbank(realId, loadStatus);
            break;

        case SAMPLE_TABLE:
            Nas_WriteIDwave(realId, loadStatus);
            break;

        default:
            break;
    }

    return ramAddr;
}

u32 __Link_BankNum(s32 tableType, u32 id) {
    AudioTable* table = __Get_ArcHeader(tableType);

    if (table->entries[id].size == 0) {
        id = table->entries[id].romAddr;
    }

    return id;
}

void* __Check_Cache(s32 tableType, s32 id) {
    void* ramAddr;

    ramAddr = EmemOnCheck(tableType, id);
    if (ramAddr != NULL) {
        return ramAddr;
    }

    ramAddr = Nas_SzCacheCheck(tableType, CACHE_EITHER, id);
    if (ramAddr != NULL) {
        return ramAddr;
    }

    return NULL;
}

AudioTable* __Get_ArcHeader(s32 tableType) {
    AudioTable* table;

    switch (tableType) {
        case SEQUENCE_TABLE:
            table = AG.sequenceTable;
            break;

        case FONT_TABLE:
            table = AG.soundFontTable;
            break;

        default:
            table = NULL;
            break;

        case SAMPLE_TABLE:
            table = AG.sampleBankTable;
            break;
    }
    return table;
}

/**
 * Read and extract information from soundFont binary loaded into ram.
 * Also relocate offsets into pointers within this loaded soundFont
 *
 * @param fontId index of font being processed
 * @param fontDataStartAddr ram address of raw soundfont binary loaded into cache
 * @param sampleBankReloc information on the sampleBank containing raw audio samples
 */
void Nas_BankOfsToAddr_Inner(s32 fontId, SoundFontData* fontDataStartAddr, SampleBankRelocInfo* sampleBankReloc) {
    u32 soundOffset;     // Relative offset from the beginning of fontData directly to the tunedSample/envelope
    u32 soundListOffset; // Relative offset from the beginning of fontData to the list of soundOffsets/sfxs
    Instrument* inst;
    Drum* drum;
    SoundEffect* soundEffect;
    s32 i;
    s32 numDrums = AG.soundFontList[fontId].numDrums;
    s32 numInstruments = AG.soundFontList[fontId].numInstruments;
    s32 numSfx = AG.soundFontList[fontId].numSfx;
    u32* fontData = (u32*)fontDataStartAddr;

    // Relocate an offset (relative to the start of the font data) to a pointer (a ram address)
#define RELOC_TO_RAM(offset) ((u32)(offset) + (u32)(fontDataStartAddr))

    // Drums relocation

    // The first u32 in fontData is an offset to a list of offsets to the drums
    soundListOffset = fontData[0];

    // If the soundFont has drums
    if ((soundListOffset != 0) && (numDrums != 0)) {

        fontData[0] = RELOC_TO_RAM(soundListOffset);

        // Loop through the drum offsets
        for (i = 0; i < numDrums; i++) {
            // Get the i'th drum offset
            soundOffset = (u32)((Drum**)fontData[0])[i];

            // Some drum data entries are empty, represented by an offset of 0 in the list of drum offsets
            if (soundOffset == 0) {
                continue;
            }

            soundOffset = RELOC_TO_RAM(soundOffset);
            ((Drum**)fontData[0])[i] = drum = (Drum*)soundOffset;

            // The drum may be in the list multiple times and already relocated
            if (drum->isRelocated) {
                continue;
            }

            __WaveTouch(&drum->tunedSample, fontDataStartAddr, sampleBankReloc);

            soundOffset = (u32)drum->envelope;
            drum->envelope = (EnvelopePoint*)RELOC_TO_RAM(soundOffset);

            drum->isRelocated = true;
        }
    }

    // Sound effects relocation

    // The second u32 in fontData is an offset to the first sound effect entry
    soundListOffset = fontData[1];

    // If the soundFont has sound effects
    if ((soundListOffset != 0) && (numSfx != 0)) {

        fontData[1] = RELOC_TO_RAM(soundListOffset);

        // Loop through the sound effects
        for (i = 0; i < numSfx; i++) {
            // Get a pointer to the i'th sound effect
            soundOffset = (u32)(((SoundEffect*)fontData[1]) + i);
            soundEffect = (SoundEffect*)soundOffset;

            // Check for NULL (note: the pointer is guaranteed to be in fontData and can never be NULL)
            if ((soundEffect == NULL) || ((u32)soundEffect->tunedSample.sample == 0)) {
                continue;
            }

            __WaveTouch(&soundEffect->tunedSample, fontDataStartAddr, sampleBankReloc);
        }
    }

    // Instruments relocation

    // Instrument Id 126 and above is reserved.
    // There can only be 126 instruments, indexed from 0 to 125
    if (numInstruments > 126) {
        numInstruments = 126;
    }

    // Starting from the 3rd u32 in fontData is the list of offsets to the instruments
    // Loop through the instruments
    for (i = 2; i <= 2 + numInstruments - 1; i++) {
        // Some instrument data entries are empty, represented by an offset of 0 in the list of instrument offsets
        if (fontData[i] != 0) {
            fontData[i] = RELOC_TO_RAM(fontData[i]);
            inst = (Instrument*)fontData[i];

            // The instrument may be in the list multiple times and already relocated
            if (!inst->isRelocated) {
                // Some instruments have a different sample for low pitches
                if (inst->normalRangeLo != 0) {
                    __WaveTouch(&inst->lowPitchTunedSample, fontDataStartAddr, sampleBankReloc);
                }

                // Every instrument has a sample for the default range
                __WaveTouch(&inst->normalPitchTunedSample, fontDataStartAddr, sampleBankReloc);

                // Some instruments have a different sample for high pitches
                if (inst->normalRangeHi != 0x7F) {
                    __WaveTouch(&inst->highPitchTunedSample, fontDataStartAddr, sampleBankReloc);
                }

                soundOffset = (u32)inst->envelope;
                inst->envelope = (EnvelopePoint*)RELOC_TO_RAM(soundOffset);

                inst->isRelocated = true;
            }
        }
    }

#undef FONT_DATA_RELOC

    // Store the relocated pointers
    AG.soundFontList[fontId].drums = (Drum**)fontData[0];
    AG.soundFontList[fontId].soundEffects = (SoundEffect*)fontData[1];
    AG.soundFontList[fontId].instruments = (Instrument**)(fontData + 2);
}

void Nas_FastCopy(u32 devAddr, u8* ramAddr, u32 size, s32 medium) {
    OSMesgQueue* msgQueue = &AG.syncDmaQueue;
    OSIoMesg* ioMesg = &AG.syncDmaIoMesg;
    size = ALIGN16(size);

    osInvalDCache2(ramAddr, size);

    while (true) {
        if (size < 0x400) {
            break;
        }
        Nas_StartDma(ioMesg, OS_MESG_PRI_HIGH, OS_READ, devAddr, ramAddr, 0x400, msgQueue, medium, "FastCopy");
        osRecvMesg(msgQueue, NULL, OS_MESG_BLOCK);
        size -= 0x400;
        devAddr += 0x400;
        ramAddr += 0x400;
    }

    if (size != 0) {
        Nas_StartDma(ioMesg, OS_MESG_PRI_HIGH, OS_READ, devAddr, ramAddr, size, msgQueue, medium, "FastCopy");
        osRecvMesg(msgQueue, NULL, OS_MESG_BLOCK);
    }
}

void Nas_FastDiskCopy(u32 devAddr, u8* addr, u32 size, s32 unkMediumParam) {
}

s32 Nas_StartDma(OSIoMesg* mesg, u32 priority, s32 direction, u32 devAddr, void* ramAddr, u32 size,
                  OSMesgQueue* reqQueue, s32 medium, const char* dmaFuncType) {
    OSPiHandle* handle;

    if (AG.resetTimer > 16) {
        return -1;
    }

    switch (medium) {
        case MEDIUM_CART:
            handle = AG.cartHandle;
            break;

        case MEDIUM_DISK_DRIVE:
            // driveHandle is uninitialized and corresponds to stubbed-out disk drive support.
            // SM64 Shindou called osDriveRomInit here.
            handle = AG.driveHandle;
            break;

        default:
            return 0;
    }

    if ((size % 0x10) != 0) {
        size = ALIGN16(size);
    }

    mesg->hdr.pri = priority;
    mesg->hdr.retQueue = reqQueue;
    mesg->dramAddr = ramAddr;
    mesg->devAddr = devAddr;
    mesg->size = size;
    handle->transferInfo.cmdType = 2;
    NA_DMA_PROC(handle, mesg, direction);
    return 0;
}

void __OfsToLbaOfs(void) {
}

void EmemLoad(u32 tableType, u32 fontId) {
    s32 didAllocate;

    __Load_Bank(tableType, fontId, &didAllocate);
}

void* __Load_Bank_BG(s32 tableType, s32 id, s32 nChunks, s32 retData, OSMesgQueue* retQueue) {
    u32 size;
    AudioTable* table;
    void* ramAddr;
    s32 medium;
    s8 cachePolicy;
    u32 devAddr;
    s32 loadStatus;
    s32 pad;
    u32 realId = __Link_BankNum(tableType, id);

    switch (tableType) {
        case SEQUENCE_TABLE:
            if (AG.seqLoadStatus[realId] == LOAD_STATUS_IN_PROGRESS) {
                return NULL;
            }
            break;

        case FONT_TABLE:
            if (AG.fontLoadStatus[realId] == LOAD_STATUS_IN_PROGRESS) {
                return NULL;
            }
            break;

        case SAMPLE_TABLE:
            if (AG.sampleFontLoadStatus[realId] == LOAD_STATUS_IN_PROGRESS) {
                return NULL;
            }
            break;
    }

    ramAddr = __Check_Cache(tableType, realId);
    if (ramAddr != NULL) {
        loadStatus = LOAD_STATUS_COMPLETE;
        osSendMesg(retQueue, (OSMesg)MK_ASYNC_MSG(retData, 0, 0, LOAD_STATUS_NOT_LOADED), OS_MESG_NOBLOCK);
    } else {
        table = __Get_ArcHeader(tableType);
        size = table->entries[realId].size;
        size = ALIGN16(size);
        medium = table->entries[id].medium;
        cachePolicy = table->entries[id].cachePolicy;
        devAddr = table->entries[realId].romAddr;
        loadStatus = LOAD_STATUS_COMPLETE;

        switch (cachePolicy) {
            case 0:
                ramAddr = EmemAlloc(tableType, realId, size);
                if (ramAddr == NULL) {
                    return ramAddr;
                }
                loadStatus = LOAD_STATUS_PERMANENTLY_LOADED;
                break;

            case 1:
                ramAddr = Nas_SzHeapAlloc(tableType, size, CACHE_PERSISTENT, realId);
                if (ramAddr == NULL) {
                    return ramAddr;
                }
                break;

            case 2:
                ramAddr = Nas_SzHeapAlloc(tableType, size, CACHE_TEMPORARY, realId);
                if (ramAddr == NULL) {
                    return ramAddr;
                }
                break;

            case 3:
            case 4:
                ramAddr = Nas_SzHeapAlloc(tableType, size, CACHE_EITHER, realId);
                if (ramAddr == NULL) {
                    return ramAddr;
                }
                break;
        }

        if (medium == MEDIUM_UNK) {
            Nas_BgCopyDisk((s16)table->header.unkMediumParam, devAddr, ramAddr, size, medium,
                                              nChunks, retQueue, MK_ASYNC_MSG(retData, tableType, id, loadStatus));
        } else {
            Nas_BgCopyReq(devAddr, ramAddr, size, medium, nChunks, retQueue,
                                     MK_ASYNC_MSG(retData, tableType, realId, loadStatus));
        }
        loadStatus = LOAD_STATUS_IN_PROGRESS;
    }

    switch (tableType) {
        case SEQUENCE_TABLE:
            Nas_WriteIDseq(realId, loadStatus);
            break;

        case FONT_TABLE:
            Nas_WriteIDbank(realId, loadStatus);
            break;

        case SAMPLE_TABLE:
            Nas_WriteIDwave(realId, loadStatus);
            break;

        default:
            break;
    }

    return ramAddr;
}

void Nas_BgDmaFrameWork(s32 resetStatus) {
    LpsDma(resetStatus);
    Nas_CheckBgWave(resetStatus);
    Nas_BgCopyMain(resetStatus);
}

void Nas_SetRomHandler(DmaHandler callback) {
    NA_DMA_PROC = callback;
}

void Nas_SetDiskHandler(void* callback) {
    NA_DISK_PROC = callback;
}

void __SetVlute(s32 fontId) {
    SoundFont* font = &AG.soundFontList[fontId];
    AudioTableEntry* entry = &AG.soundFontTable->entries[fontId];

    font->sampleBankId1 = (entry->shortData1 >> 8) & 0xFF;
    font->sampleBankId2 = (entry->shortData1) & 0xFF;
    font->numInstruments = (entry->shortData2 >> 8) & 0xFF;
    font->numDrums = entry->shortData2 & 0xFF;
    font->numSfx = entry->shortData3;
}

void Nas_InitAudio(void* heap, u32 heapSize) {
    s32 pad[18];
    s32 numFonts;
    void* ramAddr;
    s32 i;

    NA_VFRAME_CALLBACK = NULL;
    AG.resetTimer = 0;

    {
        s32 i;
        u8* audioContextPtr = (u8*)&AG;

#ifndef AVOID_UB
        //! @bug This clearing loop sets one extra byte to 0 following AG.
        //! In practice this is harmless as it would set the most significant byte in NA_VFRAME_CALLBACK to 0,
        //! which was just reset to NULL above.
        for (i = sizeof(AG); i >= 0; i--) {
            *audioContextPtr++ = 0;
        }
#else
        // Avoid out-of-bounds variable access
        for (i = sizeof(AG); i > 0; i--) {
            *audioContextPtr++ = 0;
        }
#endif
    }

    // 1000 is a conversion from seconds to milliseconds
#if !OOT_PAL_N64
    switch (osTvType) {
        case OS_TV_PAL:
            AG.maxTempoTvTypeFactors = 1000 * REFRESH_RATE_DEVIATION_PAL / REFRESH_RATE_PAL;
            AG.refreshRate = REFRESH_RATE_PAL;
            break;

        case OS_TV_MPAL:
            AG.maxTempoTvTypeFactors = 1000 * REFRESH_RATE_DEVIATION_MPAL / REFRESH_RATE_MPAL;
            AG.refreshRate = REFRESH_RATE_MPAL;
            break;

        case OS_TV_NTSC:
        default:
            AG.maxTempoTvTypeFactors = 1000 * REFRESH_RATE_DEVIATION_NTSC / REFRESH_RATE_NTSC;
            AG.refreshRate = REFRESH_RATE_NTSC;
            break;
    }
#else
    switch (osTvType) {
        case OS_TV_PAL:
        default:
            AG.maxTempoTvTypeFactors = 1000 * REFRESH_RATE_DEVIATION_PAL / REFRESH_RATE_PAL;
            AG.refreshRate = REFRESH_RATE_PAL;
            break;
    }
#endif

    Nas_InitGAudio();

    for (i = 0; i < 3; i++) {
        AG.aiBufLengths[i] = 0xA0;
    }

    AG.totalTaskCount = 0;
    AG.rspTaskIndex = 0;
    AG.curAiBufIndex = 0;
    AG.soundMode = SOUNDMODE_STEREO;
    AG.curTask = NULL;
    AG.rspTask[0].task.t.data_size = 0;
    AG.rspTask[1].task.t.data_size = 0;
    osCreateMesgQueue(&AG.syncDmaQueue, &AG.syncDmaMesg, 1);
    osCreateMesgQueue(&AG.curAudioFrameDmaQueue, AG.curAudioFrameDmaMsgBuf,
                      ARRAY_COUNT(AG.curAudioFrameDmaMsgBuf));
    osCreateMesgQueue(&AG.externalLoadQueue, AG.externalLoadMsgBuf,
                      ARRAY_COUNT(AG.externalLoadMsgBuf));
    osCreateMesgQueue(&AG.preloadSampleQueue, AG.preloadSampleMsgBuf,
                      ARRAY_COUNT(AG.preloadSampleMsgBuf));
    AG.curAudioFrameDmaCount = 0;
    AG.sampleDmaCount = 0;
    AG.cartHandle = osCartRomInit();

    if (heap == NULL) {
        AG.audioHeap = AUDIOHP;
        AG.audioHeapSize = gAudioHeapInitSizes.heapSize;
    } else {
        AG.audioHeap = heap;
        AG.audioHeapSize = heapSize;
    }

    for (i = 0; i < (s32)AG.audioHeapSize / 8; i++) {
        ((u64*)AG.audioHeap)[i] = 0;
    }

    // Main Pool Split (split entirety of audio heap into initPool and sessionPool)
    Nas_SzHeapReset(gAudioHeapInitSizes.initPoolSize);

    // Initialize the audio interface buffers
    for (i = 0; i < ARRAY_COUNT(AG.aiBuffers); i++) {
        AG.aiBuffers[i] = Nas_HeapAlloc_CL(&AG.initPool, AIBUF_SIZE);
    }

    // Set audio tables pointers
    AG.sequenceTable = (AudioTable*)AudioseqHeaderStart;
    AG.soundFontTable = &AudiobankHeaderStart;
    AG.sampleBankTable = &AudiowaveHeaderStart;
    AG.sequenceFontTable = AudiomapHeaderStart;

    AG.numSequences = AG.sequenceTable->header.numEntries;

    AG.specId = 0;
    AG.resetStatus = 1; // Set reset to immediately initialize the audio heap

    Nas_SpecChange();

    // Initialize audio tables
    Nas_BankHeaderInit(AG.sequenceTable, (u32)_AudioseqSegmentRomStart, 0);
    Nas_BankHeaderInit(AG.soundFontTable, (u32)_AudiobankSegmentRomStart, 0);
    Nas_BankHeaderInit(AG.sampleBankTable, (u32)_AudiotableSegmentRomStart, 0);
    numFonts = AG.soundFontTable->header.numEntries;
    AG.soundFontList = Nas_HeapAlloc(&AG.initPool, numFonts * sizeof(SoundFont));

    for (i = 0; i < numFonts; i++) {
        __SetVlute(i);
    }

    ramAddr = Nas_HeapAlloc(&AG.initPool, gAudioHeapInitSizes.permanentPoolSize);
    if (ramAddr == NULL) {
        gAudioHeapInitSizes.permanentPoolSize = 0;
    }

    Nas_HeapInit(&AG.permanentPool, ramAddr, gAudioHeapInitSizes.permanentPoolSize);
    AUDIO_SYSTEM_READY = true;
    osSendMesg(AG.taskStartQueueP, (OSMesg)AG.totalTaskCount, OS_MESG_NOBLOCK);
}

void LpsInit(void) {
    AG.slowLoads[0].state = SLOW_LOAD_STATE_WAITING;
    AG.slowLoads[1].state = SLOW_LOAD_STATE_WAITING;
}

s32 VoiceLoad(s32 fontId, s32 instId, s8* status) {
    Sample* sample;
    AudioSlowLoad* slowLoad;

    sample = __GetWaveTable(fontId, instId);
    if (sample == NULL) {
        *status = 0;
        return -1;
    }

    if (sample->medium == MEDIUM_RAM) {
        *status = 2;
        return 0;
    }

    slowLoad = &AG.slowLoads[AG.slowLoadPos];
    if (slowLoad->state == SLOW_LOAD_STATE_DONE) {
        slowLoad->state = SLOW_LOAD_STATE_WAITING;
    }

    slowLoad->sample = *sample;
    slowLoad->status = status;
    slowLoad->curRamAddr =
        Nas_Alloc_Single(sample->size, fontId, sample->sampleAddr, sample->medium, CACHE_TEMPORARY);

    if (slowLoad->curRamAddr == NULL) {
        if (sample->medium == MEDIUM_UNK || sample->codec == CODEC_S16_INMEMORY) {
            *status = 0;
            return -1;
        } else {
            *status = 3;
            return -1;
        }
    }

    slowLoad->state = SLOW_LOAD_STATE_START;
    slowLoad->bytesRemaining = ALIGN16(sample->size);
    slowLoad->ramAddr = slowLoad->curRamAddr;
    slowLoad->curDevAddr = (u32)sample->sampleAddr;
    slowLoad->medium = sample->medium;
    slowLoad->seqOrFontId = fontId;
    slowLoad->instId = instId;
    if (slowLoad->medium == MEDIUM_UNK) {
        slowLoad->unkMediumParam = AG.sampleBankTable->header.unkMediumParam;
    }

    AG.slowLoadPos ^= 1;
    return 0;
}

Sample* __GetWaveTable(s32 fontId, s32 instId) {
    Sample* sample;

    if (instId < 0x80) {
        Instrument* instrument = ProgToVp(fontId, instId);

        if (instrument == NULL) {
            return NULL;
        }
        sample = instrument->normalPitchTunedSample.sample;
    } else if (instId < 0x100) {
        Drum* drum = PercToPp(fontId, instId - 0x80);

        if (drum == NULL) {
            return NULL;
        }
        sample = drum->tunedSample.sample;
    } else {
        SoundEffect* soundEffect = VpercToVep(fontId, instId - 0x100);

        if (soundEffect == NULL) {
            return NULL;
        }
        sample = soundEffect->tunedSample.sample;
    }
    return sample;
}

void __WriteBackLps(void) {
}

void __SwapLoadLps(AudioSlowLoad* slowLoad) {
    Sample* sample;

    if (slowLoad->sample.sampleAddr == NULL) {
        return;
    }

    sample = __GetWaveTable(slowLoad->seqOrFontId, slowLoad->instId);
    if (sample == NULL) {
        return;
    }

    slowLoad->sample = *sample;
    sample->sampleAddr = slowLoad->ramAddr;
    sample->medium = MEDIUM_RAM;
}

void LpsDma(s32 resetStatus) {
    AudioSlowLoad* slowLoad;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(AG.slowLoads); i++) {
        slowLoad = &AG.slowLoads[i];
        switch (AG.slowLoads[i].state) {
            case SLOW_LOAD_STATE_LOADING:
                if (slowLoad->medium != MEDIUM_UNK) {
                    osRecvMesg(&slowLoad->msgQueue, NULL, OS_MESG_BLOCK);
                }

                if (resetStatus != 0) {
                    slowLoad->state = SLOW_LOAD_STATE_DONE;
                    continue;
                }
                FALLTHROUGH;
            case SLOW_LOAD_STATE_START:
                slowLoad->state = SLOW_LOAD_STATE_LOADING;
                if (slowLoad->bytesRemaining == 0) {
                    __SwapLoadLps(slowLoad);
                    slowLoad->state = SLOW_LOAD_STATE_DONE;
                    *slowLoad->status = 1;
                } else if (slowLoad->bytesRemaining < 0x400) {
                    if (slowLoad->medium == MEDIUM_UNK) {
                        u32 size = slowLoad->bytesRemaining;

                        __Nas_SlowDiskCopy(slowLoad->curDevAddr, slowLoad->curRamAddr, size,
                                                       slowLoad->unkMediumParam);
                    } else {
                        __Nas_SlowCopy(slowLoad, slowLoad->bytesRemaining);
                    }
                    slowLoad->bytesRemaining = 0;
                } else {
                    if (slowLoad->medium == MEDIUM_UNK) {
                        __Nas_SlowDiskCopy(slowLoad->curDevAddr, slowLoad->curRamAddr, 0x400,
                                                       slowLoad->unkMediumParam);
                    } else {
                        __Nas_SlowCopy(slowLoad, 0x400);
                    }
                    slowLoad->bytesRemaining -= 0x400;
                    slowLoad->curRamAddr += 0x400;
                    slowLoad->curDevAddr += 0x400;
                }
                break;
        }
    }
}

void __Nas_SlowCopy(AudioSlowLoad* slowLoad, s32 size) {
    osInvalDCache2(slowLoad->curRamAddr, size);
    osCreateMesgQueue(&slowLoad->msgQueue, &slowLoad->msg, 1);
    Nas_StartDma(&slowLoad->ioMesg, OS_MESG_PRI_NORMAL, OS_READ, slowLoad->curDevAddr, slowLoad->curRamAddr, size,
                  &slowLoad->msgQueue, slowLoad->medium, "SLOWCOPY");
}

void __Nas_SlowDiskCopy(s32 devAddr, u8* ramAddr, s32 size, s32 arg3) {
}

s32 SeqLoad(s32 seqId, u8* ramAddr, s8* status) {
    AudioSlowLoad* slowLoad;
    AudioTable* seqTable;
    u32 size;

    if (seqId >= AG.numSequences) {
        *status = 0;
        return -1;
    }

    seqId = __Link_BankNum(SEQUENCE_TABLE, seqId);
    seqTable = __Get_ArcHeader(SEQUENCE_TABLE);
    slowLoad = &AG.slowLoads[AG.slowLoadPos];
    if (slowLoad->state == SLOW_LOAD_STATE_DONE) {
        slowLoad->state = SLOW_LOAD_STATE_WAITING;
    }

    slowLoad->sample.sampleAddr = NULL;
    slowLoad->status = status;
    size = seqTable->entries[seqId].size;
    size = ALIGN16(size);
    slowLoad->curRamAddr = ramAddr;
    slowLoad->state = SLOW_LOAD_STATE_START;
    slowLoad->bytesRemaining = size;
    slowLoad->ramAddr = ramAddr;
    slowLoad->curDevAddr = seqTable->entries[seqId].romAddr;
    slowLoad->medium = seqTable->entries[seqId].medium;
    slowLoad->seqOrFontId = seqId;

    if (slowLoad->medium == MEDIUM_UNK) {
        slowLoad->unkMediumParam = seqTable->header.unkMediumParam;
    }

    AG.slowLoadPos ^= 1;
    return 0;
}

void Nas_BgCopyInit(void) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(AG.asyncLoads); i++) {
        AG.asyncLoads[i].status = 0;
    }
}

AudioAsyncLoad* Nas_BgCopyDisk(s32 unkMediumParam, u32 devAddr, void* ramAddr, s32 size, s32 medium,
                                                  s32 nChunks, OSMesgQueue* retQueue, s32 retMsg) {
    AudioAsyncLoad* asyncLoad;

    asyncLoad = Nas_BgCopyReq(devAddr, ramAddr, size, medium, nChunks, retQueue, retMsg);

    if (asyncLoad == NULL) {
        return NULL;
    }

    osSendMesg(&AG.asyncLoadUnkMediumQueue, (OSMesg)asyncLoad, OS_MESG_NOBLOCK);
    asyncLoad->unkMediumParam = unkMediumParam;
    return asyncLoad;
}

AudioAsyncLoad* Nas_BgCopyReq(u32 devAddr, void* ramAddr, u32 size, s32 medium, s32 nChunks,
                                         OSMesgQueue* retQueue, s32 retMsg) {
    AudioAsyncLoad* asyncLoad;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(AG.asyncLoads); i++) {
        if (AG.asyncLoads[i].status == 0) {
            asyncLoad = &AG.asyncLoads[i];
            break;
        }
    }

    // no more available async loads
    if (i == ARRAY_COUNT(AG.asyncLoads)) {
        return NULL;
    }

    asyncLoad->status = 1;
    asyncLoad->curDevAddr = devAddr;
    asyncLoad->ramAddr = ramAddr;
    asyncLoad->curRamAddr = ramAddr;
    asyncLoad->bytesRemaining = size;

    if (nChunks == 0) {
        asyncLoad->chunkSize = 0x1000;
    } else if (nChunks == 1) {
        asyncLoad->chunkSize = size;
    } else {
        asyncLoad->chunkSize = ALIGN256((s32)size / nChunks);
        if (asyncLoad->chunkSize < 0x100) {
            asyncLoad->chunkSize = 0x100;
        }
    }

    asyncLoad->retQueue = retQueue;
    asyncLoad->delay = 3;
    asyncLoad->medium = medium;
    asyncLoad->retMsg = retMsg;
    osCreateMesgQueue(&asyncLoad->msgQueue, &asyncLoad->msg, 1);
    return asyncLoad;
}

void Nas_BgCopyMain(s32 resetStatus) {
    AudioAsyncLoad* asyncLoad;
    s32 i;

    if (AG.resetTimer == 1) {
        return;
    }

    if (AG.curUnkMediumLoad == NULL) {
        if (resetStatus != 0) {
            // Clear and ignore queue if resetting.
            do {
            } while (osRecvMesg(&AG.asyncLoadUnkMediumQueue, (OSMesg*)&asyncLoad, OS_MESG_NOBLOCK) != -1);
        } else if (osRecvMesg(&AG.asyncLoadUnkMediumQueue, (OSMesg*)&asyncLoad, OS_MESG_NOBLOCK) == -1) {
            AG.curUnkMediumLoad = NULL;
        } else {
            AG.curUnkMediumLoad = asyncLoad;
        }
    }

    if (AG.curUnkMediumLoad != NULL) {
        __BgCopyDisk(AG.curUnkMediumLoad, resetStatus);
    }

    for (i = 0; i < ARRAY_COUNT(AG.asyncLoads); i++) {
        if (AG.asyncLoads[i].status == 1) {
            asyncLoad = &AG.asyncLoads[i];
            if (asyncLoad->medium != MEDIUM_UNK) {
                __BgCopySub(asyncLoad, resetStatus);
            }
        }
    }
}

void __BgCopyDisk(AudioAsyncLoad* asyncLoad, s32 resetStatus) {
}

void __BgCopyFinishProcess(AudioAsyncLoad* asyncLoad) {
    u32 retMsg = asyncLoad->retMsg;
    u32 fontId;
    u32 pad;
    OSMesg doneMsg;
    u32 sampleBankId1;
    u32 sampleBankId2;
    SampleBankRelocInfo sampleBankReloc;

    if (1) {}
    switch (ASYNC_TBLTYPE(retMsg)) {
        case SEQUENCE_TABLE:
            Nas_WriteIDseq(ASYNC_ID(retMsg), ASYNC_LOAD_STATUS(retMsg));
            break;

        case SAMPLE_TABLE:
            Nas_WriteIDwave(ASYNC_ID(retMsg), ASYNC_LOAD_STATUS(retMsg));
            break;

        case FONT_TABLE:
            fontId = ASYNC_ID(retMsg);
            sampleBankId1 = AG.soundFontList[fontId].sampleBankId1;
            sampleBankId2 = AG.soundFontList[fontId].sampleBankId2;
            sampleBankReloc.sampleBankId1 = sampleBankId1;
            sampleBankReloc.sampleBankId2 = sampleBankId2;
            sampleBankReloc.baseAddr1 =
                sampleBankId1 != 0xFF ? __Load_Wave_Check(sampleBankId1, &sampleBankReloc.medium1) : 0;
            sampleBankReloc.baseAddr2 =
                sampleBankId2 != 0xFF ? __Load_Wave_Check(sampleBankId2, &sampleBankReloc.medium2) : 0;
            Nas_WriteIDbank(fontId, ASYNC_LOAD_STATUS(retMsg));
            Nas_BankOfsToAddr(fontId, asyncLoad->ramAddr, &sampleBankReloc, true);
            break;
    }

    doneMsg = (OSMesg)asyncLoad->retMsg;
    if (1) {}
    asyncLoad->status = 0;
    osSendMesg(asyncLoad->retQueue, doneMsg, OS_MESG_NOBLOCK);
}

void __BgCopySub(AudioAsyncLoad* asyncLoad, s32 resetStatus) {
    AudioTable* sampleBankTable = AG.sampleBankTable;

    if (asyncLoad->delay >= 2) {
        asyncLoad->delay--;
        return;
    }

    if (asyncLoad->delay == 1) {
        asyncLoad->delay = 0;
    } else if (resetStatus != 0) {
        // Await the previous DMA response synchronously, then return.
        osRecvMesg(&asyncLoad->msgQueue, NULL, OS_MESG_BLOCK);
        asyncLoad->status = 0;
        return;
    } else if (osRecvMesg(&asyncLoad->msgQueue, NULL, OS_MESG_NOBLOCK) == -1) {
        // If the previous DMA step isn't done, return.
        return;
    }

    if (asyncLoad->bytesRemaining == 0) {
        __BgCopyFinishProcess(asyncLoad);
        return;
    }

    if (asyncLoad->bytesRemaining < asyncLoad->chunkSize) {
        if (asyncLoad->medium == MEDIUM_UNK) {
            __Nas_BgDiskCopy(asyncLoad->curDevAddr, asyncLoad->curRamAddr, asyncLoad->bytesRemaining,
                                        sampleBankTable->header.unkMediumParam);
        } else {
            __Nas_BgCopy(asyncLoad, asyncLoad->bytesRemaining);
        }
        asyncLoad->bytesRemaining = 0;
        return;
    }

    if (asyncLoad->medium == MEDIUM_UNK) {
        __Nas_BgDiskCopy(asyncLoad->curDevAddr, asyncLoad->curRamAddr, asyncLoad->chunkSize,
                                    sampleBankTable->header.unkMediumParam);
    } else {
        __Nas_BgCopy(asyncLoad, asyncLoad->chunkSize);
    }

    asyncLoad->bytesRemaining -= asyncLoad->chunkSize;
    asyncLoad->curDevAddr += asyncLoad->chunkSize;
    asyncLoad->curRamAddr += asyncLoad->chunkSize;
}

void __Nas_BgCopy(AudioAsyncLoad* asyncLoad, u32 size) {
    size = ALIGN16(size);
    osInvalDCache2(asyncLoad->curRamAddr, size);
    osCreateMesgQueue(&asyncLoad->msgQueue, &asyncLoad->msg, 1);
    Nas_StartDma(&asyncLoad->ioMesg, OS_MESG_PRI_NORMAL, OS_READ, asyncLoad->curDevAddr, asyncLoad->curRamAddr, size,
                  &asyncLoad->msgQueue, asyncLoad->medium, "BGCOPY");
}

void __Nas_BgDiskCopy(u32 devAddr, void* ramAddr, u32 size, s16 arg3) {
}

/**
 * Read and extract information from TunedSample and its Sample
 * contained in the soundFont binary loaded into ram
 * TunedSample contains metadata on a sample used by a particular instrument/drum/sfx
 * Also relocate offsets into pointers within this loaded TunedSample
 *
 * @param fontId index of font being processed
 * @param fontData ram address of raw soundfont binary loaded into cache
 * @param sampleBankReloc information on the sampleBank containing raw audio samples
 */
void __WaveTouch(TunedSample* tunedSample, SoundFontData* fontData, SampleBankRelocInfo* sampleBankReloc) {
    Sample* sample;
    void* reloc;

    // Relocate an offset (relative to data loaded in ram at `base`) to a pointer (a ram address)
#define AUDIO_RELOC(offset, base) (reloc = (void*)((u32)(offset) + (u32)(base)))

    // If this has not already been relocated
    if ((u32)tunedSample->sample <= AUDIO_RELOCATED_ADDRESS_START) {

        sample = tunedSample->sample = AUDIO_RELOC(tunedSample->sample, fontData);

        // If the sample exists and has not already been relocated
        // Note: this is important, as the same sample can be used by different drums, sound effects, instruments
        if ((sample->size != 0) && (sample->isRelocated != true)) {
            sample->loop = AUDIO_RELOC(sample->loop, fontData);
            sample->book = AUDIO_RELOC(sample->book, fontData);

            // Resolve the sample medium 2-bit bitfield into a real value based on sampleBankReloc.
            // Then relocate the offset sample within the sampleBank (not the fontData) into absolute address.
            // sampleAddr can be either rom or ram depending on sampleBank cache policy
            // in practice, this is always in rom
            switch (sample->medium) {
                case 0:
                    sample->sampleAddr = AUDIO_RELOC(sample->sampleAddr, sampleBankReloc->baseAddr1);
                    sample->medium = sampleBankReloc->medium1;
                    break;

                case 1:
                    sample->sampleAddr = AUDIO_RELOC(sample->sampleAddr, sampleBankReloc->baseAddr2);
                    sample->medium = sampleBankReloc->medium2;
                    break;

                case 2:
                case 3:
                    // Invalid? This leaves sample->medium as MEDIUM_CART and MEDIUM_DISK_DRIVE
                    // respectively, and the sampleAddr unrelocated.
                    break;
            }

            sample->isRelocated = true;

            if (sample->unk_bit26 && (sample->medium != MEDIUM_RAM)) {
                AG.usedSamples[AG.numUsedSamples++] = sample;
            }
        }
    }

#undef AUDIO_RELOC
}

/**
 * @param fontId index of font being processed
 * @param fontData ram address of raw soundfont binary loaded into cache
 * @param sampleBankReloc information on the sampleBank containing raw audio samples
 * @param isAsync bool for whether this is an asynchronous load or not
 */
void Nas_BankOfsToAddr(s32 fontId, SoundFontData* fontData, SampleBankRelocInfo* sampleBankReloc,
                                             s32 isAsync) {
    AudioPreloadReq* preload;
    AudioPreloadReq* topPreload;
    Sample* sample;
    s32 size;
    s32 nChunks;
    u8* sampleRamAddr;
    s32 preloadInProgress;
    s32 i;

    preloadInProgress = false;
    if (AG.preloadSampleStackTop != 0) {
        preloadInProgress = true;
    } else {
        awheap = 0;
    }

    AG.numUsedSamples = 0;
    Nas_BankOfsToAddr_Inner(fontId, fontData, sampleBankReloc);

    size = 0;
    for (i = 0; i < AG.numUsedSamples; i++) {
        size += ALIGN16(AG.usedSamples[i]->size);
    }
    if (size && size) {}

    for (i = 0; i < AG.numUsedSamples; i++) {
        if (AG.preloadSampleStackTop == 120) {
            break;
        }

        sample = AG.usedSamples[i];
        sampleRamAddr = NULL;
        switch (isAsync) {
            case false:
                if (sample->medium == sampleBankReloc->medium1) {
                    sampleRamAddr = Nas_Alloc_Single(sample->size, sampleBankReloc->sampleBankId1,
                                                               sample->sampleAddr, sample->medium, CACHE_PERSISTENT);
                } else if (sample->medium == sampleBankReloc->medium2) {
                    sampleRamAddr = Nas_Alloc_Single(sample->size, sampleBankReloc->sampleBankId2,
                                                               sample->sampleAddr, sample->medium, CACHE_PERSISTENT);
                } else if (sample->medium == MEDIUM_DISK_DRIVE) {
                    sampleRamAddr = Nas_Alloc_Single(sample->size, 0xFE, sample->sampleAddr, sample->medium,
                                                               CACHE_PERSISTENT);
                }
                break;

            case true:
                if (sample->medium == sampleBankReloc->medium1) {
                    sampleRamAddr = Nas_Alloc_Single(sample->size, sampleBankReloc->sampleBankId1,
                                                               sample->sampleAddr, sample->medium, CACHE_TEMPORARY);
                } else if (sample->medium == sampleBankReloc->medium2) {
                    sampleRamAddr = Nas_Alloc_Single(sample->size, sampleBankReloc->sampleBankId2,
                                                               sample->sampleAddr, sample->medium, CACHE_TEMPORARY);
                } else if (sample->medium == MEDIUM_DISK_DRIVE) {
                    sampleRamAddr = Nas_Alloc_Single(sample->size, 0xFE, sample->sampleAddr, sample->medium,
                                                               CACHE_TEMPORARY);
                }
                break;
        }
        if (sampleRamAddr == NULL) {
            continue;
        }

        switch (isAsync) {
            case false:
                if (sample->medium == MEDIUM_UNK) {
                    Nas_FastDiskCopy((u32)sample->sampleAddr, sampleRamAddr, sample->size,
                                               AG.sampleBankTable->header.unkMediumParam);
                    sample->sampleAddr = sampleRamAddr;
                    sample->medium = MEDIUM_RAM;
                } else {
                    Nas_FastCopy((u32)sample->sampleAddr, sampleRamAddr, sample->size, sample->medium);
                    sample->sampleAddr = sampleRamAddr;
                    sample->medium = MEDIUM_RAM;
                }
                if (sample->medium == MEDIUM_DISK_DRIVE) {}
                break;

            case true:
                preload = &AG.preloadSampleStack[AG.preloadSampleStackTop];
                preload->sample = sample;
                preload->ramAddr = sampleRamAddr;
                preload->encodedInfo = (AG.preloadSampleStackTop << 24) | 0xFFFFFF;
                preload->isFree = false;
                preload->endAndMediumKey = (u32)sample->sampleAddr + sample->size + sample->medium;
                AG.preloadSampleStackTop++;
                break;
        }
    }
    AG.numUsedSamples = 0;

    if (AG.preloadSampleStackTop != 0 && !preloadInProgress) {
        topPreload = &AG.preloadSampleStack[AG.preloadSampleStackTop - 1];
        sample = topPreload->sample;
        nChunks = (sample->size >> 12) + 1;
        Nas_BgCopyReq((u32)sample->sampleAddr, topPreload->ramAddr, sample->size, sample->medium, nChunks,
                                 &AG.preloadSampleQueue, topPreload->encodedInfo);
    }
}

s32 Nas_CheckBgWave(s32 resetStatus) {
    Sample* sample;
    AudioPreloadReq* preload;
    u32 preloadIndex;
    u32 key;
    u32 nChunks;
    s32 pad;

    if (AG.preloadSampleStackTop > 0) {
        if (resetStatus != 0) {
            // Clear result queue and preload stack and return.
            osRecvMesg(&AG.preloadSampleQueue, (OSMesg*)&preloadIndex, OS_MESG_NOBLOCK);
            AG.preloadSampleStackTop = 0;
            return false;
        }
        if (osRecvMesg(&AG.preloadSampleQueue, (OSMesg*)&preloadIndex, OS_MESG_NOBLOCK) == -1) {
            // Previous preload is not done yet.
            return false;
        }

        preloadIndex >>= 24;
        preload = &AG.preloadSampleStack[preloadIndex];

        if (!preload->isFree) {
            sample = preload->sample;
            key = (u32)sample->sampleAddr + sample->size + sample->medium;
            if (key == preload->endAndMediumKey) {
                // Change storage for sample to the preloaded version.
                sample->sampleAddr = preload->ramAddr;
                sample->medium = MEDIUM_RAM;
            }
            preload->isFree = true;
        }

        // Pop requests with isFree = true off the stack, as far as possible,
        // and dispatch the next DMA.
        while (true) {
            if (AG.preloadSampleStackTop <= 0) {
                break;
            }
            preload = &AG.preloadSampleStack[AG.preloadSampleStackTop - 1];
            if (preload->isFree == true) {
                AG.preloadSampleStackTop--;
                continue;
            }

            sample = preload->sample;
            nChunks = (sample->size >> 12) + 1;
            key = (u32)sample->sampleAddr + sample->size + sample->medium;
            if (key != preload->endAndMediumKey) {
                preload->isFree = true;
                AG.preloadSampleStackTop--;
            } else {
                Nas_BgCopyReq((u32)sample->sampleAddr, preload->ramAddr, sample->size, sample->medium,
                                         nChunks, &AG.preloadSampleQueue, preload->encodedInfo);
                break;
            }
        }
    }
    return true;
}

s32 __AddList(Sample* sample, s32 numSamples, Sample** sampleSet) {
    s32 i;

    for (i = 0; i < numSamples; i++) {
        if (sample->sampleAddr == sampleSet[i]->sampleAddr) {
            break;
        }
    }

    if (i == numSamples) {
        sampleSet[numSamples] = sample;
        numSamples++;
    }

    return numSamples;
}

s32 MakeWaveList(s32 fontId, Sample** sampleSet) {
    s32 i;
    s32 numSamples = 0;
    s32 numDrums = AG.soundFontList[fontId].numDrums;
    s32 numInstruments = AG.soundFontList[fontId].numInstruments;

    for (i = 0; i < numDrums; i++) {
        Drum* drum = PercToPp(fontId, i);

        if (1) {}
        if (drum != NULL) {
            numSamples = __AddList(drum->tunedSample.sample, numSamples, sampleSet);
        }
    }

    for (i = 0; i < numInstruments; i++) {
        Instrument* instrument = ProgToVp(fontId, i);

        if (instrument != NULL) {
            if (instrument->normalRangeLo != 0) {
                numSamples = __AddList(instrument->lowPitchTunedSample.sample, numSamples, sampleSet);
            }
            if (instrument->normalRangeHi != 0x7F) {
                numSamples = __AddList(instrument->highPitchTunedSample.sample, numSamples, sampleSet);
            }
            numSamples = __AddList(instrument->normalPitchTunedSample.sample, numSamples, sampleSet);
        }
    }

    // Should really also process sfx, but this method is never called.
    return numSamples;
}

void __Reload(TunedSample* tunedSample) {
    Sample* sample = tunedSample->sample;

    if ((sample->size != 0) && sample->unk_bit26 && (sample->medium != MEDIUM_RAM)) {
        AG.usedSamples[AG.numUsedSamples++] = sample;
    }
}

void WaveReload(s32 fontId, s32 async, SampleBankRelocInfo* sampleBankReloc) {
    s32 numDrums;
    s32 numInstruments;
    s32 numSfx;
    Drum* drum;
    Instrument* instrument;
    SoundEffect* soundEffect;
    AudioPreloadReq* preload;
    AudioPreloadReq* topPreload;
    u8* addr;
    s32 size;
    s32 i;
    Sample* sample;
    s32 preloadInProgress;
    s32 nChunks;

    preloadInProgress = false;
    if (AG.preloadSampleStackTop != 0) {
        preloadInProgress = true;
    }

    AG.numUsedSamples = 0;

    numDrums = AG.soundFontList[fontId].numDrums;
    numInstruments = AG.soundFontList[fontId].numInstruments;
    numSfx = AG.soundFontList[fontId].numSfx;

    for (i = 0; i < numInstruments; i++) {
        instrument = ProgToVp(fontId, i);
        if (instrument != NULL) {
            if (instrument->normalRangeLo != 0) {
                __Reload(&instrument->lowPitchTunedSample);
            }
            if (instrument->normalRangeHi != 0x7F) {
                __Reload(&instrument->highPitchTunedSample);
            }
            __Reload(&instrument->normalPitchTunedSample);
        }
    }

    for (i = 0; i < numDrums; i++) {
        drum = PercToPp(fontId, i);
        if (drum != NULL) {
            __Reload(&drum->tunedSample);
        }
    }

    for (i = 0; i < numSfx; i++) {
        soundEffect = VpercToVep(fontId, i);
        if (soundEffect != NULL) {
            __Reload(&soundEffect->tunedSample);
        }
    }

    if (AG.numUsedSamples == 0) {
        return;
    }

    size = 0;
    for (i = 0; i < AG.numUsedSamples; i++) {
        size += ALIGN16(AG.usedSamples[i]->size);
    }
    if (size) {}

    for (i = 0; i < AG.numUsedSamples; i++) {
        if (AG.preloadSampleStackTop == 120) {
            break;
        }

        sample = AG.usedSamples[i];
        if (sample->medium == MEDIUM_RAM) {
            continue;
        }

        switch (async) {
            case false:
                if (sample->medium == sampleBankReloc->medium1) {
                    addr = Nas_Alloc_Single(sample->size, sampleBankReloc->sampleBankId1, sample->sampleAddr,
                                                      sample->medium, CACHE_PERSISTENT);
                } else if (sample->medium == sampleBankReloc->medium2) {
                    addr = Nas_Alloc_Single(sample->size, sampleBankReloc->sampleBankId2, sample->sampleAddr,
                                                      sample->medium, CACHE_PERSISTENT);
                }
                break;

            case true:
                if (sample->medium == sampleBankReloc->medium1) {
                    addr = Nas_Alloc_Single(sample->size, sampleBankReloc->sampleBankId1, sample->sampleAddr,
                                                      sample->medium, CACHE_TEMPORARY);
                } else if (sample->medium == sampleBankReloc->medium2) {
                    addr = Nas_Alloc_Single(sample->size, sampleBankReloc->sampleBankId2, sample->sampleAddr,
                                                      sample->medium, CACHE_TEMPORARY);
                }
                break;
        }
        if (addr == NULL) {
            continue;
        }

        switch (async) {
            case false:
                if (sample->medium == MEDIUM_UNK) {
                    Nas_FastDiskCopy((u32)sample->sampleAddr, addr, sample->size,
                                               AG.sampleBankTable->header.unkMediumParam);
                    sample->sampleAddr = addr;
                    sample->medium = MEDIUM_RAM;
                } else {
                    Nas_FastCopy((u32)sample->sampleAddr, addr, sample->size, sample->medium);
                    sample->sampleAddr = addr;
                    sample->medium = MEDIUM_RAM;
                }
                break;

            case true:
                preload = &AG.preloadSampleStack[AG.preloadSampleStackTop];
                preload->sample = sample;
                preload->ramAddr = addr;
                preload->encodedInfo = (AG.preloadSampleStackTop << 24) | 0xFFFFFF;
                preload->isFree = false;
                preload->endAndMediumKey = (u32)sample->sampleAddr + sample->size + sample->medium;
                AG.preloadSampleStackTop++;
                break;
        }
    }
    AG.numUsedSamples = 0;

    if (AG.preloadSampleStackTop != 0 && !preloadInProgress) {
        topPreload = &AG.preloadSampleStack[AG.preloadSampleStackTop - 1];
        sample = topPreload->sample;
        nChunks = (sample->size >> 12) + 1;
        Nas_BgCopyReq((u32)sample->sampleAddr, topPreload->ramAddr, sample->size, sample->medium, nChunks,
                                 &AG.preloadSampleQueue, topPreload->encodedInfo);
    }
}

void EmemReload(void) {
    s32 pad;
    u32 fontId;
    AudioTable* sampleBankTable;
    s32 pad2;
    s32 i;

    sampleBankTable = __Get_ArcHeader(SAMPLE_TABLE);
    for (i = 0; i < AG.permanentPool.numEntries; i++) {
        SampleBankRelocInfo sampleBankReloc;

        if (AG.permanentCache[i].tableType == FONT_TABLE) {
            fontId = __Link_BankNum(FONT_TABLE, AG.permanentCache[i].id);
            sampleBankReloc.sampleBankId1 = AG.soundFontList[fontId].sampleBankId1;
            sampleBankReloc.sampleBankId2 = AG.soundFontList[fontId].sampleBankId2;

            if (sampleBankReloc.sampleBankId1 != 0xFF) {
                sampleBankReloc.sampleBankId1 =
                    __Link_BankNum(SAMPLE_TABLE, sampleBankReloc.sampleBankId1);
                sampleBankReloc.medium1 = sampleBankTable->entries[sampleBankReloc.sampleBankId1].medium;
            }

            if (sampleBankReloc.sampleBankId2 != 0xFF) {
                sampleBankReloc.sampleBankId2 =
                    __Link_BankNum(SAMPLE_TABLE, sampleBankReloc.sampleBankId2);
                sampleBankReloc.medium2 = sampleBankTable->entries[sampleBankReloc.sampleBankId2].medium;
            }
            WaveReload(fontId, false, &sampleBankReloc);
        }
    }
}

void __ExtDiskFinishCheck(void) {
}

void __ExtDiskInit(void) {
}

void __ExtDiskLoad(void) {
}

void MK_load(s32 tableType, s32 id, s8* status) {
    static u32 use = 0;

    MK_RMES[use] = status;
    Nas_PreLoad_BG(tableType, id, 0, use, &MK_QUEUE);
    use++;
    if (use == 0x10) {
        use = 0;
    }
}

void MK_FrameWork(void) {
    u32 temp;
    u32 sp20;
    s8* status;

    if (osRecvMesg(&MK_QUEUE, (OSMesg*)&sp20, OS_MESG_NOBLOCK) != -1) {
        temp = sp20 >> 24;
        status = MK_RMES[temp];
        if (status != NULL) {
            *status = 0;
        }
    }
}

void MK_Init(void) {
    osCreateMesgQueue(&MK_QUEUE, MK_QBUF, ARRAY_COUNT(MK_QBUF));
}
