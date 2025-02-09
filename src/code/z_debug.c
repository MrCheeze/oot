#include "global.h"

typedef struct DebugCamTextBufferEntry {
    /* 0x0 */ u8 x;
    /* 0x1 */ u8 y;
    /* 0x2 */ u8 colorIndex;
    /* 0x3 */ char text[21];
} DebugCamTextBufferEntry; // size = 0x18

typedef struct InputCombo {
    /* 0x0 */ u16 hold;
    /* 0x2 */ u16 press;
} InputCombo; // size = 0x4

#pragma increment_block_number "gc-eu:160 gc-eu-mq:160 gc-jp:160 gc-jp-ce:160 gc-jp-mq:160 gc-us:160 gc-us-mq:160" \
                               "ique-cn:128 ntsc-1.0:160 ntsc-1.1:160 ntsc-1.2:160 pal-1.0:160 pal-1.1:160 hiratsu3:160"

RegEditor* debug_mode;

DebugCamTextBufferEntry debug_print2_buffer[22];

s16 debug_print2_count = 0;

Color_RGBA8 print_color[] = {
    { 255, 255, 32, 192 },  // DEBUG_CAM_TEXT_YELLOW
    { 255, 150, 128, 192 }, // DEBUG_CAM_TEXT_PEACH
    { 128, 96, 0, 64 },     // DEBUG_CAM_TEXT_BROWN
    { 192, 128, 16, 128 },  // DEBUG_CAM_TEXT_ORANGE
    { 255, 192, 32, 128 },  // DEBUG_CAM_TEXT_GOLD
    { 230, 230, 220, 64 },  // DEBUG_CAM_TEXT_WHITE
    { 128, 150, 255, 128 }, // DEBUG_CAM_TEXT_BLUE
    { 128, 255, 32, 128 },  // DEBUG_CAM_TEXT_GREEN
};

#if DEBUG_FEATURES
InputCombo sRegGroupInputCombos[REG_GROUPS] = {
    { BTN_L, BTN_CUP },        //  REG
    { BTN_L, BTN_CLEFT },      // SREG
    { BTN_L, BTN_CDOWN },      // OREG
    { BTN_L, BTN_A },          // PREG
    { BTN_R, BTN_CDOWN },      // QREG
    { BTN_L, BTN_CRIGHT },     // MREG
    { BTN_L, BTN_R },          // YREG
    { BTN_L, BTN_DLEFT },      // DREG
    { BTN_L, BTN_DRIGHT },     // UREG
    { BTN_L, BTN_DUP },        // IREG
    { BTN_L, BTN_B },          // ZREG
    { BTN_L, BTN_Z },          // CREG
    { BTN_L, BTN_DDOWN },      // NREG
    { BTN_R, BTN_A },          // KREG
    { BTN_R, BTN_B },          // XREG
    { BTN_R, BTN_Z },          // cREG
    { BTN_R, BTN_L },          // sREG
    { BTN_R, BTN_CUP },        // iREG
    { BTN_R, BTN_CRIGHT },     // WREG
    { BTN_R, BTN_DLEFT },      // AREG
    { BTN_R, BTN_CLEFT },      // VREG
    { BTN_R, BTN_START },      // HREG
    { BTN_L, BTN_START },      // GREG
    { BTN_R, BTN_DRIGHT },     // mREG
    { BTN_R, BTN_DUP },        // nREG
    { BTN_START, BTN_R },      // BREG
    { BTN_START, BTN_A },      // dREG
    { BTN_START, BTN_B },      // kREG
    { BTN_START, BTN_CRIGHT }, // bREG

};

char sRegGroupChars[REG_GROUPS] = {
    ' ', //  REG
    'S', // SREG
    'O', // OREG
    'P', // PREG
    'Q', // QREG
    'M', // MREG
    'Y', // YREG
    'D', // DREG
    'U', // UREG
    'I', // IREG
    'Z', // ZREG
    'C', // CREG
    'N', // NREG
    'K', // KREG
    'X', // XREG
    'c', // cREG
    's', // sREG
    'i', // iREG
    'W', // WREG
    'A', // AREG
    'V', // VREG
    'H', // HREG
    'G', // GREG
    'm', // mREG
    'n', // nREG
    'B', // BREG
    'd', // dREG
    'k', // kREG
    'b', // bREG
};
#endif

void new_Debug_mode(void) {
    s32 i;

    debug_mode = SYSTEM_ARENA_MALLOC(sizeof(RegEditor), "../z_debug.c", 260);
    debug_mode->regPage = 0;
    debug_mode->regGroup = 0;
    debug_mode->regCur = 0;
    debug_mode->dPadInputPrev = 0;
    debug_mode->inputRepeatTimer = 0;
    for (i = 0; i < ARRAY_COUNT(debug_mode->data); i++) {
        debug_mode->data[i] = 0;
    }
}

// Function is stubbed. Name is assumed by similarities in signature to `Debug_Print2_write` and usage.
void Debug_Print_write(u8 x, u8 y, const char* text) {
}

void Debug_Print2_write(u8 x, u8 y, u8 colorIndex, const char* text) {
    DebugCamTextBufferEntry* entry = &debug_print2_buffer[debug_print2_count];
    char* textDest;
    s16 charCount;

    if (debug_print2_count < ARRAY_COUNT(debug_print2_buffer)) {
        entry->x = x;
        entry->y = y;
        entry->colorIndex = colorIndex;

        // Copy text into the entry, truncating if needed
        charCount = 0;
        textDest = entry->text;

        while ((*textDest++ = *text++) != '\0') {
            if (charCount++ > (ARRAY_COUNT(entry->text) - 1)) {
                break;
            }
        }

        *textDest = '\0';

        debug_print2_count++;
    }
}

void Debug_Print2_output(GfxPrint* printer) {
    s32 i;
    Color_RGBA8* color;
    DebugCamTextBufferEntry* entry;

    for (i = 0; i < debug_print2_count; i++) {
        entry = &debug_print2_buffer[i];
        color = &print_color[entry->colorIndex];

        gfxprint_color(printer, color->r, color->g, color->b, color->a);
        gfxprint_locate8x8(printer, entry->x, entry->y);
        gfxprint_printf(printer, "%s", entry->text);
    }
}

#if DEBUG_FEATURES
/**
 * Updates the state of the Reg Editor according to user input.
 * Also contains a controller rumble test that can be interfaced with via related REGs.
 */
void Regs_UpdateEditor(Input* input) {
    s32 dPadInputCur;
    s32 pageDataStart = ((debug_mode->regGroup * REG_PAGES) + debug_mode->regPage - 1) * REGS_PER_PAGE;
    s32 increment;
    s32 i;

    dPadInputCur = input->cur.button & (BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT);

    if (CHECK_BTN_ALL(input->cur.button, BTN_L) || CHECK_BTN_ALL(input->cur.button, BTN_R) ||
        CHECK_BTN_ALL(input->cur.button, BTN_START)) {

        for (i = 0; i < REG_GROUPS; i++) {
            if (CHECK_BTN_ALL(input->cur.button, sRegGroupInputCombos[i].hold) &&
                CHECK_BTN_ALL(input->press.button, sRegGroupInputCombos[i].press)) {
                break;
            }
        }

        // If a combo corresponding to a reg group was found
        if (i < REG_GROUPS) {
            if (i == debug_mode->regGroup) {
                // Same reg group as current, advance page index
                debug_mode->regPage = (debug_mode->regPage + 1) % (REG_PAGES + 1);
            } else {
                debug_mode->regGroup = i; // Switch current reg group
                debug_mode->regPage = 0;  // Disable reg editor
            }
        }
    } else {
        switch (debug_mode->regPage) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                if (dPadInputCur == debug_mode->dPadInputPrev) {
                    debug_mode->inputRepeatTimer--;
                    if (debug_mode->inputRepeatTimer < 0) {
                        debug_mode->inputRepeatTimer = 1;
                    } else {
                        dPadInputCur ^= debug_mode->dPadInputPrev;
                    }
                } else {
                    debug_mode->inputRepeatTimer = 16;
                    debug_mode->dPadInputPrev = dPadInputCur;
                }

                increment =
                    CHECK_BTN_ANY(dPadInputCur, BTN_DRIGHT)  ? (CHECK_BTN_ALL(input->cur.button, BTN_A | BTN_B) ? 1000
                                                                : CHECK_BTN_ALL(input->cur.button, BTN_A)       ? 100
                                                                : CHECK_BTN_ALL(input->cur.button, BTN_B)       ? 10
                                                                                                                : 1)
                    : CHECK_BTN_ANY(dPadInputCur, BTN_DLEFT) ? (CHECK_BTN_ALL(input->cur.button, BTN_A | BTN_B) ? -1000
                                                                : CHECK_BTN_ALL(input->cur.button, BTN_A)       ? -100
                                                                : CHECK_BTN_ALL(input->cur.button, BTN_B)       ? -10
                                                                                                                : -1)
                                                             : 0;

                debug_mode->data[debug_mode->regCur + pageDataStart] += increment;

                if (CHECK_BTN_ANY(dPadInputCur, BTN_DUP)) {
                    debug_mode->regCur--;
                    if (debug_mode->regCur < 0) {
                        debug_mode->regCur = REGS_PER_PAGE - 1;
                    }
                } else if (CHECK_BTN_ANY(dPadInputCur, BTN_DDOWN)) {
                    debug_mode->regCur++;
                    if (debug_mode->regCur >= REGS_PER_PAGE) {
                        debug_mode->regCur = 0;
                    }
                }

                if (iREG(0)) {
                    iREG(0) = 0;
                    z_vibctl2_vib_setQ(0.0f, iREG(1), iREG(2), iREG(3));
                }

                break;

            default:
                break;
        }
    }
}

void Regs_DrawEditor(GfxPrint* printer) {
    s32 i;
    s32 pageStart = (debug_mode->regPage - 1) * REGS_PER_PAGE;
    s32 pageDataStart = ((debug_mode->regGroup * REG_PAGES) + debug_mode->regPage - 1) * REGS_PER_PAGE;
    s32 pad;
    char regGroupName[3];

    regGroupName[0] = 'R';
    regGroupName[1] = sRegGroupChars[debug_mode->regGroup];
    regGroupName[2] = '\0';

    gfxprint_color(printer, 0, 128, 128, 128);

    for (i = 0; i < REGS_PER_PAGE; i++) {
        if (i == debug_mode->regCur) {
            gfxprint_color(printer, 0, 255, 255, 255);
        }

        gfxprint_locate8x8(printer, 3, i + 5);
        gfxprint_printf(printer, "%s%02d%6d", regGroupName, pageStart + i, debug_mode->data[i + pageDataStart]);

        if (i == debug_mode->regCur) {
            gfxprint_color(printer, 0, 128, 128, 128);
        }
    }
}
#endif

/**
 * Draws the Reg Editor and Debug Camera text on screen
 */
void Debug_mode_output(GraphicsContext* gfxCtx) {
    Gfx* gfx;
    Gfx* opaStart;
    GfxPrint printer;
    s32 pad;

    OPEN_DISPS(gfxCtx, "../z_debug.c", 628);

    gfxprint_init(&printer);
    opaStart = POLY_OPA_DISP;
    gfx = gfxopen(POLY_OPA_DISP);
    gSPDisplayList(OVERLAY_DISP++, gfx);
    gfxprint_open(&printer, gfx);

    if ((OREG(0) == 1) || (OREG(0) == 8)) {
        Debug_Print2_output(&printer);
    }

#if DEBUG_FEATURES
    if (debug_mode->regPage != 0) {
        Regs_DrawEditor(&printer);
    }
#endif

    debug_print2_count = 0;

    gfx = gfxprint_close(&printer);
    gSPEndDisplayList(gfx++);
    gfxclose(opaStart, gfx);
    POLY_OPA_DISP = gfx;

    CLOSE_DISPS(gfxCtx, "../z_debug.c", 664);

    gfxprint_cleanup(&printer);
}
