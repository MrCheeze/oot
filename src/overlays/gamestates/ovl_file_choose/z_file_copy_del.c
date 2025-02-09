#include "file_select.h"

// In iQue versions, this file contains many workarounds for EGCS internal
// compiler errors (see docs/compilers.md)

// when choosing a file to copy or erase, the 6 main menu buttons are placed at these offsets
static s16 move_ypd[] = { -48, -48, -48, -24, -24, 0 };

static s16 move_ypd0[3][3] = {
    { 0, -48, -48 },
    { -64, 16, -48 },
    { -64, -64, 32 },
};

static s16 wait_frame = 15;

/**
 * Move buttons into place for the select source screen and fade in the proper labels.
 * Update function for `CM_SETUP_COPY_SOURCE`
 */
void menu_copy_init(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 yStep;
    s16 i;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    for (i = 0; i < 5; i++) {
        yStep = (ABS(this->buttonYOffsets[i] - move_ypd[i])) / this->actionTimer;

        if (this->buttonYOffsets[i] >= move_ypd[i]) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }
#else
    array = this->buttonYOffsets;
    for (i = 0; i < 5; i++) {
        yStep = (ABS(array[i] - move_ypd[i])) / this->actionTimer;

        if (array[i] >= move_ypd[i]) {
            array[i] -= yStep;
        } else {
            array[i] += yStep;
        }
    }
#endif

    this->actionButtonAlpha[FS_BTN_ACTION_COPY] -= 25;
    this->actionButtonAlpha[FS_BTN_ACTION_ERASE] -= 25;
    this->optionButtonAlpha -= 25;
    this->actionButtonAlpha[FS_BTN_ACTION_QUIT] += 25;
    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionTimer = 8;

        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
            this->optionButtonAlpha = 0;

        this->actionButtonAlpha[FS_BTN_ACTION_QUIT] = 200;
        this->titleLabel = this->nextTitleLabel;

        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->buttonIndex = FS_BTN_COPY_QUIT;
        this->configMode++;
    }
}

/**
 * Allow the player to select a file to copy or exit back to the main menu.
 * Update function for `CM_SELECT_COPY_SOURCE`
 */
void menu_copy_which(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Input* input = &this->state.input[0];

    if (((this->buttonIndex == FS_BTN_COPY_QUIT) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->actionTimer = 8;
        this->buttonIndex = FS_BTN_MAIN_COPY;
        this->nextTitleLabel = FS_TITLE_SELECT_FILE;
        this->configMode = CM_COPY_RETURN_MAIN;
        this->warningLabel = FS_WARNING_NONE;
        Nai_FxFlagEntry(NA_SE_SY_FSEL_CLOSE, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        if (SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
            this->actionTimer = 8;
            this->selectedFileIndex = this->buttonIndex;
            this->configMode = CM_SETUP_COPY_DEST_1;
            this->nextTitleLabel = FS_TITLE_COPY_TO;
            Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_L, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        } else {
            Nai_FxFlagEntry(NA_SE_SY_FSEL_ERROR, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    } else {
        if (ABS(this->stickAdjY) >= 30) {
            Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);

            if (this->stickAdjY >= 30) {
                this->buttonIndex--;

                if (this->buttonIndex < FS_BTN_COPY_FILE_1) {
                    this->buttonIndex = FS_BTN_COPY_QUIT;
                }
            } else {
                this->buttonIndex++;

                if (this->buttonIndex > FS_BTN_COPY_QUIT) {
                    this->buttonIndex = FS_BTN_COPY_FILE_1;
                }
            }
        }

        if (this->buttonIndex != FS_BTN_COPY_QUIT) {
            if (!SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
                this->warningLabel = FS_WARNING_FILE_EMPTY;
                this->warningButtonIndex = this->buttonIndex;
                this->emptyFileTextAlpha = 255;
            } else {
                this->warningLabel = FS_WARNING_NONE;
            }
        }
    }
}

/**
 * Move the menu buttons into place for the copy destination selection and switch titles.
 * Update function for `CM_SETUP_COPY_DEST_1`
 */
void menu_copy_move1(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 yStep;
    s16 i;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    for (i = 0; i < 3; i++) {
        yStep = ABS(this->buttonYOffsets[i] - move_ypd0[this->buttonIndex][i]) / this->actionTimer;

        if (this->buttonYOffsets[i] <= move_ypd0[this->buttonIndex][i]) {
            this->buttonYOffsets[i] += yStep;
        } else {
            this->buttonYOffsets[i] -= yStep;
        }
    }
#else
    array = this->buttonYOffsets;
    for (i = 0; i < 3; i++) {
        yStep = ABS(array[i] - move_ypd0[this->buttonIndex][i]) / this->actionTimer;

        if (array[i] <= move_ypd0[this->buttonIndex][i]) {
            array[i] += yStep;
        } else {
            array[i] -= yStep;
        }
    }
#endif

#if !PLATFORM_IQUE
    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
    this->nameBoxAlpha[this->buttonIndex] -= 25;
#else
    array = this->titleAlpha;
    array[0] -= 31;
    array[1] += 31;
    array = this->nameBoxAlpha;
    array[this->buttonIndex] -= 25;
#endif

    this->actionTimer--;
    if (this->actionTimer == 0) {
#if !PLATFORM_IQUE
        this->buttonYOffsets[this->buttonIndex] = move_ypd0[this->buttonIndex][this->buttonIndex];
#else
        array = this->buttonYOffsets;
        array[this->buttonIndex] = move_ypd0[this->buttonIndex][this->buttonIndex];
#endif

        this->titleLabel = this->nextTitleLabel;

#if !PLATFORM_IQUE
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
#else
        array = this->titleAlpha;
        array[0] = 255;
        array[1] = 0;
#endif

        this->actionTimer = 8;
        this->configMode++;
    }
}

/**
 * Show the file info of the file selected to copy from.
 * Update function for `CM_SETUP_COPY_DEST_2`
 */
void menu_copy_fdin1(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    this->nameBoxAlpha[this->buttonIndex] -= 25;
    this->fileInfoAlpha[this->buttonIndex] += 25;
#else
    array = this->nameBoxAlpha;
    array[this->buttonIndex] -= 25;
    array = this->fileInfoAlpha;
    array[this->buttonIndex] += 25;
#endif

    this->actionTimer--;

    if (this->actionTimer == 0) {
#if !PLATFORM_IQUE
        this->nameBoxAlpha[this->buttonIndex] = 0;
        this->fileInfoAlpha[this->buttonIndex] = 200;
#else
        array = this->nameBoxAlpha;
        array[this->buttonIndex] = 0;
        array = this->fileInfoAlpha;
        array[this->buttonIndex] = 200;
#endif

        this->buttonIndex = FS_BTN_COPY_QUIT;
        this->actionTimer = 8;
        this->configMode = CM_SELECT_COPY_DEST;
    }
}

/**
 * Allow the player to select a slot to copy to or exit to the copy select screen.
 * Update function for `CM_SELECT_COPY_DEST`
 */
void menu_copy_where(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Input* input = &this->state.input[0];

    if (((this->buttonIndex == FS_BTN_COPY_QUIT) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->buttonIndex = this->selectedFileIndex;
        this->nextTitleLabel = FS_TITLE_COPY_FROM;
        this->actionTimer = 8;
        this->configMode = CM_EXIT_TO_COPY_SOURCE_1;
        Nai_FxFlagEntry(NA_SE_SY_FSEL_CLOSE, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        if (!SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
            this->copyDestFileIndex = this->buttonIndex;
            this->nextTitleLabel = FS_TITLE_COPY_CONFIRM;
            this->actionTimer = 8;
            this->configMode = CM_SETUP_COPY_CONFIRM_1;
            Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_L, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        } else {
            Nai_FxFlagEntry(NA_SE_SY_FSEL_ERROR, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    } else {

        if (ABS(this->stickAdjY) >= 30) {
            Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);

            if (this->stickAdjY >= 30) {
                this->buttonIndex--;

                if (this->buttonIndex == this->selectedFileIndex) {
                    this->buttonIndex--;

                    if (this->buttonIndex < FS_BTN_COPY_FILE_1) {
                        this->buttonIndex = FS_BTN_COPY_QUIT;
                    }
                } else {
                    if (this->buttonIndex < FS_BTN_COPY_FILE_1) {
                        this->buttonIndex = FS_BTN_COPY_QUIT;
                    }
                }
            } else {
                this->buttonIndex++;

                if (this->buttonIndex > FS_BTN_COPY_QUIT) {
                    this->buttonIndex = FS_BTN_COPY_FILE_1;
                }

                if (this->buttonIndex == this->selectedFileIndex) {
                    this->buttonIndex++;
                }
            }
        }

        if (this->buttonIndex != FS_BTN_COPY_QUIT) {
            if (SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
                this->warningLabel = FS_WARNING_FILE_IN_USE;
                this->warningButtonIndex = this->buttonIndex;
                this->emptyFileTextAlpha = 255;
            } else {
                this->warningLabel = FS_WARNING_NONE;
            }
        }
    }
}

/**
 * Fade out file info, bring back the name box, and get ready to return to copy source screen.
 * Update function for `CM_EXIT_TO_COPY_SOURCE_1`
 */
void menu_copy_fdout1(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    this->fileInfoAlpha[this->buttonIndex] -= 25;
    this->nameBoxAlpha[this->buttonIndex] += 25;
#else
    array = this->fileInfoAlpha;
    array[this->buttonIndex] -= 25;
    array = this->nameBoxAlpha;
    array[this->buttonIndex] += 25;
#endif

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->nextTitleLabel = FS_TITLE_COPY_FROM;

#if !PLATFORM_IQUE
        this->nameBoxAlpha[this->buttonIndex] = 200;
        this->fileInfoAlpha[this->buttonIndex] = 0;
#else
        array = this->nameBoxAlpha;
        array[this->buttonIndex] = 200;
        array = this->fileInfoAlpha;
        array[this->buttonIndex] = 0;
#endif

        this->actionTimer = 8;
        this->configMode++;
    }
}

/**
 * Move the buttons back into place and return to copy source select.
 * Update function for `CM_EXIT_TO_COPY_SOURCE_2`
 */
void menu_copy_cancel1(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s16 yStep;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    for (i = 0; i < 3; i++) {
        yStep = ABS(this->buttonYOffsets[i] - move_ypd[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= move_ypd[i]) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }
#else
    array = this->buttonYOffsets;
    for (i = 0; i < 3; i++) {
        yStep = ABS(array[i] - move_ypd[i]) / this->actionTimer;

        if (array[i] >= move_ypd[i]) {
            array[i] -= yStep;
        } else {
            array[i] += yStep;
        }
    }
#endif

#if !PLATFORM_IQUE
    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
#else
    array = this->titleAlpha;
    array[0] -= 31;
    array[1] += 31;
#endif

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->titleLabel = this->nextTitleLabel;

#if !PLATFORM_IQUE
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
#else
        array = this->titleAlpha;
        array[0] = 255;
        array[1] = 0;
#endif

        this->buttonIndex = FS_BTN_COPY_QUIT;
        this->configMode = CM_SELECT_COPY_SOURCE;
    }
}

/**
 * Rearrange buttons on the screen to prepare for copy confirmation.
 * Update function for `CM_SETUP_COPY_CONFIRM_1`
 */
void menu_copy_move2(GameState* thisx) {
    static s16 move_ypd[] = { -56, -40, -24, 0 };
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s16 yStep;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
#else
    array = this->titleAlpha;
    array[0] -= 31;
    array[1] += 31;
#endif

#if !PLATFORM_IQUE
    for (i = 0; i < 3; i++) {
        if ((i != this->copyDestFileIndex) && (i != this->selectedFileIndex)) {
            this->fileButtonAlpha[i] -= 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] -= 31;
            }
        } else if (i == this->copyDestFileIndex) {
            yStep = ABS(this->buttonYOffsets[i] - move_ypd[i]) / this->actionTimer;
            this->buttonYOffsets[i] += yStep;

            if (this->buttonYOffsets[i] >= move_ypd[i]) {
                this->buttonYOffsets[i] = move_ypd[i];
            }
        }
    }
#else
    for (i = 0; i < 3; i++) {
        if ((i != this->copyDestFileIndex) && (i != this->selectedFileIndex)) {
            array = this->fileButtonAlpha;
            array[i] -= 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                s16* nameBoxAlpha = this->nameBoxAlpha;
                s16* nameAlpha = this->nameAlpha;
                s16* fileButtonAlpha = this->fileButtonAlpha;

                nameBoxAlpha[i] = nameAlpha[i] = fileButtonAlpha[i];

                array = this->connectorAlpha;
                array[i] -= 31;
            }
        } else if (i == this->copyDestFileIndex) {
            array = this->buttonYOffsets;
            yStep = ABS(array[i] - move_ypd[i]) / this->actionTimer;
            array[i] += yStep;

            if (array[i] >= move_ypd[i]) {
                array[i] = move_ypd[i];
            }
        }
    }
#endif

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->titleLabel = this->nextTitleLabel;

#if !PLATFORM_IQUE
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
#else
        array = this->titleAlpha;
        array[0] = 255;
        array[1] = 0;
#endif

        this->actionTimer = 8;
        this->configMode++;
    }
}

/**
 * Fade in the 'Yes' button before allowing the player to decide.
 * Update function for `CM_SETUP_COPY_CONFIRM_2`
 */
void menu_copy_fdin2(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->actionButtonAlpha[FS_BTN_ACTION_YES] += 25;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->configMode = CM_COPY_CONFIRM;
        this->buttonIndex = FS_BTN_CONFIRM_QUIT;
    }
}

/**
 * Allow the player to confirm the copy, or quit back to the destination select.
 * If yes is selected, the actual copy occurs in this function before moving on to the animation.
 * Update function for `CM_COPY_CONFIRM`
 */
void menu_copy_ok(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Input* input = &this->state.input[0];
    u16 dayTime;

    if (((this->buttonIndex != FS_BTN_CONFIRM_YES) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->actionTimer = 8;
        this->nextTitleLabel = FS_TITLE_COPY_TO;
        this->configMode = CM_RETURN_TO_COPY_DEST;
        Nai_FxFlagEntry(NA_SE_SY_FSEL_CLOSE, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        dayTime = z_common_data.save.dayTime;
        sram_start_copy(this, sramCtx);
        z_common_data.save.dayTime = dayTime;
        this->fileInfoAlpha[this->copyDestFileIndex] = this->nameAlpha[this->copyDestFileIndex] = 0;
        this->nextTitleLabel = FS_TITLE_COPY_COMPLETE;
        this->actionTimer = 8;
        this->configMode = CM_COPY_ANIM_1;
        z_vibctl2_vib_setQ(300.0f, 180, 20, 100);
        Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_L, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (ABS(this->stickAdjY) >= 30) {
        Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        this->buttonIndex ^= 1;
    }
}

/**
 * Move buttons back in place and return to copy destination select.
 * Update function for `CM_RETURN_TO_COPY_DEST`
 */
void menu_copy_fdout2(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s16 yStep;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
    this->actionButtonAlpha[FS_BTN_ACTION_YES] -= 25;
#else
    array = this->titleAlpha;
    array[0] -= 31;
    array[1] += 31;
    array = this->actionButtonAlpha;
    array[FS_BTN_ACTION_YES] -= 25;
#endif

#if !PLATFORM_IQUE
    for (i = 0; i < 3; i++) {
        if ((i != this->copyDestFileIndex) && (i != this->selectedFileIndex)) {
            this->fileButtonAlpha[i] += 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] += 31;
            }
        }

        yStep = ABS(this->buttonYOffsets[i] - move_ypd0[this->selectedFileIndex][i]) / this->actionTimer;

        if (this->buttonYOffsets[i] <= move_ypd0[this->selectedFileIndex][i]) {
            this->buttonYOffsets[i] += yStep;
        } else {
            this->buttonYOffsets[i] -= yStep;
        }
    }
#else
    for (i = 0; i < 3; i++) {
        if ((i != this->copyDestFileIndex) && (i != this->selectedFileIndex)) {
            array = this->fileButtonAlpha;
            array[i] += 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                s16* nameBoxAlpha = this->nameBoxAlpha;
                s16* nameAlpha = this->nameAlpha;
                s16* fileButtonAlpha = this->fileButtonAlpha;

                nameBoxAlpha[i] = nameAlpha[i] = fileButtonAlpha[i];

                array = this->connectorAlpha;
                array[i] += 31;
            }
        }

        array = this->buttonYOffsets;
        yStep = ABS(array[i] - move_ypd0[this->selectedFileIndex][i]) / this->actionTimer;

        if (array[i] <= move_ypd0[this->selectedFileIndex][i]) {
            array[i] += yStep;
        } else {
            array[i] -= yStep;
        }
    }
#endif

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->titleLabel = this->nextTitleLabel;

#if !PLATFORM_IQUE
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
#else
        array = this->titleAlpha;
        array[0] = 255;
        array[1] = 0;
#endif

        this->actionTimer = 8;
        this->buttonIndex = FS_BTN_COPY_QUIT;
        this->configMode = CM_SELECT_COPY_DEST;
    }
}

/**
 * Hide title
 * Update function for `CM_COPY_ANIM_1`
 */
void menu_copy_move3(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->titleAlpha[0] -= 31;
    this->actionButtonAlpha[FS_BTN_ACTION_YES] -= 25;
    this->actionButtonAlpha[FS_BTN_ACTION_QUIT] -= 25;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->titleAlpha[0] = 0;
        this->actionTimer = 8;
        this->configMode++;
        PRINTF("connect_alpha=%d  decision_alpha[%d]=%d\n", this->connectorAlpha[this->copyDestFileIndex],
               this->copyDestFileIndex, this->fileInfoAlpha[this->copyDestFileIndex]);
    }
}

/**
 * Move a copy of the file window down and fade in the file info.
 * Update function for `CM_COPY_ANIM_2`
 */
void menu_copy_fdin3(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 yStep;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    this->fileInfoAlpha[this->copyDestFileIndex] += 25;
    this->nameAlpha[this->copyDestFileIndex] += 25;
    this->titleAlpha[1] += 31;
#else
    array = this->fileInfoAlpha;
    array[this->copyDestFileIndex] += 25;
    array = this->nameAlpha;
    array[this->copyDestFileIndex] += 25;
    array = this->titleAlpha;
    array[1] += 31;
#endif

#if !PLATFORM_IQUE
    yStep = ABS(this->fileNamesY[this->copyDestFileIndex] - -56) / this->actionTimer;
    this->fileNamesY[this->copyDestFileIndex] -= yStep;

    if (this->fileNamesY[this->copyDestFileIndex] <= -56) {
        this->fileNamesY[this->copyDestFileIndex] = -56;
    }
#else
    array = this->fileNamesY;
    yStep = ABS(array[this->copyDestFileIndex] - -56) / this->actionTimer;
    array[this->copyDestFileIndex] -= yStep;

    if (array[this->copyDestFileIndex] <= -56) {
        array[this->copyDestFileIndex] = -56;
    }
#endif

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionTimer = 90;
        this->titleLabel = this->nextTitleLabel;

#if !PLATFORM_IQUE
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
#else
        array = this->titleAlpha;
        array[0] = 255;
        array[1] = 0;
#endif

        this->configMode++;
    }
}

/**
 * Play a sound effect to indicate that the copy has completed. Wait for a timer or for
 * the player to press a button before moving on.
 * Update function for `CM_COPY_ANIM_3`
 */
void menu_copy_end(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    Input* input = &this->state.input[0];

    if (this->actionTimer == 75) {
        this->connectorAlpha[this->copyDestFileIndex] = 255;
        Nai_FxFlagEntry(NA_SE_EV_DIAMOND_SWITCH, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }

    this->actionTimer--;

    if (this->actionTimer < 74) {
        if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_B | BTN_START) || (this->actionTimer == 0)) {
            this->actionTimer = 8;
            this->nextTitleLabel = FS_TITLE_SELECT_FILE;
            this->configMode++;
            Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_L, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    }
}

/**
 * Fade out the info boxes for both files and bring in their name boxes. Fade out title.
 * Update function for `CM_COPY_ANIM_4`
 */
void menu_copy_end1(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16* array;

#if !PLATFORM_IQUE
    this->fileInfoAlpha[this->selectedFileIndex] -= 25;
    this->fileInfoAlpha[this->copyDestFileIndex] -= 25;
    this->nameBoxAlpha[this->selectedFileIndex] += 25;
    this->nameBoxAlpha[this->copyDestFileIndex] += 25;
    this->titleAlpha[0] -= 31;
#else
    array = this->fileInfoAlpha;
    array[this->selectedFileIndex] -= 25;
    array[this->copyDestFileIndex] -= 25;
    array = this->nameBoxAlpha;
    array[this->selectedFileIndex] += 25;
    array[this->copyDestFileIndex] += 25;
    array = this->titleAlpha;
    array[0] -= 31;
#endif

    this->actionTimer--;

    if (this->actionTimer == 0) {
#if !PLATFORM_IQUE
        this->fileNamesY[this->copyDestFileIndex] = this->buttonYOffsets[3] = 0;
#else
        array = this->fileNamesY;
        array[this->copyDestFileIndex] = 0;
        this->buttonYOffsets[3] = 0;
#endif

        this->actionTimer = 8;
        this->titleAlpha[0] = 0;
        this->configMode++;
    }
}

/**
 * Restore all buttons and labels back to their original place and go back to the main menu.
 * Update function for `CM_COPY_ANIM_5`
 */
void menu_copy_end2(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s16 yStep;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    for (i = 0; i < 5; i++) {
        yStep = ABS(this->buttonYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= 0) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }
#else
    array = this->buttonYOffsets;
    for (i = 0; i < 5; i++) {
        yStep = ABS(array[i]) / this->actionTimer;

        if (array[i] >= 0) {
            array[i] -= yStep;
        } else {
            array[i] += yStep;
        }
    }
#endif

#if !PLATFORM_IQUE
    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            this->fileButtonAlpha[i] += 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] += 31;
            }
        }
    }
#else
    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            array = this->fileButtonAlpha;
            array[i] += 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                s16* nameBoxAlpha = this->nameBoxAlpha;
                s16* nameAlpha = this->nameAlpha;
                s16* fileButtonAlpha = this->fileButtonAlpha;

                nameBoxAlpha[i] = nameAlpha[i] = fileButtonAlpha[i];

                array = this->connectorAlpha;
                array[i] += 31;
            }
        }
    }
#endif

#if !PLATFORM_IQUE
    this->actionButtonAlpha[FS_BTN_ACTION_COPY] += 25;
    this->actionButtonAlpha[FS_BTN_ACTION_ERASE] += 25;
    this->optionButtonAlpha += 25;
    this->titleAlpha[1] += 31;
#else
    array = this->actionButtonAlpha;
    array[FS_BTN_ACTION_COPY] += 25;
    array[FS_BTN_ACTION_ERASE] += 25;
    this->optionButtonAlpha += 25;
    array = this->titleAlpha;
    array[1] += 31;
#endif

    this->actionTimer--;

    if (this->actionTimer == 0) {
#if !PLATFORM_IQUE
        for (i = 0; i < 3; i++) {
            this->fileButtonAlpha[i] = 200;
            this->nameBoxAlpha[i] = this->nameAlpha[i] = this->connectorAlpha[i] = 0;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] = 255;
            }
        }
#else
        for (i = 0; i < 3; i++) {
            array = this->fileButtonAlpha;
            array[i] = 200;
            array = this->nameBoxAlpha;
            array[i] = 0;
            array = this->nameAlpha;
            array[i] = 0;
            array = this->connectorAlpha;
            array[i] = 0;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                s16* nameBoxAlpha = this->nameBoxAlpha;
                s16* nameAlpha = this->nameAlpha;
                s16* fileButtonAlpha = this->fileButtonAlpha;

                nameBoxAlpha[i] = nameAlpha[i] = fileButtonAlpha[i];

                array = this->connectorAlpha;
                array[i] = 255;
            }
        }
#endif

#if !PLATFORM_IQUE
        this->fileNamesY[this->selectedFileIndex] = 0;
#else
        array = this->fileNamesY;
        array[this->selectedFileIndex] = 0;
#endif

        this->highlightColor[3] = 70;
        this->highlightPulseDir = 1;
        XREG(35) = XREG(36);
        this->titleLabel = this->nextTitleLabel;

#if !PLATFORM_IQUE
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
#else
        array = this->titleAlpha;
        array[0] = 255;
        array[1] = 0;
#endif

        this->configMode = CM_MAIN_MENU;
    }
}

/**
 * Exit from the copy source screen to the main menu. Return all buttons and labels to their original place.
 * Update function for `CM_COPY_RETURN_MAIN`
 */
void menu_copy_return(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 i;
    s16 yStep;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    for (i = 0; i < 5; i++) {
        yStep = ABS(this->buttonYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= 0) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }
#else
    array = this->buttonYOffsets;
    for (i = 0; i < 5; i++) {
        yStep = ABS(array[i]) / this->actionTimer;

        if (array[i] >= 0) {
            array[i] -= yStep;
        } else {
            array[i] += yStep;
        }
    }
#endif

#if !PLATFORM_IQUE
    this->actionButtonAlpha[FS_BTN_ACTION_COPY] += 25;
    this->actionButtonAlpha[FS_BTN_ACTION_QUIT] -= 25;
    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
#else
    array = this->actionButtonAlpha;
    array[FS_BTN_ACTION_COPY] += 25;
    array[FS_BTN_ACTION_QUIT] -= 25;
    array = this->titleAlpha;
    array[0] -= 31;
    array[1] += 31;
#endif

    this->actionTimer--;

    if (this->actionTimer == 0) {
#if !PLATFORM_IQUE
        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = 200;
        this->actionButtonAlpha[FS_BTN_ACTION_QUIT] = 0;
#else
        array = this->actionButtonAlpha;
        array[FS_BTN_ACTION_COPY] = 200;
        array[FS_BTN_ACTION_QUIT] = 0;
#endif

        this->titleLabel = this->nextTitleLabel;

#if !PLATFORM_IQUE
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
#else
        array = this->titleAlpha;
        array[0] = 255;
        array[1] = 0;
#endif

        this->configMode = CM_MAIN_MENU;
    }

#if !PLATFORM_IQUE
    this->optionButtonAlpha = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
        this->actionButtonAlpha[FS_BTN_ACTION_COPY];
#else
    array = this->actionButtonAlpha;
    this->optionButtonAlpha = array[FS_BTN_ACTION_ERASE] = array[FS_BTN_ACTION_COPY];
#endif
}

/**
 * Move buttons into place for the erase select screen and fade in the proper labels.
 * Update function for `CM_SETUP_ERASE_SELECT`
 */
void menu_delt_init(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 i;
    s16 yStep;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    for (i = 0; i < 5; i++) {
        yStep = ABS(this->buttonYOffsets[i] - move_ypd[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= move_ypd[i]) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }
#else
    array = this->buttonYOffsets;
    for (i = 0; i < 5; i++) {
        yStep = ABS(array[i] - move_ypd[i]) / this->actionTimer;

        if (array[i] >= move_ypd[i]) {
            array[i] -= yStep;
        } else {
            array[i] += yStep;
        }
    }
#endif

#if !PLATFORM_IQUE
    this->actionButtonAlpha[FS_BTN_ACTION_COPY] -= 50;
    this->actionButtonAlpha[FS_BTN_ACTION_ERASE] -= 50;
    this->optionButtonAlpha -= 50;
    this->actionButtonAlpha[FS_BTN_ACTION_QUIT] += 25;
#else
    array = this->actionButtonAlpha;
    array[FS_BTN_ACTION_COPY] -= 50;
    array[FS_BTN_ACTION_ERASE] -= 50;
    this->optionButtonAlpha -= 50;
    array[FS_BTN_ACTION_QUIT] += 25;
#endif

    if (this->actionButtonAlpha[FS_BTN_ACTION_COPY] <= 0) {
        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
            this->optionButtonAlpha = 0;
    }

#if !PLATFORM_IQUE
    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
#else
    array = this->titleAlpha;
    array[0] -= 31;
    array[1] += 31;
#endif

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->highlightColor[3] = 70;
        this->highlightPulseDir = 1;
        XREG(35) = XREG(36);
        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
            this->optionButtonAlpha = 0;

#if !PLATFORM_IQUE
        this->actionButtonAlpha[FS_BTN_ACTION_QUIT] = 200;
#else
        array = this->actionButtonAlpha;
        array[FS_BTN_ACTION_QUIT] = 200;
#endif

        this->titleLabel = this->nextTitleLabel;

#if !PLATFORM_IQUE
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
#else
        array = this->titleAlpha;
        array[0] = 255;
        array[1] = 0;
#endif

        this->buttonIndex = FS_BTN_ERASE_QUIT;
        this->configMode++;
    }
}

/**
 * Allow the player to select a file to erase or exit back to the main menu.
 * Update function for `CM_ERASE_SELECT`
 */
void menu_delt_where(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Input* input = &this->state.input[0];

    if (((this->buttonIndex == FS_BTN_COPY_QUIT) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->buttonIndex = FS_BTN_MAIN_ERASE;
        this->actionTimer = 8;
        this->nextTitleLabel = FS_TITLE_SELECT_FILE;
        this->configMode = CM_EXIT_ERASE_TO_MAIN;
        this->warningLabel = FS_WARNING_NONE;
        Nai_FxFlagEntry(NA_SE_SY_FSEL_CLOSE, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        if (SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
            this->actionTimer = 8;
            this->selectedFileIndex = this->buttonIndex;
            this->configMode = CM_SETUP_ERASE_CONFIRM_1;
            this->nextTitleLabel = FS_TITLE_ERASE_CONFIRM;
            Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_L, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        } else {
            Nai_FxFlagEntry(NA_SE_SY_FSEL_ERROR, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    } else {
        if (ABS(this->stickAdjY) >= 30) {
            Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);

            if (this->stickAdjY >= 30) {
                this->buttonIndex--;
                if (this->buttonIndex < FS_BTN_ERASE_FILE_1) {
                    this->buttonIndex = FS_BTN_ERASE_QUIT;
                }
            } else {
                this->buttonIndex++;
                if (this->buttonIndex > FS_BTN_ERASE_QUIT) {
                    this->buttonIndex = FS_BTN_ERASE_FILE_1;
                }
            }
        }

        if (this->buttonIndex != FS_BTN_ERASE_QUIT) {
            if (!SLOT_OCCUPIED(sramCtx, this->buttonIndex)) {
                this->warningLabel = FS_WARNING_FILE_EMPTY;
                this->warningButtonIndex = this->buttonIndex;
                this->emptyFileTextAlpha = 255;
            } else {
                this->warningLabel = FS_WARNING_NONE;
            }
        } else {
            this->warningLabel = FS_WARNING_NONE;
        }
    }
}

/**
 * ...
 * Update function for `CM_SETUP_ERASE_CONFIRM_1`
 */
void menu_delt_move(GameState* thisx) {
    static s16 move_ypd[] = { 0, 16, 32 };
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s16 yStep;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            this->fileButtonAlpha[i] -= 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] -= 31;
            }
        } else {
            this->nameBoxAlpha[i] -= 25;
        }
    }
#else
    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            array = this->fileButtonAlpha;
            array[i] -= 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                s16* nameBoxAlpha = this->nameBoxAlpha;
                s16* nameAlpha = this->nameAlpha;
                s16* fileButtonAlpha = this->fileButtonAlpha;

                nameBoxAlpha[i] = nameAlpha[i] = fileButtonAlpha[i];

                array = this->connectorAlpha;
                array[i] -= 31;
            }
        } else {
            array = this->nameBoxAlpha;
            array[i] -= 25;
        }
    }
#endif

#if !PLATFORM_IQUE
    this->titleAlpha[0] -= 15;
    this->titleAlpha[1] += 15;
#else
    array = this->titleAlpha;
    array[0] -= 15;
    array[1] += 15;
#endif

#if !PLATFORM_IQUE
    yStep = ABS(this->buttonYOffsets[this->buttonIndex] - move_ypd[this->buttonIndex]) / this->actionTimer;

    if (this->buttonYOffsets[this->buttonIndex] >= move_ypd[this->buttonIndex]) {
        this->buttonYOffsets[this->buttonIndex] -= yStep;
    } else {
        this->buttonYOffsets[this->buttonIndex] += yStep;
    }
#else
    array = this->buttonYOffsets;
    yStep = ABS(array[this->buttonIndex] - move_ypd[this->buttonIndex]) / this->actionTimer;

    if (array[this->buttonIndex] >= move_ypd[this->buttonIndex]) {
        array[this->buttonIndex] -= yStep;
    } else {
        array[this->buttonIndex] += yStep;
    }
#endif

    this->actionTimer--;

    if (this->actionTimer == 0) {
#if !PLATFORM_IQUE
        for (i = 0; i < 3; i++) {
            if (i != this->buttonIndex) {
                this->fileButtonAlpha[i] = 0;

                if (SLOT_OCCUPIED(sramCtx, i)) {
                    this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                    this->connectorAlpha[i] = 0;
                }
            } else {
                this->nameBoxAlpha[i] = 0;
            }
        }
#else
        for (i = 0; i < 3; i++) {
            if (i != this->buttonIndex) {
                array = this->fileButtonAlpha;
                array[i] = 0;

                if (SLOT_OCCUPIED(sramCtx, i)) {
                    s16* nameBoxAlpha = this->nameBoxAlpha;
                    s16* nameAlpha = this->nameAlpha;
                    s16* fileButtonAlpha = this->fileButtonAlpha;

                    nameBoxAlpha[i] = nameAlpha[i] = fileButtonAlpha[i];

                    array = this->connectorAlpha;
                    array[i] = 0;
                }
            } else {
                array = this->nameBoxAlpha;
                array[i] = 0;
            }
        }
#endif

#if !PLATFORM_IQUE
        this->buttonYOffsets[this->buttonIndex] = move_ypd[this->buttonIndex];
#else
        array = this->buttonYOffsets;
        array[this->buttonIndex] = move_ypd[this->buttonIndex];
#endif

        this->actionTimer = 8;
        this->configMode++;
    }
}

/**
 * Show the file info of the file selected to erase.
 * Update function for `CM_SETUP_ERASE_CONFIRM_2`
 */
void menu_delt_fdin(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    this->actionButtonAlpha[FS_BTN_ACTION_YES] += 25;
    this->titleAlpha[0] -= 15;
    this->titleAlpha[1] += 15;
    this->fileInfoAlpha[this->buttonIndex] += 25;
#else
    array = this->actionButtonAlpha;
    array[FS_BTN_ACTION_YES] += 25;
    array = this->titleAlpha;
    array[0] -= 15;
    array[1] += 15;
    array = this->fileInfoAlpha;
    array[this->buttonIndex] += 25;
#endif

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionTimer = 8;
        this->titleLabel = this->nextTitleLabel;

#if !PLATFORM_IQUE
        this->fileInfoAlpha[this->buttonIndex] = this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->actionButtonAlpha[FS_BTN_ACTION_YES] = 200;
#else
        array = this->titleAlpha;
        this->fileInfoAlpha[this->buttonIndex] = array[0] = 255;
        array[1] = 0;
        array = this->actionButtonAlpha;
        array[FS_BTN_ACTION_YES] = 200;
#endif

        this->buttonIndex = FS_BTN_CONFIRM_QUIT;
        this->configMode = CM_ERASE_CONFIRM;
    }
}

/**
 * Allow the player to confirm their choice to erase or return back to erase select.
 * Update function for `CM_ERASE_CONFIRM`
 */
void menu_delt_ok(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    Input* input = &this->state.input[0];

    if (((this->buttonIndex != FS_BTN_CONFIRM_YES) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->buttonIndex = this->selectedFileIndex;
        this->nextTitleLabel = FS_TITLE_ERASE_FILE;
        this->configMode = CM_EXIT_TO_ERASE_SELECT_1;
        this->actionTimer = 8;
        Nai_FxFlagEntry(NA_SE_SY_FSEL_CLOSE, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        this->n64ddFlags[this->selectedFileIndex] = this->connectorAlpha[this->selectedFileIndex] = 0;
        Nai_FxFlagEntry(NA_SE_EV_DIAMOND_SWITCH, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        this->actionTimer = 8;
        this->configMode = CM_ERASE_ANIM_1;
        this->nextTitleLabel = FS_TITLE_ERASE_COMPLETE;
        z_vibctl2_vib_setQ(200.0f, 255, 20, 150);
        wait_frame = 15;
    } else if (ABS(this->stickAdjY) >= 30) {
        Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        this->buttonIndex ^= 1;
    }
}

/**
 * Fade out file info, bring back the name box, and get ready to return to erase select screen.
 * Update function for `CM_EXIT_TO_ERASE_SELECT_1`
 */
void menu_delt_fdout(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    this->fileInfoAlpha[this->buttonIndex] -= 25;
    this->nameBoxAlpha[this->buttonIndex] += 25;
    this->actionButtonAlpha[FS_BTN_ACTION_YES] -= 25;
#else
    array = this->fileInfoAlpha;
    array[this->buttonIndex] -= 25;
    array = this->nameBoxAlpha;
    array[this->buttonIndex] += 25;
    array = this->actionButtonAlpha;
    array[FS_BTN_ACTION_YES] -= 25;
#endif

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionTimer = 8;

#if !PLATFORM_IQUE
        this->fileInfoAlpha[this->buttonIndex] = 0;
#else
        array = this->fileInfoAlpha;
        array[this->buttonIndex] = 0;
#endif

        this->configMode++;
    }
}

/**
 * Move the buttons back into place and return to erase select.
 * Update function for `CM_EXIT_TO_ERASE_SELECT_2`
 */
void menu_delt_cancel(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s16 yStep;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    yStep = ABS(this->buttonYOffsets[this->buttonIndex] - move_ypd[this->buttonIndex]) / this->actionTimer;

    if (this->buttonYOffsets[this->buttonIndex] >= move_ypd[this->buttonIndex]) {
        this->buttonYOffsets[this->buttonIndex] -= yStep;
    } else {
        this->buttonYOffsets[this->buttonIndex] += yStep;
    }
#else
    array = this->buttonYOffsets;
    yStep = ABS(array[this->buttonIndex] - move_ypd[this->buttonIndex]) / this->actionTimer;

    if (array[this->buttonIndex] >= move_ypd[this->buttonIndex]) {
        array[this->buttonIndex] -= yStep;
    } else {
        array[this->buttonIndex] += yStep;
    }
#endif

#if !PLATFORM_IQUE
    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            this->fileButtonAlpha[i] += 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] += 31;
            }
        }
    }
#else
    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            array = this->fileButtonAlpha;
            array[i] += 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                s16* nameBoxAlpha = this->nameBoxAlpha;
                s16* nameAlpha = this->nameAlpha;
                s16* fileButtonAlpha = this->fileButtonAlpha;

                nameBoxAlpha[i] = nameAlpha[i] = fileButtonAlpha[i];

                array = this->connectorAlpha;
                array[i] += 31;
            }
        }
    }
#endif

#if !PLATFORM_IQUE
    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
#else
    array = this->titleAlpha;
    array[0] -= 31;
    array[1] += 31;
#endif

    this->actionTimer--;

    if (this->actionTimer == 0) {
#if !PLATFORM_IQUE
        this->buttonYOffsets[this->buttonIndex] = move_ypd[this->buttonIndex];
#else
        array = this->buttonYOffsets;
        array[this->buttonIndex] = move_ypd[this->buttonIndex];
#endif

        this->actionTimer = 8;
        this->buttonIndex = FS_BTN_ERASE_QUIT;
        this->titleLabel = this->nextTitleLabel;

#if !PLATFORM_IQUE
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
#else
        array = this->titleAlpha;
        array[0] = 255;
        array[1] = 0;
#endif

        this->configMode = CM_ERASE_SELECT;
    }
}

/**
 * Wait for an initial delay, then start fading out the selected file.
 * The actual file deletion occurs in this function
 * Update function for `CM_ERASE_ANIM_1`
 */
void menu_delt_end(GameState* thisx) {
    static s16 m;
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
#if PLATFORM_IQUE
    s16* array;
#endif

    if (wait_frame == 0) {
        if (this->actionTimer == 8) {
            m = 1;
        }

        if (this->actionTimer != 0) {
#if !PLATFORM_IQUE
            this->titleAlpha[0] -= 31;
            this->titleAlpha[1] += 31;
            this->fileInfoAlpha[this->selectedFileIndex] -= 25;
            this->actionButtonAlpha[FS_BTN_ACTION_YES] -= 25;
            this->actionButtonAlpha[FS_BTN_ACTION_QUIT] -= 25;
#else
            array = this->titleAlpha;
            array[0] -= 31;
            array[1] += 31;
            array = this->fileInfoAlpha;
            array[this->selectedFileIndex] -= 25;
            array = this->actionButtonAlpha;
            array[FS_BTN_ACTION_YES] -= 25;
            array[FS_BTN_ACTION_QUIT] -= 25;
#endif
        }

#if !PLATFORM_IQUE
        this->fileNamesY[this->selectedFileIndex] -= m;
#else
        array = this->fileNamesY;
        array[this->selectedFileIndex] -= m;
#endif

        m += 2;
        this->actionTimer--;

        if (this->actionTimer == 0) {
            sram_start_clear(this, sramCtx);
            this->titleLabel = this->nextTitleLabel;

#if !PLATFORM_IQUE
            this->titleAlpha[0] = 255;
            this->titleAlpha[1] = this->connectorAlpha[this->selectedFileIndex] = 0;
            this->fileInfoAlpha[this->selectedFileIndex] = this->nameBoxAlpha[this->selectedFileIndex] =
                this->actionButtonAlpha[FS_BTN_ACTION_YES] = this->actionButtonAlpha[FS_BTN_ACTION_QUIT] = 0;
#else
            array = this->titleAlpha;
            array[0] = 255;
            array[1] = this->connectorAlpha[this->selectedFileIndex] = 0;
            array = this->fileInfoAlpha;
            array[this->selectedFileIndex] = 0;
            array = this->nameBoxAlpha;
            array[this->selectedFileIndex] = 0;
            array = this->actionButtonAlpha;
            array[FS_BTN_ACTION_YES] = array[FS_BTN_ACTION_QUIT] = 0;
#endif

            this->configMode++;
            this->actionTimer = 90;
        }
    } else {
        wait_frame--;

        if (wait_frame == 0) {
            Nai_FxFlagEntry(NA_SE_OC_ABYSS, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    }
}

/**
 * Wait for a delay timer or for the palyer to press a button before returning to the main menu.
 * Update function for `CM_ERASE_ANIM_2`
 */
void menu_delt_end1(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    Input* input = &this->state.input[0];

    if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_B | BTN_START) || (--this->actionTimer == 0)) {
        this->buttonYOffsets[3] = 0;
        this->actionTimer = 8;
        this->nextTitleLabel = FS_TITLE_SELECT_FILE;
        this->configMode++;
        Nai_FxFlagEntry(NA_SE_SY_FSEL_CLOSE, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
}

/**
 * Exit from the erase animation to the main menu. Return all buttons and labels to their original place.
 * Update function for `CM_ERASE_ANIM_3`
 */
void menu_delt_end2(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    s16 i;
    s16 yStep;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    for (i = 0; i < 5; i++) {
        yStep = ABS(this->buttonYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= 0) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }
#else
    array = this->buttonYOffsets;
    for (i = 0; i < 5; i++) {
        yStep = ABS(array[i]) / this->actionTimer;

        if (array[i] >= 0) {
            array[i] -= yStep;
        } else {
            array[i] += yStep;
        }
    }
#endif

#if !PLATFORM_IQUE
    for (i = 0; i < 3; i++) {
        this->fileButtonAlpha[i] += 25;

        if (SLOT_OCCUPIED(sramCtx, i)) {
            this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
            this->connectorAlpha[i] += 31;
        }
    }
#else
    for (i = 0; i < 3; i++) {
        array = this->fileButtonAlpha;
        array[i] += 25;

        if (SLOT_OCCUPIED(sramCtx, i)) {
            s16* nameBoxAlpha = this->nameBoxAlpha;
            s16* nameAlpha = this->nameAlpha;
            s16* fileButtonAlpha = this->fileButtonAlpha;

            nameBoxAlpha[i] = nameAlpha[i] = fileButtonAlpha[i];

            array = this->connectorAlpha;
            array[i] += 31;
        }
    }
#endif

#if !PLATFORM_IQUE
    if (this->fileButtonAlpha[this->selectedFileIndex] >= 200) {
        this->fileButtonAlpha[this->selectedFileIndex] = 200;
    }
#else
    array = this->fileButtonAlpha;
    if (array[this->selectedFileIndex] >= 200) {
        array[this->selectedFileIndex] = 200;
    }
#endif

#if !PLATFORM_IQUE
    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
#else
    array = this->titleAlpha;
    array[0] -= 31;
    array[1] += 31;
#endif

    this->actionTimer--;

    if (this->actionTimer == 0) {
#if !PLATFORM_IQUE
        this->fileNamesY[this->selectedFileIndex] = 0;
#else
        array = this->fileNamesY;
        array[this->selectedFileIndex] = 0;
#endif

        this->highlightColor[3] = 70;
        this->highlightPulseDir = 1;
        XREG(35) = XREG(36);

#if !PLATFORM_IQUE
        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = 200;
        this->actionButtonAlpha[FS_BTN_ACTION_YES] = this->actionButtonAlpha[FS_BTN_ACTION_QUIT] = 0;
#else
        array = this->actionButtonAlpha;
        array[FS_BTN_ACTION_COPY] = 200;
        array[FS_BTN_ACTION_YES] = array[FS_BTN_ACTION_QUIT] = 0;
#endif

        this->titleLabel = this->nextTitleLabel;

#if !PLATFORM_IQUE
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
#else
        array = this->titleAlpha;
        array[0] = 255;
        array[1] = 0;
#endif

        this->configMode = CM_MAIN_MENU;
    }

#if !PLATFORM_IQUE
    this->optionButtonAlpha = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
        this->actionButtonAlpha[FS_BTN_ACTION_COPY];
#else
    array = this->actionButtonAlpha;
    this->optionButtonAlpha = array[FS_BTN_ACTION_ERASE] = array[FS_BTN_ACTION_COPY];
#endif
}

/**
 * Exit from the erase select screen to the main menu. Return all buttons and labels to their original place.
 * Update function for `CM_EXIT_ERASE_TO_MAIN`
 */
void menu_delt_return(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 i;
    s16 yStep;
#if PLATFORM_IQUE
    s16* array;
#endif

#if !PLATFORM_IQUE
    for (i = 0; i < 5; i++) {
        yStep = ABS(this->buttonYOffsets[i]) / this->actionTimer;

        if (this->buttonYOffsets[i] >= 0) {
            this->buttonYOffsets[i] -= yStep;
        } else {
            this->buttonYOffsets[i] += yStep;
        }
    }
#else
    array = this->buttonYOffsets;
    for (i = 0; i < 5; i++) {
        yStep = ABS(array[i]) / this->actionTimer;

        if (array[i] >= 0) {
            array[i] -= yStep;
        } else {
            array[i] += yStep;
        }
    }
#endif

#if !PLATFORM_IQUE
    this->actionButtonAlpha[FS_BTN_ACTION_COPY] += 25;
    this->actionButtonAlpha[FS_BTN_ACTION_ERASE] += 25;
    this->optionButtonAlpha += 25;
    this->actionButtonAlpha[FS_BTN_ACTION_QUIT] -= 50;

    if (this->actionButtonAlpha[FS_BTN_ACTION_QUIT] <= 0) {
        this->actionButtonAlpha[FS_BTN_ACTION_QUIT] = 0;
    }
#else
    array = this->actionButtonAlpha;
    array[FS_BTN_ACTION_COPY] += 25;
    array[FS_BTN_ACTION_ERASE] += 25;
    this->optionButtonAlpha += 25;
    array[FS_BTN_ACTION_QUIT] -= 50;

    if (array[FS_BTN_ACTION_QUIT] <= 0) {
        array[FS_BTN_ACTION_QUIT] = 0;
    }
#endif

#if !PLATFORM_IQUE
    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
#else
    array = this->titleAlpha;
    array[0] -= 31;
    array[1] += 31;
#endif

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->highlightColor[3] = 70;
        this->highlightPulseDir = 1;
        XREG(35) = XREG(36);

#if !PLATFORM_IQUE
        this->actionButtonAlpha[FS_BTN_ACTION_COPY] = 200;
        this->actionButtonAlpha[FS_BTN_ACTION_QUIT] = 0;
#else
        array = this->actionButtonAlpha;
        array[FS_BTN_ACTION_COPY] = 200;
        array[FS_BTN_ACTION_QUIT] = 0;
#endif

        this->titleLabel = this->nextTitleLabel;

#if !PLATFORM_IQUE
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
#else
        array = this->titleAlpha;
        array[0] = 255;
        array[1] = 0;
#endif

        this->configMode = CM_MAIN_MENU;
    }

#if !PLATFORM_IQUE
    this->optionButtonAlpha = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
        this->actionButtonAlpha[FS_BTN_ACTION_COPY];
#else
    array = this->actionButtonAlpha;
    this->optionButtonAlpha = array[FS_BTN_ACTION_ERASE] = array[FS_BTN_ACTION_COPY];
#endif
}
