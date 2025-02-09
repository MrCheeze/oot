/*
 * File: z_bg_ydan_maruta.c
 * Overlay: ovl_Bg_Ydan_Maruta
 * Description: Rotating spike log and falling ladder in Deku Tree
 */

#include "z_bg_ydan_maruta.h"
#include "assets/objects/object_ydan_objects/object_ydan_objects.h"

#define FLAGS 0

void Bg_Ydan_Maruta_actor_ct(Actor* thisx, PlayState* play);
void Bg_Ydan_Maruta_actor_dt(Actor* thisx, PlayState* play);
void Bg_Ydan_Maruta_actor_move(Actor* thisx, PlayState* play);
void Bg_Ydan_Maruta_actor_draw(Actor* thisx, PlayState* play);

static void mode_rotate(BgYdanMaruta* this, PlayState* play);
void mode_hasigo_stop(BgYdanMaruta* this, PlayState* play);
void mode_hasigo_wait(BgYdanMaruta* this, PlayState* play);
void mode_hasigo_yure(BgYdanMaruta* this, PlayState* play);
void mode_hasigo_drop(BgYdanMaruta* this, PlayState* play);

ActorProfile Bg_Ydan_Maruta_Profile = {
    /**/ ACTOR_BG_YDAN_MARUTA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_YDAN_OBJECTS,
    /**/ sizeof(BgYdanMaruta),
    /**/ Bg_Ydan_Maruta_actor_ct,
    /**/ Bg_Ydan_Maruta_actor_dt,
    /**/ Bg_Ydan_Maruta_actor_move,
    /**/ Bg_Ydan_Maruta_actor_draw,
};

static ColliderTrisElementInit YdanMarutaAtAcTrisElemData[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000004, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_WOOD,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 220.0f, -10.0f, 0.0f }, { 220.0f, 10.0f, 0.0f }, { -220.0f, 10.0f, 0.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000004, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_WOOD,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 16.0f, 0.0f, 0.0f }, { 16.0f, 135.0f, 0.0f }, { -16.0f, 135.0f, 0.0f } } },
    },
};

static ColliderTrisInit YdanMarutaAtAcTrisData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    2,
    YdanMarutaAtAcTrisElemData,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Ydan_Maruta_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgYdanMaruta* this = (BgYdanMaruta*)thisx;
    Vec3f sp4C[3];
    s32 i;
    f32 sinRotY;
    f32 cosRotY;
    CollisionHeader* colHeader = NULL;
    ColliderTrisElementInit* triInit;

    ValueSet_process(&this->dyna.actor, value_init);
    ClObjTris_ct(play, &this->collider);
    ClObjTris_set5_nzm(play, &this->collider, &this->dyna.actor, &YdanMarutaAtAcTrisData, this->elements);

    this->switchFlag = PARAMS_GET_U(this->dyna.actor.params, 0, 16);
    thisx->params = PARAMS_GET_U(thisx->params, 8, 8); // thisx is required to match here

    if (this->dyna.actor.params == 0) {
        triInit = &YdanMarutaAtAcTrisElemData[0];
        this->actionFunc = mode_rotate;
    } else {
        triInit = &YdanMarutaAtAcTrisElemData[1];
        MoveBG_ct(&this->dyna, 0);
        DynaPolyUty_bgdi_SG2KSG(&gDTFallingLadderCol, &colHeader);
        this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
        thisx->home.pos.y += -280.0f;
        if (Actor_Environment_sw_Check(play, this->switchFlag)) {
            thisx->world.pos.y = thisx->home.pos.y;
            this->actionFunc = mode_hasigo_stop;
        } else {
            this->actionFunc = mode_hasigo_wait;
        }
    }

    sinRotY = sin_s(this->dyna.actor.shape.rot.y);
    cosRotY = cos_s(this->dyna.actor.shape.rot.y);

    for (i = 0; i < 3; i++) {
        sp4C[i].x = (triInit->dim.vtx[i].x * cosRotY) + this->dyna.actor.world.pos.x;
        sp4C[i].y = triInit->dim.vtx[i].y + this->dyna.actor.world.pos.y;
        sp4C[i].z = this->dyna.actor.world.pos.z - (triInit->dim.vtx[i].x * sinRotY);
    }

    CollisionCheck_Uty_setTrisPos(&this->collider, 0, &sp4C[0], &sp4C[1], &sp4C[2]);

    sp4C[1].x = (triInit->dim.vtx[2].x * cosRotY) + this->dyna.actor.world.pos.x;
    sp4C[1].y = triInit->dim.vtx[0].y + this->dyna.actor.world.pos.y;
    sp4C[1].z = this->dyna.actor.world.pos.z - (triInit->dim.vtx[2].x * sinRotY);

    CollisionCheck_Uty_setTrisPos(&this->collider, 1, &sp4C[0], &sp4C[2], &sp4C[1]);
}

void Bg_Ydan_Maruta_actor_dt(Actor* thisx, PlayState* play) {
    BgYdanMaruta* this = (BgYdanMaruta*)thisx;

    ClObjTris_dt_nzf(play, &this->collider);
    if (this->dyna.actor.params == 1) {
        DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

static void mode_rotate(BgYdanMaruta* this, PlayState* play) {
    if (this->collider.base.atFlags & AT_HIT) {
        Actor_player_power_damage_set(play, &this->dyna.actor, 7.0f, this->dyna.actor.shape.rot.y, 6.0f);
    }
    this->dyna.actor.shape.rot.x += 0x360;
    CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_TOGE_STICK_ROLLING - SFX_FLAG);
}

void mode_hasigo_wait(BgYdanMaruta* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->unk_16A = 20;
        Actor_Environment_sw_On(play, this->switchFlag);
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        this->actionFunc = mode_hasigo_yure;
        makeOnepointDemo(play, 3010, 50, &this->dyna.actor, CAM_ID_MAIN);
    } else {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void mode_hasigo_yure(BgYdanMaruta* this, PlayState* play) {
    s16 temp;

    if (this->unk_16A != 0) {
        this->unk_16A--;
    }
    if (this->unk_16A == 0) {
        this->actionFunc = mode_hasigo_drop;
    }

    if (1) {}

    temp = (this->unk_16A % 4) - 2;
    if (temp == -2) {
        temp = 0;
    } else {
        temp *= 2;
    }

    this->dyna.actor.world.pos.x = (cos_s(this->dyna.actor.shape.rot.y) * temp) + this->dyna.actor.home.pos.x;
    this->dyna.actor.world.pos.z = (sin_s(this->dyna.actor.shape.rot.y) * temp) + this->dyna.actor.home.pos.z;

    Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_TRAP_OBJ_SLIDE - SFX_FLAG);
}

void mode_hasigo_drop(BgYdanMaruta* this, PlayState* play) {
    this->dyna.actor.velocity.y += 1.0f;
    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, this->dyna.actor.velocity.y)) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_LADDER_DOUND);
        this->actionFunc = mode_hasigo_stop;
    }
}

void mode_hasigo_stop(BgYdanMaruta* this, PlayState* play) {
}

void Bg_Ydan_Maruta_actor_move(Actor* thisx, PlayState* play) {
    BgYdanMaruta* this = (BgYdanMaruta*)thisx;

    this->actionFunc(this, play);
}

void Bg_Ydan_Maruta_actor_draw(Actor* thisx, PlayState* play) {
    BgYdanMaruta* this = (BgYdanMaruta*)thisx;

    if (this->dyna.actor.params == 0) {
        Cheap_gfx_display(play, gDTRollingSpikeTrapDL);
    } else {
        Cheap_gfx_display(play, gDTFallingLadderDL);
    }
}
