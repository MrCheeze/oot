/*
 * File: z_bg_hidan_curtain.c
 * Overlay: ovl_Bg_Hidan_Curtain
 * Description: Flame circle
 */

#include "z_bg_hidan_curtain.h"

#include "libc64/qrand.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "one_point_cutscene.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64play.h"

#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Hidan_Curtain_actor_ct(Actor* thisx, PlayState* play);
void Bg_Hidan_Curtain_actor_dt(Actor* thisx, PlayState* play);
void Bg_Hidan_Curtain_actor_move(Actor* thisx, PlayState* play2);
void Bg_Hidan_Curtain_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgHidanCurtain* this, PlayState* play);
static void mode_demo(BgHidanCurtain* this, PlayState* play);
static void mode_wait2(BgHidanCurtain* this, PlayState* play);
static void mode_up(BgHidanCurtain* this, PlayState* play);
static void mode_down(BgHidanCurtain* this, PlayState* play);
static void mode_small(BgHidanCurtain* this, PlayState* play);

typedef struct BgHidanCurtainParams {
    /* 0x00 */ s16 radius;
    /* 0x02 */ s16 height;
    /* 0x04 */ f32 scale;
    /* 0x08 */ f32 riseDist;
    /* 0x0C */ f32 riseSpeed;
} BgHidanCurtainParams; // size = 0x10

static ColliderCylinderInit HidanCurtainAtOcPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x01, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 81, 144, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit HidanCurtainStatusData = { 1, 80, 100, MASS_IMMOVABLE };

static BgHidanCurtainParams fire_curtain_status[] = { { 81, 144, 0.090f, 144.0f, 5.0f }, { 46, 88, 0.055f, 88.0f, 3.0f } };

ActorProfile Bg_Hidan_Curtain_Profile = {
    /**/ ACTOR_BG_HIDAN_CURTAIN,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(BgHidanCurtain),
    /**/ Bg_Hidan_Curtain_actor_ct,
    /**/ Bg_Hidan_Curtain_actor_dt,
    /**/ Bg_Hidan_Curtain_actor_move,
    /**/ Bg_Hidan_Curtain_actor_draw,
};

void Bg_Hidan_Curtain_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHidanCurtain* this = (BgHidanCurtain*)thisx;
    BgHidanCurtainParams* hcParams;

    PRINTF("Curtain (arg_data 0x%04x)\n", this->actor.params);
    Actor_world_to_eye(&this->actor, 20.0f);
    this->type = PARAMS_GET_U(thisx->params, 12, 4);
    if (this->type > 6) {
        // "Type is not set"
        PRINTF("Error : object のタイプが設定されていない(%s %d)(arg_data 0x%04x)\n", "../z_bg_hidan_curtain.c", 352,
               this->actor.params);
        Actor_delete(&this->actor);
        return;
    }

    this->size = ((this->type == 2) || (this->type == 4)) ? 1 : 0;
    hcParams = &fire_curtain_status[this->size];
    this->treasureFlag = PARAMS_GET_U(thisx->params, 6, 6);
    thisx->params &= 0x3F;

    if (DEBUG_FEATURES && ((this->actor.params < 0) || (this->actor.params > 0x3F))) {
        // "Save bit is not set"
        PRINTF("Warning : object のセーブビットが設定されていない(%s %d)(arg_data 0x%04x)\n", "../z_bg_hidan_curtain.c",
               373, this->actor.params);
    }

    Actor_set_scale(&this->actor, hcParams->scale);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &HidanCurtainAtOcPipeData);
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;
    this->collider.dim.radius = hcParams->radius;
    this->collider.dim.height = hcParams->height;
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_Status_set2(&thisx->colChkInfo, NULL, &HidanCurtainStatusData);
    if (this->type == 0) {
        this->actionFunc = mode_wait2;
    } else {
        this->actionFunc = mode_wait;
        if ((this->type == 4) || (this->type == 5)) {
            this->actor.world.pos.y = this->actor.home.pos.y - hcParams->riseDist;
        }
    }
    if (((this->type == 1) && Actor_Environment_Tbox_Check(play, this->treasureFlag)) ||
        (((this->type == 0) || (this->type == 6)) && Actor_Environment_room_clear_Check(play, this->actor.room))) {
        Actor_delete(&this->actor);
    }
    this->texScroll = fqrand() * 15.0f;
}

void Bg_Hidan_Curtain_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHidanCurtain* this = (BgHidanCurtain*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void mode_wait(BgHidanCurtain* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, this->actor.params)) {
        if (this->type == 1) {
            this->actionFunc = mode_demo;
            makeOnepointDemo(play, 3350, -99, &this->actor, CAM_ID_MAIN);
            this->timer = 50;
        } else if (this->type == 3) {
            this->actionFunc = mode_demo;
            makeOnepointDemo(play, 3360, 60, &this->actor, CAM_ID_MAIN);
            this->timer = 30;
        } else {
            this->actionFunc = mode_down;
        }
    }
}

static void mode_demo(BgHidanCurtain* this, PlayState* play) {
    if (this->timer-- == 0) {
        this->actionFunc = mode_down;
    }
}

static void mode_wait2(BgHidanCurtain* this, PlayState* play) {
    if (Actor_Environment_room_clear_Check(play, this->actor.room)) {
        this->actionFunc = mode_down;
    }
}

static void mode_wait3(BgHidanCurtain* this, PlayState* play) {
    if (!Actor_Environment_sw_Check(play, this->actor.params)) {
        this->actionFunc = mode_up;
    }
}

static void mode_up(BgHidanCurtain* this, PlayState* play) {
    f32 riseSpeed = fire_curtain_status[this->size].riseSpeed;

    if (chase_f(&this->actor.world.pos.y, this->actor.home.pos.y, riseSpeed)) {
        Actor_Environment_sw_Off(play, this->actor.params);
        this->actionFunc = mode_wait;
    }
}

static void mode_down(BgHidanCurtain* this, PlayState* play) {
    BgHidanCurtainParams* hcParams = &fire_curtain_status[this->size];

    if (chase_f(&this->actor.world.pos.y, this->actor.home.pos.y - hcParams->riseDist, hcParams->riseSpeed)) {
        if ((this->type == 0) || (this->type == 6)) {
            Actor_delete(&this->actor);
        } else if (this->type == 5) {
            this->actionFunc = mode_wait3;
        } else {
            if (this->type == 2) {
                this->timer = 400;
            } else if (this->type == 4) {
                this->timer = 200;
            } else if (this->type == 3) {
                this->timer = 160;
            } else { // this->type == 1
                this->timer = 300;
            }
            this->actionFunc = mode_small;
        }
    }
}

static void mode_small(BgHidanCurtain* this, PlayState* play) {
    DECR(this->timer);
    if (this->timer == 0) {
        this->actionFunc = mode_up;
    }
    if ((this->type == 1) || (this->type == 3)) {
        Actor_timer_level_SE_set(&this->actor, this->timer);
    }
}

void Bg_Hidan_Curtain_actor_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgHidanCurtain* this = (BgHidanCurtain*)thisx;
    BgHidanCurtainParams* hcParams = &fire_curtain_status[this->size];
    f32 riseProgress;

    if ((play->cameraPtrs[CAM_ID_MAIN]->setting == CAM_SET_SLOW_CHEST_CS) ||
        (play->cameraPtrs[CAM_ID_MAIN]->setting == CAM_SET_TURN_AROUND)) {
        this->collider.base.atFlags &= ~AT_HIT;
    } else {
        if (this->collider.base.atFlags & AT_HIT) {
            this->collider.base.atFlags &= ~AT_HIT;
            Actor_player_power_damage_set(play, &this->actor, 5.0f, this->actor.yawTowardsPlayer, 1.0f);
        }
        if ((this->type == 4) || (this->type == 5)) {
            this->actor.world.pos.y = (2.0f * this->actor.home.pos.y) - hcParams->riseDist - this->actor.world.pos.y;
        }

        this->actionFunc(this, play);

        if ((this->type == 4) || (this->type == 5)) {
            this->actor.world.pos.y = (2.0f * this->actor.home.pos.y) - hcParams->riseDist - this->actor.world.pos.y;
        }
        riseProgress = (hcParams->riseDist - (this->actor.home.pos.y - this->actor.world.pos.y)) / hcParams->riseDist;
        this->alpha = 255.0f * riseProgress;
        if (this->alpha > 50) {
            this->collider.dim.height = hcParams->height * riseProgress;
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
            if (!IS_CUTSCENE_LAYER) {
                Actor_level_SE_set(&this->actor, NA_SE_EV_FIRE_PILLAR_S - SFX_FLAG);
            }
        } else if ((this->type == 1) && Actor_Environment_Tbox_Check(play, this->treasureFlag)) {
            Actor_delete(&this->actor);
        }
        this->texScroll++;
    }
}

void Bg_Hidan_Curtain_actor_draw(Actor* thisx, PlayState* play) {
    BgHidanCurtain* this = (BgHidanCurtain*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_hidan_curtain.c", 685);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 220, 0, this->alpha);

    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, this->texScroll & 0x7F, 0, 0x20, 0x40, 1, 0,
                                (this->texScroll * -0xF) & 0xFF, 0x20, 0x40));

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_hidan_curtain.c", 698);

    gSPDisplayList(POLY_XLU_DISP++, gEffFireCircleDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_hidan_curtain.c", 702);
}
