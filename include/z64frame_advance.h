#ifndef Z64FRAME_ADVANCE_H
#define Z64FRAME_ADVANCE_H

#include "ultra64.h"

struct Input;
struct PlayState;

typedef struct FrameAdvanceContext {
    /* 0x0 */ s32 enabled;
    /* 0x4 */ s32 timer;
} FrameAdvanceContext; // size = 0x8

void Pause_ct(FrameAdvanceContext* frameAdvCtx);
s32 Pause_proc(FrameAdvanceContext* frameAdvCtx, struct Input* input);

int _Game_play_isPause(struct PlayState* this);

#endif
