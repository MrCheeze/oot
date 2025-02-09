/*
 * File: z_bg_spot08_bakudankabe
 * Overlay: ovl_Bg_Spot08_Bakudankabe
 * Description: Destructible Wall (Zora's Fountain)
 */

#include "z_bg_spot08_bakudankabe.h"
#include "assets/objects/object_spot08_obj/object_spot08_obj.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "assets/objects/gameplay_field_keep/gameplay_field_keep.h"

#define FLAGS ACTOR_FLAG_IGNORE_POINT_LIGHTS

void Bg_Spot08_Bakudankabe_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot08_Bakudankabe_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot08_Bakudankabe_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot08_Bakudankabe_actor_draw(Actor* thisx, PlayState* play);

static void set_collision_bakudankabe(BgSpot08Bakudankabe* this, PlayState* play);
static void eff_bakudankabe(BgSpot08Bakudankabe* this, PlayState* play);

ActorProfile Bg_Spot08_Bakudankabe_Profile = {
    /**/ ACTOR_BG_SPOT08_BAKUDANKABE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT08_OBJ,
    /**/ sizeof(BgSpot08Bakudankabe),
    /**/ Bg_Spot08_Bakudankabe_actor_ct,
    /**/ Bg_Spot08_Bakudankabe_actor_dt,
    /**/ Bg_Spot08_Bakudankabe_actor_move,
    /**/ Bg_Spot08_Bakudankabe_actor_draw,
};

static ColliderJntSphElementInit ClSphElemDt_bakudankabe[] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 0, { { 0, 50, 50 }, 70 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 0, { { -100, 50, 50 }, 70 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 0, { { 100, 50, 50 }, 70 }, 100 },
    },
};

static ColliderJntSphInit ClSphDt_bakudankabe = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    3,
    ClSphElemDt_bakudankabe,
};

static Vec3f kemu_pos[] = {
    { 0.0f, 116.65f, 50.0f },
    { 115.0f, 95.0f, 10.0f },
    { -115.0f, 95.0f, 10.0f },
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F(scale, 1, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 3200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

static void set_collision_bakudankabe(BgSpot08Bakudankabe* this, PlayState* play) {
    s32 pad;

    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->dyna.actor, &ClSphDt_bakudankabe, this->colliderItems);
}

static void eff_bakudankabe(BgSpot08Bakudankabe* this, PlayState* play) {
    s32 pad[2];
    s32 i;
    Vec3f burstDepthY;
    Vec3f burstDepthX;
    f32 sinY;
    f32 cosY;

    sinY = sin_s(this->dyna.actor.shape.rot.y);
    cosY = cos_s(this->dyna.actor.shape.rot.y);

    burstDepthX.z = 0.0f;
    burstDepthX.x = 0.0f;

    for (i = 0; i < 24; i++) {
        s16 gravityInfluence;
        s16 scale;
        f32 temp1;
        f32 temp2;
        s32 rotationSpeed;

        temp1 = (fqrand() - 0.5f) * 440.0f;
        temp2 = (fqrand() - 0.5f) * 20.0f;
        burstDepthY.x = this->dyna.actor.world.pos.x + temp2 * sinY + (temp1 * cosY);
        burstDepthY.y = (this->dyna.actor.world.pos.y + 20.0f) + (i * (65.0f / 12.0f));
        burstDepthY.z = this->dyna.actor.world.pos.z + temp2 * cosY - (temp1 * sinY);

        burstDepthX.y = (fqrand() - 0.2f) * 12.0f;
        scale = fqrand() * 75.0f + 10.0f;

        if (scale < 25) {
            gravityInfluence = -300;
        } else if (scale < 50) {
            gravityInfluence = -360;
        } else {
            gravityInfluence = -420;
        }

        if (fqrand() < 0.4f) {
            rotationSpeed = 65;
        } else {
            rotationSpeed = 33;
        }

        Effect_Kakera_ct2(play, &burstDepthY, &burstDepthX, &burstDepthY, gravityInfluence, rotationSpeed, 0x1E, 4,
                             0, scale, 1, 3, 80, KAKERA_COLOR_NONE, OBJECT_GAMEPLAY_FIELD_KEEP, gFieldKakeraDL);
    }

    for (i = 0; i < ARRAY_COUNT(kemu_pos); i++) {
        burstDepthY.x = this->dyna.actor.world.pos.x + kemu_pos[i].z * sinY + kemu_pos[i].x * cosY;
        burstDepthY.y = this->dyna.actor.world.pos.y + kemu_pos[i].y;
        burstDepthY.z = this->dyna.actor.world.pos.z + kemu_pos[i].z * cosY - (kemu_pos[i].x * sinY);
        dust_fly_set2(play, &burstDepthY, 120.0f, 4, 0x78, 0xA0, 1);
    }
}

void Bg_Spot08_Bakudankabe_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot08Bakudankabe* this = (BgSpot08Bakudankabe*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, 0);
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
        Actor_delete(&this->dyna.actor);
        return;
    }
    set_collision_bakudankabe(this, play);
    DynaPolyUty_bgdi_SG2KSG(&gZorasFountainBombableWallCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    ValueSet_process(&this->dyna.actor, value_init);
}

void Bg_Spot08_Bakudankabe_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot08Bakudankabe* this = (BgSpot08Bakudankabe*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    ClObjJntSph_dt_nzf(play, &this->collider);
}

void Bg_Spot08_Bakudankabe_actor_move(Actor* thisx, PlayState* play) {
    BgSpot08Bakudankabe* this = (BgSpot08Bakudankabe*)thisx;

    if (this->collider.base.acFlags & AC_HIT) {
        eff_bakudankabe(this, play);
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6));
        Effect_SE_Info_new(play, &this->dyna.actor.world.pos, 40, NA_SE_EV_WALL_BROKEN);
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        Actor_delete(&this->dyna.actor);
    } else if (this->dyna.actor.xzDistToPlayer < 800.0f) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void Bg_Spot08_Bakudankabe_actor_draw(Actor* thisx, PlayState* play) {
    BgSpot08Bakudankabe* this = (BgSpot08Bakudankabe*)thisx;

    CollisionCheck_Uty_convJntSphL2G(0, &this->collider);
    CollisionCheck_Uty_convJntSphL2G(1, &this->collider);
    CollisionCheck_Uty_convJntSphL2G(2, &this->collider);
    Cheap_gfx_display(play, gZorasFountainBombableWallDL);
}
