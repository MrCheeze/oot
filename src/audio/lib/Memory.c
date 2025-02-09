#include "ultra64.h"
#include "global.h"
#include "versions.h"

void Nas_Init_Single(u32 persistentSampleCacheSize, u32 temporarySampleCacheSize);
SampleCacheEntry* __Nas_Alloc_Single_Auto_Inner(u32 size);
SampleCacheEntry* __Nas_Alloc_Single_Stay_Inner(u32 size);
void __KillSwMember(SampleCacheEntry* entry);
void __RomAddrSet(SampleCacheEntry* entry, Sample* sample);
void Emem_KillSwMember(void);
void DirtyWave(s32 sampleBankId);
void Dirty_AllWave(void);

/**
 * Effectively scales `ticksPerUpdateInv` by the reciprocal of `scaleInv`
 */
f32 __CalcRelf(f32 scaleInv) {
    return (256.0f * AG.audioBufferParameters.ticksPerUpdateInvScaled) / scaleInv;
}

/**
 * Initialize the decay rate table used for decaying notes as part of adsr
 */
void MakeReleaseTable(void) {
    s32 i;

    AG.adsrDecayTable[255] = __CalcRelf(0.25f);
    AG.adsrDecayTable[254] = __CalcRelf(0.33f);
    AG.adsrDecayTable[253] = __CalcRelf(0.5f);
    AG.adsrDecayTable[252] = __CalcRelf(0.66f);
    AG.adsrDecayTable[251] = __CalcRelf(0.75f);

    for (i = 128; i < 251; i++) {
        AG.adsrDecayTable[i] = __CalcRelf(251 - i);
    }

    for (i = 16; i < 128; i++) {
        AG.adsrDecayTable[i] = __CalcRelf(4 * (143 - i));
    }

    for (i = 1; i < 16; i++) {
        AG.adsrDecayTable[i] = __CalcRelf(60 * (23 - i));
    }

    AG.adsrDecayTable[0] = 0.0f;
}

void Nas_ResetIDtable(void) {
    s32 i;

    for (i = 0; i < 0x30; i++) {
        if (AG.fontLoadStatus[i] != LOAD_STATUS_PERMANENTLY_LOADED) {
            AG.fontLoadStatus[i] = LOAD_STATUS_NOT_LOADED;
        }
    }

    for (i = 0; i < 0x30; i++) {
        if (AG.sampleFontLoadStatus[i] != LOAD_STATUS_PERMANENTLY_LOADED) {
            AG.sampleFontLoadStatus[i] = LOAD_STATUS_NOT_LOADED;
        }
    }

    for (i = 0; i < 0x80; i++) {
        if (AG.seqLoadStatus[i] != LOAD_STATUS_PERMANENTLY_LOADED) {
            AG.seqLoadStatus[i] = LOAD_STATUS_NOT_LOADED;
        }
    }
}

void Nas_ForceStopChannel(s32 fontId) {
    s32 i;

    for (i = 0; i < AG.numNotes; i++) {
        Note* note = &AG.notes[i];

        if (note->playbackState.fontId == fontId) {
            if (note->playbackState.unk_04 == 0 && note->playbackState.priority != 0) {
                note->playbackState.parentLayer->enabled = false;
                note->playbackState.parentLayer->finished = true;
            }
            Nas_StopVoice(note);
            Nas_CutList(&note->listItem);
            Nas_AddList(&AG.noteFreeLists.disabled, &note->listItem);
        }
    }
}

void Nas_ForceReleaseChannel(s32 fontId) {
    s32 i;

    for (i = 0; i < AG.numNotes; i++) {
        Note* note = &AG.notes[i];
        NotePlaybackState* playbackState = &note->playbackState;

        if (playbackState->fontId == fontId) {
            if ((playbackState->priority != 0) && (playbackState->adsr.action.s.state == ADSR_STATE_DECAY)) {
                playbackState->priority = 1;
                playbackState->adsr.fadeOutVel = AG.audioBufferParameters.ticksPerUpdateInv;
                playbackState->adsr.action.s.release = true;
            }
        }
    }
}

void Nas_ForceStopSeq(s32 seqId) {
    s32 i;

    for (i = 0; i < AG.audioBufferParameters.numSequencePlayers; i++) {
        if (AG.seqPlayers[i].enabled && AG.seqPlayers[i].seqId == seqId) {
            Nas_ReleaseGroup(&AG.seqPlayers[i]);
        }
    }
}

/**
 * Perform a writeback from the data cache to the ram.
 */
void Nas_CacheOff(void* ramAddr, u32 size) {
    osWritebackDCache2(ramAddr, size);
}

/**
 * Attempt to allocate space externally to the audio heap. If no external pool is available,
 * then allocate space on the pool provided in the argument.
 * The newly allocated space is zero'ed
 */
void* Nas_2ndHeapAlloc_CL(AudioAllocPool* pool, u32 size) {
    void* ramAddr = NULL;

    if (AG.externalPool.startRamAddr != NULL) {
        ramAddr = Nas_HeapAlloc_CL(&AG.externalPool, size);
    }
    if (ramAddr == NULL) {
        ramAddr = Nas_HeapAlloc_CL(pool, size);
    }
    return ramAddr;
}

void* Nas_2ndHeapAlloc(AudioAllocPool* pool, u32 size) {
    void* ramAddr = NULL;

    if (AG.externalPool.startRamAddr != NULL) {
        ramAddr = Nas_HeapAlloc(&AG.externalPool, size);
    }
    if (ramAddr == NULL) {
        ramAddr = Nas_HeapAlloc(pool, size);
    }
    return ramAddr;
}

void* Nas_NcHeapAlloc(AudioAllocPool* pool, u32 size) {
    void* ramAddr = Nas_HeapAlloc(pool, size);

    if (ramAddr != NULL) {
        Nas_CacheOff(ramAddr, size);
    }
    return ramAddr;
}

void* Nas_NcHeapAlloc_CL(AudioAllocPool* pool, u32 size) {
    void* ramAddr;

    ramAddr = Nas_HeapAlloc_CL(pool, size);
    if (ramAddr != NULL) {
        Nas_CacheOff(ramAddr, size);
    }
    return ramAddr;
}

/**
 * Allocates space on a pool contained within the heap and sets all the allocated space to 0
 */
void* Nas_HeapAlloc_CL(AudioAllocPool* pool, u32 size) {
    u8* ramAddr = Nas_HeapAlloc(pool, size);
    u8* ptr;

    if (ramAddr != NULL) {
        for (ptr = ramAddr; ptr < pool->curRamAddr; ptr++) {
            *ptr = 0;
        }
    }

    return ramAddr;
}

void* Nas_HeapAlloc(AudioAllocPool* pool, u32 size) {
    u32 aligned = ALIGN16(size);
    u8* ramAddr = pool->curRamAddr;

    if (pool->startRamAddr + pool->size >= pool->curRamAddr + aligned) {
        pool->curRamAddr += aligned;
    } else {
        return NULL;
    }
    pool->numEntries++;
    return ramAddr;
}

/**
 * Initialize a pool to allocate memory from the specified address, up to the specified size.
 * Store the metadata of this pool in AudioAllocPool* pool
 */
void Nas_HeapInit(AudioAllocPool* pool, void* ramAddr, u32 size) {
    pool->curRamAddr = pool->startRamAddr = (u8*)ALIGN16((u32)ramAddr);
    pool->size = size - ((u32)ramAddr & 0xF);
    pool->numEntries = 0;
}

void Nas_SzStayClear(AudioPersistentCache* persistent) {
    persistent->pool.numEntries = 0;
    persistent->numEntries = 0;
    persistent->pool.curRamAddr = persistent->pool.startRamAddr;
}

void Nas_SzAutoClear(AudioTemporaryCache* temporary) {
    temporary->pool.numEntries = 0;
    temporary->pool.curRamAddr = temporary->pool.startRamAddr;
    temporary->nextSide = 0;
    temporary->entries[0].ramAddr = temporary->pool.startRamAddr;
    temporary->entries[1].ramAddr = temporary->pool.startRamAddr + temporary->pool.size;
    temporary->entries[0].id = -1;
    temporary->entries[1].id = -1;
}

void Nas_SzCustomClear(AudioAllocPool* pool) {
    pool->numEntries = 0;
    pool->curRamAddr = pool->startRamAddr;
}

void Nas_SzStayDelete(s32 tableType) {
    AudioCache* loadedCache;
    AudioAllocPool* persistentPool;
    AudioPersistentCache* persistent;
    void* entryRamAddr;
    u8* loadStatus;

    switch (tableType) {
        case SEQUENCE_TABLE:
            loadedCache = &AG.seqCache;
            loadStatus = AG.seqLoadStatus;
            break;

        case FONT_TABLE:
            loadedCache = &AG.fontCache;
            loadStatus = AG.fontLoadStatus;
            break;

        case SAMPLE_TABLE:
            loadedCache = &AG.sampleBankCache;
            loadStatus = AG.sampleFontLoadStatus;
            break;
    }

    persistent = &loadedCache->persistent;
    persistentPool = &persistent->pool;

    if (persistent->numEntries == 0) {
        return;
    }

    entryRamAddr = persistent->entries[persistent->numEntries - 1].ramAddr;
    persistentPool->curRamAddr = entryRamAddr;
    persistentPool->numEntries--;

    if (tableType == SAMPLE_TABLE) {
        DirtyWave(persistent->entries[persistent->numEntries - 1].id);
    }
    if (tableType == FONT_TABLE) {
        Nas_ForceStopChannel(persistent->entries[persistent->numEntries - 1].id);
    }

    loadStatus[persistent->entries[persistent->numEntries - 1].id] = LOAD_STATUS_NOT_LOADED;
    persistent->numEntries--;
}

void Nas_SzHeapReset(s32 initPoolSize) {
    Nas_HeapInit(&AG.initPool, AG.audioHeap, initPoolSize);
    Nas_HeapInit(&AG.sessionPool, AG.audioHeap + initPoolSize,
                       AG.audioHeapSize - initPoolSize);
    AG.externalPool.startRamAddr = NULL;
}

void Nas_SzHeapDivide(AudioSessionPoolSplit* split) {
    AG.sessionPool.curRamAddr = AG.sessionPool.startRamAddr;
    Nas_HeapInit(&AG.miscPool, Nas_HeapAlloc(&AG.sessionPool, split->miscPoolSize),
                       split->miscPoolSize);
    Nas_HeapInit(&AG.cachePool, Nas_HeapAlloc(&AG.sessionPool, split->cachePoolSize),
                       split->cachePoolSize);
}

void Nas_SzDataDivide(AudioCachePoolSplit* split) {
    AG.cachePool.curRamAddr = AG.cachePool.startRamAddr;
    Nas_HeapInit(&AG.persistentCommonPool,
                       Nas_HeapAlloc(&AG.cachePool, split->persistentCommonPoolSize),
                       split->persistentCommonPoolSize);
    Nas_HeapInit(&AG.temporaryCommonPool,
                       Nas_HeapAlloc(&AG.cachePool, split->temporaryCommonPoolSize),
                       split->temporaryCommonPoolSize);
}

void Nas_SzStayDivide(AudioCommonPoolSplit* split) {
    AG.persistentCommonPool.curRamAddr = AG.persistentCommonPool.startRamAddr;
    Nas_HeapInit(&AG.seqCache.persistent.pool,
                       Nas_HeapAlloc(&AG.persistentCommonPool, split->seqCacheSize), split->seqCacheSize);
    Nas_HeapInit(&AG.fontCache.persistent.pool,
                       Nas_HeapAlloc(&AG.persistentCommonPool, split->fontCacheSize), split->fontCacheSize);
    Nas_HeapInit(&AG.sampleBankCache.persistent.pool,
                       Nas_HeapAlloc(&AG.persistentCommonPool, split->sampleBankCacheSize),
                       split->sampleBankCacheSize);
    Nas_SzStayClear(&AG.seqCache.persistent);
    Nas_SzStayClear(&AG.fontCache.persistent);
    Nas_SzStayClear(&AG.sampleBankCache.persistent);
}

void Nas_SzAutoDivide(AudioCommonPoolSplit* split) {
    AG.temporaryCommonPool.curRamAddr = AG.temporaryCommonPool.startRamAddr;
    Nas_HeapInit(&AG.seqCache.temporary.pool,
                       Nas_HeapAlloc(&AG.temporaryCommonPool, split->seqCacheSize), split->seqCacheSize);
    Nas_HeapInit(&AG.fontCache.temporary.pool,
                       Nas_HeapAlloc(&AG.temporaryCommonPool, split->fontCacheSize), split->fontCacheSize);
    Nas_HeapInit(&AG.sampleBankCache.temporary.pool,
                       Nas_HeapAlloc(&AG.temporaryCommonPool, split->sampleBankCacheSize),
                       split->sampleBankCacheSize);
    Nas_SzAutoClear(&AG.seqCache.temporary);
    Nas_SzAutoClear(&AG.fontCache.temporary);
    Nas_SzAutoClear(&AG.sampleBankCache.temporary);
}

void* Nas_SzHeapAlloc(s32 tableType, s32 size, s32 cache, s32 id) {
    AudioCache* loadedCache;
    AudioTemporaryCache* temporaryCache;
    AudioAllocPool* temporaryPool;
    void* persistentRamAddr;
    void* temporaryRamAddr;
    u8 loadStatusEntry0;
    u8 loadStatusEntry1;
    s32 i;
    u8* loadStatus;
    s32 side;

    switch (tableType) {
        case SEQUENCE_TABLE:
            loadedCache = &AG.seqCache;
            loadStatus = AG.seqLoadStatus;
            break;

        case FONT_TABLE:
            loadedCache = &AG.fontCache;
            loadStatus = AG.fontLoadStatus;
            break;

        case SAMPLE_TABLE:
            loadedCache = &AG.sampleBankCache;
            loadStatus = AG.sampleFontLoadStatus;
            break;
    }

    if (cache == CACHE_TEMPORARY) {
        temporaryCache = &loadedCache->temporary;
        temporaryPool = &temporaryCache->pool;

        if (temporaryPool->size < size) {
            return NULL;
        }

        loadStatusEntry0 =
            (temporaryCache->entries[0].id == -1) ? LOAD_STATUS_NOT_LOADED : loadStatus[temporaryCache->entries[0].id];
        loadStatusEntry1 =
            (temporaryCache->entries[1].id == -1) ? LOAD_STATUS_NOT_LOADED : loadStatus[temporaryCache->entries[1].id];

        if (tableType == FONT_TABLE) {
            if (loadStatusEntry0 == LOAD_STATUS_MAYBE_DISCARDABLE) {
                for (i = 0; i < AG.numNotes; i++) {
                    if (AG.notes[i].playbackState.fontId == temporaryCache->entries[0].id &&
                        AG.notes[i].noteSubEu.bitField0.enabled) {
                        break;
                    }
                }

                if (i == AG.numNotes) {
                    Nas_WriteIDbank(temporaryCache->entries[0].id, LOAD_STATUS_DISCARDABLE);
                    loadStatusEntry0 = LOAD_STATUS_DISCARDABLE;
                }
            }

            if (loadStatusEntry1 == LOAD_STATUS_MAYBE_DISCARDABLE) {
                for (i = 0; i < AG.numNotes; i++) {
                    if (AG.notes[i].playbackState.fontId == temporaryCache->entries[1].id &&
                        AG.notes[i].noteSubEu.bitField0.enabled) {
                        break;
                    }
                }

                if (i == AG.numNotes) {
                    Nas_WriteIDbank(temporaryCache->entries[1].id, LOAD_STATUS_DISCARDABLE);
                    loadStatusEntry1 = LOAD_STATUS_DISCARDABLE;
                }
            }
        }

        if (loadStatusEntry0 == LOAD_STATUS_NOT_LOADED) {
            temporaryCache->nextSide = 0;
        } else if (loadStatusEntry1 == LOAD_STATUS_NOT_LOADED) {
            temporaryCache->nextSide = 1;
        } else if (loadStatusEntry0 == LOAD_STATUS_DISCARDABLE && loadStatusEntry1 == LOAD_STATUS_DISCARDABLE) {
            // Use the opposite side from last time.
        } else if (loadStatusEntry0 == LOAD_STATUS_DISCARDABLE) {
            temporaryCache->nextSide = 0;
        } else if (loadStatusEntry1 == LOAD_STATUS_DISCARDABLE) {
            temporaryCache->nextSide = 1;
        } else {
            // Check if there is a side which isn't in active use, if so, evict that one.
            if (tableType == SEQUENCE_TABLE) {
                if (loadStatusEntry0 == LOAD_STATUS_COMPLETE) {
                    for (i = 0; i < AG.audioBufferParameters.numSequencePlayers; i++) {
                        if (AG.seqPlayers[i].enabled &&
                            AG.seqPlayers[i].seqId == temporaryCache->entries[0].id) {
                            break;
                        }
                    }

                    if (i == AG.audioBufferParameters.numSequencePlayers) {
                        temporaryCache->nextSide = 0;
                        goto done;
                    }
                }

                if (loadStatusEntry1 == LOAD_STATUS_COMPLETE) {
                    for (i = 0; i < AG.audioBufferParameters.numSequencePlayers; i++) {
                        if (AG.seqPlayers[i].enabled &&
                            AG.seqPlayers[i].seqId == temporaryCache->entries[1].id) {
                            break;
                        }
                    }

                    if (i == AG.audioBufferParameters.numSequencePlayers) {
                        temporaryCache->nextSide = 1;
                        goto done;
                    }
                }
            } else if (tableType == FONT_TABLE) {
                if (loadStatusEntry0 == LOAD_STATUS_COMPLETE) {
                    for (i = 0; i < AG.numNotes; i++) {
                        if (AG.notes[i].playbackState.fontId == temporaryCache->entries[0].id &&
                            AG.notes[i].noteSubEu.bitField0.enabled) {
                            break;
                        }
                    }
                    if (i == AG.numNotes) {
                        temporaryCache->nextSide = 0;
                        goto done;
                    }
                }

                if (loadStatusEntry1 == LOAD_STATUS_COMPLETE) {
                    for (i = 0; i < AG.numNotes; i++) {
                        if (AG.notes[i].playbackState.fontId == temporaryCache->entries[1].id &&
                            AG.notes[i].noteSubEu.bitField0.enabled) {
                            break;
                        }
                    }
                    if (i == AG.numNotes) {
                        temporaryCache->nextSide = 1;
                        goto done;
                    }
                }
            }

            // No such luck. Evict the side that wasn't chosen last time, except
            // if it is being loaded into.
            if (temporaryCache->nextSide == 0) {
                if (loadStatusEntry0 == LOAD_STATUS_IN_PROGRESS) {
                    if (loadStatusEntry1 == LOAD_STATUS_IN_PROGRESS) {
                        goto fail;
                    }
                    temporaryCache->nextSide = 1;
                }
            } else {
                if (loadStatusEntry1 == LOAD_STATUS_IN_PROGRESS) {
                    if (loadStatusEntry0 == LOAD_STATUS_IN_PROGRESS) {
                        goto fail;
                    }
                    temporaryCache->nextSide = 0;
                }
            }

            if (0) {
            fail:
                // Both sides are being loaded into.
                return NULL;
            }
        }
    done:

        side = temporaryCache->nextSide;

        if (temporaryCache->entries[side].id != -1) {
            if (tableType == SAMPLE_TABLE) {
                DirtyWave(temporaryCache->entries[side].id);
            }

            loadStatus[temporaryCache->entries[side].id] = LOAD_STATUS_NOT_LOADED;

            if (tableType == FONT_TABLE) {
                Nas_ForceStopChannel(temporaryCache->entries[side].id);
            }
        }

        switch (side) {
            case 0:
                temporaryCache->entries[0].ramAddr = temporaryPool->startRamAddr;
                temporaryCache->entries[0].id = id;
                temporaryCache->entries[0].size = size;
                temporaryPool->curRamAddr = temporaryPool->startRamAddr + size;

                if (temporaryCache->entries[1].id != -1 &&
                    temporaryCache->entries[1].ramAddr < temporaryPool->curRamAddr) {
                    if (tableType == SAMPLE_TABLE) {
                        DirtyWave(temporaryCache->entries[1].id);
                    }

                    loadStatus[temporaryCache->entries[1].id] = LOAD_STATUS_NOT_LOADED;

                    switch (tableType) {
                        case SEQUENCE_TABLE:
                            Nas_ForceStopSeq((s32)temporaryCache->entries[1].id);
                            break;

                        case FONT_TABLE:
                            Nas_ForceStopChannel((s32)temporaryCache->entries[1].id);
                            break;
                    }

                    temporaryCache->entries[1].id = -1;
                    temporaryCache->entries[1].ramAddr = temporaryPool->startRamAddr + temporaryPool->size;
                }

                temporaryRamAddr = temporaryCache->entries[0].ramAddr;
                break;

            case 1:
                temporaryCache->entries[1].ramAddr =
                    (u8*)((u32)(temporaryPool->startRamAddr + temporaryPool->size - size) & ~0xF);
                temporaryCache->entries[1].id = id;
                temporaryCache->entries[1].size = size;
                if (temporaryCache->entries[0].id != -1 &&
                    temporaryCache->entries[1].ramAddr < temporaryPool->curRamAddr) {
                    if (tableType == SAMPLE_TABLE) {
                        DirtyWave(temporaryCache->entries[0].id);
                    }

                    loadStatus[temporaryCache->entries[0].id] = LOAD_STATUS_NOT_LOADED;
                    switch (tableType) {
                        case SEQUENCE_TABLE:
                            Nas_ForceStopSeq(temporaryCache->entries[0].id);
                            break;

                        case FONT_TABLE:
                            Nas_ForceStopChannel(temporaryCache->entries[0].id);
                            break;
                    }

                    temporaryCache->entries[0].id = -1;
                    temporaryPool->curRamAddr = temporaryPool->startRamAddr;
                }
                temporaryRamAddr = temporaryCache->entries[1].ramAddr;
                break;

            default:
                return NULL;
        }

        temporaryCache->nextSide ^= 1;
        return temporaryRamAddr;
    }

    persistentRamAddr = Nas_HeapAlloc(&loadedCache->persistent.pool, size);
    loadedCache->persistent.entries[loadedCache->persistent.numEntries].ramAddr = persistentRamAddr;

    if (persistentRamAddr == NULL) {
        switch (cache) {
            case CACHE_EITHER:
                return Nas_SzHeapAlloc(tableType, size, CACHE_TEMPORARY, id);

            case CACHE_TEMPORARY:
            case CACHE_PERSISTENT:
                return NULL;
        }
    }

    loadedCache->persistent.entries[loadedCache->persistent.numEntries].id = id;
    loadedCache->persistent.entries[loadedCache->persistent.numEntries].size = size;

    return loadedCache->persistent.entries[loadedCache->persistent.numEntries++].ramAddr;
}

void* Nas_SzCacheCheck(s32 tableType, s32 cache, s32 id) {
    void* ramAddr;

    // Always search the permanent cache in addition to the regular ones.
    ramAddr = EmemOnCheck(tableType, id);
    if (ramAddr != NULL) {
        return ramAddr;
    }
    if (cache == CACHE_PERMANENT) {
        return NULL;
    }
    return __Nas_SzCacheCheck_Inner(tableType, cache, id);
}

void* __Nas_SzCacheCheck_Inner(s32 tableType, s32 cache, s32 id) {
    u32 i;
    AudioCache* loadedCache;
    AudioTemporaryCache* temporary;
    AudioPersistentCache* persistent;

    switch (tableType) {
        case SEQUENCE_TABLE:
            loadedCache = &AG.seqCache;
            break;

        case FONT_TABLE:
            loadedCache = &AG.fontCache;
            break;

        case SAMPLE_TABLE:
            loadedCache = &AG.sampleBankCache;
            break;
    }

    temporary = &loadedCache->temporary;
    if (cache == CACHE_TEMPORARY) {
        if (temporary->entries[0].id == id) {
            temporary->nextSide = 1;
            return temporary->entries[0].ramAddr;
        } else if (temporary->entries[1].id == id) {
            temporary->nextSide = 0;
            return temporary->entries[1].ramAddr;
        } else {
            return NULL;
        }
    }

    persistent = &loadedCache->persistent;
    for (i = 0; i < persistent->numEntries; i++) {
        if (persistent->entries[i].id == id) {
            return persistent->entries[i].ramAddr;
        }
    }

    if (cache == CACHE_EITHER) {
        return Nas_SzCacheCheck(tableType, CACHE_TEMPORARY, id);
    }
    return NULL;
}

void Nas_InitFilterCoef(f32 p, f32 q, u16* out) {
    // With the bug below fixed, this mysterious unused function computes two recurrences
    // out[0..7] = a_i, out[8..15] = b_i, where
    // a_{-2} = b_{-1} = 262159 = 2^18 + 15
    // a_{-1} = b_{-2} = 0
    // a_i = q * a_{i-1} + p * a_{i-2}
    // b_i = q * b_{i-1} + p * b_{i-2}
    // These grow exponentially if p < -1 or p + |q| > 1.
    s32 i;
    f32 tmp[16];

    tmp[0] = (f32)(q * 262159.0f);
    tmp[8] = (f32)(p * 262159.0f);
    tmp[1] = (f32)((q * p) * 262159.0f);
    tmp[9] = (f32)(((p * p) + q) * 262159.0f);

    for (i = 2; i < 8; i++) {
        //! @bug value should be stored to tmp[i] and tmp[8 + i], otherwise we read
        //! garbage in later loop iterations.
        out[i] = q * tmp[i - 2] + p * tmp[i - 1];
        out[8 + i] = q * tmp[6 + i] + p * tmp[7 + i];
    }

    for (i = 0; i < 16; i++) {
        out[i] = tmp[i];
    }
}

void Nas_ClearFilter(s16* filter) {
    s32 i;

    for (i = 0; i < 8; i++) {
        filter[i] = 0;
    }
}

void Nas_SetLPFilter(s16* filter, s32 cutoff) {
    s32 i;
    s16* ptr = &LSF_TABLE[8 * cutoff];

    for (i = 0; i < 8; i++) {
        filter[i] = ptr[i];
    }
}

void Nas_SetHPFilter(s16* filter, s32 cutoff) {
    s32 i;
    s16* ptr = &HSF_TABLE[8 * (cutoff - 1)];

    for (i = 0; i < 8; i++) {
        filter[i] = ptr[i];
    }
}

void Nas_SetBPFilter(s16* filter, s32 lowPassCutoff, s32 highPassCutoff) {
    s32 i;

    if (lowPassCutoff == 0 && highPassCutoff == 0) {
        // Identity filter
        Nas_SetLPFilter(filter, 0);
    } else if (highPassCutoff == 0) {
        Nas_SetLPFilter(filter, lowPassCutoff);
    } else if (lowPassCutoff == 0) {
        Nas_SetHPFilter(filter, highPassCutoff);
    } else {
        s16* ptr1 = &LSF_TABLE[8 * lowPassCutoff];
        s16* ptr2 = &HSF_TABLE[8 * (highPassCutoff - 1)];

        for (i = 0; i < 8; i++) {
            filter[i] = (ptr1[i] + ptr2[i]) / 2;
        }
    }
}

void __DownDelay(SynthesisReverb* reverb) {
}

void __Nas_DelayDown(void) {
    s32 count;
    s32 i;
    s32 j;

    if (AG.audioBufferParameters.specUnk4 == 2) {
        count = 2;
    } else {
        count = 1;
    }

    for (i = 0; i < AG.numSynthesisReverbs; i++) {
        for (j = 0; j < count; j++) {
            __DownDelay(&AG.synthesisReverbs[i]);
        }
    }
}

/**
 * Clear the current Audio Interface Buffer
 */
void __Nas_DacClear(void) {
    s32 curAiBufferIndex = AG.curAiBufIndex;
    s32 i;

    AG.aiBufLengths[curAiBufferIndex] = AG.audioBufferParameters.minAiBufferLength;

    for (i = 0; i < AIBUF_LEN; i++) {
        AG.aiBuffers[curAiBufferIndex][i] = 0;
    }
}

s32 Nas_SpecChange(void) {
    s32 i;
    s32 j;
    s32 sp24;

    if (AG.audioBufferParameters.specUnk4 == 2) {
        sp24 = 2;
    } else {
        sp24 = 1;
    }

    switch (AG.resetStatus) {
        case 5:
            for (i = 0; i < AG.audioBufferParameters.numSequencePlayers; i++) {
                Nas_ReleaseGroup_Force(&AG.seqPlayers[i]);
            }
            AG.audioResetFadeOutFramesLeft = 2 / sp24;
            AG.resetStatus--;
            break;

        case 4:
            if (AG.audioResetFadeOutFramesLeft != 0) {
                AG.audioResetFadeOutFramesLeft--;
                __Nas_DelayDown();
            } else {
                for (i = 0; i < AG.numNotes; i++) {
                    if (AG.notes[i].noteSubEu.bitField0.enabled &&
                        AG.notes[i].playbackState.adsr.action.s.state != ADSR_STATE_DISABLED) {
                        AG.notes[i].playbackState.adsr.fadeOutVel =
                            AG.audioBufferParameters.ticksPerUpdateInv;
                        AG.notes[i].playbackState.adsr.action.s.release = true;
                    }
                }
                AG.audioResetFadeOutFramesLeft = 8 / sp24;
                AG.resetStatus--;
            }
            break;

        case 3:
            if (AG.audioResetFadeOutFramesLeft != 0) {
                AG.audioResetFadeOutFramesLeft--;
                __Nas_DelayDown();
            } else {
                AG.audioResetFadeOutFramesLeft = 2 / sp24;
                AG.resetStatus--;
            }
            break;

        case 2:
            __Nas_DacClear();
            if (AG.audioResetFadeOutFramesLeft != 0) {
                AG.audioResetFadeOutFramesLeft--;
            } else {
                AG.resetStatus--;
                Emem_KillSwMember();
                Dirty_AllWave();
            }
            break;

        case 1:
            __Nas_MemoryReconfig();
            AG.resetStatus = 0;
            for (i = 0; i < 3; i++) {
                AG.aiBufLengths[i] = AG.audioBufferParameters.maxAiBufferLength;
                for (j = 0; j < AIBUF_LEN; j++) {
                    AG.aiBuffers[i][j] = 0;
                }
            }
            break;
    }

    if (AG.resetStatus < 3) {
        return false;
    }

    return true;
}

void __Nas_MemoryReconfig(void) {
    s32 pad1[4];
    s16* ramAddr;
    s32 persistentSize;
    s32 temporarySize;
    s32 cachePoolSize;
    s32 miscPoolSize;
    OSIntMask intMask;
    s32 i;
    s32 j;
    s32 pad2;
    AudioSpec* spec = &NA_SPEC_CONFIG[AG.specId]; // Audio Specifications

    AG.sampleDmaCount = 0;

    // audio buffer parameters
    AG.audioBufferParameters.samplingFrequency = spec->samplingFrequency;
    AG.audioBufferParameters.aiSamplingFrequency =
        osAiSetFrequency(AG.audioBufferParameters.samplingFrequency);
    AG.audioBufferParameters.samplesPerFrameTarget =
        ALIGN16(AG.audioBufferParameters.samplingFrequency / AG.refreshRate);
    AG.audioBufferParameters.minAiBufferLength = AG.audioBufferParameters.samplesPerFrameTarget - 0x10;
    AG.audioBufferParameters.maxAiBufferLength = AG.audioBufferParameters.samplesPerFrameTarget + 0x10;
    AG.audioBufferParameters.ticksPerUpdate =
        ((AG.audioBufferParameters.samplesPerFrameTarget + 0x10) / 0xD0) + 1;
    AG.audioBufferParameters.samplesPerTick =
        (AG.audioBufferParameters.samplesPerFrameTarget / AG.audioBufferParameters.ticksPerUpdate) & ~7;
    AG.audioBufferParameters.samplesPerTickMax = AG.audioBufferParameters.samplesPerTick + 8;
    AG.audioBufferParameters.samplesPerTickMin = AG.audioBufferParameters.samplesPerTick - 8;
    AG.audioBufferParameters.resampleRate = 32000.0f / (s32)AG.audioBufferParameters.samplingFrequency;
    AG.audioBufferParameters.ticksPerUpdateInvScaled =
        (1.0f / 256.0f) / AG.audioBufferParameters.ticksPerUpdate;
    AG.audioBufferParameters.ticksPerUpdateScaled = AG.audioBufferParameters.ticksPerUpdate / 4.0f;
    AG.audioBufferParameters.ticksPerUpdateInv = 1.0f / AG.audioBufferParameters.ticksPerUpdate;

    // SampleDma buffer size
    AG.sampleDmaBufSize1 = spec->sampleDmaBufSize1;
    AG.sampleDmaBufSize2 = spec->sampleDmaBufSize2;

    AG.numNotes = spec->numNotes;
    AG.audioBufferParameters.numSequencePlayers = spec->numSequencePlayers;
    if (AG.audioBufferParameters.numSequencePlayers > 4) {
        AG.audioBufferParameters.numSequencePlayers = 4;
    }
    AG.unk_2 = spec->unk_14;

    // (ticks / min)
    // 60 * 1000 is a conversion from milliseconds to minutes
    AG.maxTempo = (u32)(AG.audioBufferParameters.ticksPerUpdate * (f32)(60 * 1000 * SEQTICKS_PER_BEAT) /
                               AGC.seqTicksPerBeat / AG.maxTempoTvTypeFactors);

    AG.unk_2870 = AG.refreshRate;
    AG.unk_2870 *= AG.audioBufferParameters.ticksPerUpdate;
    AG.unk_2870 /= AG.audioBufferParameters.aiSamplingFrequency;
    AG.unk_2870 /= AG.maxTempo;

    AG.audioBufferParameters.specUnk4 = spec->unk_04;
    AG.audioBufferParameters.samplesPerFrameTarget *= AG.audioBufferParameters.specUnk4;
    AG.audioBufferParameters.maxAiBufferLength *= AG.audioBufferParameters.specUnk4;
    AG.audioBufferParameters.minAiBufferLength *= AG.audioBufferParameters.specUnk4;
    AG.audioBufferParameters.ticksPerUpdate *= AG.audioBufferParameters.specUnk4;

    if (AG.audioBufferParameters.specUnk4 >= 2) {
        AG.audioBufferParameters.maxAiBufferLength -= 0x10;
    }

    // Determine the length of the buffer for storing the audio command list passed to the rsp audio microcode
    AG.maxAudioCmds = AG.numNotes * 0x10 * AG.audioBufferParameters.ticksPerUpdate +
                             spec->numReverbs * 0x18 + FRAMERATE_CONST(0x140, 0x1C0);

    // Calculate sizes for various caches on the audio heap
    persistentSize =
        spec->persistentSeqCacheSize + spec->persistentFontCacheSize + spec->persistentSampleBankCacheSize + 0x10;
    temporarySize =
        spec->temporarySeqCacheSize + spec->temporaryFontCacheSize + spec->temporarySampleBankCacheSize + 0x10;
    cachePoolSize = persistentSize + temporarySize;
    miscPoolSize = AG.sessionPool.size - cachePoolSize - 0x100;

    if (AG.externalPool.startRamAddr != NULL) {
        AG.externalPool.curRamAddr = AG.externalPool.startRamAddr;
    }

    // Session Pool Split (split into Cache and Misc pools)
    AG.sessionPoolSplit.miscPoolSize = miscPoolSize;
    AG.sessionPoolSplit.cachePoolSize = cachePoolSize;
    Nas_SzHeapDivide(&AG.sessionPoolSplit);

    // Cache Pool Split (split into Persistent and Temporary pools)
    AG.cachePoolSplit.persistentCommonPoolSize = persistentSize;
    AG.cachePoolSplit.temporaryCommonPoolSize = temporarySize;
    Nas_SzDataDivide(&AG.cachePoolSplit);

    // Persistent Pool Split (split into Sequences, SoundFonts, Samples pools)
    AG.persistentCommonPoolSplit.seqCacheSize = spec->persistentSeqCacheSize;
    AG.persistentCommonPoolSplit.fontCacheSize = spec->persistentFontCacheSize;
    AG.persistentCommonPoolSplit.sampleBankCacheSize = spec->persistentSampleBankCacheSize;
    Nas_SzStayDivide(&AG.persistentCommonPoolSplit);

    // Temporary Pool Split (split into Sequences, SoundFonts, Samples pools)
    AG.temporaryCommonPoolSplit.seqCacheSize = spec->temporarySeqCacheSize;
    AG.temporaryCommonPoolSplit.fontCacheSize = spec->temporaryFontCacheSize;
    AG.temporaryCommonPoolSplit.sampleBankCacheSize = spec->temporarySampleBankCacheSize;
    Nas_SzAutoDivide(&AG.temporaryCommonPoolSplit);

    Nas_ResetIDtable();

    // Initialize notes
    AG.notes = Nas_HeapAlloc_CL(&AG.miscPool, AG.numNotes * sizeof(Note));
    Nas_ChannelInit();
    Nas_InitChannelList();
    AG.noteSubsEu = Nas_HeapAlloc_CL(&AG.miscPool, AG.audioBufferParameters.ticksPerUpdate *
                                                                          AG.numNotes * sizeof(NoteSubEu));
    // Initialize audio binary interface command list buffers
    for (i = 0; i != 2; i++) {
        AG.abiCmdBufs[i] =
            Nas_NcHeapAlloc_CL(&AG.miscPool, AG.maxAudioCmds * sizeof(Acmd));
    }

    // Initialize the decay rate table for adsr
    AG.adsrDecayTable = Nas_HeapAlloc(&AG.miscPool, 256 * sizeof(f32));
    MakeReleaseTable();

    // Initialize reverbs
    for (i = 0; i < 4; i++) {
        AG.synthesisReverbs[i].useReverb = 0;
    }

    AG.numSynthesisReverbs = spec->numReverbs;
    for (i = 0; i < AG.numSynthesisReverbs; i++) {
        ReverbSettings* settings = &spec->reverbSettings[i];
        SynthesisReverb* reverb = &AG.synthesisReverbs[i];

        reverb->downsampleRate = settings->downsampleRate;
        reverb->windowSize = settings->windowSize * 64;
        reverb->windowSize /= reverb->downsampleRate;
        reverb->decayRatio = settings->decayRatio;
        reverb->volume = settings->volume;
        reverb->unk_14 = settings->unk_6 * 64;
        reverb->unk_16 = settings->unk_8;
        reverb->unk_18 = 0;
        reverb->leakRtl = settings->leakRtl;
        reverb->leakLtr = settings->leakLtr;
        reverb->unk_05 = settings->unk_10;
        reverb->unk_08 = settings->unk_12;
        reverb->useReverb = 8;
        reverb->leftRingBuf =
            Nas_2ndHeapAlloc_CL(&AG.miscPool, reverb->windowSize * SAMPLE_SIZE);
        reverb->rightRingBuf =
            Nas_2ndHeapAlloc_CL(&AG.miscPool, reverb->windowSize * SAMPLE_SIZE);
        reverb->nextRingBufPos = 0;
        reverb->unk_20 = 0;
        reverb->curFrame = 0;
        reverb->bufSizePerChan = reverb->windowSize;
        reverb->framesToIgnore = 2;
        reverb->resampleFlags = 1;
        reverb->tunedSample.sample = &reverb->sample;
        reverb->sample.loop = &reverb->loop;
        reverb->tunedSample.tuning = 1.0f;
        reverb->sample.codec = CODEC_REVERB;
        reverb->sample.medium = MEDIUM_RAM;
        reverb->sample.size = reverb->windowSize * SAMPLE_SIZE;
        reverb->sample.sampleAddr = (u8*)reverb->leftRingBuf;
        reverb->loop.header.start = 0;
        reverb->loop.header.count = 1;
        reverb->loop.header.end = reverb->windowSize;

        if (reverb->downsampleRate != 1) {
            reverb->unk_0E = 0x8000 / reverb->downsampleRate;
            reverb->unk_30 = Nas_HeapAlloc_CL(&AG.miscPool, sizeof(RESAMPLE_STATE));
            reverb->unk_34 = Nas_HeapAlloc_CL(&AG.miscPool, sizeof(RESAMPLE_STATE));
            reverb->unk_38 = Nas_HeapAlloc_CL(&AG.miscPool, sizeof(RESAMPLE_STATE));
            reverb->unk_3C = Nas_HeapAlloc_CL(&AG.miscPool, sizeof(RESAMPLE_STATE));
            for (j = 0; j < AG.audioBufferParameters.ticksPerUpdate; j++) {
                ramAddr = Nas_2ndHeapAlloc_CL(&AG.miscPool, DMEM_2CH_SIZE);
                reverb->items[0][j].toDownsampleLeft = ramAddr;
                reverb->items[0][j].toDownsampleRight = ramAddr + DMEM_1CH_SIZE / SAMPLE_SIZE;

                ramAddr = Nas_2ndHeapAlloc_CL(&AG.miscPool, DMEM_2CH_SIZE);
                reverb->items[1][j].toDownsampleLeft = ramAddr;
                reverb->items[1][j].toDownsampleRight = ramAddr + DMEM_1CH_SIZE / SAMPLE_SIZE;
            }
        }

        if (settings->lowPassFilterCutoffLeft != 0) {
            reverb->filterLeftState =
                Nas_NcHeapAlloc_CL(&AG.miscPool, 2 * (FILTER_BUF_PART1 + FILTER_BUF_PART2));
            reverb->filterLeft = Nas_NcHeapAlloc(&AG.miscPool, FILTER_SIZE);
            Nas_SetLPFilter(reverb->filterLeft, settings->lowPassFilterCutoffLeft);
        } else {
            reverb->filterLeft = NULL;
        }

        if (settings->lowPassFilterCutoffRight != 0) {
            reverb->filterRightState =
                Nas_NcHeapAlloc_CL(&AG.miscPool, 2 * (FILTER_BUF_PART1 + FILTER_BUF_PART2));
            reverb->filterRight = Nas_NcHeapAlloc(&AG.miscPool, FILTER_SIZE);
            Nas_SetLPFilter(reverb->filterRight, settings->lowPassFilterCutoffRight);
        } else {
            reverb->filterRight = NULL;
        }
    }

    // Initialize sequence players
    Nas_InitPlayer();
    for (j = 0; j < AG.audioBufferParameters.numSequencePlayers; j++) {
        Nas_AssignSubTrack(j);
        Nas_InitMySeq(&AG.seqPlayers[j]);
    }

    // Initialize two additional sample caches for individual samples
    Nas_Init_Single(spec->persistentSampleCacheSize, spec->temporarySampleCacheSize);
    Nas_WaveDmaNew(AG.numNotes);

    // Initialize Loads
    AG.preloadSampleStackTop = 0;
    LpsInit();
    MK_Init();
    Nas_BgCopyInit();
    AG.unk_4 = 0x1000;
    EmemReload();

    intMask = osSetIntMask(OS_IM_NONE);
    osWritebackDCacheAll();
    osSetIntMask(intMask);
}

void* EmemOnCheck(s32 tableType, s32 id) {
    s32 i;

    for (i = 0; i < AG.permanentPool.numEntries; i++) {
        if (AG.permanentCache[i].tableType == tableType && AG.permanentCache[i].id == id) {
            return AG.permanentCache[i].ramAddr;
        }
    }
    return NULL;
}

void* EmemAlloc(s32 tableType, s32 id, u32 size) {
    void* ramAddr;
    s32 index = AG.permanentPool.numEntries;

    ramAddr = Nas_HeapAlloc(&AG.permanentPool, size);
    AG.permanentCache[index].ramAddr = ramAddr;
    if (ramAddr == NULL) {
        return NULL;
    }
    AG.permanentCache[index].tableType = tableType;
    AG.permanentCache[index].id = id;
    AG.permanentCache[index].size = size;

    //! @bug UB: missing return. "ramAddr" is in v0 at this point, but doing an
    //! explicit return uses an additional register.
#ifdef AVOID_UB
    return ramAddr;
#endif
}

void* Nas_Alloc_Single(u32 size, s32 fontId, void* sampleAddr, s8 medium, s32 cache) {
    SampleCacheEntry* entry;

    if (cache == CACHE_TEMPORARY) {
        entry = __Nas_Alloc_Single_Auto_Inner(size);
    } else {
        entry = __Nas_Alloc_Single_Stay_Inner(size);
    }
    if (entry != NULL) {
        //! @bug Should use sampleBankId, not fontId
        entry->sampleBankId = fontId;
        entry->sampleAddr = sampleAddr;
        entry->origMedium = medium;
        return entry->allocatedAddr;
    }
    return NULL;
}

/**
 * Initializes the persistent and temporary caches used for individual samples. Will attempt to use heap space available
 * on the external pool. If no external pool is provided, then default to using space on the misc pool.
 */
void Nas_Init_Single(u32 persistentSampleCacheSize, u32 temporarySampleCacheSize) {
    void* ramAddr;

    ramAddr = Nas_2ndHeapAlloc(&AG.miscPool, persistentSampleCacheSize);
    if (ramAddr == NULL) {
        AG.persistentSampleCache.pool.size = 0;
    } else {
        Nas_HeapInit(&AG.persistentSampleCache.pool, ramAddr, persistentSampleCacheSize);
    }
    ramAddr = Nas_2ndHeapAlloc(&AG.miscPool, temporarySampleCacheSize);
    if (ramAddr == NULL) {
        AG.temporarySampleCache.pool.size = 0;
    } else {
        Nas_HeapInit(&AG.temporarySampleCache.pool, ramAddr, temporarySampleCacheSize);
    }
    AG.persistentSampleCache.numEntries = 0;
    AG.temporarySampleCache.numEntries = 0;
}

SampleCacheEntry* __Nas_Alloc_Single_Auto_Inner(u32 size) {
    u8* allocAfter;
    u8* allocBefore;
    void* ramAddr;
    s32 index;
    s32 i;
    SampleCacheEntry* entry;
    AudioPreloadReq* preload;
    AudioSampleCache* cache;
    u8* startRamAddr;
    u8* endRamAddr;

    cache = &AG.temporarySampleCache;
    allocBefore = cache->pool.curRamAddr;
    ramAddr = Nas_HeapAlloc(&cache->pool, size);
    if (ramAddr == NULL) {
        // Reset the pool and try again. We still keep pointers to within the
        // pool, so we have to be careful to discard existing overlapping
        // allocations further down.
        u8* old = cache->pool.curRamAddr;

        cache->pool.curRamAddr = cache->pool.startRamAddr;
        ramAddr = Nas_HeapAlloc(&cache->pool, size);
        if (ramAddr == NULL) {
            cache->pool.curRamAddr = old;
            return NULL;
        }
        allocBefore = cache->pool.startRamAddr;
    }

    allocAfter = cache->pool.curRamAddr;

    index = -1;
    for (i = 0; i < AG.preloadSampleStackTop; i++) {
        preload = &AG.preloadSampleStack[i];
        if (!preload->isFree) {
            startRamAddr = preload->ramAddr;
            endRamAddr = preload->ramAddr + preload->sample->size - 1;

            if (endRamAddr < allocBefore && startRamAddr < allocBefore) {
                continue;
            }
            if (endRamAddr >= allocAfter && startRamAddr >= allocAfter) {
                continue;
            }

            // Overlap, skip this preload.
            preload->isFree = true;
        }
    }

    for (i = 0; i < cache->numEntries; i++) {
        if (!cache->entries[i].inUse) {
            continue;
        }

        startRamAddr = cache->entries[i].allocatedAddr;
        endRamAddr = startRamAddr + cache->entries[i].size - 1;

        if (endRamAddr < allocBefore && startRamAddr < allocBefore) {
            continue;
        }
        if (endRamAddr >= allocAfter && startRamAddr >= allocAfter) {
            continue;
        }

        // Overlap, discard existing entry.
        __KillSwMember(&cache->entries[i]);
        if (index == -1) {
            index = i;
        }
    }

    if (index == -1) {
        index = cache->numEntries++;
    }

    entry = &cache->entries[index];
    entry->inUse = true;
    entry->allocatedAddr = ramAddr;
    entry->size = size;

    return entry;
}

void __SearchBank(SampleCacheEntry* entry, s32 fontId) {
    Drum* drum;
    Instrument* inst;
    SoundEffect* soundEffect;
    s32 instId;
    s32 drumId;
    s32 sfxId;

    for (instId = 0; instId < AG.soundFontList[fontId].numInstruments; instId++) {
        inst = ProgToVp(fontId, instId);
        if (inst != NULL) {
            if (inst->normalRangeLo != 0) {
                __RomAddrSet(entry, inst->lowPitchTunedSample.sample);
            }
            if (inst->normalRangeHi != 0x7F) {
                __RomAddrSet(entry, inst->highPitchTunedSample.sample);
            }
            __RomAddrSet(entry, inst->normalPitchTunedSample.sample);
        }
    }

    for (drumId = 0; drumId < AG.soundFontList[fontId].numDrums; drumId++) {
        drum = PercToPp(fontId, drumId);
        if (drum != NULL) {
            __RomAddrSet(entry, drum->tunedSample.sample);
        }
    }

    for (sfxId = 0; sfxId < AG.soundFontList[fontId].numSfx; sfxId++) {
        soundEffect = VpercToVep(fontId, sfxId);
        if (soundEffect != NULL) {
            __RomAddrSet(entry, soundEffect->tunedSample.sample);
        }
    }
}

void __KillSwMember(SampleCacheEntry* entry) {
    s32 numFonts;
    s32 sampleBankId1;
    s32 sampleBankId2;
    s32 fontId;

    numFonts = AG.soundFontTable->header.numEntries;
    for (fontId = 0; fontId < numFonts; fontId++) {
        sampleBankId1 = AG.soundFontList[fontId].sampleBankId1;
        sampleBankId2 = AG.soundFontList[fontId].sampleBankId2;
        if (((sampleBankId1 != 0xFF) && (entry->sampleBankId == sampleBankId1)) ||
            ((sampleBankId2 != 0xFF) && (entry->sampleBankId == sampleBankId2)) || entry->sampleBankId == 0) {
            if (Nas_SzCacheCheck(FONT_TABLE, CACHE_EITHER, fontId) != NULL) {
                if (Nas_CheckIDbank(fontId) != 0) {
                    __SearchBank(entry, fontId);
                }
            }
        }
    }
}

void __RomAddrSet(SampleCacheEntry* entry, Sample* sample) {
    if (sample != NULL) {
        if (sample->sampleAddr == entry->allocatedAddr) {
            sample->sampleAddr = entry->sampleAddr;
            sample->medium = entry->origMedium;
        }
    }
}

SampleCacheEntry* __Nas_Alloc_Single_Stay_Inner(u32 size) {
    AudioSampleCache* cache;
    SampleCacheEntry* entry;
    void* ramAddr;

    cache = &AG.persistentSampleCache;
    ramAddr = Nas_HeapAlloc(&cache->pool, size);
    if (ramAddr == NULL) {
        return NULL;
    }
    entry = &cache->entries[cache->numEntries];
    entry->inUse = true;
    entry->allocatedAddr = ramAddr;
    entry->size = size;
    cache->numEntries++;

    return entry;
}

void __Do_EmemKill(SampleCacheEntry* entry, s32 sampleBankId1, s32 sampleBankId2, s32 fontId) {
    if ((entry->sampleBankId == sampleBankId1) || (entry->sampleBankId == sampleBankId2) ||
        (entry->sampleBankId == 0)) {
        __SearchBank(entry, fontId);
    }
}

void Emem_KillSwMember(void) {
    s32 numFonts;
    s32 sampleBankId1;
    s32 sampleBankId2;
    s32 fontId;
    s32 j;

    numFonts = AG.soundFontTable->header.numEntries;
    for (fontId = 0; fontId < numFonts; fontId++) {
        sampleBankId1 = AG.soundFontList[fontId].sampleBankId1;
        sampleBankId2 = AG.soundFontList[fontId].sampleBankId2;
        if ((sampleBankId1 == 0xFF) && (sampleBankId2 == 0xFF)) {
            continue;
        }
        if (Nas_SzCacheCheck(FONT_TABLE, CACHE_PERMANENT, fontId) == NULL ||
            !Nas_CheckIDbank(fontId)) {
            continue;
        }

        for (j = 0; j < AG.persistentSampleCache.numEntries; j++) {
            __Do_EmemKill(&AG.persistentSampleCache.entries[j], sampleBankId1,
                                                sampleBankId2, fontId);
        }
        for (j = 0; j < AG.temporarySampleCache.numEntries; j++) {
            __Do_EmemKill(&AG.temporarySampleCache.entries[j], sampleBankId1,
                                                sampleBankId2, fontId);
        }
    }
}

typedef struct StorageChange {
    u32 oldAddr;
    u32 newAddr;
    u32 size;
    u8 newMedium;
} StorageChange;

void __RestoreAddr(StorageChange* change, Sample* sample) {
    if (sample != NULL) {
        u32 startAddr = change->oldAddr;
        u32 endAddr = change->oldAddr + change->size;

        if (((u32)sample->sampleAddr >= startAddr) && ((u32)sample->sampleAddr < endAddr)) {
            sample->sampleAddr += -startAddr + change->newAddr;
            sample->medium = change->newMedium;
        }
    }
}

void __ExchangeWave(s32 apply, s32 sampleBankId);

void DirtyWave(s32 sampleBankId) {
    __ExchangeWave(false, sampleBankId);
}

void EntryWave(s32 sampleBankId) {
    __ExchangeWave(true, sampleBankId);
}

void __ExchangeWave(s32 apply, s32 sampleBankId) {
    AudioTable* sampleBankTable;
    AudioTableEntry* entry;
    s32 numFonts;
    s32 instId;
    s32 drumId;
    s32 sfxId;
    StorageChange change;
    s32 sampleBankId1;
    s32 sampleBankId2;
    s32 fontId;
    Drum* drum;
    Instrument* inst;
    SoundEffect* soundEffect;
    u32* fakematch;
    s32 pad[4];

    sampleBankTable = AG.sampleBankTable;
    numFonts = AG.soundFontTable->header.numEntries;
    change.oldAddr = (u32)Nas_SzCacheCheck(SAMPLE_TABLE, CACHE_EITHER, sampleBankId);
    if (change.oldAddr == 0) {
        return;
    }

    entry = &sampleBankTable->entries[sampleBankId];
    change.size = entry->size;
    change.newMedium = entry->medium;

    if ((change.newMedium == MEDIUM_CART) || (change.newMedium == MEDIUM_DISK_DRIVE)) {
        change.newAddr = entry->romAddr;
    } else {
        change.newAddr = 0;
    }

    fakematch = &change.oldAddr;
    if (apply && (apply == true)) {
        u32 temp = change.newAddr;

        change.newAddr = *fakematch; // = change.oldAddr
        change.oldAddr = temp;
        change.newMedium = MEDIUM_RAM;
    }

    for (fontId = 0; fontId < numFonts; fontId++) {
        sampleBankId1 = AG.soundFontList[fontId].sampleBankId1;
        sampleBankId2 = AG.soundFontList[fontId].sampleBankId2;
        if ((sampleBankId1 != 0xFF) || (sampleBankId2 != 0xFF)) {
            if (!Nas_CheckIDbank(fontId) ||
                Nas_SzCacheCheck(FONT_TABLE, CACHE_EITHER, fontId) == NULL) {
                continue;
            }

            if (sampleBankId1 == sampleBankId) {
            } else if (sampleBankId2 == sampleBankId) {
            } else {
                continue;
            }

            for (instId = 0; instId < AG.soundFontList[fontId].numInstruments; instId++) {
                inst = ProgToVp(fontId, instId);
                if (inst != NULL) {
                    if (inst->normalRangeLo != 0) {
                        __RestoreAddr(&change, inst->lowPitchTunedSample.sample);
                    }
                    if (inst->normalRangeHi != 0x7F) {
                        __RestoreAddr(&change, inst->highPitchTunedSample.sample);
                    }
                    __RestoreAddr(&change, inst->normalPitchTunedSample.sample);
                }
            }

            for (drumId = 0; drumId < AG.soundFontList[fontId].numDrums; drumId++) {
                drum = PercToPp(fontId, drumId);
                if (drum != NULL) {
                    __RestoreAddr(&change, drum->tunedSample.sample);
                }
            }

            for (sfxId = 0; sfxId < AG.soundFontList[fontId].numSfx; sfxId++) {
                soundEffect = VpercToVep(fontId, sfxId);
                if (soundEffect != NULL) {
                    __RestoreAddr(&change, soundEffect->tunedSample.sample);
                }
            }
        }
    }
}

void Dirty_AllWave(void) {
    AudioCache* cache;
    AudioPersistentCache* persistent;
    AudioTemporaryCache* temporary;
    u32 i;

    cache = &AG.sampleBankCache;
    temporary = &cache->temporary;

    if (temporary->entries[0].id != -1) {
        DirtyWave(temporary->entries[0].id);
    }

    if (temporary->entries[1].id != -1) {
        DirtyWave(temporary->entries[1].id);
    }

    persistent = &cache->persistent;
    for (i = 0; i < persistent->numEntries; i++) {
        DirtyWave(persistent->entries[i].id);
    }
}
