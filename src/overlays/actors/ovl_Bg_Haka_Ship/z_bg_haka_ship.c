/*
 * File: z_bg_haka_ship.c
 * Overlay: ovl_Bg_Haka_Ship
 * Description: Shadow Temple Ship
 */

#include "z_bg_haka_ship.h"

#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "one_point_cutscene.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64play.h"
#include "z64skin_matrix.h"

#include "assets/objects/object_haka_objects/object_haka_objects.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Haka_Ship_actor_ct(Actor* thisx, PlayState* play);
void Bg_Haka_Ship_actor_dt(Actor* thisx, PlayState* play);
void Bg_Haka_Ship_actor_move(Actor* thisx, PlayState* play);
void Bg_Haka_Ship_actor_draw(Actor* thisx, PlayState* play);
void mode_syarin(BgHakaShip* this, PlayState* play);
static void mode_wait(BgHakaShip* this, PlayState* play);
static void mode_ready(BgHakaShip* this, PlayState* play);
static void mode_move(BgHakaShip* this, PlayState* play);
static void mode_stop(BgHakaShip* this, PlayState* play);
static void mode_yure(BgHakaShip* this, PlayState* play);
static void mode_drop(BgHakaShip* this, PlayState* play);

ActorProfile Bg_Haka_Ship_Profile = {
    /**/ ACTOR_BG_HAKA_SHIP,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HAKA_OBJECTS,
    /**/ sizeof(BgHakaShip),
    /**/ Bg_Haka_Ship_actor_ct,
    /**/ Bg_Haka_Ship_actor_dt,
    /**/ Bg_Haka_Ship_actor_move,
    /**/ Bg_Haka_Ship_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Haka_Ship_actor_ct(Actor* thisx, PlayState* play) {
    BgHakaShip* this = (BgHakaShip*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    this->switchFlag = PARAMS_GET_U(thisx->params, 8, 8);
    this->dyna.actor.params &= 0xFF;

    if (this->dyna.actor.params == 0) {
        DynaPolyUty_bgdi_SG2KSG(&object_haka_objects_Col_00E408, &colHeader);
        this->counter = 8;
        this->actionFunc = mode_wait;
    } else {
        DynaPolyUty_bgdi_SG2KSG(&object_haka_objects_Col_00ED7C, &colHeader);
        this->actionFunc = mode_syarin;
    }
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->dyna.actor.world.rot.y = this->dyna.actor.shape.rot.y - 0x4000;
    this->yOffset = 0;
    if (this->dyna.actor.params == 0 &&
        Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_BG_HAKA_SHIP,
                           this->dyna.actor.world.pos.x + -10.0f, this->dyna.actor.world.pos.y + 82.0f,
                           this->dyna.actor.world.pos.z, 0, 0, 0, 1) == NULL) {
        Actor_delete(&this->dyna.actor);
    }
}

void Bg_Haka_Ship_actor_dt(Actor* thisx, PlayState* play) {
    BgHakaShip* this = (BgHakaShip*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    Nai_StopAllObjFx(&this->bellSfxPos);
}

void mode_syarin(BgHakaShip* this, PlayState* play) {
    Actor* parent = this->dyna.actor.parent;

    if (parent != NULL && parent->update != NULL) {
        this->dyna.actor.world.pos.x = parent->world.pos.x + -10.0f;
        this->dyna.actor.world.pos.y = parent->world.pos.y + 82.0f;
        this->dyna.actor.world.pos.z = parent->world.pos.z;
    } else {
        this->dyna.actor.parent = NULL;
    }
}

static void mode_wait(BgHakaShip* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, this->switchFlag)) {
        if (this->counter) {
            this->counter--;
        }
        if (this->counter == 0) {
            this->counter = 130;
            this->actionFunc = mode_ready;
            PRINTF("シーン 外輪船 ...  アァクション！！\n");
            makeOnepointDemo(play, 3390, 999, &this->dyna.actor, CAM_ID_MAIN);
        }
    }
}

static void mode_ready(BgHakaShip* this, PlayState* play) {
    if (this->counter) {
        this->counter--;
    }
    this->yOffset = sinf(this->counter * (M_PI / 25)) * 6144.0f;
    if (this->counter == 0) {
        this->counter = 50;
        this->actionFunc = mode_move;
    }
}

static void mode_move(BgHakaShip* this, PlayState* play) {
    f32 distanceFromHome;
    Actor* child;

    if (this->counter) {
        this->counter--;
    }
    if (this->counter == 0) {
        this->counter = 50;
    }
    this->dyna.actor.world.pos.y = (sinf(this->counter * (M_PI / 25)) * 50.0f) + this->dyna.actor.home.pos.y;

    distanceFromHome = this->dyna.actor.home.pos.x - this->dyna.actor.world.pos.x;
    if (distanceFromHome > 7650.0f) {
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x - 7650.0f;
        this->dyna.actor.speed = 0.0f;
    }
    if (distanceFromHome > 7600.0f && !Game_play_demo_mode_check(play)) {
        this->counter = 40;
        this->dyna.actor.speed = 0.0f;
        message_set(play, 0x5071, NULL);
        this->actionFunc = mode_stop;
    } else {
        chase_f(&this->dyna.actor.speed, 4.0f, 0.2f);
    }
    child = this->dyna.actor.child;
    if (child != NULL && child->update != NULL) {
        child->shape.rot.z += ((655.0f / 13.0f) * this->dyna.actor.speed);
    } else {
        this->dyna.actor.child = NULL;
    }
    this->yOffset = sinf(this->counter * (M_PI / 25)) * 6144.0f;
}

static void mode_stop(BgHakaShip* this, PlayState* play) {
    if (this->counter) {
        this->counter--;
    }
    if (this->counter == 0) {
        this->counter = 40;
        this->actionFunc = mode_yure;
    }
    chase_angle(&this->yOffset, 0, 128);
}

static void mode_yure(BgHakaShip* this, PlayState* play) {
    if (this->counter != 0) {
        this->counter--;
    }
    this->dyna.actor.world.pos.y = this->counter % 4 * 3 - 6 + this->dyna.actor.home.pos.y;
    if (!this->counter) {
        this->dyna.actor.gravity = -1.0f;
        this->actionFunc = mode_drop;
    }
    Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_BLOCKSINK - SFX_FLAG);
}

static void mode_drop(BgHakaShip* this, PlayState* play) {
    Actor* child;

    if (this->dyna.actor.home.pos.y - this->dyna.actor.world.pos.y > 2000.0f) {
        Actor_delete(&this->dyna.actor);
        child = this->dyna.actor.child;
        if (child != NULL && child->update != NULL) {
            Actor_delete(child);
        }
    } else {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCKSINK - SFX_FLAG);
        if ((this->dyna.actor.home.pos.y - this->dyna.actor.world.pos.y > 500.0f) &&
            MoveBG_checkRidePlayerStatus(&this->dyna)) {
            Game_play_down_restart(play);
        }
    }
}

void Bg_Haka_Ship_actor_move(Actor* thisx, PlayState* play) {
    BgHakaShip* this = (BgHakaShip*)thisx;

    this->actionFunc(this, play);
    if (this->dyna.actor.params == 0) {
        Actor_position_moveF(&this->dyna.actor);
    }
}

void Bg_Haka_Ship_actor_draw(Actor* thisx, PlayState* play) {
    BgHakaShip* this = (BgHakaShip*)thisx;
    f32 angleTemp;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_haka_ship.c", 528);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    if (1) {}

    if (this->dyna.actor.params == 0) {
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_haka_ship.c", 534);
        gSPDisplayList(POLY_OPA_DISP++, object_haka_objects_DL_00D330);
        angleTemp = BINANG_TO_RAD(this->yOffset);
        Matrix_translate(-3670.0f, 620.0f, 1150.0f, MTXMODE_APPLY);
        Matrix_rotateZ(angleTemp, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_haka_ship.c", 547);
        gSPDisplayList(POLY_OPA_DISP++, object_haka_objects_DL_005A70);
        Matrix_translate(0.0f, 0.0f, -2300.0f, MTXMODE_APPLY);
        Matrix_rotateZ(-(2.0f * angleTemp), MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_haka_ship.c", 556);
        gSPDisplayList(POLY_OPA_DISP++, object_haka_objects_DL_005A70);
    } else {
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_haka_ship.c", 562);
        gSPDisplayList(POLY_OPA_DISP++, object_haka_objects_DL_00E910);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_haka_ship.c", 568);

    if (this->actionFunc == mode_ready || this->actionFunc == mode_move) {
        s32 pad;
        Vec3f sp2C;

        sp2C.x = this->dyna.actor.world.pos.x + -367.0f;
        sp2C.y = this->dyna.actor.world.pos.y + 62.0f;
        sp2C.z = this->dyna.actor.world.pos.z;

        Skin_Matrix_MulVector(&play->viewProjectionMtxF, &sp2C, &this->bellSfxPos);
        Na_StartObjectSe_F(&this->bellSfxPos, NA_SE_EV_SHIP_BELL - SFX_FLAG);
    }
}
