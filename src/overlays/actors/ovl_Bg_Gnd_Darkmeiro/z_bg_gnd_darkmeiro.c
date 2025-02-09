/*
 * File: z_bg_gnd_darkmeiro.c
 * Overlay: ovl_Bg_Gnd_Darkmeiro
 * Description: Shadow trial actors (invisible path, clear block, and timer)
 */

#include "z_bg_gnd_darkmeiro.h"

#include "gfx.h"
#include "sfx.h"
#include "z_lib.h"
#include "z64play.h"

#include "assets/objects/object_demo_kekkai/object_demo_kekkai.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Gnd_Darkmeiro_Actor_ct(Actor* thisx, PlayState* play2);
void Bg_Gnd_Darkmeiro_Actor_dt(Actor* thisx, PlayState* play2);
void Bg_Gnd_Darkmeiro_Actor_move(Actor* thisx, PlayState* play2);
void Bg_Gnd_Darkmeiro_Actor_draw0(Actor* thisx, PlayState* play);
void Bg_Gnd_Darkmeiro_Actor_draw1(Actor* thisx, PlayState* play);
void Bg_Gnd_Darkmeiro_Actor_draw2(Actor* thisx, PlayState* play);

static void move_wait(BgGndDarkmeiro* this, PlayState* play);
void move_leader(BgGndDarkmeiro* this, PlayState* play);
void move_static(BgGndDarkmeiro* this, PlayState* play);
void move_switch(BgGndDarkmeiro* this, PlayState* play);

ActorProfile Bg_Gnd_Darkmeiro_Profile = {
    /**/ ACTOR_BG_GND_DARKMEIRO,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_DEMO_KEKKAI,
    /**/ sizeof(BgGndDarkmeiro),
    /**/ Bg_Gnd_Darkmeiro_Actor_ct,
    /**/ Bg_Gnd_Darkmeiro_Actor_dt,
    /**/ Bg_Gnd_Darkmeiro_Actor_move,
    /**/ NULL,
};

void check_movebg_switch(BgGndDarkmeiro* this, PlayState* play) {
    if (this->actionFlags & 2) {
        if (this->timer1 == 0) {
            DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
            this->actionFlags &= ~2;
        }
    } else if (this->timer1 != 0) {
        DynaPolygonInfo_clearThrough(play, &play->colCtx.dyna, this->dyna.bgId);
        this->actionFlags |= 2;
    }
}

void Bg_Gnd_Darkmeiro_Actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    CollisionHeader* colHeader = NULL;
    BgGndDarkmeiro* this = (BgGndDarkmeiro*)thisx;

    this->updateFunc = move_wait;
    Actor_set_scale(&this->dyna.actor, 0.1f);
    switch (PARAMS_GET_U(this->dyna.actor.params, 0, 8)) {
        case DARKMEIRO_INVISIBLE_PATH:
            this->dyna.actor.draw = Bg_Gnd_Darkmeiro_Actor_draw0;
            this->dyna.actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
            break;
        case DARKMEIRO_CLEAR_BLOCK:
            DynaPolyUty_bgdi_SG2KSG(&gClearBlockCol, &colHeader);
            this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
            if (PARAMS_GET_U(this->dyna.actor.params, 8, 6) == 0x3F) {
                this->updateFunc = move_static;
                this->dyna.actor.draw = Bg_Gnd_Darkmeiro_Actor_draw2;
            } else {
                this->actionFlags = this->timer1 = this->timer2 = 0;
                thisx->draw = Bg_Gnd_Darkmeiro_Actor_draw1;
                this->updateFunc = move_switch;
                if (!Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6))) {
                    DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
                } else {
                    this->timer1 = 64;
                    this->actionFlags |= 2;
                }
            }
            break;
        case DARKMEIRO_BLOCK_TIMER:
            this->actionFlags = this->timer1 = this->timer2 = 0;
            this->updateFunc = move_leader;
            thisx->draw = NULL;
            if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6) + 1)) {
                this->timer1 = 64;
                this->actionFlags |= 4;
            }
            if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6) + 2)) {
                this->timer2 = 64;
                this->actionFlags |= 8;
            }
            if ((this->timer1 != 0) || (this->timer2 != 0)) {
                Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6));
            } else {
                Actor_Environment_sw_Off(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6));
            }
            break;
    }
}

void Bg_Gnd_Darkmeiro_Actor_dt(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgGndDarkmeiro* this = (BgGndDarkmeiro*)thisx;

    if (PARAMS_GET_U(this->dyna.actor.params, 0, 8) != 1) {
        return;
    }

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void move_wait(BgGndDarkmeiro* this, PlayState* play) {
}

void move_leader(BgGndDarkmeiro* this, PlayState* play) {
    s16 timeLeft;

    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6) + 1)) {
        if (this->actionFlags & 4) {
            if (this->timer1 > 0) {
                this->timer1--;
            } else {
                Actor_Environment_sw_Off(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6) + 1);
                this->actionFlags &= ~4;
            }
        } else {
            this->actionFlags |= 4;
            this->timer1 = 304;
            Nai_FxFlagEntry(NA_SE_EV_RED_EYE, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    }

    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6) + 2)) {
        if (this->actionFlags & 8) {
            if (this->timer2 > 0) {
                this->timer2--;
            } else {
                Actor_Environment_sw_Off(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6) + 2);
                this->actionFlags &= ~8;
            }
        } else {
            this->actionFlags |= 8;
            this->timer2 = 304;
            Nai_FxFlagEntry(NA_SE_EV_RED_EYE, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    }

    timeLeft = CLAMP_MIN(this->timer1, this->timer2);
    if (timeLeft > 0) {
        Actor_timer_level_SE_set(&this->dyna.actor, timeLeft);
    }
    if ((this->timer1 >= 64) || (this->timer2 >= 64)) {
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6));
    } else {
        Actor_Environment_sw_Off(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6));
    }
}

void move_static(BgGndDarkmeiro* this, PlayState* play) {
}

void move_switch(BgGndDarkmeiro* this, PlayState* play) {
    if (this->timer1 > 0) {
        this->timer1--;
    }

    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6))) {
        this->timer1 = 64;
    }

    check_movebg_switch(this, play);
}

void Bg_Gnd_Darkmeiro_Actor_move(Actor* thisx, PlayState* play2) {
    BgGndDarkmeiro* this = (BgGndDarkmeiro*)thisx;
    PlayState* play = play2;

    this->updateFunc(this, play2);
}

void Bg_Gnd_Darkmeiro_Actor_draw0(Actor* thisx, PlayState* play) {
    Cheap_gfx_display_xlu(play, gShadowTrialPathDL);
}

void Bg_Gnd_Darkmeiro_Actor_draw1(Actor* thisx, PlayState* play) {
    BgGndDarkmeiro* this = (BgGndDarkmeiro*)thisx;
    s16 vanishTimer;

    vanishTimer = this->timer1;
    if (vanishTimer != 0) {
        if (vanishTimer > 64) {
            this->timer2 = (this->timer2 < 120) ? this->timer2 + 8 : 127;
        } else if (vanishTimer > 16) {
            this->timer2 = (cos_s((u16)this->timer1 * 0x1000) * 64.0f) + 127.0f;
            if (this->timer2 > 127) {
                this->timer2 = 127;
            }
        } else {
            this->timer2 = vanishTimer * 8;
        }

        OPEN_DISPS(play->state.gfxCtx, "../z_bg_gnd_darkmeiro.c", 378);
        //! @bug Due to a bug in the display list, the transparency data is not used.
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 198, 202, 208, this->timer2);
        CLOSE_DISPS(play->state.gfxCtx, "../z_bg_gnd_darkmeiro.c", 380);

        Cheap_gfx_display_xlu(play, gClearBlockDL);
    }
}

void Bg_Gnd_Darkmeiro_Actor_draw2(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_bg_gnd_darkmeiro.c", 391);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 198, 202, 208, 255);
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_gnd_darkmeiro.c", 393);

    Cheap_gfx_display_xlu(play, gClearBlockDL);
}
