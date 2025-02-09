#include "global.h"
#include "versions.h"

#define SAMPLES_TO_OVERPRODUCE 0x10
#define EXTRA_BUFFERED_AI_SAMPLES_TARGET 0x80

AudioTask* __Nas_AudioMain(void);
void __Nas_GroupFadeIn(s32 seqPlayerIndex, s32 fadeTimer);
void __Nas_GroupFadeOut(s32 seqPlayerIndex, s32 fadeTimer);
void Nap_AudioPortProcess(u32);
void __SetGrpParam(SequencePlayer* seqPlayer, AudioCmd* cmd);
void __SetSubParam(SequenceChannel* channel, AudioCmd* cmd);
s32 Nap_SilenceCheck_Inner(s32 flags);

// AudioMgr_Retrace
AudioTask* Nas_AudioMain(void) {
    return __Nas_AudioMain();
}

#if !(OOT_VERSION < PAL_1_0 || !PLATFORM_N64)
static s32 now_maxtask = 0x80;
static AudioTask* next_task = NULL;
#endif

/**
 * This is Nas_AudioInput for the audio thread
 */
AudioTask* __Nas_AudioMain(void) {
#if OOT_VERSION < PAL_1_0 || !PLATFORM_N64
    static s32 now_maxtask = 0x80;
    static AudioTask* next_task = NULL;
#endif
    u32 samplesRemainingInAi;
    s32 abiCmdCnt;
    s32 pad;
    s32 j;
    s32 sp5C;
    s16* curAiBuffer;
    OSTask_t* task;
    s32 index;
    u32 sp4C;
    s32 sp48;
    s32 i;

    AG.totalTaskCount++;
    if (AG.totalTaskCount % (AG.audioBufferParameters.specUnk4) != 0) {
        if (NA_VFRAME_CALLBACK != NULL) {
            NA_VFRAME_CALLBACK();
        }

        if ((AG.totalTaskCount % AG.audioBufferParameters.specUnk4) + 1 ==
            AG.audioBufferParameters.specUnk4) {
            return next_task;
        } else {
            return NULL;
        }
    }

    osSendMesg(AG.taskStartQueueP, (OSMesg)AG.totalTaskCount, OS_MESG_NOBLOCK);
    AG.rspTaskIndex ^= 1;
    AG.curAiBufIndex++;
    AG.curAiBufIndex %= 3;
    index = (AG.curAiBufIndex - 2 + 3) % 3;
    samplesRemainingInAi = osAiGetLength() / 4;

    if (AG.resetTimer < 16) {
        if (AG.aiBufLengths[index] != 0) {
            osAiSetNextBuffer2(AG.aiBuffers[index], AG.aiBufLengths[index] * 4);
            if (AG.aiBuffers[index]) {}
            if (AG.aiBufLengths[index]) {}
        }
    }

    if (NA_VFRAME_CALLBACK != NULL) {
        NA_VFRAME_CALLBACK();
    }

    sp5C = AG.curAudioFrameDmaCount;
    for (i = 0; i < AG.curAudioFrameDmaCount; i++) {
        if (osRecvMesg(&AG.curAudioFrameDmaQueue, NULL, OS_MESG_NOBLOCK) == 0) {
            sp5C--;
        }
    }

    if (sp5C != 0) {
        for (i = 0; i < sp5C; i++) {
            osRecvMesg(&AG.curAudioFrameDmaQueue, NULL, OS_MESG_BLOCK);
        }
    }

    sp48 = MQ_GET_COUNT(&AG.curAudioFrameDmaQueue);
    if (sp48 != 0) {
        for (i = 0; i < sp48; i++) {
            osRecvMesg(&AG.curAudioFrameDmaQueue, NULL, OS_MESG_NOBLOCK);
        }
    }

    AG.curAudioFrameDmaCount = 0;
    Nas_WaveDmaFrameWork();
    Nas_BgDmaFrameWork(AG.resetStatus);
    MK_FrameWork();

    if (AG.resetStatus != 0) {
        if (Nas_SpecChange() == 0) {
            if (AG.resetStatus == 0) {
                osSendMesg(AG.audioResetQueueP, (OSMesg)(u32)AG.specId, OS_MESG_NOBLOCK);
            }

            next_task = NULL;
            return NULL;
        }
    }

    if (AG.resetTimer > 16) {
        return NULL;
    }
    if (AG.resetTimer != 0) {
        AG.resetTimer++;
    }

    AG.curTask = &AG.rspTask[AG.rspTaskIndex];
    AG.curAbiCmdBuf = AG.abiCmdBufs[AG.rspTaskIndex];

    index = AG.curAiBufIndex;
    curAiBuffer = AG.aiBuffers[index];

    AG.aiBufLengths[index] =
        (s16)((((AG.audioBufferParameters.samplesPerFrameTarget - samplesRemainingInAi) +
                EXTRA_BUFFERED_AI_SAMPLES_TARGET) &
               ~0xF) +
              SAMPLES_TO_OVERPRODUCE);
    if (AG.aiBufLengths[index] < AG.audioBufferParameters.minAiBufferLength) {
        AG.aiBufLengths[index] = AG.audioBufferParameters.minAiBufferLength;
    }

    if (AG.aiBufLengths[index] > AG.audioBufferParameters.maxAiBufferLength) {
        AG.aiBufLengths[index] = AG.audioBufferParameters.maxAiBufferLength;
    }

    j = 0;
    if (AG.resetStatus == 0) {
        // msg = 0000RREE R = read pos, E = End Pos
        while (osRecvMesg(AG.threadCmdProcQueueP, (OSMesg*)&sp4C, OS_MESG_NOBLOCK) != -1) {
            if (1) {}
            Nap_AudioPortProcess(sp4C);
            j++;
        }
        if ((j == 0) && (AG.threadCmdQueueFinished)) {
            Nap_SendStart();
        }
    }

    AG.curAbiCmdBuf =
        Nas_smzAudioFrame(AG.curAbiCmdBuf, &abiCmdCnt, curAiBuffer, AG.aiBufLengths[index]);

    // Update audioRandom to the next random number
    AG.audioRandom = (AG.audioRandom + AG.totalTaskCount) * osGetCount();
    AG.audioRandom = AG.audioRandom + AG.aiBuffers[index][AG.totalTaskCount & 0xFF];

    // WAVEMEM_TABLE[8] interprets compiled assembly code as s16 samples as a way to generate sound with noise.
    // Start with the address of Nas_AudioMain, and offset it by a random number between 0 - 0xFFF0
    // Use the resulting address as the starting address to interpret an array of samples i.e. `s16 samples[]`
    WAVEMEM_TABLE[8] = (s16*)((u8*)Nas_AudioMain + (AG.audioRandom & 0xFFF0));

    index = AG.rspTaskIndex;
    AG.curTask->msgQueue = NULL;
    AG.curTask->unk_44 = NULL;

    task = &AG.curTask->task.t;
    task->type = M_AUDTASK;
    task->flags = 0;
    task->ucode_boot = aspMainTextStart;
    task->ucode_boot_size = SP_UCODE_SIZE;
    task->ucode_data_size = (size_t)(aspMainDataEnd - aspMainDataStart) * sizeof(u64) - 1;
    task->ucode = aspMainTextStart;
    task->ucode_data = aspMainDataStart;
    task->ucode_size = SP_UCODE_SIZE;
    task->dram_stack = NULL;
    task->dram_stack_size = 0;
    task->output_buff = NULL;
    task->output_buff_size = NULL;
    if (1) {}
    task->data_ptr = (u64*)AG.abiCmdBufs[index];
    task->data_size = abiCmdCnt * sizeof(Acmd);
    task->yield_data_ptr = NULL;
    task->yield_data_size = 0;

    if (now_maxtask < abiCmdCnt) {
        now_maxtask = abiCmdCnt;
    }

    if (AG.audioBufferParameters.specUnk4 == 1) {
        return AG.curTask;
    } else {
        next_task = AG.curTask;
        return NULL;
    }
}

void Nap_AudioSysProcess(AudioCmd* cmd) {
    s32 i;
    s32 pad[3];
    u32 flags;

    switch (cmd->op) {
        case AUDIOCMD_OP_GLOBAL_SYNC_LOAD_SEQ_PARTS:
            Nas_PreLoadSeq(cmd->arg1, cmd->arg2);
            break;

        case AUDIOCMD_OP_GLOBAL_INIT_SEQPLAYER:
            Nas_StartMySeq(cmd->arg0, cmd->arg1, cmd->arg2);
            __Nas_GroupFadeIn(cmd->arg0, cmd->asInt);
            break;

        case AUDIOCMD_OP_GLOBAL_INIT_SEQPLAYER_SKIP_TICKS:
            Nas_StartSeq_Skip(cmd->arg0, cmd->arg1, cmd->asInt);
            break;

        case AUDIOCMD_OP_GLOBAL_DISABLE_SEQPLAYER:
            if (AG.seqPlayers[cmd->arg0].enabled) {
                if (cmd->asInt == 0) {
                    Nas_ReleaseGroup_Force(&AG.seqPlayers[cmd->arg0]);
                } else {
                    __Nas_GroupFadeOut(cmd->arg0, cmd->asInt);
                }
            }
            break;

        case AUDIOCMD_OP_GLOBAL_SET_SOUND_MODE:
            AG.soundMode = cmd->asUInt;
            break;

        case AUDIOCMD_OP_GLOBAL_MUTE:
            for (i = 0; i < AG.audioBufferParameters.numSequencePlayers; i++) {
                SequencePlayer* seqPlayer = &AG.seqPlayers[i];

                seqPlayer->muted = true;
                seqPlayer->recalculateVolume = true;
            }
            break;

        case AUDIOCMD_OP_GLOBAL_UNMUTE:
            if (cmd->asUInt == 1) {
                for (i = 0; i < AG.numNotes; i++) {
                    Note* note = &AG.notes[i];
                    NoteSubEu* subEu = &note->noteSubEu;

                    if (subEu->bitField0.enabled && (note->playbackState.unk_04 == 0) &&
                        (note->playbackState.parentLayer->channel->muteBehavior & MUTE_BEHAVIOR_3)) {
                        subEu->bitField0.finished = true;
                    }
                }
            }

            for (i = 0; i < AG.audioBufferParameters.numSequencePlayers; i++) {
                SequencePlayer* seqPlayer = &AG.seqPlayers[i];

                seqPlayer->muted = false;
                seqPlayer->recalculateVolume = true;
            }
            break;

        case AUDIOCMD_OP_GLOBAL_SYNC_LOAD_INSTRUMENT:
            Nas_LoadVoice(cmd->arg0, cmd->arg1, cmd->arg2);
            break;

        case AUDIOCMD_OP_GLOBAL_ASYNC_LOAD_SAMPLE_BANK:
            Nas_PreLoadWave_BG(cmd->arg0, cmd->arg1, cmd->arg2, &AG.externalLoadQueue);
            break;

        case AUDIOCMD_OP_GLOBAL_ASYNC_LOAD_FONT:
            Nas_PreLoadBank_BG(cmd->arg0, cmd->arg1, cmd->arg2, &AG.externalLoadQueue);
            break;

        case AUDIOCMD_OP_GLOBAL_ASYNC_LOAD_SEQ:
            Nas_PreLoadSeq_BG(cmd->arg0, cmd->arg1, cmd->arg2, &AG.externalLoadQueue);
            break;

        case AUDIOCMD_OP_GLOBAL_DISCARD_SEQ_FONTS:
            Nas_FlushBank(cmd->arg1);
            break;

        case AUDIOCMD_OP_GLOBAL_SET_CHANNEL_MASK:
            AG.threadCmdChannelMask[cmd->arg0] = cmd->asUShort;
            break;

        case AUDIOCMD_OP_GLOBAL_RESET_AUDIO_HEAP:
            AG.resetStatus = 5;
            AG.specId = cmd->asUInt;
            break;

        case AUDIOCMD_OP_GLOBAL_SET_CUSTOM_UPDATE_FUNCTION:
            NA_VFRAME_CALLBACK = (AudioCustomUpdateFunction)cmd->asUInt;
            break;

        case AUDIOCMD_OP_GLOBAL_SET_DRUM_FONT:
        case AUDIOCMD_OP_GLOBAL_SET_SFX_FONT:
        case AUDIOCMD_OP_GLOBAL_SET_INSTRUMENT_FONT:
            OverwriteBank(cmd->op - AUDIOCMD_OP_GLOBAL_SET_DRUM_FONT, cmd->arg0, cmd->arg1, cmd->data);
            break;

        case AUDIOCMD_OP_GLOBAL_DISABLE_ALL_SEQPLAYERS:
            flags = cmd->asUInt;
            if (flags == 1) {
                for (i = 0; i < AG.audioBufferParameters.numSequencePlayers; i++) {
                    SequencePlayer* seqPlayer = &AG.seqPlayers[i];

                    if (seqPlayer->enabled) {
                        Nas_ReleaseGroup_Force(seqPlayer);
                    }
                }
            }
            Nap_SilenceCheck_Inner(flags);
            break;

        case AUDIOCMD_OP_GLOBAL_POP_PERSISTENT_CACHE:
            Nas_SzStayDelete(cmd->asInt);
            break;

        default:
            break;
    }
}

void __Nas_GroupFadeOut(s32 seqPlayerIndex, s32 fadeTimer) {
    SequencePlayer* seqPlayer = &AG.seqPlayers[seqPlayerIndex];

    if (fadeTimer == 0) {
        fadeTimer = 1;
    }

    seqPlayer->fadeVelocity = -(seqPlayer->fadeVolume / fadeTimer);
    seqPlayer->state = 2;
    seqPlayer->fadeTimer = fadeTimer;
}

void __Nas_GroupFadeIn(s32 seqPlayerIndex, s32 fadeTimer) {
    SequencePlayer* seqPlayer;

    if (fadeTimer != 0) {
        seqPlayer = &AG.seqPlayers[seqPlayerIndex];
        seqPlayer->state = 1;
        seqPlayer->fadeTimerUnkEu = fadeTimer;
        seqPlayer->fadeTimer = fadeTimer;
        seqPlayer->fadeVolume = 0.0f;
        seqPlayer->fadeVelocity = 0.0f;
    }
}

void Nap_AudioPortInit(void) {
    AG.threadCmdWritePos = 0;
    AG.threadCmdReadPos = 0;
    AG.threadCmdQueueFinished = false;

    AG.taskStartQueueP = &AG.taskStartQueue;
    AG.threadCmdProcQueueP = &AG.threadCmdProcQueue;
    AG.audioResetQueueP = &AG.audioResetQueue;

    osCreateMesgQueue(AG.taskStartQueueP, AG.taskStartMsgBuf, ARRAY_COUNT(AG.taskStartMsgBuf));
    osCreateMesgQueue(AG.threadCmdProcQueueP, AG.threadCmdProcMsgBuf,
                      ARRAY_COUNT(AG.threadCmdProcMsgBuf));
    osCreateMesgQueue(AG.audioResetQueueP, AG.audioResetMsgBuf, ARRAY_COUNT(AG.audioResetMsgBuf));
}

void Nap_PortSet(u32 opArgs, void** data) {
    AudioCmd* cmd = &AG.threadCmdBuf[AG.threadCmdWritePos & 0xFF];

    cmd->opArgs = opArgs;
    cmd->data = *data;

    AG.threadCmdWritePos++;

    if (AG.threadCmdWritePos == AG.threadCmdReadPos) {
        AG.threadCmdWritePos--;
    }
}

void Nap_SetF32(u32 opArgs, f32 data) {
    Nap_PortSet(opArgs, (void**)&data);
}

void Nap_SetS32(u32 opArgs, s32 data) {
    Nap_PortSet(opArgs, (void**)&data);
}

void Nap_SetS8(u32 opArgs, s8 data) {
    u32 uData = data << 0x18;

    Nap_PortSet(opArgs, (void**)&uData);
}

void Nap_SetU16(u32 opArgs, u16 data) {
    u32 uData = data << 0x10;

    Nap_PortSet(opArgs, (void**)&uData);
}

s32 Nap_SendStart(void) {
    static s32 worst = 0;
    s32 ret;

    if (worst < (u8)((AG.threadCmdWritePos - AG.threadCmdReadPos) + 0x100)) {
        worst = (u8)((AG.threadCmdWritePos - AG.threadCmdReadPos) + 0x100);
    }

    ret = osSendMesg(AG.threadCmdProcQueueP,
                     (OSMesg)(((AG.threadCmdReadPos & 0xFF) << 8) | (AG.threadCmdWritePos & 0xFF)),
                     OS_MESG_NOBLOCK);
    if (ret != -1) {
        AG.threadCmdReadPos = AG.threadCmdWritePos;
        ret = 0;
    } else {
        return -1;
    }

    return ret;
}

void Nap_FlushPort(void) {
    AG.threadCmdQueueFinished = false;
    AG.threadCmdReadPos = AG.threadCmdWritePos;
}

void Nap_Process1Command(AudioCmd* cmd) {
    SequencePlayer* seqPlayer;
    u16 threadCmdChannelMask;
    s32 channelIndex;

    if ((cmd->op & 0xF0) == 0xF0) {
        Nap_AudioSysProcess(cmd);
        return;
    }

    if (cmd->arg0 < AG.audioBufferParameters.numSequencePlayers) {
        seqPlayer = &AG.seqPlayers[cmd->arg0];
        if (cmd->op & 0x80) {
            Nap_AudioSysProcess(cmd);
            return;
        }
        if (cmd->op & 0x40) {
            __SetGrpParam(seqPlayer, cmd);
            return;
        }

        if (cmd->arg1 < SEQ_NUM_CHANNELS) {
            __SetSubParam(seqPlayer->channels[cmd->arg1], cmd);
            return;
        }
        if (cmd->arg1 == AUDIOCMD_ALL_CHANNELS) {
            threadCmdChannelMask = AG.threadCmdChannelMask[cmd->arg0];
            for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
                if (threadCmdChannelMask & 1) {
                    __SetSubParam(seqPlayer->channels[channelIndex], cmd);
                }
                threadCmdChannelMask = threadCmdChannelMask >> 1;
            }
        }
    }
}

void Nap_AudioPortProcess(u32 msg) {
    static u8 begin = 0;
    AudioCmd* cmd;
    u8 endPos;

    if (!AG.threadCmdQueueFinished) {
        begin = msg >> 8;
    }

    while (true) {
        endPos = msg & 0xFF;
        if (begin == endPos) {
            AG.threadCmdQueueFinished = false;
            return;
        }

        cmd = &AG.threadCmdBuf[begin++ & 0xFF];
        if (cmd->op == AUDIOCMD_OP_GLOBAL_STOP_AUDIOCMDS) {
            AG.threadCmdQueueFinished = true;
            return;
        }

        Nap_Process1Command(cmd);
        cmd->op = AUDIOCMD_OP_NOOP;
    }
}

u32 Nap_GetEvent(u32* out) {
    u32 sp1C;

    if (osRecvMesg(&AG.externalLoadQueue, (OSMesg*)&sp1C, OS_MESG_NOBLOCK) == -1) {
        *out = 0;
        return 0;
    }
    *out = sp1C & 0xFFFFFF;
    return sp1C >> 0x18;
}

u8* Nap_SeqToBank(s32 seqId, u32* outNumFonts) {
    return Nas_SeqToBank(seqId, outNumFonts);
}

void Nap_BankToWave(s32 fontId, u32* sampleBankId1, u32* sampleBankId2) {
    *sampleBankId1 = AG.soundFontList[fontId].sampleBankId1;
    *sampleBankId2 = AG.soundFontList[fontId].sampleBankId2;
}

s32 Nap_CheckSpecChange(void) {
    s32 pad;
    s32 specId;

    if (osRecvMesg(AG.audioResetQueueP, (OSMesg*)&specId, OS_MESG_NOBLOCK) == -1) {
        return 0;
    } else if (AG.specId != specId) {
        return -1;
    } else {
        return 1;
    }
}

void __ClearSpecChangeQ(void) {
    // macro?
    // clang-format off
    s32 chk = -1; OSMesg msg; do {} while (osRecvMesg(AG.audioResetQueueP, &msg, OS_MESG_NOBLOCK) != chk);
    // clang-format on
}

s32 Nap_StartSpecChange(s32 specId) {
    s32 resetStatus;
    OSMesg msg;
    s32 pad;

    __ClearSpecChangeQ();
    resetStatus = AG.resetStatus;
    if (resetStatus != 0) {
        Nap_FlushPort();
        if (AG.specId == specId) {
            return -2;
        } else if (resetStatus > 2) {
            AG.specId = specId;
            return -3;
        } else {
            osRecvMesg(AG.audioResetQueueP, &msg, OS_MESG_BLOCK);
        }
    }

    __ClearSpecChangeQ();
    AUDIOCMD_GLOBAL_RESET_AUDIO_HEAP(specId);

    return Nap_SendStart();
}

void Nap_StartReset(void) {
    AG.resetTimer = 1;
    if (AUDIO_SYSTEM_READY) {
        Nap_StartSpecChange(0);
        AG.resetStatus = 0;
    }
}

s8 Nap_ReadSubPort(s32 seqPlayerIndex, s32 channelIndex, s32 ioPort) {
    SequencePlayer* seqPlayer = &AG.seqPlayers[seqPlayerIndex];
    SequenceChannel* channel;

    if (seqPlayer->enabled) {
        channel = seqPlayer->channels[channelIndex];
        return channel->seqScriptIO[ioPort];
    } else {
        return SEQ_IO_VAL_NONE;
    }
}

s8 Nap_ReadGrpPort(s32 seqPlayerIndex, s32 ioPort) {
    return AG.seqPlayers[seqPlayerIndex].seqScriptIO[ioPort];
}

void Nap_Set2ndHeap(void* ramAddr, u32 size) {
    Nas_HeapInit(&AG.externalPool, ramAddr, size);
}

void Nap_Stop2ndHeap(void) {
    AG.externalPool.startRamAddr = NULL;
}

void __SetGrpParam(SequencePlayer* seqPlayer, AudioCmd* cmd) {
    f32 fadeVolume;

    switch (cmd->op) {
        case AUDIOCMD_OP_SEQPLAYER_FADE_VOLUME_SCALE:
            if (seqPlayer->fadeVolumeScale != cmd->asFloat) {
                seqPlayer->fadeVolumeScale = cmd->asFloat;
                seqPlayer->recalculateVolume = true;
            }
            break;

        case AUDIOCMD_OP_SEQPLAYER_SET_TEMPO:
            seqPlayer->tempo = cmd->asInt * SEQTICKS_PER_BEAT;
            break;

        case AUDIOCMD_OP_SEQPLAYER_CHANGE_TEMPO:
            seqPlayer->tempoChange = cmd->asInt * SEQTICKS_PER_BEAT;
            break;

        case AUDIOCMD_OP_SEQPLAYER_CHANGE_TEMPO_SEQTICKS:
            seqPlayer->tempoChange = cmd->asInt;
            break;

        case AUDIOCMD_OP_SEQPLAYER_SET_TRANSPOSITION:
            seqPlayer->transposition = cmd->asSbyte;
            break;

        case AUDIOCMD_OP_SEQPLAYER_SET_IO:
            seqPlayer->seqScriptIO[cmd->arg2] = cmd->asSbyte;
            break;

        case AUDIOCMD_OP_SEQPLAYER_FADE_TO_SET_VOLUME:
            fadeVolume = (s32)cmd->arg1 / 127.0f;
            goto apply_fade;

        case AUDIOCMD_OP_SEQPLAYER_FADE_TO_SCALED_VOLUME:
            fadeVolume = ((s32)cmd->arg1 / 100.0f) * seqPlayer->fadeVolume;
        apply_fade:
            if (seqPlayer->state != 2) {
                seqPlayer->volume = seqPlayer->fadeVolume;
                if (cmd->asInt == 0) {
                    seqPlayer->fadeVolume = fadeVolume;
                } else {
                    s32 fadeTimer = cmd->asInt;

                    seqPlayer->state = 0;
                    seqPlayer->fadeTimer = fadeTimer;
                    seqPlayer->fadeVelocity = (fadeVolume - seqPlayer->fadeVolume) / fadeTimer;
                }
            }
            break;

        case AUDIOCMD_OP_SEQPLAYER_RESET_VOLUME:
            if (seqPlayer->state != 2) {
                if (cmd->asInt == 0) {
                    seqPlayer->fadeVolume = seqPlayer->volume;
                } else {
                    s32 fadeTimer = cmd->asInt;

                    seqPlayer->state = 0;
                    seqPlayer->fadeTimer = fadeTimer;
                    seqPlayer->fadeVelocity = (seqPlayer->volume - seqPlayer->fadeVolume) / fadeTimer;
                }
            }
            break;

        case AUDIOCMD_OP_SEQPLAYER_SET_BEND:
            seqPlayer->bend = cmd->asFloat;
            if (seqPlayer->bend == 1.0f) {
                seqPlayer->applyBend = false;
            } else {
                seqPlayer->applyBend = true;
            }
            break;

        default:
            break;
    }
}

void __SetSubParam(SequenceChannel* channel, AudioCmd* cmd) {
    switch (cmd->op) {
        case AUDIOCMD_OP_CHANNEL_SET_VOL_SCALE:
            if (channel->volumeScale != cmd->asFloat) {
                channel->volumeScale = cmd->asFloat;
                channel->changes.s.volume = true;
            }
            break;

        case AUDIOCMD_OP_CHANNEL_SET_VOL:
            if (channel->volume != cmd->asFloat) {
                channel->volume = cmd->asFloat;
                channel->changes.s.volume = true;
            }
            break;

        case AUDIOCMD_OP_CHANNEL_SET_PAN:
            if (channel->newPan != cmd->asSbyte) {
                channel->newPan = cmd->asSbyte;
                channel->changes.s.pan = true;
            }
            break;

        case AUDIOCMD_OP_CHANNEL_SET_PAN_WEIGHT:
            //! @bug: Should compare `asSbyte` to `panChannelWeight`
            if (channel->newPan != cmd->asSbyte) {
                channel->panChannelWeight = cmd->asSbyte;
                channel->changes.s.pan = true;
            }
            break;

        case AUDIOCMD_OP_CHANNEL_SET_FREQ_SCALE:
            if (channel->freqScale != cmd->asFloat) {
                channel->freqScale = cmd->asFloat;
                channel->changes.s.freqScale = true;
            }
            break;

        case AUDIOCMD_OP_CHANNEL_SET_REVERB_VOLUME:
            if (channel->targetReverbVol != cmd->asSbyte) {
                channel->targetReverbVol = cmd->asSbyte;
            }
            break;

        case AUDIOCMD_OP_CHANNEL_SET_IO:
            if (cmd->arg2 < ARRAY_COUNT(channel->seqScriptIO)) {
                channel->seqScriptIO[cmd->arg2] = cmd->asSbyte;
            }
            break;

        case AUDIOCMD_OP_CHANNEL_SET_MUTE:
            channel->muted = cmd->asSbyte;
            break;

        case AUDIOCMD_OP_CHANNEL_SET_MUTE_BEHAVIOR:
            channel->muteBehavior = cmd->asSbyte;
            break;

        case AUDIOCMD_OP_CHANNEL_SET_VIBRATO_DEPTH:
            channel->vibratoDepthTarget = cmd->asUbyte * 8;
            channel->vibratoDepthChangeDelay = 1;
            break;

        case AUDIOCMD_OP_CHANNEL_SET_VIBRATO_RATE:
            channel->vibratoRateTarget = cmd->asUbyte * 32;
            channel->vibratoRateChangeDelay = 1;
            break;

        case AUDIOCMD_OP_CHANNEL_SET_COMB_FILTER_SIZE:
            channel->combFilterSize = cmd->asUbyte;
            break;

        case AUDIOCMD_OP_CHANNEL_SET_COMB_FILTER_GAIN:
            channel->combFilterGain = cmd->asUShort;
            break;

        case AUDIOCMD_OP_CHANNEL_SET_STEREO:
            channel->stereo.asByte = cmd->asUbyte;
            break;

        default:
            break;
    }
}

/**
 * Call an audio-thread command that has no code to process it. Unused.
 */
void Nad_BankProgOn(s32 arg0, s32 arg1, s32 arg2) {
    AUDIOCMD_GLOBAL_NOOP_1(arg0, arg1, arg2, 1);
}

/**
 * Call an audio-thread command that has no code to process it. Unused.
 */
void Nad_BankProgOff(void) {
    AUDIOCMD_GLOBAL_NOOP_1(0, 0, 0, 0);
}

/**
 * Call an audio-thread command that has no code to process it. Unused.
 */
void Nad_BankBend(u32 arg0, s32 arg1) {
    AUDIOCMD_GLOBAL_NOOP_2(0, 0, arg1, arg0);
}

void Nap_WaitVsync(void) {
    osRecvMesg(AG.taskStartQueueP, NULL, OS_MESG_NOBLOCK);
    osRecvMesg(AG.taskStartQueueP, NULL, OS_MESG_BLOCK);
}

s32 Nap_GetRemainSample(s32 seqPlayerIndex, s32 channelIndex, s32 layerIndex) {
    SequencePlayer* seqPlayer;
    SequenceLayer* layer;
    Note* note;
    TunedSample* tunedSample;
    s32 loopEnd;
    s32 samplePos;

    seqPlayer = &AG.seqPlayers[seqPlayerIndex];
    if (seqPlayer->enabled && seqPlayer->channels[channelIndex]->enabled) {
        layer = seqPlayer->channels[channelIndex]->layers[layerIndex];
        if (layer == NULL) {
            return 0;
        }

        if (layer->enabled) {
            if (layer->note == NULL) {
                return 0;
            }

            if (!layer->bit3) {
                return 0;
            }

            note = layer->note;
            if (layer == note->playbackState.parentLayer) {
                tunedSample = note->noteSubEu.tunedSample;
                if (tunedSample == NULL) {
                    return 0;
                }
                loopEnd = tunedSample->sample->loop->header.end;
                samplePos = note->synthesisState.samplePosInt;
                return loopEnd - samplePos;
            }
            return 0;
        }
    }
    return 0;
}

s32 Nap_SilenceCheck(void) {
    return Nap_SilenceCheck_Inner(0);
}

void Nap_SilenceCheck_Device(void) {
    Nap_SilenceCheck_Inner(2);
}

s32 Nap_SilenceCheck_Inner(s32 flags) {
    s32 phi_v1;
    NotePlaybackState* playbackState;
    NoteSubEu* noteSubEu;
    s32 i;
    Note* note;
    TunedSample* tunedSample;

    phi_v1 = 0;
    for (i = 0; i < AG.numNotes; i++) {
        note = &AG.notes[i];
        playbackState = &note->playbackState;
        if (note->noteSubEu.bitField0.enabled) {
            noteSubEu = &note->noteSubEu;
            if (playbackState->adsr.action.s.state != 0) {
                if (flags >= 2) {
                    tunedSample = noteSubEu->tunedSample;
                    if (tunedSample == NULL || noteSubEu->bitField1.isSyntheticWave) {
                        continue;
                    }
                    if (tunedSample->sample->medium == MEDIUM_RAM) {
                        continue;
                    }
                }

                phi_v1++;
                if ((flags & 1) == 1) {
                    playbackState->adsr.fadeOutVel = AG.audioBufferParameters.ticksPerUpdateInv;
                    playbackState->adsr.action.s.release = 1;
                }
            }
        }
    }
    return phi_v1;
}

u32 Nap_GetRandom(void) {
    static u32 get = 0x12345678;

    get = ((osGetCount() + 0x1234567) * (get + AG.totalTaskCount));
    get += AG.audioRandom;

    return get;
}

void Nas_InitGAudio(void) {
    Nap_AudioPortInit();
}
