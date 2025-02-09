/*
 * File: z_bg_gnd_soulmeiro.c
 * Overlay: ovl_Bg_Gnd_Soulmeiro
 * Description: Web-Blocked Ceiling Hole (Inside Ganon's Castle)
 */

#include "z_bg_gnd_soulmeiro.h"

#include "libc64/qrand.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "rand.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"

#include "assets/objects/object_demo_kekkai/object_demo_kekkai.h"

#define FLAGS 0

void Bg_Gnd_Soulmeiro_Actor_ct(Actor* thisx, PlayState* play);
void Bg_Gnd_Soulmeiro_Actor_dt(Actor* thisx, PlayState* play);
void Bg_Gnd_Soulmeiro_Actor_move(Actor* thisx, PlayState* play);
void Bg_Gnd_Soulmeiro_Actor_draw(Actor* thisx, PlayState* play);

void roof_burn(BgGndSoulmeiro* this, PlayState* play);
static void move_wait(BgGndSoulmeiro* this, PlayState* play);
static void move_wait2(BgGndSoulmeiro* this, PlayState* play);

ActorProfile Bg_Gnd_Soulmeiro_Profile = {
    /**/ ACTOR_BG_GND_SOULMEIRO,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_DEMO_KEKKAI,
    /**/ sizeof(BgGndSoulmeiro),
    /**/ Bg_Gnd_Soulmeiro_Actor_ct,
    /**/ Bg_Gnd_Soulmeiro_Actor_dt,
    /**/ Bg_Gnd_Soulmeiro_Actor_move,
    /**/ Bg_Gnd_Soulmeiro_Actor_draw,
};

static ColliderCylinderInit SoulmeiroOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0x00020800, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 50, 20, 20, { 0, 0, 0 } },
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

void Bg_Gnd_Soulmeiro_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgGndSoulmeiro* this = (BgGndSoulmeiro*)thisx;

    ValueSet_process(&this->actor, value_init);
    this->actionFunc = NULL;

    switch (PARAMS_GET_U(this->actor.params, 0, 8)) {
        case 0:
            ClObjPipe_ct(play, &this->collider);
            ClObjPipe_set5(play, &this->collider, &this->actor, &SoulmeiroOcInfoData);
            this->actionFunc = move_wait;
            if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 8, 6))) {

                Actor_info_make_actor(&play->actorCtx, play, ACTOR_MIR_RAY, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, 0, 0, 0, 9);
                this->actor.draw = NULL;
                Actor_delete(&this->actor);
                return;
            } else {
                this->actor.draw = Bg_Gnd_Soulmeiro_Actor_draw;
            }
            break;
        case 1:
        case 2:
            if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 8, 6))) {
                this->actor.draw = Bg_Gnd_Soulmeiro_Actor_draw;
            } else {
                this->actor.draw = NULL;
            }
            this->actionFunc = move_wait2;
            break;
    }
}

void Bg_Gnd_Soulmeiro_Actor_dt(Actor* thisx, PlayState* play) {
    BgGndSoulmeiro* this = (BgGndSoulmeiro*)thisx;

    if (PARAMS_GET_U(this->actor.params, 0, 8) == 0) {
        ClObjPipe_dt(play, &this->collider);
    }
}

void roof_burn(BgGndSoulmeiro* this, PlayState* play) {
    static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };
    Vec3f vecA;
    Vec3f vecB;
    Actor* thisx = &this->actor;

    if (this->unk_198 != 0) {
        this->unk_198--;
    }

    if (this->unk_198 == 20) {
        Actor_Environment_sw_On(play, PARAMS_GET_U(thisx->params, 8, 6));
        thisx->draw = NULL;
    }

    // This should be this->unk_198 == 0, this is required to match
    if (!this->unk_198) {
        Actor_Environment_sw_On(play, PARAMS_GET_U(thisx->params, 8, 6));
        Actor_delete(&this->actor);
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_MIR_RAY, thisx->world.pos.x, thisx->world.pos.y, thisx->world.pos.z, 0,
                    0, 0, 9);
    } else if ((this->unk_198 % 6) == 0) {
        s32 i;
        s16 temp_2 = fqrand() * (10922.0f); // This should be: 0x10000 / 6.0f
        s16 temp_1;
        f32 temp_3;
        f32 temp_4;
        f32 distXZ;

        vecA.y = 0.0f;
        vecB.y = thisx->world.pos.y;

        for (i = 0; i < 6; i++) {
            temp_1 = rnd_fx(0x2800) + temp_2;
            temp_3 = sin_s(temp_1);
            temp_4 = cos_s(temp_1);

            vecB.x = thisx->world.pos.x + (120.0f * temp_3);
            vecB.z = thisx->world.pos.z + (120.0f * temp_4);
            distXZ = search_position_distanceXZ(&thisx->home.pos, &vecB) * (1.0f / 120.0f);
            if (distXZ < 0.7f) {
                temp_3 = sin_s(temp_1 + 0x8000);
                temp_4 = cos_s(temp_1 + 0x8000);
                vecB.x = thisx->world.pos.x + (120.0f * temp_3);
                vecB.z = thisx->world.pos.z + (120.0f * temp_4);
                distXZ = search_position_distanceXZ(&thisx->home.pos, &vecB) * (1.0f / 120.0f);
            }

            vecA.x = 4.0f * temp_3 * distXZ;
            vecA.y = 0.0f;
            vecA.z = 4.0f * temp_4 * distXZ;
            _Effect_SS_Db_ct(play, &thisx->home.pos, &vecA, &zero_vec, 60, 6, 255, 255, 150, 170, 255, 0, 0, 1, 14,
                                 true);
            temp_2 += 0x2AAA;
        }
    }
}

static void move_wait(BgGndSoulmeiro* this, PlayState* play) {
    s32 pad;

    if (!Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 8, 6))) {
        this->actor.draw = Bg_Gnd_Soulmeiro_Actor_draw;
        if (this->collider.base.acFlags & AC_HIT) {
            Nai_FxFlagEntry(NA_SE_SY_CORRECT_CHIME, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            this->unk_198 = 40;
            this->actionFunc = roof_burn;
        } else {
            CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

static void move_wait2(BgGndSoulmeiro* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 8, 6))) {
        this->actor.draw = Bg_Gnd_Soulmeiro_Actor_draw;
    } else {
        this->actor.draw = NULL;
    }
}

void Bg_Gnd_Soulmeiro_Actor_move(Actor* thisx, PlayState* play) {
    BgGndSoulmeiro* this = (BgGndSoulmeiro*)thisx;

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void Bg_Gnd_Soulmeiro_Actor_draw(Actor* thisx, PlayState* play) {
    static Gfx* shape_model[] = {
        gSpiritTrialWebDL,
        gSpiritTrialLightSourceDL,
        gSpiritTrialLightFloorDL,
    };
    s32 params = PARAMS_GET_U(thisx->params, 0, 8);

    if (1) {}

    switch (params) {
        case 0:
            OPEN_DISPS(play->state.gfxCtx, "../z_bg_gnd_soulmeiro.c", 398);
            _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_gnd_soulmeiro.c", 400);
            gSPDisplayList(POLY_XLU_DISP++, shape_model[params]);
            CLOSE_DISPS(play->state.gfxCtx, "../z_bg_gnd_soulmeiro.c", 403);
            break;
        case 1:
            Cheap_gfx_display_xlu(play, shape_model[params]);
            break;
        case 2:
            Cheap_gfx_display(play, shape_model[params]);
            break;
    }
}
