/*
 * File: z_bg_menkuri_nisekabe.c
 * Overlay: ovl_Bg_Menkuri_Nisekabe
 * Description: False Stone Walls (Gerudo Training Grounds)
 */

#include "z_bg_menkuri_nisekabe.h"
#include "assets/objects/object_menkuri_objects/object_menkuri_objects.h"

#define FLAGS 0

void Bg_Menkuri_Nisekabe_Actor_ct(Actor* thisx, PlayState* play);
void Bg_Menkuri_Nisekabe_Actor_dt(Actor* thisx, PlayState* play);
void Bg_Menkuri_Nisekabe_Actor_move(Actor* thisx, PlayState* play);
void Bg_Menkuri_Nisekabe_Actor_draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Menkuri_Nisekabe_Profile = {
    /**/ ACTOR_BG_MENKURI_NISEKABE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_MENKURI_OBJECTS,
    /**/ sizeof(BgMenkuriNisekabe),
    /**/ Bg_Menkuri_Nisekabe_Actor_ct,
    /**/ Bg_Menkuri_Nisekabe_Actor_dt,
    /**/ Bg_Menkuri_Nisekabe_Actor_move,
    /**/ Bg_Menkuri_Nisekabe_Actor_draw,
};

static Gfx* shape_model[] = { gGTGFakeWallDL, gGTGFakeCeilingDL };

void Bg_Menkuri_Nisekabe_Actor_ct(Actor* thisx, PlayState* play) {
    BgMenkuriNisekabe* this = (BgMenkuriNisekabe*)thisx;

    Actor_set_scale(&this->actor, 0.1f);
}

void Bg_Menkuri_Nisekabe_Actor_dt(Actor* thisx, PlayState* play) {
}

void Bg_Menkuri_Nisekabe_Actor_move(Actor* thisx, PlayState* play) {
    BgMenkuriNisekabe* this = (BgMenkuriNisekabe*)thisx;

    if (play->actorCtx.lensActive) {
        this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
    } else {
        this->actor.flags &= ~ACTOR_FLAG_REACT_TO_LENS;
    }
}

void Bg_Menkuri_Nisekabe_Actor_draw(Actor* thisx, PlayState* play) {
    BgMenkuriNisekabe* this = (BgMenkuriNisekabe*)thisx;
    u32 index = PARAMS_GET_U(this->actor.params, 0, 8);

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) {
        Cheap_gfx_display_xlu(play, shape_model[index]);
    } else {
        Cheap_gfx_display(play, shape_model[index]);
    }
}
