#include "global.h"

#pragma increment_block_number "gc-eu:128 gc-eu-mq:128 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128" \
                               "ntsc-1.2:128 pal-1.1:128 hiratsu3:128"

ALIGNED(16) u8 szpbuf[0x400];
u8* szpbufend;
uintptr_t curr_dmasrc;
size_t curr_dmasiz;
u8* srclim;

void* firstdata(void) {
    s32 pad[2];
    size_t dmaSize;
    size_t bufferSize;

    srclim = szpbufend - 0x19;

    bufferSize = szpbufend - szpbuf;
    dmaSize = (bufferSize > curr_dmasiz) ? curr_dmasiz : bufferSize;

    percial_DMA(curr_dmasrc, szpbuf, dmaSize);
    curr_dmasrc += dmaSize;
    curr_dmasiz -= dmaSize;
    return szpbuf;
}

void* nextdata(u8* curSrcPos) {
    u8* dst;
    size_t restSize;
    size_t dmaSize;

    restSize = szpbufend - curSrcPos;
    dst = (restSize & 7) ? (szpbuf - (restSize & 7)) + 8 : szpbuf;

    bcopy(curSrcPos, dst, restSize);
    dmaSize = (szpbufend - dst) - restSize;
    if (curr_dmasiz < dmaSize) {
        dmaSize = curr_dmasiz;
    }

    if (dmaSize != 0) {
        percial_DMA(curr_dmasrc, dst + restSize, dmaSize);
        curr_dmasrc += dmaSize;
        curr_dmasiz -= dmaSize;
        if (curr_dmasiz == 0) {
            srclim = dst + restSize + dmaSize;
        }
    }

    return dst;
}

void slidstart_szs(u8* src, u8* dst) {
    Yaz0Header* header = (Yaz0Header*)src;
    u32 bitIdx = 0;
    u8* dstEnd = dst + header->decSize;
    u32 chunkHeader;
    u32 nibble;
    u8* backPtr;
    u32 chunkSize;
    u32 off;

    src += sizeof(Yaz0Header);

    do {
        if (bitIdx == 0) {
            if ((srclim < src) && (curr_dmasiz != 0)) {
                src = nextdata(src);
            }

            chunkHeader = *src++;
            bitIdx = 8;
        }

        if (chunkHeader & (1 << 7)) { // uncompressed
            *dst = *src;
            dst++;
            src++;
        } else { // compressed
            off = ((*src & 0xF) << 8 | *(src + 1));
            nibble = *src >> 4;
            backPtr = dst - off;
            src += 2;

            chunkSize = (nibble == 0)              // N = chunkSize; B = back offset
                            ? (u32)(*src++ + 0x12) // 3 bytes 0B BB NN
                            : nibble + 2;          // 2 bytes NB BB

            do {
                *dst++ = *(backPtr++ - 1);
                chunkSize--;
            } while (chunkSize != 0);
        }
        chunkHeader <<= 1;
        bitIdx--;
    } while (dst != dstEnd);
}

void slidma(uintptr_t romStart, u8* dst, size_t size) {
    curr_dmasrc = romStart;
    curr_dmasiz = size;
    szpbufend = szpbuf + sizeof(szpbuf);
    slidstart_szs(firstdata(), dst);
}
