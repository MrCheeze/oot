/*
 * File: z_en_okarina_effect.c
 * Overlay: ovl_En_Okarina_Effect
 * Description: Manages the storm created when playing Song of Storms
 */

#include "z_en_okarina_effect.h"
#include "terminal.h"
#include "versions.h"

#include "z64frame_advance.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void En_Okarina_Effect_actor_ct(Actor* thisx, PlayState* play);
void En_Okarina_Effect_actor_dt(Actor* thisx, PlayState* play);
void En_Okarina_Effect_actor_move(Actor* thisx, PlayState* play);

void mode_arasi(EnOkarinaEffect* this, PlayState* play);
void mode_arasi2(EnOkarinaEffect* this, PlayState* play);

ActorProfile En_Okarina_Effect_Profile = {
    /**/ ACTOR_EN_OKARINA_EFFECT,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnOkarinaEffect),
    /**/ En_Okarina_Effect_actor_ct,
    /**/ En_Okarina_Effect_actor_dt,
    /**/ En_Okarina_Effect_actor_move,
    /**/ NULL,
};

void En_Okarina_Effect_actor_set_process(EnOkarinaEffect* this, EnOkarinaEffectActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Okarina_Effect_actor_dt(Actor* thisx, PlayState* play) {
    EnOkarinaEffect* this = (EnOkarinaEffect*)thisx;

    play->envCtx.precipitation[PRECIP_SOS_MAX] = 0;
#if OOT_VERSION < PAL_1_0
    if ((E_wether_flg == WEATHER_MODE_CLEAR) && (play->envCtx.stormRequest == STORM_REQUEST_START))
#else
    if ((E_wether_flg != WEATHER_MODE_RAIN) && (E_wether_flg != WEATHER_MODE_HEAVY_RAIN) &&
        (play->envCtx.stormRequest == STORM_REQUEST_START))
#endif
    {
        play->envCtx.stormRequest = STORM_REQUEST_STOP;
        Rain_sound_set_off(play);
    }
    play->envCtx.lightningState = LIGHTNING_LAST;
}

void En_Okarina_Effect_actor_ct(Actor* thisx, PlayState* play) {
    EnOkarinaEffect* this = (EnOkarinaEffect*)thisx;

    PRINTF("\n\n");
    // "Ocarina Storm Effect"
    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ オカリナあらし効果ビカビカビカ〜 ☆☆☆☆☆ \n" VT_RST);
    PRINTF("\n\n");
    if (play->envCtx.precipitation[PRECIP_RAIN_CUR] != 0) {
        Actor_delete(&this->actor);
    }
    En_Okarina_Effect_actor_set_process(this, mode_arasi);
}

void mode_arasi(EnOkarinaEffect* this, PlayState* play) {
    this->timer = 400; // 20 seconds
    play->envCtx.precipitation[PRECIP_SOS_MAX] = 20;
    play->envCtx.stormRequest = STORM_REQUEST_START;
    if ((E_wether_flg != WEATHER_MODE_CLEAR) || play->envCtx.skyboxConfig != 0) {
        play->envCtx.stormState = STORM_STATE_ON;
    }
    play->envCtx.lightningState = LIGHTNING_ON;
    Rain_sound_set_on(play);
    En_Okarina_Effect_actor_set_process(this, mode_arasi2);
}

void mode_arasi2(EnOkarinaEffect* this, PlayState* play) {
    eventbit_reset(play, 5);

    if (((play->pauseCtx.state == PAUSE_STATE_OFF) && (play->gameOverCtx.state == GAMEOVER_INACTIVE) &&
         (play->msgCtx.msgLength == 0) && (!_Game_play_isPause(play)) &&
         ((play->transitionMode == TRANS_MODE_OFF) || (z_common_data.gameMode != GAMEMODE_NORMAL))) ||
        (this->timer >= 250)) {
        if ((play->envCtx.lightMode != LIGHT_MODE_TIME) || play->envCtx.lightConfig != 1) {
            this->timer--;
        }
        PRINTF("\nthis->timer=[%d]", this->timer);
        if (this->timer == 308) {
            PRINTF("\n\n\n豆よ のびろ 指定\n\n\n"); // "Let's grow some beans"
            eventbit_set(play, 5);
        }
    }

    if (E_wether_apl) {
        this->timer = 0;
    }

    if (this->timer == 0) {
        play->envCtx.precipitation[PRECIP_SOS_MAX] = 0;
        if (play->csCtx.state == CS_STATE_IDLE) {
            Rain_sound_set_off(play);
        } else if (Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN) == NA_BGM_NATURE_AMBIENCE) {
            Na_ControlNaturalSound(NATURE_CHANNEL_LIGHTNING, CHANNEL_IO_PORT_1, 0);
            Na_ControlNaturalSound(NATURE_CHANNEL_RAIN, CHANNEL_IO_PORT_1, 0);
        }
        PRINTF("\n\n\nE_wether_flg=[%d]", E_wether_flg);
        PRINTF("\nrain_evt_trg=[%d]\n\n", play->envCtx.stormRequest);
        if (E_wether_flg == WEATHER_MODE_CLEAR && (play->envCtx.stormRequest == STORM_REQUEST_START)) {
            play->envCtx.stormRequest = STORM_REQUEST_STOP;
        } else {
            play->envCtx.stormRequest = STORM_REQUEST_NONE;
            play->envCtx.stormState = STORM_STATE_OFF;
        }
        play->envCtx.lightningState = LIGHTNING_LAST;
        Actor_delete(&this->actor);
    }
}

void En_Okarina_Effect_actor_move(Actor* thisx, PlayState* play) {
    EnOkarinaEffect* this = (EnOkarinaEffect*)thisx;

    this->actionFunc(this, play);

    if (DEBUG_FEATURES && BREG(0) != 0) {
        Debug_Display_new(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 0xFF, 0, 0xFF, 0xFF, 4, play->state.gfxCtx);
    }
}
