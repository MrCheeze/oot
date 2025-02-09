/*
 * File: z_bg_spot17_funen
 * Overlay: ovl_Bg_Spot17_Funen
 * Description: Crater Smoke Cone
 */

#include "z_bg_spot17_funen.h"
#include "assets/objects/object_spot17_obj/object_spot17_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Spot17_Funen_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot17_Funen_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot17_Funen_actor_move_WAIT(Actor* thisx, PlayState* play);
void Bg_Spot17_Funen_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot17_Funen_actor_draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Spot17_Funen_Profile = {
    /**/ ACTOR_BG_SPOT17_FUNEN,
    /**/ ACTORCAT_SWITCH,
    /**/ FLAGS,
    /**/ OBJECT_SPOT17_OBJ,
    /**/ sizeof(BgSpot17Funen),
    /**/ Bg_Spot17_Funen_actor_ct,
    /**/ Bg_Spot17_Funen_actor_dt,
    /**/ Bg_Spot17_Funen_actor_move_WAIT,
    /**/ NULL,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Spot17_Funen_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot17Funen* this = (BgSpot17Funen*)thisx;

    ValueSet_process(&this->actor, value_init);
    PRINTF("spot17 obj. 噴煙 (arg_data 0x%04x)\n", this->actor.params);
}

void Bg_Spot17_Funen_actor_dt(Actor* thisx, PlayState* play) {
}

void Bg_Spot17_Funen_actor_move_WAIT(Actor* thisx, PlayState* play) {
    BgSpot17Funen* this = (BgSpot17Funen*)thisx;

    this->actor.draw = Bg_Spot17_Funen_actor_draw;
    this->actor.update = Bg_Spot17_Funen_actor_move;
}

void Bg_Spot17_Funen_actor_move(Actor* thisx, PlayState* play) {
}

void Bg_Spot17_Funen_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot17_funen.c", 153);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    Matrix_rotateY((s16)(getRealCameraAngleY(GET_ACTIVE_CAM(play)) - thisx->shape.rot.y + 0x8000) * 9.58738019108e-05f,
                   MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_spot17_funen.c", 161);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (0 - play->gameplayFrames) & 0x7F, 0x20, 0x20,
                                1, 0, (0 - play->gameplayFrames) & 0x7F, 0x20, 0x20));
    gSPDisplayList(POLY_XLU_DISP++, gCraterSmokeConeDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot17_funen.c", 176);
}
