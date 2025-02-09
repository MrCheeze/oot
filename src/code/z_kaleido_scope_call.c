#pragma increment_block_number "gc-eu:192 gc-eu-mq:192 gc-jp:192 gc-jp-ce:192 gc-jp-mq:192 gc-us:192 gc-us-mq:192" \
                               "ique-cn:192 ntsc-1.0:224 ntsc-1.1:224 ntsc-1.2:224 pal-1.0:192 pal-1.1:192 hiratsu3:192"

#include "global.h"
#include "terminal.h"

void (*kaleido_scope_move_func)(PlayState* play);
void (*kaleido_scope_draw_func)(PlayState* play);
f32 LargeMapMark_MarkScale;
u32 LargeMapMark_NeedDraw;
PauseMapMarksData* LargeMapMark_lmarkpos_p;

extern void kaleido_scope_move(PlayState* play);
extern void kaleido_scope_draw(PlayState* play);

void load_player(void) {
    KaleidoMgrOverlay* playerActorOvl = &KaleidoArea_dlftbl[KALEIDO_OVL_PLAYER_ACTOR];

    if (KaleidoArea_visit != playerActorOvl) {
        if (KaleidoArea_visit != NULL) {
            PRINTF_COLOR_GREEN();
            PRINTF(T("カレイド領域 強制排除\n", "Kaleido area forced exclusion\n"));
            PRINTF_RST();

            KaleidoArea_DoUnlink(KaleidoArea_visit);
        }

        PRINTF_COLOR_GREEN();
        PRINTF(T("プレイヤーアクター搬入\n", "Player actor import\n"));
        PRINTF_RST();

        KaleidoArea_DoLink(playerActorOvl);
    }
}

void kaleido_scope_ct_ovl(PlayState* play) {
    PRINTF(T("カレイド・スコープ入れ替え コンストラクト \n", "Kaleidoscope replacement construction\n"));

    kaleido_scope_move_func = KaleidoArea_dllcnv(kaleido_scope_move);
    kaleido_scope_draw_func = KaleidoArea_dllcnv(kaleido_scope_draw);

    LOG_ADDRESS("kaleido_scope_move", kaleido_scope_move, "../z_kaleido_scope_call.c", 98);
    LOG_ADDRESS("kaleido_scope_move_func", kaleido_scope_move_func, "../z_kaleido_scope_call.c", 99);
    LOG_ADDRESS("kaleido_scope_draw", kaleido_scope_draw, "../z_kaleido_scope_call.c", 100);
    LOG_ADDRESS("kaleido_scope_draw_func", kaleido_scope_draw_func, "../z_kaleido_scope_call.c", 101);

    kaleido_scope_ct(play);
}

void kaleido_scope_dt_ovl(PlayState* play) {
    PRINTF(T("カレイド・スコープ入れ替え デストラクト \n", "Kaleidoscope replacement destruction\n"));

    kaleido_scope_dt(play);
}

void kaleido_scope_move_ovl(PlayState* play) {
    KaleidoMgrOverlay* kaleidoScopeOvl = &KaleidoArea_dlftbl[KALEIDO_OVL_KALEIDO_SCOPE];
    PauseContext* pauseCtx = &play->pauseCtx;

    if (IS_PAUSED(&play->pauseCtx)) {
        if (pauseCtx->state == PAUSE_STATE_WAIT_LETTERBOX) {
            if (shrink_window_getnowval() == 0) {
#if DEBUG_FEATURES
                R_HREG_MODE = HREG_MODE_UCODE_DISAS;
                R_UCODE_DISAS_LOG_MODE = 3;
#endif

                R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_SETUP;
                pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
                pauseCtx->savePromptState = PAUSE_SAVE_PROMPT_STATE_APPEARING;
                pauseCtx->state = (pauseCtx->state & 0xFFFF) + 1; // PAUSE_STATE_WAIT_BG_PRERENDER
            }
        } else if (pauseCtx->state == PAUSE_STATE_GAME_OVER_START) {
#if DEBUG_FEATURES
            R_HREG_MODE = HREG_MODE_UCODE_DISAS;
            R_UCODE_DISAS_LOG_MODE = 3;
#endif

            R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_SETUP;
            pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
            pauseCtx->savePromptState = PAUSE_SAVE_PROMPT_STATE_APPEARING; // copied from pause menu, not needed here
            pauseCtx->state = (pauseCtx->state & 0xFFFF) + 1;              // PAUSE_STATE_GAME_OVER_WAIT_BG_PRERENDER
        } else if ((pauseCtx->state == PAUSE_STATE_WAIT_BG_PRERENDER) ||
                   (pauseCtx->state == PAUSE_STATE_GAME_OVER_WAIT_BG_PRERENDER)) {
            PRINTF("PR_KAREIDOSCOPE_MODE=%d\n", R_PAUSE_BG_PRERENDER_STATE);

            if (R_PAUSE_BG_PRERENDER_STATE >= PAUSE_BG_PRERENDER_READY) {
                pauseCtx->state++; // PAUSE_STATE_INIT or PAUSE_STATE_GAME_OVER_INIT
            }
        } else if (pauseCtx->state != PAUSE_STATE_OFF) {
            if (KaleidoArea_visit != kaleidoScopeOvl) {
                if (KaleidoArea_visit != NULL) {
                    PRINTF_COLOR_GREEN();
                    PRINTF(T("カレイド領域 プレイヤー 強制排除\n", "Kaleido area Player Forced Elimination\n"));
                    PRINTF_RST();

                    KaleidoArea_DoUnlink(KaleidoArea_visit);
                }

                PRINTF_COLOR_GREEN();
                PRINTF(T("カレイド領域 カレイドスコープ搬入\n", "Kaleido area Kaleidoscope loading\n"));
                PRINTF_RST();

                KaleidoArea_DoLink(kaleidoScopeOvl);
            }

            if (KaleidoArea_visit == kaleidoScopeOvl) {
                kaleido_scope_move_func(play);

                if (!IS_PAUSED(&play->pauseCtx)) {
                    PRINTF_COLOR_GREEN();
                    PRINTF(T("カレイド領域 カレイドスコープ排出\n", "Kaleido area Kaleidoscope Emission\n"));
                    PRINTF_RST();

                    KaleidoArea_DoUnlink(kaleidoScopeOvl);
                    load_player();
                }
            }
        }
    }
}

void kaleido_scope_draw_ovl(PlayState* play) {
    KaleidoMgrOverlay* kaleidoScopeOvl = &KaleidoArea_dlftbl[KALEIDO_OVL_KALEIDO_SCOPE];

    if (R_PAUSE_BG_PRERENDER_STATE >= PAUSE_BG_PRERENDER_READY) {
        if (((play->pauseCtx.state >= PAUSE_STATE_OPENING_1) && (play->pauseCtx.state <= PAUSE_STATE_SAVE_PROMPT)) ||
            ((play->pauseCtx.state >= PAUSE_STATE_GAME_OVER_SHOW_MESSAGE) &&
             (play->pauseCtx.state <= PAUSE_STATE_CLOSING))) {
            if (KaleidoArea_visit == kaleidoScopeOvl) {
                kaleido_scope_draw_func(play);
            }
        }
    }
}
