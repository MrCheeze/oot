/*
 * File: z_bg_spot17_bakudankabe.c
 * Overlay: ovl_Bg_Spot17_Bakudankabe
 * Description: Death Mountain Crater Bombable Wall
 */

#include "z_bg_spot17_bakudankabe.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "assets/objects/object_spot17_obj/object_spot17_obj.h"
#include "assets/objects/gameplay_field_keep/gameplay_field_keep.h"

#define FLAGS 0

void Bg_Spot17_Bakudankabe_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot17_Bakudankabe_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot17_Bakudankabe_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot17_Bakudankabe_actor_draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Spot17_Bakudankabe_Profile = {
    /**/ ACTOR_BG_SPOT17_BAKUDANKABE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT17_OBJ,
    /**/ sizeof(BgSpot17Bakudankabe),
    /**/ Bg_Spot17_Bakudankabe_actor_ct,
    /**/ Bg_Spot17_Bakudankabe_actor_dt,
    /**/ Bg_Spot17_Bakudankabe_actor_move,
    /**/ Bg_Spot17_Bakudankabe_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 3000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

static void eff_bakudankabe(BgSpot17Bakudankabe* this, PlayState* play) {
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

    for (i = 0; i < 20; i++) {
        s16 gravityInfluence;
        s16 scale;
        f32 temp1;
        f32 temp2;
        s32 rotationSpeed;

        temp1 = (fqrand() - 0.5f) * 140.0f;
        temp2 = (fqrand() - 0.5f) * 20.0f;

        burstDepthY.x = this->dyna.actor.world.pos.x + temp2 * sinY + (temp1 * cosY);
        burstDepthY.y = this->dyna.actor.world.pos.y + 30.0f + (i * 6.5f);
        burstDepthY.z = this->dyna.actor.world.pos.z + temp2 * cosY - (temp1 * sinY);

        burstDepthX.y = (fqrand() - 0.2f) * 12.0f;
        scale = fqrand() * 55.0f + 8.0f;

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
    xyz_t_move(&burstDepthY, &this->dyna.actor.world.pos);
    dust_fly_set2(play, &burstDepthY, 60.0f, 4, 110, 160, 1);
    burstDepthY.y += 40.0f;
    dust_fly_set2(play, &burstDepthY, 60.0f, 4, 120, 160, 1);
    burstDepthY.y += 40.0f;
    dust_fly_set2(play, &burstDepthY, 60.0f, 4, 110, 160, 1);
}

void Bg_Spot17_Bakudankabe_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot17Bakudankabe* this = (BgSpot17Bakudankabe*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, 0);
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
        Actor_delete(&this->dyna.actor);
        return;
    }

    DynaPolyUty_bgdi_SG2KSG(&gCraterBombableWallCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    ValueSet_process(&this->dyna.actor, value_init);
}

void Bg_Spot17_Bakudankabe_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot17Bakudankabe* this = (BgSpot17Bakudankabe*)thisx;
    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void Bg_Spot17_Bakudankabe_actor_move(Actor* thisx, PlayState* play) {
    BgSpot17Bakudankabe* this = (BgSpot17Bakudankabe*)thisx;
    if (this->dyna.actor.xzDistToPlayer < 650.0f && BlastVsMyCheck_c(play, &this->dyna.actor) != NULL) {
        eff_bakudankabe(this, play);
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6));
        Effect_SE_Info_new(play, &this->dyna.actor.world.pos, 40, NA_SE_EV_WALL_BROKEN);
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        Actor_delete(&this->dyna.actor);
    }
}

void Bg_Spot17_Bakudankabe_actor_draw(Actor* thisx, PlayState* play) {
    PlayState* play2 = (PlayState*)play;
    s8 r = coss(play2->gameplayFrames * 1500) >> 8;
    s8 g = coss(play2->gameplayFrames * 1500) >> 8;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot17_bakudankabe.c", 269);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_spot17_bakudankabe.c", 273);

    r = (r >> 1) + 0xC0;
    g = (g >> 1) + 0xC0;

    gDPSetEnvColor(POLY_OPA_DISP++, r, g, 255, 128);

    gSPDisplayList(POLY_OPA_DISP++, gCraterBombableWallDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot17_bakudankabe.c", 283);

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot17_bakudankabe.c", 286);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_spot17_bakudankabe.c", 290);
    gSPDisplayList(POLY_XLU_DISP++, gCraterBombableWallCracksDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot17_bakudankabe.c", 295);
}
