#ifndef QUAKE_H
#define QUAKE_H

#include "z64camera.h"
#include "z64math.h"

typedef struct ShakeInfo {
    /* 0x00 */ Vec3f atOffset;
    /* 0x0C */ Vec3f eyeOffset;
    /* 0x18 */ s16 upPitchOffset; // gives a "roll" effect by offsetting the Up vector
    /* 0x1A */ s16 upYawOffset; // gives a "roll" effect by offsetting the Up vector
    /* 0x1C */ s16 fovOffset; // binary angle
    /* 0x20 */ f32 maxOffset;
} ShakeInfo; // size = 0x24

typedef enum QuakeType {
    /* 0 */ QUAKE_TYPE_NONE,
    /* 1 */ QUAKE_TYPE_1, // Periodic, sustaining, random X perturbations
    /* 2 */ QUAKE_TYPE_2, // Aperiodic, sustaining, random X perturbations
    /* 3 */ QUAKE_TYPE_3, // Periodic, decaying
    /* 4 */ QUAKE_TYPE_4, // Aperiodic, decaying, random X perturbations
    /* 5 */ QUAKE_TYPE_5, // Periodic, sustaining
    /* 6 */ QUAKE_TYPE_6 // See below
} QuakeType;

// Quake type 6 is Jump-Periodic, sustaining, random X perturbations,
// resets period every 16 frames (jumps, similar to sawtooth),
// continues indefinitely i.e. does not terminate when the timer reaches 0
// must be manually removed

s16 startQuake(Camera* camera, u32 type);

u32 setSpeedQuake(s16 index, s16 speed);
u32 setScaleQuake(s16 index, s16 y, s16 x, s16 fov, s16 roll);
u32 setTimerQuake(s16 index, s16 duration);
u32 setRotateQuake(s16 index, s16 isRelativeToScreen, Vec3s orientation);

s16 getTimerQuake(s16 index);
u32 stopQuake(s16 index);

void initQuakeControl(void);
s16 cameraQuakeControl(Camera* camera, ShakeInfo* camShake);

#endif
