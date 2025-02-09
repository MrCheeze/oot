#include "global.h"
#include "terminal.h"

s8 vidirty = 1;
vu8 __viblack = true;
u8 viextendvstart = 0;
u32 vispecial = OS_VI_DITHER_FILTER_ON | OS_VI_GAMMA_OFF;
f32 vixscale = 1.0;
f32 viyscale = 1.0;

void viBlack(u32 black) {
    if (black) {
        // Black the screen on next call to viRetrace, skip most VI configuration

        PRINTF(VT_COL(YELLOW, BLACK) "osViSetYScale1(%f);\n" VT_RST, 1.0f);

        if (osTvType == OS_TV_PAL) {
            osViSetMode(&osViModePalLan1);
        }

        // Reset the VI y scale. The VI y scale is different between NTSC (1.0) and PAL (0.833)
        // and should be reset to 1.0 during PreNMI to ensure there are no issues when restarting.
        // (see section 30.4.3 VI Processing with PreNMI Events in the N64 Programming Manual)
        osViSetYScale(1.0f);
    } else {
        osViSetMode(&vimode);

        if (viextendvstart != 0) {
            osViExtendVStart(viextendvstart);
        }

        if (vispecial != 0) {
            osViSetSpecialFeatures(vispecial);
        }

        if (vixscale != 1.0f) {
            osViSetXScale(vixscale);
        }

        if (viyscale != 1.0f) {
            PRINTF(VT_COL(YELLOW, BLACK) "osViSetYScale3(%f);\n" VT_RST, viyscale);
            osViSetYScale(viyscale);
        }
    }

    __viblack = black;
}

void viRetrace(void) {
    if (__viblack) {
        osViBlack(true);
    } else {
        osViBlack(false);
    }
}
