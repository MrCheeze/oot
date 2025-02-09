/*
 * File: z_bg_spot11_bakudankabe.c
 * Overlay: ovl_Bg_Spot11_Bakudankabe
 * Description: Destructible Wall (Desert Colossus)
 */

#include "z_bg_spot11_bakudankabe.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "assets/objects/object_spot11_obj/object_spot11_obj.h"
#include "assets/objects/gameplay_field_keep/gameplay_field_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Spot11_Bakudankabe_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot11_Bakudankabe_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot11_Bakudankabe_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot11_Bakudankabe_actor_draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Spot11_Bakudankabe_Profile = {
    /**/ ACTOR_BG_SPOT11_BAKUDANKABE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT11_OBJ,
    /**/ sizeof(BgSpot11Bakudankabe),
    /**/ Bg_Spot11_Bakudankabe_actor_ct,
    /**/ Bg_Spot11_Bakudankabe_actor_dt,
    /**/ Bg_Spot11_Bakudankabe_actor_move,
    /**/ Bg_Spot11_Bakudankabe_actor_draw,
};

static ColliderCylinderInit ClPipeDt_bakudankabe = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 40, 80, 0, { 2259, 108, -1580 } },
};

static Vec3f offset_pos = { 2259.0f, 108.0f, -1550.0f };
static Vec3f sound_pos = { 2259.0f, 108.0f, -1550.0f };

static void set_collision_bakudankabe(BgSpot11Bakudankabe* this, PlayState* play) {
    s32 pad;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->dyna.actor, &ClPipeDt_bakudankabe);
    this->collider.dim.pos.x += (s16)this->dyna.actor.world.pos.x;
    this->collider.dim.pos.y += (s16)this->dyna.actor.world.pos.y;
    this->collider.dim.pos.z += (s16)this->dyna.actor.world.pos.z;
}

static void eff_bakudankabe(BgSpot11Bakudankabe* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;
    Vec3f burstDepthY;
    Vec3f burstDepthX;
    s32 i;

    burstDepthX.z = 0;
    burstDepthX.x = 0;

    for (i = 0; i < 20; i++) {
        s16 scale;
        s32 gravityInfluence;
        s32 rotationSpeed;

        xyz_t_add(&thisx->world.pos, &offset_pos, &burstDepthY);

        burstDepthY.x += (fqrand() - 0.5f) * 120.0f;
        burstDepthY.y += (30.0f + (i * 6.5f));
        burstDepthY.z += (fqrand() - 0.5f) * 20.0f;

        burstDepthX.y = (fqrand() - 0.2f) * 12.0f;
        scale = (fqrand() * 55.0f) + 8.0f;

        if (scale < 20) {
            gravityInfluence = -300;
        } else if (scale < 35) {
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
    xyz_t_add(&thisx->world.pos, &offset_pos, &burstDepthY);
    dust_fly_set2(play, &burstDepthY, 70, 4, 110, 160, 1);
    burstDepthY.y += 40;
    dust_fly_set2(play, &burstDepthY, 70, 5, 110, 160, 1);
    burstDepthY.y += 40;
    dust_fly_set2(play, &burstDepthY, 70, 4, 110, 160, 1);
}

void Bg_Spot11_Bakudankabe_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot11Bakudankabe* this = (BgSpot11Bakudankabe*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, 0);
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
        Actor_delete(&this->dyna.actor);
        return;
    }
    set_collision_bakudankabe(this, play);
    DynaPolyUty_bgdi_SG2KSG(&gDesertColossusBombableWallCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    Actor_set_scale(&this->dyna.actor, 1.0f);
    PRINTF("(spot11 爆弾壁)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void Bg_Spot11_Bakudankabe_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot11Bakudankabe* this = (BgSpot11Bakudankabe*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    ClObjPipe_dt(play, &this->collider);
}

void Bg_Spot11_Bakudankabe_actor_move(Actor* thisx, PlayState* play) {
    BgSpot11Bakudankabe* this = (BgSpot11Bakudankabe*)thisx;

    if (this->collider.base.acFlags & AC_HIT) {
        eff_bakudankabe(this, play);
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6));
        Effect_SE_Info_new(play, &sound_pos, 40, NA_SE_EV_WALL_BROKEN);
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        Actor_delete(&this->dyna.actor);
        return;
    }
    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
}

void Bg_Spot11_Bakudankabe_actor_draw(Actor* thisx, PlayState* play) {
    BgSpot11Bakudankabe* this = (BgSpot11Bakudankabe*)thisx;

    Cheap_gfx_display(play, gDesertColossusBombableWallDL);
}
