#ifndef SPEEDMETER_H
#define SPEEDMETER_H

#include "ultra64/ultratypes.h"

struct GraphicsContext;
struct GameState;

typedef struct SpeedMeter {
    /* 0x00 */ char unk_00[0x18];
    /* 0x18 */ s32 x; // Unused
    /* 0x1C */ s32 y;
} SpeedMeter; // size = 0x20

void speed_meter_init(SpeedMeter* this);
void speed_meter_cleanup(SpeedMeter* this);
void speed_meter_draw(SpeedMeter* this, struct GraphicsContext* gfxCtx);
void speed_meter_draw_memory(SpeedMeter* meter, struct GraphicsContext* gfxCtx, struct GameState* state);

#endif
