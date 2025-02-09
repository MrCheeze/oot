/*
 * File: z_bg_hidan_fwbig.c
 * Overlay: ovl_Bg_Hidan_Fwbig
 * Description: Large fire walls at Fire Temple (flame wall before bombable door and the one that chases the player in
 * the lava room)
 */

#include "z_bg_hidan_fwbig.h"

#include "ichain.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "one_point_cutscene.h"
#include "segmented_address.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef enum HidanFwbigMoveState {
    /* 0 */ FWBIG_MOVE,
    /* 1 */ FWBIG_RESET,
    /* 2 */ FWBIG_KILL
} HidanFwbigMoveState;

void Bg_Hidan_Fwbig_actor_ct(Actor* thisx, PlayState* play2);
void Bg_Hidan_Fwbig_actor_dt(Actor* thisx, PlayState* play);
void Bg_Hidan_Fwbig_actor_move(Actor* thisx, PlayState* play);
void Bg_Hidan_Fwbig_actor_draw(Actor* thisx, PlayState* play);

void hidan_movefwbig_set_pos(BgHidanFwbig* this);

static void mode_wait(BgHidanFwbig* this, PlayState* play);
static void mode_demo(BgHidanFwbig* this, PlayState* play);
static void mode_down(BgHidanFwbig* this, PlayState* play);
static void mode_small(BgHidanFwbig* this, PlayState* play);
static void mode_move_wait(BgHidanFwbig* this, PlayState* play);
static void mode_move(BgHidanFwbig* this, PlayState* play);

ActorProfile Bg_Hidan_Fwbig_Profile = {
    /**/ ACTOR_BG_HIDAN_FWBIG,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_HIDAN_OBJECTS,
    /**/ sizeof(BgHidanFwbig),
    /**/ Bg_Hidan_Fwbig_actor_ct,
    /**/ Bg_Hidan_Fwbig_actor_dt,
    /**/ Bg_Hidan_Fwbig_actor_move,
    /**/ Bg_Hidan_Fwbig_actor_draw,
};

static ColliderCylinderInit HidanFwbigAtOcPipeData = {
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
    { 30, 130, 0, { 0, 0, 0 } },
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 1000, ICHAIN_STOP),
};

void Bg_Hidan_Fwbig_actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgHidanFwbig* this = (BgHidanFwbig*)thisx;
    Player* player = GET_PLAYER(play);

    ValueSet_process(&this->actor, value_init);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &HidanFwbigAtOcPipeData);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->direction = (u16)PARAMS_GET_NOMASK(thisx->params, 8);
    thisx->params &= 0xFF;
    if (this->direction != 0) {
        this->actor.home.pos.x = 1560.0f;
        this->actor.home.pos.z = 0.0f;
        if (player->actor.world.pos.z > 300.0f) {
            this->direction = -1;
            this->actor.home.rot.y = this->actor.shape.rot.y = -0x4E38;
        } else if (player->actor.world.pos.z < -300.0f) {
            this->direction = 1;
            this->actor.home.rot.y = this->actor.shape.rot.y = -0x31C8;
        } else {
            Actor_delete(&this->actor);
            return;
        }
        hidan_movefwbig_set_pos(this);
        Actor_set_scale(&this->actor, 0.15f);
        this->collider.dim.height = 230;
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->moveState = FWBIG_MOVE;
        this->actionFunc = mode_move_wait;
        this->actor.world.pos.y = this->actor.home.pos.y - (2400.0f * this->actor.scale.y);
    } else {
        Actor_set_scale(&this->actor, 0.1f);
        this->actionFunc = mode_wait;
    }
}

void Bg_Hidan_Fwbig_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHidanFwbig* this = (BgHidanFwbig*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void hidan_movefwbig_set_pos(BgHidanFwbig* this) {
    s16 startAngle = this->actor.shape.rot.y + this->direction * -0x4000;

    this->actor.world.pos.x = (sin_s(startAngle) * 885.4f) + this->actor.home.pos.x;
    this->actor.world.pos.z = (cos_s(startAngle) * 885.4f) + this->actor.home.pos.z;
}

static void mode_wait(BgHidanFwbig* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, this->actor.params)) {
        this->actionFunc = mode_demo;
        makeOnepointDemo(play, 3340, -99, &this->actor, CAM_ID_MAIN);
        this->timer = 35;
    }
}

static void mode_demo(BgHidanFwbig* this, PlayState* play) {
    if (this->timer-- == 0) {
        this->actionFunc = mode_down;
    }
}

static void mode_up(BgHidanFwbig* this, PlayState* play) {
    if (chase_f(&this->actor.world.pos.y, this->actor.home.pos.y, 10.0f)) {
        if (this->direction == 0) {
            Actor_Environment_sw_Off(play, this->actor.params);
            this->actionFunc = mode_wait;
        } else {
            this->actionFunc = mode_move;
        }
    }
}

static void mode_down(BgHidanFwbig* this, PlayState* play) {
    if (chase_f(&this->actor.world.pos.y, this->actor.home.pos.y - (2400.0f * this->actor.scale.y), 10.0f)) {
        if (this->direction == 0) {
            this->actionFunc = mode_small;
            this->timer = 150;
        } else if (this->moveState == FWBIG_KILL) {
            Actor_delete(&this->actor);
        } else {
            if (this->moveState == FWBIG_MOVE) {
                this->actor.shape.rot.y -= (this->direction * 0x1800);
            } else {
                this->moveState = FWBIG_MOVE;
                this->actor.shape.rot.y = this->actor.home.rot.y;
            }
            hidan_movefwbig_set_pos(this);
            this->actionFunc = mode_up;
        }
    }
}

static void mode_small(BgHidanFwbig* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        this->actionFunc = mode_up;
    }
    Actor_timer_level_SE_set(&this->actor, this->timer);
}

static void mode_move_wait(BgHidanFwbig* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->actor.world.pos.x < 1150.0f) {
        this->actionFunc = mode_up;
        makeOnepointDemo(play, 3290, -99, &this->actor, CAM_ID_MAIN);
    }
}

static void mode_move(BgHidanFwbig* this, PlayState* play) {
    if (!player_demo_check(play)) {
        if (chase_angle(&this->actor.shape.rot.y, this->actor.home.rot.y + (this->direction * 0x6390), 0x20)) {
            this->moveState = FWBIG_RESET;
            this->actionFunc = mode_down;
        } else {
            hidan_movefwbig_set_pos(this);
        }
    }
}

void set_hian_fwbig_atoc_pipe(BgHidanFwbig* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f playerRelativePos;
    f32 cs;
    f32 sn;

    Actor_search_position_project_distanceXZ(&this->actor, &playerRelativePos, &player->actor.world.pos);
    playerRelativePos.z = ((playerRelativePos.z >= 0.0f) ? 1.0f : -1.0f) * 25.0f * -1.0f;
    if (this->direction == 0) {
        playerRelativePos.x = CLAMP(playerRelativePos.x, -360.0f, 360.0f);
    } else {
        playerRelativePos.x = CLAMP(playerRelativePos.x, -500.0f, 500.0f);
    }

    sn = sin_s(this->actor.shape.rot.y);
    cs = cos_s(this->actor.shape.rot.y);
    this->collider.dim.pos.x = this->actor.world.pos.x + (playerRelativePos.x * cs) + (playerRelativePos.z * sn);
    this->collider.dim.pos.z = this->actor.world.pos.z - (playerRelativePos.x * sn) + (playerRelativePos.z * cs);
    this->collider.dim.pos.y = this->actor.world.pos.y;

    this->actor.world.rot.y = (playerRelativePos.z < 0.0f) ? this->actor.shape.rot.y : this->actor.shape.rot.y + 0x8000;
}

void Bg_Hidan_Fwbig_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHidanFwbig* this = (BgHidanFwbig*)thisx;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        Actor_player_power_damage_set(play, &this->actor, 5.0f, this->actor.world.rot.y, 1.0f);
        if (this->direction != 0) {
            this->actionFunc = mode_down;
        }
    }
    if ((this->direction != 0) && (play->roomCtx.prevRoom.num == this->actor.room)) {
        this->moveState = FWBIG_KILL;
        this->actionFunc = mode_down;
    }

    this->actionFunc(this, play);

    if ((this->actor.home.pos.y - 200.0f) < this->actor.world.pos.y) {
        if (!IS_CUTSCENE_LAYER) {
            Actor_level_SE_set(&this->actor, NA_SE_EV_BURNING - SFX_FLAG);
        } else if ((s16)this->actor.world.pos.x == -513) {
            Actor_level_SE_set(&this->actor, NA_SE_EV_FLAME_OF_FIRE - SFX_FLAG);
        }
        set_hian_fwbig_atoc_pipe(this, play);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void Bg_Hidan_Fwbig_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    f32 height;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_hidan_fwbig.c", 630);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gEffUnknown4Tex));

    gSPSegment(POLY_XLU_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gEffUnknown5Tex));

    height = thisx->scale.y * 2400.0f;
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 220, 0,
                    ((height - (thisx->home.pos.y - thisx->world.pos.y)) * 255.0f) / height);

    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, play->gameplayFrames % 0x80, 0, 0x20, 0x40, 1, 0,
                                (u8)(play->gameplayFrames * -15), 0x20, 0x40));

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_hidan_fwbig.c", 660);

    gSPDisplayList(POLY_XLU_DISP++, gFireTempleBigFireWallDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_hidan_fwbig.c", 664);
}
