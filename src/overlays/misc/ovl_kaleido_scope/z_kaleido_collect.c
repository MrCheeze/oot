#include "z_kaleido_scope.h"
#include "assets/textures/parameter_static/parameter_static.h"
#include "assets/textures/icon_item_static/icon_item_static.h"

#define SONG_MAX_LENGTH 8

#if !PLATFORM_GC || OOT_VERSION == HIRATSU3
#define QUEST_OCARINA_BTN_A_COLOR_R 80
#define QUEST_OCARINA_BTN_A_COLOR_G 150
#define QUEST_OCARINA_BTN_A_COLOR_B 255
#else
#define QUEST_OCARINA_BTN_A_COLOR_R 80
#define QUEST_OCARINA_BTN_A_COLOR_G 255
#define QUEST_OCARINA_BTN_A_COLOR_B 150
#endif

void collect_disp(PlayState* play, GraphicsContext* gfxCtx) {
    static s16 heart_color[][4] = {
        { 255, 0, 0, 255 },
        { 255, 70, 0, 150 },
        { 255, 70, 0, 150 },
        { 255, 0, 0, 255 },
    };

    // Shining medallions
    static s16 seal_env_col[6 + 6][3] = {
        // Target env color when msp == 0
        { 0, 0, 0 }, // QUEST_MEDALLION_FOREST
        { 0, 0, 0 }, // QUEST_MEDALLION_FIRE
        { 0, 0, 0 }, // QUEST_MEDALLION_WATER
        { 0, 0, 0 }, // QUEST_MEDALLION_SPIRIT
        { 0, 0, 0 }, // QUEST_MEDALLION_SHADOW
        { 0, 0, 0 }, // QUEST_MEDALLION_LIGHT

        // Target env color when msp == 2
        { 0, 60, 0 },   // QUEST_MEDALLION_FOREST
        { 90, 0, 0 },   // QUEST_MEDALLION_FIRE
        { 0, 40, 110 }, // QUEST_MEDALLION_WATER
        { 80, 40, 0 },  // QUEST_MEDALLION_SPIRIT
        { 70, 0, 90 },  // QUEST_MEDALLION_SHADOW
        { 90, 90, 0 },  // QUEST_MEDALLION_LIGHT
    };
    // Current (animated) env color for each medallion
    static s16 mpr[6] = { 255, 255, 255, 255, 255, 255 };
    static s16 mpg[6] = { 255, 255, 255, 255, 255, 255 };
    static s16 mpb[6] = { 150, 150, 150, 150, 150, 150 };
    static s16 msc = 20;
    static s16 msp = 0;

    static s16 mr = 0;
    static s16 mg = 0;
    static s16 mb = 0;
    static s16 ma = 0;
    static s16 mc = 20;
    static s16 mp = 0;

    static s16 onpu_pt = 0;
    static s16 counter = 0;
    static u8 onpu_buff[SONG_MAX_LENGTH + 1] = { 0 };
    static void* onpu[] = {
        gOcarinaBtnIconATex,      // OCARINA_BTN_A
        gOcarinaBtnIconCDownTex,  // OCARINA_BTN_C_DOWN
        gOcarinaBtnIconCRightTex, // OCARINA_BTN_C_RIGHT
        gOcarinaBtnIconCLeftTex,  // OCARINA_BTN_C_LEFT
        gOcarinaBtnIconCUpTex,    // OCARINA_BTN_C_UP
    };
    static u16 onpu_ypd[] = {
        0xFFCC, 0xFFCC, 0xFFCC, 0xFFCC, 0xFFCC,
    };
    static s16 onpu_alpha[SONG_MAX_LENGTH] = { 0 };
    static s32 sea03_pos = 0;

    static s16 melody_r[] = {
        150, // QUEST_SONG_MINUET
        255, // QUEST_SONG_BOLERO
        100, // QUEST_SONG_SERENADE
        255, // QUEST_SONG_REQUIEM
        255, // QUEST_SONG_NOCTURNE
        255, // QUEST_SONG_PRELUDE
        255, // QUEST_SONG_LULLABY
        255, // QUEST_SONG_EPONA
        255, // QUEST_SONG_SARIA
        255, // QUEST_SONG_SUN
        255, // QUEST_SONG_TIME
        255, // QUEST_SONG_STORMS
    };
    static s16 melody_g[] = {
        255, // QUEST_SONG_MINUET
        80,  // QUEST_SONG_BOLERO
        150, // QUEST_SONG_SERENADE
        160, // QUEST_SONG_REQUIEM
        100, // QUEST_SONG_NOCTURNE
        240, // QUEST_SONG_PRELUDE
        255, // QUEST_SONG_LULLABY
        255, // QUEST_SONG_EPONA
        255, // QUEST_SONG_SARIA
        255, // QUEST_SONG_SUN
        255, // QUEST_SONG_TIME
        255, // QUEST_SONG_STORMS
    };
    static s16 melody_b[] = {
        100, // QUEST_SONG_MINUET
        40,  // QUEST_SONG_BOLERO
        255, // QUEST_SONG_SERENADE
        0,   // QUEST_SONG_REQUIEM
        255, // QUEST_SONG_NOCTURNE
        100, // QUEST_SONG_PRELUDE
        255, // QUEST_SONG_LULLABY
        255, // QUEST_SONG_EPONA
        255, // QUEST_SONG_SARIA
        255, // QUEST_SONG_SUN
        255, // QUEST_SONG_TIME
        255, // QUEST_SONG_STORMS
    };

    enum {
        /* -3 */ CURSOR_TO_LEFT = -3, // Cursor on the "scroll to left page" position
        /* -2 */ CURSOR_TO_RIGHT,     // Cursor on the "scroll to right page" position
        /* -1 */ CURSOR_NONE          // No position in that direction, cursor stays where it is
    };
    // Each {up, down, left, right} entry defines where the cursor can move next
    static s8 curmv[][4] = {
        /* QUEST_MEDALLION_FOREST */
        { QUEST_MEDALLION_LIGHT, QUEST_MEDALLION_FIRE, QUEST_MEDALLION_LIGHT, CURSOR_TO_RIGHT },
        /* QUEST_MEDALLION_FIRE */
        { QUEST_MEDALLION_FOREST, QUEST_MEDALLION_WATER, QUEST_MEDALLION_WATER, CURSOR_TO_RIGHT },
        /* QUEST_MEDALLION_WATER */ { CURSOR_NONE, QUEST_GORON_RUBY, QUEST_MEDALLION_SPIRIT, QUEST_MEDALLION_FIRE },
        /* QUEST_MEDALLION_SPIRIT */
        { QUEST_MEDALLION_SHADOW, QUEST_MEDALLION_WATER, QUEST_SONG_STORMS, QUEST_MEDALLION_WATER },
        /* QUEST_MEDALLION_SHADOW */
        { QUEST_MEDALLION_LIGHT, QUEST_MEDALLION_SPIRIT, QUEST_HEART_PIECE, QUEST_MEDALLION_LIGHT },
        /* QUEST_MEDALLION_LIGHT */ { CURSOR_NONE, CURSOR_NONE, QUEST_MEDALLION_SHADOW, QUEST_MEDALLION_FOREST },
        /* QUEST_SONG_MINUET */ { QUEST_SONG_LULLABY, CURSOR_NONE, CURSOR_TO_LEFT, QUEST_SONG_BOLERO },
        /* QUEST_SONG_BOLERO */ { QUEST_SONG_EPONA, CURSOR_NONE, QUEST_SONG_MINUET, QUEST_SONG_SERENADE },
        /* QUEST_SONG_SERENADE */ { QUEST_SONG_SARIA, CURSOR_NONE, QUEST_SONG_BOLERO, QUEST_SONG_REQUIEM },
        /* QUEST_SONG_REQUIEM */ { QUEST_SONG_SUN, CURSOR_NONE, QUEST_SONG_SERENADE, QUEST_SONG_NOCTURNE },
        /* QUEST_SONG_NOCTURNE */ { QUEST_SONG_TIME, CURSOR_NONE, QUEST_SONG_REQUIEM, QUEST_SONG_PRELUDE },
        /* QUEST_SONG_PRELUDE */ { QUEST_SONG_STORMS, CURSOR_NONE, QUEST_SONG_NOCTURNE, QUEST_KOKIRI_EMERALD },
        /* QUEST_SONG_LULLABY */ { QUEST_SKULL_TOKEN, QUEST_SONG_MINUET, CURSOR_TO_LEFT, QUEST_SONG_EPONA },
        /* QUEST_SONG_EPONA */ { QUEST_SKULL_TOKEN, QUEST_SONG_BOLERO, QUEST_SONG_LULLABY, QUEST_SONG_SARIA },
        /* QUEST_SONG_SARIA */ { QUEST_SKULL_TOKEN, QUEST_SONG_SERENADE, QUEST_SONG_EPONA, QUEST_SONG_SUN },
        /* QUEST_SONG_SUN */ { QUEST_HEART_PIECE, QUEST_SONG_REQUIEM, QUEST_SONG_SARIA, QUEST_SONG_TIME },
        /* QUEST_SONG_TIME */ { QUEST_HEART_PIECE, QUEST_SONG_NOCTURNE, QUEST_SONG_SUN, QUEST_SONG_STORMS },
        /* QUEST_SONG_STORMS */ { QUEST_HEART_PIECE, QUEST_SONG_PRELUDE, QUEST_SONG_TIME, QUEST_MEDALLION_SPIRIT },
        /* QUEST_KOKIRI_EMERALD */ { QUEST_MEDALLION_WATER, CURSOR_NONE, QUEST_SONG_PRELUDE, QUEST_GORON_RUBY },
        /* QUEST_GORON_RUBY */ { QUEST_MEDALLION_WATER, CURSOR_NONE, QUEST_KOKIRI_EMERALD, QUEST_ZORA_SAPPHIRE },
        /* QUEST_ZORA_SAPPHIRE */ { QUEST_MEDALLION_WATER, CURSOR_NONE, QUEST_GORON_RUBY, CURSOR_TO_RIGHT },
        /* QUEST_STONE_OF_AGONY */ { CURSOR_NONE, QUEST_SKULL_TOKEN, CURSOR_TO_LEFT, QUEST_GERUDOS_CARD },
        /* QUEST_GERUDOS_CARD */ { CURSOR_NONE, QUEST_SKULL_TOKEN, QUEST_STONE_OF_AGONY, QUEST_HEART_PIECE },
        /* QUEST_SKULL_TOKEN */ { QUEST_STONE_OF_AGONY, QUEST_SONG_LULLABY, CURSOR_TO_LEFT, QUEST_HEART_PIECE },
        /* QUEST_HEART_PIECE */ { CURSOR_NONE, QUEST_SONG_TIME, QUEST_GERUDOS_CARD, QUEST_MEDALLION_SHADOW },
    };
    static s32 sUnused2 = 0;

    PauseContext* pauseCtx = &play->pauseCtx;
    Input* input = &play->state.input[0];
    s16 stepRed;
    s16 stepGreen;
    s16 stepBlue;
    s16 stepAlpha;
    s16 nextCursorPoint;
    s16 prevCursorPoint;
    s16 bufI;
    s16 j;
    s16 cursor;
    s16 i;
    s16 targetColorIndex;
    s16 pad2;
    s16 cursorItem;
    s16 gsTokenDigits[3];

    OPEN_DISPS(gfxCtx, "../z_kaleido_collect.c", 248);

    if ((((u32)pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) || (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT) ||
         (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG)) &&
        (pauseCtx->pageIndex == PAUSE_QUEST)) {

        pauseCtx->cursorColorSet = 0;

        if (pauseCtx->cursorSpecialPos == 0) {
            pauseCtx->nameColorSet = 0;

            if ((pauseCtx->state != PAUSE_STATE_MAIN) || ((pauseCtx->stickAdjX == 0) && (pauseCtx->stickAdjY == 0))) {
                // No cursor movement
                cursor = pauseCtx->cursorSlot[PAUSE_QUEST];
            } else {
                // Move cursor based on stick input

                prevCursorPoint = pauseCtx->cursorPoint[PAUSE_QUEST];

                if (pauseCtx->stickAdjX < -30) {
                    // Move cursor left
                    nextCursorPoint = curmv[prevCursorPoint][2];
                    if (nextCursorPoint == CURSOR_TO_LEFT) {
                        key_arrangement(play, PAUSE_CURSOR_PAGE_LEFT);
                        pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
                    } else {
                        while (nextCursorPoint >= 0) {
                            if ((s16)item_check(pauseCtx, nextCursorPoint)) {
                                break;
                            }
                            nextCursorPoint = curmv[nextCursorPoint][2];
                        }
                    }
                } else if (pauseCtx->stickAdjX > 30) {
                    // Move cursor right
                    nextCursorPoint = curmv[prevCursorPoint][3];
                    if (nextCursorPoint == CURSOR_TO_RIGHT) {
                        key_arrangement(play, PAUSE_CURSOR_PAGE_RIGHT);
                        pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
                    } else {
                        while (nextCursorPoint >= 0) {
                            if ((s16)item_check(pauseCtx, nextCursorPoint)) {
                                break;
                            }
                            nextCursorPoint = curmv[nextCursorPoint][3];
                        }
                    }
                }

                if (pauseCtx->stickAdjY < -30) {
                    // Move cursor down
                    nextCursorPoint = curmv[prevCursorPoint][1];
                    while (nextCursorPoint >= 0) {
                        if ((s16)item_check(pauseCtx, nextCursorPoint)) {
                            break;
                        }
                        nextCursorPoint = curmv[nextCursorPoint][1];
                    }
                } else if (pauseCtx->stickAdjY > 30) {
                    // Move cursor up
                    nextCursorPoint = curmv[prevCursorPoint][0];
                    while (nextCursorPoint >= 0) {
                        if ((s16)item_check(pauseCtx, nextCursorPoint)) {
                            break;
                        }
                        nextCursorPoint = curmv[nextCursorPoint][0];
                    }
                }

                // if the cursor point changed
                if (pauseCtx->cursorPoint[PAUSE_QUEST] != prevCursorPoint) {
                    pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
                    Nai_FxFlagEntry(NA_SE_SY_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                }

                // Update cursor item and slot

                if (pauseCtx->cursorPoint[PAUSE_QUEST] != QUEST_HEART_PIECE) {
                    if (CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST])) {
                        if (pauseCtx->cursorPoint[PAUSE_QUEST] < QUEST_SONG_MINUET) {
                            cursorItem =
                                ITEM_MEDALLION_FOREST - QUEST_MEDALLION_FOREST + pauseCtx->cursorPoint[PAUSE_QUEST];
                            PRINTF("000 ccc=%d\n", cursorItem);
                        } else if (pauseCtx->cursorPoint[PAUSE_QUEST] < QUEST_KOKIRI_EMERALD) {
                            cursorItem = ITEM_SONG_MINUET - QUEST_SONG_MINUET + pauseCtx->cursorPoint[PAUSE_QUEST];
                            PRINTF("111 ccc=%d\n", cursorItem);
                        } else {
                            cursorItem =
                                ITEM_KOKIRI_EMERALD - QUEST_KOKIRI_EMERALD + pauseCtx->cursorPoint[PAUSE_QUEST];
                            PRINTF("222 ccc=%d (%d, %d, %d)\n", cursorItem, pauseCtx->cursorPoint[PAUSE_QUEST],
                                   QUEST_KOKIRI_EMERALD, ITEM_KOKIRI_EMERALD);
                        }
                    } else {
                        cursorItem = PAUSE_ITEM_NONE;
                        PRINTF("999 ccc=%d (%d,  %d)\n", PAUSE_ITEM_NONE, pauseCtx->cursorPoint[PAUSE_QUEST], 0x18);
                    }
                } else {
                    if ((z_common_data.save.info.inventory.questItems & 0xF0000000) != 0) {
                        cursorItem = ITEM_HEART_CONTAINER;
                    } else {
                        cursorItem = PAUSE_ITEM_NONE;
                    }
                    PRINTF("888 ccc=%d (%d,  %d,  %x)\n", cursorItem, pauseCtx->cursorPoint[PAUSE_QUEST],
                           ITEM_HEART_CONTAINER, z_common_data.save.info.inventory.questItems & 0xF0000000);
                }

                cursor = pauseCtx->cursorPoint[PAUSE_QUEST];
                pauseCtx->cursorItem[pauseCtx->pageIndex] = cursorItem;
                pauseCtx->cursorSlot[pauseCtx->pageIndex] = cursor;
            }

            cursol_defalute(pauseCtx, cursor * 4, pauseCtx->questVtx);

            if ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) &&
                (pauseCtx->cursorSpecialPos == 0)) {
                if ((cursor >= QUEST_SONG_MINUET) && (cursor < QUEST_KOKIRI_EMERALD)) {
                    if (CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST])) {
                        cursor = pauseCtx->cursorSlot[PAUSE_QUEST];
                        pauseCtx->ocarinaSongIdx = ocarina_number[cursor - QUEST_SONG_MINUET];
                        counter = 10;

                        for (i = 0; i < SONG_MAX_LENGTH; i++) {
                            onpu_buff[i] = OCARINA_BTN_INVALID;
                            onpu_alpha[i] = 0;
                        }
                        onpu_pt = 0;

                        Na_SetOcarinaModeFlag(OCARINA_INSTRUMENT_DEFAULT);
                        Na_SetOcarinaPlayCheckFlag2((1 << pauseCtx->ocarinaSongIdx) + 0x8000);
                        pauseCtx->ocarinaStaff = Na_GetOcarinaSeqInfoWork();
                        pauseCtx->ocarinaStaff->pos = 0;
                        pauseCtx->ocarinaStaff->state = 0xFF;

                        R_PAUSE_SONG_OCA_BTN_Y(OCARINA_BTN_A) = -62;
                        R_PAUSE_SONG_OCA_BTN_Y(OCARINA_BTN_C_DOWN) = -56;
                        R_PAUSE_SONG_OCA_BTN_Y(OCARINA_BTN_C_RIGHT) = -49;
                        R_PAUSE_SONG_OCA_BTN_Y(OCARINA_BTN_C_LEFT) = -46;
                        R_PAUSE_SONG_OCA_BTN_Y(OCARINA_BTN_C_UP) = -41;

                        pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG;

                        Na_SetOcarinaModeFlag(OCARINA_INSTRUMENT_OFF);
                    }
                }
            } else if (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT) {
                // Abort having the player play the song if the stick is moved
                if ((pauseCtx->stickAdjX != 0) || (pauseCtx->stickAdjY != 0)) {
                    pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;

                    Na_SetOcarinaModeFlag(OCARINA_INSTRUMENT_OFF);
                }
            } else if (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG) {
                if (CHECK_BTN_ALL(input->press.button, BTN_A) && (cursor >= QUEST_SONG_MINUET) &&
                    (cursor < QUEST_KOKIRI_EMERALD)) {

                    pauseCtx->mainState = PAUSE_MAIN_STATE_SONG_PLAYBACK_START;

                    counter = 10;
                }
            }
        } else if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
            if (pauseCtx->stickAdjX > 30) {
                // Move cursor right from the "scroll to left page" position

                pauseCtx->cursorPoint[PAUSE_QUEST] = QUEST_STONE_OF_AGONY;
                pauseCtx->nameDisplayTimer = 0;
                pauseCtx->cursorSpecialPos = 0;

                cursor = pauseCtx->cursorPoint[PAUSE_QUEST];
                cursol_defalute(pauseCtx, cursor * 4, pauseCtx->questVtx);
                Nai_FxFlagEntry(NA_SE_SY_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);

                if (CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST])) {
                    cursorItem = ITEM_STONE_OF_AGONY - QUEST_STONE_OF_AGONY + pauseCtx->cursorPoint[PAUSE_QUEST];
                } else {
                    cursorItem = PAUSE_ITEM_NONE;
                }
                cursor = pauseCtx->cursorPoint[PAUSE_QUEST];
                pauseCtx->cursorItem[pauseCtx->pageIndex] = cursorItem;
                pauseCtx->cursorSlot[pauseCtx->pageIndex] = cursor;
            }
        } else { // cursorSpecialPos == PAUSE_CURSOR_PAGE_RIGHT
            if (pauseCtx->stickAdjX < -30) {
                // Move cursor left from the "scroll to right page" position

                pauseCtx->cursorPoint[PAUSE_QUEST] = QUEST_MEDALLION_FOREST;
                pauseCtx->nameDisplayTimer = 0;
                pauseCtx->cursorSpecialPos = 0;

                cursor = pauseCtx->cursorPoint[PAUSE_QUEST];
                cursol_defalute(pauseCtx, cursor * 4, pauseCtx->questVtx);
                Nai_FxFlagEntry(NA_SE_SY_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);

                if (CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST])) {
                    // Only the first if may be reached, as QUEST_MEDALLION_FOREST is set above
                    if (pauseCtx->cursorPoint[PAUSE_QUEST] < QUEST_SONG_MINUET) {
                        cursorItem =
                            ITEM_MEDALLION_FOREST - QUEST_MEDALLION_FOREST + pauseCtx->cursorPoint[PAUSE_QUEST];
                    } else if (pauseCtx->cursorPoint[PAUSE_QUEST] < QUEST_SONG_LULLABY) {
                        // this would result in the wrong item
                        cursorItem = ITEM_SCALE_GOLDEN - QUEST_SONG_MINUET + pauseCtx->cursorPoint[PAUSE_QUEST];
                    } else {
                        // this would result in the wrong item
                        cursorItem = ITEM_DUNGEON_COMPASS - QUEST_SONG_LULLABY + pauseCtx->cursorPoint[PAUSE_QUEST];
                    }
                } else {
                    cursorItem = PAUSE_ITEM_NONE;
                }
                cursor = pauseCtx->cursorPoint[PAUSE_QUEST];
                pauseCtx->cursorItem[pauseCtx->pageIndex] = cursorItem;
                pauseCtx->cursorSlot[pauseCtx->pageIndex] = cursor;
            }
        }

    } else if (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PLAYBACK_START) {
        // After a short delay, start playing the selected song back to the player

        pauseCtx->cursorColorSet = 8;

        if (--counter == 0) {
            for (i = 0; i < SONG_MAX_LENGTH; i++) {
                onpu_buff[i] = OCARINA_BTN_INVALID;
                onpu_alpha[i] = 0;
            }
            onpu_pt = 0;

            R_PAUSE_SONG_OCA_BTN_Y(OCARINA_BTN_A) = -62;
            R_PAUSE_SONG_OCA_BTN_Y(OCARINA_BTN_C_DOWN) = -56;
            R_PAUSE_SONG_OCA_BTN_Y(OCARINA_BTN_C_RIGHT) = -49;
            R_PAUSE_SONG_OCA_BTN_Y(OCARINA_BTN_C_LEFT) = -46;
            R_PAUSE_SONG_OCA_BTN_Y(OCARINA_BTN_C_UP) = -41;

            cursor = pauseCtx->cursorSlot[PAUSE_QUEST];
            Na_SetOcarinaModeFlag(OCARINA_INSTRUMENT_DEFAULT);
            Na_SetOcarinaModeFlag(OCARINA_INSTRUMENT_DEFAULT);
            pauseCtx->ocarinaSongIdx = ocarina_number[cursor - QUEST_SONG_MINUET];
            Na_SetOcarinaSeq(pauseCtx->ocarinaSongIdx + 1, 1);

            pauseCtx->mainState = PAUSE_MAIN_STATE_SONG_PLAYBACK;

            pauseCtx->ocarinaStaff = Na_GetOcarinaSeqInfoWork();
            pauseCtx->ocarinaStaff->pos = 0;

            cursor = pauseCtx->cursorSlot[PAUSE_QUEST];
            cursol_defalute(pauseCtx, cursor * 4, pauseCtx->questVtx);
        }
    } else {
        cursor = pauseCtx->cursorSlot[PAUSE_QUEST];
        cursol_defalute(pauseCtx, cursor * 4, pauseCtx->questVtx);
    }

    // Draw medallions
    // QUEST_MEDALLION_FOREST to QUEST_MEDALLION_LIGHT

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    msc--;

    for (j = 0, bufI = 0; j < QUEST_SONG_MINUET - QUEST_MEDALLION_FOREST; j++, bufI += 4) {
        if ((msp != 1) && (msp != 3)) {
            targetColorIndex = (msp != 0) ? j + 6 : j;

            if (msc != 0) {
                stepRed = ABS(mpr[j] - seal_env_col[targetColorIndex][0]) / msc;
                stepGreen =
                    ABS(mpg[j] - seal_env_col[targetColorIndex][1]) / msc;
                stepBlue = ABS(mpb[j] - seal_env_col[targetColorIndex][2]) / msc;
                if (mpr[j] >= seal_env_col[targetColorIndex][0]) {
                    mpr[j] -= stepRed;
                } else {
                    mpr[j] += stepRed;
                }
                if (mpg[j] >= seal_env_col[targetColorIndex][1]) {
                    mpg[j] -= stepGreen;
                } else {
                    mpg[j] += stepGreen;
                }
                if (mpb[j] >= seal_env_col[targetColorIndex][2]) {
                    mpb[j] -= stepBlue;
                } else {
                    mpb[j] += stepBlue;
                }
            } else {
                mpr[j] = seal_env_col[targetColorIndex][0];
                mpg[j] = seal_env_col[targetColorIndex][1];
                mpb[j] = seal_env_col[targetColorIndex][2];
            }
        }

        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST + j)) {
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
            gDPSetEnvColor(POLY_OPA_DISP++, mpr[j], mpg[j], mpb[j], 0);
            gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[bufI], 4, 0);

            kaleido_scope_item_set(gfxCtx, item_data[ITEM_MEDALLION_FOREST + j], QUEST_ICON_WIDTH,
                                               QUEST_ICON_HEIGHT, 0);
        }
    }

    if (msc == 0) {
        msc = R_PAUSE_QUEST_MEDALLION_SHINE_TIME(msp);
        if (++msp >= 4) {
            msp = 0;
        }
    }

    // Draw songs
    // QUEST_SONG_MINUET to QUEST_SONG_STORMS

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    gDPLoadTextureBlock(POLY_OPA_DISP++, gSongNoteTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    for (j = 0; j < QUEST_KOKIRI_EMERALD - QUEST_SONG_MINUET; j++, bufI += 4) {
        if (CHECK_QUEST_ITEM(QUEST_SONG_MINUET + j)) {
            if ((QUEST_SONG_MINUET + j) == cursor) {
                pauseCtx->questVtx[bufI + 0].v.ob[0] = pauseCtx->questVtx[bufI + 2].v.ob[0] =
                    pauseCtx->questVtx[bufI + 0].v.ob[0] - 2;

                pauseCtx->questVtx[bufI + 1].v.ob[0] = pauseCtx->questVtx[bufI + 3].v.ob[0] =
                    pauseCtx->questVtx[bufI + 1].v.ob[0] + 4;

                pauseCtx->questVtx[bufI + 0].v.ob[1] = pauseCtx->questVtx[bufI + 1].v.ob[1] =
                    pauseCtx->questVtx[bufI + 0].v.ob[1] + 2;

                pauseCtx->questVtx[bufI + 2].v.ob[1] = pauseCtx->questVtx[bufI + 3].v.ob[1] =
                    pauseCtx->questVtx[bufI + 2].v.ob[1] - 4;
            }

            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, melody_r[j], melody_g[j], melody_b[j],
                            pauseCtx->alpha);
            gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[bufI], 4, 0);
            gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
        }
    }

    // Draw spiritual stones
    // QUEST_KOKIRI_EMERALD, QUEST_GORON_RUBY, QUEST_ZORA_SAPPHIRE

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    for (j = 0; j < QUEST_STONE_OF_AGONY - QUEST_KOKIRI_EMERALD; j++, bufI += 4) {
        if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD + j)) {
            gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[bufI], 4, 0);
            kaleido_scope_item_set(gfxCtx, item_data[ITEM_KOKIRI_EMERALD + j], QUEST_ICON_WIDTH,
                                               QUEST_ICON_HEIGHT, 0);
        }
    }

    // Draw QUEST_STONE_OF_AGONY, QUEST_GERUDOS_CARD, QUEST_SKULL_TOKEN

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

    for (j = 0; j < QUEST_HEART_PIECE - QUEST_STONE_OF_AGONY; j++, bufI += 4) {
        if (CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY + j)) {
            gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[bufI], 4, 0);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
            kaleido_scope_item_set(gfxCtx, item_data[ITEM_STONE_OF_AGONY + j], QUEST_ICON_WIDTH,
                                               QUEST_ICON_HEIGHT, 0);
        }
    }

    // Draw heart pieces
    // QUEST_HEART_PIECE

    stepRed = ABS(mr - heart_color[mp][0]) / mc;
    stepGreen = ABS(mg - heart_color[mp][1]) / mc;
    stepBlue = ABS(mb - heart_color[mp][2]) / mc;
    stepAlpha = ABS(ma - heart_color[mp][3]) / mc;
    if (mr >= heart_color[mp][0]) {
        mr -= stepRed;
    } else {
        mr += stepRed;
    }
    if (mg >= heart_color[mp][1]) {
        mg -= stepGreen;
    } else {
        mg += stepGreen;
    }
    if (mb >= heart_color[mp][2]) {
        mb -= stepBlue;
    } else {
        mb += stepBlue;
    }
    if (ma >= heart_color[mp][3]) {
        ma -= stepAlpha;
    } else {
        ma += stepAlpha;
    }

    if (--mc == 0) {
        mr = heart_color[mp][0];
        mg = heart_color[mp][1];
        mb = heart_color[mp][2];
        ma = heart_color[mp][3];
        mc = ZREG(24 + mp);
        if (++mp >= ARRAY_COUNT(heart_color)) {
            mp = 0;
        }
    }

    if ((z_common_data.save.info.inventory.questItems >> QUEST_HEART_PIECE_COUNT) != 0) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

        if ((pauseCtx->state == PAUSE_STATE_OPENING_1) || (pauseCtx->state == PAUSE_STATE_CLOSING)) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, heart_color[0][0], heart_color[0][1], heart_color[0][2],
                            pauseCtx->alpha);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, mr, mg, mb, ma);
        }

        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[bufI], 4, 0);

        POLY_OPA_DISP = texture_QuadrangleIA8(
            POLY_OPA_DISP,
            item_data[ITEM_HEART_PIECE_2 - 1 +
                       (((z_common_data.save.info.inventory.questItems & 0xF0000000) & 0xF0000000) >>
                        QUEST_HEART_PIECE_COUNT)],
            48, 48, 0);
    }

    //

    if (pauseCtx->state == PAUSE_STATE_MAIN) {
        bufI += (QUEST_QUAD_SONG_NOTE_A1 - QUEST_HEART_PIECE) * 4;

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

        // Update cursor color
        if ((pauseCtx->cursorSpecialPos == 0) && (cursor >= QUEST_SONG_MINUET) && (cursor < QUEST_KOKIRI_EMERALD)) {
            if ((pauseCtx->mainState < PAUSE_MAIN_STATE_3) || (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT) ||
                (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG)) {
                if (pauseCtx->cursorItem[pauseCtx->pageIndex] != PAUSE_ITEM_NONE) {
                    pauseCtx->cursorColorSet = 8;
                    if ((pauseCtx->mainState >= PAUSE_MAIN_STATE_SONG_PLAYBACK) &&
                        (pauseCtx->mainState <= PAUSE_MAIN_STATE_SONG_PROMPT_DONE)) {
                        pauseCtx->cursorColorSet = 0;
                    }
                }
            }
        }

        if (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PLAYBACK) {
            // Draw ocarina buttons as the song playback progresses
            // QUEST_QUAD_SONG_NOTE_A1 to QUEST_QUAD_SONG_NOTE_A8

            pauseCtx->ocarinaStaff = Na_GetOcarinaSeqInfoWork();

            if (pauseCtx->ocarinaStaff->pos != 0) {
                if (onpu_pt == (pauseCtx->ocarinaStaff->pos - 1)) {
                    onpu_pt++;
                    onpu_buff[pauseCtx->ocarinaStaff->pos - 1] = pauseCtx->ocarinaStaff->buttonIndex;
                }

                for (j = 0, i = 0; j < SONG_MAX_LENGTH; j++, i += 4, bufI += 4) {
                    if (onpu_buff[j] == OCARINA_BTN_INVALID) {
                        break;
                    }

                    if (onpu_alpha[j] != 255) {
                        onpu_alpha[j] += R_OCARINA_BUTTONS_APPEAR_ALPHA_STEP;
                        if (onpu_alpha[j] >= 255) {
                            onpu_alpha[j] = 255;
                        }
                    }

                    pauseCtx->questVtx[bufI + 0].v.ob[1] = pauseCtx->questVtx[bufI + 1].v.ob[1] =
                        R_PAUSE_SONG_OCA_BTN_Y(onpu_buff[j]);

                    pauseCtx->questVtx[bufI + 2].v.ob[1] = pauseCtx->questVtx[bufI + 3].v.ob[1] =
                        pauseCtx->questVtx[bufI + 0].v.ob[1] - 12;

                    gDPPipeSync(POLY_OPA_DISP++);

                    if (onpu_buff[j] == OCARINA_BTN_A) {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, QUEST_OCARINA_BTN_A_COLOR_R, QUEST_OCARINA_BTN_A_COLOR_G,
                                        QUEST_OCARINA_BTN_A_COLOR_B, onpu_alpha[j]);
                    } else {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 50, onpu_alpha[j]);
                    }

                    gDPSetEnvColor(POLY_OPA_DISP++, 10, 10, 10, 0);
                    gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[bufI], 4, 0);

                    gDPLoadTextureBlock(POLY_OPA_DISP++, onpu[onpu_buff[j]], G_IM_FMT_IA,
                                        G_IM_SIZ_8b, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                        G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
                }
            }
        } else if (((pauseCtx->mainState >= PAUSE_MAIN_STATE_SONG_PROMPT_INIT) &&
                    (pauseCtx->mainState <= PAUSE_MAIN_STATE_SONG_PROMPT_DONE)) ||
                   (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG)) {
            // Draw the buttons for playing a song
            // QUEST_QUAD_SONG_NOTE_A1 to QUEST_QUAD_SONG_NOTE_A8

            // temps reused, fake?
            stepGreen = pauseCtx->ocarinaSongIdx;
            stepRed = na_oca_note[stepGreen].numButtons;

            j = bufI;

            for (i = 0; i < stepRed; i++, bufI += 4) {
                pauseCtx->questVtx[bufI + 0].v.ob[1] = pauseCtx->questVtx[bufI + 1].v.ob[1] =
                    R_PAUSE_SONG_OCA_BTN_Y(na_oca_note[stepGreen].buttonsIndex[i]);

                pauseCtx->questVtx[bufI + 2].v.ob[1] = pauseCtx->questVtx[bufI + 3].v.ob[1] =
                    pauseCtx->questVtx[bufI + 0].v.ob[1] - 12;

                gDPPipeSync(POLY_OPA_DISP++);

                if (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG) {
                    if (na_oca_note[stepGreen].buttonsIndex[i] == OCARINA_BTN_A) {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, QUEST_OCARINA_BTN_A_COLOR_R, QUEST_OCARINA_BTN_A_COLOR_G,
                                        QUEST_OCARINA_BTN_A_COLOR_B, 200);
                    } else {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 50, 200);
                    }
                } else {
                    // Gray out buttons during the player playing the song
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 150, 150, 150, 150);
                }

                gDPSetEnvColor(POLY_OPA_DISP++, 10, 10, 10, 0);

                gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[bufI], 4, 0);

                gDPLoadTextureBlock(POLY_OPA_DISP++,
                                    onpu[na_oca_note[stepGreen].buttonsIndex[i]], G_IM_FMT_IA,
                                    G_IM_SIZ_8b, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                    G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
            }

            if (pauseCtx->mainState != PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG) {
                // Draw the buttons colored as the player plays the song
                // QUEST_QUAD_SONG_NOTE_B1 to QUEST_QUAD_SONG_NOTE_B8

                pauseCtx->ocarinaStaff = Na_GetOcarinaInfoWork();

                if (pauseCtx->ocarinaStaff->pos != 0) {
                    if (onpu_pt == (pauseCtx->ocarinaStaff->pos - 1)) {
                        if ((pauseCtx->ocarinaStaff->buttonIndex >= OCARINA_BTN_A) &&
                            (pauseCtx->ocarinaStaff->buttonIndex <= OCARINA_BTN_C_UP)) {
                            onpu_buff[pauseCtx->ocarinaStaff->pos - 1] = pauseCtx->ocarinaStaff->buttonIndex;
                            onpu_buff[pauseCtx->ocarinaStaff->pos] = OCARINA_BTN_INVALID;
                            onpu_pt++;
                        }
                    }
                }

                bufI = j + ((QUEST_QUAD_SONG_NOTE_B1 - QUEST_QUAD_SONG_NOTE_A1) * 4);

                for (i = 0; i < SONG_MAX_LENGTH; i++, bufI += 4) {
                    if (onpu_buff[i] == OCARINA_BTN_INVALID) {
                        continue;
                    }

                    if (onpu_alpha[i] != 255) {
                        onpu_alpha[i] += R_OCARINA_BUTTONS_APPEAR_ALPHA_STEP;
                        if (onpu_alpha[i] >= 255) {
                            onpu_alpha[i] = 255;
                        }
                    }

                    pauseCtx->questVtx[bufI + 0].v.ob[1] = pauseCtx->questVtx[bufI + 1].v.ob[1] =
                        R_PAUSE_SONG_OCA_BTN_Y(onpu_buff[i]);

                    pauseCtx->questVtx[bufI + 2].v.ob[1] = pauseCtx->questVtx[bufI + 3].v.ob[1] =
                        pauseCtx->questVtx[bufI + 0].v.ob[1] - 12;

                    gDPPipeSync(POLY_OPA_DISP++);

                    if (onpu_buff[i] == OCARINA_BTN_A) {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, QUEST_OCARINA_BTN_A_COLOR_R, QUEST_OCARINA_BTN_A_COLOR_G,
                                        QUEST_OCARINA_BTN_A_COLOR_B, onpu_alpha[i]);
                    } else {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 50, onpu_alpha[i]);
                    }

                    gDPSetEnvColor(POLY_OPA_DISP++, 10, 10, 10, 0);

                    gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[bufI], 4, 0);

                    gDPLoadTextureBlock(POLY_OPA_DISP++, onpu[onpu_buff[i]], G_IM_FMT_IA,
                                        G_IM_SIZ_8b, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                        G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
                }

                if (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT_INIT) {
                    for (i = 0; i < SONG_MAX_LENGTH; i++) {
                        onpu_buff[i] = OCARINA_BTN_INVALID;
                        onpu_alpha[i] = 0;
                    }
                    onpu_pt = 0;

                    Na_SetOcarinaModeFlag(OCARINA_INSTRUMENT_DEFAULT);
                    Na_SetOcarinaPlayCheckFlag2((1 << pauseCtx->ocarinaSongIdx) + 0x8000);
                    pauseCtx->ocarinaStaff = Na_GetOcarinaSeqInfoWork();
                    pauseCtx->ocarinaStaff->pos = 0;
                    pauseCtx->ocarinaStaff->state = 0xFE;

                    pauseCtx->mainState = PAUSE_MAIN_STATE_SONG_PROMPT;
                }
            }
        }
    }

    // Draw amount of gold skulltula tokens
    // QUEST_QUAD_SKULL_TOKENS_DIGIT1_SHADOW to QUEST_QUAD_SKULL_TOKENS_DIGIT3

    if (CHECK_QUEST_ITEM(QUEST_SKULL_TOKEN)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        gsTokenDigits[0] = gsTokenDigits[1] = 0;
        gsTokenDigits[2] = z_common_data.save.info.inventory.gsTokens;

        while (gsTokenDigits[2] >= 100) {
            gsTokenDigits[0]++;
            gsTokenDigits[2] -= 100;
        }

        while (gsTokenDigits[2] >= 10) {
            gsTokenDigits[1]++;
            gsTokenDigits[2] -= 10;
        }

        gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[QUEST_QUAD_SKULL_TOKENS_DIGIT1_SHADOW * 4], 6 * 4, 0);

        for (i = 0, j = 0; i < 2; i++) {
            if (i == 0) {
                // Text shadow
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, pauseCtx->alpha);
            } else {
                // Text color
                if (z_common_data.save.info.inventory.gsTokens == 100) {
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 200, 50, 50, pauseCtx->alpha);
                } else {
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
                }
            }

            // Variable reused as a flag indicating all digits onwards should be displayed
            cursorItem = false;

            for (bufI = 0; bufI < 3; bufI++, j += 4) {
                if ((bufI >= 2) || (gsTokenDigits[bufI] != 0) || cursorItem) {
                    gDPLoadTextureBlock(POLY_OPA_DISP++, ((u8*)gCounterDigit0Tex + (8 * 16 * gsTokenDigits[bufI])),
                                        G_IM_FMT_I, G_IM_SIZ_8b, 8, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                    gSP1Quadrangle(POLY_OPA_DISP++, j, j + 2, j + 3, j + 1, 0);

                    cursorItem = true;
                }
            }
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_kaleido_collect.c", 863);
}

s32 item_check(PauseContext* pauseCtx, s32 point) {
    pauseCtx->cursorPoint[PAUSE_QUEST] = point;

    return true;
}
