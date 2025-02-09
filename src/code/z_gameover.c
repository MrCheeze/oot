#include "z64game_over.h"

#include "global.h"
#include "versions.h"

void gameover_ct(PlayState* play) {
    play->gameOverCtx.state = GAMEOVER_INACTIVE;
}

void gameover_draw(PlayState* play) {
    GameOverContext* gameOverCtx = &play->gameOverCtx;

    if ((gameOverCtx->state >= GAMEOVER_DEATH_WAIT_GROUND && gameOverCtx->state < GAMEOVER_REVIVE_START) ||
        (gameOverCtx->state >= GAMEOVER_REVIVE_RUMBLE && gameOverCtx->state < GAMEOVER_REVIVE_FADE_OUT)) {
        Gameover_anten_proc(play);
    }
}

s16 go_ct = 0;

void gameover_move(PlayState* play) {
    GameOverContext* gameOverCtx = &play->gameOverCtx;
    s16 i;
    s16 j;
    s32 rumbleStrength;
    s32 rumbleDuration;
    s32 rumbleDecreaseRate;

    switch (gameOverCtx->state) {
        case GAMEOVER_DEATH_START:
            message_close(play);

            z_common_data.timerState = TIMER_STATE_OFF;
            z_common_data.subTimerState = SUBTIMER_STATE_OFF;
            CLEAR_EVENTINF(EVENTINF_MARATHON_ACTIVE);

            // search inventory for spoiling items and revert if necessary
            for (i = 0; i < ARRAY_COUNT(Warashibe_item); i++) {
                if (INV_CONTENT(ITEM_POCKET_EGG) == Warashibe_item[i]) {
                    INV_CONTENT(Warashibe_before_item[i]) = Warashibe_before_item[i];

                    // search c buttons for the found spoiling item and revert if necessary
                    for (j = 1; j < ARRAY_COUNT(z_common_data.save.info.equips.buttonItems); j++) {
                        if (z_common_data.save.info.equips.buttonItems[j] == Warashibe_item[i]) {
                            z_common_data.save.info.equips.buttonItems[j] = Warashibe_before_item[i];
                            item_textuer_dma(play, j);
                        }
                    }
                }
            }

            // restore "temporary B" to the B Button if not a sword item
            if (z_common_data.save.info.equips.buttonItems[0] != ITEM_SWORD_KOKIRI &&
                z_common_data.save.info.equips.buttonItems[0] != ITEM_SWORD_MASTER &&
                z_common_data.save.info.equips.buttonItems[0] != ITEM_SWORD_BIGGORON &&
                z_common_data.save.info.equips.buttonItems[0] != ITEM_GIANTS_KNIFE) {

                if (z_common_data.buttonStatus[0] != BTN_ENABLED) {
                    z_common_data.save.info.equips.buttonItems[0] = z_common_data.buttonStatus[0];
                } else {
                    z_common_data.save.info.equips.buttonItems[0] = ITEM_NONE;
                }
            }

#if OOT_VERSION < PAL_1_1
            z_common_data.nayrusLoveTimer = 0;
#else
            z_common_data.nayrusLoveTimer = 2000;
#endif

            z_common_data.save.info.playerData.naviTimer = 0;
            z_common_data.seqId = (u8)NA_BGM_DISABLED;
            z_common_data.natureAmbienceId = NATURE_ID_DISABLED;
            z_common_data.eventInf[0] = 0;
            z_common_data.eventInf[1] = 0;
            z_common_data.eventInf[2] = 0;
            z_common_data.eventInf[3] = 0;
            z_common_data.buttonStatus[0] = z_common_data.buttonStatus[1] = z_common_data.buttonStatus[2] =
                z_common_data.buttonStatus[3] = z_common_data.buttonStatus[4] = BTN_ENABLED;
            z_common_data.forceRisingButtonAlphas = z_common_data.nextHudVisibilityMode = z_common_data.hudVisibilityMode =
                z_common_data.hudVisibilityModeTimer = 0; // false, HUD_VISIBILITY_NO_CHANGE

            Gameover_anten_init(play);
            go_ct = 20;

            if (1) {}
            rumbleStrength = R_GAME_OVER_RUMBLE_STRENGTH;
            rumbleDuration = R_GAME_OVER_RUMBLE_DURATION;
            rumbleDecreaseRate = R_GAME_OVER_RUMBLE_DECREASE_RATE;

            z_vibctl2_vib_setQ(0.0f, ((rumbleStrength > 100) ? 255 : (rumbleStrength * 255) / 100),
                           (CLAMP_MAX(rumbleDuration * 3, 255)),
                           ((rumbleDecreaseRate > 100) ? 255 : (rumbleDecreaseRate * 255) / 100));

            gameOverCtx->state = GAMEOVER_DEATH_WAIT_GROUND;
            break;

        case GAMEOVER_DEATH_WAIT_GROUND:
            break;

        case GAMEOVER_DEATH_DELAY_MENU:
            go_ct--;

            if (go_ct == 0) {
                play->pauseCtx.state = PAUSE_STATE_GAME_OVER_START;
                gameOverCtx->state++;
                z_vibctl2_StageInit();
            }
            break;

        case GAMEOVER_REVIVE_START:
            gameOverCtx->state++;
            go_ct = 0;
            Gameover_anten_init(play);
            shrink_window_setval(32);
            return;

        case GAMEOVER_REVIVE_RUMBLE:
            go_ct = 50;
            gameOverCtx->state++;

            if (1) {}
            rumbleStrength = R_GAME_OVER_RUMBLE_STRENGTH;
            rumbleDuration = R_GAME_OVER_RUMBLE_DURATION;
            rumbleDecreaseRate = R_GAME_OVER_RUMBLE_DECREASE_RATE;

            z_vibctl2_vib_setQ(0.0f, ((rumbleStrength > 100) ? 255 : (rumbleStrength * 255) / 100),
                           (CLAMP_MAX(rumbleDuration * 3, 255)),
                           ((rumbleDecreaseRate > 100) ? 255 : (rumbleDecreaseRate * 255) / 100));
            break;

        case GAMEOVER_REVIVE_WAIT_GROUND:
            go_ct--;

            if (go_ct == 0) {
                go_ct = 64;
                gameOverCtx->state++;
            }
            break;

        case GAMEOVER_REVIVE_WAIT_FAIRY:
            go_ct--;

            if (go_ct == 0) {
                go_ct = 50;
                gameOverCtx->state++;
            }
            break;

        case GAMEOVER_REVIVE_FADE_OUT:
            Gameover_anten_return(play);
            go_ct--;

            if (go_ct == 0) {
                gameOverCtx->state = GAMEOVER_INACTIVE;
            }
            break;
    }
}
