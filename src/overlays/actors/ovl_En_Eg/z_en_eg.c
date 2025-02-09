/*
 * File: z_en_eg.c
 * Overlay: ovl_En_Eg
 * Description: Triggers a void out (used in the tower collapse sequence?)
 */

#include "z_en_eg.h"
#include "terminal.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Eg_Actor_ct(Actor* thisx, PlayState* play);
void En_Eg_Actor_dt(Actor* thisx, PlayState* play);
void En_Eg_main(Actor* thisx, PlayState* play);
void En_Eg_draw(Actor* thisx, PlayState* play);

void En_Eg_main_Hahen_Wait(EnEg* this, PlayState* play);

static s32 set_wipe = false;

static EnEgActionFunc proc[] = {
    En_Eg_main_Hahen_Wait,
};

ActorProfile En_Eg_Profile = {
    /**/ ACTOR_EN_EG,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_ZL2,
    /**/ sizeof(EnEg),
    /**/ En_Eg_Actor_ct,
    /**/ En_Eg_Actor_dt,
    /**/ En_Eg_main,
    /**/ En_Eg_draw,
};

void En_Eg_Set_TimeoverSound(void) {
    Na_StartFixSe_F(NA_SE_OC_ABYSS);
}

void En_Eg_Actor_dt(Actor* thisx, PlayState* play) {
}

void En_Eg_Actor_ct(Actor* thisx, PlayState* play) {
    EnEg* this = (EnEg*)thisx;

    this->action = 0;
}

void En_Eg_main_Hahen_Wait(EnEg* this, PlayState* play) {
    if (!set_wipe && (z_common_data.subTimerSeconds <= 0) && Actor_Environment_sw_Check(play, 0x36) &&
        (!DEBUG_FEATURES || kREG(0) == 0)) {
        // Void the player out
        Game_play_down_restart_top(play);
        z_common_data.respawnFlag = -2;
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0);
        play->transitionType = TRANS_TYPE_FADE_BLACK;
        En_Eg_Set_TimeoverSound();
        set_wipe = true;
    }
}

void En_Eg_main(Actor* thisx, PlayState* play) {
    EnEg* this = (EnEg*)thisx;
    s32 action = this->action;

    if (((action < 0) || (0 < action)) || (proc[action] == NULL)) {
        // "Main Mode is wrong!!!!!!!!!!!!!!!!!!!!!!!!!"
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        proc[action](this, play);
    }
}

void En_Eg_draw(Actor* thisx, PlayState* play) {
}
