/*
 * File: z_bg_hidan_dalm.c
 * Overlay: ovl_Bg_Hidan_Dalm
 * Description: Hammerable Totem Pieces (Fire Temple)
 */

#include "z_bg_hidan_dalm.h"

#include "libc64/qrand.h"
#include "ichain.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS 0

void Bg_Hidan_Dalm_actor_ct(Actor* thisx, PlayState* play);
void Bg_Hidan_Dalm_actor_dt(Actor* thisx, PlayState* play);
void Bg_Hidan_Dalm_actor_move(Actor* thisx, PlayState* play);
void Bg_Hidan_Dalm_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgHidanDalm* this, PlayState* play);
static void mode_damage(BgHidanDalm* this, PlayState* play);

ActorProfile Bg_Hidan_Dalm_Profile = {
    /**/ ACTOR_BG_HIDAN_DALM,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HIDAN_OBJECTS,
    /**/ sizeof(BgHidanDalm),
    /**/ Bg_Hidan_Dalm_actor_ct,
    /**/ Bg_Hidan_Dalm_actor_dt,
    /**/ Bg_Hidan_Dalm_actor_move,
    /**/ Bg_Hidan_Dalm_actor_draw,
};

static ColliderTrisElementInit HidanDalmAcTrisElemData[4] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000040, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON | ACELEM_NO_AT_INFO | ACELEM_NO_DAMAGE | ACELEM_NO_SWORD_SFX | ACELEM_NO_HITMARK,
            OCELEM_NONE,
        },
        { { { 305.0f, 0.0f, -300.0f }, { 305.0f, 600.0f, -300.0f }, { 305.0f, 600.0f, 300.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000040, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON | ACELEM_NO_AT_INFO | ACELEM_NO_DAMAGE | ACELEM_NO_SWORD_SFX | ACELEM_NO_HITMARK,
            OCELEM_NONE,
        },
        { { { 305.0f, 0.0f, -300.0f }, { 305.0f, 600.0f, 300.0f }, { 305.0f, 0.0f, 300.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000040, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON | ACELEM_NO_AT_INFO | ACELEM_NO_DAMAGE | ACELEM_NO_SWORD_SFX | ACELEM_NO_HITMARK,
            OCELEM_NONE,
        },
        { { { -305.0f, 0.0f, -300.0f }, { -305.0f, 600.0f, 300.0f }, { -305.0f, 600.0f, -300.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000040, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON | ACELEM_NO_AT_INFO | ACELEM_NO_DAMAGE | ACELEM_NO_SWORD_SFX | ACELEM_NO_HITMARK,
            OCELEM_NONE,
        },
        { { { -305.0f, 0.0f, -300.0f }, { -305.0f, 0.0f, 300.0f }, { -305.0f, 600.0f, 300.0f } } },
    },
};

static ColliderTrisInit HidanDalmAcTrisData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    4,
    HidanDalmAcTrisElemData,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -200, ICHAIN_STOP),
};

void Bg_Hidan_Dalm_actor_ct(Actor* thisx, PlayState* play) {
    BgHidanDalm* this = (BgHidanDalm*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(thisx, value_init);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gFireTempleHammerableTotemCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
    ClObjTris_ct(play, &this->collider);
    ClObjTris_set5_nzm(play, &this->collider, thisx, &HidanDalmAcTrisData, this->colliderItems);

    this->switchFlag = PARAMS_GET_U(thisx->params, 8, 8);
    thisx->params &= 0xFF;
    if (Actor_Environment_sw_Check(play, this->switchFlag)) {
        Actor_delete(thisx);
    } else {
        this->actionFunc = mode_wait;
    }
}

void Bg_Hidan_Dalm_actor_dt(Actor* thisx, PlayState* play) {
    BgHidanDalm* this = (BgHidanDalm*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    ClObjTris_dt_nzf(play, &this->collider);
}

static void mode_wait(BgHidanDalm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((this->collider.base.acFlags & AC_HIT) && !player_demo_check(play) &&
        (player->meleeWeaponAnimation == PLAYER_MWA_HAMMER_FORWARD ||
         player->meleeWeaponAnimation == PLAYER_MWA_HAMMER_SIDE)) {
        this->collider.base.acFlags &= ~AC_HIT;
        if ((this->collider.elements[0].base.acElemFlags & ACELEM_HIT) ||
            (this->collider.elements[1].base.acElemFlags & ACELEM_HIT)) {
            this->dyna.actor.world.rot.y -= 0x4000;
        } else {
            this->dyna.actor.world.rot.y += 0x4000;
        }
        this->dyna.actor.world.pos.x += 32.5f * sin_s(this->dyna.actor.world.rot.y);
        this->dyna.actor.world.pos.z += 32.5f * cos_s(this->dyna.actor.world.rot.y);

        player_demo_mode_set(play, &this->dyna.actor, PLAYER_CSACTION_8);
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actionFunc = mode_damage;
        this->dyna.actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND_TOUCH;
        this->dyna.actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
        this->dyna.actor.speed = 10.0f;
        Actor_Environment_sw_On(play, this->switchFlag);
        player_SE_set(GET_PLAYER(play), NA_SE_IT_HAMMER_HIT);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_DARUMA_VANISH);
    } else {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
}

static void mode_damage(BgHidanDalm* this, PlayState* play) {
    static Vec3f acc = { 0, 0, 0 };
    s32 i;
    Vec3f velocity;
    Vec3f pos;

    if (chase_f(&this->dyna.actor.scale.x, 0.0f, 0.004f)) {
        player_demo_mode_set(play, &this->dyna.actor, PLAYER_CSACTION_7);
        Actor_delete(&this->dyna.actor);
    }

    this->dyna.actor.scale.y = this->dyna.actor.scale.z = this->dyna.actor.scale.x;

    pos.x = this->dyna.actor.world.pos.x;
    pos.y = this->dyna.actor.world.pos.y + this->dyna.actor.scale.x * 160.0f;
    pos.z = this->dyna.actor.world.pos.z;

    for (i = 0; i < 4; i++) {
        velocity.x = 5.0f * sin_s(this->dyna.actor.world.rot.y + 0x8000) + (fqrand() - 0.5f) * 5.0f;
        velocity.z = 5.0f * cos_s(this->dyna.actor.world.rot.y + 0x8000) + (fqrand() - 0.5f) * 5.0f;
        velocity.y = (fqrand() - 0.5f) * 1.5f;
        Effect_SS_KiraKira_ct_direct(play, &pos, &velocity, &acc);
    }
}

void Bg_Hidan_Dalm_actor_move(Actor* thisx, PlayState* play) {
    BgHidanDalm* this = (BgHidanDalm*)thisx;

    this->actionFunc(this, play);
    Actor_position_moveF(&this->dyna.actor);
    Actor_BGcheck2(play, &this->dyna.actor, 10.0f, 15.0f, 32.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

/**
 * Update vertices of collider tris based on the current matrix
 */
void set_ac_tris(BgHidanDalm* this) {
    Vec3f pos2;
    Vec3f pos1;
    Vec3f pos0;

    Matrix_Position(&HidanDalmAcTrisElemData[0].dim.vtx[0], &pos0);
    Matrix_Position(&HidanDalmAcTrisElemData[0].dim.vtx[1], &pos1);
    Matrix_Position(&HidanDalmAcTrisElemData[0].dim.vtx[2], &pos2);
    CollisionCheck_Uty_setTrisPos(&this->collider, 0, &pos0, &pos1, &pos2);
    Matrix_Position(&HidanDalmAcTrisElemData[1].dim.vtx[2], &pos1);
    CollisionCheck_Uty_setTrisPos(&this->collider, 1, &pos0, &pos2, &pos1);

    Matrix_Position(&HidanDalmAcTrisElemData[2].dim.vtx[0], &pos0);
    Matrix_Position(&HidanDalmAcTrisElemData[2].dim.vtx[1], &pos1);
    Matrix_Position(&HidanDalmAcTrisElemData[2].dim.vtx[2], &pos2);
    CollisionCheck_Uty_setTrisPos(&this->collider, 2, &pos0, &pos1, &pos2);
    Matrix_Position(&HidanDalmAcTrisElemData[3].dim.vtx[1], &pos2);
    CollisionCheck_Uty_setTrisPos(&this->collider, 3, &pos0, &pos2, &pos1);
}

void Bg_Hidan_Dalm_actor_draw(Actor* thisx, PlayState* play) {
    BgHidanDalm* this = (BgHidanDalm*)thisx;

    if (this->dyna.actor.params == 0) {
        Cheap_gfx_display(play, gFireTempleHammerableTotemBodyDL);
    } else {
        Cheap_gfx_display(play, gFireTempleHammerableTotemHeadDL);
    }

    if (this->actionFunc == mode_wait) {
        set_ac_tris(this);
    }
}
