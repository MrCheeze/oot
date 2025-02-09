/*
 * File: z_bg_bombwall.c
 * Overlay: ovl_Bg_Bombwall
 * Description: Bombable Wall
 */

#include "z_bg_bombwall.h"
#include "libc64/qrand.h"
#include "ichain.h"
#include "sfx.h"
#include "z_lib.h"
#include "z64play.h"

#include "assets/objects/gameplay_field_keep/gameplay_field_keep.h"

#define FLAGS ACTOR_FLAG_IGNORE_POINT_LIGHTS

void Bg_Bombwall_actor_ct(Actor* thisx, PlayState* play);
void Bg_Bombwall_actor_dt(Actor* thisx, PlayState* play);
void Bg_Bombwall_actor_move(Actor* thisx, PlayState* play);
void Bg_Bombwall_actor_draw(Actor* thisx, PlayState* play);

void mvSet_before(BgBombwall* this, PlayState* play);
static void mv_before(BgBombwall* this, PlayState* play);
void mvSet_timer(BgBombwall* this, PlayState* play);
void mv_timer(BgBombwall* this, PlayState* play);
void mvSet_after(BgBombwall* this, PlayState* play);

static ColliderTrisElementInit ClObjTrisElemDt_base[3] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x40000048, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -70.0f, 176.0f, 0.0f }, { -70.0f, -4.0f, 0.0f }, { 0.0f, -4.0f, 30.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x40000048, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 70.0f, 176.0f, 0.0f }, { -70.0f, 176.0f, 0.0f }, { 0.0f, -4.0f, 30.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x40000048, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 70.0f, -4.0f, 0.0f }, { 70.0f, 176.0f, 0.0f }, { 0.0f, -4.0f, 30.0f } } },
    },
};

static ColliderTrisInit ClObjTrisDt_base = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    3,
    ClObjTrisElemDt_base,
};

ActorProfile Bg_Bombwall_Profile = {
    /**/ ACTOR_BG_BOMBWALL,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_FIELD_KEEP,
    /**/ sizeof(BgBombwall),
    /**/ Bg_Bombwall_actor_ct,
    /**/ Bg_Bombwall_actor_dt,
    /**/ Bg_Bombwall_actor_move,
    /**/ Bg_Bombwall_actor_draw,
};

void setDynaPoly(BgBombwall* this, PlayState* play) {
    s32 pad;
    s32 pad2;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gBgBombwallCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    if (this->dyna.bgId == BG_ACTOR_MAX) {
        // "Warning : move BG login failed"
        PRINTF("Warning : move BG 登録失敗(%s %d)(arg_data 0x%04x)\n", "../z_bg_bombwall.c", 243,
               this->dyna.actor.params);
    }
}

void rotY_bombwall(Vec3f* arg0, Vec3f* arg1, f32 arg2, f32 arg3) {
    arg0->x = (arg1->z * arg2) + (arg1->x * arg3);
    arg0->y = arg1->y;
    arg0->z = (arg1->z * arg3) - (arg1->x * arg2);
}

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 300, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

void Bg_Bombwall_actor_ct(Actor* thisx, PlayState* play) {
    s32 i;
    s32 j;
    Vec3f vecs[3];
    Vec3f sp80;
    s32 pad;
    BgBombwall* this = (BgBombwall*)thisx;
    f32 sin = sin_s(this->dyna.actor.shape.rot.y);
    f32 cos = cos_s(this->dyna.actor.shape.rot.y);

    ValueSet_process(&this->dyna.actor, value_init);
    Actor_set_scale(&this->dyna.actor, 0.1f);

    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
        mvSet_after(this, play);
    } else {
        setDynaPoly(this, play);
        this->unk_2A2 |= 2;
        ClObjTris_ct(play, &this->collider);
        ClObjTris_set5_nzm(play, &this->collider, &this->dyna.actor, &ClObjTrisDt_base, this->colliderItems);

        for (i = 0; i <= 2; i++) {
            for (j = 0; j <= 2; j++) {
                sp80.x = ClObjTrisDt_base.elements[i].dim.vtx[j].x;
                sp80.y = ClObjTrisDt_base.elements[i].dim.vtx[j].y;
                sp80.z = ClObjTrisDt_base.elements[i].dim.vtx[j].z + 2.0f;

                rotY_bombwall(&vecs[j], &sp80, sin, cos);

                vecs[j].x += this->dyna.actor.world.pos.x;
                vecs[j].y += this->dyna.actor.world.pos.y;
                vecs[j].z += this->dyna.actor.world.pos.z;
            }
            CollisionCheck_Uty_setTrisPos(&this->collider, i, &vecs[0], &vecs[1], &vecs[2]);
        }

        this->unk_2A2 |= 1;
        mvSet_before(this, play);
    }

    PRINTF("(field keep 汎用爆弾壁)(arg_data 0x%04x)(angY %d)\n", this->dyna.actor.params,
           this->dyna.actor.shape.rot.y);
}

void clear_bombwall(BgBombwall* this, PlayState* play) {
    if (this->unk_2A2 & 2) {
        DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
        this->unk_2A2 &= ~2;
    }

    if (this->unk_2A2 & 1) {
        ClObjTris_dt_nzf(play, &this->collider);
        this->unk_2A2 &= ~1;
    }
}

void Bg_Bombwall_actor_dt(Actor* thisx, PlayState* play) {
    BgBombwall* this = (BgBombwall*)thisx;

    clear_bombwall(this, play);
}

static Vec3s pos_data[] = {
    { 40, 85, 21 }, { -43, 107, 14 }, { -1, 142, 14 }, { -27, 44, 27 }, { 28, 24, 20 }, { -39, 54, 21 }, { 49, 50, 20 },
};

void eff_bombwall(BgBombwall* this, PlayState* play) {
    s16 rand;
    s16 rand2;
    Vec3f sp88;
    s32 i;
    f32 sin = sin_s(this->dyna.actor.shape.rot.y);
    f32 cos = cos_s(this->dyna.actor.shape.rot.y);
    Vec3f* pos = &this->dyna.actor.world.pos;
    f32 temp;
    f32 new_var;

    for (i = 0; i < 7; i++) {
        new_var = pos_data[i].x;
        temp = new_var * cos;
        sp88.x = ((sin * pos_data[i].z) + ((f32)temp)) + pos->x;
        sp88.y = pos->y + pos_data[i].y;
        sp88.z = ((pos_data[i].z * cos) - (sin * pos_data[i].x)) + pos->z;
        rand = ((s16)(fqrand() * 120.0f)) + 0x14;
        rand2 = ((s16)(fqrand() * 240.0f)) + 0x14;
        dust_fly_set2(play, &sp88, 50.0f, 2, rand, rand2, 1);
    }

    sp88.x = pos->x;
    new_var = pos->y + 90.0f;
    sp88.y = pos->y + 90.0f;
    sp88.z = pos->z + 15.0f;
    dust_fly_set2(play, &sp88, 40.0f, 4, 0xA, 0x32, 1);
}

void mvSet_before(BgBombwall* this, PlayState* play) {
    this->dList = gBgBombwallNormalDL;
    this->actionFunc = mv_before;
}

static void mv_before(BgBombwall* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        mvSet_timer(this, play);
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6));
    } else if (this->dyna.actor.xzDistToPlayer < 600.0f) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void mvSet_timer(BgBombwall* this, PlayState* play) {
    this->dList = gBgBombwallNormalDL;
    this->unk_2A0 = 1;
    eff_bombwall(this, play);
    this->actionFunc = mv_timer;
}

void mv_timer(BgBombwall* this, PlayState* play) {
    if (this->unk_2A0 > 0) {
        this->unk_2A0--;
    } else {
        mvSet_after(this, play);

        if (PARAMS_GET_U(this->dyna.actor.params, 15, 1) != 0) {
            Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        }
    }
}

void mvSet_after(BgBombwall* this, PlayState* play) {
    this->dList = gBgBombwallBrokenDL;
    clear_bombwall(this, play);
    this->actionFunc = NULL;
}

void Bg_Bombwall_actor_move(Actor* thisx, PlayState* play) {
    BgBombwall* this = (BgBombwall*)thisx;

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void Bg_Bombwall_actor_draw(Actor* thisx, PlayState* play) {
    BgBombwall* this = (BgBombwall*)thisx;

    Cheap_gfx_display(play, this->dList);
}
