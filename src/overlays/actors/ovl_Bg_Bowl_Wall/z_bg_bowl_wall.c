/*
 * File: z_bg_bowl_wall.c
 * Overlay: Bg_Bowl_Wall
 * Description: Bombchu Bowling Alley Wall
 */

#include "z_bg_bowl_wall.h"
#include "overlays/actors/ovl_En_Wall_Tubo/z_en_wall_tubo.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#include "gfx.h"
#include "gfx_setupdl.h"
#include "rand.h"
#include "sfx.h"
#include "quake.h"
#include "sys_matrix.h"
#include "terminal.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"

#include "assets/objects/object_bowl/object_bowl.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Bowl_Wall_actor_ct(Actor* thisx, PlayState* play);
void Bg_Bowl_Wall_actor_dt(Actor* thisx, PlayState* play);
void Bg_Bowl_Wall_actor_move(Actor* thisx, PlayState* play);
void Bg_Bowl_Wall_actor_draw(Actor* thisx, PlayState* play2);

static void mode_wait_init(BgBowlWall* this, PlayState* play);
static void mode_wait(BgBowlWall* this, PlayState* play);
static void mode_break(BgBowlWall* this, PlayState* play);
void mode_revert_wait(BgBowlWall* this, PlayState* play);
void mode_revert(BgBowlWall* this, PlayState* play);

ActorProfile Bg_Bowl_Wall_Profile = {
    /**/ ACTOR_BG_BOWL_WALL,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_BOWL,
    /**/ sizeof(BgBowlWall),
    /**/ Bg_Bowl_Wall_actor_ct,
    /**/ Bg_Bowl_Wall_actor_dt,
    /**/ Bg_Bowl_Wall_actor_move,
    /**/ Bg_Bowl_Wall_actor_draw,
};

static Vec3f hosei_pos[] = {
    { 0.0f, 210.0f, -20.0f },
    { 0.0f, 170.0f, -20.0f },
    { -170.0f, 0.0f, -20.0f },
    { 170.0f, 0.0f, -20.0f },
};

static s16 init_angle_data[] = { 0x0000, 0x0000, 0x3FFF, -0x3FFF };

void Bg_Bowl_Wall_actor_ct(Actor* thisx, PlayState* play) {
    BgBowlWall* this = (BgBowlWall*)thisx;
    s32 pad1;
    s32 pad2;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, 0);

    if (this->dyna.actor.params == 0) {
        DynaPolyUty_bgdi_SG2KSG(&gBowlingFirstAndFinalRoundCol, &colHeader);
    } else {
        DynaPolyUty_bgdi_SG2KSG(&gBowlingSecondRoundCol, &colHeader);
    }

    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->initPos = this->dyna.actor.world.pos;
    PRINTF("\n\n");
    PRINTF(VT_FGCOL(GREEN) " ☆☆☆☆☆ ボーリングおじゃま壁発生 ☆☆☆☆☆ %d\n" VT_RST, this->dyna.actor.params);
    this->actionFunc = mode_wait_init;
    this->dyna.actor.scale.x = this->dyna.actor.scale.y = this->dyna.actor.scale.z = 1.0f;
}

void Bg_Bowl_Wall_actor_dt(Actor* thisx, PlayState* play) {
    BgBowlWall* this = (BgBowlWall*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mode_wait_init(BgBowlWall* this, PlayState* play) {
    s32 pad;
    EnWallTubo* bullseye;
    Actor* lookForGirl;
    s16 type;

    type = this->dyna.actor.params;
    if (type != 0) {
        type += (s16)rnd_f(2.99f);
        this->dyna.actor.shape.rot.z = this->dyna.actor.world.rot.z = init_angle_data[type];
        PRINTF("\n\n");
    }
    this->bullseyeCenter.x = hosei_pos[type].x + this->dyna.actor.world.pos.x;
    this->bullseyeCenter.y = hosei_pos[type].y + this->dyna.actor.world.pos.y;
    this->bullseyeCenter.z = hosei_pos[type].z + this->dyna.actor.world.pos.z;
    if (1) {}
    bullseye = (EnWallTubo*)Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_EN_WALL_TUBO,
                                               this->bullseyeCenter.x, this->bullseyeCenter.y, this->bullseyeCenter.z,
                                               0, 0, 0, this->dyna.actor.params);
    if (bullseye != NULL) {
        bullseye->explosionCenter = this->bullseyeCenter;
        if (type != 0) {
            bullseye->explosionCenter = this->bullseyeCenter = this->dyna.actor.world.pos;
        }
        if (this->chuGirl == NULL) {
            lookForGirl = play->actorCtx.actorLists[ACTORCAT_NPC].head;
            while (lookForGirl != NULL) {
                if (lookForGirl->id != ACTOR_EN_BOM_BOWL_MAN) {
                    lookForGirl = lookForGirl->next;
                } else {
                    this->chuGirl = (EnBomBowlMan*)lookForGirl;
                    break;
                }
            }
        }
        this->actionFunc = mode_wait;
    }
}

static void mode_wait(BgBowlWall* this, PlayState* play) {
    if (this->isHit) {
        this->actionFunc = mode_break;
    }
}

static void mode_break(BgBowlWall* this, PlayState* play) {
    s16 pad;
    Vec3f effectAccel = { 0.0f, 0.1f, 0.0f };
    Vec3f effectVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f effectPos;
    s16 quakeIndex;
    s32 wallFallen;
    s32 i;

    wallFallen = false;

    if (this->dyna.actor.params == 0) { // wall collapses backwards
        add_calc_short_angle2(&this->dyna.actor.shape.rot.x, -0x3E80, 3, 500, 0);
        this->dyna.actor.world.rot.x = this->dyna.actor.shape.rot.x;
        if (this->dyna.actor.shape.rot.x < -0x3C1E) {
            wallFallen = true;
        }
    } else { // wall slides downwards
        add_calc2(&this->dyna.actor.world.pos.y, this->initPos.y - 450.0f, 0.3f, 10.0f);
        if (this->dyna.actor.world.pos.y < (this->initPos.y - 400.0f)) {
            wallFallen = true;
        }
    }

    if (wallFallen) {
        for (i = 0; i < 15; i++) {
            effectPos.x = rnd_fx(300.0f) + this->bullseyeCenter.x;
            effectPos.y = -100.0f;
            effectPos.z = rnd_fx(400.0f) + this->bullseyeCenter.z;
            Effect_SS_Bomb2_2_ct(play, &effectPos, &effectVelocity, &effectAccel, 100, 30);
            effectPos.y = -50.0f;
            Effect_Hahen_Kakusan_ct3(play, &effectPos, 10.0f, 0, 50, 15, 3, HAHEN_OBJECT_DEFAULT, 10, NULL);
            Actor_SE_set(&this->dyna.actor, NA_SE_IT_BOMB_EXPLOSION);
        }
        quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_1);
        setSpeedQuake(quakeIndex, 0x7FFF);
        setScaleQuake(quakeIndex, 300, 0, 0, 0);
        setTimerQuake(quakeIndex, 30);
        this->timer = 20;
        this->actionFunc = mode_revert_wait;
    }
}

void mode_revert_wait(BgBowlWall* this, PlayState* play) {
    if (this->timer >= 2) {
        if (this->dyna.actor.params == 0) {
            add_calc_short_angle2(&this->dyna.actor.shape.rot.x, -0x3E80, 1, 200, 0);
        } else {
            add_calc2(&this->dyna.actor.world.pos.y, this->initPos.y - 450.0f, 0.3f, 10.0f);
        }
    } else if (this->timer == 1) {
        this->dyna.actor.world.rot.x = this->dyna.actor.shape.rot.x = 0;
        this->dyna.actor.world.pos.y = this->initPos.y - 450.0f;
        this->chuGirl->wallStatus[this->dyna.actor.params] = 2;
        this->actionFunc = mode_revert;
    }
}

void mode_revert(BgBowlWall* this, PlayState* play) {
    if (this->chuGirl->wallStatus[this->dyna.actor.params] != 2) {
        add_calc2(&this->dyna.actor.world.pos.y, this->initPos.y, 0.3f, 50.0f);
        if (fabsf(this->dyna.actor.world.pos.y - this->initPos.y) <= 10.0f) {
            this->dyna.actor.world.pos.y = this->initPos.y;
            this->isHit = false;
            this->actionFunc = mode_wait_init;
        }
    }
}

void Bg_Bowl_Wall_actor_move(Actor* thisx, PlayState* play) {
    BgBowlWall* this = (BgBowlWall*)thisx;

    if (this->timer != 0) {
        this->timer--;
    }

    this->actionFunc(this, play);
}

void Bg_Bowl_Wall_actor_draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgBowlWall* this = (BgBowlWall*)thisx;
    u32 frames;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_bowl_wall.c", 441);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x8, tex_scroll2(play->state.gfxCtx, 0, -2 * (frames = play->state.frames), 16, 16));
    gDPPipeSync(POLY_OPA_DISP++);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_bowl_wall.c", 453);

    if (this->dyna.actor.params == 0) {
        gSPDisplayList(POLY_OPA_DISP++, gBowlingRound1WallDL);
    } else {
        gSPDisplayList(POLY_OPA_DISP++, gBowlingRound2WallDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_bowl_wall.c", 464);
}
