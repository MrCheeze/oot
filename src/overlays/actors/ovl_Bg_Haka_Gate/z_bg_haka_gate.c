/*
 * File: z_bg_haka_gate.c
 * Overlay: ovl_Bg_Haka_Gate
 * Description: Truth Spinner Puzzle (Shadow Temple)
 */

#include "z_bg_haka_gate.h"

#include "libc64/qrand.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "one_point_cutscene.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_haka_objects/object_haka_objects.h"

#define FLAGS 0

// general purpose timer
#define vTimer actionVar1

// variables for turning the statue. Deg10 rotations are in tenths of a degree
#define vTurnDirection actionVar1
#define vTurnRateDeg10 actionVar2
#define vTurnAngleDeg10 actionVar3
#define vRotYDeg10 actionVar4
#define vInitTurnAngle actionVar5

// opening angle for floor
#define vOpenAngle actionVar2

// variables for the skull flames
#define vFlameScale actionVar3
#define vIsSkullOfTruth actionVar4
#define vScrollTimer actionVar5

#define SKULL_OF_TRUTH_FOUND 100

void Bg_Haka_Gate_actor_ct(Actor* thisx, PlayState* play);
void Bg_Haka_Gate_actor_dt(Actor* thisx, PlayState* play);
void Bg_Haka_Gate_actor_move(Actor* thisx, PlayState* play);
void Bg_Haka_Gate_actor_draw(Actor* thisx, PlayState* play);

static void mode_stop(BgHakaGate* this, PlayState* play);
static void mode_wall_stop(BgHakaGate* this, PlayState* play);
static void mode_wall_wait(BgHakaGate* this, PlayState* play);
void mode_wall_rotate(BgHakaGate* this, PlayState* play);
static void mode_floor_wait(BgHakaGate* this, PlayState* play);
void mode_floor_move(BgHakaGate* this, PlayState* play);
static void mode_gate_wait(BgHakaGate* this, PlayState* play);
void mode_gate_move(BgHakaGate* this, PlayState* play);
void mode_syokudai_wait(BgHakaGate* this, PlayState* play);
void mode_syokudai_wait2(BgHakaGate* this, PlayState* play);

static s16 zou_true_angle = 0x100;
static u8 syokudai_cnt = 1;
static f32 keep_player_distance = 0;

static s16 wall_angle;

ActorProfile Bg_Haka_Gate_Profile = {
    /**/ ACTOR_BG_HAKA_GATE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_HAKA_OBJECTS,
    /**/ sizeof(BgHakaGate),
    /**/ Bg_Haka_Gate_actor_ct,
    /**/ Bg_Haka_Gate_actor_dt,
    /**/ Bg_Haka_Gate_actor_move,
    /**/ Bg_Haka_Gate_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Haka_Gate_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHakaGate* this = (BgHakaGate*)thisx;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(thisx, value_init);
    this->switchFlag = PARAMS_GET_U(thisx->params, 8, 8);
    thisx->params &= 0xFF;
    MoveBG_ct(&this->dyna, 0);
    if (thisx->params == BGHAKAGATE_SKULL) {
        if (zou_true_angle != 0x100) {
            this->actionFunc = mode_syokudai_wait2;
        } else if (ABS(thisx->shape.rot.y) < 0x4000) {
            if ((fqrand() * 3.0f) < syokudai_cnt) {
                this->vIsSkullOfTruth = true;
                zou_true_angle = thisx->shape.rot.y + 0x8000;
                if (Actor_Environment_sw_Check(play, this->switchFlag)) {
                    this->actionFunc = mode_stop;
                } else {
                    this->actionFunc = mode_syokudai_wait;
                }
            } else {
                syokudai_cnt++;
                this->actionFunc = mode_syokudai_wait2;
            }
        } else {
            this->actionFunc = mode_syokudai_wait2;
        }
        this->vScrollTimer = fqrand() * 20.0f;
        thisx->flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        if (Actor_Environment_sw_Check(play, this->switchFlag)) {
            this->vFlameScale = 350;
        }
    } else {
        if (thisx->params == BGHAKAGATE_STATUE) {
            DynaPolyUty_bgdi_SG2KSG(&object_haka_objects_Col_0131C4, &colHeader);
            this->vTimer = 0;
            keep_player_distance = 0.0f;
            if (Actor_Environment_sw_Check(play, this->switchFlag)) {
                this->actionFunc = mode_wall_stop;
            } else {
                this->actionFunc = mode_wall_wait;
            }
        } else if (thisx->params == BGHAKAGATE_FLOOR) {
            DynaPolyUty_bgdi_SG2KSG(&object_haka_objects_Col_010E10, &colHeader);
            if (Actor_Environment_sw_Check(play, this->switchFlag)) {
                this->actionFunc = mode_stop;
            } else {
                this->actionFunc = mode_floor_wait;
            }
        } else { // BGHAKAGATE_GATE
            DynaPolyUty_bgdi_SG2KSG(&object_haka_objects_Col_00A938, &colHeader);
            if (Actor_Environment_sw_Check(play, this->switchFlag)) {
                this->actionFunc = mode_stop;
                thisx->world.pos.y += 80.0f;
            } else {
                thisx->flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
                Actor_world_to_eye(thisx, 30.0f);
                this->actionFunc = mode_gate_wait;
            }
        }
        this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
    }
}

void Bg_Haka_Gate_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHakaGate* this = (BgHakaGate*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    if (this->dyna.actor.params == BGHAKAGATE_STATUE) {
        zou_true_angle = 0x100;
        syokudai_cnt = 1;
    }
}

static void mode_stop(BgHakaGate* this, PlayState* play) {
}

static void mode_wall_stop(BgHakaGate* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->dyna.unk_150 != 0.0f) {
        player->stateFlags2 &= ~PLAYER_STATE2_4;
        this->dyna.unk_150 = 0.0f;
    }
}

static void mode_wall_wait(BgHakaGate* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 playerDirection;
    f32 forceDirection;

    if (this->dyna.unk_150 != 0.0f) {
        if (this->vTimer == 0) {
            this->vInitTurnAngle = this->dyna.actor.shape.rot.y - this->dyna.actor.yawTowardsPlayer;
            keep_player_distance = this->dyna.actor.xzDistToPlayer;
            forceDirection = (this->dyna.unk_150 >= 0.0f) ? 1.0f : -1.0f;
            playerDirection = ((s16)(this->dyna.actor.yawTowardsPlayer - player->actor.shape.rot.y) > 0) ? -1 : 1;
            this->vTurnDirection = playerDirection * forceDirection;
            this->actionFunc = mode_wall_rotate;
        } else {
            player->stateFlags2 &= ~PLAYER_STATE2_4;
            this->dyna.unk_150 = 0.0f;
            if (this->vTimer != 0) {
                this->vTimer--;
            }
        }
    } else {
        if (syokudai_cnt == SKULL_OF_TRUTH_FOUND) {
            this->actionFunc = mode_wall_stop;
        } else {
            this->vTimer = 0;
        }
    }
}

void mode_wall_rotate(BgHakaGate* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 turnFinished;
    s16 turnAngle;

    this->vTurnRateDeg10++;
    this->vTurnRateDeg10 = CLAMP_MAX(this->vTurnRateDeg10, 5);
    turnFinished = chase_s(&this->vTurnAngleDeg10, 600, this->vTurnRateDeg10);
    turnAngle = this->vTurnAngleDeg10 * this->vTurnDirection;
    this->dyna.actor.shape.rot.y = DEG_TO_BINANG((this->vRotYDeg10 + turnAngle) * 0.1f);
    if ((player->stateFlags2 & PLAYER_STATE2_4) && (keep_player_distance > 0.0f)) {
        player->actor.world.pos.x =
            this->dyna.actor.home.pos.x +
            (sin_s(this->dyna.actor.shape.rot.y - this->vInitTurnAngle) * keep_player_distance);
        player->actor.world.pos.z =
            this->dyna.actor.home.pos.z +
            (cos_s(this->dyna.actor.shape.rot.y - this->vInitTurnAngle) * keep_player_distance);
    } else {
        keep_player_distance = 0.0f;
    }
    wall_angle = this->dyna.actor.shape.rot.y;
    if (turnFinished) {
        player->stateFlags2 &= ~PLAYER_STATE2_4;
        this->vRotYDeg10 = (this->vRotYDeg10 + turnAngle) % 3600;
        this->vTurnRateDeg10 = 0;
        this->vTurnAngleDeg10 = 0;
        this->vTimer = 5;
        this->actionFunc = mode_wall_wait;
        this->dyna.unk_150 = 0.0f;
    }
    Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
}

static void mode_floor_wait(BgHakaGate* this, PlayState* play) {
    if ((keep_player_distance > 1.0f) && (wall_angle != 0)) {
        Player* player = GET_PLAYER(play);
        f32 radialDist;
        f32 angDist;
        f32 cos = cos_s(wall_angle);
        f32 sin = sin_s(wall_angle);
        f32 dx = player->actor.world.pos.x - this->dyna.actor.world.pos.x;
        f32 dz = player->actor.world.pos.z - this->dyna.actor.world.pos.z;

        radialDist = dx * cos - dz * sin;
        angDist = dx * sin + dz * cos;

        if ((radialDist > 110.0f) || (fabsf(angDist) > 40.0f)) {
            s16 yawDiff = zou_true_angle - wall_angle;

            keep_player_distance = 0.0f;
            if (ABS(yawDiff) < 0x80) {
                Actor_Environment_sw_On(play, this->switchFlag);
                syokudai_cnt = SKULL_OF_TRUTH_FOUND;
                this->actionFunc = mode_stop;
            } else {
                Na_StartSystemSe_F(NA_SE_SY_ERROR);
                Actor_SE_set(&this->dyna.actor, NA_SE_EV_GROUND_GATE_OPEN);
                DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
                this->vTimer = 60;
                this->actionFunc = mode_floor_move;
            }
        }
    }
}

void mode_floor_move(BgHakaGate* this, PlayState* play) {
    if (this->vTimer != 0) {
        this->vTimer--;
    }
    if (this->vTimer == 0) {
        if (chase_angle(&this->vOpenAngle, 0, 0x800)) {
            DynaPolygonInfo_clearThrough(play, &play->colCtx.dyna, this->dyna.bgId);
            this->actionFunc = mode_floor_wait;
        }
    } else {
        chase_angle(&this->vOpenAngle, 0x3000, 0x800);
    }
}

static void mode_gate_wait(BgHakaGate* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, this->switchFlag)) {
        makeActorAttentionDemo(play, &this->dyna.actor);
        this->actionFunc = mode_gate_move;
    }
}

void mode_gate_move(BgHakaGate* this, PlayState* play) {
    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 80.0f, 1.0f)) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_STOP);
        this->dyna.actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actionFunc = mode_stop;
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_SLIDE - SFX_FLAG);
    }
}

void mode_syokudai_wait(BgHakaGate* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, this->switchFlag) && chase_s(&this->vFlameScale, 350, 20)) {
        this->actionFunc = mode_stop;
    }
}

void mode_syokudai_wait2(BgHakaGate* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, this->switchFlag)) {
        chase_s(&this->vFlameScale, 350, 20);
    }
    if (play->actorCtx.lensActive) {
        this->dyna.actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
    } else {
        this->dyna.actor.flags &= ~ACTOR_FLAG_REACT_TO_LENS;
    }
}

void Bg_Haka_Gate_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHakaGate* this = (BgHakaGate*)thisx;

    this->actionFunc(this, play);
    if (this->dyna.actor.params == BGHAKAGATE_SKULL) {
        this->vScrollTimer++;
    }
}

void haka_syoku_draw_torch(BgHakaGate* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;
    f32 scale;

    if (this->vFlameScale > 0) {
        OPEN_DISPS(play->state.gfxCtx, "../z_bg_haka_gate.c", 716);

        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x40, 1, 0,
                                    (this->vScrollTimer * -20) & 0x1FF, 0x20, 0x80));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);

        Matrix_translate(thisx->world.pos.x, thisx->world.pos.y + 15.0f, thisx->world.pos.z, MTXMODE_NEW);
        Matrix_rotateY(BINANG_TO_RAD(getRealCameraAngleY(GET_ACTIVE_CAM(play))), MTXMODE_APPLY);
        scale = this->vFlameScale * 0.00001f;

        Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_haka_gate.c", 744);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        CLOSE_DISPS(play->state.gfxCtx, "../z_bg_haka_gate.c", 749);
    }
}

void Bg_Haka_Gate_actor_draw(Actor* thisx, PlayState* play) {
    static Gfx* shape_model[] = {
        object_haka_objects_DL_012270,
        object_haka_objects_DL_010A10,
        object_haka_objects_DL_00A860,
        object_haka_objects_DL_00F1B0,
    };
    BgHakaGate* this = (BgHakaGate*)thisx;
    MtxF currentMtxF;

    if (CHECK_FLAG_ALL(thisx->flags, ACTOR_FLAG_REACT_TO_LENS)) {
        Cheap_gfx_display_xlu(play, object_haka_objects_DL_00F1B0);
    } else {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        if (thisx->params == BGHAKAGATE_FLOOR) {
            OPEN_DISPS(play->state.gfxCtx, "../z_bg_haka_gate.c", 781);
            Matrix_get(&currentMtxF);
            Matrix_translate(0.0f, 0.0f, -2000.0f, MTXMODE_APPLY);
            Matrix_rotateX(BINANG_TO_RAD(this->vOpenAngle), MTXMODE_APPLY);
            Matrix_translate(0.0f, 0.0f, 2000.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_haka_gate.c", 788);
            gSPDisplayList(POLY_OPA_DISP++, object_haka_objects_DL_010A10);
            Matrix_put(&currentMtxF);
            Matrix_translate(0.0f, 0.0f, 2000.0f, MTXMODE_APPLY);
            Matrix_rotateX(BINANG_TO_RAD(-this->vOpenAngle), MTXMODE_APPLY);
            Matrix_translate(0.0f, 0.0f, -2000.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_haka_gate.c", 796);
            gSPDisplayList(POLY_OPA_DISP++, object_haka_objects_DL_010C10);
            CLOSE_DISPS(play->state.gfxCtx, "../z_bg_haka_gate.c", 800);
        } else {
            Cheap_gfx_display(play, shape_model[thisx->params]);
        }
    }
    if (thisx->params == BGHAKAGATE_SKULL) {
        haka_syoku_draw_torch(this, play);
    }
}
