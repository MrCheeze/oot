#include "z64.h"

ALIGNED(16) u64 sys_fifo_buffer[0x3000];

ALIGNED(16) u64 sys_yield_buffer[OS_YIELD_DATA_SIZE / sizeof(u64)];

ALIGNED(16) u64 sys_dram_stack[SP_DRAM_STACK_SIZE64];

ALIGNED(16) GfxPool sys_dynamic[2];
