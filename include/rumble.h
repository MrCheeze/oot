#ifndef RUMBLE_H
#define RUMBLE_H

#include "ultra64.h"

#define RUMBLE_MAX_REQUESTS 64

typedef enum RumbleState {
    RUMBLE_STATE_CLEAR,
    RUMBLE_STATE_RUNNING,
    RUMBLE_STATE_RESET
} RumbleState;

typedef struct RumbleMgr {
    /* 0x000 */ u8 rumbleEnable[MAXCONTROLLERS];
    /* 0x004 */ u8 reqStrengths[RUMBLE_MAX_REQUESTS];       // Source strength modulated by distance to the source
    /* 0x044 */ u8 reqDurations[RUMBLE_MAX_REQUESTS];       // Duration until decreaseRate kicks in
    /* 0x084 */ u8 reqDecreaseRates[RUMBLE_MAX_REQUESTS];   // Decreases the strength by this much every Vertical Retrace, once the strength hits 0 the request slot is freed
    /* 0x0C4 */ u8 reqAccumulators[RUMBLE_MAX_REQUESTS];    // Starts at 0, incremented by the strength every Vertical Retrace
    /* 0x104 */ u8 state;
    /* 0x105 */ u8 updateEnabled;
    /* 0x106 */ u16 onTimer;    // Duration for which there has been an active rumble request running
    /* 0x108 */ u16 offTimer;   // Duration for which there has not been an active rumble request running, capped at 5
    /* 0x10A */ u8 overrideStrength;    // overrides requests with these parameters
    /* 0x10B */ u8 overrideDuration;
    /* 0x10C */ u8 overrideDecreaseRate;
    /* 0x10D */ u8 overrideAccumulator;
} RumbleMgr; // size = 0x10E

// internal

void vibctl2_init(RumbleMgr* rumbleMgr);
void vibctl2_cleanup(RumbleMgr* rumbleMgr);
void vibctl2_move(RumbleMgr* rumbleMgr);

// external

void z_vibctl2_vib_force_set(f32 distSq, u8 sourceStrength, u8 duration, u8 decreaseRate);
void z_vibctl2_vib_setQ(f32 distSq, u8 sourceStrength, u8 duration, u8 decreaseRate);

void z_vibctl2_init(void);
void z_vibctl2_cleanup(void);

s32 z_vibctl2_RumblePackIsConnected(void);

void z_vibctl2_StageInit(void);
void z_vibctl2_StageCancel(void);

void z_vibctl2_pause(u32 enable);

#endif
