/*
 * File: z_bg_hidan_rock.c
 * Overlay: ovl_Bg_Hidan_Rock
 * Description: Stone blocks (Fire Temple)
 */

#include "z_bg_hidan_rock.h"

#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "rumble.h"
#include "segmented_address.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64play.h"
#include "z64player.h"
#include "z64skin_matrix.h"

#include "assets/objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS 0

void Bg_Hidan_Rock_actor_ct(Actor* thisx, PlayState* play);
void Bg_Hidan_Rock_actor_dt(Actor* thisx, PlayState* play);
void Bg_Hidan_Rock_actor_move(Actor* thisx, PlayState* play);
void Bg_Hidan_Rock_actor_draw(Actor* thisx, PlayState* play);

static void mode_up_init(BgHidanRock* this);

void mode_push(BgHidanRock* this, PlayState* play);
void mode_goal_wait(BgHidanRock* this, PlayState* play);
void mode_hane_wait(BgHidanRock* this, PlayState* play);
static void mode_yure(BgHidanRock* this, PlayState* play);
static void mode_down(BgHidanRock* this, PlayState* play);
static void mode_sizumu(BgHidanRock* this, PlayState* play);
static void mode_stop(BgHidanRock* this, PlayState* play);
static void mode_up(BgHidanRock* this, PlayState* play);

static void draw_fire(PlayState* play, BgHidanRock* this);

static Vec3f goal_fire_pos = { 3310.0f, 120.0f, 0.0f };

ActorProfile Bg_Hidan_Rock_Profile = {
    /**/ ACTOR_BG_HIDAN_ROCK,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HIDAN_OBJECTS,
    /**/ sizeof(BgHidanRock),
    /**/ Bg_Hidan_Rock_actor_ct,
    /**/ Bg_Hidan_Rock_actor_dt,
    /**/ Bg_Hidan_Rock_actor_move,
    /**/ Bg_Hidan_Rock_actor_draw,
};

static ColliderCylinderInit HidanRockAtPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x01, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { 45, 77, -40, { 3310, 120, 0 } },
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -600, ICHAIN_STOP),
};

void Bg_Hidan_Rock_actor_ct(Actor* thisx, PlayState* play) {
    BgHidanRock* this = (BgHidanRock*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(thisx, value_init);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);

    this->type = PARAMS_GET_U(thisx->params, 0, 8);
    this->unk_169 = 0;

    thisx->params = PARAMS_GET_U(thisx->params, 8, 8);

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, thisx, &HidanRockAtPipeData);

    if (this->type == 0) {
        if (Actor_Environment_sw_Check(play, thisx->params)) {
            xyz_t_move(&thisx->home.pos, &goal_fire_pos);
            xyz_t_move(&thisx->world.pos, &goal_fire_pos);
            this->timer = 60;
            this->actionFunc = mode_goal_wait;
        } else {
            this->actionFunc = mode_push;
        }
        thisx->flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED;
        DynaPolyUty_bgdi_SG2KSG(&gFireTempleStoneBlock1Col, &colHeader);
    } else {
        DynaPolyUty_bgdi_SG2KSG(&gFireTempleStoneBlock2Col, &colHeader);
        this->collider.dim.pos.x = thisx->home.pos.x;
        this->collider.dim.pos.y = thisx->home.pos.y;
        this->collider.dim.pos.z = thisx->home.pos.z;
        this->actionFunc = mode_hane_wait;
    }

    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
    this->timer = 0;
}

void Bg_Hidan_Rock_actor_dt(Actor* thisx, PlayState* play) {
    BgHidanRock* this = (BgHidanRock*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    ClObjPipe_dt(play, &this->collider);
    Nai_StopAllObjFx(&this->unk_170);
}

static void mode_up_init(BgHidanRock* this) {
    this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED;
    this->actionFunc = mode_up;
}

void mode_push(BgHidanRock* this, PlayState* play) {
    static f32 move_length = 0.0f;
    f32 sp2C;
    s32 temp_v1;
    s32 frame;
    Player* player = GET_PLAYER(play);

    if (this->dyna.unk_150 != 0.0f) {
        if (this->timer == 0) {
            if (move_length == 0.0f) {
                if (this->dyna.unk_150 > 0.0f) {
                    move_length += 0.01f;
                } else {
                    move_length -= 0.01f;
                }
            }

            this->dyna.actor.speed += 0.05f;
            this->dyna.actor.speed = CLAMP_MAX(this->dyna.actor.speed, 2.0f);

            if (move_length > 0.0f) {
                temp_v1 = chase_f(&move_length, 20.0f, this->dyna.actor.speed);
            } else {
                temp_v1 = chase_f(&move_length, -20.0f, this->dyna.actor.speed);
            }

            this->dyna.actor.world.pos.x = (sin_s(this->dyna.unk_158) * move_length) + this->dyna.actor.home.pos.x;
            this->dyna.actor.world.pos.z = (cos_s(this->dyna.unk_158) * move_length) + this->dyna.actor.home.pos.z;

            if (temp_v1) {
                player->stateFlags2 &= ~PLAYER_STATE2_4;
                this->dyna.unk_150 = 0.0f;
                this->dyna.actor.home.pos.x = this->dyna.actor.world.pos.x;
                this->dyna.actor.home.pos.z = this->dyna.actor.world.pos.z;
                move_length = 0.0f;
                this->dyna.actor.speed = 0.0f;
                this->timer = 5;
            }

            Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
        } else {
            player->stateFlags2 &= ~PLAYER_STATE2_4;
            this->dyna.unk_150 = 0.0f;
            if (this->timer != 0) {
                this->timer--;
            }
        }
    } else {
        this->timer = 0;
    }

    sp2C = this->dyna.actor.world.pos.z - goal_fire_pos.z;
    if (sp2C < 0.5f) {
        Actor_Environment_sw_On(play, this->dyna.actor.params);
        xyz_t_move(&this->dyna.actor.home.pos, &goal_fire_pos);
        this->dyna.actor.world.pos.x = goal_fire_pos.x;
        this->dyna.actor.world.pos.z = goal_fire_pos.z;
        this->dyna.actor.speed = 0.0f;
        move_length = 0.0f;
        player->stateFlags2 &= ~PLAYER_STATE2_4;
        this->actionFunc = mode_down;
    }

    frame = play->gameplayFrames & 0xFF;
    if (play->gameplayFrames & 0x100) {
        this->unk_16C = 0.0f;
    } else if (frame < 128) {
        this->unk_16C = sinf(frame * (4 * 0.001f * M_PI)) * 19.625f;
    } else if (frame < 230) {
        this->unk_16C = 19.625f;
    } else {
        this->unk_16C -= 1.0f;
        this->unk_16C = CLAMP_MIN(this->unk_16C, 0.0f);
    }

    if (sp2C < 100.0f) {
        this->unk_16C = CLAMP_MAX(this->unk_16C, 6.125f);
    }
}

void mode_goal_wait(BgHidanRock* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        mode_up_init(this);
    }
}

void mode_hane_wait(BgHidanRock* this, PlayState* play) {
    if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
        this->timer = 20;
        this->dyna.actor.world.rot.y = getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x4000;
        this->actionFunc = mode_yure;
    }
}

static void mode_yure(BgHidanRock* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer != 0) {
        this->dyna.actor.world.pos.x =
            this->dyna.actor.home.pos.x + 5.0f * sin_s(this->dyna.actor.world.rot.y + this->timer * 0x4000);
        this->dyna.actor.world.pos.z =
            this->dyna.actor.home.pos.z + 5.0f * cos_s(this->dyna.actor.world.rot.y + this->timer * 0x4000);
    } else {
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z;
        mode_up_init(this);
    }

    if (!(this->timer % 4)) {
        z_vibctl2_vib_setQ(this->dyna.actor.xyzDistToPlayerSq, 180, 10, 100);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_SHAKE);
    }
}

static void mode_down(BgHidanRock* this, PlayState* play) {
    this->timer--;
    if (this->dyna.actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        if (this->type == 0) {
            this->timer = 60;
            this->actionFunc = mode_goal_wait;
        } else {
            this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - 15.0f;
            this->actionFunc = mode_sizumu;
            this->dyna.actor.flags &= ~(ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED);
        }

        Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        Actor_SE_set(&this->dyna.actor,
                      NA_SE_PL_WALK_GROUND + T_BGCheck_getSoundGroundLabel(&play->colCtx, this->dyna.actor.floorPoly,
                                                                      this->dyna.actor.floorBgId));
    }

    this->unk_16C -= 0.5f;
    this->unk_16C = CLAMP_MIN(this->unk_16C, 0.0f);

    if (this->type == 0) {
        if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
            if (this->unk_169 == 0) {
                this->unk_169 = 3;
            }
            changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_ELEVATOR_PLATFORM);
        } else if (!MoveBG_checkRidePlayerStatus(&this->dyna)) {
            if (this->unk_169 != 0) {
                changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_DUNGEON0);
            }
            this->unk_169 = 0;
        }
    }
}

static void mode_sizumu(BgHidanRock* this, PlayState* play) {
    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 1.0f)) {
        this->actionFunc = mode_hane_wait;
    }
}

static void mode_stop(BgHidanRock* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        this->actionFunc = mode_down;
        this->dyna.actor.velocity.y = 0.0f;
    }
}

static void mode_up(BgHidanRock* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->timer++;
    if (this->dyna.unk_150 != 0.0f) {
        this->dyna.actor.speed = 0.0f;
        player->stateFlags2 &= ~PLAYER_STATE2_4;
    }

    if ((this->type == 0 && (add_calc(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 1820.0f,
                                                0.25f, 20.0f, 0.5f) < 0.1f)) ||
        ((this->type != 0) && (add_calc(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 480.0,
                                                  0.25f, 20.0f, 0.5f) < 0.1f))) {
        if (this->type == 0) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        }
        this->timer = 20;
        this->actionFunc = mode_stop;
    }

    this->unk_16C = (this->dyna.actor.world.pos.y + 50.0f - this->dyna.actor.home.pos.y + 40.0f) / 80.0f;
    if (this->type == 0) {
        if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
            if (this->unk_169 == 0) {
                this->unk_169 = 3;
            }
            changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_ELEVATOR_PLATFORM);
        } else if (!MoveBG_checkRidePlayerStatus(&this->dyna)) {
            if (this->unk_169 != 0) {
                changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_DUNGEON0);
            }
            this->unk_169 = 0;
        }
    }
}

void Bg_Hidan_Rock_actor_move(Actor* thisx, PlayState* play) {
    BgHidanRock* this = (BgHidanRock*)thisx;

    this->actionFunc(this, play);
    if (this->actionFunc == mode_down) {
        Actor_position_moveF(&this->dyna.actor);
        Actor_BGcheck2(play, &this->dyna.actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    }

    if (this->unk_16C > 0.0f) {
        this->collider.dim.height = HidanRockAtPipeData.dim.height * this->unk_16C;
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }
}

static void* hibasira_txt[] = {
    gFireTempleBigVerticalFlame0Tex, gFireTempleBigVerticalFlame1Tex, gFireTempleBigVerticalFlame2Tex,
    gFireTempleBigVerticalFlame3Tex, gFireTempleBigVerticalFlame4Tex, gFireTempleBigVerticalFlame5Tex,
    gFireTempleBigVerticalFlame6Tex, gFireTempleBigVerticalFlame7Tex,
};

static void draw_fire(PlayState* play, BgHidanRock* this) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_hidan_rock.c", 808);

    POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_20);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x01, 255, 255, 0, 150);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 255);

    if (this->type == 0) {
        Matrix_translate(goal_fire_pos.x, goal_fire_pos.y - 40.0f, goal_fire_pos.z, MTXMODE_NEW);
    } else {
        Matrix_translate(this->dyna.actor.home.pos.x, this->dyna.actor.home.pos.y - 40.0f, this->dyna.actor.home.pos.z,
                         MTXMODE_NEW);
    }

    Matrix_rotateXYZ(0, getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x8000, 0, MTXMODE_APPLY);
    Matrix_translate(-10.5f, 0.0f, 0.0f, MTXMODE_APPLY);
    Matrix_scale(6.0f, this->unk_16C, 6.0f, MTXMODE_APPLY);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(hibasira_txt[play->gameplayFrames & 7]));
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_hidan_rock.c", 853);
    gSPDisplayList(POLY_XLU_DISP++, gFireTempleBigVerticalFlameDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_hidan_rock.c", 857);
}

void Bg_Hidan_Rock_actor_draw(Actor* thisx, PlayState* play) {
    BgHidanRock* this = (BgHidanRock*)thisx;
    s32 pad;

    if (this->type == 0) {
        Cheap_gfx_display(play, gFireTempleStoneBlock1DL);
    } else {
        Cheap_gfx_display(play, gFireTempleStoneBlock2DL);
    }

    if (this->unk_16C > 0.0f) {
        if (this->type == 0) {
            Skin_Matrix_MulVector(&play->viewProjectionMtxF, &goal_fire_pos, &this->unk_170);
        } else {
            Skin_Matrix_MulVector(&play->viewProjectionMtxF, &this->dyna.actor.home.pos, &this->unk_170);
        }

        Na_StartObjectSe_F(&this->unk_170, NA_SE_EV_FIRE_PILLAR - SFX_FLAG);
        draw_fire(play, this);
    }
}
