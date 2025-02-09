/**
 * @file z_rumble.c
 *
 * This file implements an interface for the game state to set up, manage and request use of the rumble pak. Despite
 * some parts of the system appearing to accommodate all four controller ports, only controller 1 can be instructed
 * to rumble.
 * This file is half of the system that runs on the graph thread alongside the game state. The rest of the system that
 * processes the requests runs on the padmgr thread and is implemented in `sys_rumble.c`.
 *
 * @see sys_rumble.c
 *
 * @note Original filename is likely z_vibrate.c or similar as it is ordered after z_ss_sram.c and before z_view.c
 */
#include "global.h"

RumbleMgr sys_vibctl2;

/**
 * Padmgr callback to update the state of rumble on Vertical Retrace.
 *
 * Unlike every other function in this file, this runs on the padmgr thread.
 */
void z_vibctl2_callback(PadMgr* padMgr, void* arg) {
    vibctl2_move(&sys_vibctl2);
    padmgr_RumbleSetTbl(padMgr, sys_vibctl2.rumbleEnable);
}

/**
 * Forces the rumble state to use the supplied parameters.
 * The parameters are the same as in `z_vibctl2_vib_setQ`.
 *
 * @see z_vibctl2_vib_setQ
 */
void z_vibctl2_vib_force_set(f32 distSq, u8 sourceStrength, u8 duration, u8 decreaseRate) {
    s32 dist;
    s32 strength;

    if (distSq > SQ(1000)) {
        dist = 1000;
    } else {
        dist = sqrtf(distSq);
    }

    if (dist < 1000 && sourceStrength != 0 && decreaseRate != 0) {
        // Decrease the strength linearly with distance
        strength = sourceStrength - (dist * 255) / 1000;

        if (strength > 0) {
            // Note: sys_vibctl2 is a shared resource between the graph and padmgr threads, no locking is done
            // to ensure that the entire request is written before it is possibly used.
            sys_vibctl2.overrideStrength = strength;
            sys_vibctl2.overrideDuration = duration;
            sys_vibctl2.overrideDecreaseRate = decreaseRate;
        }
    }
}

/**
 * Submits a request to the rumble manager with the properties given in the arguments. If there is no free request slot
 * the request is silently dropped.
 *
 * @param distSq
 *     Squared distance, usually taken to be from an apparent source to the player in world coordinates.
 * @param sourceStrength
 *     The strength of the rumble at 0 distance from the source.
 *     The rumble source strength decreases linearly with distance, a distance of 0 results in the full source strength
 *     while a distance of 1000 or greater is discarded. A source strength of 0 is discarded. A minimum source strength
 *     of 1 drops to 0 at 3 units of distance from the source. A maximum source strength of 255 drops to 0 at 1000
 *     units of distance from the source.
 *     Note that, once the request has been submitted, if the distance to the source changes in subsequent frames while
 *     the rumble request is still running, the request will not be updated with the new distance.
 * @param duration
 *     The duration for which the rumble will sustain full strength. It is measured in Vertical Retraces rather than
 *     game frames. There are ~60 Retraces/s on NTSC and 50 Retraces/s on PAL.
 * @param decreaseRate
 *     The amount by which to lower the strength every Vertical Retrace once duration has hit 0.
 */
void z_vibctl2_vib_setQ(f32 distSq, u8 sourceStrength, u8 duration, u8 decreaseRate) {
    s32 dist;
    s32 strength;
    s32 i;

    if (distSq > SQ(1000)) {
        dist = 1000;
    } else {
        dist = sqrtf(distSq);
    }

    if (dist < 1000 && sourceStrength != 0 && decreaseRate != 0) {
        // Decrease the strength linearly with distance
        strength = sourceStrength - (dist * 255) / 1000;

        for (i = 0; i < RUMBLE_MAX_REQUESTS; i++) {
            // Search for an empty slot
            if (sys_vibctl2.reqStrengths[i] == 0) {
                if (strength > 0) {
                    // Note: sys_vibctl2 is a shared resource between the graph and padmgr threads, no locking is done
                    // to ensure that the entire request is written before it is possibly used.
                    sys_vibctl2.reqStrengths[i] = strength;
                    sys_vibctl2.reqDurations[i] = duration;
                    sys_vibctl2.reqDecreaseRates[i] = decreaseRate;
                }
                break;
            }
        }
    }
}

void z_vibctl2_init(void) {
    vibctl2_init(&sys_vibctl2);
    PADMGR_SET_RETRACE_CALLACK(&padmgr, z_vibctl2_callback, NULL);
}

void z_vibctl2_cleanup(void) {
    PadMgr* padmgr_ = &padmgr;

    PADMGR_UNSET_RETRACE_CALLACK(padmgr_, z_vibctl2_callback, NULL);
    vibctl2_cleanup(&sys_vibctl2);
}

s32 z_vibctl2_RumblePackIsConnected(void) {
    return padmgr.pakType[0] == CONT_PAK_RUMBLE;
}

void z_vibctl2_StageInit(void) {
    sys_vibctl2.state = RUMBLE_STATE_RESET;
}

void z_vibctl2_StageCancel(void) {
    sys_vibctl2.state = RUMBLE_STATE_CLEAR;
}

void z_vibctl2_pause(u32 enable) {
    sys_vibctl2.updateEnabled = !!enable;
}
