#include "ultra64.h"
#include "global.h"
#include "terminal.h"

typedef struct SfxRequest {
    /* 0x00 */ u16 sfxId;
    /* 0x04 */ Vec3f* pos;
    /* 0x08 */ u8 token;
    /* 0x0C */ f32* freqScale;
    /* 0x10 */ f32* vol;
    /* 0x14 */ s8* reverbAdd;
} SfxRequest; // size = 0x18

typedef struct UnusedBankLerp {
    /* 0x00 */ f32 value;
    /* 0x04 */ f32 target;
    /* 0x08 */ f32 step;
    /* 0x0C */ u16 remainingFrames;
} UnusedBankLerp; // size = 0x10

SfxBankEntry EntrySeHandle_0[9];
SfxBankEntry EntrySeHandle_1[12];
SfxBankEntry EntrySeHandle_2[22];
SfxBankEntry EntrySeHandle_3[20];
SfxBankEntry EntrySeHandle_4[8];
SfxBankEntry EntrySeHandle_5[3];
SfxBankEntry EntrySeHandle_6[5];
SfxRequest EntrySeBuffer[0x100];
u8 StatEntryPoint[7];
u8 StatFreePoint[7];
u8 now_play_sounds[7];
ActiveSfx play_next[7][3];
u8 now_check_sub;
u8 se_handle_lock_flag[7];
UnusedBankLerp FxHandleVol[7];

#if DEBUG_FEATURES
u16 gAudioSfxSwapSource[10];
u16 gAudioSfxSwapTarget[10];
u8 gAudioSfxSwapMode[10];
#endif

void Nai_FxSetLockFlag(u16 muteMask) {
    u8 bankId;

    for (bankId = 0; bankId < ARRAY_COUNT(EntrySeStatus); bankId++) {
        if (muteMask & 1) {
            se_handle_lock_flag[bankId] = true;
        } else {
            se_handle_lock_flag[bankId] = false;
        }
        muteMask = muteMask >> 1;
    }
}

void Nai_FxSetBgmMuteFlag(u8 channelIndex) {
    se_bgm_mute_flag |= (1 << channelIndex);
    Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_SFX, 0x40, 0xF);
    Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_SFX, 0x40, 0xF);
}

void Nai_FxClearBgmMuteFlag(u8 channelIndex) {
    se_bgm_mute_flag &= ((1 << channelIndex) ^ 0xFFFF);
    if (se_bgm_mute_flag == 0) {
        Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_SFX, 0x7F, 0xF);
        Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_SFX, 0x7F, 0xF);
    }
}

void Nai_FxFlagEntry(u16 sfxId, Vec3f* pos, u8 token, f32* freqScale, f32* vol, s8* reverbAdd) {
    u8 i;
    SfxRequest* req;

    if (!se_handle_lock_flag[SFX_BANK_SHIFT(sfxId)]) {
        req = &EntrySeBuffer[_se_ent_writepoint];

#if DEBUG_FEATURES
        if (!gAudioSfxSwapOff) {
            for (i = 0; i < 10; i++) {
                if (sfxId == gAudioSfxSwapSource[i]) {
                    if (gAudioSfxSwapMode[i] == 0) { // "SWAP"
                        sfxId = gAudioSfxSwapTarget[i];
                    } else { // "ADD"
                        req->sfxId = gAudioSfxSwapTarget[i];
                        req->pos = pos;
                        req->token = token;
                        req->freqScale = freqScale;
                        req->vol = vol;
                        req->reverbAdd = reverbAdd;
                        _se_ent_writepoint++;
                        req = &EntrySeBuffer[_se_ent_writepoint];
                    }
                    i = 10; // "break;"
                }
            }
        }
#endif

        req->sfxId = sfxId;
        req->pos = pos;
        req->token = token;
        req->freqScale = freqScale;
        req->vol = vol;
        req->reverbAdd = reverbAdd;
        _se_ent_writepoint++;
    }
}

void Nai_StopBufferFx(u8 aspect, SfxBankEntry* cmp) {
    SfxRequest* req;
    s32 remove;
    u8 i = _se_ent_readpoint;

    for (; i != _se_ent_writepoint; i++) {
        remove = false;
        req = &EntrySeBuffer[i];
        switch (aspect) {
            case 0:
                if (SFX_BANK_MASK(req->sfxId) == SFX_BANK_MASK(cmp->sfxId)) {
                    remove = true;
                }
                break;
            case 1:
                if (SFX_BANK_MASK(req->sfxId) == SFX_BANK_MASK(cmp->sfxId) && (&req->pos->x == cmp->posX)) {
                    remove = true;
                }
                break;
            case 2:
                if (&req->pos->x == cmp->posX) {
                    remove = true;
                }
                break;
            case 3:
                if (&req->pos->x == cmp->posX && req->sfxId == cmp->sfxId) {
                    remove = true;
                }
                break;
            case 4:
                if (req->token == cmp->token && req->sfxId == cmp->sfxId) {
                    remove = true;
                }
                break;
            case 5:
                if (req->sfxId == cmp->sfxId) {
                    remove = true;
                }
                break;
        }
        if (remove) {
            req->sfxId = NA_SE_NONE;
        }
    }
}

void Nai_EntryBufferFx(void) {
    u16 sfxId;
    u8 count;
    u8 index;
    SfxRequest* req;
    SfxBankEntry* entry;
    SfxParams* sfxParams;
    s32 bankId;
    u8 evictImportance;
    u8 evictIndex;

    req = &EntrySeBuffer[_se_ent_readpoint];
    evictIndex = 0x80;

    if (req->sfxId == NA_SE_NONE) {
        return;
    }

    bankId = SFX_BANK(req->sfxId);

#if DEBUG_FEATURES
    if ((1 << bankId) & D_801333F0) {
        Nad_GameScrollPrint("SE", req->sfxId);
        bankId = SFX_BANK(req->sfxId);
    }
#endif

    count = 0;
    index = EntrySeStatus[bankId][0].next;
    while (index != 0xFF && index != 0) {
        if (EntrySeStatus[bankId][index].posX == &req->pos->x) {
            if ((se_indi_info_adrs[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)].params & SFX_FLAG_5) &&
                se_indi_info_adrs[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)].importance ==
                    EntrySeStatus[bankId][index].sfxImportance) {
                return;
            }
            if (EntrySeStatus[bankId][index].sfxId == req->sfxId) {
                count = _se_handle_objsounds[se_scene_mode][bankId];
            } else {
                if (count == 0) {
                    evictIndex = index;
                    sfxId = EntrySeStatus[bankId][index].sfxId & 0xFFFF;
                    evictImportance = se_indi_info_adrs[SFX_BANK_SHIFT(sfxId)][SFX_INDEX(sfxId)].importance;
                } else if (EntrySeStatus[bankId][index].sfxImportance < evictImportance) {
                    evictIndex = index;
                    sfxId = EntrySeStatus[bankId][index].sfxId & 0xFFFF;
                    evictImportance = se_indi_info_adrs[SFX_BANK_SHIFT(sfxId)][SFX_INDEX(sfxId)].importance;
                }
                count++;
                if (count == _se_handle_objsounds[se_scene_mode][bankId]) {
                    if (se_indi_info_adrs[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)].importance >= evictImportance) {
                        index = evictIndex;
                    } else {
                        index = 0;
                    }
                }
            }
            if (count == _se_handle_objsounds[se_scene_mode][bankId]) {
                sfxParams = &se_indi_info_adrs[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)];
                if ((req->sfxId & 0xC00) || (sfxParams->params & SFX_FLAG_2) || (index == evictIndex)) {
                    if ((EntrySeStatus[bankId][index].sfxParams & SFX_FLAG_3) &&
                        EntrySeStatus[bankId][index].state != SFX_STATE_QUEUED) {
                        Nai_FxClearBgmMuteFlag(EntrySeStatus[bankId][index].channelIndex);
                    }
                    EntrySeStatus[bankId][index].token = req->token;
                    EntrySeStatus[bankId][index].sfxId = req->sfxId;
                    EntrySeStatus[bankId][index].state = SFX_STATE_QUEUED;
                    EntrySeStatus[bankId][index].freshness = 2;
                    EntrySeStatus[bankId][index].freqScale = req->freqScale;
                    EntrySeStatus[bankId][index].vol = req->vol;
                    EntrySeStatus[bankId][index].reverbAdd = req->reverbAdd;
                    EntrySeStatus[bankId][index].sfxParams = sfxParams->params;
                    EntrySeStatus[bankId][index].sfxImportance = sfxParams->importance;
                } else if (EntrySeStatus[bankId][index].state == SFX_STATE_PLAYING_2) {
                    EntrySeStatus[bankId][index].state = SFX_STATE_PLAYING_1;
                }
                index = 0;
            }
        }
        if (index != 0) {
            index = EntrySeStatus[bankId][index].next;
        }
    }
    if (EntrySeStatus[bankId][StatFreePoint[bankId]].next != 0xFF && index != 0) {
        index = StatFreePoint[bankId];
        entry = &EntrySeStatus[bankId][index];
        entry->posX = &req->pos->x;
        entry->posY = &req->pos->y;
        entry->posZ = &req->pos->z;
        entry->token = req->token;
        entry->freqScale = req->freqScale;
        entry->vol = req->vol;
        entry->reverbAdd = req->reverbAdd;
        sfxParams = &se_indi_info_adrs[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)];
        entry->sfxParams = sfxParams->params;
        entry->sfxImportance = sfxParams->importance;
        entry->sfxId = req->sfxId;
        entry->state = SFX_STATE_QUEUED;
        entry->freshness = 2;
        entry->prev = StatEntryPoint[bankId];
        EntrySeStatus[bankId][StatEntryPoint[bankId]].next = StatFreePoint[bankId];
        StatEntryPoint[bankId] = StatFreePoint[bankId];
        StatFreePoint[bankId] = EntrySeStatus[bankId][StatFreePoint[bankId]].next;
        EntrySeStatus[bankId][StatFreePoint[bankId]].prev = 0xFF;
        entry->next = 0xFF;
    }
}

void Nai_FxFlagFree(u8 bankId, u8 entryIndex) {
    SfxBankEntry* entry = &EntrySeStatus[bankId][entryIndex];
    u8 i;

    if (entry->sfxParams & SFX_FLAG_3) {
        Nai_FxClearBgmMuteFlag(entry->channelIndex);
    }
    if (entryIndex == StatEntryPoint[bankId]) {
        StatEntryPoint[bankId] = entry->prev;
    } else {
        EntrySeStatus[bankId][entry->next].prev = entry->prev;
    }
    EntrySeStatus[bankId][entry->prev].next = entry->next;
    entry->next = StatFreePoint[bankId];
    entry->prev = 0xFF;
    EntrySeStatus[bankId][StatFreePoint[bankId]].prev = entryIndex;
    StatFreePoint[bankId] = entryIndex;
    entry->state = SFX_STATE_EMPTY;

    for (i = 0; i < _se_handle_sounds[se_scene_mode][bankId]; i++) {
        if (play_next[bankId][i].entryIndex == entryIndex) {
            play_next[bankId][i].entryIndex = 0xFF;
            i = _se_handle_sounds[se_scene_mode][bankId];
        }
    }
}

void Nai_FxNextPlayCheck(u8 bankId) {
    u8 numChosenSfx;
    u8 numChannels;
    u8 entryIndex;
    u8 i;
    u8 j;
    u8 k;
    u8 sfxImportance;
    u8 needNewSfx;
    u8 chosenEntryIndex;
    u16 temp3;
    f32 tempf1;
    SfxBankEntry* entry;
    ActiveSfx chosenSfx[MAX_CHANNELS_PER_BANK];
    ActiveSfx* activeSfx;
    s32 pad;

    numChosenSfx = 0;
    for (i = 0; i < MAX_CHANNELS_PER_BANK; i++) {
        chosenSfx[i].priority = 0x7FFFFFFF;
        chosenSfx[i].entryIndex = 0xFF;
    }
    entryIndex = EntrySeStatus[bankId][0].next;
    k = 0;
    while (entryIndex != 0xFF) {
        if ((EntrySeStatus[bankId][entryIndex].state == SFX_STATE_QUEUED) &&
            (EntrySeStatus[bankId][entryIndex].sfxId & 0xC00)) {
            EntrySeStatus[bankId][entryIndex].freshness--;
        } else if (!(EntrySeStatus[bankId][entryIndex].sfxId & 0xC00) &&
                   (EntrySeStatus[bankId][entryIndex].state == SFX_STATE_PLAYING_2)) {
            AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, EntrySeStatus[bankId][entryIndex].channelIndex, 0, 0);
            Nai_FxFlagFree(bankId, entryIndex);
        }
        if (EntrySeStatus[bankId][entryIndex].freshness == 0) {
            Nai_FxFlagFree(bankId, entryIndex);
        } else if (EntrySeStatus[bankId][entryIndex].state != SFX_STATE_EMPTY) {
            entry = &EntrySeStatus[bankId][entryIndex];

            if (&_dummy_zero_f.x == entry[0].posX) {
                entry->dist = 0.0f;
            } else {
                tempf1 = *entry->posY * 1;
                entry->dist = (SQ(*entry->posX) + SQ(tempf1) + SQ(*entry->posZ)) / SFX_DIST_SCALING;
            }
            sfxImportance = entry->sfxImportance;
            if (entry->sfxParams & SFX_FLAG_4) {
                entry->priority = SQ(0xFF - sfxImportance) * SQ(76);
            } else {
                if (entry->dist > 0x7FFFFFD0) {
                    entry->dist = 0x70000008;
                    PRINTF(VT_COL(RED, WHITE) "<INAGAKI CHECK> dist over! "
                                              "flag:%04X ptr:%08X pos:%f-%f-%f" VT_RST "\n",
                           entry->sfxId, entry->posX, entry->posZ, *entry->posX, *entry->posY, *entry->posZ);
                }
                entry->priority = (u32)entry->dist + (SQ(0xFF - sfxImportance) * SQ(76));
#if OOT_VERSION < NTSC_1_1 || !PLATFORM_N64
                temp3 = entry->sfxId; // fake
                entry->priority = entry->priority + temp3 - temp3;
#endif
                if (*entry->posZ < 0.0f) {
                    entry->priority += (s32)(-*entry->posZ * 6.0f);
                }
            }
            if (entry->dist > SQ(1e5f)) {
                if (entry->state == SFX_STATE_PLAYING_1) {
                    AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, entry->channelIndex, 0, 0);
                    if (entry->sfxId & 0xC00) {
                        Nai_FxFlagFree(bankId, entryIndex);
                        entryIndex = k;
                    }
                }
            } else {
                numChannels = _se_handle_sounds[se_scene_mode][bankId];
                for (i = 0; i < numChannels; i++) {
                    if (chosenSfx[i].priority >= entry->priority) {
                        if (numChosenSfx < _se_handle_sounds[se_scene_mode][bankId]) {
                            numChosenSfx++;
                        }
                        for (j = numChannels - 1; j > i; j--) {
                            chosenSfx[j].priority = chosenSfx[j - 1].priority;
                            chosenSfx[j].entryIndex = chosenSfx[j - 1].entryIndex;
                        }
                        chosenSfx[i].priority = entry->priority;
                        chosenSfx[i].entryIndex = entryIndex;
                        i = numChannels; // "break;"
                    }
                }
            }
            k = entryIndex;
        }
        entryIndex = EntrySeStatus[bankId][k].next;
    }
    for (i = 0; i < numChosenSfx; i++) {
        entry = &EntrySeStatus[bankId][chosenSfx[i].entryIndex];
        if (entry->state == SFX_STATE_QUEUED) {
            entry->state = SFX_STATE_READY;
        } else if (entry->state == SFX_STATE_PLAYING_1) {
            entry->state = SFX_STATE_PLAYING_REFRESH;
        }
    }

    // Pick something to play for all channels.
    numChannels = _se_handle_sounds[se_scene_mode][bankId];
    for (i = 0; i < numChannels; i++) {
        needNewSfx = false;
        activeSfx = &play_next[bankId][i];

        if (activeSfx->entryIndex == 0xFF) {
            needNewSfx = true;
        } else {
            entry = &EntrySeStatus[bankId][activeSfx[0].entryIndex];
            if (entry->state == SFX_STATE_PLAYING_1) {
                if (entry->sfxId & 0xC00) {
                    Nai_FxFlagFree(bankId, activeSfx->entryIndex);
                } else {
                    entry->state = SFX_STATE_QUEUED;
                }
                needNewSfx = true;
            } else if (entry->state == SFX_STATE_EMPTY) {
                activeSfx->entryIndex = 0xFF;
                needNewSfx = true;
            } else {
                // Sfx is already playing as it should, nothing to do.
                for (j = 0; j < numChannels; j++) {
                    if (activeSfx->entryIndex == chosenSfx[j].entryIndex) {
                        chosenSfx[j].entryIndex = 0xFF;
                        j = numChannels;
                    }
                }
                numChosenSfx--;
            }
        }

        if (needNewSfx == true) {
            for (j = 0; j < numChannels; j++) {
                chosenEntryIndex = chosenSfx[j].entryIndex;
                if ((chosenEntryIndex != 0xFF) &&
                    (EntrySeStatus[bankId][chosenEntryIndex].state != SFX_STATE_PLAYING_REFRESH)) {
                    for (k = 0; k < numChannels; k++) {
                        if (chosenEntryIndex == play_next[bankId][k].entryIndex) {
                            needNewSfx = false;
                            k = numChannels; // "break;"
                        }
                    }
                    if (needNewSfx == true) {
                        activeSfx->entryIndex = chosenEntryIndex;
                        chosenSfx[j].entryIndex = 0xFF;
                        j = numChannels + 1;
                        numChosenSfx--;
                    }
                }
            }
            if (j == numChannels) {
                // nothing found
                activeSfx->entryIndex = 0xFF;
            }
        }
    }
}

void Nai_FxNowPlayCheck(u8 bankId) {
    u8 entryIndex;
    SequenceChannel* channel;
    SfxBankEntry* entry;
    u8 i;

    for (i = 0; i < _se_handle_sounds[se_scene_mode][bankId]; i++) {
        entryIndex = play_next[bankId][i].entryIndex;
        if (entryIndex != 0xFF) {
            entry = &EntrySeStatus[bankId][entryIndex];
            channel = AG.seqPlayers[SEQ_PLAYER_SFX].channels[now_check_sub];
            if (entry->state == SFX_STATE_READY) {
                entry->channelIndex = now_check_sub;
                if (entry->sfxParams & SFX_FLAG_3) {
                    Nai_FxSetBgmMuteFlag(now_check_sub);
                }
                if ((entry->sfxParams & SFX_PARAM_67_MASK) != (0 << SFX_PARAM_67_SHIFT)) {
                    switch (entry->sfxParams & SFX_PARAM_67_MASK) {
                        case (1 << SFX_PARAM_67_SHIFT):
                            entry->unk_2F = Nap_GetRandom() & 0xF;
                            break;
                        case (2 << SFX_PARAM_67_SHIFT):
                            entry->unk_2F = Nap_GetRandom() & 0x1F;
                            break;
                        case (3 << SFX_PARAM_67_SHIFT):
                            entry->unk_2F = Nap_GetRandom() & 0x3F;
                            break;
                        default:
                            entry->unk_2F = 0;
                            break;
                    }
                }
                Naa_PlaySeParaSet(bankId, entryIndex, now_check_sub);

                // ioPort 0, enable the sfx to play in `NA_BGM_GENERAL_SFX`
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, now_check_sub, 0, 1);

                // ioPort 4, write the lower bits sfx index for `NA_BGM_GENERAL_SFX` to find the right code to execute
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, now_check_sub, 4, entry->sfxId & 0xFF);

                // If the sfx bank has more than 255 entries (greater than a u8 can store),
                // then store the Id in upper and lower bits
                if (_se_handle_extend_senum[bankId]) {
                    // ioPort 5, write the upper bits sfx index for `NA_BGM_GENERAL_SFX`, for banks with > 0xFF entries
                    AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, now_check_sub, 5, (entry->sfxId & 0x100) >> 8);
                }
                if (entry->sfxId & 0xC00) {
                    entry->state = SFX_STATE_PLAYING_1;
                } else {
                    entry->state = SFX_STATE_PLAYING_2;
                }
            } else if ((u8)channel->seqScriptIO[1] == (u8)SEQ_IO_VAL_NONE) {
                Nai_FxFlagFree(bankId, entryIndex);
            } else if (entry->state == SFX_STATE_PLAYING_REFRESH) {
                Naa_PlaySeParaSet(bankId, entryIndex, now_check_sub);
                if (entry->sfxId & 0xC00) {
                    entry->state = SFX_STATE_PLAYING_1;
                } else {
                    entry->state = SFX_STATE_PLAYING_2;
                }
            }
        }
        now_check_sub++;
    }
}

void Nai_StopAllHandleFx(u8 bankId) {
    SfxBankEntry* entry;
    s32 pad;
    SfxBankEntry cmp;
    u8 entryIndex = EntrySeStatus[bankId][0].next;

    while (entryIndex != 0xFF) {
        entry = &EntrySeStatus[bankId][entryIndex];
        if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
            AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, entry->channelIndex, 0, 0);
        }
        if (entry->state != SFX_STATE_EMPTY) {
            Nai_FxFlagFree(bankId, entryIndex);
        }
        entryIndex = EntrySeStatus[bankId][0].next;
    }
    cmp.sfxId = bankId << 12;
    Nai_StopBufferFx(0, &cmp);
}

void Nai_StopAllObjHandleFx2(u8 bankId, Vec3f* pos) {
    SfxBankEntry* entry;
    u8 entryIndex = EntrySeStatus[bankId][0].next;
    u8 prevEntryIndex = 0;

    while (entryIndex != 0xFF) {
        entry = &EntrySeStatus[bankId][entryIndex];
        if (entry->posX == &pos->x) {
            if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, entry->channelIndex, 0, 0);
            }
            if (entry->state != SFX_STATE_EMPTY) {
                Nai_FxFlagFree(bankId, entryIndex);
            }
        } else {
            prevEntryIndex = entryIndex;
        }
        entryIndex = EntrySeStatus[bankId][prevEntryIndex].next;
    }
}

void Nai_StopAllObjHandleFx(u8 bankId, Vec3f* pos) {
    SfxBankEntry cmp;

    Nai_StopAllObjHandleFx2(bankId, pos);
    cmp.sfxId = bankId << 12;
    cmp.posX = &pos->x;
    Nai_StopBufferFx(1, &cmp);
}

void Nai_StopAllObjFx(Vec3f* pos) {
    u8 i;
    SfxBankEntry cmp;

    for (i = 0; i < ARRAY_COUNT(EntrySeStatus); i++) {
        Nai_StopAllObjHandleFx2(i, pos);
    }
    cmp.posX = &pos->x;
    Nai_StopBufferFx(2, &cmp);
}

void Nai_StopObjFlagFx(Vec3f* pos, u16 sfxId) {
    SfxBankEntry* entry;
    u8 entryIndex = EntrySeStatus[SFX_BANK(sfxId)][0].next;
    u8 prevEntryIndex = 0;
    SfxBankEntry cmp;

    while (entryIndex != 0xFF) {
        entry = &EntrySeStatus[SFX_BANK(sfxId)][entryIndex];
        if (entry->posX == &pos->x && entry->sfxId == sfxId) {
            if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, entry->channelIndex, 0, 0);
            }
            if (entry->state != SFX_STATE_EMPTY) {
                Nai_FxFlagFree(SFX_BANK(sfxId), entryIndex);
            }
            entryIndex = 0xFF;
        } else {
            prevEntryIndex = entryIndex;
        }
        if (entryIndex != 0xFF) {
            entryIndex = EntrySeStatus[SFX_BANK(sfxId)][prevEntryIndex].next;
        }
    }
    cmp.posX = &pos->x;
    cmp.sfxId = sfxId;
    Nai_StopBufferFx(3, &cmp);
}

void Nai_StopPlayerFlagFx(u8 token, u16 sfxId) {
    SfxBankEntry* entry;
    u8 entryIndex = EntrySeStatus[SFX_BANK(sfxId)][0].next;
    u8 prevEntryIndex = 0;
    SfxBankEntry cmp;

    while (entryIndex != 0xFF) {
        entry = &EntrySeStatus[SFX_BANK(sfxId)][entryIndex];
        if (entry->token == token && entry->sfxId == sfxId) {
            if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, entry->channelIndex, 0, 0);
            }
            if (entry->state != SFX_STATE_EMPTY) {
                Nai_FxFlagFree(SFX_BANK(sfxId), entryIndex);
            }
        } else {
            prevEntryIndex = entryIndex;
        }
        if (entryIndex != 0xFF) {
            entryIndex = EntrySeStatus[SFX_BANK(sfxId)][prevEntryIndex].next;
        }
    }
    cmp.token = token;
    cmp.sfxId = sfxId;
    Nai_StopBufferFx(4, &cmp);
}

void Nai_StopFx(u32 sfxId) {
    SfxBankEntry* entry;
    u8 entryIndex = EntrySeStatus[SFX_BANK(sfxId)][0].next;
    u8 prevEntryIndex = 0;
    SfxBankEntry cmp;

    while (entryIndex != 0xFF) {
        entry = &EntrySeStatus[SFX_BANK(sfxId)][entryIndex];
        if (entry->sfxId == sfxId) {
            if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, entry->channelIndex, 0, 0);
            }
            if (entry->state != SFX_STATE_EMPTY) {
                Nai_FxFlagFree(SFX_BANK(sfxId), entryIndex);
            }
        } else {
            prevEntryIndex = entryIndex;
        }
        entryIndex = EntrySeStatus[SFX_BANK(sfxId)][prevEntryIndex].next;
    }
    cmp.sfxId = sfxId;
    Nai_StopBufferFx(5, &cmp);
}

void Nai_FxBufFlagCheck(void) {
    while (_se_ent_writepoint != _se_ent_readpoint) {
        Nai_EntryBufferFx();
        _se_ent_readpoint++;
    }
}

void Nai_SetFxHandleVolume(u8 bankId, u8 target, u16 delay) {
    if (delay == 0) {
        delay++;
    }
    FxHandleVol[bankId].target = target / 127.0f;
    FxHandleVol[bankId].remainingFrames = delay;
    FxHandleVol[bankId].step = ((FxHandleVol[bankId].value - FxHandleVol[bankId].target) / delay);
}

void Nai_FxHandleFrameCheck(u8 bankId) {
    if (FxHandleVol[bankId].remainingFrames != 0) {
        FxHandleVol[bankId].remainingFrames--;
        if (FxHandleVol[bankId].remainingFrames != 0) {
            FxHandleVol[bankId].value -= FxHandleVol[bankId].step;
        } else {
            FxHandleVol[bankId].value = FxHandleVol[bankId].target;
        }
    }
}

void Nai_FxPlayCheck(void) {
    u8 bankId;

    if (IS_SEQUENCE_CHANNEL_VALID(AG.seqPlayers[SEQ_PLAYER_SFX].channels[0])) {
        now_check_sub = 0;
        for (bankId = 0; bankId < ARRAY_COUNT(EntrySeStatus); bankId++) {
            Nai_FxNextPlayCheck(bankId);
            Nai_FxNowPlayCheck(bankId);
            Nai_FxHandleFrameCheck(bankId);
        }
    }
}

u8 Nai_FxFlagCheck(u32 sfxId) {
    SfxBankEntry* entry;
    u8 entryIndex = EntrySeStatus[SFX_BANK(sfxId)][0].next;

    while (entryIndex != 0xFF) {
        entry = &EntrySeStatus[SFX_BANK(sfxId)][entryIndex];
        if (entry->sfxId == sfxId) {
            return true;
        }
        entryIndex = entry->next;
    }
    return false;
}

void Nai_fx_inter_init(void) {
    u8 bankId;
    u8 i;
    u8 entryIndex;

    _se_ent_writepoint = 0;
    _se_ent_readpoint = 0;
    se_bgm_mute_flag = 0;
    for (bankId = 0; bankId < ARRAY_COUNT(EntrySeStatus); bankId++) {
        StatEntryPoint[bankId] = 0;
        StatFreePoint[bankId] = 1;
        now_play_sounds[bankId] = 0;
        se_handle_lock_flag[bankId] = false;
        FxHandleVol[bankId].value = 1.0f;
        FxHandleVol[bankId].remainingFrames = 0;
    }
    for (bankId = 0; bankId < ARRAY_COUNT(EntrySeStatus); bankId++) {
        for (i = 0; i < MAX_CHANNELS_PER_BANK; i++) {
            play_next[bankId][i].entryIndex = 0xFF;
        }
    }
    for (bankId = 0; bankId < ARRAY_COUNT(EntrySeStatus); bankId++) {
        EntrySeStatus[bankId][0].prev = 0xFF;
        EntrySeStatus[bankId][0].next = 0xFF;
        for (i = 1; i < SeEntryBufMax[bankId] - 1; i++) {
            EntrySeStatus[bankId][i].prev = i - 1;
            EntrySeStatus[bankId][i].next = i + 1;
        }
        EntrySeStatus[bankId][i].prev = i - 1;
        EntrySeStatus[bankId][i].next = 0xFF;
    }

#if DEBUG_FEATURES
    if (D_801333F8 == 0) {
        for (bankId = 0; bankId < 10; bankId++) {
            gAudioSfxSwapSource[bankId] = 0;
            gAudioSfxSwapTarget[bankId] = 0;
            gAudioSfxSwapMode[bankId] = 0;
        }
        D_801333F8++;
    }
#endif
}

// EntrySeBuffer ring buffer endpoints. read index <= write index, wrapping around mod 256.
u8 _se_ent_writepoint = 0;
u8 _se_ent_readpoint = 0;

/**
 * Array of pointers to arrays of SfxBankEntry of sizes: 9, 12, 22, 20, 8, 3, 5
 *
 * 0 : Player Bank          size 9
 * 1 : Item Bank            size 12
 * 2 : Environment Bank     size 22
 * 3 : Enemy Bank           size 20
 * 4 : System Bank          size 8
 * 5 : Ocarina Bank         size 3
 * 6 : Voice Bank           size 5
 */
SfxBankEntry* EntrySeStatus[7] = {
    EntrySeHandle_0, EntrySeHandle_1, EntrySeHandle_2, EntrySeHandle_3, EntrySeHandle_4, EntrySeHandle_5, EntrySeHandle_6,
};

u8 SeEntryBufMax[ARRAY_COUNT(EntrySeStatus)] = {
    ARRAY_COUNT(EntrySeHandle_0), ARRAY_COUNT(EntrySeHandle_1), ARRAY_COUNT(EntrySeHandle_2), ARRAY_COUNT(EntrySeHandle_3),
    ARRAY_COUNT(EntrySeHandle_4), ARRAY_COUNT(EntrySeHandle_5), ARRAY_COUNT(EntrySeHandle_6),
};

u8 se_scene_mode = 0;

u16 se_bgm_mute_flag = 0;

// The center of the screen in projected coordinates.
// Gives the impression that the sfx has no specific location
Vec3f _dummy_zero_f = { 0.0f, 0.0f, 0.0f };

// Reused as either frequency or volume multiplicative scaling factor
// Does not alter or change frequency or volume
f32 _dummy_one = 1.0f;

s32 _dummy_zero_u8 = 0; // unused

// Adds no reverb to the existing reverb
s8 _dummy_zero_s8 = 0;

s32 AudioGameFrameCount = 0; // unused

#if DEBUG_FEATURES
u8 D_801333F0 = 0;
u8 gAudioSfxSwapOff = 0;
u8 D_801333F8 = 0;
s32 D_801333FC = 0; // unused
#endif
