#include "global.h"

void Nas_smzSetParamDirect(Note* note, NoteSubEu* sub, NoteSubAttributes* attrs) {
    f32 volLeft;
    f32 volRight;
    s32 halfPanIndex;
    u64 pad;
    u8 strongLeft;
    u8 strongRight;
    f32 vel;
    u8 pan;
    u8 reverbVol;
    StereoData stereoData;
    s32 stereoHeadsetEffects = note->playbackState.stereoHeadsetEffects;

    vel = attrs->velocity;
    pan = attrs->pan;
    reverbVol = attrs->reverbVol;
    stereoData = attrs->stereo.s;

    sub->bitField0 = note->noteSubEu.bitField0;
    sub->bitField1 = note->noteSubEu.bitField1;
    sub->waveSampleAddr = note->noteSubEu.waveSampleAddr;
    sub->harmonicIndexCurAndPrev = note->noteSubEu.harmonicIndexCurAndPrev;

    Nas_smzSetPitchDirect(sub, attrs->frequency);

    pan &= 0x7F;

    sub->bitField0.stereoStrongRight = false;
    sub->bitField0.stereoStrongLeft = false;
    sub->bitField0.stereoHeadsetEffects = stereoData.stereoHeadsetEffects;
    sub->bitField0.usesHeadsetPanEffects = stereoData.usesHeadsetPanEffects;
    if (stereoHeadsetEffects && (AG.soundMode == SOUNDMODE_HEADSET)) {
        halfPanIndex = pan >> 1;
        if (halfPanIndex > 0x3F) {
            halfPanIndex = 0x3F;
        }

        sub->haasEffectRightDelaySize = CDELAYTABLE[halfPanIndex];
        sub->haasEffectLeftDelaySize = CDELAYTABLE[0x3F - halfPanIndex];
        sub->bitField1.useHaasEffect = true;

        volLeft = PhoneLeft[pan];
        volRight = PhoneLeft[0x7F - pan];
    } else if (stereoHeadsetEffects && (AG.soundMode == SOUNDMODE_STEREO)) {
        strongLeft = strongRight = 0;
        sub->haasEffectLeftDelaySize = 0;
        sub->haasEffectRightDelaySize = 0;
        sub->bitField1.useHaasEffect = false;

        volLeft = WideLeft[pan];
        volRight = WideLeft[0x7F - pan];
        if (pan < 0x20) {
            strongLeft = 1;
        } else if (pan > 0x60) {
            strongRight = 1;
        }

        sub->bitField0.stereoStrongRight = strongRight;
        sub->bitField0.stereoStrongLeft = strongLeft;

        switch (stereoData.bit2) {
            case 0:
                break;

            case 1:
                sub->bitField0.stereoStrongRight = stereoData.strongRight;
                sub->bitField0.stereoStrongLeft = stereoData.strongLeft;
                break;

            case 2:
                sub->bitField0.stereoStrongRight = stereoData.strongRight | strongRight;
                sub->bitField0.stereoStrongLeft = stereoData.strongLeft | strongLeft;
                break;

            case 3:
                sub->bitField0.stereoStrongRight = stereoData.strongRight ^ strongRight;
                sub->bitField0.stereoStrongLeft = stereoData.strongLeft ^ strongLeft;
                break;
        }

    } else if (AG.soundMode == SOUNDMODE_MONO) {
        sub->bitField0.stereoHeadsetEffects = false;
        sub->bitField0.usesHeadsetPanEffects = false;
        volLeft = 0.707f; // approx 1/sqrt(2)
        volRight = 0.707f;
    } else {
        sub->bitField0.stereoStrongRight = stereoData.strongRight;
        sub->bitField0.stereoStrongLeft = stereoData.strongLeft;
        volLeft = StereoLeft[pan];
        volRight = StereoLeft[0x7F - pan];
    }

    vel = 0.0f > vel ? 0.0f : vel;
    vel = 1.0f < vel ? 1.0f : vel;

    sub->targetVolLeft = (s32)((vel * volLeft) * (0x1000 - 0.001f));
    sub->targetVolRight = (s32)((vel * volRight) * (0x1000 - 0.001f));

    sub->gain = attrs->gain;
    sub->filter = attrs->filter;
    sub->combFilterSize = attrs->combFilterSize;
    sub->combFilterGain = attrs->combFilterGain;
    sub->reverbVol = reverbVol;
}

void Nas_smzSetPitchDirect(NoteSubEu* noteSubEu, f32 resamplingRateInput) {
    f32 resamplingRate = 0.0f;

    if (resamplingRateInput < 2.0f) {
        noteSubEu->bitField1.hasTwoParts = false;
        resamplingRate = CLAMP_MAX(resamplingRateInput, 1.99998f);

    } else {
        noteSubEu->bitField1.hasTwoParts = true;
        if (resamplingRateInput > 3.99996f) {
            resamplingRate = 1.99998f;
        } else {
            resamplingRate = resamplingRateInput * 0.5f;
        }
    }
    noteSubEu->resamplingRateFixedPoint = (s32)(resamplingRate * 32768.0f);
}

void Nas_StartVoice(Note* note) {
    if (note->playbackState.parentLayer->adsr.decayIndex == 0) {
        Nas_EnvInit(&note->playbackState.adsr, note->playbackState.parentLayer->channel->adsr.envelope,
                       &note->playbackState.adsrVolScaleUnused);
    } else {
        Nas_EnvInit(&note->playbackState.adsr, note->playbackState.parentLayer->adsr.envelope,
                       &note->playbackState.adsrVolScaleUnused);
    }

    note->playbackState.unk_04 = 0;
    note->playbackState.adsr.action.s.state = ADSR_STATE_INITIAL;
    note->noteSubEu = NA_SVCINIT_TABLE;
}

void Nas_StopVoice(Note* note) {
    if (note->noteSubEu.bitField0.needsInit == true) {
        note->noteSubEu.bitField0.needsInit = false;
    }
    note->playbackState.priority = 0;
    note->noteSubEu.bitField0.enabled = false;
    note->playbackState.unk_04 = 0;
    note->noteSubEu.bitField0.finished = false;
    note->playbackState.parentLayer = NO_LAYER;
    note->playbackState.prevParentLayer = NO_LAYER;
    note->playbackState.adsr.action.s.state = ADSR_STATE_DISABLED;
    note->playbackState.adsr.current = 0;
}

void Nas_UpdateChannel(void) {
    s32 pad[2];
    NoteAttributes* attrs;
    NoteSubEu* noteSubEu2;
    NoteSubEu* noteSubEu;
    Note* note;
    NotePlaybackState* playbackState;
    NoteSubAttributes subAttrs;
    u8 bookOffset;
    f32 scale;
    s32 i;

    for (i = 0; i < AG.numNotes; i++) {
        note = &AG.notes[i];
        noteSubEu2 = &AG.noteSubsEu[AG.noteSubEuOffset + i];
        playbackState = &note->playbackState;
        if (playbackState->parentLayer != NO_LAYER) {
            if ((u32)playbackState->parentLayer < 0x7FFFFFFF) {
                continue;
            }

            if (note != playbackState->parentLayer->note && playbackState->unk_04 == 0) {
                playbackState->adsr.action.s.release = true;
                playbackState->adsr.fadeOutVel = AG.audioBufferParameters.ticksPerUpdateInv;
                playbackState->priority = 1;
                playbackState->unk_04 = 2;
                goto out;
            } else if (!playbackState->parentLayer->enabled && playbackState->unk_04 == 0 &&
                       playbackState->priority >= 1) {
                // do nothing
            } else if (playbackState->parentLayer->channel->seqPlayer == NULL) {
                Nas_ReleaseSubTrack(playbackState->parentLayer->channel);
                playbackState->priority = 1;
                playbackState->unk_04 = 1;
                continue;
            } else if (playbackState->parentLayer->channel->seqPlayer->muted &&
                       (playbackState->parentLayer->channel->muteBehavior & MUTE_BEHAVIOR_STOP_NOTES)) {
                // do nothing
            } else {
                goto out;
            }

            Nas_Release_Channel_Force(playbackState->parentLayer);
            Nas_CutList(&note->listItem);
            Nas_AddListHead(&note->listItem.pool->decaying, &note->listItem);
            playbackState->priority = 1;
            playbackState->unk_04 = 2;
        } else if (playbackState->unk_04 == 0 && playbackState->priority >= 1) {
            continue;
        }

    out:
        if (playbackState->priority != 0) {
            if (1) {}
            noteSubEu = &note->noteSubEu;
            if (playbackState->unk_04 >= 1 || noteSubEu->bitField0.finished) {
                if (playbackState->adsr.action.s.state == ADSR_STATE_DISABLED || noteSubEu->bitField0.finished) {
                    if (playbackState->wantedParentLayer != NO_LAYER) {
                        Nas_StopVoice(note);
                        if (playbackState->wantedParentLayer->channel != NULL) {
                            Nas_EntryTrack(note, playbackState->wantedParentLayer);
                            Nas_ChannelModInit(note);
                            Nas_SweepInit(note);
                            Nas_CutList(&note->listItem);
                            Nas_AddList(&note->listItem.pool->active, &note->listItem);
                            playbackState->wantedParentLayer = NO_LAYER;
                            // don't skip
                        } else {
                            Nas_StopVoice(note);
                            Nas_CutList(&note->listItem);
                            Nas_AddList(&note->listItem.pool->disabled, &note->listItem);
                            playbackState->wantedParentLayer = NO_LAYER;
                            goto skip;
                        }
                    } else {
                        if (playbackState->parentLayer != NO_LAYER) {
                            playbackState->parentLayer->bit1 = true;
                        }
                        Nas_StopVoice(note);
                        Nas_CutList(&note->listItem);
                        Nas_AddList(&note->listItem.pool->disabled, &note->listItem);
                        continue;
                    }
                }
            } else if (playbackState->adsr.action.s.state == ADSR_STATE_DISABLED) {
                if (playbackState->parentLayer != NO_LAYER) {
                    playbackState->parentLayer->bit1 = true;
                }
                Nas_StopVoice(note);
                Nas_CutList(&note->listItem);
                Nas_AddList(&note->listItem.pool->disabled, &note->listItem);
                continue;
            }

            scale = Nas_EnvProcess(&playbackState->adsr);
            Nas_ChannelModulation(note);
            attrs = &playbackState->attributes;
            if (playbackState->unk_04 == 1 || playbackState->unk_04 == 2) {
                subAttrs.frequency = attrs->freqScale;
                subAttrs.velocity = attrs->velocity;
                subAttrs.pan = attrs->pan;
                subAttrs.reverbVol = attrs->reverb;
                subAttrs.stereo = attrs->stereo;
                subAttrs.gain = attrs->gain;
                subAttrs.filter = attrs->filter;
                subAttrs.combFilterSize = attrs->combFilterSize;
                subAttrs.combFilterGain = attrs->combFilterGain;
                bookOffset = noteSubEu->bitField1.bookOffset;
            } else {
                SequenceLayer* layer = playbackState->parentLayer;
                SequenceChannel* channel = layer->channel;

                subAttrs.frequency = layer->noteFreqScale;
                subAttrs.velocity = layer->noteVelocity;
                subAttrs.pan = layer->notePan;
                if (layer->stereo.asByte == 0) {
                    subAttrs.stereo = channel->stereo;
                } else {
                    subAttrs.stereo = layer->stereo;
                }
                subAttrs.reverbVol = channel->targetReverbVol;
                subAttrs.gain = channel->gain;
                subAttrs.filter = channel->filter;
                subAttrs.combFilterSize = channel->combFilterSize;
                subAttrs.combFilterGain = channel->combFilterGain;
                bookOffset = channel->bookOffset & 0x7;

                if (channel->seqPlayer->muted && (channel->muteBehavior & MUTE_BEHAVIOR_3)) {
                    subAttrs.frequency = 0.0f;
                    subAttrs.velocity = 0.0f;
                }
            }

            subAttrs.frequency *= playbackState->vibratoFreqScale * playbackState->portamentoFreqScale;
            subAttrs.frequency *= AG.audioBufferParameters.resampleRate;
            subAttrs.velocity *= scale;
            Nas_smzSetParamDirect(note, noteSubEu2, &subAttrs);
            noteSubEu->bitField1.bookOffset = bookOffset;
        skip:;
        }
    }
}

TunedSample* NoteToVoice(Instrument* instrument, s32 semitone) {
    TunedSample* tunedSample;

    if (semitone < instrument->normalRangeLo) {
        tunedSample = &instrument->lowPitchTunedSample;
    } else if (semitone <= instrument->normalRangeHi) {
        tunedSample = &instrument->normalPitchTunedSample;
    } else {
        tunedSample = &instrument->highPitchTunedSample;
    }
    return tunedSample;
}

Instrument* ProgToVp(s32 fontId, s32 instId) {
    Instrument* inst;

    if (fontId == 0xFF) {
        return NULL;
    }

    if (!Nas_CheckIDbank(fontId)) {
        AG.audioErrorFlags = fontId + 0x10000000;
        return NULL;
    }

    if (instId >= AG.soundFontList[fontId].numInstruments) {
        AG.audioErrorFlags = ((fontId << 8) + instId) + 0x3000000;
        return NULL;
    }

    inst = AG.soundFontList[fontId].instruments[instId];
    if (inst == NULL) {
        AG.audioErrorFlags = ((fontId << 8) + instId) + 0x1000000;
        return inst;
    }

    return inst;
}

Drum* PercToPp(s32 fontId, s32 drumId) {
    Drum* drum;

    if (fontId == 0xFF) {
        return NULL;
    }

    if (!Nas_CheckIDbank(fontId)) {
        AG.audioErrorFlags = fontId + 0x10000000;
        return NULL;
    }

    if (drumId >= AG.soundFontList[fontId].numDrums) {
        AG.audioErrorFlags = ((fontId << 8) + drumId) + 0x4000000;
        return NULL;
    }
    if ((u32)AG.soundFontList[fontId].drums < AUDIO_RELOCATED_ADDRESS_START) {
        return NULL;
    }
    drum = AG.soundFontList[fontId].drums[drumId];

    if (drum == NULL) {
        AG.audioErrorFlags = ((fontId << 8) + drumId) + 0x5000000;
    }

    return drum;
}

SoundEffect* VpercToVep(s32 fontId, s32 sfxId) {
    SoundEffect* soundEffect;

    if (fontId == 0xFF) {
        return NULL;
    }

    if (!Nas_CheckIDbank(fontId)) {
        AG.audioErrorFlags = fontId + 0x10000000;
        return NULL;
    }

    if (sfxId >= AG.soundFontList[fontId].numSfx) {
        AG.audioErrorFlags = ((fontId << 8) + sfxId) + 0x4000000;
        return NULL;
    }

    if ((u32)AG.soundFontList[fontId].soundEffects < AUDIO_RELOCATED_ADDRESS_START) {
        return NULL;
    }

    soundEffect = &AG.soundFontList[fontId].soundEffects[sfxId];

    if (soundEffect == NULL) {
        AG.audioErrorFlags = ((fontId << 8) + sfxId) + 0x5000000;
    }

    if (soundEffect->tunedSample.sample == NULL) {
        return NULL;
    }

    return soundEffect;
}

s32 OverwriteBank(s32 instrumentType, s32 fontId, s32 index, void* value) {
    if (fontId == 0xFF) {
        return -1;
    }

    if (!Nas_CheckIDbank(fontId)) {
        return -2;
    }

    switch (instrumentType) {
        case 0:
            if (index >= AG.soundFontList[fontId].numDrums) {
                return -3;
            }
            AG.soundFontList[fontId].drums[index] = value;
            break;

        case 1:
            if (index >= AG.soundFontList[fontId].numSfx) {
                return -3;
            }
            AG.soundFontList[fontId].soundEffects[index] = *(SoundEffect*)value;
            break;

        default:
            if (index >= AG.soundFontList[fontId].numInstruments) {
                return -3;
            }
            AG.soundFontList[fontId].instruments[index] = value;
            break;
    }

    return 0;
}

void __Nas_Release_Channel_Main(SequenceLayer* layer, s32 target) {
    Note* note;
    NoteAttributes* attrs;
    SequenceChannel* channel;
    s32 i;

    if (layer == NO_LAYER) {
        return;
    }

    layer->bit3 = false;

    if (layer->note == NULL) {
        return;
    }

    note = layer->note;
    attrs = &note->playbackState.attributes;

    if (note->playbackState.wantedParentLayer == layer) {
        note->playbackState.wantedParentLayer = NO_LAYER;
    }

    if (note->playbackState.parentLayer != layer) {
        if (note->playbackState.parentLayer == NO_LAYER && note->playbackState.wantedParentLayer == NO_LAYER &&
            note->playbackState.prevParentLayer == layer && target != ADSR_STATE_DECAY) {
            note->playbackState.adsr.fadeOutVel = AG.audioBufferParameters.ticksPerUpdateInv;
            note->playbackState.adsr.action.s.release = true;
        }
        return;
    }

    if (note->playbackState.adsr.action.s.state != ADSR_STATE_DECAY) {
        attrs->freqScale = layer->noteFreqScale;
        attrs->velocity = layer->noteVelocity;
        attrs->pan = layer->notePan;

        if (layer->channel != NULL) {
            channel = layer->channel;
            attrs->reverb = channel->targetReverbVol;
            attrs->gain = channel->gain;
            attrs->filter = channel->filter;

            if (attrs->filter != NULL) {
                for (i = 0; i < 8; i++) {
                    attrs->filterBuf[i] = attrs->filter[i];
                }
                attrs->filter = attrs->filterBuf;
            }

            attrs->combFilterGain = channel->combFilterGain;
            attrs->combFilterSize = channel->combFilterSize;
            if (channel->seqPlayer->muted && (channel->muteBehavior & MUTE_BEHAVIOR_3)) {
                note->noteSubEu.bitField0.finished = true;
            }

            if (layer->stereo.asByte == 0) {
                attrs->stereo = channel->stereo;
            } else {
                attrs->stereo = layer->stereo;
            }
            note->playbackState.priority = channel->someOtherPriority;
        } else {
            attrs->stereo = layer->stereo;
            note->playbackState.priority = 1;
        }

        note->playbackState.prevParentLayer = note->playbackState.parentLayer;
        note->playbackState.parentLayer = NO_LAYER;
        if (target == ADSR_STATE_RELEASE) {
            note->playbackState.adsr.fadeOutVel = AG.audioBufferParameters.ticksPerUpdateInv;
            note->playbackState.adsr.action.s.release = true;
            note->playbackState.unk_04 = 2;
        } else {
            note->playbackState.unk_04 = 1;
            note->playbackState.adsr.action.s.decay = true;
            if (layer->adsr.decayIndex == 0) {
                note->playbackState.adsr.fadeOutVel = AG.adsrDecayTable[layer->channel->adsr.decayIndex];
            } else {
                note->playbackState.adsr.fadeOutVel = AG.adsrDecayTable[layer->adsr.decayIndex];
            }
            note->playbackState.adsr.sustain =
                ((f32)(s32)(layer->channel->adsr.sustain) * note->playbackState.adsr.current) / 256.0f;
        }
    }

    if (target == ADSR_STATE_DECAY) {
        Nas_CutList(&note->listItem);
        Nas_AddListHead(&note->listItem.pool->decaying, &note->listItem);
    }
}

void Nas_Release_Channel(SequenceLayer* layer) {
    __Nas_Release_Channel_Main(layer, ADSR_STATE_DECAY);
}

void Nas_Release_Channel_Force(SequenceLayer* layer) {
    __Nas_Release_Channel_Main(layer, ADSR_STATE_RELEASE);
}

/**
 * Extract the synthetic wave to use from WAVEMEM_TABLE and update corresponding frequencies
 *
 * @param note
 * @param layer
 * @param waveId the index of the type of synthetic wave to use, offset by 128
 * @return harmonicIndex, the index of the harmonic for the synthetic wave contained in WAVEMEM_TABLE
 */
s32 Nas_WaveMemoryMake(Note* note, SequenceLayer* layer, s32 waveId) {
    f32 freqScale;
    f32 freqRatio;
    u8 harmonicIndex;

    if (waveId < 128) {
        waveId = 128;
    }

    freqScale = layer->freqScale;
    if (layer->portamento.mode != 0 && 0.0f < layer->portamento.extent) {
        freqScale *= (layer->portamento.extent + 1.0f);
    }

    // Map frequency to the harmonic to use from WAVEMEM_TABLE
    if (freqScale < 0.99999f) {
        harmonicIndex = 0;
        freqRatio = 1.0465f;
    } else if (freqScale < 1.99999f) {
        harmonicIndex = 1;
        freqRatio = 1.0465f / 2;
    } else if (freqScale < 3.99999f) {
        harmonicIndex = 2;
        freqRatio = 1.0465f / 4 + 1.005E-3;
    } else {
        harmonicIndex = 3;
        freqRatio = 1.0465f / 8 - 2.5E-6;
    }

    // Update results
    layer->freqScale *= freqRatio;
    note->playbackState.waveId = waveId;
    note->playbackState.harmonicIndex = harmonicIndex;

    // Save the pointer to the synthethic wave
    // waveId index starts at 128, there are WAVE_SAMPLE_COUNT samples to read from
    note->noteSubEu.waveSampleAddr = &WAVEMEM_TABLE[waveId - 128][harmonicIndex * WAVE_SAMPLE_COUNT];

    return harmonicIndex;
}

void Nas_ContinueWaveMemory(Note* note, SequenceLayer* layer) {
    s32 prevHarmonicIndex;
    s32 curHarmonicIndex;
    s32 waveId = layer->instOrWave;

    if (waveId == 0xFF) {
        waveId = layer->channel->instOrWave;
    }

    prevHarmonicIndex = note->playbackState.harmonicIndex;
    curHarmonicIndex = Nas_WaveMemoryMake(note, layer, waveId);

    if (curHarmonicIndex != prevHarmonicIndex) {
        note->noteSubEu.harmonicIndexCurAndPrev = (curHarmonicIndex << 2) + prevHarmonicIndex;
    }
}

void __Nas_InitList(AudioListItem* list) {
    list->prev = list;
    list->next = list;
    list->u.count = 0;
}

void Nas_InitChNode(NotePool* pool) {
    __Nas_InitList(&pool->disabled);
    __Nas_InitList(&pool->decaying);
    __Nas_InitList(&pool->releasing);
    __Nas_InitList(&pool->active);
    pool->disabled.pool = pool;
    pool->decaying.pool = pool;
    pool->releasing.pool = pool;
    pool->active.pool = pool;
}

void Nas_InitChannelList(void) {
    s32 i;

    Nas_InitChNode(&AG.noteFreeLists);
    for (i = 0; i < AG.numNotes; i++) {
        AG.notes[i].listItem.u.value = &AG.notes[i];
        AG.notes[i].listItem.prev = NULL;
        Nas_AddList(&AG.noteFreeLists.disabled, &AG.notes[i].listItem);
    }
}

void Nas_DeAllocAllVoices(NotePool* pool) {
    s32 i;
    AudioListItem* source;
    AudioListItem* cur;
    AudioListItem* dest;

    for (i = 0; i < 4; i++) {
        switch (i) {
            case 0:
                source = &pool->disabled;
                dest = &AG.noteFreeLists.disabled;
                break;

            case 1:
                source = &pool->decaying;
                dest = &AG.noteFreeLists.decaying;
                break;

            case 2:
                source = &pool->releasing;
                dest = &AG.noteFreeLists.releasing;
                break;

            case 3:
                source = &pool->active;
                dest = &AG.noteFreeLists.active;
                break;
        }

        while (true) {
            cur = source->next;
            if (cur == source || cur == NULL) {
                break;
            }
            Nas_CutList(cur);
            Nas_AddList(dest, cur);
        }
    }
}

void Nas_AllocVoices(NotePool* pool, s32 count) {
    s32 i;
    s32 j;
    Note* note;
    AudioListItem* source;
    AudioListItem* dest;

    Nas_DeAllocAllVoices(pool);

    for (i = 0, j = 0; j < count; i++) {
        if (i == 4) {
            return;
        }

        switch (i) {
            case 0:
                source = &AG.noteFreeLists.disabled;
                dest = &pool->disabled;
                break;

            case 1:
                source = &AG.noteFreeLists.decaying;
                dest = &pool->decaying;
                break;

            case 2:
                source = &AG.noteFreeLists.releasing;
                dest = &pool->releasing;
                break;

            case 3:
                source = &AG.noteFreeLists.active;
                dest = &pool->active;
                break;
        }

        while (j < count) {
            note = Nas_GetList(source);
            if (note == NULL) {
                break;
            }
            Nas_AddList(dest, &note->listItem);
            j++;
        }
    }
}

void Nas_AddListHead(AudioListItem* list, AudioListItem* item) {
    // add 'item' to the front of the list given by 'list', if it's not in any list
    if (item->prev == NULL) {
        item->prev = list;
        item->next = list->next;
        list->next->prev = item;
        list->next = item;
        list->u.count++;
        item->pool = list->pool;
    }
}

void Nas_CutList(AudioListItem* item) {
    // remove 'item' from the list it's in, if any
    if (item->prev != NULL) {
        item->prev->next = item->next;
        item->next->prev = item->prev;
        item->prev = NULL;
    }
}

Note* __Nas_GetLowerPrio(AudioListItem* list, s32 limit) {
    AudioListItem* cur = list->next;
    AudioListItem* best;

    if (cur == list) {
        return NULL;
    }

    for (best = cur; cur != list; cur = cur->next) {
        if (((Note*)best->u.value)->playbackState.priority >= ((Note*)cur->u.value)->playbackState.priority) {
            best = cur;
        }
    }

    if (best == NULL) {
        return NULL;
    }

    if (limit <= ((Note*)best->u.value)->playbackState.priority) {
        return NULL;
    }

    return best->u.value;
}

void Nas_EntryTrack(Note* note, SequenceLayer* layer) {
    s32 pad[3];
    s16 instId;
    NotePlaybackState* playbackState = &note->playbackState;
    NoteSubEu* sub = &note->noteSubEu;

    note->playbackState.prevParentLayer = NO_LAYER;
    note->playbackState.parentLayer = layer;
    playbackState->priority = layer->channel->notePriority;
    layer->notePropertiesNeedInit = true;
    layer->bit3 = true;
    layer->note = note;
    layer->channel->noteUnused = note;
    layer->channel->layerUnused = layer;
    layer->noteVelocity = 0.0f;
    Nas_StartVoice(note);
    instId = layer->instOrWave;

    if (instId == 0xFF) {
        instId = layer->channel->instOrWave;
    }
    sub->tunedSample = layer->tunedSample;

    if (instId >= 0x80 && instId < 0xC0) {
        sub->bitField1.isSyntheticWave = true;
    } else {
        sub->bitField1.isSyntheticWave = false;
    }

    if (sub->bitField1.isSyntheticWave) {
        Nas_WaveMemoryMake(note, layer, instId);
    }

    playbackState->fontId = layer->channel->fontId;
    playbackState->stereoHeadsetEffects = layer->channel->stereoHeadsetEffects;
    sub->bitField1.reverbIndex = layer->channel->reverbIndex & 3;
}

void __Nas_InterTrack(Note* note, SequenceLayer* layer) {
    // similar to __Nas_InterReleaseTrack, hard to say what the difference is
    Nas_Release_Channel_Force(note->playbackState.parentLayer);
    note->playbackState.wantedParentLayer = layer;
}

void __Nas_InterReleaseTrack(Note* note, SequenceLayer* layer) {
    note->playbackState.wantedParentLayer = layer;
    note->playbackState.priority = layer->channel->notePriority;

    note->playbackState.adsr.fadeOutVel = AG.audioBufferParameters.ticksPerUpdateInv;
    note->playbackState.adsr.action.s.release = true;
}

Note* __Nas_ChLookFree(NotePool* pool, SequenceLayer* layer) {
    Note* note = Nas_GetList(&pool->disabled);
    if (note != NULL) {
        Nas_EntryTrack(note, layer);
        Nas_AddListHead(&pool->active, &note->listItem);
    }
    return note;
}

Note* __Nas_ChLookRelease(NotePool* pool, SequenceLayer* layer) {
    Note* note = Nas_GetList(&pool->decaying);
    if (note != NULL) {
        __Nas_InterReleaseTrack(note, layer);
        Nas_AddList(&pool->releasing, &note->listItem);
    }
    return note;
}

Note* __Nas_ChLookRelWait(NotePool* pool, SequenceLayer* layer) {
    Note* rNote;
    Note* aNote;
    s32 rPriority;
    s32 aPriority;

    rPriority = aPriority = 0x10;
    rNote = __Nas_GetLowerPrio(&pool->releasing, layer->channel->notePriority);

    if (rNote != NULL) {
        rPriority = rNote->playbackState.priority;
    }

    aNote = __Nas_GetLowerPrio(&pool->active, layer->channel->notePriority);

    if (aNote != NULL) {
        aPriority = aNote->playbackState.priority;
    }

    if (rNote == NULL && aNote == NULL) {
        return NULL;
    }

    if (aPriority < rPriority) {
        Nas_CutList(&aNote->listItem);
        __Nas_InterTrack(aNote, layer);
        Nas_AddList(&pool->releasing, &aNote->listItem);
        aNote->playbackState.priority = layer->channel->notePriority;
        return aNote;
    }
    rNote->playbackState.wantedParentLayer = layer;
    rNote->playbackState.priority = layer->channel->notePriority;
    return rNote;
}

Note* Nas_AllocationOnRequest(SequenceLayer* layer) {
    Note* note;
    u32 policy = layer->channel->noteAllocPolicy;

    if (policy & 1) {
        note = layer->note;
        if (note != NULL && note->playbackState.prevParentLayer == layer &&
            note->playbackState.wantedParentLayer == NO_LAYER) {
            __Nas_InterReleaseTrack(note, layer);
            Nas_CutList(&note->listItem);
            Nas_AddList(&note->listItem.pool->releasing, &note->listItem);
            return note;
        }
    }

    if (policy & 2) {
        if (!(note = __Nas_ChLookFree(&layer->channel->notePool, layer)) &&
            !(note = __Nas_ChLookRelease(&layer->channel->notePool, layer)) &&
            !(note = __Nas_ChLookRelWait(&layer->channel->notePool, layer))) {
            goto null_return;
        }
        return note;
    }

    if (policy & 4) {
        if (!(note = __Nas_ChLookFree(&layer->channel->notePool, layer)) &&
            !(note = __Nas_ChLookFree(&layer->channel->seqPlayer->notePool, layer)) &&
            !(note = __Nas_ChLookRelease(&layer->channel->notePool, layer)) &&
            !(note = __Nas_ChLookRelease(&layer->channel->seqPlayer->notePool, layer)) &&
            !(note = __Nas_ChLookRelWait(&layer->channel->notePool, layer)) &&
            !(note = __Nas_ChLookRelWait(&layer->channel->seqPlayer->notePool, layer))) {
            goto null_return;
        }
        return note;
    }

    if (policy & 8) {
        if (!(note = __Nas_ChLookFree(&AG.noteFreeLists, layer)) &&
            !(note = __Nas_ChLookRelease(&AG.noteFreeLists, layer)) &&
            !(note = __Nas_ChLookRelWait(&AG.noteFreeLists, layer))) {
            goto null_return;
        }
        return note;
    }

    if (!(note = __Nas_ChLookFree(&layer->channel->notePool, layer)) &&
        !(note = __Nas_ChLookFree(&layer->channel->seqPlayer->notePool, layer)) &&
        !(note = __Nas_ChLookFree(&AG.noteFreeLists, layer)) &&
        !(note = __Nas_ChLookRelease(&layer->channel->notePool, layer)) &&
        !(note = __Nas_ChLookRelease(&layer->channel->seqPlayer->notePool, layer)) &&
        !(note = __Nas_ChLookRelease(&AG.noteFreeLists, layer)) &&
        !(note = __Nas_ChLookRelWait(&layer->channel->notePool, layer)) &&
        !(note = __Nas_ChLookRelWait(&layer->channel->seqPlayer->notePool, layer)) &&
        !(note = __Nas_ChLookRelWait(&AG.noteFreeLists, layer))) {
        goto null_return;
    }
    return note;

null_return:
    layer->bit3 = true;
    return NULL;
}

void Nas_ChannelInit(void) {
    Note* note;
    s32 i;

    for (i = 0; i < AG.numNotes; i++) {
        note = &AG.notes[i];
        note->noteSubEu = NA_CHINIT_TABLE;
        note->playbackState.priority = 0;
        note->playbackState.unk_04 = 0;
        note->playbackState.parentLayer = NO_LAYER;
        note->playbackState.wantedParentLayer = NO_LAYER;
        note->playbackState.prevParentLayer = NO_LAYER;
        note->playbackState.waveId = 0;
        note->playbackState.attributes.velocity = 0.0f;
        note->playbackState.adsrVolScaleUnused = 0;
        note->playbackState.adsr.action.asByte = 0;
        note->playbackState.vibratoState.active = 0;
        note->playbackState.portamento.cur = 0;
        note->playbackState.portamento.speed = 0;
        note->playbackState.stereoHeadsetEffects = false;
        note->startSamplePos = 0;
        note->synthesisState.synthesisBuffers =
            Nas_NcHeapAlloc(&AG.miscPool, sizeof(NoteSynthesisBuffers));
    }
}
