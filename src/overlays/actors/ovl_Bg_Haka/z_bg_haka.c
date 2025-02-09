/*
 * File: z_bg_haka.c
 * Overlay: ovl_Bg_Haka
 * Description: Gravestone
 */

#include "z_bg_haka.h"

#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/object_haka/object_haka.h"

#define FLAGS 0

void Bg_Haka_actor_ct(Actor* thisx, PlayState* play);
void Bg_Haka_actor_dt(Actor* thisx, PlayState* play);
void Bg_Haka_actor_move(Actor* thisx, PlayState* play);
void Bg_Haka_actor_draw(Actor* thisx, PlayState* play);

void set_player_sound(BgHaka* this, Player* player);
static void mode_wait(BgHaka* this, PlayState* play);
static void mode_move(BgHaka* this, PlayState* play);
static void mode_stop(BgHaka* this, PlayState* play);
static void mode_message_wait(BgHaka* this, PlayState* play);

ActorProfile Bg_Haka_Profile = {
    /**/ ACTOR_BG_HAKA,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HAKA,
    /**/ sizeof(BgHaka),
    /**/ Bg_Haka_actor_ct,
    /**/ Bg_Haka_actor_dt,
    /**/ Bg_Haka_actor_move,
    /**/ Bg_Haka_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(minVelocityY, 0, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Haka_actor_ct(Actor* thisx, PlayState* play) {
    BgHaka* this = (BgHaka*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gGravestoneCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->actionFunc = mode_wait;
}

void Bg_Haka_actor_dt(Actor* thisx, PlayState* play) {
    BgHaka* this = (BgHaka*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void set_player_sound(BgHaka* this, Player* player) {
    Vec3f playerRelativePos;

    Actor_search_position_project_distanceXZ(&this->dyna.actor, &playerRelativePos, &player->actor.world.pos);
    if (fabsf(playerRelativePos.x) < 34.6f && playerRelativePos.z > -112.8f && playerRelativePos.z < -36.0f) {
        player->stateFlags2 |= PLAYER_STATE2_FORCE_SAND_FLOOR_SOUND;
    }
}

static void mode_wait(BgHaka* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->dyna.unk_150 != 0.0f) {
        if (play->sceneId == SCENE_GRAVEYARD && !LINK_IS_ADULT && IS_DAY) {
            this->dyna.unk_150 = 0.0f;
            player->stateFlags2 &= ~PLAYER_STATE2_4;
            if (!Game_play_demo_mode_check(play)) {
                message_set(play, 0x5073, NULL);
                this->dyna.actor.params = 100; // Used as a cooldown for displaying Graveyard Boy's warning
                this->actionFunc = mode_message_wait;
            }
        } else if (0.0f < this->dyna.unk_150 ||
                   (play->sceneId == SCENE_LAKE_HYLIA && !LINK_IS_ADULT && !Actor_Environment_sw_Check(play, 0x23))) {
            this->dyna.unk_150 = 0.0f;
            player->stateFlags2 &= ~PLAYER_STATE2_4;
        } else {
            this->dyna.actor.world.rot.y = this->dyna.actor.shape.rot.y + 0x8000;
            this->actionFunc = mode_move;
        }
    }
    set_player_sound(this, player);
}

static void mode_move(BgHaka* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 reachedMaxPullDist;

    this->dyna.actor.speed += 0.05f;
    this->dyna.actor.speed = CLAMP_MAX(this->dyna.actor.speed, 1.5f);
    // minVelocityY is used to keep track of the distance pulled from home
    reachedMaxPullDist = chase_f(&this->dyna.actor.minVelocityY, 60.0f, this->dyna.actor.speed);
    this->dyna.actor.world.pos.x =
        sin_s(this->dyna.actor.world.rot.y) * this->dyna.actor.minVelocityY + this->dyna.actor.home.pos.x;
    this->dyna.actor.world.pos.z =
        cos_s(this->dyna.actor.world.rot.y) * this->dyna.actor.minVelocityY + this->dyna.actor.home.pos.z;
    if (reachedMaxPullDist) {
        this->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~PLAYER_STATE2_4;
        if (this->dyna.actor.params == 1) {
            Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        } else if (!IS_DAY && play->sceneId == SCENE_GRAVEYARD) {
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_POH, this->dyna.actor.home.pos.x, this->dyna.actor.home.pos.y,
                        this->dyna.actor.home.pos.z, 0, this->dyna.actor.shape.rot.y, 0, 1);
        }
        this->actionFunc = mode_stop;
    }
    Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
}

static void mode_stop(BgHaka* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->dyna.unk_150 != 0.0f) {
        this->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~PLAYER_STATE2_4;
    }
}

static void mode_message_wait(BgHaka* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;

    if (this->dyna.actor.params != 0) {
        this->dyna.actor.params--;
    }
    if (this->dyna.unk_150 != 0.0f) {
        this->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~PLAYER_STATE2_4;
    }
    if (this->dyna.actor.params == 0) {
        this->actionFunc = mode_wait;
    }
    set_player_sound(this, player);
}

void Bg_Haka_actor_move(Actor* thisx, PlayState* play) {
    BgHaka* this = (BgHaka*)thisx;

    this->actionFunc(this, play);
}

void Bg_Haka_actor_draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_bg_haka.c", 401);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_haka.c", 406);
    gSPDisplayList(POLY_OPA_DISP++, gGravestoneStoneDL);
    Matrix_translate(0.0f, 0.0f, thisx->minVelocityY * 10.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_haka.c", 416);
    gSPDisplayList(POLY_XLU_DISP++, gGravestoneEarthDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_haka.c", 421);
}
