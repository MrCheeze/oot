/*
 * File: z_bg_hidan_kowarerukabe.c
 * Overlay: ovl_Bg_Hidan_Kowarerukabe
 * Description: Fire Temple Bombable Walls and Floors
 */

#include "z_bg_hidan_kowarerukabe.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"

#include "libc64/qrand.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"

#include "assets/objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "assets/objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS 0

typedef enum FireTempleBombableObjectsType {
    /* 0 */ CRACKED_STONE_FLOOR,
    /* 1 */ BOMBABLE_WALL,
    /* 2 */ LARGE_BOMBABLE_WALL
} FireTempleBombableObjectsType;

void Bg_Hidan_Kowarerukabe_actor_ct(Actor* thisx, PlayState* play);
void Bg_Hidan_Kowarerukabe_actor_dt(Actor* thisx, PlayState* play);
void Bg_Hidan_Kowarerukabe_actor_move(Actor* thisx, PlayState* play);
void Bg_Hidan_Kowarerukabe_actor_draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Hidan_Kowarerukabe_Profile = {
    /**/ ACTOR_BG_HIDAN_KOWARERUKABE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HIDAN_OBJECTS,
    /**/ sizeof(BgHidanKowarerukabe),
    /**/ Bg_Hidan_Kowarerukabe_actor_ct,
    /**/ Bg_Hidan_Kowarerukabe_actor_dt,
    /**/ Bg_Hidan_Kowarerukabe_actor_move,
    /**/ Bg_Hidan_Kowarerukabe_actor_draw,
};

static Gfx* shapeDtTbl[] = {
    gFireTempleCrackedStoneFloorDL,
    gFireTempleBombableWallDL,
    gFireTempleLargeBombableWallDL,
};

static ColliderJntSphElementInit CrossSphElemDt_base[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 100 }, 100 },
    },
};

static ColliderJntSphInit CrossSphDt_base = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    1,
    CrossSphElemDt_base,
};

static void ct_common_setDynaPoly(BgHidanKowarerukabe* this, PlayState* play) {
    static CollisionHeader* BGDT_info[] = {
        &gFireTempleCrackedStoneFloorCol,
        &gFireTempleBombableWallCol,
        &gFireTempleLargeBombableWallCol,
    };
    s32 pad;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    if (BGDT_info[PARAMS_GET_U(this->dyna.actor.params, 0, 8)] != NULL) {
        MoveBG_ct(&this->dyna, 0);
        DynaPolyUty_bgdi_SG2KSG(BGDT_info[PARAMS_GET_U(this->dyna.actor.params, 0, 8)], &colHeader);
        this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    } else {
        this->dyna.bgId = BGACTOR_NEG_ONE;
    }
}

void ct_common_setCollision(BgHidanKowarerukabe* this, PlayState* play) {
    static s16 rdt[] = { 80, 45, 80 };
    static s16 offsetY[] = { 0, 500, 500 };
    s32 pad;

    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->dyna.actor, &CrossSphDt_base, this->colliderItems);

    this->collider.elements[0].dim.modelSphere.radius = rdt[PARAMS_GET_U(this->dyna.actor.params, 0, 8)];
    this->collider.elements[0].dim.modelSphere.center.y = offsetY[PARAMS_GET_U(this->dyna.actor.params, 0, 8)];
}

void ct_common_setPosOffsetY(BgHidanKowarerukabe* this) {
    static f32 offsetY[] = { 0.7f, 0.0f, 0.0f };

    this->dyna.actor.world.pos.y =
        offsetY[PARAMS_GET_U(this->dyna.actor.params, 0, 8)] + this->dyna.actor.home.pos.y;
}

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

void Bg_Hidan_Kowarerukabe_actor_ct(Actor* thisx, PlayState* play) {
    BgHidanKowarerukabe* this = (BgHidanKowarerukabe*)thisx;

    ct_common_setDynaPoly(this, play);

    if (PARAMS_GET_U(this->dyna.actor.params, 0, 8) < CRACKED_STONE_FLOOR ||
        PARAMS_GET_U(this->dyna.actor.params, 0, 8) > LARGE_BOMBABLE_WALL) {
        // "Error: Fire Temple Breakable Walls. arg_data I can't determine the (%s %d)(arg_data 0x%04x)"
        PRINTF("Error : 炎の神殿 壊れる壁 の arg_data が判別出来ない(%s %d)(arg_data 0x%04x)\n",
               "../z_bg_hidan_kowarerukabe.c", 254, this->dyna.actor.params);
        Actor_delete(&this->dyna.actor);
        return;
    }

    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6))) {
        Actor_delete(&this->dyna.actor);
        return;
    }

    ValueSet_process(&this->dyna.actor, value_init);
    Actor_set_scale(&this->dyna.actor, 0.1f);
    ct_common_setCollision(this, play);
    ct_common_setPosOffsetY(this);
    // "(fire walls, floors, destroyed by bombs)(arg_data 0x%04x)"
    PRINTF("(hidan 爆弾で壊れる 壁 床)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void Bg_Hidan_Kowarerukabe_actor_dt(Actor* thisx, PlayState* play) {
    BgHidanKowarerukabe* this = (BgHidanKowarerukabe*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    ClObjJntSph_dt_nzf(play, &this->collider);
}

void set_kemuri(BgHidanKowarerukabe* this, PlayState* play) {
    s32 pad;
    Vec3f pos;

    pos = this->dyna.actor.world.pos;
    pos.y += 10.0f;

    dust_fly_set2(play, &pos, 0.0f, 0, 600, 300, 1);

    pos.x = ((fqrand() - 0.5f) * 80.0f) + this->dyna.actor.world.pos.x;
    pos.y = (fqrand() * 100.0f) + this->dyna.actor.world.pos.y;
    pos.z = ((fqrand() - 0.5f) * 80.0f) + this->dyna.actor.world.pos.z;

    dust_fly_set2(play, &pos, 100.0f, 4, 200, 250, 1);
}

void set_kakera_bmfl(BgHidanKowarerukabe* this, PlayState* play) {
    s32 i;
    s32 j;
    Vec3f velocity;
    Vec3f pos;
    s16 arg5;
    Actor* thisx = &this->dyna.actor;
    f32 sin = sin_s(thisx->shape.rot.y);
    f32 cos = cos_s(thisx->shape.rot.y);
    f32 tmp1;
    f32 tmp2;
    s16 arg9;

    pos.y = thisx->world.pos.y + 10.0f;

    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++) {
            tmp1 = 24 * (i - 2);
            tmp2 = 24 * (j - 2);

            pos.x = (tmp2 * sin) + (tmp1 * cos) + thisx->world.pos.x;
            pos.z = (tmp2 * cos) - (tmp1 * sin) + thisx->world.pos.z;

            tmp1 = 8.0f * fqrand() * (i - 2);
            tmp2 = 8.0f * fqrand() * (j - 2);

            velocity.x = (tmp2 * sin) + (tmp1 * cos);
            velocity.y = 30.0f * fqrand();
            velocity.z = (tmp2 * cos) - (tmp1 * sin);

            arg9 = ((fqrand() - 0.5f) * 11.0f * 1.4f) + 11.0f;

            arg5 = (((i == 0) || (i == 4)) && ((j == 0) || (j == 4))) ? 65 : 64;

            Effect_Kakera_ct2(play, &pos, &velocity, &thisx->world.pos, -550, arg5, 15, 15, 0, arg9, 2, 16, 100,
                                 KAKERA_COLOR_NONE, OBJECT_GAMEPLAY_DANGEON_KEEP, gBrownFragmentDL);
        }
    }
}

void set_kakera_bmwall1(BgHidanKowarerukabe* this, PlayState* play) {
    s32 i;
    s32 j;
    Vec3f velocity;
    Vec3f pos;
    s16 arg5;
    Actor* thisx = &this->dyna.actor;
    f32 sin = sin_s(thisx->shape.rot.y);
    f32 cos = cos_s(thisx->shape.rot.y);
    f32 tmp1;
    f32 tmp2;
    s16 arg9;

    for (i = 0; i < 5; i++) {
        pos.y = (20 * i) + thisx->world.pos.y;
        for (j = 0; j < 5; j++) {
            tmp1 = 16 * (j - 2);

            pos.x = (tmp1 * cos) + thisx->world.pos.x;
            pos.z = -(tmp1 * sin) + thisx->world.pos.z;

            tmp1 = 3.0f * fqrand() * (j - 2);
            tmp2 = 6.0f * fqrand();

            velocity.x = (tmp2 * sin) + (tmp1 * cos);
            velocity.y = 18.0f * fqrand();
            velocity.z = (tmp2 * cos) - (tmp1 * sin);

            arg9 = ((fqrand() - 0.5f) * 11.0f * 1.4f) + 11.0f;
            arg5 = (arg9 >= 15) ? 32 : 64;

            if (fqrand() < 5.0f) {
                arg5 |= 1;
            }

            Effect_Kakera_ct2(play, &pos, &velocity, &thisx->world.pos, -540, arg5, 20, 20, 0, arg9, 2, 32, 100,
                                 KAKERA_COLOR_NONE, OBJECT_GAMEPLAY_DANGEON_KEEP, gBrownFragmentDL);
        }
    }
}

void set_kakera_bmwall2(BgHidanKowarerukabe* this, PlayState* play) {
    s32 i;
    s32 j;
    Vec3f velocity;
    Vec3f pos;
    s16 arg5;
    Actor* thisx = &this->dyna.actor;
    f32 sin = sin_s(thisx->shape.rot.y);
    f32 cos = cos_s(thisx->shape.rot.y);
    f32 tmp1;
    f32 tmp2;
    s16 arg9;

    for (i = 0; i < 5; i++) {
        pos.y = (24 * i) + thisx->world.pos.y;
        for (j = 0; j < 5; j++) {
            tmp1 = 28 * (j - 2);

            pos.x = (tmp1 * cos) + thisx->world.pos.x;
            pos.z = -(tmp1 * sin) + thisx->world.pos.z;

            tmp1 = 6.0f * fqrand() * (j - 2);
            tmp2 = 6.0f * fqrand();

            velocity.x = (tmp2 * sin) + (tmp1 * cos);
            velocity.y = 34.0f * fqrand();
            velocity.z = (tmp2 * cos) - (tmp1 * sin);

            arg9 = ((fqrand() - 0.5f) * 14.0f * 1.6f) + 14.0f;
            arg5 = (arg9 > 20) ? 32 : 64;

            if (fqrand() < 5.0f) {
                arg5 |= 1;
            }

            Effect_Kakera_ct2(play, &pos, &velocity, &thisx->world.pos, -650, arg5, 20, 20, 0, arg9, 2, 32, 100,
                                 KAKERA_COLOR_NONE, OBJECT_GAMEPLAY_DANGEON_KEEP, gBrownFragmentDL);
        }
    }
}

void set_effectKakera(BgHidanKowarerukabe* this, PlayState* play) {
    switch (PARAMS_GET_U(this->dyna.actor.params, 0, 8)) {
        case CRACKED_STONE_FLOOR:
            set_kakera_bmfl(this, play);
            break;
        case BOMBABLE_WALL:
            set_kakera_bmwall1(this, play);
            break;
        case LARGE_BOMBABLE_WALL:
            set_kakera_bmwall2(this, play);
            break;
    }

    set_kemuri(this, play);
}

void Bg_Hidan_Kowarerukabe_actor_move(Actor* thisx, PlayState* play) {
    BgHidanKowarerukabe* this = (BgHidanKowarerukabe*)thisx;
    s32 pad;

    if (BlastVsMyCheck(play, &this->collider.base) != NULL) {
        set_effectKakera(this, play);
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6));

        if (PARAMS_GET_U(this->dyna.actor.params, 0, 8) == 0) {
            Effect_SE_Info_new(play, &this->dyna.actor.world.pos, 40, NA_SE_EV_EXPLOSION);
        } else {
            Effect_SE_Info_new(play, &this->dyna.actor.world.pos, 40, NA_SE_EV_WALL_BROKEN);
        }

        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        Actor_delete(&this->dyna.actor);
        return;
    }

    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
}

void Bg_Hidan_Kowarerukabe_actor_draw(Actor* thisx, PlayState* play) {
    BgHidanKowarerukabe* this = (BgHidanKowarerukabe*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_hidan_kowarerukabe.c", 565);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_hidan_kowarerukabe.c", 568);
    gSPDisplayList(POLY_OPA_DISP++, shapeDtTbl[PARAMS_GET_U(this->dyna.actor.params, 0, 8)]);

    CollisionCheck_Uty_convJntSphL2G(0, &this->collider);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_hidan_kowarerukabe.c", 573);
}
