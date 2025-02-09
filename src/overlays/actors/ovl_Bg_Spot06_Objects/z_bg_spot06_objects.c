/*
 * File: z_bg_spot06_objects.c
 * Overlay: ovl_Bg_Spot06_Objects
 * Description: Lake Hylia Objects
 */

#include "z_bg_spot06_objects.h"
#include "assets/objects/object_spot06_objects/object_spot06_objects.h"

#define FLAGS ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR

typedef enum LakeHyliaObjectsType {
    /* 0x0 */ LHO_WATER_TEMPLE_ENTRACE_GATE,
    /* 0x1 */ LHO_WATER_TEMPLE_ENTRANCE_LOCK,
    /* 0x2 */ LHO_WATER_PLANE,
    /* 0x3 */ LHO_ICE_BLOCK
} LakeHyliaObjectsType;

typedef enum LakeHyliaWaterBoxIndices {
    /* 0x0 */ LHWB_GERUDO_VALLEY_RIVER_UPPER, // entrance from Gerudo Valley
    /* 0x1 */ LHWB_GERUDO_VALLEY_RIVER_LOWER, // river flowing from Gerudo Valley
    /* 0x2 */ LHWB_MAIN_1,                    // main water box
    /* 0x3 */ LHWB_MAIN_2                     // extension of main water box
} LakeHyliaWaterBoxIndices;

// Lake Hylia water plane levels
#define WATER_LEVEL_RAISED (-1313)
#define WATER_LEVEL_RIVER_RAISED (WATER_LEVEL_RAISED + 200)
#define WATER_LEVEL_LOWERED (WATER_LEVEL_RAISED - 680)
#define WATER_LEVEL_RIVER_LOWERED (WATER_LEVEL_RIVER_RAISED - 80)

void Bg_Spot06_Objects_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot06_Objects_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot06_Objects_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot06_Objects_actor_draw(Actor* thisx, PlayState* play);
static void mode_gate_wait(BgSpot06Objects* this, PlayState* play);
void mode_gate_ready(BgSpot06Objects* this, PlayState* play);
void mode_gate_up(BgSpot06Objects* this, PlayState* play);
static void mode_stop(BgSpot06Objects* this, PlayState* play);
void mode_zou_wait(BgSpot06Objects* this, PlayState* play);
void mode_zou_carry(BgSpot06Objects* this, PlayState* play);
void mode_zou_up(BgSpot06Objects* this, PlayState* play);
void mode_zou_float(BgSpot06Objects* this, PlayState* play);
static void mode_water_wait(BgSpot06Objects* this, PlayState* play);
static void mode_water_down(BgSpot06Objects* this, PlayState* play);

ActorProfile Bg_Spot06_Objects_Profile = {
    /**/ ACTOR_BG_SPOT06_OBJECTS,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_SPOT06_OBJECTS,
    /**/ sizeof(BgSpot06Objects),
    /**/ Bg_Spot06_Objects_actor_ct,
    /**/ Bg_Spot06_Objects_actor_dt,
    /**/ Bg_Spot06_Objects_actor_move,
    /**/ Bg_Spot06_Objects_actor_draw,
};

static ColliderJntSphElementInit Spot06ObjAcOcJntSphElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000080, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 1, { { 0, 0, -160 }, 18 }, 100 },
    },
};

static ColliderJntSphInit Spot06ObjAcOcJntSphData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    Spot06ObjAcOcJntSphElemData,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static InitChainEntry value_init2[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void Bg_Spot06_Objects_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot06Objects* this = (BgSpot06Objects*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    this->switchFlag = PARAMS_GET_U(thisx->params, 0, 8);
    thisx->params = PARAMS_GET_U(thisx->params, 8, 8);

    PRINTF("spot06 obj nthisx->arg_data=[%d]", thisx->params);

    switch (thisx->params) {
        case LHO_WATER_TEMPLE_ENTRACE_GATE:
            ValueSet_process(thisx, value_init);
            MoveBG_ct(&this->dyna, 0);
            DynaPolyUty_bgdi_SG2KSG(&gLakeHyliaWaterTempleGateCol, &colHeader);
            this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);

            if (LINK_IS_ADULT && Actor_Environment_sw_Check(play, this->switchFlag)) {
                thisx->world.pos.y = thisx->home.pos.y + 120.0f;
                this->actionFunc = mode_stop;
            } else {
                this->actionFunc = mode_gate_wait;
            }

            if (1) {}

            break;
        case LHO_WATER_TEMPLE_ENTRANCE_LOCK:
            ValueSet_process(thisx, value_init);
            ClObjJntSph_ct(play, &this->collider);
            ClObjJntSph_set5_nzm(play, &this->collider, thisx, &Spot06ObjAcOcJntSphData, this->colliderItem);

            if (LINK_IS_ADULT && Actor_Environment_sw_Check(play, this->switchFlag)) {
                if (!GET_EVENTCHKINF(EVENTCHKINF_RESTORED_LAKE_HYLIA)) {
                    thisx->home.pos.y = thisx->world.pos.y = WATER_LEVEL_LOWERED;
                } else {
                    thisx->home.pos.y = thisx->world.pos.y = WATER_LEVEL_RAISED;
                }

                this->actionFunc = mode_zou_float;
                thisx->world.pos.z -= 100.0f;
                thisx->home.pos.z = thisx->world.pos.z + 16.0f;
                this->collider.elements[0].dim.worldSphere.radius =
                    this->collider.elements[0].dim.modelSphere.radius * 2;
                this->collider.elements[0].dim.worldSphere.center.z = thisx->world.pos.z + 16.0f;
            } else {
                this->actionFunc = mode_zou_wait;
                this->collider.elements[0].dim.worldSphere.radius = this->collider.elements[0].dim.modelSphere.radius;
                this->collider.elements[0].dim.worldSphere.center.z = thisx->world.pos.z;
            }

            this->collider.elements[0].dim.worldSphere.center.x = thisx->world.pos.x;
            this->collider.elements[0].dim.worldSphere.center.y = thisx->world.pos.y;
            thisx->colChkInfo.mass = MASS_IMMOVABLE;
            break;
        case LHO_WATER_PLANE:
            ValueSet_process(thisx, value_init2);
            thisx->flags = ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED;

            if (LINK_IS_ADULT && !GET_EVENTCHKINF(EVENTCHKINF_RESTORED_LAKE_HYLIA)) {
                if (!IS_CUTSCENE_LAYER) {
                    this->lakeHyliaWaterLevel = -681.0f;
                    play->colCtx.colHeader->waterBoxes[LHWB_GERUDO_VALLEY_RIVER_LOWER].ySurface =
                        WATER_LEVEL_RIVER_LOWERED;
                    play->colCtx.colHeader->waterBoxes[LHWB_GERUDO_VALLEY_RIVER_LOWER].zMin -= 50;
                    play->colCtx.colHeader->waterBoxes[LHWB_MAIN_1].ySurface = WATER_LEVEL_LOWERED;
                    play->colCtx.colHeader->waterBoxes[LHWB_MAIN_2].ySurface = WATER_LEVEL_LOWERED;
                    this->actionFunc = mode_stop;
                } else {
                    thisx->world.pos.y = this->lakeHyliaWaterLevel = -681.0f;
                    thisx->world.pos.y += WATER_LEVEL_RAISED;
                    this->actionFunc = mode_water_wait;
                }
            } else {
                this->lakeHyliaWaterLevel = 0.0f;
                this->actionFunc = mode_stop;
            }
            break;
        case LHO_ICE_BLOCK:
            ValueSet_process(thisx, value_init);
            MoveBG_ct(&this->dyna, 0);
            DynaPolyUty_bgdi_SG2KSG(&gLakeHyliaZoraShortcutIceblockCol, &colHeader);
            this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
            this->actionFunc = mode_stop;

            if (!LINK_IS_ADULT) {
                Actor_delete(thisx);
            }
            break;
    }
}

void Bg_Spot06_Objects_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot06Objects* this = (BgSpot06Objects*)thisx;

    switch (this->dyna.actor.params) {
        case LHO_WATER_TEMPLE_ENTRACE_GATE:
        case LHO_ICE_BLOCK:
            DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
            break;
        case LHO_WATER_TEMPLE_ENTRANCE_LOCK:
            ClObjJntSph_dt_nzf(play, &this->collider);
            break;
        case LHO_WATER_PLANE:
            break;
    }
}

/**
 * Water Temple entrance gate effect functions
 */
void spot06_gate_set_bubble(BgSpot06Objects* this, PlayState* play) {
    Vec3f sp34;
    f32 tmp;

    if ((play->gameplayFrames % 3) == 0) {
        tmp = rnd_fx(160.0f);
        sp34.x = (sin_s(this->dyna.actor.shape.rot.y + 0x4000) * tmp) + this->dyna.actor.world.pos.x;
        sp34.y = this->dyna.actor.world.pos.y;
        sp34.z = (cos_s(this->dyna.actor.shape.rot.y + 0x4000) * tmp) + this->dyna.actor.world.pos.z;
        Effect_SS_Bubble_ct(play, &sp34, 50.0f, 70.0f, 10.0f, (fqrand() * 0.05f) + 0.175f);
    }
}

/**
 * This is where the gate waits for the switch to be set by the fish shaped lock.
 */
static void mode_gate_wait(BgSpot06Objects* this, PlayState* play) {
    s32 i;

    if (Actor_Environment_sw_Check(play, this->switchFlag)) {
        this->timer = 100;
        this->dyna.actor.world.pos.y += 3.0f;
        this->actionFunc = mode_gate_ready;

        for (i = 0; i < 15; i++) {
            spot06_gate_set_bubble(this, play);
        }
    }
}

/**
 * This is where the gate waits a few frames before rising after the switch is set.
 */
void mode_gate_ready(BgSpot06Objects* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        this->actionFunc = mode_gate_up;
    }
}

/**
 * This is where the gate finally rises upward.
 */
void mode_gate_up(BgSpot06Objects* this, PlayState* play) {
    spot06_gate_set_bubble(this, play);

    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 120.0f, 0.6f)) {
        this->actionFunc = mode_stop;
        this->timer = 0;
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_STOP);
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_SLIDE - SFX_FLAG);
    }
}

static void mode_stop(BgSpot06Objects* this, PlayState* play) {
}

/**
 * Fish shaped lock effect functions
 */
void spot06_zou_set_ripple(BgSpot06Objects* this, PlayState* play, s32 flag) {
    if (flag || !(play->gameplayFrames % 7)) {
        Effect_SS_G_Ripple_ct2(play, &this->dyna.actor.home.pos, 300, 700, 0);
    }
}

void spot06_zou_set_bubble(BgSpot06Objects* this, PlayState* play, s32 flag) {
    if (!(play->gameplayFrames % 7) || flag) {
        Effect_SS_Bubble_ct(play, &this->dyna.actor.world.pos, 0.0f, 40.0f, 30.0f, (fqrand() * 0.05f) + 0.175f);
    }
}

/**
 * This is where the fish shaped lock waits to be pulled out by the hookshot. Once it does it will spawn bubbles.
 */
void mode_zou_wait(BgSpot06Objects* this, PlayState* play) {
    s32 pad;
    s32 i;
    s32 pad2;
    Vec3f effectPos;
    f32 sin;
    f32 cos;

    if (this->collider.base.acFlags & AC_HIT) {
        this->timer = 130;
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        sin = sin_s(this->dyna.actor.world.rot.y);
        cos = cos_s(this->dyna.actor.world.rot.y);
        this->dyna.actor.world.pos.x += (3.0f * sin);
        this->dyna.actor.world.pos.z += (3.0f * cos);

        for (i = 0; i < 20; i++) {
            spot06_zou_set_bubble(this, play, 1);
        }

        effectPos.x = this->dyna.actor.world.pos.x + (5.0f * sin);
        effectPos.y = this->dyna.actor.world.pos.y;
        effectPos.z = this->dyna.actor.world.pos.z + (5.0f * cos);

        for (i = 0; i < 3; i++) {
            Effect_SS_Bubble_ct(play, &effectPos, 0.0f, 20.0f, 20.0f, (fqrand() * 0.1f) + 0.7f);
        }

        Effect_SS_G_Splash_sc_cl_ct(play, &this->dyna.actor.world.pos, NULL, NULL, 1, 700);
        this->collider.elements[0].dim.worldSphere.radius = 45;
        this->actionFunc = mode_zou_carry;
        Nai_FxFlagEntry(NA_SE_SY_CORRECT_CHIME, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        Actor_Environment_sw_On(play, this->switchFlag);
        makeOnepointDemo(play, 4120, 170, &this->dyna.actor, CAM_ID_MAIN);
    } else {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
}

/**
 * Once the fish shaped lock is pulled out from the Hookshot it will move outward.
 */
void mode_zou_carry(BgSpot06Objects* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    this->dyna.actor.world.pos.x += (0.3f * sin_s(this->dyna.actor.world.rot.y));
    this->dyna.actor.world.pos.z += (0.3f * cos_s(this->dyna.actor.world.rot.y));
    spot06_zou_set_bubble(this, play, 0);

    if (this->timer == 0) {
        this->dyna.actor.velocity.y = 0.5f;
        this->dyna.actor.flags &= ~ACTOR_FLAG_HOOKSHOT_ATTACHED;

        this->actionFunc = mode_zou_up;
    }
}

/**
 * After being pulled all the way out the fish shaped lock will rise to the surface, creating bubbles in the water as it
 * does so.
 */
void mode_zou_up(BgSpot06Objects* this, PlayState* play) {
    f32 cos;
    f32 pad;

    this->dyna.actor.world.pos.y += this->dyna.actor.velocity.y;

    if (this->dyna.actor.velocity.y <= 0.0f) {
        cos = cos_s(this->dyna.actor.shape.rot.x) * 4.3f;
        this->dyna.actor.world.pos.x += (cos * sin_s(this->dyna.actor.shape.rot.y));
        this->dyna.actor.world.pos.z += (cos * cos_s(this->dyna.actor.shape.rot.y));
        this->dyna.actor.world.pos.y -= 1.3f;
        spot06_zou_set_ripple(this, play, 0);

        if (chase_angle(&this->dyna.actor.shape.rot.x, 0, 0x260) != 0) {
            this->dyna.actor.home.pos.x =
                this->dyna.actor.world.pos.x - (sin_s(this->dyna.actor.shape.rot.y) * 16.0f);
            this->dyna.actor.home.pos.z =
                this->dyna.actor.world.pos.z - (cos_s(this->dyna.actor.shape.rot.y) * 16.0f);
            this->dyna.actor.world.pos.y = -1993.0f;
            this->timer = 32;
            this->dyna.actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            this->collider.elements[0].dim.worldSphere.radius = this->collider.elements[0].dim.modelSphere.radius * 2;
            this->actionFunc = mode_zou_float;
        }
    } else {
        if (this->dyna.actor.world.pos.y >= -1973.0f) {
            this->dyna.actor.velocity.y = 0.0f;
            spot06_zou_set_ripple(this, play, 1);
            Effect_SS_G_Splash_sc_cl_ct(play, &this->dyna.actor.home.pos, NULL, NULL, 1, 700);
        } else if (this->dyna.actor.shape.rot.x == -0x4000) {
            this->dyna.actor.velocity.y += 0.02f;
            this->dyna.actor.world.pos.x = rnd_fx(1.0f) + this->dyna.actor.home.pos.x;
            this->dyna.actor.world.pos.z = rnd_fx(1.0f) + this->dyna.actor.home.pos.z;
            this->dyna.actor.velocity.y =
                (this->dyna.actor.velocity.y > 10.0f) ? (10.0f) : (this->dyna.actor.velocity.y);
            spot06_zou_set_bubble(this, play, 0);
        } else {
            spot06_zou_set_bubble(this, play, 0);

            if (chase_angle(&this->dyna.actor.shape.rot.x, -0x4000, 0x30)) {
                this->dyna.actor.home.pos.x = this->dyna.actor.world.pos.x;
                this->dyna.actor.home.pos.y = -1993.0f;
                this->dyna.actor.home.pos.z = this->dyna.actor.world.pos.z;
            }
        }
    }
}

/**
 * Once the fish shaped lock finishes rising to the surface it will float and create ripples in the water every few
 * frames.
 */
void mode_zou_float(BgSpot06Objects* this, PlayState* play) {
    spot06_zou_set_ripple(this, play, 0);

    if (this->timer != 0) {
        this->timer--;
    }

    this->dyna.actor.world.pos.y = (2.0f * sinf(this->timer * (M_PI / 16.0f))) + this->dyna.actor.home.pos.y;

    if (this->timer == 0) {
        this->timer = 32;
    }
}

void Bg_Spot06_Objects_actor_move(Actor* thisx, PlayState* play) {
    BgSpot06Objects* this = (BgSpot06Objects*)thisx;

    this->actionFunc(this, play);

    if (thisx->params == LHO_WATER_TEMPLE_ENTRANCE_LOCK) {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

/**
 * Draw the Lake Hylia water plane, and scroll its texture
 */
void Bg_Spot06_Suimen_draw(BgSpot06Objects* this, PlayState* play) {
    s32 pad;
    s32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot06_objects.c", 844);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_spot06_objects.c", 850);

    gameplayFrames = play->state.frames;

    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, -gameplayFrames, gameplayFrames, 32, 32, 1,
                                gameplayFrames, gameplayFrames, 32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, -gameplayFrames, gameplayFrames * 6, 32, 32, 1,
                                gameplayFrames, gameplayFrames * 6, 32, 32));

    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 128);

    if ((this->lakeHyliaWaterLevel < -680.0f) && !IS_CUTSCENE_LAYER) {
        gSPDisplayList(POLY_XLU_DISP++, gLakeHyliaLowWaterDL);
    } else {
        gSPDisplayList(POLY_XLU_DISP++, gLakeHyliaHighWaterDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot06_objects.c", 879);
}

void Bg_Spot06_Objects_actor_draw(Actor* thisx, PlayState* play) {
    BgSpot06Objects* this = (BgSpot06Objects*)thisx;

    switch (this->dyna.actor.params) {
        case LHO_WATER_TEMPLE_ENTRACE_GATE:
            Cheap_gfx_display(play, gLakeHyliaWaterTempleGateDL);
            break;
        case LHO_WATER_TEMPLE_ENTRANCE_LOCK:
            Cheap_gfx_display(play, gLakeHyliaWaterTempleKeyDL);

            if (this->actionFunc == mode_zou_up) {
                CollisionCheck_Uty_convJntSphL2G(1, &this->collider);
            }
            break;
        case LHO_WATER_PLANE:
            Bg_Spot06_Suimen_draw(this, play);
            break;
        case LHO_ICE_BLOCK:
            Cheap_gfx_display(play, gLakeHyliaZoraShortcutIceblockDL);
            break;
    }
}

/**
 * This is where the Lake Hylia water plane waits for the cutscene to set the water risen flag after the Water Temple is
 * cleared.
 */
static void mode_water_wait(BgSpot06Objects* this, PlayState* play) {
    if (GET_EVENTCHKINF(EVENTCHKINF_RESTORED_LAKE_HYLIA)) {
        this->actionFunc = mode_water_down;
    }
}

/**
 * This is where the Lake Hylia water plane rises in the cutscene after the Water Temple is cleared.
 */
static void mode_water_down(BgSpot06Objects* this, PlayState* play) {
    s32 pad;

    this->dyna.actor.world.pos.y = this->lakeHyliaWaterLevel + WATER_LEVEL_RAISED;

    if (this->lakeHyliaWaterLevel >= 0.0001f) {
        this->dyna.actor.world.pos.y = WATER_LEVEL_RAISED;
        this->actionFunc = mode_stop;
    } else {
        add_calc(&this->lakeHyliaWaterLevel, 1.0f, 0.1f, 1.0f, 0.001f);
        play->colCtx.colHeader->waterBoxes[LHWB_GERUDO_VALLEY_RIVER_LOWER].ySurface = WATER_LEVEL_RIVER_LOWERED;
        play->colCtx.colHeader->waterBoxes[LHWB_MAIN_1].ySurface = this->dyna.actor.world.pos.y;
        play->colCtx.colHeader->waterBoxes[LHWB_MAIN_2].ySurface = this->dyna.actor.world.pos.y;
    }

    Actor_fix_level_SE_set(&this->dyna.actor, NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
}
