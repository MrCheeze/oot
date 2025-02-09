#include "global.h"

void exit_game(SetupState* this) {
    PRINTF(T("ゼルダ共通データ初期化\n", "Zelda common data initialization\n"));
    z_common_data_init();
    this->state.running = false;
    SET_NEXT_GAMESTATE(&this->state, title_init, ConsoleLogoState);
}

void first_game_cleanup(GameState* thisx) {
}

void first_game_init(GameState* thisx) {
    SetupState* this = (SetupState*)thisx;

    this->state.destroy = first_game_cleanup;
    exit_game(this);
}
