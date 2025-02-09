/*
 * File: z_bg_dodoago.c
 * Overlay: ovl_Bg_Dodoago
 * Description: Dodongo Head Statue in Dodongo's Cavern
 */

#include "z_bg_dodoago.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"

#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "macros.h"
#include "one_point_cutscene.h"
#include "rand.h"
#include "rumble.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"

#include "assets/objects/object_ddan_objects/object_ddan_objects.h"

#define FLAGS 0

void Bg_Dodoago_actor_ct(Actor* thisx, PlayState* play);
void Bg_Dodoago_actor_dt(Actor* thisx, PlayState* play);
void Bg_Dodoago_actor_move(Actor* thisx, PlayState* play);
void Bg_Dodoago_actor_draw(Actor* thisx, PlayState* play);

static void move_wait(BgDodoago* this, PlayState* play);
static void move_open(BgDodoago* this, PlayState* play);
static void move_open_wait(BgDodoago* this, PlayState* play);
void move_flash(BgDodoago* this, PlayState* play);

ActorProfile Bg_Dodoago_Profile = {
    /**/ ACTOR_BG_DODOAGO,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_DDAN_OBJECTS,
    /**/ sizeof(BgDodoago),
    /**/ Bg_Dodoago_actor_ct,
    /**/ Bg_Dodoago_actor_dt,
    /**/ Bg_Dodoago_actor_move,
    /**/ Bg_Dodoago_actor_draw,
};

static ColliderCylinderInit AcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ALL,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 80, 30, 80, { 0, 0, 0 } },
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 50, 60, 280, { 0, 0, 0 } },
};

static s16 c_no = false;

static u8 demo_idx;

static u8 demo_camera_eye[90]; // unknown length

static s32 demo_time;

void Bg_Dodoago_actor_set_process(BgDodoago* this, BgDodoagoActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

static void eff_set(Vec3f* meanPos, PlayState* play) {
    Vec3f pos;
    Color_RGBA8 primColor = { 100, 100, 100, 0 };
    Color_RGBA8 envColor = { 40, 40, 40, 0 };
    static Vec3f kirakira_vec = { 0.0f, -1.5f, 0.0f };
    static Vec3f kirakira_acc = { 0.0f, -0.2f, 0.0f };
    s32 i;

    for (i = 4; i > 0; i--) {
        pos.x = rnd_fx(20.0f) + meanPos->x;
        pos.y = rnd_fx(10.0f) + meanPos->y;
        pos.z = rnd_fx(20.0f) + meanPos->z;
        Effect_SS_KiraKira_ct(play, &pos, &kirakira_vec, &kirakira_acc, &primColor, &envColor);
    }
}

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 5000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 800, ICHAIN_STOP),
};

void Bg_Dodoago_actor_ct(Actor* thisx, PlayState* play) {
    BgDodoago* this = (BgDodoago*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gDodongoLowerJawCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    Shape_Info_init(&this->dyna.actor.shape, 0.0f, NULL, 0.0f);

    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
        Bg_Dodoago_actor_set_process(this, move_open_wait);
        this->dyna.actor.shape.rot.x = 0x1333;
        play->roomCtx.drawParams[BGDODOAGO_EYE_LEFT] = play->roomCtx.drawParams[BGDODOAGO_EYE_RIGHT] = 255;
        return;
    }

    ClObjPipe_ct(play, &this->colliderMain);
    ClObjPipe_ct(play, &this->colliderLeft);
    ClObjPipe_ct(play, &this->colliderRight);
    ClObjPipe_set5(play, &this->colliderMain, &this->dyna.actor, &AcInfoData);
    ClObjPipe_set5(play, &this->colliderLeft, &this->dyna.actor, &OcInfoData);
    ClObjPipe_set5(play, &this->colliderRight, &this->dyna.actor, &OcInfoData);

    Bg_Dodoago_actor_set_process(this, move_wait);
    demo_idx = false;
}

void Bg_Dodoago_actor_dt(Actor* thisx, PlayState* play) {
    BgDodoago* this = (BgDodoago*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    ClObjPipe_dt(play, &this->colliderMain);
    ClObjPipe_dt(play, &this->colliderLeft);
    ClObjPipe_dt(play, &this->colliderRight);
}

static void move_wait(BgDodoago* this, PlayState* play) {
    Actor* explosive = BlastVsMyCheck(play, &this->colliderMain.base);

    if (explosive != NULL) {
        this->state =
            (search_position_angleY(&this->dyna.actor.world.pos, &explosive->world.pos) >= this->dyna.actor.shape.rot.y)
                ? BGDODOAGO_EYE_RIGHT
                : BGDODOAGO_EYE_LEFT;

        if (((play->roomCtx.drawParams[BGDODOAGO_EYE_LEFT] == 255) && (this->state == BGDODOAGO_EYE_RIGHT)) ||
            ((play->roomCtx.drawParams[BGDODOAGO_EYE_RIGHT] == 255) && (this->state == BGDODOAGO_EYE_LEFT))) {
            Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6));
            this->state = 0;
            Nai_FxFlagEntry(NA_SE_SY_CORRECT_CHIME, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            Bg_Dodoago_actor_set_process(this, move_open);
            makeOnepointDemo(play, 3380, 160, &this->dyna.actor, CAM_ID_MAIN);
        } else if (play->roomCtx.drawParams[this->state] == 0) {
            makeOnepointDemo(play, 3065, 40, &this->dyna.actor, CAM_ID_MAIN);
            Bg_Dodoago_actor_set_process(this, move_flash);
            Nai_FxFlagEntry(NA_SE_SY_CORRECT_CHIME, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        } else {
            makeOnepointDemo(play, 3065, 20, &this->dyna.actor, CAM_ID_MAIN);
            Nai_FxFlagEntry(NA_SE_SY_ERROR, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            demo_time += 30;
            return;
        }

        // the flag is never set back to false, so this only runs once
        if (!c_no) {
            // this disables the bomb catcher (see Bg_Dodoago_actor_move) for a few seconds
            this->dyna.actor.parent = explosive;
            c_no = true;
            demo_time = 50;
        }
    } else if (event_check(EVENTCHKINF_B0)) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, &this->colliderMain);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, &this->colliderLeft);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, &this->colliderRight);

        this->colliderMain.dim.pos.z += 200;

        this->colliderLeft.dim.pos.z += 215;
        this->colliderLeft.dim.pos.x += 90;

        this->colliderRight.dim.pos.z += 215;
        this->colliderRight.dim.pos.x -= 90;

        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderMain.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderLeft.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderRight.base);
    }
}

static void move_open(BgDodoago* this, PlayState* play) {
    Vec3f pos;
    Vec3f dustOffsets[] = {
        { 0.0f, -200.0f, 430.0f },   { 20.0f, -200.0f, 420.0f }, { -20.0f, -200.0f, 420.0f },
        { 40.0, -200.0f, 380.0f },   { -40.0, -200.0f, 380.0f }, { 50.0, -200.0f, 350.0f },
        { -50.0f, -200.0f, 350.0f }, { 60.0f, -200.0f, 320.0f }, { -60.0f, -200.0f, 320.0f },
        { 70.0f, -200.0f, 290.0f },  { -70.0, -200.0f, 290.0f },
    };
    s32 i;

    // make both eyes red (one already is)
    if (play->roomCtx.drawParams[BGDODOAGO_EYE_LEFT] < 255) {
        play->roomCtx.drawParams[BGDODOAGO_EYE_LEFT] += 5;
    }
    if (play->roomCtx.drawParams[BGDODOAGO_EYE_RIGHT] < 255) {
        play->roomCtx.drawParams[BGDODOAGO_EYE_RIGHT] += 5;
    }

    if (play->roomCtx.drawParams[BGDODOAGO_EYE_LEFT] != 255 || play->roomCtx.drawParams[BGDODOAGO_EYE_RIGHT] != 255) {
        demo_time--;
        return;
    }

    if (demo_time == 108) {
        for (i = ARRAY_COUNT(dustOffsets) - 1; i >= 0; i--) {
            pos.x = dustOffsets[i].x + this->dyna.actor.world.pos.x;
            pos.y = dustOffsets[i].y + this->dyna.actor.world.pos.y;
            pos.z = dustOffsets[i].z + this->dyna.actor.world.pos.z;
            dust_fly_set2(play, &pos, 2.0f, 3, 200, 75, 1);
        }
    }

    pos.x = this->dyna.actor.world.pos.x + 200.0f;
    pos.y = this->dyna.actor.world.pos.y - 20.0f;
    pos.z = this->dyna.actor.world.pos.z + 100.0f;
    eff_set(&pos, play);

    pos.x = this->dyna.actor.world.pos.x - 200.0f;
    pos.y = this->dyna.actor.world.pos.y - 20.0f;
    pos.z = this->dyna.actor.world.pos.z + 100.0f;
    eff_set(&pos, play);

    chase_s(&this->state, 100, 3);
    z_vibctl2_vib_setQ(500.0f, 120, 20, 10);

    if (add_calc_short_angle2(&this->dyna.actor.shape.rot.x, 0x1333, 110 - this->state, 0x3E8, 0x32) == 0) {
        Bg_Dodoago_actor_set_process(this, move_open_wait);
        Nai_FxFlagEntry(NA_SE_EV_STONE_BOUND, &this->dyna.actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else {
        Nai_FxFlagEntry(NA_SE_EV_STONE_STATUE_OPEN - SFX_FLAG, &this->dyna.actor.projectedPos, 4,
                             &_dummy_one, &_dummy_one, &_dummy_zero_s8);
    }
}

static void move_open_wait(BgDodoago* this, PlayState* play) {
}

void move_flash(BgDodoago* this, PlayState* play) {
    play->roomCtx.drawParams[this->state] += 5;

    if (play->roomCtx.drawParams[this->state] == 255) {
        Bg_Dodoago_actor_set_process(this, move_wait);
    }
}

void Bg_Dodoago_actor_move(Actor* thisx, PlayState* play) {
    BgDodoago* this = (BgDodoago*)thisx;
    Actor* actor;
    EnBom* bomb;

    if (this->dyna.actor.parent == NULL) {
        // this is a "bomb catcher", it kills the XZ speed and sets the timer for bombs that are dropped through the
        // holes in the bridge above the skull
        if ((this->colliderLeft.base.ocFlags1 & OC1_HIT) || (this->colliderRight.base.ocFlags1 & OC1_HIT)) {

            if (this->colliderLeft.base.ocFlags1 & OC1_HIT) {
                actor = this->colliderLeft.base.oc;
            } else {
                actor = this->colliderRight.base.oc;
            }
            this->colliderLeft.base.ocFlags1 &= ~OC1_HIT;
            this->colliderRight.base.ocFlags1 &= ~OC1_HIT;

            if (actor->category == ACTORCAT_EXPLOSIVE && actor->id == ACTOR_EN_BOM && actor->params == 0) {
                bomb = (EnBom*)actor;
                // disable the bomb catcher for a few seconds
                this->dyna.actor.parent = &bomb->actor;
                bomb->timer = 50;
                bomb->actor.speed = 0.0f;
                demo_time = 0;
            }
        }
    } else {
        demo_time++;
        Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6));
        if (!demo_idx && demo_time > 140) {
            if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
                // this prevents clearing the actor's parent pointer, effectively disabling the bomb catcher
                demo_idx++;
            } else {
                this->dyna.actor.parent = NULL;
            }
        }
    }
    this->actionFunc(this, play);
}

void Bg_Dodoago_actor_draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_bg_dodoago.c", 672);

    if (event_check(EVENTCHKINF_B0)) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_dodoago.c", 677);
        gSPDisplayList(POLY_OPA_DISP++, gDodongoLowerJawDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_dodoago.c", 681);
}
