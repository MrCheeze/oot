#include "global.h"

u64* poly_tbl = gspF3DZEX2_NoN_PosLight_fifoTextStart;
u64* sDefaultGSPUCodeData = gspF3DZEX2_NoN_PosLight_fifoDataStart;

u64* ucode_GetRspBootTextStart(void) {
    return rspbootTextStart;
}

size_t ucode_GetRspBootTextSize(void) {
    return (size_t)((u8*)rspbootTextEnd - (u8*)rspbootTextStart);
}

u64* ucode_GetPolyTextStart(void) {
    return poly_tbl;
}

u64* ucode_GetPolyDataStart(void) {
    return sDefaultGSPUCodeData;
}
