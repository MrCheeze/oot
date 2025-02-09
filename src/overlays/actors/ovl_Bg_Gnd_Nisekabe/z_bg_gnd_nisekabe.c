/*
 * File: z_bg_gnd_nisekabe.c
 * Overlay: ovl_Bg_Gnd_Nisekabe
 * Description: Ganon's Castle Fake Wall
 */

#include "z_bg_gnd_nisekabe.h"

#include "z64play.h"

#include "assets/objects/object_demo_kekkai/object_demo_kekkai.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Gnd_Nisekabe_Actor_ct(Actor* thisx, PlayState* play);
void Bg_Gnd_Nisekabe_Actor_dt(Actor* thisx, PlayState* play);
void Bg_Gnd_Nisekabe_Actor_move(Actor* thisx, PlayState* play);
void Bg_Gnd_Nisekabe_Actor_draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Gnd_Nisekabe_Profile = {
    /**/ ACTOR_BG_GND_NISEKABE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_DEMO_KEKKAI,
    /**/ sizeof(BgGndNisekabe),
    /**/ Bg_Gnd_Nisekabe_Actor_ct,
    /**/ Bg_Gnd_Nisekabe_Actor_dt,
    /**/ Bg_Gnd_Nisekabe_Actor_move,
    /**/ Bg_Gnd_Nisekabe_Actor_draw,
};

void Bg_Gnd_Nisekabe_Actor_ct(Actor* thisx, PlayState* play) {
    BgGndNisekabe* this = (BgGndNisekabe*)thisx;

    Actor_set_scale(&this->actor, 0.1);
    this->actor.cullingVolumeDistance = 3000.0;
}

void Bg_Gnd_Nisekabe_Actor_dt(Actor* thisx, PlayState* play) {
}

void Bg_Gnd_Nisekabe_Actor_move(Actor* thisx, PlayState* play) {
    BgGndNisekabe* this = (BgGndNisekabe*)thisx;

    if (play->actorCtx.lensActive) {
        this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
    } else {
        this->actor.flags &= ~ACTOR_FLAG_REACT_TO_LENS;
    }
}

void Bg_Gnd_Nisekabe_Actor_draw(Actor* thisx, PlayState* play) {
    static Gfx* shape_model[] = {
        gLightTrialFakeWallDL,
        gGanonsCastleUnusedFakeWallDL,
        gGanonsCastleScrubsFakeWallDL,
    };
    BgGndNisekabe* this = (BgGndNisekabe*)thisx;
    u32 index = PARAMS_GET_U(this->actor.params, 0, 8);

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) {
        Cheap_gfx_display_xlu(play, shape_model[index]);
    } else {
        Cheap_gfx_display(play, shape_model[index]);
    }
}
