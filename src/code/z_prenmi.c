#include "global.h"
#include "terminal.h"
#include "versions.h"

void game_next_NULL(PreNMIState* this) {
    this->state.running = false;
    this->state.init = NULL;
    this->state.size = 0;
}

void prenmi_move(PreNMIState* this) {
    PRINTF(VT_COL(YELLOW, BLACK) "prenmi_move\n" VT_RST);

    // Strings existing only in rodata
    if (0) {
        PRINTF("../z_prenmi.c");
        PRINTF("(int)volume = %d\n");
    }

    if (this->timer == 0) {
#if OOT_VERSION < PAL_1_0
        osViSetYScale(1.0f);
        osViBlack(true);
#else
        viBlack(true);
#endif
        game_next_NULL(this);
        return;
    }

    this->timer--;
}

void prenmi_draw(PreNMIState* this) {
    GraphicsContext* gfxCtx = this->state.gfxCtx;

    PRINTF(VT_COL(YELLOW, BLACK) "prenmi_draw\n" VT_RST);

    OPEN_DISPS(gfxCtx, "../z_prenmi.c", 96);

    gSPSegment(POLY_OPA_DISP++, 0x00, NULL);
    DisplayList_initialize(gfxCtx, 0, 0, 0);
    fill_rectangle(gfxCtx);
    gDPSetFillColor(POLY_OPA_DISP++, (GPACK_RGBA5551(255, 255, 255, 1) << 16) | GPACK_RGBA5551(255, 255, 255, 1));
    gDPFillRectangle(POLY_OPA_DISP++, 0, this->timer + 100, SCREEN_WIDTH - 1, this->timer + 100);

    CLOSE_DISPS(gfxCtx, "../z_prenmi.c", 112);
}

void prenmi_main(GameState* thisx) {
    PreNMIState* this = (PreNMIState*)thisx;

    prenmi_move(this);
    prenmi_draw(this);

    this->state.inPreNMIState = true;
}

void prenmi_cleanup(GameState* thisx) {
}

void prenmi_init(GameState* thisx) {
    PreNMIState* this = (PreNMIState*)thisx;

    this->state.main = prenmi_main;
    this->state.destroy = prenmi_cleanup;
    this->timer = 30;
    this->unk_A8 = 10;

    R_UPDATE_RATE = 1;
}
