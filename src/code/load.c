#include "global.h"

s32 _loadOneBlock(JpegHuffmanTable* hTable0, JpegHuffmanTable* hTable1, u16* mcu, s16* unk);
s32 __decode(JpegHuffmanTable* hTable, s16* outCoeff, s8* outZeroCount);
u16 ___getCode(u8 len);

u8* STREAM;
u32 CURRENT;
u8 POINTER;
u8 THROUGH;
u32 BUFFER;

s32 jpgLoad(JpegDecoder* decoder, u16* mcuBuff, s32 count, u8 isFollowing, JpegDecoderState* state) {
    s16 pad;
    s16 unk0;
    s16 unk1;
    s16 unk2;
    u32 idx;
    s32 inc;
    u16 unkCount;

    JpegHuffmanTable* hTable0;
    JpegHuffmanTable* hTable1;
    JpegHuffmanTable* hTable2;
    JpegHuffmanTable* hTable3;

    inc = 0;
    STREAM = decoder->imageData;
    if (decoder->mode == 0) {
        unkCount = 2;
    } else {
        unkCount = 4;
        if (decoder->unk_05 == 1) {
            inc = 8 * 8 * 2;
        }
    }

    hTable0 = decoder->hTablePtrs[0];
    hTable1 = decoder->hTablePtrs[1];
    hTable2 = decoder->hTablePtrs[2];
    hTable3 = decoder->hTablePtrs[3];

    if (!isFollowing) {
        CURRENT = 0;
        POINTER = 32;
        BUFFER = 0;
        THROUGH = 0;
        unk0 = 0;
        unk1 = 0;
        unk2 = 0;
    } else {
        CURRENT = state->byteIdx;
        POINTER = state->bitIdx;
        BUFFER = state->curWord;
        THROUGH = state->dontSkip;
        unk0 = state->unk_0C;
        unk1 = state->unk_0E;
        unk2 = state->unk_10;
    }

    while (count != 0) {
        for (idx = 0; idx < unkCount; idx++) {
            if (_loadOneBlock(hTable0, hTable1, mcuBuff, &unk0)) {
                return 2;
            }
            mcuBuff += 8 * 8;
        }

        if (_loadOneBlock(hTable2, hTable3, mcuBuff, &unk1)) {
            return 2;
        }
        mcuBuff += 8 * 8;

        if (_loadOneBlock(hTable2, hTable3, mcuBuff, &unk2)) {
            return 2;
        }

        count--;
        mcuBuff += 8 * 8;
        mcuBuff += inc;
    }

    state->byteIdx = CURRENT;
    state->bitIdx = POINTER;
    state->curWord = BUFFER;
    state->dontSkip = THROUGH;
    state->unk_0C = unk0;
    state->unk_0E = unk1;
    state->unk_10 = unk2;
    return 0;
}

s32 _loadOneBlock(JpegHuffmanTable* hTable0, JpegHuffmanTable* hTable1, u16* mcu, s16* unk) {
    s8 i = 0;
    s8 zeroCount;
    s16 coeff;

    if (__decode(hTable0, &coeff, &zeroCount)) {
        return 1;
    }

    *unk += coeff;
    mcu[i++] = *unk;
    while (i < 8 * 8) {
        if (__decode(hTable1, &coeff, &zeroCount) != 0) {
            return 1;
        }

        if (coeff == 0) {
            if (zeroCount == 0xF) {
                while (zeroCount-- >= 0) {
                    mcu[i++] = 0;
                }
            } else {
                while (i < 8 * 8) {
                    mcu[i++] = 0;
                }
                break;
            }
        } else {
            while (0 < zeroCount--) {
                mcu[i++] = 0;
            }
            mcu[i++] = coeff;
        }
    }

    return 0;
}

s32 __decode(JpegHuffmanTable* hTable, s16* outCoeff, s8* outZeroCount) {
    u8 codeIdx;
    u8 sym;
    u16 codeOff = 0;
    u16 buff = ___getCode(16);

    for (codeIdx = 0; codeIdx < 16; codeIdx++) {
        if (hTable->codesB[codeIdx] == 0xFFFF) {
            continue;
        }

        codeOff = buff >> (15 - codeIdx);
        if (codeOff <= hTable->codesB[codeIdx]) {
            break;
        }
    }

    if (codeIdx >= 16) {
        return 1;
    }

    sym = hTable->symbols[hTable->codeOffs[codeIdx] + codeOff - hTable->codesA[codeIdx]];
    *outZeroCount = sym >> 4;
    sym &= 0xF;

    POINTER += codeIdx - 15;
    *outCoeff = 0;
    if (sym) {
        *outCoeff = ___getCode(sym);
        if (*outCoeff < (1 << (sym - 1))) {
            *outCoeff += (-1U << sym) + 1;
        }
    }

    return 0;
}

u16 ___getCode(u8 len) {
    u8 byteCount;
    u8 data;
    s32 ret = 0;
    u32 temp;

    for (byteCount = POINTER >> 3; byteCount > 0; byteCount--) {
        data = STREAM[CURRENT++];
        if (THROUGH) {
            if (data == 0) {
                data = STREAM[CURRENT++];
            }
        }

        THROUGH = (data == 0xFF) ? 1 : 0;

        BUFFER <<= 8;
        BUFFER |= data;
        POINTER -= 8;
    }

    ret = BUFFER << POINTER;
    temp = ret;
    ret = temp >> -len;
    POINTER += len;
    return ret;
}
