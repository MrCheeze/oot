/**
 * @file code_800F9280.c
 *
 * This file implements a set of high-level audio sequence commands that allow sequences to be modified in real-time.
 * These commands are intended to interface external to the audio library.
 *
 * These commands are generated using `Nai_SeqFlagEntry`, and a user-friendly interface for this function
 * can be found in `seqcmd.h`
 *
 * These commands change sequences by generating internal audio commands `Nap_PortSet` which allows these
 * sequence requests to be passed onto the audio thread. It is worth noting all functions in this file are
 * called from the graph thread.
 *
 * These commands are not to be confused with the sequence instructions used by the sequences themselves
 * which are a midi-based scripting language.
 *
 * Nor are these commands to be confused with the internal audio commands used to transfer requests from
 * the graph thread to the audio thread.
 */
#include "ultra64.h"
#include "global.h"
#include "ultra64/abi.h"

// Direct audio command (skips the queueing system)
#define SEQCMD_SET_SEQPLAYER_VOLUME_NOW(seqPlayerIndex, duration, volume)                       \
    Nai_SeqControl((SEQCMD_OP_SET_SEQPLAYER_VOLUME << 28) | ((u8)(seqPlayerIndex) << 24) | \
                        ((u8)(duration) << 16) | ((u8)((volume)*127.0f)));

typedef struct SeqRequest {
    /* 0x0 */ u8 seqId;
    /* 0x1 */ u8 priority; // higher values have higher priority
} SeqRequest;              // size = 0x2

SeqRequest seq_entry[4][5];
u8 seq_entries[4];
u32 SeqFlagBuffer[0x100];
ActiveSequence SeqInterWork[4];

void Nai_StartSeqSet(u8 seqPlayerIndex, u8 seqId, u8 seqArgs, u16 fadeInDuration) {
    u8 channelIndex;
    u16 skipTicks;
    s32 pad;

    if (!SeqStartCancel || (seqPlayerIndex == SEQ_PLAYER_SFX)) {
        seqArgs &= 0x7F;
        if (DEBUG_FEATURES && (seqArgs == 0x7F)) {
            // `fadeInDuration` interpreted as seconds, 60 is refresh rate and does not account for PAL
            skipTicks = (fadeInDuration >> 3) * 60 * AG.audioBufferParameters.ticksPerUpdate;
            AUDIOCMD_GLOBAL_INIT_SEQPLAYER_SKIP_TICKS((u32)seqPlayerIndex, (u32)seqId, skipTicks);
        } else {
            // `fadeInDuration` interpreted as 1/30th of a second, does not account for change in refresh rate for PAL
            AUDIOCMD_GLOBAL_INIT_SEQPLAYER((u32)seqPlayerIndex, (u32)seqId,
                                           (fadeInDuration * (u16)AG.audioBufferParameters.ticksPerUpdate) / 4);
        }

        SeqInterWork[seqPlayerIndex].seqId = seqId | (seqArgs << 8);
        SeqInterWork[seqPlayerIndex].prevSeqId = seqId | (seqArgs << 8);

        if (SeqInterWork[seqPlayerIndex].volCur != 1.0f) {
            AUDIOCMD_SEQPLAYER_FADE_VOLUME_SCALE((u32)seqPlayerIndex, SeqInterWork[seqPlayerIndex].volCur);
        }

        SeqInterWork[seqPlayerIndex].tempoTimer = 0;
        SeqInterWork[seqPlayerIndex].tempoOriginal = 0;
        SeqInterWork[seqPlayerIndex].tempoCmd = 0;

        for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
            SeqInterWork[seqPlayerIndex].channelData[channelIndex].volCur = 1.0f;
            SeqInterWork[seqPlayerIndex].channelData[channelIndex].volTimer = 0;
            SeqInterWork[seqPlayerIndex].channelData[channelIndex].freqScaleCur = 1.0f;
            SeqInterWork[seqPlayerIndex].channelData[channelIndex].freqScaleTimer = 0;
        }

        SeqInterWork[seqPlayerIndex].freqScaleChannelFlags = 0;
        SeqInterWork[seqPlayerIndex].volChannelFlags = 0;
    }
}

void Nai_StopSeqSet(u8 seqPlayerIndex, u16 fadeOutDuration) {
    AUDIOCMD_GLOBAL_DISABLE_SEQPLAYER(seqPlayerIndex,
                                      (fadeOutDuration * (u16)AG.audioBufferParameters.ticksPerUpdate) / 4);
    SeqInterWork[seqPlayerIndex].seqId = NA_BGM_DISABLED;
}

void Nai_SeqControl(u32 cmd) {
    s32 priority;
    s32 channelMaskEnable;
    u16 channelMaskDisable;
    u16 fadeTimer;
    u16 val;
    u8 oldSpecId;
    u8 specId;
    u8 op;
    u8 subOp;
    u8 seqPlayerIndex;
    u8 seqId;
    u8 seqArgs;
    u8 found;
    u8 ioPort;
    u8 duration;
    u8 channelIndex;
    u8 i;
    f32 freqScaleTarget;
    s32 pad;

#if DEBUG_FEATURES
    if (gAudioDebugPrintSeqCmd && (cmd & SEQCMD_OP_MASK) != (SEQCMD_OP_SET_SEQPLAYER_IO << 28)) {
        Nad_GameScrollPrint("SEQ H", (cmd >> 16) & 0xFFFF);
        Nad_GameScrollPrint("    L", cmd & 0xFFFF);
    }
#endif

    op = cmd >> 28;
    seqPlayerIndex = (cmd & 0xF000000) >> 24;

    switch (op) {
        case SEQCMD_OP_PLAY_SEQUENCE:
            // Play a new sequence
            seqId = cmd & 0xFF;
            seqArgs = (cmd & 0xFF00) >> 8;
            // `fadeTimer` is only shifted 13 bits instead of 16 bits.
            // `fadeTimer` continues to be scaled in `Nai_StartSeqSet`
            fadeTimer = (cmd & 0xFF0000) >> 13;
            if (!SeqInterWork[seqPlayerIndex].isWaitingForFonts && (seqArgs < 0x80)) {
                Nai_StartSeqSet(seqPlayerIndex, seqId, seqArgs, fadeTimer);
            }
            break;

        case SEQCMD_OP_STOP_SEQUENCE:
            // Stop a sequence and disable the sequence player
            fadeTimer = (cmd & 0xFF0000) >> 13;
            Nai_StopSeqSet(seqPlayerIndex, fadeTimer);
            break;

        case SEQCMD_OP_QUEUE_SEQUENCE:
            // Queue a sequence into `seq_entry`
            seqId = cmd & 0xFF;
            seqArgs = (cmd & 0xFF00) >> 8;
            fadeTimer = (cmd & 0xFF0000) >> 13;
            priority = seqArgs;

            // Checks if the requested sequence is first in the list of requests
            // If it is already queued and first in the list, then play the sequence immediately
            for (i = 0; i < seq_entries[seqPlayerIndex]; i++) {
                if (seq_entry[seqPlayerIndex][i].seqId == seqId) {
                    if (i == 0) {
                        Nai_StartSeqSet(seqPlayerIndex, seqId, seqArgs, fadeTimer);
                    }
                    return;
                }
            }

            // Searches the sequence requests for the first request that does not have a higher priority
            // than the current incoming request
            found = seq_entries[seqPlayerIndex];
            for (i = 0; i < seq_entries[seqPlayerIndex]; i++) {
                if (priority >= seq_entry[seqPlayerIndex][i].priority) {
                    found = i;
                    i = seq_entries[seqPlayerIndex]; // "break;"
                }
            }

            // Check if the queue is full
            if (seq_entries[seqPlayerIndex] < ARRAY_COUNT(seq_entry[seqPlayerIndex])) {
                seq_entries[seqPlayerIndex]++;
            }

            for (i = seq_entries[seqPlayerIndex] - 1; i != found; i--) {
                // Move all requests of lower priority backwards 1 place in the queue
                // If the queue is full, overwrite the entry with the lowest priority
                seq_entry[seqPlayerIndex][i].priority = seq_entry[seqPlayerIndex][i - 1].priority;
                seq_entry[seqPlayerIndex][i].seqId = seq_entry[seqPlayerIndex][i - 1].seqId;
            }

            // Fill the newly freed space in the queue with the new request
            seq_entry[seqPlayerIndex][found].priority = seqArgs;
            seq_entry[seqPlayerIndex][found].seqId = seqId;

            // The sequence is first in queue, so start playing.
            if (found == 0) {
                Nai_StartSeqSet(seqPlayerIndex, seqId, seqArgs, fadeTimer);
            }
            break;

        case SEQCMD_OP_UNQUEUE_SEQUENCE:
            // Unqueue sequence
            fadeTimer = (cmd & 0xFF0000) >> 13;

            found = seq_entries[seqPlayerIndex];
            for (i = 0; i < seq_entries[seqPlayerIndex]; i++) {
                seqId = cmd & 0xFF;
                if (seq_entry[seqPlayerIndex][i].seqId == seqId) {
                    found = i;
                    i = seq_entries[seqPlayerIndex]; // "break;"
                }
            }

            if (found != seq_entries[seqPlayerIndex]) {
                // Move all requests of lower priority forward 1 place in the queue
                for (i = found; i < seq_entries[seqPlayerIndex] - 1; i++) {
                    seq_entry[seqPlayerIndex][i].priority = seq_entry[seqPlayerIndex][i + 1].priority;
                    seq_entry[seqPlayerIndex][i].seqId = seq_entry[seqPlayerIndex][i + 1].seqId;
                }
                seq_entries[seqPlayerIndex]--;
            }

            // If the sequence was first in queue (it is currently playing),
            // Then stop the sequence and play the next sequence in the queue.
            if (found == 0) {
                Nai_StopSeqSet(seqPlayerIndex, fadeTimer);
                if (seq_entries[seqPlayerIndex] != 0) {
                    Nai_StartSeqSet(seqPlayerIndex, seq_entry[seqPlayerIndex][0].seqId,
                                        seq_entry[seqPlayerIndex][0].priority, fadeTimer);
                }
            }
            break;

        case SEQCMD_OP_SET_SEQPLAYER_VOLUME:
            // Transition volume to a target volume for an entire player
            duration = (cmd & 0xFF0000) >> 15;
            val = cmd & 0xFF;
            if (duration == 0) {
                duration++;
            }
            // Volume is scaled relative to 127
            SeqInterWork[seqPlayerIndex].volTarget = (f32)val / 127.0f;
            if (SeqInterWork[seqPlayerIndex].volCur != SeqInterWork[seqPlayerIndex].volTarget) {
                SeqInterWork[seqPlayerIndex].volStep =
                    (SeqInterWork[seqPlayerIndex].volCur - SeqInterWork[seqPlayerIndex].volTarget) / (f32)duration;
                SeqInterWork[seqPlayerIndex].volTimer = duration;
            }
            break;

        case SEQCMD_OP_SET_SEQPLAYER_FREQ:
            // Transition freq scale to a target freq for all channels
            duration = (cmd & 0xFF0000) >> 15;
            val = cmd & 0xFFFF;
            if (duration == 0) {
                duration++;
            }
            // Frequency is scaled relative to 1000
            freqScaleTarget = (f32)val / 1000.0f;
            for (i = 0; i < SEQ_NUM_CHANNELS; i++) {
                SeqInterWork[seqPlayerIndex].channelData[i].freqScaleTarget = freqScaleTarget;
                SeqInterWork[seqPlayerIndex].channelData[i].freqScaleTimer = duration;
                SeqInterWork[seqPlayerIndex].channelData[i].freqScaleStep =
                    (SeqInterWork[seqPlayerIndex].channelData[i].freqScaleCur - freqScaleTarget) / (f32)duration;
            }
            SeqInterWork[seqPlayerIndex].freqScaleChannelFlags = 0xFFFF;
            break;

        case SEQCMD_OP_SET_CHANNEL_FREQ:
            // Transition freq scale to a target for a specific channel
            duration = (cmd & 0xFF0000) >> 15;
            channelIndex = (cmd & 0xF000) >> 12;
            val = cmd & 0xFFF;
            if (duration == 0) {
                duration++;
            }
            // Frequency is scaled relative to 1000
            freqScaleTarget = (f32)val / 1000.0f;
            SeqInterWork[seqPlayerIndex].channelData[channelIndex].freqScaleTarget = freqScaleTarget;
            SeqInterWork[seqPlayerIndex].channelData[channelIndex].freqScaleStep =
                (SeqInterWork[seqPlayerIndex].channelData[channelIndex].freqScaleCur - freqScaleTarget) / (f32)duration;
            SeqInterWork[seqPlayerIndex].channelData[channelIndex].freqScaleTimer = duration;
            SeqInterWork[seqPlayerIndex].freqScaleChannelFlags |= 1 << channelIndex;
            break;

        case SEQCMD_OP_SET_CHANNEL_VOLUME:
            // Transition volume to a target volume for a specific channel
            duration = (cmd & 0xFF0000) >> 15;
            channelIndex = (cmd & 0xF00) >> 8;
            val = cmd & 0xFF;
            if (duration == 0) {
                duration++;
            }
            // Volume is scaled relative to 127
            SeqInterWork[seqPlayerIndex].channelData[channelIndex].volTarget = (f32)val / 127.0f;
            if (SeqInterWork[seqPlayerIndex].channelData[channelIndex].volCur !=
                SeqInterWork[seqPlayerIndex].channelData[channelIndex].volTarget) {
                SeqInterWork[seqPlayerIndex].channelData[channelIndex].volStep =
                    (SeqInterWork[seqPlayerIndex].channelData[channelIndex].volCur -
                     SeqInterWork[seqPlayerIndex].channelData[channelIndex].volTarget) /
                    (f32)duration;
                SeqInterWork[seqPlayerIndex].channelData[channelIndex].volTimer = duration;
                SeqInterWork[seqPlayerIndex].volChannelFlags |= 1 << channelIndex;
            }
            break;

        case SEQCMD_OP_SET_SEQPLAYER_IO:
            // Set global io port
            ioPort = (cmd & 0xFF0000) >> 16;
            val = cmd & 0xFF;
            AUDIOCMD_SEQPLAYER_SET_IO(seqPlayerIndex, ioPort, val);
            break;

        case SEQCMD_OP_SET_CHANNEL_IO:
            // Set io port if channel masked
            channelIndex = (cmd & 0xF00) >> 8;
            ioPort = (cmd & 0xFF0000) >> 16;
            val = cmd & 0xFF;
            if (!(SeqInterWork[seqPlayerIndex].channelPortMask & (1 << channelIndex))) {
                AUDIOCMD_CHANNEL_SET_IO(seqPlayerIndex, (u32)channelIndex, ioPort, val);
            }
            break;

        case SEQCMD_OP_SET_CHANNEL_IO_DISABLE_MASK:
            // Disable channel io specifically for `SEQCMD_OP_SET_CHANNEL_IO`.
            // This can be bypassed by setting channel io through using `AUDIOCMD_CHANNEL_SET_IO` directly.
            // This is accomplished by setting a channel mask.
            SeqInterWork[seqPlayerIndex].channelPortMask = cmd & 0xFFFF;
            break;

        case SEQCMD_OP_SET_CHANNEL_DISABLE_MASK:
            // Disable or Reenable channels

            // Disable channels
            channelMaskDisable = cmd & 0xFFFF;
            if (channelMaskDisable != 0) {
                // Apply channel mask `channelMaskDisable`
                AUDIOCMD_GLOBAL_SET_CHANNEL_MASK(seqPlayerIndex, channelMaskDisable);
                // Disable channels
                AUDIOCMD_CHANNEL_SET_MUTE(seqPlayerIndex, AUDIOCMD_ALL_CHANNELS, true);
            }

            // Reenable channels
            channelMaskEnable = (channelMaskDisable ^ 0xFFFF);
            if (channelMaskEnable != 0) {
                // Apply channel mask `channelMaskEnable`
                AUDIOCMD_GLOBAL_SET_CHANNEL_MASK(seqPlayerIndex, channelMaskEnable);
                // Enable channels
                AUDIOCMD_CHANNEL_SET_MUTE(seqPlayerIndex, AUDIOCMD_ALL_CHANNELS, false);
            }
            break;

        case SEQCMD_OP_TEMPO_CMD:
            // Update a tempo using a sub-command system.
            // Stores the cmd for processing elsewhere.
            SeqInterWork[seqPlayerIndex].tempoCmd = cmd;
            break;

        case SEQCMD_OP_SETUP_CMD:
            // Queue a sub-command to execute once the sequence is finished playing
            subOp = (cmd & 0xF00000) >> 20;
            if (subOp != SEQCMD_SUB_OP_SETUP_RESET_SETUP_CMDS) {
                // Ensure the maximum number of setup commands is not exceeded
                if (SeqInterWork[seqPlayerIndex].setupCmdNum < (ARRAY_COUNT(SeqInterWork[seqPlayerIndex].setupCmd) - 1)) {
                    found = SeqInterWork[seqPlayerIndex].setupCmdNum++;
                    if (found < ARRAY_COUNT(SeqInterWork[seqPlayerIndex].setupCmd)) {
                        SeqInterWork[seqPlayerIndex].setupCmd[found] = cmd;
                        // Adds a delay of 2 frames before executing any setup commands.
                        // This allows setup commands to be requested along with a new sequence on a seqPlayerIndex.
                        // This 2 frame delay ensures the player is enabled before its state is checked for
                        // the purpose of deciding if the setup commands should be run.
                        // Otherwise, the setup commands will be executed before the sequence starts,
                        // when the player is still disabled, instead of when the newly played sequence ends.
                        SeqInterWork[seqPlayerIndex].setupCmdTimer = 2;
                    }
                }
            } else {
                // `SEQCMD_SUB_OP_SETUP_RESET_SETUP_CMDS`
                // Discard all setup command requests on `seqPlayerIndex`
                SeqInterWork[seqPlayerIndex].setupCmdNum = 0;
            }
            break;

        case SEQCMD_OP_GLOBAL_CMD:
            // Apply a command that applies to all sequence players
            subOp = (cmd & 0xF00) >> 8;
            val = cmd & 0xFF;
            switch (subOp) {
                case SEQCMD_SUB_OP_GLOBAL_SET_SOUND_MODE:
                    // Set sound mode
                    AUDIOCMD_GLOBAL_SET_SOUND_MODE(SndOutTbl[val]);
                    break;

                case SEQCMD_SUB_OP_GLOBAL_DISABLE_NEW_SEQUENCES:
                    // Disable the starting of new sequences (except for the sfx player)
                    SeqStartCancel = val & 1;
                    break;
            }
            break;

        case SEQCMD_OP_RESET_AUDIO_HEAP:
            // Resets the audio heap based on the audio specifications and sfx channel layout
            specId = cmd & 0xFF;
            se_scene_mode = (cmd & 0xFF00) >> 8;
            oldSpecId = SndSpecNum;
            SndSpecNum = specId;
            Nap_StartSpecChange(specId);
            Nai_StartSpecMode(oldSpecId);
            AUDIOCMD_GLOBAL_STOP_AUDIOCMDS();
            break;
    }
}

/**
 * Add the sequence cmd to the `SeqFlagBuffer` queue
 */
void Nai_SeqFlagEntry(u32 cmd) {
    SeqFlagBuffer[SeqFlagWritePoint++] = cmd;
}

void Nai_SeqBufFlagCheck(void) {
    while (SeqFlagWritePoint != SeqFlagReadPoint) {
        Nai_SeqControl(SeqFlagBuffer[SeqFlagReadPoint++]);
    }
}

u16 Nai_GetPlayingSeqFlag(u8 seqPlayerIndex) {
    if (!AG.seqPlayers[seqPlayerIndex].enabled) {
        return NA_BGM_DISABLED;
    }
    return SeqInterWork[seqPlayerIndex].seqId;
}

s32 Nai_CheckSeqFlagBuffer2(u32 cmdVal, u32 cmdMask) {
    u8 i;

    for (i = SeqFlagReadPoint; i != SeqFlagWritePoint; i++) {
        if ((SeqFlagBuffer[i] & cmdMask) == cmdVal) {
            return false;
        }
    }

    return true;
}

// Unused
void Nai_ClearAllSeqFlag(u8 seqPlayerIndex) {
    seq_entries[seqPlayerIndex] = 0;
}

/**
 * Check if the setup command is queued. If it is, then replace the command
 * with `SEQCMD_SUB_OP_SETUP_RESTORE_SEQPLAYER_VOLUME`.
 * Unused
 */
void Nai_ClearSeqECheckFlag(u8 seqPlayerIndex, u8 setupOpDisabled) {
    u8 i;

    for (i = 0; i < SeqInterWork[seqPlayerIndex].setupCmdNum; i++) {
        u8 setupOp = (SeqInterWork[seqPlayerIndex].setupCmd[i] & 0xF00000) >> 20;

        if (setupOp == setupOpDisabled) {
            SeqInterWork[seqPlayerIndex].setupCmd[i] = 0xFF000000;
        }
    }
}

void Nai_SetSeqLineVolMove(u8 seqPlayerIndex, u8 scaleIndex, u8 targetVol, u8 volFadeTimer) {
    f32 volScale;
    u8 i;

    SeqInterWork[seqPlayerIndex].volScales[scaleIndex] = targetVol & 0x7F;

    if (volFadeTimer != 0) {
        SeqInterWork[seqPlayerIndex].fadeVolUpdate = true;
        SeqInterWork[seqPlayerIndex].volFadeTimer = volFadeTimer;
    } else {
        for (i = 0, volScale = 1.0f; i < VOL_SCALE_INDEX_MAX; i++) {
            volScale *= SeqInterWork[seqPlayerIndex].volScales[i] / 127.0f;
        }

        SEQCMD_SET_SEQPLAYER_VOLUME_NOW(seqPlayerIndex, volFadeTimer, volScale);
    }
}

/**
 * Update different commands and requests for active sequences
 */
void Nai_SeqGameFrameWork(void) {
    u32 tempoCmd;
    u8 tempoOp;
    u16 tempoPrev;
    u16 tempoTarget;
    u8 setupOp;
    u8 targetSeqPlayerIndex;
    u8 setupVal2;
    u8 setupVal1;
    u16 seqId;
    s32 pad[2];
    u16 channelMask;
    u32 retMsg;
    f32 volume;
    u8 tempoTimer;
    u8 seqPlayerIndex;
    u8 j;
    u8 channelIndex;

    for (seqPlayerIndex = 0; seqPlayerIndex < 4; seqPlayerIndex++) {

        // The setup for this block of code was not fully implemented until Majora's Mask.
        // The intent was to load soundfonts asynchronously before playing a
        // sequence in `Nai_StartSeqSet` using `(seqArgs & 0x80)`.
        // Checks if the requested sequence is finished loading fonts
        if (SeqInterWork[seqPlayerIndex].isWaitingForFonts) {
            switch (Nap_GetEvent(&retMsg)) {
                case SEQ_PLAYER_BGM_MAIN + 1:
                case SEQ_PLAYER_FANFARE + 1:
                case SEQ_PLAYER_SFX + 1:
                case SEQ_PLAYER_BGM_SUB + 1:
                    // The fonts have been loaded successfully.
                    SeqInterWork[seqPlayerIndex].isWaitingForFonts = false;
                    // Queue the same command that was stored previously
                    // The code to store this command is missing in OoT, so no command is executed
                    Nai_SeqControl(SeqInterWork[seqPlayerIndex].startSeqCmd);
                    break;
            }
        }

        // Update global volume
        if (SeqInterWork[seqPlayerIndex].fadeVolUpdate) {
            volume = 1.0f;
            for (j = 0; j < VOL_SCALE_INDEX_MAX; j++) {
                volume *= (SeqInterWork[seqPlayerIndex].volScales[j] / 127.0f);
            }
            SEQCMD_SET_SEQPLAYER_VOLUME(seqPlayerIndex, SeqInterWork[seqPlayerIndex].volFadeTimer,
                                        (u8)(volume * 127.0f));
            SeqInterWork[seqPlayerIndex].fadeVolUpdate = false;
        }

        if (SeqInterWork[seqPlayerIndex].volTimer != 0) {
            SeqInterWork[seqPlayerIndex].volTimer--;

            if (SeqInterWork[seqPlayerIndex].volTimer != 0) {
                SeqInterWork[seqPlayerIndex].volCur -= SeqInterWork[seqPlayerIndex].volStep;
            } else {
                SeqInterWork[seqPlayerIndex].volCur = SeqInterWork[seqPlayerIndex].volTarget;
            }

            AUDIOCMD_SEQPLAYER_FADE_VOLUME_SCALE((u32)seqPlayerIndex, SeqInterWork[seqPlayerIndex].volCur);
        }

        // Process tempo
        if (SeqInterWork[seqPlayerIndex].tempoCmd != 0) {
            tempoCmd = SeqInterWork[seqPlayerIndex].tempoCmd;
            tempoTimer = (tempoCmd & 0xFF0000) >> 15;
            tempoTarget = tempoCmd & 0xFFF;
            if (tempoTimer == 0) {
                tempoTimer++;
            }

            // Process tempo commands
            if (AG.seqPlayers[seqPlayerIndex].enabled) {
                tempoPrev = AG.seqPlayers[seqPlayerIndex].tempo / SEQTICKS_PER_BEAT;
                tempoOp = (tempoCmd & 0xF000) >> 12;
                switch (tempoOp) {
                    case SEQCMD_SUB_OP_TEMPO_SPEED_UP:
                        // Speed up tempo by `tempoTarget` amount
                        tempoTarget += tempoPrev;
                        break;

                    case SEQCMD_SUB_OP_TEMPO_SLOW_DOWN:
                        // Slow down tempo by `tempoTarget` amount
                        if (tempoTarget < tempoPrev) {
                            tempoTarget = tempoPrev - tempoTarget;
                        }
                        break;

                    case SEQCMD_SUB_OP_TEMPO_SCALE:
                        // Scale tempo by a multiplicative factor
                        tempoTarget = tempoPrev * (tempoTarget / 100.0f);
                        break;

                    case SEQCMD_SUB_OP_TEMPO_RESET:
                        // Reset tempo to original tempo
                        tempoTarget = (SeqInterWork[seqPlayerIndex].tempoOriginal != 0)
                                          ? SeqInterWork[seqPlayerIndex].tempoOriginal
                                          : tempoPrev;
                        break;

                    default: // `SEQCMD_SUB_OP_TEMPO_SET`
                        // `tempoTarget` is the new tempo
                        break;
                }

                if (tempoTarget > 300) {
                    tempoTarget = 300;
                }

                if (SeqInterWork[seqPlayerIndex].tempoOriginal == 0) {
                    SeqInterWork[seqPlayerIndex].tempoOriginal = tempoPrev;
                }

                SeqInterWork[seqPlayerIndex].tempoTarget = tempoTarget;
                SeqInterWork[seqPlayerIndex].tempoCur = AG.seqPlayers[seqPlayerIndex].tempo / SEQTICKS_PER_BEAT;
                SeqInterWork[seqPlayerIndex].tempoStep =
                    (SeqInterWork[seqPlayerIndex].tempoCur - SeqInterWork[seqPlayerIndex].tempoTarget) / tempoTimer;
                SeqInterWork[seqPlayerIndex].tempoTimer = tempoTimer;
                SeqInterWork[seqPlayerIndex].tempoCmd = 0;
            }
        }

        // Step tempo to target
        if (SeqInterWork[seqPlayerIndex].tempoTimer != 0) {
            SeqInterWork[seqPlayerIndex].tempoTimer--;
            if (SeqInterWork[seqPlayerIndex].tempoTimer != 0) {
                SeqInterWork[seqPlayerIndex].tempoCur -= SeqInterWork[seqPlayerIndex].tempoStep;
            } else {
                SeqInterWork[seqPlayerIndex].tempoCur = SeqInterWork[seqPlayerIndex].tempoTarget;
            }

            AUDIOCMD_SEQPLAYER_SET_TEMPO((u32)seqPlayerIndex, SeqInterWork[seqPlayerIndex].tempoCur);
        }

        // Update channel volumes
        if (SeqInterWork[seqPlayerIndex].volChannelFlags != 0) {
            for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
                if (SeqInterWork[seqPlayerIndex].channelData[channelIndex].volTimer != 0) {
                    SeqInterWork[seqPlayerIndex].channelData[channelIndex].volTimer--;
                    if (SeqInterWork[seqPlayerIndex].channelData[channelIndex].volTimer != 0) {
                        SeqInterWork[seqPlayerIndex].channelData[channelIndex].volCur -=
                            SeqInterWork[seqPlayerIndex].channelData[channelIndex].volStep;
                    } else {
                        SeqInterWork[seqPlayerIndex].channelData[channelIndex].volCur =
                            SeqInterWork[seqPlayerIndex].channelData[channelIndex].volTarget;
                        SeqInterWork[seqPlayerIndex].volChannelFlags ^= (1 << channelIndex);
                    }

                    AUDIOCMD_CHANNEL_SET_VOL_SCALE(seqPlayerIndex, (u32)channelIndex,
                                                   SeqInterWork[seqPlayerIndex].channelData[channelIndex].volCur);
                }
            }
        }

        // Update frequencies
        if (SeqInterWork[seqPlayerIndex].freqScaleChannelFlags != 0) {
            for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
                if (SeqInterWork[seqPlayerIndex].channelData[channelIndex].freqScaleTimer != 0) {
                    SeqInterWork[seqPlayerIndex].channelData[channelIndex].freqScaleTimer--;
                    if (SeqInterWork[seqPlayerIndex].channelData[channelIndex].freqScaleTimer != 0) {
                        SeqInterWork[seqPlayerIndex].channelData[channelIndex].freqScaleCur -=
                            SeqInterWork[seqPlayerIndex].channelData[channelIndex].freqScaleStep;
                    } else {
                        SeqInterWork[seqPlayerIndex].channelData[channelIndex].freqScaleCur =
                            SeqInterWork[seqPlayerIndex].channelData[channelIndex].freqScaleTarget;
                        SeqInterWork[seqPlayerIndex].freqScaleChannelFlags ^= (1 << channelIndex);
                    }

                    AUDIOCMD_CHANNEL_SET_FREQ_SCALE(seqPlayerIndex, (u32)channelIndex,
                                                    SeqInterWork[seqPlayerIndex].channelData[channelIndex].freqScaleCur);
                }
            }
        }

        // Process setup commands
        if (SeqInterWork[seqPlayerIndex].setupCmdNum != 0) {
            // If there is a SeqCmd to reset the audio heap queued, then drop all setup commands
            if (!Nai_CheckSeqFlagBuffer2(SEQCMD_OP_RESET_AUDIO_HEAP << 28, SEQCMD_OP_MASK)) {
                SeqInterWork[seqPlayerIndex].setupCmdNum = 0;
                return;
            }

            // Only process setup commands once the timer reaches zero
            if (SeqInterWork[seqPlayerIndex].setupCmdTimer != 0) {
                SeqInterWork[seqPlayerIndex].setupCmdTimer--;
                continue;
            }

            // Only process setup commands if `seqPlayerIndex` if no longer playing
            // i.e. the `seqPlayer` is no longer enabled
            if (AG.seqPlayers[seqPlayerIndex].enabled) {
                continue;
            }

            for (j = 0; j < SeqInterWork[seqPlayerIndex].setupCmdNum; j++) {
                setupOp = (SeqInterWork[seqPlayerIndex].setupCmd[j] & 0x00F00000) >> 20;
                targetSeqPlayerIndex = (SeqInterWork[seqPlayerIndex].setupCmd[j] & 0x000F0000) >> 16;
                setupVal2 = (SeqInterWork[seqPlayerIndex].setupCmd[j] & 0xFF00) >> 8;
                setupVal1 = SeqInterWork[seqPlayerIndex].setupCmd[j] & 0xFF;

                switch (setupOp) {
                    case SEQCMD_SUB_OP_SETUP_RESTORE_SEQPLAYER_VOLUME:
                        // Restore `targetSeqPlayerIndex` volume back to normal levels
                        Nai_SetSeqLineVolMove(targetSeqPlayerIndex, VOL_SCALE_INDEX_FANFARE, 0x7F, setupVal1);
                        break;

                    case SEQCMD_SUB_OP_SETUP_RESTORE_SEQPLAYER_VOLUME_IF_QUEUED:
                        // Restore `targetSeqPlayerIndex` volume back to normal levels,
                        // but only if the number of sequence queue requests from `seq_entry`
                        // exactly matches the argument to the command
                        if (setupVal1 == seq_entries[seqPlayerIndex]) {
                            Nai_SetSeqLineVolMove(targetSeqPlayerIndex, VOL_SCALE_INDEX_FANFARE, 0x7F, setupVal2);
                        }
                        break;

                    case SEQCMD_SUB_OP_SETUP_SEQ_UNQUEUE:
                        // Unqueue `seqPlayerIndex` from seq_entry
                        //! @bug this command does not work as intended as unqueueing
                        //! the sequence relies on `SeqInterWork[seqPlayerIndex].seqId`
                        //! However, `SeqInterWork[seqPlayerIndex].seqId` is reset before the sequence on
                        //! `seqPlayerIndex` is requested to stop, i.e. before the sequence is disabled and setup
                        //! commands (including this command) can run. A simple fix would have been to unqueue based on
                        //! `SeqInterWork[seqPlayerIndex].prevSeqId` instead
                        SEQCMD_UNQUEUE_SEQUENCE(seqPlayerIndex, 0, SeqInterWork[seqPlayerIndex].seqId);
                        break;

                    case SEQCMD_SUB_OP_SETUP_RESTART_SEQ:
                        // Restart the currently active sequence on `targetSeqPlayerIndex` with full volume.
                        // Sequence on `targetSeqPlayerIndex` must still be active to play (can be muted)
                        SEQCMD_PLAY_SEQUENCE(targetSeqPlayerIndex, 1, 0, SeqInterWork[targetSeqPlayerIndex].seqId);
                        SeqInterWork[targetSeqPlayerIndex].fadeVolUpdate = true;
                        SeqInterWork[targetSeqPlayerIndex].volScales[VOL_SCALE_INDEX_FANFARE] = 0x7F;
                        break;

                    case SEQCMD_SUB_OP_SETUP_TEMPO_SCALE:
                        // Scale tempo by a multiplicative factor
                        SEQCMD_SCALE_TEMPO(targetSeqPlayerIndex, setupVal2, setupVal1);
                        break;

                    case SEQCMD_SUB_OP_SETUP_TEMPO_RESET:
                        // Reset tempo to previous tempo
                        SEQCMD_RESET_TEMPO(targetSeqPlayerIndex, setupVal1);
                        break;

                    case SEQCMD_SUB_OP_SETUP_PLAY_SEQ:
                        // Play the requested sequence
                        // Uses the fade timer set by `SEQCMD_SUB_OP_SETUP_SET_FADE_TIMER`
                        seqId = SeqInterWork[seqPlayerIndex].setupCmd[j] & 0xFFFF;
                        SEQCMD_PLAY_SEQUENCE(targetSeqPlayerIndex, SeqInterWork[targetSeqPlayerIndex].setupFadeTimer, 0,
                                             seqId);
                        Nai_SetSeqLineVolMove(targetSeqPlayerIndex, VOL_SCALE_INDEX_FANFARE, 0x7F, 0);
                        SeqInterWork[targetSeqPlayerIndex].setupFadeTimer = 0;
                        break;

                    case SEQCMD_SUB_OP_SETUP_SET_FADE_TIMER:
                        // A command specifically to support `SEQCMD_SUB_OP_SETUP_PLAY_SEQ`
                        // Sets the fade timer for the sequence requested in `SEQCMD_SUB_OP_SETUP_PLAY_SEQ`
                        SeqInterWork[seqPlayerIndex].setupFadeTimer = setupVal2;
                        break;

                    case SEQCMD_SUB_OP_SETUP_RESTORE_SEQPLAYER_VOLUME_WITH_SCALE_INDEX:
                        // Restore the volume back to default levels
                        // Allows a `scaleIndex` to be specified.
                        Nai_SetSeqLineVolMove(targetSeqPlayerIndex, setupVal2, 0x7F, setupVal1);
                        break;

                    case SEQCMD_SUB_OP_SETUP_POP_PERSISTENT_CACHE:
                        // Discard audio data by popping one more audio caches from the audio heap
                        if (setupVal1 & (1 << SEQUENCE_TABLE)) {
                            AUDIOCMD_GLOBAL_POP_PERSISTENT_CACHE(SEQUENCE_TABLE);
                        }
                        if (setupVal1 & (1 << FONT_TABLE)) {
                            AUDIOCMD_GLOBAL_POP_PERSISTENT_CACHE(FONT_TABLE);
                        }
                        if (setupVal1 & (1 << SAMPLE_TABLE)) {
                            AUDIOCMD_GLOBAL_POP_PERSISTENT_CACHE(SAMPLE_TABLE);
                        }
                        break;

                    case SEQCMD_SUB_OP_SETUP_SET_CHANNEL_DISABLE_MASK:
                        // Disable (or reenable) specific channels of `targetSeqPlayerIndex`
                        channelMask = SeqInterWork[seqPlayerIndex].setupCmd[j] & 0xFFFF;
                        SEQCMD_SET_CHANNEL_DISABLE_MASK(targetSeqPlayerIndex, channelMask);
                        break;

                    case SEQCMD_SUB_OP_SETUP_SET_SEQPLAYER_FREQ:
                        // Scale all channels of `targetSeqPlayerIndex`
                        SEQCMD_SET_SEQPLAYER_FREQ(targetSeqPlayerIndex, setupVal2, (setupVal1 * 10) & 0xFFFF);
                        break;
                }
            }

            SeqInterWork[seqPlayerIndex].setupCmdNum = 0;
        }
    }
}

u8 Nai_SpecBusyCheck(void) {
    if (SpecBusyFlag != 0) {
        if (SpecBusyFlag == 1) {
            if (Nap_CheckSpecChange() == 1) {
                SpecBusyFlag = 0;
                AUDIOCMD_SEQPLAYER_SET_IO(SEQ_PLAYER_SFX, 0, se_scene_mode);
                Nai_InitInterface_spec();
            }
        } else if (SpecBusyFlag == 2) {
            while (Nap_CheckSpecChange() != 1) {}
            SpecBusyFlag = 0;
            AUDIOCMD_SEQPLAYER_SET_IO(SEQ_PLAYER_SFX, 0, se_scene_mode);
            Nai_InitInterface_spec();
        }
    }

    return SpecBusyFlag;
}

void Nai_seq_inter_init_spec(void) {
    u8 seqPlayerIndex;
    u8 scaleIndex;

    for (seqPlayerIndex = 0; seqPlayerIndex < 4; seqPlayerIndex++) {
        seq_entries[seqPlayerIndex] = 0;

        SeqInterWork[seqPlayerIndex].seqId = NA_BGM_DISABLED;
        SeqInterWork[seqPlayerIndex].prevSeqId = NA_BGM_DISABLED;
        SeqInterWork[seqPlayerIndex].tempoTimer = 0;
        SeqInterWork[seqPlayerIndex].tempoOriginal = 0;
        SeqInterWork[seqPlayerIndex].tempoCmd = 0;
        SeqInterWork[seqPlayerIndex].channelPortMask = 0;
        SeqInterWork[seqPlayerIndex].setupCmdNum = 0;
        SeqInterWork[seqPlayerIndex].setupFadeTimer = 0;
        SeqInterWork[seqPlayerIndex].freqScaleChannelFlags = 0;
        SeqInterWork[seqPlayerIndex].volChannelFlags = 0;
        for (scaleIndex = 0; scaleIndex < VOL_SCALE_INDEX_MAX; scaleIndex++) {
            SeqInterWork[seqPlayerIndex].volScales[scaleIndex] = 0x7F;
        }

        SeqInterWork[seqPlayerIndex].volFadeTimer = 1;
        SeqInterWork[seqPlayerIndex].fadeVolUpdate = true;
    }
}

void Nai_seq_inter_init(void) {
    u8 seqPlayerIndex;
    u8 scaleIndex;

    for (seqPlayerIndex = 0; seqPlayerIndex < 4; seqPlayerIndex++) {
        SeqInterWork[seqPlayerIndex].volCur = 1.0f;
        SeqInterWork[seqPlayerIndex].volTimer = 0;
        SeqInterWork[seqPlayerIndex].fadeVolUpdate = false;
        for (scaleIndex = 0; scaleIndex < VOL_SCALE_INDEX_MAX; scaleIndex++) {
            SeqInterWork[seqPlayerIndex].volScales[scaleIndex] = 0x7F;
        }
    }
    Nai_seq_inter_init_spec();
}

u8 SeqFlagWritePoint = 0;
u8 SeqFlagReadPoint = 0;
u8 SeqStartCancel = false;

#if DEBUG_FEATURES
u8 gAudioDebugPrintSeqCmd = true;
#endif

u8 SndOutTbl[] = {
    SOUNDMODE_STEREO,
    SOUNDMODE_HEADSET,
    SOUNDMODE_SURROUND,
    SOUNDMODE_MONO,
};

u8 SndSpecNum = 0;

u8 SpecBusyFlag = 0;

u8 DelaySeqFlagSw[0xA0]; // unused
