#include "global.h"

typedef struct FlagSetEntry {
    /* 0x00 */ u16* value;
    /* 0x04 */ const char* name;
} FlagSetEntry; // size = 0x08

void flg_set(PlayState* play) {
    static s32 a = 0;
    static u32 b = 0;
    static s32 wait_time = 0;
    static s32 i; // ? this doesn't need to be static

    FlagSetEntry entries[53] = {
        { &z_common_data.save.info.eventChkInf[0], "event_chk_inf[0]" },
        { &z_common_data.save.info.eventChkInf[1], "event_chk_inf[1]" },
        { &z_common_data.save.info.eventChkInf[2], "event_chk_inf[2]" },
        { &z_common_data.save.info.eventChkInf[3], "event_chk_inf[3]" },
        { &z_common_data.save.info.eventChkInf[4], "event_chk_inf[4]" },
        { &z_common_data.save.info.eventChkInf[5], "event_chk_inf[5]" },
        { &z_common_data.save.info.eventChkInf[6], "event_chk_inf[6]" },
        { &z_common_data.save.info.eventChkInf[7], "event_chk_inf[7]" },
        { &z_common_data.save.info.eventChkInf[8], "event_chk_inf[8]" },
        { &z_common_data.save.info.eventChkInf[9], "event_chk_inf[9]" },
        { &z_common_data.save.info.eventChkInf[10], "event_chk_inf[10]" },
        { &z_common_data.save.info.eventChkInf[11], "event_chk_inf[11]" },
        { &z_common_data.save.info.eventChkInf[12], "event_chk_inf[12]" },
        { &z_common_data.save.info.eventChkInf[13], "event_chk_inf[13]" },
        { &z_common_data.save.info.itemGetInf[0], "item_get_inf[0]" },
        { &z_common_data.save.info.itemGetInf[1], "item_get_inf[1]" },
        { &z_common_data.save.info.itemGetInf[2], "item_get_inf[2]" },
        { &z_common_data.save.info.itemGetInf[3], "item_get_inf[3]" },
        { &z_common_data.save.info.infTable[0], "inf_table[0]" },
        { &z_common_data.save.info.infTable[1], "inf_table[1]" },
        { &z_common_data.save.info.infTable[2], "inf_table[2]" },
        { &z_common_data.save.info.infTable[3], "inf_table[3]" },
        { &z_common_data.save.info.infTable[4], "inf_table[4]" },
        { &z_common_data.save.info.infTable[5], "inf_table[5]" },
        { &z_common_data.save.info.infTable[6], "inf_table[6]" },
        { &z_common_data.save.info.infTable[7], "inf_table[7]" },
        { &z_common_data.save.info.infTable[8], "inf_table[8]" },
        { &z_common_data.save.info.infTable[9], "inf_table[9]" },
        { &z_common_data.save.info.infTable[10], "inf_table[10]" },
        { &z_common_data.save.info.infTable[11], "inf_table[11]" },
        { &z_common_data.save.info.infTable[12], "inf_table[12]" },
        { &z_common_data.save.info.infTable[13], "inf_table[13]" },
        { &z_common_data.save.info.infTable[14], "inf_table[14]" },
        { &z_common_data.save.info.infTable[15], "inf_table[15]" },
        { &z_common_data.save.info.infTable[16], "inf_table[16]" },
        { &z_common_data.save.info.infTable[17], "inf_table[17]" },
        { &z_common_data.save.info.infTable[18], "inf_table[18]" },
        { &z_common_data.save.info.infTable[19], "inf_table[19]" },
        { &z_common_data.save.info.infTable[20], "inf_table[20]" },
        { &z_common_data.save.info.infTable[21], "inf_table[21]" },
        { &z_common_data.save.info.infTable[22], "inf_table[22]" },
        { &z_common_data.save.info.infTable[23], "inf_table[23]" },
        { &z_common_data.save.info.infTable[24], "inf_table[24]" },
        { &z_common_data.save.info.infTable[25], "inf_table[25]" },
        { &z_common_data.save.info.infTable[26], "inf_table[26]" },
        { &z_common_data.save.info.infTable[27], "inf_table[27]" },
        { &z_common_data.save.info.infTable[28], "inf_table[28]" },
        { &z_common_data.save.info.infTable[29], "inf_table[29]" },
        { &z_common_data.eventInf[0], "event_inf[0]" },
        { &z_common_data.eventInf[1], "event_inf[1]" },
        { &z_common_data.eventInf[2], "event_inf[2]" },
        { &z_common_data.eventInf[3], "event_inf[3]" },
    };

    GraphicsContext* gfxCtx = play->state.gfxCtx;
    Input* input = &play->state.input[0];
    Gfx* gfx;
    Gfx* polyOpa;

    OPEN_DISPS(gfxCtx, "../flg_set.c", 131);

    {
        GfxPrint printer;
        s32 pad;

        polyOpa = POLY_OPA_DISP;
        gfx = gfxopen(polyOpa);
        gSPDisplayList(OVERLAY_DISP++, gfx);

        gfxprint_init(&printer);
        gfxprint_open(&printer, gfx);
        gfxprint_color(&printer, 250, 50, 50, 255);
        gfxprint_locate8x8(&printer, 4, 13);
        gfxprint_printf(&printer, entries[a].name);
        gfxprint_locate8x8(&printer, 4, 15);

        for (i = 15; i >= 0; i--) {
            if ((u32)i == b) {
                gfxprint_color(&printer, 200, 200, 200, 255);
            } else {
                gfxprint_color(&printer, 100, 100, 100, 255);
            }

            if (*entries[a].value & (1 << i)) {
                gfxprint_printf(&printer, "1");
            } else {
                gfxprint_printf(&printer, "0");
            }

            if ((i % 4) == 0) {
                gfxprint_printf(&printer, " ");
            }
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_DLEFT)) {
            wait_time = 10;
            b++;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_DRIGHT)) {
            b--;
            wait_time = 10;
        }

        if (wait_time == 0) {
            if (CHECK_BTN_ALL(input->cur.button, BTN_DLEFT)) {
                b++;
                wait_time = 2;
            }
            if (CHECK_BTN_ALL(input->cur.button, BTN_DRIGHT)) {
                b--;
                wait_time = 2;
            }
        }

        b %= 16;
        if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
            a--;
            if (a < 0) {
                a = 0;
            }
            wait_time = 10;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
            wait_time = 10;
            a++;
            if (!entries[a].value) {
                a--;
            }
        }

        if (wait_time == 0) {
            if (CHECK_BTN_ALL(input->cur.button, BTN_DUP)) {
                a--;
                wait_time = 2;
                if (a < 0) {
                    a = 0;
                }
            }
            if (CHECK_BTN_ALL(input->cur.button, BTN_DDOWN)) {
                wait_time = 2;
                a++;
                if (!entries[a].value) {
                    a--;
                }
            }
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
            *entries[a].value ^= (1 << b);
        }

        if (wait_time != 0) {
            wait_time--;
        }

        gfx = gfxprint_close(&printer);
        gfxprint_cleanup(&printer);

        gSPEndDisplayList(gfx++);
        gfxclose(polyOpa, gfx);
        POLY_OPA_DISP = gfx;
    }

    if (CHECK_BTN_ALL(input->press.button, BTN_L)) {
        play->pauseCtx.debugState = 0;
    }

    CLOSE_DISPS(gfxCtx, "../flg_set.c", 241);
}
