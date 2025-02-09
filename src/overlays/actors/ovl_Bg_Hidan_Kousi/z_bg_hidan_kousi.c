/*
 * File: z_bg_hidan_kousi.c
 * Overlay: ovl_Bg_Hidan_Kousi
 * Description:
 */

#include "z_bg_hidan_kousi.h"

#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "one_point_cutscene.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64play.h"

#include "assets/objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Hidan_Kousi_actor_ct(Actor* thisx, PlayState* play);
void Bg_Hidan_Kousi_actor_dt(Actor* thisx, PlayState* play);
void Bg_Hidan_Kousi_actor_move(Actor* thisx, PlayState* play);
void Bg_Hidan_Kousi_actor_draw(Actor* thisx, PlayState* play);

void set_hiraitaPos(BgHidanKousi* this);
void mode_toji(BgHidanKousi* this, PlayState* play);
void mode_camerawait(BgHidanKousi* this, PlayState* play);
void mode_hiraHajime(BgHidanKousi* this, PlayState* play);
void mode_hiraIteru(BgHidanKousi* this, PlayState* play);
void mode_hiraIta(BgHidanKousi* this, PlayState* play);

static f32 kousiKyori[] = { 120.0f, 150.0f, 150.0f };

ActorProfile Bg_Hidan_Kousi_Profile = {
    /**/ ACTOR_BG_HIDAN_KOUSI,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_HIDAN_OBJECTS,
    /**/ sizeof(BgHidanKousi),
    /**/ Bg_Hidan_Kousi_actor_ct,
    /**/ Bg_Hidan_Kousi_actor_dt,
    /**/ Bg_Hidan_Kousi_actor_move,
    /**/ Bg_Hidan_Kousi_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static CollisionHeader* bgData[] = {
    &gFireTempleMetalFenceWithSlantCol,
    &gFireTempleMetalFenceCol,
    &gFireTempleMetalFence2Col,
};

static s16 offAngleY[] = {
    0x4000,
    0xC000,
    0xC000,
    0x0000,
};

static Gfx* kousiModel[] = {
    gFireTempleMetalFenceWithSlantDL,
    gFireTempleMetalFenceDL,
    gFireTempleMetalFence2DL,
};

void Bg_Hidan_Kousi_actor_set_process(BgHidanKousi* this, BgHidanKousiActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Bg_Hidan_Kousi_actor_ct(Actor* thisx, PlayState* play) {
    BgHidanKousi* this = (BgHidanKousi*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, 0);
    Actor_world_to_eye(thisx, 50.0f);
    PRINTF("◯◯◯炎の神殿オブジェクト【格子(arg_data : %0x)】出現 (%d %d)\n", thisx->params,
           PARAMS_GET_U(thisx->params, 0, 8), PARAMS_GET_U((s32)thisx->params, 8, 8));

    ValueSet_process(thisx, value_init);

#if DEBUG_FEATURES
    if (PARAMS_GET_U(thisx->params, 0, 8) < 0 || PARAMS_GET_U(thisx->params, 0, 8) >= 3) {
        PRINTF("arg_data おかしい 【格子】\n");
    }
#endif

    DynaPolyUty_bgdi_SG2KSG(bgData[PARAMS_GET_U(thisx->params, 0, 8)], &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
    thisx->world.rot.y = offAngleY[PARAMS_GET_U(this->dyna.actor.params, 0, 8)] + thisx->shape.rot.y;
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(thisx->params, 8, 8))) {
        set_hiraitaPos(this);
        Bg_Hidan_Kousi_actor_set_process(this, mode_hiraIta);
    } else {
        Bg_Hidan_Kousi_actor_set_process(this, mode_toji);
    }
}

void Bg_Hidan_Kousi_actor_dt(Actor* thisx, PlayState* play) {
    BgHidanKousi* this = (BgHidanKousi*)thisx;
    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void set_hiraitaPos(BgHidanKousi* this) {
    s32 pad[2];
    Vec3s* rot = &this->dyna.actor.world.rot;
    f32 temp1 = kousiKyori[PARAMS_GET_U(this->dyna.actor.params, 0, 8)] * sin_s(rot->y);
    f32 temp2 = kousiKyori[PARAMS_GET_U(this->dyna.actor.params, 0, 8)] * cos_s(rot->y);

    this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x + temp1;
    this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z + temp2;
}

void mode_toji(BgHidanKousi* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 8))) {
        Bg_Hidan_Kousi_actor_set_process(this, mode_camerawait);
        makeActorAttentionDemo(play, &this->dyna.actor);
        this->unk_168 = 0xC8;
    }
}

void mode_camerawait(BgHidanKousi* this, PlayState* play) {
    this->unk_168--;
    if (this->dyna.actor.category == getAttentionDemoPart() || (this->unk_168 <= 0)) {
        Bg_Hidan_Kousi_actor_set_process(this, mode_hiraHajime);
    }
}

void mode_hiraHajime(BgHidanKousi* this, PlayState* play) {
    this->dyna.actor.speed += 0.2f;
    if (this->dyna.actor.speed > 2.0f) {
        this->dyna.actor.speed = 2.0f;
        Bg_Hidan_Kousi_actor_set_process(this, mode_hiraIteru);
    }
    Actor_position_moveF(&this->dyna.actor);
    Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_SLIDE - SFX_FLAG);
}

void mode_hiraIteru(BgHidanKousi* this, PlayState* play) {
    Actor_position_move(&this->dyna.actor);
    if (kousiKyori[PARAMS_GET_U(this->dyna.actor.params, 0, 8)] <
        search_position_distance(&this->dyna.actor.home.pos, &this->dyna.actor.world.pos)) {
        set_hiraitaPos(this);
        Bg_Hidan_Kousi_actor_set_process(this, mode_hiraIta);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_STOP);
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_SLIDE - SFX_FLAG);
    }
}

void mode_hiraIta(BgHidanKousi* this, PlayState* play) {
}

void Bg_Hidan_Kousi_actor_move(Actor* thisx, PlayState* play) {
    BgHidanKousi* this = (BgHidanKousi*)thisx;

    this->actionFunc(this, play);
}

void Bg_Hidan_Kousi_actor_draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_bg_hidan_kousi.c", 350);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_hidan_kousi.c", 354);
    gSPDisplayList(POLY_OPA_DISP++, kousiModel[PARAMS_GET_U(thisx->params, 0, 8)]);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_hidan_kousi.c", 359);
}
