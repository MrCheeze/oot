#include "global.h"

typedef enum LetterboxState {
    /* 0 */ LETTERBOX_STATE_IDLE,
    /* 1 */ LETTERBOX_STATE_GROWING,
    /* 2 */ LETTERBOX_STATE_SHRINKING
} LetterboxState;

s32 shrink_window_action = LETTERBOX_STATE_IDLE;

s32 shrink_window_value = 0;
s32 shrink_window_nowval = 0;

void shrink_window_setval(s32 target) {
    if (R_HREG_MODE == HREG_MODE_LETTERBOX && R_LETTERBOX_ENABLE_LOGS == 1) {
        PRINTF("shrink_window_setval(%d)\n", target);
    }

    shrink_window_value = target;
}

u32 shrink_window_getval(void) {
    return shrink_window_value;
}

void shrink_window_setnowval(s32 size) {
    if (R_HREG_MODE == HREG_MODE_LETTERBOX && R_LETTERBOX_ENABLE_LOGS == 1) {
        PRINTF("shrink_window_setnowval(%d)\n", size);
    }

    shrink_window_nowval = size;
}

u32 shrink_window_getnowval(void) {
    return shrink_window_nowval;
}

void shrink_window_init(void) {
    if (R_HREG_MODE == HREG_MODE_LETTERBOX && R_LETTERBOX_ENABLE_LOGS == 1) {
        PRINTF("shrink_window_init()\n");
    }

    shrink_window_action = LETTERBOX_STATE_IDLE;
    shrink_window_value = 0;
    shrink_window_nowval = 0;
}

void shrink_window_cleanup(void) {
    if (R_HREG_MODE == HREG_MODE_LETTERBOX && R_LETTERBOX_ENABLE_LOGS == 1) {
        PRINTF("shrink_window_cleanup()\n");
    }

    shrink_window_nowval = 0;
}

void shrink_window_move(s32 updateRate) {
    s32 step;

    if (updateRate == 3) {
        step = 10;
    } else {
        step = 30 / updateRate;
    }

    if (shrink_window_nowval < shrink_window_value) {
        if (shrink_window_action != LETTERBOX_STATE_GROWING) {
            shrink_window_action = LETTERBOX_STATE_GROWING;
        }

        if (shrink_window_nowval + step < shrink_window_value) {
            shrink_window_nowval += step;
        } else {
            shrink_window_nowval = shrink_window_value;
        }
    } else if (shrink_window_value < shrink_window_nowval) {
        if (shrink_window_action != LETTERBOX_STATE_SHRINKING) {
            shrink_window_action = LETTERBOX_STATE_SHRINKING;
        }

        if (shrink_window_value < shrink_window_nowval - step) {
            shrink_window_nowval -= step;
        } else {
            shrink_window_nowval = shrink_window_value;
        }
    } else {
        shrink_window_action = LETTERBOX_STATE_IDLE;
    }

    if (DEBUG_FEATURES && (R_HREG_MODE == HREG_MODE_LETTERBOX)) {
        if (R_LETTERBOX_INIT != HREG_MODE_LETTERBOX) {
            R_LETTERBOX_INIT = HREG_MODE_LETTERBOX;
            R_LETTERBOX_ENABLE_LOGS = 0;
            HREG(82) = 0; // this reg is not used in this mode
            R_LETTERBOX_STATE = 0;
            R_LETTERBOX_SIZE = 0;
            R_LETTERBOX_TARGET_SIZE = 0;
            R_LETTERBOX_STEP = 0;

            // these regs are not used in this mode
            HREG(87) = 0;
            HREG(88) = 0;
            HREG(89) = 0;
        }

        R_LETTERBOX_STATE = shrink_window_action;
        R_LETTERBOX_SIZE = shrink_window_nowval;
        R_LETTERBOX_TARGET_SIZE = shrink_window_value;
        R_LETTERBOX_STEP = step;
    }
}
