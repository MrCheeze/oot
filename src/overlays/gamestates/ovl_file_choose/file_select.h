#ifndef FILE_SELECT_H
#define FILE_SELECT_H

#include "libc/stddef.h"
#include "ultra64.h"
#include "global.h"
#include "versions.h"


#define GET_NEWF(sramCtx, slotNum, index) (sramCtx->readBuff[sram_save_address[slotNum] + offsetof(SaveContext, save.info.playerData.newf[index])])

#define SLOT_OCCUPIED(sramCtx, slotNum) \
    ((GET_NEWF(sramCtx, slotNum, 0) == 'Z') || \
     (GET_NEWF(sramCtx, slotNum, 1) == 'E') || \
     (GET_NEWF(sramCtx, slotNum, 2) == 'L') || \
     (GET_NEWF(sramCtx, slotNum, 3) == 'D') || \
     (GET_NEWF(sramCtx, slotNum, 4) == 'A') || \
     (GET_NEWF(sramCtx, slotNum, 5) == 'Z'))

// Init mode: Loads saves from SRAM, handles initial language selection in PAL N64 versions
// Config mode: Handles the bulk of the file select, various configuration tasks like picking a file, copy/erase, and the options menu
// Select mode: Displays the selected file with various details about it, and allows the player to confirm and open it
typedef enum MenuMode {
    /* 0 */ FS_MENU_MODE_INIT,
    /* 1 */ FS_MENU_MODE_CONFIG,
    /* 2 */ FS_MENU_MODE_SELECT
} MenuMode;

typedef enum ConfigMode {
    /* 00 */ CM_FADE_IN_START,
    /* 01 */ CM_FADE_IN_END,
    /* 02 */ CM_MAIN_MENU,
    /* 03 */ CM_SETUP_COPY_SOURCE,
    /* 04 */ CM_SELECT_COPY_SOURCE,
    /* 05 */ CM_SETUP_COPY_DEST_1,
    /* 06 */ CM_SETUP_COPY_DEST_2,
    /* 07 */ CM_SELECT_COPY_DEST,
    /* 08 */ CM_EXIT_TO_COPY_SOURCE_1,
    /* 09 */ CM_EXIT_TO_COPY_SOURCE_2,
    /* 10 */ CM_SETUP_COPY_CONFIRM_1,
    /* 11 */ CM_SETUP_COPY_CONFIRM_2,
    /* 12 */ CM_COPY_CONFIRM,
    /* 13 */ CM_RETURN_TO_COPY_DEST,
    /* 14 */ CM_COPY_ANIM_1,
    /* 15 */ CM_COPY_ANIM_2,
    /* 16 */ CM_COPY_ANIM_3,
    /* 17 */ CM_COPY_ANIM_4,
    /* 18 */ CM_COPY_ANIM_5,
    /* 19 */ CM_COPY_RETURN_MAIN,
    /* 20 */ CM_SETUP_ERASE_SELECT,
    /* 21 */ CM_ERASE_SELECT,
    /* 22 */ CM_SETUP_ERASE_CONFIRM_1,
    /* 23 */ CM_SETUP_ERASE_CONFIRM_2,
    /* 24 */ CM_ERASE_CONFIRM,
    /* 25 */ CM_EXIT_TO_ERASE_SELECT_1,
    /* 26 */ CM_EXIT_TO_ERASE_SELECT_2,
    /* 27 */ CM_ERASE_ANIM_1,
    /* 28 */ CM_ERASE_ANIM_2,
    /* 29 */ CM_ERASE_ANIM_3,
    /* 30 */ CM_EXIT_ERASE_TO_MAIN,
    /* 31 */ CM_UNUSED_31,
    /* 32 */ CM_ROTATE_TO_NAME_ENTRY,
    /* 33 */ CM_NAME_ENTRY,
    /* 34 */ CM_START_NAME_ENTRY,
    /* 35 */ CM_NAME_ENTRY_TO_MAIN,
    /* 36 */ CM_MAIN_TO_OPTIONS,
    /* 37 */ CM_OPTIONS_MENU,
    /* 38 */ CM_START_OPTIONS,
    /* 39 */ CM_OPTIONS_TO_MAIN,
    /* 40 */ CM_UNUSED_DELAY
} ConfigMode;

typedef enum SelectMode {
    /* 0 */ SM_FADE_MAIN_TO_SELECT,
    /* 1 */ SM_MOVE_FILE_TO_TOP,
    /* 2 */ SM_FADE_IN_FILE_INFO,
    /* 3 */ SM_CONFIRM_FILE,
    /* 4 */ SM_FADE_OUT_FILE_INFO,
    /* 5 */ SM_MOVE_FILE_TO_SLOT,
    /* 6 */ SM_FADE_OUT,
    /* 7 */ SM_LOAD_GAME
} SelectMode;

typedef enum TitleLabel {
    /* 0 */ FS_TITLE_SELECT_FILE,   // "Please select a file."
    /* 1 */ FS_TITLE_OPEN_FILE,     // "Open this file?"
    /* 2 */ FS_TITLE_COPY_FROM,     // "Copy which file?"
    /* 3 */ FS_TITLE_COPY_TO,       // "Copy to which file?"
    /* 4 */ FS_TITLE_COPY_CONFIRM,  // "Are you sure?"
    /* 5 */ FS_TITLE_COPY_COMPLETE, // "File copied."
    /* 6 */ FS_TITLE_ERASE_FILE,    // "Erase which file?"
    /* 7 */ FS_TITLE_ERASE_CONFIRM, // "Are you sure?"
    /* 8 */ FS_TITLE_ERASE_COMPLETE // "File erased."
} TitleLabel;

typedef enum WarningLabel {
    /* -1 */ FS_WARNING_NONE = -1,
    /*  0 */ FS_WARNING_NO_FILE_COPY,   // "No file to copy."
    /*  1 */ FS_WARNING_NO_FILE_ERASE,  // "No file to erase."
    /*  2 */ FS_WARNING_NO_EMPTY_FILES, // "There is no empty file."
    /*  3 */ FS_WARNING_FILE_EMPTY,     // "This is an empty file."
    /*  4 */ FS_WARNING_FILE_IN_USE     // "This file is in use."
} WarningLabel;

typedef enum MainMenuButtonIndex {
    /* 0 */ FS_BTN_MAIN_FILE_1,
    /* 1 */ FS_BTN_MAIN_FILE_2,
    /* 2 */ FS_BTN_MAIN_FILE_3,
    /* 3 */ FS_BTN_MAIN_COPY,
    /* 4 */ FS_BTN_MAIN_ERASE,
    /* 5 */ FS_BTN_MAIN_OPTIONS
} MainMenuButtonIndex;

typedef enum CopyMenuButtonIndex {
    /* 0 */ FS_BTN_COPY_FILE_1,
    /* 1 */ FS_BTN_COPY_FILE_2,
    /* 2 */ FS_BTN_COPY_FILE_3,
    /* 3 */ FS_BTN_COPY_QUIT
} CopyMenuButtonIndex;

typedef enum EraseMenuButtonIndex {
    /* 0 */ FS_BTN_ERASE_FILE_1,
    /* 1 */ FS_BTN_ERASE_FILE_2,
    /* 2 */ FS_BTN_ERASE_FILE_3,
    /* 3 */ FS_BTN_ERASE_QUIT
} EraseMenuButtonIndex;

typedef enum SelectMenuButtonIndex {
    /* 0 */ FS_BTN_SELECT_FILE_1,
    /* 1 */ FS_BTN_SELECT_FILE_2,
    /* 2 */ FS_BTN_SELECT_FILE_3,
    /* 3 */ FS_BTN_SELECT_YES,
    /* 4 */ FS_BTN_SELECT_QUIT
} SelectMenuButtonIndex;

typedef enum ConfirmButtonIndex {
    /* 0 */ FS_BTN_CONFIRM_YES,
    /* 1 */ FS_BTN_CONFIRM_QUIT
} ConfirmButtonIndex;

typedef enum ActionButtonIndex {
    /* 0 */ FS_BTN_ACTION_COPY,
    /* 1 */ FS_BTN_ACTION_ERASE,
    /* 2 */ FS_BTN_ACTION_YES,
    /* 3 */ FS_BTN_ACTION_QUIT
} ActionButtonIndex;

typedef enum SettingIndex {
    /* 0 */ FS_SETTING_AUDIO,
    /* 1 */ FS_SETTING_TARGET,
#if OOT_PAL_N64
    /* 2 */ FS_SETTING_LANGUAGE,
#endif
    /*   */ FS_SETTING_MAX
} SettingIndex;

typedef enum AudioOption {
    /* 0 */ FS_AUDIO_STEREO,
    /* 1 */ FS_AUDIO_MONO,
    /* 2 */ FS_AUDIO_HEADSET,
    /* 3 */ FS_AUDIO_SURROUND
} AudioOption;

typedef enum CharPage {
    /* 0 */ FS_CHAR_PAGE_HIRA,
    /* 1 */ FS_CHAR_PAGE_KATA,
    /* 2 */ FS_CHAR_PAGE_ENG,
    /* 3 */ FS_CHAR_PAGE_HIRA_TO_KATA,
    /* 4 */ FS_CHAR_PAGE_KATA_TO_HIRA,
    /* 5 */ FS_CHAR_PAGE_HIRA_TO_ENG,
    /* 6 */ FS_CHAR_PAGE_ENG_TO_HIRA,
    /* 7 */ FS_CHAR_PAGE_KATA_TO_ENG,
    /* 8 */ FS_CHAR_PAGE_ENG_TO_KATA
} CharPage;

typedef enum KeyboardButton {
    /* 00 */ FS_KBD_BTN_HIRA,
    /* 01 */ FS_KBD_BTN_KATA,
    /* 02 */ FS_KBD_BTN_ENG,
    /* 03 */ FS_KBD_BTN_BACKSPACE,
    /* 04 */ FS_KBD_BTN_END,
    /* 99 */ FS_KBD_BTN_NONE = 99
} KeyboardButton;

void menu_copy_init(GameState* thisx);
void menu_copy_which(GameState* thisx);
void menu_copy_move1(GameState* thisx);
void menu_copy_fdin1(GameState* thisx);
void menu_copy_where(GameState* thisx);
void menu_copy_fdout1(GameState* thisx);
void menu_copy_cancel1(GameState* thisx);
void menu_copy_move2(GameState* thisx);
void menu_copy_fdin2(GameState* thisx);
void menu_copy_ok(GameState* thisx);
void menu_copy_fdout2(GameState* thisx);
void menu_copy_move3(GameState* thisx);
void menu_copy_fdin3(GameState* thisx);
void menu_copy_end(GameState* thisx);
void menu_copy_end1(GameState* thisx);
void menu_copy_end2(GameState* thisx);

void menu_copy_return(GameState* thisx);
void menu_delt_init(GameState* thisx);
void menu_delt_where(GameState* thisx);
void menu_delt_move(GameState* thisx);
void menu_delt_fdin(GameState* thisx);
void menu_delt_ok(GameState* thisx);
void menu_delt_fdout(GameState* thisx);
void menu_delt_cancel(GameState* thisx);
void menu_delt_end(GameState* thisx);
void menu_delt_end1(GameState* thisx);
void menu_delt_end2(GameState* thisx);
void menu_delt_return(GameState* thisx);

void menu_nameset_play(GameState* thisx);
void menu_nameset_fdin(GameState* thisx);
void menu_option_play(GameState* thisx);
void menu_option_fdin(GameState* thisx);

void file_choose_title_draw(GameState* thisx);
void file_choose_menu_draw(GameState* thisx);
void file_choose_decision_draw(GameState* thisx);

void cursor_color_change(GameState* thisx);
void menu_option_display(GameState* thisx);

void menu_nameset_display(GameState* thisx);
void file_choose_moji_set(GraphicsContext* gfxCtx, void* texture, s16 vtx);

#if OOT_VERSION == PAL_1_1
extern s16 D_808124C0[];
#endif

#endif
