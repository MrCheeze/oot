#ifndef SEGMENTED_ADDRESS_H
#define SEGMENTED_ADDRESS_H

#include "ultra64.h"
#include "stdint.h"

extern uintptr_t SegmentBaseAddress[NUM_SEGMENTS];

#define SEGMENTED_TO_VIRTUAL(addr) (void*)(SegmentBaseAddress[SEGMENT_NUMBER(addr)] + SEGMENT_OFFSET(addr) + K0BASE)

#endif
