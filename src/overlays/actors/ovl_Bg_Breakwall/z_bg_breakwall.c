/*
 * File: z_bg_breakwall.c
 * Overlay: Bg_Breakwall
 * Description: Bombable Wall
 */

#include "z_bg_breakwall.h"

#include "libc64/qrand.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "rand.h"
#include "regs.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/scenes/dungeons/ddan/ddan_scene.h"
#include "assets/objects/object_bwall/object_bwall.h"
#include "assets/objects/object_kingdodongo/object_kingdodongo.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef struct BombableWallInfo {
    /* 0x00 */ CollisionHeader* colHeader;
    /* 0x04 */ Gfx* dList;
    /* 0x08 */ s8 colType;
} BombableWallInfo;

void Bg_Breakwall_actor_ct(Actor* thisx, PlayState* play);
void Bg_Breakwall_actor_dt(Actor* thisx, PlayState* play);
void Bg_Breakwall_actor_move(Actor* thisx, PlayState* play);
void Bg_Breakwall_actor_draw(Actor* thisx, PlayState* play);

static void move_dma_wait(BgBreakwall* this, PlayState* play);
static void move_wait(BgBreakwall* this, PlayState* play);
void move_boss_dodongo(BgBreakwall* this, PlayState* play);

ActorProfile Bg_Breakwall_Profile = {
    /**/ ACTOR_BG_BREAKWALL,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(BgBreakwall),
    /**/ Bg_Breakwall_actor_ct,
    /**/ Bg_Breakwall_actor_dt,
    /**/ Bg_Breakwall_actor_move,
    /**/ NULL,
};

static ColliderQuadInit AcSwordInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_OTHER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000048, 0x00, 0x00 },
        { 0x00000048, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static BombableWallInfo bg_break_status[] = {
    { &object_bwall_Col_000118, object_bwall_DL_000040, 0 },
    { &object_bwall_Col_000118, object_bwall_DL_000040, 0 },
    { &object_kingdodongo_Col_0264A8, object_kingdodongo_DL_025BD0, 1 },
    { &object_kingdodongo_Col_025B64, NULL, -1 },
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 400, ICHAIN_STOP),
};

void Bg_Breakwall_actor_set_process(BgBreakwall* this, BgBreakwallActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Bg_Breakwall_actor_ct(Actor* thisx, PlayState* play) {
    BgBreakwall* this = (BgBreakwall*)thisx;
    s32 pad;
    s32 wallType = (u8)PARAMS_GET_U(this->dyna.actor.params, 13, 2);

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, 0);
    this->bombableWallDList = bg_break_status[wallType].dList;
    this->colType = bg_break_status[wallType].colType;

    if (this->colType == 1) {
        this->dyna.actor.world.rot.x = 0x4000;
    }

    if (this->bombableWallDList != NULL) {
        if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
            Actor_delete(&this->dyna.actor);
            return;
        }

        Shape_Info_init(&this->dyna.actor.shape, 0.0f, NULL, 0.0f);
        ClObjSwrd_ct(play, &this->collider);
        ClObjSwrd_set5(play, &this->collider, &this->dyna.actor, &AcSwordInfoData);
    } else {
        this->dyna.actor.world.pos.y -= 40.0f;
    }

    this->requiredObjectSlot = (wallType >= BWALL_KD_FLOOR) ? Object_Exchange_bank_check(&play->objectCtx, OBJECT_KINGDODONGO)
                                                            : Object_Exchange_bank_check(&play->objectCtx, OBJECT_BWALL);

    if (this->requiredObjectSlot < 0) {
        Actor_delete(&this->dyna.actor);
    } else {
        Bg_Breakwall_actor_set_process(this, move_dma_wait);
    }
}

void Bg_Breakwall_actor_dt(Actor* thisx, PlayState* play) {
    BgBreakwall* this = (BgBreakwall*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

/**
 * Spawns fragments using ACTOR_EN_A_OBJ whenever the wall or floor is exploded.
 * Returns the last spawned actor
 */
Actor* hahen_set3(PlayState* play, BgBreakwall* this, Vec3f* pos, f32 velocity, f32 scaleY, f32 scaleX,
                                  s32 count, f32 accel) {
    Actor* actor;
    Vec3f actorPos;
    s32 k;
    s32 j;
    s32 i;
    s16 angle1;
    s16 angle2 = 0;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f }; // unused
    Vec3s actorRotList[] = { { 0, 0, 0 }, { 0, 0, 0x4000 }, { 0, 0, -0x4000 }, { 0, 0, 0 } };
    Vec3f actorScaleList[] = {
        { 0.004f, 0.004f, 0.004f },
        { 0.004f, 0.004f, 0.004f },
        { 0.004f, 0.004f, 0.004f },
        { 0.004f, 0.004f, 0.004f },
    };
    Vec3f actorPosList[][4] = {
        { { 40.0f, 15.0f, 0.0f }, { 30.0f, 57.0f, 0.0f }, { 50.0f, 57.0f, 0.0f }, { 40.0f, 70.0f, 0.0f } },
        { { 55.0f, -15.0f, 0.0f }, { 30.0f, -32.0f, 0.0f }, { 50.0f, -32.0f, 0.0f }, { 20.0f, -10.0f, 0.0f } },
        { { -40.0f, 14.0f, 0.0f }, { -50.0f, 57.0f, 0.0f }, { -30.0f, 57.0f, 0.0f }, { -40.0f, 70.0f, 0.0f } },
        { { -55.0f, -15.0f, 0.0f }, { -55.0f, -32.0f, 0.0f }, { -30.0f, -32.0f, 0.0f }, { -20.0f, -10.0f, 0.0f } },
    };
    s32 pad;

    for (k = 3; k >= 0; k--) {
        if ((k == 0) || (k == 3)) {
            actorScaleList[k].x *= scaleX;
            actorScaleList[k].y *= scaleY;
            actorScaleList[k].z *= scaleY;
        } else {
            actorScaleList[k].x *= scaleY;
            actorScaleList[k].y *= scaleX;
            actorScaleList[k].z *= scaleX;
        }
    }

    for (i = 0; i < count; angle2 += 0x4000, i++) {
        angle1 = ABS(this->dyna.actor.world.rot.y) + angle2;
        Matrix_translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                         MTXMODE_NEW);
        Matrix_rotateXYZ(this->dyna.actor.world.rot.x, this->dyna.actor.world.rot.y, this->dyna.actor.world.rot.z,
                         MTXMODE_APPLY);
        Matrix_translate(pos->x, pos->y, pos->z, MTXMODE_APPLY);

        for (j = 3; j >= 0; j--) {
            for (k = 3; k >= 0; k--) {
                Matrix_Position(&actorPosList[j][k], &actorPos);
                actor = Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_A_OBJ, rnd_fx(20.0f) + actorPos.x,
                                    rnd_fx(20.0f) + actorPos.y, rnd_fx(20.0f) + actorPos.z,
                                    actorRotList[k].x, actorRotList[k].y + angle1, actorRotList[k].z, 0x000B);

                if ((j & 1) == 0) {
                    dust_fly_set2(play, &actorPos, velocity * 200.0f, 1, 650, 150, 1);
                }

                if (actor != NULL) {
                    actor->speed = fqrand() + (accel * 0.6f);
                    actor->velocity.y = fqrand() + (accel * 0.6f);
                    actor->world.rot.y += (s16)((fqrand() - 0.5f) * 3000.0f);
                    actor->world.rot.x = (s16)(fqrand() * 3500.0f) + 2000;
                    actor->world.rot.z = (s16)(fqrand() * 3500.0f) + 2000;
                    actor->parent = &this->dyna.actor;
                    actor->scale.x = actorScaleList[k].x + rnd_fx(0.001f);
                    actor->scale.y = actorScaleList[k].y + rnd_fx(0.001f);
                    actor->scale.z = actorScaleList[k].z + rnd_fx(0.001f);
                }
            }
        }
    }

    return actor;
}

/**
 * Sets up the collision model as well is the object dependency and action function to use.
 */
static void move_dma_wait(BgBreakwall* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        CollisionHeader* colHeader = NULL;
        s32 wallType = (u8)PARAMS_GET_U(this->dyna.actor.params, 13, 2);

        this->dyna.actor.objectSlot = this->requiredObjectSlot;
        Actor_set_segment(play, &this->dyna.actor);
        this->dyna.actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->dyna.actor.draw = Bg_Breakwall_actor_draw;
        DynaPolyUty_bgdi_SG2KSG(bg_break_status[wallType].colHeader, &colHeader);
        this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

        if (wallType == BWALL_KD_LAVA_COVER) {
            Bg_Breakwall_actor_set_process(this, move_boss_dodongo);
        } else {
            Bg_Breakwall_actor_set_process(this, move_wait);
        }
    }
}

/**
 * Checks for an explosion using quad collision. If the wall or floor is exploded then it will spawn fragments and
 * despawn itself.
 */
static void move_wait(BgBreakwall* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        Vec3f effectPos;
        s32 wallType = (u8)PARAMS_GET_U(this->dyna.actor.params, 13, 2);

        DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
        effectPos.y = effectPos.z = effectPos.x = 0.0f;

        if (this->dyna.actor.world.rot.x == 0) {
            effectPos.y = 55.0f;
        } else {
            effectPos.z = 25.0f;
            effectPos.y = -10.0f;
        }

        hahen_set3(play, this, &effectPos, 0.0f, 6.4f, 5.0f, 1, 2.0f);
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6));

        if (wallType == BWALL_KD_FLOOR) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_EXPLOSION);
        } else {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_WALL_BROKEN);
        }

        if ((wallType == BWALL_DC_ENTRANCE) && !event_check(EVENTCHKINF_B0)) {
            event_set(EVENTCHKINF_B0);
            set_showdata(play, gDcOpeningCs);
            z_common_data.cutsceneTrigger = 1;
            Nai_FxFlagEntry(NA_SE_SY_CORRECT_CHIME, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_49);
        }

        if (this->dyna.actor.params < 0) {
            Nai_FxFlagEntry(NA_SE_SY_TRE_BOX_APPEAR, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }

        Actor_delete(&this->dyna.actor);
    }
}

/**
 * Moves the actor's y position to cover the lava floor in King Dodongo's lair after he is defeated so the player is no
 * longer hurt by the lava.
 */
void move_boss_dodongo(BgBreakwall* this, PlayState* play) {
    chase_f(&this->dyna.actor.world.pos.y, KREG(80) + this->dyna.actor.home.pos.y, 1.0f);
}

void Bg_Breakwall_actor_move(Actor* thisx, PlayState* play) {
    BgBreakwall* this = (BgBreakwall*)thisx;

    this->actionFunc(this, play);
}

/**
 * These are the quads used for the wall and floor collision. These are used for the detecting when a bomb explosion has
 * collided with a wall, and can be adjusted for different wall or floor sizes.
 */
static Vec3f collision_data[][4] = {
    { { 800.0f, 1600.0f, 100.0f }, { -800.0f, 1600.0f, 100.0f }, { 800.0f, 0.0f, 100.0f }, { -800.0f, 0.0f, 100.0f } },
    { { 10.0f, 0.0f, 10.0f }, { -10.0f, 0.0f, 10.0f }, { 10.0f, 0.0f, -10.0f }, { -10.0f, 0.0f, -10.0f } },
};

void Bg_Breakwall_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgBreakwall* this = (BgBreakwall*)thisx;

    if (this->bombableWallDList != NULL) {
        OPEN_DISPS(play->state.gfxCtx, "../z_bg_breakwall.c", 767);

        _texture_z_light_fog_prim(play->state.gfxCtx);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_breakwall.c", 771);
        gSPDisplayList(POLY_OPA_DISP++, this->bombableWallDList);

        if (this->colType >= 0) {
            Vec3f colQuad[4];
            Vec3f* src = &collision_data[this->colType][0];
            Vec3f* dst = &colQuad[0];
            s32 i;

            for (i = 0; i < 4; i++) {
                Matrix_Position(src++, dst++);
            }

            CollisionCheck_Uty_setSword4Pos(&this->collider, &colQuad[0], &colQuad[1], &colQuad[2], &colQuad[3]);
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }

        CLOSE_DISPS(play->state.gfxCtx, "../z_bg_breakwall.c", 822);
    }
}
