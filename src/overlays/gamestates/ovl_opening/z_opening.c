/*
 * File: z_opening.c
 * Overlay: ovl_opening
 * Description: Initializes the game into the title screen
 */

#include "global.h"

void opening_move(TitleSetupState* this) {
    z_common_data.gameMode = GAMEMODE_TITLE_SCREEN;
    this->state.running = false;
    z_common_data.save.linkAge = LINK_AGE_ADULT;
    save_initialize999();
    z_common_data.save.cutsceneIndex = 0xFFF3;
    z_common_data.sceneLayer = 7;
    SET_NEXT_GAMESTATE(&this->state, play_init, PlayState);
}

void opening_draw(TitleSetupState* this) {
}

void opening_main(GameState* thisx) {
    TitleSetupState* this = (TitleSetupState*)thisx;

    DisplayList_initialize(this->state.gfxCtx, 0, 0, 0);
    opening_move(this);
    opening_draw(this);
}

void opening_cleanup(GameState* thisx) {
}

void opening_init(GameState* thisx) {
    TitleSetupState* this = (TitleSetupState*)thisx;

    R_UPDATE_RATE = 1;
    new_Matrix(&this->state);
    initView(&this->view, this->state.gfxCtx);
    this->state.main = opening_main;
    this->state.destroy = opening_cleanup;
}
